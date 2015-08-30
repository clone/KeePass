/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2010 Dominik Reichl <dominik.reichl@t-online.de>

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
// #include "PrivateConfig.h"
// #include "WinUtil.h"
// #include "../../KeePassLibCpp/PwManager.h"
// #include "../../KeePassLibCpp/Util/AppUtil.h"

/*
CPrivateConfig::CPrivateConfig(BOOL bRequireWriteAccess)
{
	TCHAR tszAppDir[MAX_PATH * 2];
	DWORD dwSubLen;

	tszAppDir[0] = 0; tszAppDir[1] = 0;

	ASSERT(SI_REGSIZE >= (MAX_PATH * 2));

	m_szFileLocal[0] = 0; m_szFileLocal[1] = 0;
	dwSubLen = 1 + (DWORD)_tcslen(PWM_EXENAME) + 4 + 2;
	VERIFY(GetApplicationDirectory(m_szFileLocal, (MAX_PATH * 2) - dwSubLen, TRUE, FALSE));
	_tcscat_s(m_szFileLocal, _countof(m_szFileLocal), _T("\\"));
	_tcscat_s(m_szFileLocal, _countof(m_szFileLocal), PWM_EXENAME);
	_tcscat_s(m_szFileLocal, _countof(m_szFileLocal), _T(".ini"));

	m_szFileUser[0] = 0; m_szFileUser[1] = 0;

	HINSTANCE hShell32 = AU_LoadLibrary(_T("Shell32.dll"));
	if(hShell32 != NULL)
	{
		LPSHGETSPECIALFOLDERPATH lpGet = (LPSHGETSPECIALFOLDERPATH)GetProcAddress(hShell32,
#ifndef _UNICODE
			"SHGetSpecialFolderPathA"
#else
			"SHGetSpecialFolderPathW"
#endif
			);

		// WinNT 4.0 doesn't support this function, therefore check for NULL pointer without assertion
		if(lpGet != NULL) lpGet(NULL, m_szFileUser, CSIDL_APPDATA, TRUE);

		lpGet = NULL; FreeLibrary(hShell32); hShell32 = NULL;
	} else { ASSERT(FALSE); }

	if(m_szFileUser[0] == 0)
	{
		_tcscpy_s(m_szFileUser, _countof(m_szFileUser), PWM_EXENAME); // Not found -- make the same as m_szFileGeneric
		_tcscat_s(m_szFileUser, _countof(m_szFileUser), _T(".ini"));
	}
	else
	{
		// Up to now, m_szFileUser just contains the users directory path, make
		// it a valid file location now.

		DWORD uLen = (DWORD)_tcslen(m_szFileUser); ASSERT(uLen != 0);

		if(uLen != 0)
		{
			if(m_szFileUser[uLen - 1] != _T('\\'))
				_tcscat_s(m_szFileUser, _countof(m_szFileUser), _T("\\"));

			_tcscat_s(m_szFileUser, _countof(m_szFileUser), PWM_EXENAME);

			_tcscpy_s(tszAppDir, _countof(tszAppDir), m_szFileUser);

			_tcscat_s(m_szFileUser, _countof(m_szFileUser), _T("\\"));
			_tcscat_s(m_szFileUser, _countof(m_szFileUser), PWM_EXENAME);
			_tcscat_s(m_szFileUser, _countof(m_szFileUser), _T(".ini"));
		}
	}

	_tcscpy_s(m_szFileGeneric, _countof(m_szFileGeneric), PWM_EXENAME);
	_tcscat_s(m_szFileGeneric, _countof(m_szFileGeneric), _T(".ini"));

	m_nUseDir = 0;
	if(bRequireWriteAccess == TRUE)
	{
		if(_FileWritable(m_szFileLocal) == FALSE)
		{
			m_nUseDir = 1;

			if(tszAppDir[0] != 0) CreateDirectory(tszAppDir, NULL);

			if(_FileWritable(m_szFileUser) == FALSE)
				m_nUseDir = 2; // Hopefully this never happens...
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

BOOL CPrivateConfig::Set(LPCTSTR pszField, LPCTSTR pszValue)
{
	LPCTSTR lpExeName = PWM_EXENAME;
	BOOL bRet = FALSE;

	ASSERT(pszField != NULL); if(pszField == NULL) return FALSE;
	ASSERT(pszField[0] != 0); if(pszField[0] == 0) return FALSE;
	// pszValue may be NULL -- this deletes the specified key

#ifndef _WIN32_WCE
	TCHAR tszTemp[SI_REGSIZE];
	tszTemp[0] = 0; tszTemp[1] = 0;
	if(pszValue == NULL) // Delete key?
	{
		// If the key doesn't exist, we don't need to delete it
		if(this->Get(pszField, tszTemp) == FALSE) return TRUE;
	}
	else
	{
		// Check if the existing value in the INI file is the same as the
		// one to be written. If so, we don't need to write it again. For
		// devices that cache reads but not writes, this leads to a performance
		// improvement. Thanks to Brad Clarke for the idea and patch.

		if(this->Get(pszField, tszTemp) == TRUE)
			if(_tcscmp(tszTemp, pszValue) == 0) return TRUE;
	}

	if(m_nUseDir == 0)
		bRet = WritePrivateProfileString(lpExeName, pszField, pszValue, m_szFileLocal);

	if(bRet == FALSE)
	{
		if(m_nUseDir <= 1)
			bRet = WritePrivateProfileString(lpExeName, pszField, pszValue, m_szFileUser);

		if(bRet == FALSE)
			bRet = WritePrivateProfileString(lpExeName, pszField, pszValue, m_szFileGeneric);
	}
#else
	bRet = FALSE;
	ASSERT(FALSE); // Implement before using on WinCE
#endif

	return bRet;
}

#pragma warning(push)
#pragma warning(disable: 4996) // _tcscpy deprecated

BOOL CPrivateConfig::Get(LPCTSTR pszField, LPTSTR pszValue) const
{
	LPCTSTR lpExeName = PWM_EXENAME;
	LPCTSTR lpNotFound = PCFG_NOTFOUND;
	TCHAR tszTemp[SI_REGSIZE];

	ASSERT(pszValue != NULL); if(pszValue == NULL) return FALSE;
	pszValue[0] = 0; pszValue[1] = 0; // Reset string
	tszTemp[0] = 0; tszTemp[1] = 0;

	ASSERT(pszField != NULL); if(pszField == NULL) return FALSE;
	ASSERT(pszField[0] != 0); if(pszField[0] == 0) return FALSE;

#ifndef _WIN32_WCE
	if(m_nUseDir == 0)
		GetPrivateProfileString(lpExeName, pszField, lpNotFound, tszTemp, SI_REGSIZE - 1, m_szFileLocal);

	if((_tcscmp(tszTemp, lpNotFound) == 0) || (m_nUseDir != 0))
	{
		tszTemp[0] = 0; tszTemp[1] = 0;

		if(m_nUseDir <= 1)
			GetPrivateProfileString(lpExeName, pszField, lpNotFound, tszTemp, SI_REGSIZE - 1, m_szFileUser);

		if((_tcscmp(tszTemp, lpNotFound) == 0) || (m_nUseDir == 2))
		{
			tszTemp[0] = 0; tszTemp[1] = 0;
			GetPrivateProfileString(lpExeName, pszField, lpNotFound, tszTemp, SI_REGSIZE - 1, m_szFileGeneric);

			if(_tcscmp(tszTemp, lpNotFound) == 0) return FALSE;
		}
	}

	_tcscpy(pszValue, tszTemp);
	return TRUE;
#else
	ASSERT(FALSE); // Implement before using on WinCE
	return FALSE;
#endif
}

#pragma warning(pop)

BOOL CPrivateConfig::SetBool(const TCHAR *pszField, BOOL bValue)
{
	return this->Set(pszField, (bValue == FALSE) ? _T("False") : _T("True"));
}

BOOL CPrivateConfig::GetBool(const TCHAR *pszField, BOOL bDefault) const
{
	TCHAR tszTemp[SI_REGSIZE];
	if(this->Get(pszField, tszTemp) == FALSE) return bDefault;

	if(_tcsicmp(tszTemp, _T("True")) == 0) return TRUE;
	else if(_tcsicmp(tszTemp, _T("False")) == 0) return FALSE;

	return bDefault;
}
*/
