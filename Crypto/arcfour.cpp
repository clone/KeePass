/*
  Copyright (c) 2003/2004, Dominik Reichl <dominik.reichl@t-online.de>
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer. 
  - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  - Neither the name of ReichlSoft nor the names of its contributors may be
    used to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#include "StdAfx.h"
#include "arcfour.h"

void arcfourCrypt(BYTE *pBuf, unsigned long uBufLen, BYTE *pKey, unsigned long uKeyLen)
{
	BYTE S[256];
	BYTE i, j;
	BYTE t;
	DWORD w, k;

	ASSERT((sizeof(BYTE) == 1) && (pBuf != NULL) && (pKey != NULL) && (uKeyLen != 0));
	ASSERT((IsBadWritePtr(pBuf, 1) == FALSE) && (IsBadReadPtr(pKey, 1) == FALSE));

	for(w = 0; w < 256; w++) S[w] = (BYTE)w; // Fill linearly

	i = 0; j = 0; k = 0;
	for(w = 0; w < 256; w++) // Key setup
	{
#pragma warning(push)
#pragma warning(disable: 4244)
		j += S[w] + (BYTE)(pKey[k] + (BYTE)((uBufLen & 0xFF) << 2));
#pragma warning(pop)

		t = S[i];
		S[i] = S[j];
		S[j] = t;

		k++;
		if(k == uKeyLen) k = 0;
	}

	i = 0; j = 0;

	for(w = 0; w < uBufLen; w++) // Generate random bytes and XOR with PT
	{
		i++;
#pragma warning(push)
#pragma warning(disable: 4244)
		j += S[i];
#pragma warning(pop)

		t = S[i];
		S[i] = S[j];
		S[j] = t;

		t = (BYTE)(S[i] + S[j]);
		pBuf[w] ^= S[t];
	}
}
