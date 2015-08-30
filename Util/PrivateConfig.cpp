/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2005 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "PrivateConfig.h"
#include "WinUtil.h"
#include "../PwSafe/PwManager.h"

CPrivateConfig::CPrivateConfig(BOOL bReqWriteAccess)
{
	TCHAR tszAppDir[MAX_PATH * 2];

	tszAppDir[0] = 0; tszAppDir[1] = 0;

	ASSERT(SI_REGSIZE >= (MAX_PATH * 2));

	GetApplicationDirectory(m_szFileLocal, MAX_PATH * 2 - 2, TRUE, FALSE);
	_tcscat(m_szFileLocal, _T("\\"));
	_tcscat(m_szFileLocal, PWM_EXENAME);
	_tcscat(m_szFileLocal, _T(".ini"));

	m_szFileUser[0] = 0; m_szFileUser[1] = 0;

	HINSTANCE hShell32 = LoadLibrary(_T("Shell32"));
	if(hShell32 != NULL)
	{
		LPSHGETSPECIALFOLDERPATH lpGet = (LPSHGETSPECIALFOLDERPATH)GetProcAddress(hShell32, "SHGetSpecialFolderPathA");

		// WinNT 4.0 doesn't support this function, therefore check for NULL pointer without assertion
		if(lpGet != NULL) lpGet(NULL, m_szFileUser, CSIDL_APPDATA, TRUE);

		lpGet = NULL; FreeLibrary(hShell32); hShell32 = NULL;
	} else { ASSERT(FALSE); }

	if(m_szFileUser[0] == 0)
	{
		_tcscpy(m_szFileUser, PWM_EXENAME);
		_tcscat(m_szFileUser, _T(".ini"));
	}
	else
	{
		DWORD uLen = (DWORD)_tcslen(m_szFileUser);
		if(uLen != 0)
		{
			if(m_szFileUser[uLen - 1] != _T('\\'))
				_tcscat(m_szFileUser, _T("\\"));

			_tcscat(m_szFileUser, PWM_EXENAME);

			_tcscpy(tszAppDir, m_szFileUser);

			_tcscat(m_szFileUser, _T("\\"));
			_tcscat(m_szFileUser, PWM_EXENAME);
			_tcscat(m_szFileUser, _T(".ini"));
		}
	}

	_tcscpy(m_szFileGeneric, PWM_EXENAME);
	_tcscat(m_szFileGeneric, _T(".ini"));

	m_nUseDir = 0;
	if(bReqWriteAccess == TRUE)
	{
		if(_FileWritable(m_szFileLocal) == FALSE)
		{
			m_nUseDir = 1;

			if(tszAppDir[0] != 0) CreateDirectory(tszAppDir, NULL);

			if(_FileWritable(m_szFileUser) == FALSE)
				m_nUseDir = 2;
		}
	}
	else // bReqWriteAccess == FALSE
	{
		if(_FileAccessible(m_szFileLocal) == FALSE)
		{
			m_nUseDir = 1;

			if(_FileAccessible(m_szFileUser) == FALSE)
				m_nUseDir = 2;
		}
	}
}

CPrivateConfig::~CPrivateConfig()
{
}

BOOL CPrivateConfig::Set(const TCHAR *pszField, PCFG_IN TCHAR *pszValue)
{
	BOOL bRet = FALSE;

	ASSERT(pszField != NULL); if(pszField == NULL) return FALSE;
	ASSERT(_tcslen(pszField) > 0); if(_tcslen(pszField) == 0) return FALSE;
	// pszValue may be NULL

#ifndef _WIN32_WCE
	// First check if the existing value in the INI file is the same as the
	// one to be written. If so, we don't need to write it again. For
	// devices that cache reads but not writes, this leads to a performance
	// improvement. Thanks to Brad Clarke for the idea and patch.
	TCHAR tszTemp[SI_REGSIZE];
	if((pszValue != NULL) && (Get(pszField, tszTemp) == TRUE))
	{
		if(_tcscmp(tszTemp, pszValue) == 0) return TRUE;
	}

	if(m_nUseDir == 0)
		bRet = WritePrivateProfileString(PWM_EXENAME, pszField, pszValue, m_szFileLocal);

	if(bRet == FALSE)
	{
		if(m_nUseDir <= 1)
			bRet = WritePrivateProfileString(PWM_EXENAME, pszField, pszValue, m_szFileUser);

		if(bRet == FALSE)
			bRet = WritePrivateProfileString(PWM_EXENAME, pszField, pszValue, m_szFileGeneric);
	}
#else
	bRet = FALSE;
	ASSERT(FALSE); // Implement before using on WinCE
#endif

	return bRet;
}

BOOL CPrivateConfig::Get(const TCHAR *pszField, PCFG_OUT TCHAR *pszValue)
{
	BOOL bRet = FALSE;
	TCHAR chEmpty[2] = { 0, 0 };

	ASSERT((pszField != NULL) && (pszValue != NULL));
	if((pszField == NULL) || (pszValue == NULL)) return FALSE;

	pszValue[0] = 0; pszValue[1] = 0; // Reset string

#ifndef _WIN32_WCE
	if(m_nUseDir == 0)
		bRet = GetPrivateProfileString(PWM_EXENAME, pszField, chEmpty, pszValue, SI_REGSIZE, m_szFileLocal);

	if(bRet == FALSE)
	{
		if(m_nUseDir <= 1)
			bRet = GetPrivateProfileString(PWM_EXENAME, pszField, chEmpty, pszValue, SI_REGSIZE, m_szFileUser);

		if(bRet == FALSE)
			bRet = GetPrivateProfileString(PWM_EXENAME, pszField, chEmpty, pszValue, SI_REGSIZE, m_szFileGeneric);
	}
#else
	bRet = FALSE;
	ASSERT(FALSE); // Implement before using on WinCE
#endif

	return bRet;
}

BOOL CPrivateConfig::SetBool(const TCHAR *pszField, BOOL bValue)
{
	return Set(pszField, (bValue == TRUE) ? _T("True") : _T("False"));
}

BOOL CPrivateConfig::GetBool(const TCHAR *pszField, BOOL bDefault)
{
	TCHAR tszTemp[SI_REGSIZE];

	if(Get(pszField, tszTemp) == FALSE) return bDefault;

	if(_tcscmp(tszTemp, _T("True")) == 0) return TRUE;
	else if(_tcscmp(tszTemp, _T("False")) == 0) return FALSE;

	return bDefault;
}
