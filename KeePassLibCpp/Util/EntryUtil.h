/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2013 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___ENTRY_UTIL_H___
#define ___ENTRY_UTIL_H___

#pragma once

#include "../PwManager.h"
#include "StrUtil.h"

// LPTSTR PwEntryToString(const PW_ENTRY *lpEntry);
// BOOL StringToPwEntry(PW_ENTRY *pEntry, LPCTSTR lpEntryString);

// BOOL ReadPwStringItem(LPCTSTR lpEntryString, DWORD *pdwPos, CString *pStore);

// typedef int(*LPENTRYCOMPARE)(void *, const void *, const void *);

// typedef struct _CEE_CONTEXT
// {
//	DWORD dwSortByField;
//	LPCTSTRCMPEX lpCompare;
// } CEE_CONTEXT;

// int _CompareEntriesEx(void *pContext, const void *pEntryX, const void *pEntryY);

// #ifdef _MFC_VER
// CString PwEntryToString(PW_ENTRY *lpEntry, CPwManager* lpContext);
// bool StringToPwEntry(PW_ENTRY *lpEntry, LPCTSTR lpEntryString);
// #endif // _MFC_VER

class CEntryUtil : boost::noncopyable
{
private:
	CEntryUtil();

public:
	static std::basic_string<TCHAR> CreateSummaryList(CPwManager* pMgr,
		const std::vector<DWORD>& vEntryIndices);
};

#endif // ___ENTRY_UTIL_H___
