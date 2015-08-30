/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2015 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "../Plugins/KpApiImpl.h"
#include "../Plugins/PluginMgr.h"
#include "../../KeePassLibCpp/PwManager.h"
#include "../../KeePassLibCpp/Util/AppUtil.h"
#include "../../KeePassLibCpp/Util/TranslateEx.h"

// A GUID used to detect non-existing keys in queries
#define PCFG_NOTFOUND _T("DD1B11FE241D475AA14A7555729D6BCB")

typedef HRESULT(WINAPI *LPSHGETSPECIALFOLDERPATH)(HWND hwndOwner,
	LPTSTR lpszPath, int nFolder, BOOL fCreate);

static std::basic_string<TCHAR> g_strFileOverrideGlobal;
static std::basic_string<TCHAR> g_strFileOverrideUser;

CPrivateConfigEx::CPrivateConfigEx(BOOL bRequireWriteAccess)
{
	m_bPreferUser = FALSE;
	m_bCanWrite = bRequireWriteAccess;
	m_bTriedToCreateUserPath = FALSE;

	this->GetConfigPaths();

	TCHAR tszPref[SI_REGSIZE];
	if(this->GetIn(PWMKEY_PREFERUSER, tszPref, CFG_ID_GLOBAL) == TRUE)
		m_bPreferUser = ((_tcscmp(tszPref, CFG_VAL_TRUE) == 0) ? TRUE : FALSE);
}

CPrivateConfigEx::~CPrivateConfigEx()
{
	this->FlushGlobal(TRUE);
}

void CPrivateConfigEx::GetConfigPaths()
{
	TCHAR tszAppDir[MAX_PATH * 3];
	tszAppDir[0] = 0; tszAppDir[1] = 0;

	ASSERT(SI_REGSIZE >= _countof(tszAppDir));

	TCHAR tszTemp[SI_REGSIZE];
	VERIFY(AU_GetApplicationDirectory(tszTemp, SI_REGSIZE - 1, TRUE, FALSE));
	m_strFileGlobal = tszTemp;
	m_strFileGlobal += _T("\\");
	m_strFileGlobal += PWM_EXENAME;

	m_strFileEnforced = m_strFileGlobal;
	m_strFileEnforced += CFG_SUFFIX_ENF;

	m_strFileGlobal += CFG_SUFFIX_STD;

	tszTemp[0] = 0;
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
		if(lpGet != NULL) lpGet(NULL, tszTemp, CSIDL_APPDATA, TRUE);

		lpGet = NULL; FreeLibrary(hShell32); hShell32 = NULL;
	} else { ASSERT(FALSE); }

	if(tszTemp[0] == 0) // GetSpecialFolderPath failed
	{
		m_strFileUser = PWM_EXENAME;
		m_strFileUser += CFG_SUFFIX_STD;

		m_strUserPath.clear();
	}
	else
	{
		const DWORD uLen = (DWORD)_tcslen(tszTemp); ASSERT(uLen != 0);

		if(tszTemp[uLen - 1] != _T('\\'))
			_tcscat_s(tszTemp, _countof(tszTemp), _T("\\"));

		_tcscat_s(tszTemp, _countof(tszTemp), PWM_EXENAME);

		_tcscpy_s(tszAppDir, _countof(tszAppDir), tszTemp);

		m_strFileUser = tszTemp;
		m_strUserPath = tszTemp;

		m_strFileUser += _T("\\");
		m_strFileUser += PWM_EXENAME;
		m_strFileUser += CFG_SUFFIX_STD;
	}

	ApplyFileOverrides();

	if(_FileAccessible(m_strFileGlobal.c_str()) == TRUE)
	{
		m_strFileCachedGlobal = WU_GetTempFile(CFG_SUFFIX_STD);

		if(CopyFile(m_strFileGlobal.c_str(), m_strFileCachedGlobal.c_str(),
			FALSE) == FALSE)
		{
			ASSERT(FALSE);
			m_strFileCachedGlobal.clear();
		}
	}
}

void CPrivateConfigEx::ApplyFileOverrides()
{
	if(g_strFileOverrideGlobal.size() > 0) m_strFileGlobal = g_strFileOverrideGlobal;
	if(g_strFileOverrideUser.size() > 0) m_strFileUser = g_strFileOverrideUser;
}

void CPrivateConfigEx::FlushGlobal(BOOL bDeleteCache)
{
	if(m_strFileCachedGlobal.size() == 0) return;

	if(m_bCanWrite == TRUE)
	{
		CPrivateConfigEx::FlushIni(m_strFileCachedGlobal.c_str());

		const BOOL bCopyResult = CopyFile(m_strFileCachedGlobal.c_str(),
			m_strFileGlobal.c_str(), FALSE);

		if(bCopyResult != FALSE)
			_CallPlugins(KPM_OPTIONS_SAVE_GLOBAL, (LPARAM)m_strFileGlobal.c_str(), 0);
		else { ASSERT(FALSE); }
	}

	if(bDeleteCache == TRUE)
	{
		if(DeleteFile(m_strFileCachedGlobal.c_str()) != FALSE)
			m_strFileCachedGlobal.clear();
		else { ASSERT(FALSE); }
	}
}

void CPrivateConfigEx::FlushIni(LPCTSTR lpIniFilePath)
{
	ASSERT(lpIniFilePath != NULL); if(lpIniFilePath == NULL) return;
	ASSERT(lpIniFilePath[0] != 0); if(lpIniFilePath[0] == 0) return;

	// Do not verify the following call, as it always returns FALSE
	// on Windows 95 / 98 / ME
	WritePrivateProfileString(NULL, NULL, NULL, lpIniFilePath);
}

void CPrivateConfigEx::PrepareUserWrite()
{
	ASSERT(m_bCanWrite == TRUE);

	if((m_bPreferUser == TRUE) || (_FileWritable(m_strFileGlobal.c_str()) == FALSE))
	{
		if(m_strUserPath.size() > 0)
		{
			if(m_bTriedToCreateUserPath == FALSE)
			{
				CreateDirectory(m_strUserPath.c_str(), NULL); // Ensure existance
				m_bTriedToCreateUserPath = TRUE; // Try once only
			}
		}
	}
}

BOOL CPrivateConfigEx::Get(LPCTSTR pszField, LPTSTR pszValue) const
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
	// pszValue may be NULL -- this deletes the item

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

	CKpApiImpl::Instance().SetStatusBarText(TRL("Failed to save the configuration."));
	return FALSE;
}

BOOL CPrivateConfigEx::GetIn(LPCTSTR pszField, LPTSTR pszValue, int nConfigID) const
{
	LPCTSTR lpNotFound = PCFG_NOTFOUND;

	TCHAR tszTemp[SI_REGSIZE];
	tszTemp[0] = 0; tszTemp[1] = 0;

	ASSERT(pszValue != NULL); if(pszValue == NULL) return FALSE;
	pszValue[0] = 0; pszValue[1] = 0; // Reset string

	ASSERT(pszField != NULL); if(pszField == NULL) return FALSE;
	ASSERT(pszField[0] != 0); if(pszField[0] == 0) return FALSE;

	LPCTSTR lpFile = m_strFileUser.c_str();
	if(nConfigID == CFG_ID_GLOBAL)
	{
		if(m_strFileCachedGlobal.size() != 0)
			lpFile = m_strFileCachedGlobal.c_str();
		else
			lpFile = m_strFileGlobal.c_str();
	}
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
	// pszValue may be NULL -- this deletes the item

	ASSERT(m_bCanWrite == TRUE);

	LPCTSTR lpFile = m_strFileUser.c_str();
	if(nConfigID == CFG_ID_GLOBAL)
	{
		if(m_strFileCachedGlobal.size() != 0)
			lpFile = m_strFileCachedGlobal.c_str();
		else
			lpFile = m_strFileGlobal.c_str();
	}
	else if(nConfigID == CFG_ID_USER)
		this->PrepareUserWrite();

	return ((WritePrivateProfileString(PWM_EXENAME, pszField, pszValue, lpFile) ==
		FALSE) ? FALSE : TRUE); // Zero-based success mapping
}

BOOL CPrivateConfigEx::GetBool(const TCHAR *pszField, BOOL bDefault) const
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

BOOL CPrivateConfigEx::GetEnforcedBool(LPCTSTR pszField, BOOL bDefault,
	BOOL bAllowGlobal) const
{
	TCHAR tszTemp[SI_REGSIZE];

	if(this->GetIn(pszField, tszTemp, CFG_ID_ENFORCED) == TRUE)
	{
		if(_tcsicmp(tszTemp, CFG_VAL_TRUE) == 0) return TRUE;
		else if(_tcsicmp(tszTemp, CFG_VAL_FALSE) == 0) return FALSE;
		return bDefault;
	}

	if(bAllowGlobal == TRUE)
	{
		if(this->GetIn(pszField, tszTemp, CFG_ID_GLOBAL) == TRUE)
		{
			if(_tcsicmp(tszTemp, CFG_VAL_TRUE) == 0) return TRUE;
			else if(_tcsicmp(tszTemp, CFG_VAL_FALSE) == 0) return FALSE;
		}
	}

	return bDefault;
}

std::vector<std::basic_string<TCHAR> > CPrivateConfigEx::GetArray(
	LPCTSTR pszPrefix) const
{
	std::vector<std::basic_string<TCHAR> > v;

	ASSERT(pszPrefix != NULL); if(pszPrefix == NULL) return v;
	ASSERT(pszPrefix[0] != 0); if(pszPrefix[0] == 0) return v;

	DWORD uIndex = 0;
	TCHAR tszValue[SI_REGSIZE];

	while(true)
	{
		CString strKey;
		strKey.Format(_T("%s%u"), pszPrefix, uIndex);

		if(this->Get(strKey, tszValue) == FALSE) break;

		std::basic_string<TCHAR> strValue = tszValue;
		v.push_back(strValue);

		++uIndex;
	}

	return v;
}

std::basic_string<TCHAR> CPrivateConfigEx::GetSafe(const TCHAR *pszField) const
{
	TCHAR tszTemp[SI_REGSIZE];

	if(this->Get(pszField, tszTemp) == FALSE)
		return std::basic_string<TCHAR>();

	return std::basic_string<TCHAR>(tszTemp);
}

void CPrivateConfigEx::LoadStaticConfigFileOverrides(bool bPreserveExisting)
{
	std::basic_string<TCHAR> str;

	str = this->GetSafe(PWMKEY_CFGOVERRIDE_GLOBAL);
	if((g_strFileOverrideGlobal.size() == 0) || !bPreserveExisting)
		g_strFileOverrideGlobal = str;

	str = this->GetSafe(PWMKEY_CFGOVERRIDE_USER);
	if((g_strFileOverrideUser.size() == 0) || !bPreserveExisting)
		g_strFileOverrideUser = str;

	ApplyFileOverrides();
}

void CPrivateConfigEx::SetConfigFileOverride(int nConfigID, LPCTSTR lpPath)
{
	if(lpPath == NULL) { ASSERT(FALSE); return; }

	if(nConfigID == CFG_ID_GLOBAL)
		g_strFileOverrideGlobal = lpPath;
	else if(nConfigID == CFG_ID_USER)
		g_strFileOverrideUser = lpPath;
	else { ASSERT(FALSE); }
}
