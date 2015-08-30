/*
  Copyright (c) 2003, Dominik Reichl <dominik.reichl@t-online.de>
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
#include "NewRandom.h"
#include "MemUtil.h"

CNewRandom::CNewRandom()
{
	Reset();
}

CNewRandom::~CNewRandom()
{
	Reset();
}

void CNewRandom::Reset()
{
	mem_erase(m_pPseudoRandom, INTRAND_SIZE);
	m_dwCounter = 0;
}

void CNewRandom::Initialize()
{
	DWORD inx;

	WORD ww;
	DWORD dw;
	LARGE_INTEGER li;
	SYSTEMTIME st;
	POINT pt;

	Reset();

	inx = 0;

	dw = GetTickCount();
	memcpy(&m_pPseudoRandom[inx], (BYTE *)&dw, 4); inx += 4;

	QueryPerformanceCounter(&li);
	memcpy(&m_pPseudoRandom[inx], &li, 8); inx += 8;

	GetLocalTime(&st);
	memcpy(&m_pPseudoRandom[inx], &st, 16); inx += 16;

	GetCursorPos(&pt);
	memcpy(&m_pPseudoRandom[inx], &pt, 8); inx += 8;

	SystemParametersInfo(SPI_GETSCREENSAVETIMEOUT, 0, &dw, 0);
	memcpy(&m_pPseudoRandom[inx], &dw, 4); inx += 4;

	ww = (WORD)(rand());
	memcpy(&m_pPseudoRandom[inx], &ww, 2); inx += 2;
	ww = (WORD)(rand());
	memcpy(&m_pPseudoRandom[inx], &ww, 2); inx += 2;
	ww = (WORD)(rand());
	memcpy(&m_pPseudoRandom[inx], &ww, 2); inx += 2;

	// You can add some random sources here

	ASSERT(inx <= INTRAND_SIZE);
}

void CNewRandom::GetRandomBuffer(BYTE *pBuf, DWORD dwSize)
{
	sha256_ctx hashctx;
	BYTE aTemp[32];
	DWORD dw;

	ASSERT(pBuf != NULL);

	while(dwSize != 0)
	{
		m_dwCounter++;
		sha256_begin(&hashctx);
		sha256_hash(m_pPseudoRandom, INTRAND_SIZE, &hashctx);
		sha256_hash((BYTE *)&m_dwCounter, 4, &hashctx);
		sha256_end(aTemp, &hashctx);

		dw = (dwSize < 32) ? dwSize : 32;
		memcpy(pBuf, aTemp, dw);
		pBuf += dw;
		dwSize -= dw;
	}
}

// Fast XorShift random number generator
unsigned long randXorShift()
{
	static unsigned long w = 88675123;
	static unsigned long x = 362436069;
	static unsigned long y = 2463534242;
	static unsigned long z = 521288629;
	unsigned long tmp;

	tmp = (x ^ (x << 15));
	x = y; y = z; z = w;
	w = (w ^ (w >> 21)) ^ (tmp ^ (tmp >> 4));

	return w;
}
