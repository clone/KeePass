/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2009 Dominik Reichl <dominik.reichl@t-online.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "StdAfx.h"
#include <tchar.h>
#include "KpApiImpl.h"
#include "PluginMgr.h"
#include "../../KeePassLibCpp/Util/TranslateEx.h"
#include "../../KeePassLibCpp/Util/StrUtil.h"
#include "../../KeePassLibCpp/Util/MemUtil.h"
#include "../Util/CmdLine/CommandLineOption.h"
#include "../Util/CmdLine/Executable.h"
#include "../Util/VersionInfo.h"

static LPCTSTR g_strEmpty = _T("");

CPluginManager& CPluginManager::Instance()
{
	static CPluginManager singleton;
	return singleton;
}

CPluginManager::CPluginManager()
{
	m_dwFreePluginID = 0;
	m_plugins.clear();
	// m_vKnownNames.clear();

	// ZeroMemory(&m_kpAppInfo, sizeof(KP_APP_INFO));

	m_dwFirstCommand = 0;
	m_dwLastCommand = 0;
}

CPluginManager::~CPluginManager()
{
	this->CleanUp();
}

void CPluginManager::CleanUp()
{
	this->UnloadAllPlugins();

	m_plugins.clear();
	// m_vKnownNames.clear();

	m_dwFreePluginID = 0;
	// ZeroMemory(&m_kpAppInfo, sizeof(KP_APP_INFO));
}

void CPluginManager::ClearStructure(KP_PLUGIN_INSTANCE* p)
{
	if(p == NULL) { ASSERT(FALSE); return; }

	p->dwPluginID = 0;
	p->hinstDLL = NULL;
	p->pInterface = NULL;

	p->strPath = _T("");

	p->strName = _T("");
	p->strVersion = _T("");
	p->strAuthor = _T("");
}

// BOOL CPluginManager::SetAppInfo(const KP_APP_INFO *pAppInfo)
// {
//	ASSERT(pAppInfo != NULL); if(pAppInfo == NULL) return FALSE;
//
//	memcpy(&m_kpAppInfo, pAppInfo, sizeof(KP_APP_INFO));
//	return TRUE;
// }

BOOL CPluginManager::SetDirectCommandRange(DWORD dwFirstCommand, DWORD dwLastCommand)
{
	ASSERT(dwFirstCommand <= dwLastCommand);
	if(dwFirstCommand > dwLastCommand) return FALSE;

	m_dwFirstCommand = dwFirstCommand;
	m_dwLastCommand = dwLastCommand;

	return TRUE;
}

BOOL CPluginManager::_AssignPluginCommands()
{
	DWORD posCmd = m_dwFirstCommand;
	BOOL bRet = TRUE;

	ASSERT(m_dwLastCommand != 0); if(m_dwLastCommand == 0) return FALSE;

	for(size_t i = 0; i < m_plugins.size(); ++i)
	{
		KP_PLUGIN_INSTANCE* p = &m_plugins[i];
		if(p == NULL) { ASSERT(FALSE); bRet = FALSE; continue; }

		if((p->hinstDLL != NULL) && (p->pInterface != NULL))
		{
			const DWORD dwNumCommands = p->pInterface->GetMenuItemCount();
			KP_MENU_ITEM* pMenuItems = p->pInterface->GetMenuItems();

			if((dwNumCommands == 0) || (pMenuItems == NULL)) continue;

			for(DWORD j = 0; j < dwNumCommands; ++j)
			{
				// Check if we have haven't run out of commands yet...
				if(posCmd >= (m_dwLastCommand - 1))
				{
					pMenuItems[j].dwCommandID = 0; // 0 = command unused
					bRet = FALSE;
					continue;
				}
				else pMenuItems[j].dwCommandID = posCmd; // Assign!

				// Fix command string
				if(pMenuItems[j].lpCommandString == NULL)
					pMenuItems[j].lpCommandString = const_cast<LPTSTR>(g_strEmpty);

				++posCmd;
			}
		}
	}

	ASSERT(posCmd <= (m_dwLastCommand + 1));
	return bRet;
}

bool CPluginManager::_IsValidPlugin(LPCTSTR lpFile, KP_PLUGIN_INSTANCE* pOutStruct)
{
	if(lpFile == NULL) { ASSERT(FALSE); return false; }

	CVersionInfo v;
	if(v.Load(lpFile) == FALSE) return false;

	CString strProdName = PWM_PRODUCT_NAME_SHORT;
	strProdName += _T(" Plugin");

	if(v.GetProductName() != strProdName) return false;

	if(pOutStruct != NULL)
	{
		pOutStruct->strName = (LPCTSTR)v.GetFileDescription();
		pOutStruct->strVersion = (LPCTSTR)v.GetFileVersionAsString();
		pOutStruct->strAuthor = (LPCTSTR)v.GetCompanyName();
	}

	return true;
}

std::vector<std_string> CPluginManager::FindPluginCandidates()
{
	std::vector<std_string> v;

	const std_string strBaseDir = Executable::instance().getPathOnly();
	const std_string strSearchPattern = (strBaseDir + _T("*.dll"));

	WIN32_FIND_DATA wfd;
	ZeroMemory(&wfd, sizeof(WIN32_FIND_DATA));
	HANDLE hFind = FindFirstFile(strSearchPattern.c_str(), &wfd);
	if(hFind == INVALID_HANDLE_VALUE) return v; // Valid, but no files

	while(1)
	{
		v.push_back(strBaseDir + wfd.cFileName);

		if(FindNextFile(hFind, &wfd) == FALSE) break;
	}

	FindClose(hFind);
	return v;
}

BOOL CPluginManager::LoadAllPlugins()
{
	std::vector<std_string> vCandidates = FindPluginCandidates();
	IKpUnknown* pAPI = &CKpApiImpl::Instance();

	for(size_t i = 0; i < vCandidates.size(); ++i)
	{
		KP_PLUGIN_INSTANCE kppi;
		CPluginManager::ClearStructure(&kppi);

		std_string strPluginPath = vCandidates[i];
		if(_IsValidPlugin(strPluginPath.c_str(), &kppi) == false) continue;

		kppi.hinstDLL = LoadLibrary(strPluginPath.c_str());
		if(kppi.hinstDLL == NULL) continue;

		// Call optional library initialization function
		LPKPLIBFUNC lpInit = (LPKPLIBFUNC)GetProcAddress(kppi.hinstDLL, KP_I_INITIALIZELIB);
		if(lpInit != NULL)
		{
			if(lpInit(pAPI) != S_OK) { FreePluginDllEx(kppi.hinstDLL); continue; }
		}

		LPKPCREATEINSTANCE lpCreate = (LPKPCREATEINSTANCE)GetProcAddress(
			kppi.hinstDLL, KP_I_CREATEINSTANCE);
		if(lpCreate == NULL) { FreePluginDllEx(kppi.hinstDLL); continue; }

		const HRESULT hr = lpCreate(IID_IKpPlugin, (void**)&kppi.pInterface, pAPI);
		if((hr != S_OK) || (kppi.pInterface == NULL))
			{ FreePluginDllEx(kppi.hinstDLL); continue; }

		kppi.dwPluginID = m_dwFreePluginID;
		++m_dwFreePluginID;

		kppi.strPath = strPluginPath;

		m_plugins.push_back(kppi);
	}

	return this->_AssignPluginCommands();
}

void CPluginManager::FreePluginDllEx(HMODULE h)
{
	if(h == NULL) { ASSERT(FALSE); return; }

	// Call optional library release function
	LPKPLIBFUNC lpRelease = (LPKPLIBFUNC)GetProcAddress(h, KP_I_RELEASELIB);
	if(lpRelease != NULL)
	{
		IKpUnknown* pAPI = &CKpApiImpl::Instance();
		VERIFY(lpRelease(pAPI) == S_OK);
	}

	FreeLibrary(h);
}

BOOL CPluginManager::UnloadAllPlugins()
{
	for(size_t i = 0; i < m_plugins.size(); ++i)
	{
		KP_PLUGIN_INSTANCE* p = &m_plugins[i];

		if(p->hinstDLL != NULL) // Unloaded already?
		{
			if(p->pInterface != NULL)
			{
				VERIFY(p->pInterface->Release() == 0);
				p->pInterface = NULL;
			}

			FreePluginDllEx(p->hinstDLL);
			p->hinstDLL = NULL;
		}
	}

	return TRUE;
}

// BOOL CPluginManager::EnablePluginByID(DWORD dwPluginID, BOOL bEnable)
// {
//	BOOL bRet = FALSE;
//
//	for(unsigned int i = 0; i < m_plugins.size(); i++)
//	{
//		if(m_plugins[i].dwPluginID == dwPluginID)
//		{
//			m_plugins[i].bEnabled = bEnable;
//			bRet = TRUE;
//			break;
//		}
//	}
//
//	return bRet;
// }

// BOOL CPluginManager::EnablePluginByStr(LPCTSTR lpPluginFile, BOOL bEnable)
// {
//	CString strT, strShort;
//	unsigned int i;
//	BOOL bRet = FALSE;
//
//	ASSERT(lpPluginFile != NULL); if(lpPluginFile == NULL) return FALSE;
//
//	// Only plugins in the KeePass directory are allowed
//	if(_tcschr(lpPluginFile, _T('\\')) != NULL) return FALSE;
//	if(_tcschr(lpPluginFile, _T('/')) != NULL) return FALSE;
//
//	for(i = 0; i < (unsigned int)m_plugins.size(); i++)
//	{
//		strT = m_plugins[i].tszFile;
//		strShort = CsFileOnly(&strT);
//
//		if(_tcscmp((LPCTSTR)strShort, lpPluginFile) == 0)
//		{
//			m_plugins[i].bEnabled = bEnable;
//			bRet = TRUE;
//			break;
//		}
//	}
//
//	return bRet;
// }

KP_PLUGIN_INSTANCE* CPluginManager::GetPluginByID(DWORD dwID)
{
	for(size_t i = 0; i < m_plugins.size(); ++i)
	{
		if(m_plugins[i].dwPluginID == dwID)
			return &m_plugins[i];
	}

	return NULL;
}

BOOL CPluginManager::CallSinglePlugin(DWORD dwPluginID, DWORD dwCode, LPARAM lParamW,
	LPARAM lParamL)
{
	KP_PLUGIN_INSTANCE* p = GetPluginByID(dwPluginID);
	if(p == NULL) { ASSERT(FALSE); return FALSE; }

	if((p->hinstDLL != NULL) && (p->pInterface != NULL))
		return p->pInterface->OnMessage(dwCode, lParamW, lParamL);

	return TRUE; // TRUE = continue work (default value if failed)
}

BOOL CPluginManager::CallPlugins(DWORD dwCode, LPARAM lParamW, LPARAM lParamL)
{
	BOOL bRet = TRUE;

	if(dwCode == KPM_DIRECT_EXEC) // Call single plugin
	{
		for(size_t i = 0; i < m_plugins.size(); ++i)
		{
			KP_PLUGIN_INSTANCE* p = &m_plugins[i];
			if((p->hinstDLL == NULL) || (p->pInterface == NULL)) continue;

			const DWORD dwNumCommands = p->pInterface->GetMenuItemCount();
			KP_MENU_ITEM* pMenuItems = p->pInterface->GetMenuItems();

			if((dwNumCommands == 0) || (pMenuItems == NULL)) continue;

			for(size_t j = 0; j < dwNumCommands; ++j)
			{
				if(pMenuItems[j].dwCommandID == (DWORD)lParamW)
					return p->pInterface->OnMessage(KPM_DIRECT_EXEC, lParamW, lParamL);
			}
		}
	}
	else // Call all plugins
	{
		for(size_t i = 0; i < m_plugins.size(); ++i)
		{
			KP_PLUGIN_INSTANCE* p = &m_plugins[i];
			if((p->hinstDLL == NULL) || (p->pInterface == NULL)) continue;

			bRet &= p->pInterface->OnMessage(dwCode, lParamW, lParamL);
		}
	}

	return bRet;
}

bool CPluginManager::UsesCmdArg(const std_string& argument) const
{
	typedef std::vector<KP_PLUGIN_INSTANCE>::const_iterator kpi_const_iterator;
	const kpi_const_iterator kpi_end(m_plugins.end());

	for(kpi_const_iterator it = m_plugins.begin(); it != kpi_end; ++it)
	{
		if((it->hinstDLL == NULL) || (it->pInterface == NULL)) { ASSERT(FALSE); continue; }

		LPCTSTR cmdLineArgPrefix = it->pInterface->GetProperty(KPPS_COMMANDLINEARGPREFIX);
		if((cmdLineArgPrefix == NULL) || (cmdLineArgPrefix[0] == 0))
			continue; // Plugin has no command line options

		const CommandLineOption commandLineOption(cmdLineArgPrefix);
		ASSERT(_T('.') == *commandLineOption.optionName().rbegin()); // Assure option is a plugin option
		if(commandLineOption.isOption(argument)) return true;
	}

	if(argument == DUMMY_COMMAND_LINE_OPT) return true;

	return false;
}
