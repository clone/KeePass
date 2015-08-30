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
#include "PrivateConfigEx.h"

#include "WinUtil.h"
#include "../Plugins/MsgInterface.h"
#include "../../KeePassLibCpp/PwManager.h"
#include "../../KeePassLibCpp/Util/AppUtil.h"
#include "../../KeePassLibCpp/Util/TranslateEx.h"

CPrivateConfigEx::CPrivateConfigEx(BOOL bRequireWriteAccess)
{
	m_bPreferUser = FALSE;

	this->GetConfigPaths(bRequireWriteAccess);

	TCHAR tszPref[SI_REGSIZE];
	if(this->GetIn(PWMKEY_PREFERUSER, tszPref, CFG_ID_GLOBAL) == TRUE)
		m_bPreferUser = ((_tcscmp(tszPref, CFG_VAL_TRUE) == 0) ? TRUE : FALSE);
}

CPrivateConfigEx::~CPrivateConfigEx()
{
}

void CPrivateConfigEx::GetConfigPaths(BOOL bRequireWriteAccess)
{
	TCHAR tszAppDir[MAX_PATH * 3];
	tszAppDir[0] = 0; tszAppDir[1] = 0;

	ASSERT(SI_REGSIZE >= _countof(tszAppDir));

	TCHAR tszTemp[SI_REGSIZE];
	VERIFY(GetApplicationDirectory(tszTemp, SI_REGSIZE - 1, TRUE, FALSE));
	m_strFileGlobal = tszTemp;
	m_strFileGlobal += _T("\\");
	m_strFileGlobal += PWM_EXENAME;

	m_strFileEnforced = m_strFileGlobal;
	m_strFileEnforced += CFG_SUFFIX_ENF;

	m_strFileGlobal += CFG_SUFFIX_STD;

	tszTemp[0] = 0;
	HINSTANCE hShell32 = LoadLibrary(_T("Shell32"));
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
		if(lpGet != NULL) lpGet(NULL, tszTemp, CSIDL_APPDATA, TRUE);

		lpGet = NULL; FreeLibrary(hShell32); hShell32 = NULL;
	} else { ASSERT(FALSE); }

	if(tszTemp[0] == 0) // GetSpecialFolderPath failed
	{
		m_strFileUser = PWM_EXENAME;
		m_strFileUser += CFG_SUFFIX_STD;
	}
	else
	{
		const DWORD uLen = (DWORD)_tcslen(tszTemp); ASSERT(uLen != 0);

		if(tszTemp[uLen - 1] != _T('\\'))
			_tcscat_s(tszTemp, _countof(tszTemp), _T("\\"));

		_tcscat_s(tszTemp, _countof(tszTemp), PWM_EXENAME);

		_tcscpy_s(tszAppDir, _countof(tszAppDir), tszTemp);

		m_strFileUser = tszTemp;
		m_strFileUser += _T("\\");
		m_strFileUser += PWM_EXENAME;
		m_strFileUser += CFG_SUFFIX_STD;
	}

	if(bRequireWriteAccess == TRUE)
	{
		if(_FileWritable(m_strFileGlobal.c_str()) == FALSE)
		{
			if(tszAppDir[0] != 0) CreateDirectory(tszAppDir, NULL);
		}
	}
}

BOOL CPrivateConfigEx::Get(LPCTSTR pszField, LPTSTR pszValue)
{
	if(this->GetIn(pszField, pszValue, CFG_ID_ENFORCED) == TRUE)
		return TRUE;

	TCHAR tszGlobal[SI_REGSIZE];
	BOOL bGlobal = this->GetIn(pszField, tszGlobal, CFG_ID_GLOBAL);

	TCHAR tszUser[SI_REGSIZE];
	BOOL bUser = this->GetIn(pszField, tszUser, CFG_ID_USER);

	if((bGlobal == FALSE) && (bUser == FALSE))
	{
		pszValue[0] = 0; pszValue[1] = 0;
		return FALSE;
	}
	else if((bGlobal == TRUE) && (bUser == FALSE))
	{
		_tcscpy_s(pszValue, SI_REGSIZE - 1, tszGlobal);
		return TRUE;
	}
	else if((bGlobal == FALSE) && (bUser == TRUE))
	{
		_tcscpy_s(pszValue, SI_REGSIZE - 1, tszUser);
		return TRUE;
	}

	// (bGlobal == TRUE) && (bUser == TRUE)
	if(m_bPreferUser == TRUE) _tcscpy_s(pszValue, SI_REGSIZE - 1, tszUser);
	else _tcscpy_s(pszValue, SI_REGSIZE - 1, tszGlobal);
	return TRUE;
}

BOOL CPrivateConfigEx::Set(LPCTSTR pszField, LPCTSTR pszValue)
{
	if(m_bPreferUser == TRUE)
	{
		if(this->SetIn(pszField, pszValue, CFG_ID_USER) == TRUE) return TRUE;
		if(this->SetIn(pszField, pszValue, CFG_ID_GLOBAL) == TRUE) return TRUE;
	}
	else // Don't prefer user -- use global first
	{
		if(this->SetIn(pszField, pszValue, CFG_ID_GLOBAL) == TRUE) return TRUE;
		if(this->SetIn(pszField, pszValue, CFG_ID_USER) == TRUE) return TRUE;
	}

	KP_Call(KPC_STATUSBARTEXT, (LPARAM)TRL("Failed to save the configuration."), 0, 0);
	return FALSE;
}

BOOL CPrivateConfigEx::GetIn(LPCTSTR pszField, LPTSTR pszValue, int nConfigID)
{
	LPCTSTR lpNotFound = PCFG_NOTFOUND;

	TCHAR tszTemp[SI_REGSIZE];
	tszTemp[0] = 0; tszTemp[1] = 0;

	ASSERT(pszValue != NULL); if(pszValue == NULL) return FALSE;
	pszValue[0] = 0; pszValue[1] = 0; // Reset string

	ASSERT(pszField != NULL); if(pszField == NULL) return FALSE;
	ASSERT(pszField[0] != 0); if(pszField[0] == 0) return FALSE;

	LPCTSTR lpFile = m_strFileUser.c_str();
	if(nConfigID == CFG_ID_GLOBAL) lpFile = m_strFileGlobal.c_str();
	else if(nConfigID == CFG_ID_ENFORCED) lpFile = m_strFileEnforced.c_str();

	GetPrivateProfileString(PWM_EXENAME, pszField, lpNotFound, tszTemp,
		SI_REGSIZE - 1, lpFile);
	if(_tcscmp(tszTemp, lpNotFound) == 0) return FALSE;

	_tcscpy_s(pszValue, SI_REGSIZE - 1, tszTemp);
	return TRUE;
}

BOOL CPrivateConfigEx::SetIn(LPCTSTR pszField, LPCTSTR pszValue, int nConfigID)
{
	ASSERT(pszField != NULL); if(pszField == NULL) return FALSE;
	ASSERT(_tcslen(pszField) > 0); if(_tcslen(pszField) == 0) return FALSE;
	ASSERT(pszValue != NULL); if(pszValue == NULL) return FALSE;

	LPCTSTR lpFile = m_strFileUser.c_str();
	if(nConfigID == CFG_ID_GLOBAL) lpFile = m_strFileGlobal.c_str();

	return ((WritePrivateProfileString(PWM_EXENAME, pszField, pszValue, lpFile) ==
		FALSE) ? FALSE : TRUE); // Zero-based success mapping
}

BOOL CPrivateConfigEx::GetBool(const TCHAR *pszField, BOOL bDefault)
{
	ASSERT((bDefault == FALSE) || (bDefault == TRUE));

	TCHAR tszTemp[SI_REGSIZE];
	if(this->Get(pszField, tszTemp) == FALSE) return bDefault;

	if(_tcsicmp(tszTemp, CFG_VAL_TRUE) == 0) return TRUE;
	else if(_tcsicmp(tszTemp, CFG_VAL_FALSE) == 0) return FALSE;

	return bDefault; // Parsing failed
}

BOOL CPrivateConfigEx::SetBool(const TCHAR *pszField, BOOL bValue)
{
	ASSERT((bValue == FALSE) || (bValue == TRUE));
	return this->Set(pszField, (bValue == FALSE) ? CFG_VAL_FALSE : CFG_VAL_TRUE);
}
