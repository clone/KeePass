/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2007 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "PluginMgr.h"
#include "../../KeePassLibCpp/Util/TranslateEx.h"
#include "../../KeePassLibCpp/Util/StrUtil.h"
#include "../../KeePassLibCpp/Util/MemUtil.h"
#include "../Util/CmdLine/CommandLineOption.h"

CPluginManager& CPluginManager::Instance()
{
	static CPluginManager singleton;
	return singleton;
}

CPluginManager::CPluginManager()
{
	m_dwFreePluginID = 0;
	m_plugins.clear();
	m_vKnownNames.clear();

	ZeroMemory(&m_kpAppInfo, sizeof(KP_APP_INFO));

	m_dwFirstCommand = 0;
	m_dwLastCommand = 0;
}

CPluginManager::~CPluginManager()
{
	CleanUp();
}

void CPluginManager::CleanUp()
{
	UnloadAllPlugins();

	m_plugins.clear();
	m_vKnownNames.clear();

	m_dwFreePluginID = 0;
	ZeroMemory(&m_kpAppInfo, sizeof(KP_APP_INFO));
}

BOOL CPluginManager::SetAppInfo(const KP_APP_INFO *pAppInfo)
{
	ASSERT(pAppInfo != NULL); if(pAppInfo == NULL) return FALSE;

	memcpy(&m_kpAppInfo, pAppInfo, sizeof(KP_APP_INFO));
	return TRUE;
}

BOOL CPluginManager::SetDirectCommandRange(DWORD dwFirstCommand, DWORD dwLastCommand)
{
	ASSERT(dwFirstCommand <= dwLastCommand);
	if(dwFirstCommand > dwLastCommand) return FALSE;

	m_dwFirstCommand = dwFirstCommand;
	m_dwLastCommand = dwLastCommand;

	return TRUE;
}

BOOL CPluginManager::AssignPluginCommands()
{
	unsigned int i;
	DWORD j, posCmd = m_dwFirstCommand;
	KP_PLUGIN_INSTANCE *p;
	BOOL bRet = TRUE;

	ASSERT(m_dwLastCommand != 0); if(m_dwLastCommand == 0) return FALSE;

	for(i = 0; i < (unsigned int)m_plugins.size(); i++)
	{
		p = &m_plugins[i];
		ASSERT(p != NULL); if(p == NULL) return FALSE;

		if((p->hinstDLL != NULL) && (p->info.dwNumCommands != 0) &&
			(p->info.pMenuItems != NULL))
		{
			for(j = 0; j < p->info.dwNumCommands; j++)
			{
				p->info.pMenuItems[j].dwCommandID = 0; // 0 = command unused

				// Check if we have haven't run out of commands yet...
				if(posCmd >= (m_dwLastCommand - 1)) continue;

				p->info.pMenuItems[j].dwCommandID = posCmd; // Assign!

				// Fix command string
				if(p->info.pMenuItems[j].lpCommandString == NULL)
					p->info.pMenuItems[j].lpCommandString = _T("");

				posCmd++;
			}
		}
	}

	ASSERT(posCmd <= (m_dwLastCommand + 1));
	return bRet;
}

BOOL CPluginManager::AddPlugin(LPCTSTR lpFile, BOOL bEnable)
{
	KP_PLUGIN_INSTANCE kppi;
	KP_PLUGIN_INSTANCE *p;

	ASSERT(lpFile != NULL); if(lpFile == NULL) return FALSE;

	ZeroMemory(&kppi, sizeof(KP_PLUGIN_INSTANCE));

	if(IsPluginValid(lpFile) == FALSE) return FALSE;

	kppi.bEnabled = bEnable;
	kppi.hinstDLL = NULL;
	_tcsncpy_s(kppi.tszFile, _countof(kppi.tszFile), lpFile, MAX_PATH - 1);

	int i;
	bool bInListAlready = false;
	for(i = 0; i < (int)m_plugins.size(); i++)
	{
		p = &m_plugins[i];

		if(_tcscmp(lpFile, p->tszFile) == 0)
		{
			bInListAlready = true;
			break;
		}
	}

	if(bInListAlready == false)
	{
		kppi.dwPluginID = m_dwFreePluginID;
		m_dwFreePluginID++;
		m_plugins.push_back(kppi);
	}

	return TRUE;
}

BOOL CPluginManager::IsPluginValid(LPCTSTR lpFile)
{
	FILE *fp = NULL;
	BOOL bInit = FALSE, bCall = FALSE, bExit = FALSE;

	_tfopen_s(&fp, lpFile, _T("rb"));
	if(fp == NULL) return FALSE;

	fseek(fp, 0, SEEK_END);
	unsigned long uFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	BYTE *p = new BYTE[uFileSize + 1];
	if(uFileSize != 0) fread(p, 1, uFileSize, fp);
	p[uFileSize] = 0;
	fclose(fp); fp = NULL;

	const size_t nInit = strlen(KP_I_INIT);
	const size_t nCall = strlen(KP_I_CALL);
	const size_t nExit = strlen(KP_I_EXIT);

	LPCSTR lp = (LPCSTR)p;
	for(unsigned long i = 0; i < uFileSize; i++)
	{
		if(strncmp(lp, KP_I_INIT, nInit) == 0) bInit = TRUE;
		if(strncmp(lp, KP_I_CALL, nCall) == 0) bCall = TRUE;
		if(strncmp(lp, KP_I_EXIT, nExit) == 0) bExit = TRUE;

		lp++;
	}

	SAFE_DELETE_ARRAY(p);
	if((bInit == FALSE) || (bCall == FALSE) || (bExit == FALSE)) return FALSE;
	return TRUE;
}

BOOL CPluginManager::AddAllPlugins(LPCTSTR lpBaseSearchPath, LPCTSTR lpMask, BOOL bOnlyKnown)
{
	HANDLE hFind;
	WIN32_FIND_DATA wfd;
	TCHAR tszPath[MAX_PATH];

	ASSERT(lpBaseSearchPath != NULL); if(lpBaseSearchPath == NULL) return FALSE;

	_tcscpy_s(tszPath, _countof(tszPath), lpBaseSearchPath);
	_tcscat_s(tszPath, _countof(tszPath), lpMask);

	ZeroMemory(&wfd, sizeof(WIN32_FIND_DATA));
	hFind = FindFirstFile(tszPath, &wfd);
	if(hFind == INVALID_HANDLE_VALUE) return TRUE; // Valid, but no files

	while(1)
	{
		_tcscpy_s(tszPath, _countof(tszPath), lpBaseSearchPath);
		_tcscat_s(tszPath, _countof(tszPath), wfd.cFileName);

		if(bOnlyKnown == FALSE) AddPlugin(tszPath, FALSE); // Add but don't enable
		else
		{
			for(unsigned int i = 0; i < m_vKnownNames.size(); i++)
				if(_tcscmp(wfd.cFileName, m_vKnownNames[i].c_str()) == 0)
				{
					AddPlugin(tszPath, TRUE); // Add and enable
					break;
				}
		}

		if(FindNextFile(hFind, &wfd) == FALSE) break;
	}

	FindClose(hFind); hFind = NULL;
	return TRUE;
}

BOOL CPluginManager::LoadAllPlugins()
{
	unsigned int i;
	KP_PLUGIN_INSTANCE *p;

	for(i = 0; i < (unsigned int)m_plugins.size(); i++)
	{
		p = &m_plugins[i];

		if(p->bEnabled == FALSE) continue; // Disabled?

		ASSERT(p->hinstDLL == NULL); if(p->hinstDLL != NULL) continue; // Loaded already?

		p->hinstDLL = LoadLibrary(p->tszFile);
		if(p->hinstDLL == NULL) continue;

		p->lpInit = (LPKEEPLUGININIT)GetProcAddress(p->hinstDLL, KP_I_INIT);
		if(p->lpInit == NULL) { FreeLibrary(p->hinstDLL); p->hinstDLL = NULL; continue; }

		p->lpCall = (LPKEEPLUGINCALL)GetProcAddress(p->hinstDLL, KP_I_CALL);
		if(p->lpCall == NULL) { FreeLibrary(p->hinstDLL); p->hinstDLL = NULL; continue; }

		p->lpExit = (LPKEEPLUGINEXIT)GetProcAddress(p->hinstDLL, KP_I_EXIT);
		if(p->lpExit == NULL) { FreeLibrary(p->hinstDLL); p->hinstDLL = NULL; continue; }

		// Call the initialization function of the plugin
		if(p->lpInit(&m_kpAppInfo, &p->info) == FALSE)
			{ FreeLibrary(p->hinstDLL); p->hinstDLL = NULL; continue; }

		if(p->info.dwForAppVer != m_kpAppInfo.dwAppVersion)
		{
			CString str;
			TCHAR tszTemp[20];

			str = TRL("Plugin"); str += _T(": <");
			str += p->tszFile; str += _T(">\r\n\r\n");
			str += TRL("This plugin has been designed for a different application version and may be incompatible with this one.");
			str += _T("\r\n\r\n");
			str += TRL("Application version:"); str += _T(" ");
			_stprintf_s(tszTemp, _countof(tszTemp), _T("%u.%u.%u.%u"),
				(m_kpAppInfo.dwAppVersion >> 24) & 0xFF,
				(m_kpAppInfo.dwAppVersion >> 16) & 0xFF,
				(m_kpAppInfo.dwAppVersion >> 8) & 0xFF,
				m_kpAppInfo.dwAppVersion & 0xFF);
			str += tszTemp; str += _T("\r\n");
			str += TRL("Plugin has been designed for:"); str += _T(" ");
			_stprintf_s(tszTemp, _countof(tszTemp), _T("%u.%u.%u.%u"),
				(p->info.dwForAppVer >> 24) & 0xFF,
				(p->info.dwForAppVer >> 16) & 0xFF,
				(p->info.dwForAppVer >> 8) & 0xFF,
				p->info.dwForAppVer & 0xFF);
			str += tszTemp; str += _T("\r\n\r\n");
			str += TRL("It is possible that the plugin is compatible, but it's also possible that it will crash KeePass.");
			str += _T("\r\n");
			str += TRL("Therefore save all data before you continue."); str += _T("\r\n\r\n");
			str += TRL("Do you want to load the plugin?");

			int nRet;
			nRet = MessageBox(GetDesktopWindow(), str, TRL("Plugin Manager"), MB_YESNO | MB_ICONQUESTION);

			if(nRet == IDNO)
			{
				p->lpExit(0, 0);
				p->bEnabled = FALSE;
				p->lpInit = NULL; p->lpCall = NULL; p->lpExit = NULL;
				FreeLibrary(p->hinstDLL); p->hinstDLL = NULL;
				continue;
			}
		}
	}

	return AssignPluginCommands();
}

BOOL CPluginManager::UnloadAllPlugins()
{
	unsigned int i;
	KP_PLUGIN_INSTANCE *p;

	for(i = 0; i < (unsigned int)m_plugins.size(); i++)
	{
		p = &m_plugins[i];

		if(p->hinstDLL != NULL) // Unloaded already?
		{
			ASSERT(p->lpExit != NULL);
			if(p->lpExit != NULL) p->lpExit(0, 0);
			p->lpInit = NULL; p->lpCall = NULL; p->lpExit = NULL;

			FreeLibrary(p->hinstDLL); p->hinstDLL = NULL;
		}
	}

	return TRUE;
}

BOOL CPluginManager::EnablePluginByID(DWORD dwPluginID, BOOL bEnable)
{
	unsigned int i;
	BOOL bRet = FALSE;

	for(i = 0; i < m_plugins.size(); i++)
	{
		if(m_plugins[i].dwPluginID == dwPluginID)
		{
			m_plugins[i].bEnabled = bEnable;
			bRet = TRUE;
			break;
		}
	}

	return bRet;
}

BOOL CPluginManager::EnablePluginByStr(LPCTSTR lpPluginFile, BOOL bEnable)
{
	CString strT, strShort;
	unsigned int i;
	BOOL bRet = FALSE;

	ASSERT(lpPluginFile != NULL); if(lpPluginFile == NULL) return FALSE;

	// Only plugins in the KeePass directory are allowed
	if(_tcschr(lpPluginFile, _T('\\')) != NULL) return FALSE;
	if(_tcschr(lpPluginFile, _T('/')) != NULL) return FALSE;

	for(i = 0; i < (unsigned int)m_plugins.size(); i++)
	{
		strT = m_plugins[i].tszFile;
		strShort = CsFileOnly(&strT);

		if(_tcscmp((LPCTSTR)strShort, lpPluginFile) == 0)
		{
			m_plugins[i].bEnabled = bEnable;
			bRet = TRUE;
			break;
		}
	}

	return bRet;
}

KP_PLUGIN_INSTANCE *CPluginManager::GetPluginByID(DWORD dwID)
{
	for(unsigned int i = 0; i < (unsigned int)m_plugins.size(); i++)
	{
		if(m_plugins[i].dwPluginID == dwID)
			return &m_plugins[i];
	}

	return NULL;
}

BOOL CPluginManager::CallSinglePlugin(DWORD dwPluginID, DWORD dwCode, LPARAM lParamW, LPARAM lParamL)
{
	KP_PLUGIN_INSTANCE *p = GetPluginByID(dwPluginID);
	ASSERT(p != NULL); if(p == NULL) return FALSE;

	if(p->hinstDLL != NULL)
		return p->lpCall(dwCode, lParamW, lParamL);

	return TRUE; // TRUE = continue work
}

BOOL CPluginManager::CallPlugins(DWORD dwCode, LPARAM lParamW, LPARAM lParamL)
{
	BOOL bRet = TRUE;
	unsigned int i, j;
	KP_PLUGIN_INSTANCE *p;
	KP_MENU_ITEM *pMenuItems;

	if(dwCode == KPM_DIRECT_EXEC)
	{
		for(i = 0; i < (unsigned int)m_plugins.size(); i++)
		{
			p = &m_plugins[i];

			if(p->hinstDLL == NULL) continue;

			pMenuItems = p->info.pMenuItems;
			for(j = 0; j < (unsigned int)p->info.dwNumCommands; j++)
			{
				if(pMenuItems[j].dwCommandID == (DWORD)lParamW)
					return p->lpCall(KPM_DIRECT_EXEC, lParamW, lParamL);
			}
		}
	}
	else
	{
		for(i = 0; i < (unsigned int)m_plugins.size(); i++) // Call all plugins
		{
			p = &m_plugins[i];

			if(p->hinstDLL == NULL) continue;

			bRet &= p->lpCall(dwCode, lParamW, lParamL);
		}
	}

	return bRet;
}

BOOL CPluginManager::UsesCmdArg(const std_string& argument) const
{
	typedef std::vector<KP_PLUGIN_INSTANCE>::const_iterator const_iterator;
	const const_iterator end(m_plugins.end());

	for(const_iterator it = m_plugins.begin(); it != end; ++it)
	{
		ASSERT(it->hinstDLL != NULL); // Assure plugin is loaded.

		const TCHAR* const cmdLineArgPrefix = it->info.cmdLineArgPrefix;
		if(cmdLineArgPrefix == NULL) continue; // Plugin has no command line options

		const CommandLineOption commandLineOption(cmdLineArgPrefix);
		ASSERT(_T('.') == *commandLineOption.optionName().rbegin()); // Assure option is a plugin option
		if(commandLineOption.isOption(argument)) return true;
	}

	return false;
}
