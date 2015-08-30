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

#ifndef ___PW_COMPAT_H___
#define ___PW_COMPAT_H___

#include "SysDefEx.h"
#include "Util/StrUtil.h"

#pragma pack(1)

typedef struct _PW_DBHEADER_V1 // Old version 0.1.x database header
{
	DWORD dwSignature1;
	DWORD dwSignature2;
	DWORD dwFlags;
	DWORD dwVersion;

	BYTE aMasterSeed[16];
	UINT8 aEncryptionIV[16];

	DWORD dwGroups;
	DWORD dwEntries;
} PW_DBHEADER_V1, *PPW_DBHEADER_V1;

typedef struct _PW_DBHEADER_V2 // Old version 0.2.x database header
{
	DWORD dwSignature1;
	DWORD dwSignature2;
	DWORD dwFlags;
	DWORD dwVersion;

	BYTE aMasterSeed[16];
	UINT8 aEncryptionIV[16];

	DWORD dwGroups;
	DWORD dwEntries;

	BYTE aContentsHash[32];
} PW_DBHEADER_V2, *PPW_DBHEADER_V2;

#pragma pack()

TCHAR *_UTF8ToStringV2(const UTF8_BYTE *pUTF8String); // Don't use outside PwCompat.*

#endif // ___PW_COMPAT_H___
