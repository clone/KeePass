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

#ifndef ___MEMORY_UTILITIES_H___
#define ___MEMORY_UTILITIES_H___

#pragma once

#include "../PwStructs.h"

// Securely erase memory
void mem_erase(unsigned char *p, size_t u);

// Time conversion functions
void _PackTimeToStruct(BYTE *pBytes, DWORD dwYear, DWORD dwMonth, DWORD dwDay, DWORD dwHour, DWORD dwMinute, DWORD dwSecond);
void _UnpackStructToTime(const BYTE *pBytes, DWORD *pdwYear, DWORD *pdwMonth, DWORD *pdwDay, DWORD *pdwHour, DWORD *pdwMinute, DWORD *pdwSecond);

// Getting the time
void _GetCurrentPwTime(PW_TIME *p);

// Compare two PW_TIME structures, returns -1 if pt1<pt2, returns 1 if pt1>pt2,
// returns 0 if pt1=pt2
int _pwtimecmp(const PW_TIME *pt1, const PW_TIME *pt2);

// Fast arithmetic time addition, possibly incorrect calendar-day
void _pwtimeadd(PW_TIME *pTime, const PW_TIME *pTimeAdd);

// Packs an array of integers to a TCHAR string
void ar2str(TCHAR *tszString, INT *pArray, INT nItemCount);

// Unpacks a TCHAR string to an array of integers
void str2ar(TCHAR *tszString, INT *pArray, INT nItemCount);

// Hash a file
BOOL SHA256_HashFile(LPCTSTR lpFile, BYTE *pHash);

#endif
