/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2012 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "KeyTransform_BCrypt.h"

#include "Rijndael.h"
#include "../Util/MemUtil.h"

CKeyTransform::CKeyTransform(UINT64 qwRounds, UINT8* pBuf, const UINT8* pKey)
{
	m_qwRounds = qwRounds;
	m_pBuf = pBuf;
	m_pKey = pKey;
	m_bSucceeded = false;
}

void CKeyTransform::Run()
{
	ASSERT(m_pBuf != NULL); if(m_pBuf == NULL) return;
	ASSERT(m_pKey != NULL); if(m_pKey == NULL) return;

	BYTE vData[16]; // Local copy of the data to be transformed
	memcpy(&vData[0], m_pBuf, 16);

	CKeyTransformBCrypt bCrypt;
	if(bCrypt.TransformKey(m_pKey, vData, m_qwRounds) != S_OK)
	{
		CRijndael aes;
		if(aes.Init(CRijndael::ECB, CRijndael::EncryptDir, m_pKey,
			CRijndael::Key32Bytes, 0) != RIJNDAEL_SUCCESS) { ASSERT(FALSE); return; }

		for(UINT64 qw = 0; qw < m_qwRounds; ++qw)
			aes.BlockEncrypt(&vData[0], 128, &vData[0]);
	}

	memcpy(m_pBuf, &vData[0], 16);
	mem_erase(&vData[0], 16);

	m_bSucceeded = true;
}

bool CKeyTransform::Transform256(UINT64 qwRounds, UINT8* pBuffer256,
	const UINT8* pKeySeed256)
{
	ASSERT(pBuffer256 != NULL); if(pBuffer256 == NULL) return false;
	ASSERT(pKeySeed256 != NULL); if(pKeySeed256 == NULL) return false;

	BYTE vBuf[32]; // Local copy of the data to be transformed
	memcpy(&vBuf[0], pBuffer256, 32);

	BYTE vKey[32]; // Local copy of the transformation key
	memcpy(&vKey[0], pKeySeed256, 32);

	CKeyTransform ktLeft(qwRounds, &vBuf[0], &vKey[0]);
	CKeyTransform ktRight(qwRounds, &vBuf[16], &vKey[0]);

	// No multi-threading support for _WIN32_WCE builds
#if defined(_WIN32_WCE)
	ktLeft.Run();
	ktRight.Run();
#else
	try
	{
		CKeyTransformWrapper wrLeft(&ktLeft);
		boost::thread thLeft(wrLeft);
		ktRight.Run();
		thLeft.join();
	}
	catch(...) { ASSERT(FALSE); return false; }
#endif

	ASSERT(ktLeft.Succeeded() && ktRight.Succeeded());
	if(!ktLeft.Succeeded() || !ktRight.Succeeded()) return false;

	memcpy(pBuffer256, &vBuf[0], 32);
	mem_erase(&vBuf[0], 32);
	mem_erase(&vKey[0], 32);
	return true;
}

UINT64 CKeyTransform::Benchmark(DWORD dwTimeMs)
{
	CKeyTransformBenchmark ktLeft(dwTimeMs), ktRight(dwTimeMs);

	// No multi-threading support for _WIN32_WCE builds
#if defined(_WIN32_WCE)
	ktLeft.Run();
	return (ktLeft.GetComputedRounds() >> 1);
#else
	try
	{
		CKeyTransformBenchmarkWrapper wrLeft(&ktLeft);
		boost::thread thLeft(wrLeft);
		ktRight.Run();
		thLeft.join();
	}
	catch(...) { ASSERT(FALSE); return 0; }

	const UINT64 qwLeft = ktLeft.GetComputedRounds();
	const UINT64 qwRight = ktRight.GetComputedRounds();
	const UINT64 qwSum = qwLeft + qwRight;
	if((qwSum < qwLeft) || (qwSum < qwRight)) // Overflow
		return ((qwLeft > qwRight) ? qwLeft : qwRight);

	return (qwSum >> 1);
#endif
}

CKeyTransformBenchmark::CKeyTransformBenchmark(DWORD dwTimeMs)
{
	m_dwTimeMs = dwTimeMs;
	m_qwComputedRounds = 0;
}

void CKeyTransformBenchmark::Run()
{
	ASSERT(m_dwTimeMs != 0); if(m_dwTimeMs == 0) return;
	ASSERT(m_qwComputedRounds == 0);

	BYTE vKey[32];
	memset(&vKey[0], 0x4B, 32);

	BYTE vBuf[16];
	memset(&vBuf[0], 0x7E, 16);

	CKeyTransformBCrypt bCrypt;
	if(bCrypt.Benchmark(&vKey[0], &vBuf[0], &m_qwComputedRounds, m_dwTimeMs) == S_OK)
		return;

	const DWORD dwStartTime = timeGetTime();

	CRijndael aes;
	if(aes.Init(CRijndael::ECB, CRijndael::EncryptDir, &vKey[0],
		CRijndael::Key32Bytes, 0) != RIJNDAEL_SUCCESS) { ASSERT(FALSE); return; }

	while(true)
	{
		for(DWORD dw = 0; dw < 256; ++dw)
			aes.BlockEncrypt(&vBuf[0], 128, &vBuf[0]);

		m_qwComputedRounds += 256;
		if(m_qwComputedRounds < 256) // Overflow?
		{
			m_qwComputedRounds = (UINT64_MAX - 8);
			break;
		}

		if((timeGetTime() - dwStartTime) >= m_dwTimeMs) break;
	}
}
