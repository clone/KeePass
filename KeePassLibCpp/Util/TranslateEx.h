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

#ifndef ___TRANSLATE_EX_H___
#define ___TRANSLATE_EX_H___

#include "../SysDefEx.h"

#define MAX_TRANSLATION_STRINGS 564

// Translate a string
#ifndef _UNICODE
#define TRL(sptr) _TRL(sptr)
#else
#define TRL(sptr) _TRL(_T(sptr))
#endif

#define TRL_VAR(stptr) _TRL(stptr)

#define TRL_MODE_DEF FALSE
#define TRL_MODE_TRL TRUE

C_FN_SHARE BOOL LoadTranslationTable(LPCTSTR pszTableName);
C_FN_SHARE BOOL FreeCurrentTranslationTable();

C_FN_SHARE LPCTSTR _TRL(LPCTSTR pszDefString);

C_FN_SHARE LPCTSTR GetCurrentTranslationTable();

#endif // ___TRANSLATE_EX_H___
