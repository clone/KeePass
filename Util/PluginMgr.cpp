/*
  Copyright (c) 2003-2005, Dominik Reichl <dominik.reichl@t-online.de>
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer. 
  - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  - Neither the name of ReichlSoft nor the names of its contributors may be
    used to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#include "StdAfx.h"
#include <tchar.h>
#include "PluginMgr.h"
#include "../NewGUI/TranslateEx.h"
#include "../Util/StrUtil.h"

CPluginManager::CPluginManager()
{
	m_dwFreePluginID = 0;
	m_plugins.clear();

	ZeroMemory(&m_kpAppInfo, sizeof(KP_APP_INFO));

	m_dwFirstCommand = 0;
	m_dwLastCommand = 0;
}

CPluginManager::~CPluginManager()
{
	_CleanUp();
}

void CPluginManager::_CleanUp()
{
	UnloadAllPlugins();

	m_plugins.clear();

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

BOOL CPluginManager::AddPlugin(LPCTSTR lpFile)
{
	KP_PLUGIN_INSTANCE kppi;
	KP_PLUGIN_INSTANCE *p;
	FILE *fp;

	ASSERT(lpFile != NULL); if(lpFile == NULL) return FALSE;

	ZeroMemory(&kppi, sizeof(KP_PLUGIN_INSTANCE));

	fp = _tfopen(lpFile, _T("rb"));
	if(fp == NULL) return FALSE;
	fclose(fp); fp = NULL;

	kppi.bEnabled = FALSE;
	kppi.hinstDLL = NULL;
	_tcsncpy(kppi.tszFile, lpFile, MAX_PATH - 1);

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

BOOL CPluginManager::AddAllPlugins(LPCTSTR lpBaseSearchPath, LPCTSTR lpMask, BOOL bShowErrors)
{
	HANDLE hFind;
	WIN32_FIND_DATA wfd;
	TCHAR tszPath[MAX_PATH];

	ASSERT(lpBaseSearchPath != NULL); if(lpBaseSearchPath == NULL) return FALSE;

	_tcscpy(tszPath, lpBaseSearchPath);
	_tcscat(tszPath, lpMask);

	ZeroMemory(&wfd, sizeof(WIN32_FIND_DATA));
	hFind = FindFirstFile(tszPath, &wfd);
	if(hFind == INVALID_HANDLE_VALUE) return TRUE; // Valid, but no files

	while(1)
	{
		_tcscpy(tszPath, lpBaseSearchPath);
		_tcscat(tszPath, wfd.cFileName);

		if((AddPlugin(tszPath) == FALSE) && (bShowErrors == TRUE))
		{
			std::string str;
			str = TRL("Plugin");
			str += " <";
			str += wfd.cFileName;
			str += "> ";
			str += TRL("cannot be loaded!");
			MessageBox(GetDesktopWindow(), str.c_str(), TRL("Plugin Manager"),
				MB_OK | MB_ICONWARNING);
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
			std::string str;
			TCHAR tszTemp[20];

			str = TRL("Plugin"); str += ": <";
			str += p->tszFile; str += ">\r\n\r\n";
			str += TRL("This plugin has been designed for a different application version and may be incompatible with this one.");
			str += "\r\n\r\n";
			str += TRL("Application version:"); str += " ";
			_stprintf(tszTemp, _T("%u.%u.%u.%u"), (m_kpAppInfo.dwAppVersion >> 24) & 0xFF,
				(m_kpAppInfo.dwAppVersion >> 16) & 0xFF,
				(m_kpAppInfo.dwAppVersion >> 8) & 0xFF,
				m_kpAppInfo.dwAppVersion & 0xFF);
			str += tszTemp; str += "\r\n";
			str += TRL("Plugin has been designed for:"); str += " ";
			_stprintf(tszTemp, _T("%u.%u.%u.%u"), (p->info.dwForAppVer >> 24) & 0xFF,
				(p->info.dwForAppVer >> 16) & 0xFF,
				(p->info.dwForAppVer >> 8) & 0xFF,
				p->info.dwForAppVer & 0xFF);
			str += tszTemp; str += "\r\n\r\n";
			str += TRL("It is possible that the plugin is compatible, but it's also possible that it will crash KeePass.");
			str += "\r\n";
			str += TRL("Therefore save all data before you continue."); str += "\r\n\r\n";
			str += TRL("Do you want to load the plugin?");

			int nRet;
			nRet = MessageBox(GetDesktopWindow(), str.c_str(), TRL("Plugin Manager"), MB_YESNO | MB_ICONQUESTION);

			if(nRet == IDNO)
			{
				p->lpExit(0, 0);
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
	unsigned int i;

	for(i = 0; i < (unsigned int)m_plugins.size(); i++)
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

C_FN_SHARE DWORD KP_Query(DWORD dwID, LPARAM lParam)
{
	DWORD dwRet = 0;

	UNREFERENCED_PARAMETER(lParam);

	switch(dwID)
	{
	case KPQUERY_NULL:
		break;

	case KPQUERY_VERSION:
		dwRet = PWM_VERSION_DW;
		break;

	default:
		break;
	}

	return dwRet;
}
