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

#include "StdAfx.h"
#include "Base64.h"
#include "StrUtil.h"
#include "MemUtil.h"
#include "../SDK/Details/KpDefs.h"

static const char* g_pCodes =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const unsigned char g_pMap[256] =
{
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,
	 52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,
	255, 254, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,
	  7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,
	 19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,
	255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,
	 37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
	 49,  50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255
};

CBase64Codec::CBase64Codec()
{
}

bool CBase64Codec::Encode(const BYTE *pIn, DWORD uInLen, BYTE *pOut, DWORD *uOutLen)
{
	ASSERT((pIn != NULL) && (uInLen != 0) && (pOut != NULL) && (uOutLen != NULL));

	const DWORD len2 = (((uInLen + 2) / 3) << 2);
	if((*uOutLen) < (len2 + 1)) return false;

	BYTE *p = pOut;
	const DWORD leven = (3 * (uInLen / 3));

	DWORD i;
	for(i = 0; i < leven; i += 3)
	{
		*p++ = g_pCodes[pIn[0] >> 2];
		*p++ = g_pCodes[((pIn[0] & 3) << 4) + (pIn[1] >> 4)];
		*p++ = g_pCodes[((pIn[1] & 0xf) << 2) + (pIn[2] >> 6)];
		*p++ = g_pCodes[pIn[2] & 0x3f];
		pIn += 3;
	}

	if(i < uInLen)
	{
		const DWORD a = pIn[0];
		const DWORD b = (((i + 1) < uInLen) ? pIn[1] : 0);
		const DWORD c = 0;

		*p++ = g_pCodes[a >> 2];
		*p++ = g_pCodes[((a & 3) << 4) + (b >> 4)];
		*p++ = (((i + 1) < uInLen) ? g_pCodes[((b & 0xf) << 2) + (c >> 6)] : '=');
		*p++ = '=';
	}

	*p = 0; // Append NULL byte
	*uOutLen = static_cast<DWORD>(p - pOut);
	return true;
}

bool CBase64Codec::Decode(const BYTE *pIn, DWORD uInLen, BYTE *pOut, DWORD *uOutLen)
{
	DWORD g = 3;

	ASSERT((pIn != NULL) && (uInLen != 0) && (pOut != NULL) && (uOutLen != NULL));

	DWORD t, x, y, z;
	for(x = y = z = t = 0; x < uInLen; ++x)
	{
		BYTE c = g_pMap[pIn[x]];
		if(c == 255) continue;
		if(c == 254) { c = 0; g--; }

		t = (t << 6) | c;

		if(++y == 4)
		{
			if((z + g) > *uOutLen) { return false; } // Buffer overflow
			pOut[z++] = (BYTE)((t>>16)&255);
			if(g > 1) pOut[z++] = (BYTE)((t>>8)&255);
			if(g > 2) pOut[z++] = (BYTE)(t&255);
			y = t = 0;
		}
	}

	*uOutLen = z;
	return true;
}

bool CBase64Codec::DecodeUrlA(LPCSTR lpBase64, std::vector<BYTE>& rStorage)
{
	ASSERT(lpBase64 != NULL); if(lpBase64 == NULL) return false;
	ASSERT(strlen(CB64_PROTOCOL) == CB64_PROTOCOL_LEN);

	const DWORD dwSrcLen = static_cast<DWORD>(strlen(lpBase64));
	if(dwSrcLen <= CB64_PROTOCOL_LEN) return false;

	if(strncmp(lpBase64, CB64_PROTOCOL, CB64_PROTOCOL_LEN) != 0) return false;

	DWORD dwBufLen = dwSrcLen + 32;
	BYTE *pb = new BYTE[dwBufLen];
	if(pb == NULL) return false;

	CBase64Codec::Decode((const BYTE *)&lpBase64[CB64_PROTOCOL_LEN],
		dwSrcLen - CB64_PROTOCOL_LEN, pb, &dwBufLen);

	rStorage.resize(dwBufLen);
	memcpy(&rStorage[0], pb, dwBufLen);

	delete []pb; pb = NULL;
	return true;
}

bool CBase64Codec::DecodeUrlT(LPCTSTR lpBase64, std::vector<BYTE>& rStorage)
{
#ifndef _UNICODE
	return CBase64Codec::DecodeUrlA(lpBase64, rStorage);
#else // Unicode
	LPCSTR p = _StringToAnsi(lpBase64);
	const bool bRes = CBase64Codec::DecodeUrlA(p, rStorage);
	SAFE_DELETE_ARRAY(p);
	return bRes;
#endif
}

bool CBase64Codec::IsBase64UrlStringT(LPCTSTR lpString)
{
	ASSERT(lpString != NULL); if(lpString == NULL) return false;

	if(_tcslen(lpString) <= CB64_PROTOCOL_LEN) return false;
	if(_tcsncmp(lpString, _T(CB64_PROTOCOL), CB64_PROTOCOL_LEN) != 0) return false;

	return true;
}
