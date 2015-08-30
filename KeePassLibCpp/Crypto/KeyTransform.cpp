/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2009 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "../SysDefEx.h"
#include <mmsystem.h>
#include "KeyTransform.h"

#include "Rijndael.h"
#include "../Util/MemUtil.h"

CKeyTransform::CKeyTransform()
{
	m_dwRounds = 0;
	m_pBuf = NULL;
	m_pKey = NULL;
	m_pbSucceeded = NULL;
}

CKeyTransform::CKeyTransform(const CKeyTransform& cc)
{
	m_dwRounds = cc.m_dwRounds;
	m_pBuf = cc.m_pBuf;
	m_pKey = cc.m_pKey;
	m_pbSucceeded = cc.m_pbSucceeded;
}

CKeyTransform::CKeyTransform(DWORD dwRounds, UINT8 *pBuf, const UINT8 *pKey,
	bool *pbSucceeded)
{
	m_dwRounds = dwRounds;
	m_pBuf = pBuf;
	m_pKey = pKey;
	m_pbSucceeded = pbSucceeded;
}

CKeyTransform::~CKeyTransform()
{
}

void CKeyTransform::operator()()
{
	ASSERT(m_pBuf != NULL); if(m_pBuf == NULL) return;
	ASSERT(m_pKey != NULL); if(m_pKey == NULL) return;
	ASSERT(m_pbSucceeded != NULL); if(m_pbSucceeded == NULL) return;

	BYTE vData[16]; // Local copy of the data to be transformed
	memcpy(&vData[0], m_pBuf, 16);

	CRijndael aes;
	if(aes.Init(CRijndael::ECB, CRijndael::EncryptDir, m_pKey,
		CRijndael::Key32Bytes, 0) != RIJNDAEL_SUCCESS) { ASSERT(FALSE); return; }

	for(DWORD dw = 0; dw < m_dwRounds; ++dw)
		aes.BlockEncrypt(&vData[0], 128, &vData[0]);

	memcpy(m_pBuf, &vData[0], 16);
	mem_erase(&vData[0], 16);

	*m_pbSucceeded = true;
}

bool CKeyTransform::Transform256(DWORD dwRounds, UINT8 *pBuffer256, const UINT8 *pKeySeed256)
{
	ASSERT(pBuffer256 != NULL); if(pBuffer256 == NULL) return false;
	ASSERT(pKeySeed256 != NULL); if(pKeySeed256 == NULL) return false;

	BYTE vBuf[32]; // Local copy of the data to be transformed
	memcpy(&vBuf[0], pBuffer256, 32);

	BYTE vKey[32]; // Local copy of the transformation key
	memcpy(&vKey[0], pKeySeed256, 32);

	bool bSucceededLeft = false, bSucceededRight = false;

	CKeyTransform ktLeft(dwRounds, &vBuf[0], &vKey[0], &bSucceededLeft);
	CKeyTransform ktRight(dwRounds, &vBuf[16], &vKey[0], &bSucceededRight);

#if defined(_M_X64) || defined(_WIN32_WCE)
#pragma message("No multi-threading support for x64 and _WIN32_WCE builds.")
	ktLeft();
	ktRight();
#else // Supports multi-threading
	try
	{
		boost::thread thLeft(ktLeft);
		ktRight();
		thLeft.join();
	}
	catch(...) { ASSERT(FALSE); return false; }
#endif

	ASSERT(bSucceededLeft && bSucceededRight);
	if((bSucceededLeft == false) || (bSucceededRight == false))
		return false;

	memcpy(pBuffer256, &vBuf[0], 32);
	mem_erase(&vBuf[0], 32);
	mem_erase(&vKey[0], 32);
	return true;
}

DWORD CKeyTransform::Benchmark(DWORD dwTimeMs)
{
	CKeyTransformBenchmark ktLeft(dwTimeMs), ktRight(dwTimeMs);

#if defined(_M_X64) || defined(_WIN32_WCE)
#pragma message("No multi-threading support for x64 and _WIN32_WCE builds.")
	ktLeft();
	ktRight();
#else // Supports multi-threading
	try
	{
		boost::thread thLeft(ktLeft);
		ktRight();
		thLeft.join();
	}
	catch(...) { ASSERT(FALSE); return 0; }
#endif

	const DWORD dwLeft = ktLeft.GetComputedRounds();
	const DWORD dwRight = ktRight.GetComputedRounds();
	const DWORD dwSum = dwLeft + dwRight;
	if((dwSum < dwLeft) || (dwSum < dwRight)) return DWORD_MAX - 8;

	return dwSum;
}

CKeyTransformBenchmark::CKeyTransformBenchmark()
{
	m_dwTimeMs = 0;
	m_dwComputedRounds = 0;
}

CKeyTransformBenchmark::CKeyTransformBenchmark(const CKeyTransformBenchmark& cc)
{
	m_dwTimeMs = cc.m_dwTimeMs;
	m_dwComputedRounds = cc.m_dwComputedRounds;
}

CKeyTransformBenchmark::CKeyTransformBenchmark(DWORD dwTimeMs)
{
	m_dwTimeMs = dwTimeMs;
	m_dwComputedRounds = 0;
}

CKeyTransformBenchmark::~CKeyTransformBenchmark()
{
}

DWORD CKeyTransformBenchmark::GetComputedRounds()
{
	return m_dwComputedRounds;
}

void CKeyTransformBenchmark::operator()()
{
	ASSERT(m_dwTimeMs != 0); if(m_dwTimeMs == 0) return;
	ASSERT(m_dwComputedRounds == 0);

	const DWORD dwStartTime = timeGetTime();

	BYTE vKey[32];
	memset(&vKey[0], 0x4B, 32);

	BYTE vBuf[32];
	memset(&vBuf[0], 0x7E, 32);

	CRijndael aes;
	if(aes.Init(CRijndael::ECB, CRijndael::EncryptDir, &vKey[0],
		CRijndael::Key32Bytes, 0) != RIJNDAEL_SUCCESS) { ASSERT(FALSE); return; }

	while(true)
	{
		for(DWORD dw = 0; dw < 64; ++dw)
			aes.BlockEncrypt(&vBuf[0], 128, &vBuf[0]);

		m_dwComputedRounds += 64;

		if(m_dwComputedRounds < 64) // Overflow?
		{
			m_dwComputedRounds = DWORD_MAX - 8;
			break;
		}

		if((timeGetTime() - dwStartTime) >= m_dwTimeMs) break;
	}
}
