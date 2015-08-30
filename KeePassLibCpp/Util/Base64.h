/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2010 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___BASE64_H___
#define ___BASE64_H___

#pragma once

#include "../SysDefEx.h"
#include <vector>

class CBase64Codec
{
private:
	CBase64Codec();

public:
	static bool Encode(const BYTE *pIn, DWORD uInLen, BYTE *pOut, DWORD *uOutLen);
	static bool Decode(const BYTE *pIn, DWORD uInLen, BYTE *pOut, DWORD *uOutLen);

	static bool DecodeUrlT(LPCTSTR lpBase64, std::vector<BYTE>& rStorage);

	static bool IsBase64UrlStringT(LPCTSTR lpString);

private:
	static bool DecodeUrlA(LPCSTR lpBase64, std::vector<BYTE>& rStorage);
};

#endif // ___BASE64_H___
