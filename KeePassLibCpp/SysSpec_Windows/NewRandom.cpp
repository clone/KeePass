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

#if (defined(_WIN32) || defined(_WIN64))

#include <wincrypt.h>
#include <objbase.h>
#include <mmsystem.h>

#include "../Util/NewRandom.h"
#include "../Util/MemUtil.h"

static DWORD g_dwNewRandomInstanceCounter = 0;

static unsigned long g_xorW = 0;
static unsigned long g_xorX = 0;
static unsigned long g_xorY = 0;
static unsigned long g_xorZ = 0;

CNewRandom::CNewRandom()
{
	ASSERT(m_vPseudoRandom.size() == 0);

	m_dwCounter = 0;
}

CNewRandom::~CNewRandom()
{
	m_vPseudoRandom.clear();
	
	this->ClearUserEntropyPool();
}

void CNewRandom::Initialize()
{
	++g_dwNewRandomInstanceCounter;

	DWORD dw;
	dw = timeGetTime();
	AddRandomObject(&dw, 4);

	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	AddRandomObject(&li, sizeof(LARGE_INTEGER));

	SYSTEMTIME st;
	ZeroMemory(&st, sizeof(SYSTEMTIME));
	GetLocalTime(&st);
	AddRandomObject(&st, sizeof(SYSTEMTIME));

	POINT pt;
	GetCursorPos(&pt);
	AddRandomObject(&pt, sizeof(POINT));

	WORD ww;
	ww = (WORD)(rand());
	AddRandomObject(&ww, 2);
	ww = (WORD)(rand());
	AddRandomObject(&ww, 2);
	ww = (WORD)(rand());
	AddRandomObject(&ww, 2);

	GetCaretPos(&pt);
	AddRandomObject(&pt, sizeof(POINT));

	MEMORYSTATUS ms;
	GlobalMemoryStatus(&ms);
	AddRandomObject(&ms, sizeof(MEMORYSTATUS));

	dw = (DWORD)(UINT_PTR)GetActiveWindow();
	AddRandomObject(&dw, 4);

	dw = (DWORD)(UINT_PTR)GetCapture();
	AddRandomObject(&dw, 4);

	dw = (DWORD)(UINT_PTR)GetClipboardOwner();
	AddRandomObject(&dw, 4);

#ifndef _WIN32_WCE
	// No support under Windows CE
	dw = (DWORD)(UINT_PTR)GetClipboardViewer();
	AddRandomObject(&dw, 4);
#endif

	dw = GetCurrentProcessId();
	AddRandomObject(&dw, 4);

	dw = (DWORD)(UINT_PTR)GetCurrentProcess();
	AddRandomObject(&dw, 4);

	dw = (DWORD)(UINT_PTR)GetActiveWindow();
	AddRandomObject(&dw, 4);

	dw = GetCurrentThreadId();
	AddRandomObject(&dw, 4);

	dw = (DWORD)(UINT_PTR)GetCurrentThread();
	AddRandomObject(&dw, 4);

	dw = (DWORD)(UINT_PTR)GetDesktopWindow();
	AddRandomObject(&dw, 4);

	dw = (DWORD)(UINT_PTR)GetFocus();
	AddRandomObject(&dw, 4);

	dw = (DWORD)(UINT_PTR)GetForegroundWindow();
	AddRandomObject(&dw, 4);

#ifndef _WIN32_WCE
	dw = (DWORD)GetInputState();
	AddRandomObject(&dw, 4); 
#endif

	dw = GetMessagePos();
	AddRandomObject(&dw, 4);

#ifndef _WIN32_WCE
	dw = (DWORD)GetMessageTime();
	AddRandomObject(&dw, 4);
#endif

	dw = (DWORD)(UINT_PTR)GetOpenClipboardWindow();
	AddRandomObject(&dw, 4);

	dw = (DWORD)(UINT_PTR)GetProcessHeap();
	AddRandomObject(&dw, 4);

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	AddRandomObject(&si, sizeof(SYSTEM_INFO));

	dw = (DWORD)randXorShift();
	AddRandomObject(&dw, 4);

#ifndef _WIN32_WCE
	STARTUPINFO sui;
	GetStartupInfo(&sui);
	AddRandomObject(&sui, sizeof(STARTUPINFO));
#endif

	GUID guid;
	VERIFY(CoCreateGuid(&guid) == S_OK);
	AddRandomObject(&guid, sizeof(GUID));

	BYTE pbCrypt[64];
	CNewRandom::SysCryptGetRandom(&pbCrypt[0], 64);
	AddRandomObject(&pbCrypt[0], 64);

	AddRandomObject(&g_dwNewRandomInstanceCounter, 4);
}

void CNewRandom::AddRandomObject(__in_bcount(uSize) const void *pObj, size_t uSize)
{
	ASSERT(pObj != NULL); if(pObj == NULL) return;

	const BYTE *p = (const BYTE *)pObj;
	for(size_t i = 0; i < uSize; ++i)
	{
		m_vPseudoRandom.push_back(*p);
		++p;
	}
}

void CNewRandom::AddToUserEntropyPool(const BYTE *pData, DWORD dwSize)
{
	ASSERT(pData != NULL); if(pData == NULL) return;

	for(DWORD i = 0; i < dwSize; ++i)
		m_vUserRandom.push_back(pData[i]);
}

void CNewRandom::ClearUserEntropyPool()
{
	m_vUserRandom.clear();
}

void CNewRandom::GetRandomBuffer(__out_bcount(dwSize) BYTE *pBuf, DWORD dwSize)
{
	sha256_ctx hashctx;
	BYTE aTemp[32];
	DWORD dw;

	ASSERT(pBuf != NULL); if(pBuf == NULL) return;

	if(m_vPseudoRandom.size() == 0) this->Initialize();

	while(dwSize != 0)
	{
		++m_dwCounter;
		sha256_begin(&hashctx);

		if(m_vPseudoRandom.size() > 0)
			sha256_hash(&m_vPseudoRandom[0], (unsigned long)m_vPseudoRandom.size(),
				&hashctx);
		else { ASSERT(FALSE); }

		if(m_vUserRandom.size() > 0)
			sha256_hash(&m_vUserRandom[0], (unsigned long)m_vUserRandom.size(),
				&hashctx);

		sha256_hash((BYTE *)&m_dwCounter, 4, &hashctx);
		sha256_end(aTemp, &hashctx);

		dw = ((dwSize < 32) ? dwSize : 32);
		memcpy(pBuf, aTemp, dw);
		pBuf += dw;
		dwSize -= dw;
	}
}

void CNewRandom::SysCryptGetRandom(BYTE *pBuf, DWORD dwSize)
{
	HCRYPTPROV hContext = NULL;

	BOOL bReg = CryptAcquireContext(&hContext, NULL, NULL, PROV_RSA_FULL,
		CRYPT_VERIFYCONTEXT | CRYPT_SILENT);
	if((bReg == FALSE) || (hContext == NULL))
		bReg = CryptAcquireContext(&hContext, NULL, NULL, PROV_RSA_FULL,
			CRYPT_VERIFYCONTEXT); // Windows 98 does not support CRYPT_SILENT
	if((bReg == FALSE) || (hContext == NULL)) { ASSERT(FALSE); return; }

	VERIFY(CryptGenRandom(hContext, dwSize, pBuf));
	VERIFY(CryptReleaseContext(hContext, 0));
}

// Seed the xorshift random number generator
void srandXorShift(unsigned long *pSeed128)
{
#ifdef _DEBUG
	static BOOL _bOnceOnly = FALSE;
	ASSERT(_bOnceOnly == FALSE);
	_bOnceOnly = TRUE;
#endif

	ASSERT(pSeed128 != NULL); // No NULL parameter allowed
	if(pSeed128 == NULL) return;

	if((g_xorW == 0) && (g_xorX == 0) && (g_xorY == 0) && (g_xorZ == 0))
	{
		g_xorW = pSeed128[0];
		g_xorX = pSeed128[1];
		g_xorY = pSeed128[2];
		g_xorZ = pSeed128[3];

		if((g_xorW + g_xorX + g_xorY + g_xorZ) == 0) g_xorX += 0xB7E15163;
	}
}

// Fast XorShift random number generator
unsigned long randXorShift()
{
	unsigned long tmp;

	tmp = (g_xorX ^ (g_xorX << 15));
	g_xorX = g_xorY; g_xorY = g_xorZ; g_xorZ = g_xorW;
	g_xorW = (g_xorW ^ (g_xorW >> 21)) ^ (tmp ^ (tmp >> 4));

	return g_xorW;
}

void randCreateUUID(BYTE *pUUID16, CNewRandom *pRandomSource)
{
	BYTE *p = pUUID16;
	DWORD *pdw1 = (DWORD *)pUUID16, *pdw2 = (DWORD *)&pUUID16[4],
		*pdw3 = (DWORD *)&pUUID16[8], *pdw4 = (DWORD *)&pUUID16[12];
	DWORD dw1, dw2, dw3, dw4;

	ASSERT(pRandomSource != NULL); if(pRandomSource == NULL) return;

	ASSERT((sizeof(DWORD) == 4) && (sizeof(USHORT) == 2) && (pUUID16 != NULL));
	if(pUUID16 == NULL) return;

	SYSTEMTIME st;
	ZeroMemory(&st, sizeof(SYSTEMTIME));
	GetSystemTime(&st);

	_PackTimeToStruct(p, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	p += 5; // +5 => 5 bytes filled
	*p = (BYTE)((st.wMilliseconds >> 2) & 0xFF); // Store milliseconds
	++p; // +1 => 6 bytes filled

	// Use the xorshift random number generator as pseudo-counter
	DWORD dwPseudoCounter = randXorShift();
	memcpy(p, &dwPseudoCounter, 2); // Use only 2/4 bytes
	p += 2; // +2 => 8 bytes filled

	pRandomSource->GetRandomBuffer(p, 8); // +8 => 16 bytes filled

	dw1 = *pdw1; dw2 = *pdw2; dw3 = *pdw3; dw4 = *pdw4; // Load to local

	// Mix buffer using PHTs for better read- and processability
	dw1 += dw2; dw2 += dw1; dw3 += dw4; dw4 += dw3;
	dw2 += dw3; dw3 += dw2; dw1 += dw4; dw4 += dw1;
	dw1 += dw3; dw3 += dw1; dw2 += dw4; dw4 += dw2;
	dw1 += dw2; dw2 += dw1; dw3 += dw4; dw4 += dw3;
	dw2 += dw3; dw3 += dw2; dw1 += dw4; dw4 += dw1;
	dw1 += dw3; dw3 += dw1; dw2 += dw4; dw4 += dw2;

	*pdw1 = dw1; *pdw2 = dw2; *pdw3 = dw3; *pdw4 = dw4; // Save
}

#endif // (defined(_WIN32) || defined(_WIN64))
