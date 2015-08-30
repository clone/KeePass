/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2014 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___POPULAR_PASSWORDS_H___
#define ___POPULAR_PASSWORDS_H___

#pragma once

#include "../SysDefEx.h"
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <tchar.h>
#include <vector>
#include <set>
#include <string>
#include "StrUtil.h"
#include "MemUtil.h"

typedef std::basic_string<WCHAR> TppWord;
typedef std::set<TppWord> TppDict;
typedef boost::shared_ptr<TppDict> TppDictPtr;

class CPopularPasswords : boost::noncopyable
{
private:
	CPopularPasswords();

public:
	static size_t GetMaxLength();
	static bool ContainsLength(size_t uLen);

	static bool IsPopular(LPCWSTR lpw, size_t* pdwDictSize);

	static void Add(const UTF8_BYTE* pTextUTF8);
	static void AddResUTF8(LPCTSTR lpResName, LPCTSTR lpResType);

private:
	static std::vector<TppDictPtr> m_vDicts;
};

#endif // ___POPULAR_PASSWORDS_H___
