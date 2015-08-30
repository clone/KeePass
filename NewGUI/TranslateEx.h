/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2005 Dominik Reichl <dominik.reichl@t-online.de>

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

#define MAX_TRANSLATION_STRINGS 490

// Translate string, sptr must be a char*, _not_ a TCHAR* !
#define TRL(sptr) _TRL(sptr)

#define TRL_MODE_DEF FALSE
#define TRL_MODE_TRL TRUE

C_FN_SHARE BOOL LoadTranslationTable(const char *pszTableName);
C_FN_SHARE BOOL FreeCurrentTranslationTable();

C_FN_SHARE const char *_TRL(const char *pszDefString);

C_FN_SHARE const TCHAR *GetCurrentTranslationTable();

#endif // ___TRANSLATE_EX_H___
