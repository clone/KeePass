/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2006 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___PW_UTIL_H___
#define ___PW_UTIL_H___

#include "../SysDefEx.h"

// Very simple password quality estimation function
C_FN_SHARE DWORD EstimatePasswordBits(LPCTSTR pszPassword);

C_FN_SHARE BOOL LoadHexKey32(FILE *fp, BYTE *pBuf);
C_FN_SHARE BOOL SaveHexKey32(FILE *fp, BYTE *pBuf);

CPP_FN_SHARE CString PWM_FormatStaticError(int nErrorCode, DWORD dwFlags);

#endif // ___PW_UTIL_H___
