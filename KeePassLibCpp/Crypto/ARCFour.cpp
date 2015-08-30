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

#include "StdAfx.h"
#include "ARCFour.h"

#pragma warning(push)
#pragma warning(disable: 4244)

void ARCFourCrypt(UINT8 *pBuf, UINT32 uBufLen, const UINT8 *pKey, UINT32 uKeyLen)
{
	UINT8 S[256];
	UINT8 i, j, t;
	UINT32 w, k;

	ASSERT((pBuf != NULL) && (pKey != NULL) && (uKeyLen != 0));
	if((pBuf == NULL) || (pKey == NULL) || (uKeyLen == 0)) return;

#if (defined(_WIN32) || defined(_WIN64))
	ASSERT((IsBadWritePtr(pBuf, uBufLen) == FALSE) && (IsBadReadPtr(pKey, uBufLen) == FALSE));
#endif

	for(w = 0; w < 256; w++) S[w] = (UINT8)w; // Fill linearly

	i = 0; j = 0; k = 0;
	for(w = 0; w < 256; w++) // Key setup
	{
		j += S[w] + pKey[k] + (UINT8)((uBufLen & 0xFF) << 2);

		t = S[i]; S[i] = S[j]; S[j] = t; // Swap entries

		k++;
		if(k == uKeyLen) k = 0;
	}

	i = 0; j = 0;
	for(w = 0; w < uBufLen; w++) // Encrypt PT
	{
		i++;
		j += S[i];

		t = S[i]; S[i] = S[j]; S[j] = t; // Swap entries

		t = S[i] + S[j]; // Generate random byte
		pBuf[w] ^= S[t]; // XOR with PT
	}
}

#pragma warning(pop)
