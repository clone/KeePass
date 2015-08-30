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

#ifndef ___PRIVATE_CONFIG_EX_H___
#define ___PRIVATE_CONFIG_EX_H___

#pragma once

#include "../../KeePassLibCpp/SysDefEx.h"
#include "../../KeePassLibCpp/SDK/Details/KpDefs.h"

#include <string>
#include <vector>
#include <boost/utility.hpp>

#define CFG_VAL_TRUE      _T("True")
#define CFG_VAL_FALSE     _T("False")

#define CFG_ID_ENFORCED 0
#define CFG_ID_GLOBAL   1
#define CFG_ID_USER     2
#define CFG_ID_COUNT    3

#define CFG_SUFFIX_STD _T(".ini")
#define CFG_SUFFIX_ENF _T(".enforced.ini")

class CPrivateConfigEx : boost::noncopyable
{
public:
	CPrivateConfigEx(BOOL bRequireWriteAccess);
	virtual ~CPrivateConfigEx();

	BOOL Get(LPCTSTR pszField, LPTSTR pszValue) const;
	BOOL Set(LPCTSTR pszField, LPCTSTR pszValue);

	BOOL GetBool(const TCHAR *pszField, BOOL bDefault) const;
	BOOL SetBool(const TCHAR *pszField, BOOL bValue);

	BOOL GetEnforcedBool(LPCTSTR pszField, BOOL bDefault, BOOL bAllowGlobal) const;

	std::vector<std::basic_string<TCHAR> > GetArray(LPCTSTR pszPrefix) const;

	std::basic_string<TCHAR> GetSafe(const TCHAR *pszField) const;

	// Get the KeePass application data path (directory, not INI
	// file), without a terminating separator
	std::basic_string<TCHAR> GetUserPath() const { return m_strUserPath; }

private:
	void GetConfigPaths();
	void FlushGlobal(BOOL bDeleteCache);
	void PrepareUserWrite();

	BOOL GetIn(LPCTSTR pszField, LPTSTR pszValue, int nConfigID) const;
	BOOL SetIn(LPCTSTR pszField, LPCTSTR pszValue, int nConfigID);

	static void FlushIni(LPCTSTR lpIniFilePath);

	static BOOL m_bInstanceActive;

	BOOL m_bCanWrite;

	std::basic_string<TCHAR> m_strFileEnforced;
	std::basic_string<TCHAR> m_strFileGlobal;
	std::basic_string<TCHAR> m_strFileUser;

	std::basic_string<TCHAR> m_strUserPath;
	BOOL m_bTriedToCreateUserPath;

	BOOL m_bPreferUser;

	std::basic_string<TCHAR> m_strFileCachedGlobal;
};

#endif // ___PRIVATE_CONFIG_EX_H___
