/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2012 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___KEYSOURCESPOOL_H___
#define ___KEYSOURCESPOOL_H___

#pragma once

#include "../../KeePassLibCpp/SysDefEx.h"
#include <utility>
#include <tchar.h>
#include "PrivateConfigEx.h"

#define CKSP_MAX_ITEMS 16

typedef std::basic_string<TCHAR> std_string;

typedef std::pair<std_string, std_string> KspKvp;
typedef std::vector<KspKvp> KspVec;

class CKeySourcesPool : boost::noncopyable
{
private:
	CKeySourcesPool();

public:
	static BOOL GetEnabled();
	static void SetEnabled(BOOL bEnabled);

	static bool Load(const CPrivateConfigEx* pSource);
	static bool Save(CPrivateConfigEx* pOut);

	static std_string Get(LPCTSTR lpDatabasePath);
	static void Set(LPCTSTR lpDatabasePath, LPCTSTR lpKeyFilePath);

private:
	// static std_string _PathToID(LPCTSTR lpDatabasePath);
	static void _SetAndInc(CPrivateConfigEx* pOut, LPCTSTR lpKey,
		LPCTSTR lpValue, DWORD* pdwIndex);
	static void _ClearAllConfigItems(CPrivateConfigEx* pOut);
	static void _UpdateItem(KspVec::iterator& it);

	static BOOL m_bEnabled;
	static bool m_bEnabledAtLoad;
	static KspVec m_vec;
};

#endif // ___KEYSOURCESPOOL_H___
