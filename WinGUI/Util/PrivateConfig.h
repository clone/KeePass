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

// CONFIGURATION FILE MANAGER
// Created: Dominik Reichl, 2003

#ifndef ___PRIVATE_CONFIG_H___
#define ___PRIVATE_CONFIG_H___

#include "../../KeePassLibCpp/SysDefEx.h"

#define SI_REGSIZE 2048

// A GUID used to detect non-existing keys in queries
#define PCFG_NOTFOUND _T("2C3317110FC211DA94B900E08161165F")

// WINSHELLAPI (DECLSPEC_IMPORT?)
typedef HRESULT(WINAPI *LPSHGETSPECIALFOLDERPATH)(HWND hwndOwner, LPTSTR lpszPath, int nFolder, BOOL fCreate);

class CPP_CLASS_SHARE CPrivateConfig
{
public:
	CPrivateConfig(BOOL bRequireWriteAccess);
	virtual ~CPrivateConfig();

	BOOL Set(LPCTSTR pszField, LPCTSTR pszValue);
	BOOL Get(LPCTSTR pszField, LPTSTR pszValue);

	BOOL SetBool(const TCHAR *pszField, BOOL bValue);
	BOOL GetBool(const TCHAR *pszField, BOOL bDefault);

private:
	int m_nUseDir;
	TCHAR m_szFileLocal[MAX_PATH * 2];
	TCHAR m_szFileUser[MAX_PATH * 2];
	TCHAR m_szFileGeneric[MAX_PATH];
};

#endif // ___PRIVATE_CONFIG_H___
