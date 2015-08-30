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

#ifndef ___PRIVATE_CONFIG_EX_H___
#define ___PRIVATE_CONFIG_EX_H___

#include "../../KeePassLibCpp/SysDefEx.h"
#include "PrivateConfig.h" // Definitions

#include <string>

#define CFG_VAL_TRUE      _T("True")
#define CFG_VAL_FALSE     _T("False")

#define CFG_ID_ENFORCED 0
#define CFG_ID_GLOBAL   1
#define CFG_ID_USER     2
#define CFG_ID_COUNT    3

#define CFG_SUFFIX_STD _T(".ini")
#define CFG_SUFFIX_ENF _T(".enforced.ini")

class CPrivateConfigEx
{
public:
	CPrivateConfigEx(BOOL bRequireWriteAccess);
	virtual ~CPrivateConfigEx();

	BOOL Get(LPCTSTR pszField, LPTSTR pszValue);
	BOOL Set(LPCTSTR pszField, LPCTSTR pszValue);

	BOOL GetBool(const TCHAR *pszField, BOOL bDefault);
	BOOL SetBool(const TCHAR *pszField, BOOL bValue);

private:
	void GetConfigPaths(BOOL bRequireWriteAccess);
	BOOL GetIn(LPCTSTR pszField, LPTSTR pszValue, int nConfigID);
	BOOL SetIn(LPCTSTR pszField, LPCTSTR pszValue, int nConfigID);

	std::basic_string<TCHAR> m_strFileEnforced;
	std::basic_string<TCHAR> m_strFileGlobal;
	std::basic_string<TCHAR> m_strFileUser;

	BOOL m_bPreferUser;
};

#endif // ___PRIVATE_CONFIG_EX_H___
