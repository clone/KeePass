/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2011 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___TRANSLATE_EX_H___
#define ___TRANSLATE_EX_H___

#pragma once

#include "../SysDefEx.h"

// Translate a string (character sequence in code)
#ifndef TRL
#define TRL(w__sptr_) _TRL(_T(w__sptr_))
#else
#error TRL is defined already!
#endif

// Translate a variable (do not apply _T macro)
#ifndef TRL_VAR
#define TRL_VAR(w__stptr_) _TRL(w__stptr_)
#else
#error TRL_VAR is defined already!
#endif

#define TRL_MODE_DEF FALSE
#define TRL_MODE_TRL TRUE

BOOL LoadTranslationTable(LPCTSTR pszTableName);
BOOL FreeCurrentTranslationTable();

void _SortTrlTable();

LPCTSTR _TRL(LPCTSTR pszDefString);

LPCTSTR GetCurrentTranslationTable();

#endif // ___TRANSLATE_EX_H___
