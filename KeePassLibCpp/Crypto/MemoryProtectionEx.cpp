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
#include "MemoryProtectionEx.h"
#include "../Util/AppUtil.h"
#include "../Util/StrUtil.h"
#include "ARCFour.h"

// #include <mmsystem.h>
// #include <string>

BOOL CMemoryProtectionEx::m_bEnabled = FALSE;
BOOL CMemoryProtectionEx::m_bEnabledAtStart = FALSE;

bool CMemoryProtectionEx::m_bSupportTested = false;
HMODULE CMemoryProtectionEx::m_hCryptLib = NULL;
LPFNCRYPTPROTECTMEMORY CMemoryProtectionEx::m_lpCryptProtectMemory = NULL;
LPFNCRYPTUNPROTECTMEMORY CMemoryProtectionEx::m_lpCryptUnprotectMemory = NULL;

#ifdef _DEBUG
static DWORD g_dwLoadCount = 0;
#endif

CMemoryProtectionEx::CMemoryProtectionEx()
{
}

CMemoryProtectionEx::~CMemoryProtectionEx()
{
}

void CMemoryProtectionEx::SetEnabled(BOOL bEnabled)
{
	m_bEnabled = ((bEnabled == FALSE) ? FALSE : TRUE);
}

void CMemoryProtectionEx::SetEnabledAtStart(BOOL bEnabled)
{
	ASSERT((m_bEnabled == FALSE) && (m_bEnabledAtStart == FALSE)); // Start values

	m_bEnabledAtStart = ((bEnabled == FALSE) ? FALSE : TRUE);
	m_bEnabled = m_bEnabledAtStart;
}

BOOL* CMemoryProtectionEx::GetEnabledPtr()
{
	return &m_bEnabled;
}

void CMemoryProtectionEx::LoadCryptLib()
{
	CMemoryProtectionEx::FreeCryptLib();

	m_bSupportTested = true;

	if(m_bEnabledAtStart == FALSE) return; // DPAPI disabled
	if(AU_IsAtLeastWinVistaSystem() == FALSE) return; // Requires Windows Vista or higher

	m_hCryptLib = ::LoadLibrary(MPU_CRYPTLIB_NAME);
	if(m_hCryptLib != NULL)
	{
		m_lpCryptProtectMemory = (LPFNCRYPTPROTECTMEMORY)::GetProcAddress(
			m_hCryptLib, MPU_CRYPTPROT_NAME);
		m_lpCryptUnprotectMemory = (LPFNCRYPTUNPROTECTMEMORY)::GetProcAddress(
			m_hCryptLib, MPU_CRYPTUNPROT_NAME);

		if((m_lpCryptProtectMemory == NULL) || (m_lpCryptUnprotectMemory == NULL))
		{
			ASSERT(FALSE);
			CMemoryProtectionEx::FreeCryptLib();
		}

#ifdef _DEBUG
		++g_dwLoadCount;
		ASSERT(g_dwLoadCount <= 1);
#endif
	}
	else { ASSERT(FALSE); }
}

void CMemoryProtectionEx::FreeCryptLib()
{
	if(m_hCryptLib != NULL) { VERIFY(::FreeLibrary(m_hCryptLib)); }

	m_bSupportTested = false;
	m_hCryptLib = NULL;
	m_lpCryptProtectMemory = NULL;
	m_lpCryptUnprotectMemory = NULL;
}

void CMemoryProtectionEx::Release()
{
	CMemoryProtectionEx::FreeCryptLib();
}

DWORD CMemoryProtectionEx::ToBlockSize(DWORD dwByteCount)
{
	const DWORD dwMod = (dwByteCount % CRYPTPROTECTMEMORY_BLOCK_SIZE);
	if(dwMod == 0) return dwByteCount;

	return (dwByteCount + CRYPTPROTECTMEMORY_BLOCK_SIZE - dwMod);
}

HRESULT CMemoryProtectionEx::EncryptMemory(LPVOID lpBuffer, DWORD cbSize)
{
	if(lpBuffer == NULL) { ASSERT(FALSE); return E_POINTER; }
	if(cbSize == 0) return S_OK;
	if(CMemoryProtectionEx::ToBlockSize(cbSize) != cbSize) { ASSERT(FALSE); return E_INVALIDARG; }

	if(!m_bSupportTested) CMemoryProtectionEx::LoadCryptLib();
	if(m_lpCryptProtectMemory == NULL) return E_FAIL; // DPAPI not available or disabled

#ifdef _DEBUG
#if (defined(_WIN32) || defined(_WIN64))
	ASSERT((IsBadWritePtr(lpBuffer, cbSize) == FALSE) && (IsBadReadPtr(lpBuffer, cbSize) == FALSE));
#endif

	BYTE* lpOrg = new BYTE[cbSize];
	memcpy(lpOrg, lpBuffer, cbSize);
#endif

	if(m_lpCryptProtectMemory(lpBuffer, cbSize, CRYPTPROTECTMEMORY_SAME_PROCESS)
		== FALSE) { ASSERT(FALSE); return E_FAIL; }

#ifdef _DEBUG
	if(cbSize >= 4) { ASSERT(memcmp(lpBuffer, lpOrg, cbSize) != 0); }
	SecureZeroMemory(lpOrg, cbSize);
	SAFE_DELETE_ARRAY(lpOrg);
#endif

	return S_OK;
}

HRESULT CMemoryProtectionEx::DecryptMemory(LPVOID lpBuffer, DWORD cbSize)
{
	if(lpBuffer == NULL) { ASSERT(FALSE); return E_POINTER; }
	if(cbSize == 0) return S_OK;
	if(CMemoryProtectionEx::ToBlockSize(cbSize) != cbSize) { ASSERT(FALSE); return E_INVALIDARG; }

	if(!m_bSupportTested) CMemoryProtectionEx::LoadCryptLib();
	if(m_lpCryptUnprotectMemory == NULL) return E_FAIL; // DPAPI not available or disabled

#if (defined(_WIN32) || defined(_WIN64))
	ASSERT((IsBadWritePtr(lpBuffer, cbSize) == FALSE) && (IsBadReadPtr(lpBuffer, cbSize) == FALSE));
#endif

	if(m_lpCryptUnprotectMemory(lpBuffer, cbSize, CRYPTPROTECTMEMORY_SAME_PROCESS)
		== FALSE) { ASSERT(FALSE); return E_FAIL; }

	return S_OK;
}

HRESULT CMemoryProtectionEx::EncryptText(LPTSTR lpText, DWORD dwCharacters)
{
	if(lpText == NULL) { ASSERT(FALSE); return E_POINTER; }
	// dwCharacters == 0 will encrypt one block
	ASSERT(static_cast<DWORD>(_tcslen(lpText)) == dwCharacters);

	if(!m_bSupportTested) CMemoryProtectionEx::LoadCryptLib();
	if(m_lpCryptProtectMemory == NULL) return E_FAIL; // DPAPI not available or disabled

	const DWORD dwBytes = CMemoryProtectionEx::ToBlockSize(
		(dwCharacters + 1) * sizeof(TCHAR));

#ifdef _DEBUG
#if (defined(_WIN32) || defined(_WIN64))
	ASSERT((IsBadWritePtr(lpText, dwBytes) == FALSE) && (IsBadReadPtr(lpText, dwBytes) == FALSE));
#endif

	LPTSTR lpOrg = _TcsSafeDupAlloc(lpText);
#endif

	if(m_lpCryptProtectMemory(lpText, dwBytes, CRYPTPROTECTMEMORY_SAME_PROCESS)
		== FALSE) { ASSERT(FALSE); return E_FAIL; }

#ifdef _DEBUG
	if(_tcslen(lpOrg) >= 4)
	{
		ASSERT(memcmp(lpText, lpOrg, (_tcslen(lpOrg) + 1) * sizeof(TCHAR)) != 0);
	}
	SecureZeroMemory(lpOrg, _tcslen(lpOrg) * sizeof(TCHAR));
	SAFE_DELETE_ARRAY(lpOrg);
#endif

	return S_OK;
}

HRESULT CMemoryProtectionEx::DecryptText(LPTSTR lpText, DWORD dwCharacters)
{
	if(lpText == NULL) { ASSERT(FALSE); return E_POINTER; }
	// dwCharacters == 0 will decrypt one block

	if(!m_bSupportTested) CMemoryProtectionEx::LoadCryptLib();
	if(m_lpCryptUnprotectMemory == NULL) return E_FAIL; // DPAPI not available or disabled

	const DWORD dwBytes = CMemoryProtectionEx::ToBlockSize(
		(dwCharacters + 1) * sizeof(TCHAR));

#if (defined(_WIN32) || defined(_WIN64))
	ASSERT((IsBadWritePtr(lpText, dwBytes) == FALSE) && (IsBadReadPtr(lpText, dwBytes) == FALSE));
#endif

	if(m_lpCryptUnprotectMemory(lpText, dwBytes, CRYPTPROTECTMEMORY_SAME_PROCESS)
		== FALSE) { ASSERT(FALSE); return E_FAIL; }

	ASSERT(static_cast<DWORD>(_tcslen(lpText)) == dwCharacters);
	return S_OK;
}

/* void CMemoryProtectionEx::Benchmark()
{
#ifndef _WIN32_WCE
	LPCTSTR lpOrg = _T("123456789012345");
	std::basic_string<TCHAR> strText = lpOrg;

	const DWORD dwBytes = ((strText.size() + 1) * sizeof(TCHAR));
	ASSERT(CMemoryProtectionEx::ToBlockSize(dwBytes) == dwBytes);

	BYTE vKey12[12];
	ZeroMemory(vKey12, 12);

	const DWORD dwRounds = 0xFFFFFF;

	const DWORD dwArcFourStart = timeGetTime();
	for(DWORD i = 0; i < dwRounds; ++i)
	{
		ARCFourCrypt((UINT8*)strText.c_str(), dwBytes, vKey12, 12);
		ARCFourCrypt((UINT8*)strText.c_str(), dwBytes, vKey12, 12);
	}
	const DWORD dwArcFourEnd = timeGetTime();

	const DWORD dwDpapiStart = timeGetTime();
	for(DWORD j = 0; j < dwRounds; ++j)
	{
		CMemoryProtectionEx::EncryptText(strText.c_str(), strText.size());
		CMemoryProtectionEx::DecryptText(strText.c_str(), strText.size());
	}
	const DWORD dwDpapiEnd = timeGetTime();

	const DWORD dwArcFourTime = dwArcFourEnd - dwArcFourStart;
	const DWORD dwDpapiTime = dwDpapiEnd - dwDpapiStart;
#endif // _WIN32_WCE
} */
