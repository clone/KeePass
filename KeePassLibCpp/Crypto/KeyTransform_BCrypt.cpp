/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2013 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "KeyTransform_BCrypt.h"
#include "../Util/AppUtil.h"

BOOL CKeyTransformBCrypt::m_bEnableBCrypt = TRUE;

CKeyTransformBCrypt::CKeyTransformBCrypt()
{
	if(m_bEnableBCrypt == FALSE) { m_hLib = NULL; return; }

	// BCrypt.dll is only supported on >= Vista
	if(AU_IsAtLeastWinVistaSystem() == FALSE) { m_hLib = NULL; return; }

	m_hLib = AU_LoadLibrary(BCRYPT_DLLNAME);
	if(m_hLib == NULL) return;

	m_lpBCryptOpenAlgorithmProvider = (LPBCRYPTOPENALGORITHMPROVIDER)
		GetProcAddress(m_hLib, BCFN_OAP);
	m_lpBCryptCloseAlgorithmProvider = (LPBCRYPTCLOSEALGORITHMPROVIDER)
		GetProcAddress(m_hLib, BCFN_CAP);
	m_lpBCryptGetProperty = (LPBCRYPTGETPROPERTY)GetProcAddress(m_hLib, BCFN_GP);
	m_lpBCryptSetProperty = (LPBCRYPTSETPROPERTY)GetProcAddress(m_hLib, BCFN_SP);
	m_lpBCryptGenerateSymmetricKey = (LPBCRYPTGENERATESYMMETRICKEY)
		GetProcAddress(m_hLib, BCFN_GSK);
	m_lpBCryptImportKey = (LPBCRYPTIMPORTKEY)GetProcAddress(m_hLib, BCFN_IK);
	m_lpBCryptDestroyKey = (LPBCRYPTDESTROYKEY)GetProcAddress(m_hLib, BCFN_DK);
	m_lpBCryptEncrypt = (LPBCRYPTENCRYPT)GetProcAddress(m_hLib, BCFN_E);

	if((m_lpBCryptOpenAlgorithmProvider == NULL) || (m_lpBCryptCloseAlgorithmProvider == NULL) ||
		(m_lpBCryptGetProperty == NULL) || (m_lpBCryptSetProperty == NULL) ||
		(m_lpBCryptGenerateSymmetricKey == NULL) || (m_lpBCryptImportKey == NULL) ||
		(m_lpBCryptDestroyKey == NULL) || (m_lpBCryptEncrypt == NULL))
	{
		ASSERT(FALSE);
		this->_FreeLib();
	}
}

CKeyTransformBCrypt::~CKeyTransformBCrypt()
{
	this->_FreeLib();
}

BOOL* CKeyTransformBCrypt::GetEnabledPtr()
{
	return &m_bEnableBCrypt;
}

void CKeyTransformBCrypt::_FreeLib()
{
	if(m_hLib != NULL)
	{
		VERIFY(FreeLibrary(m_hLib));
		m_hLib = NULL;
	}
}

#define KTBC_FAIL { ASSERT(FALSE); VERIFY(m_lpBCryptCloseAlgorithmProvider(hAes, \
	0) == 0); return E_FAIL; }

HRESULT CKeyTransformBCrypt::_InitBCrypt(BCRYPT_ALG_HANDLE& hAes, BCRYPT_KEY_HANDLE& hKey,
	boost::scoped_array<UCHAR>& pKeyObj, const BYTE* pbKey32)
{
	if(m_lpBCryptOpenAlgorithmProvider(&hAes, BCRYPT_AES_ALGORITHM, NULL, 0) != 0)
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	DWORD dwKeyObjLen = 0;
	ULONG uResult = 0;
	if(m_lpBCryptGetProperty(hAes, BCRYPT_OBJECT_LENGTH, (PUCHAR)&dwKeyObjLen,
		sizeof(DWORD), &uResult, 0) != 0) KTBC_FAIL;
	if(dwKeyObjLen == 0) KTBC_FAIL;

	pKeyObj.reset(new UCHAR[dwKeyObjLen]);

	if(m_lpBCryptSetProperty(hAes, BCRYPT_CHAINING_MODE, (PUCHAR)BCRYPT_CHAIN_MODE_ECB,
		static_cast<ULONG>((wcslen(BCRYPT_CHAIN_MODE_ECB) + 1) * sizeof(wchar_t)), 0) != 0)
		KTBC_FAIL;

	BCRYPT_KEY_DATA_BLOB_32 keyBlob;
	ZeroMemory(&keyBlob, sizeof(BCRYPT_KEY_DATA_BLOB_32));
	keyBlob.dwMagic = BCRYPT_KEY_DATA_BLOB_MAGIC;
	keyBlob.dwVersion = BCRYPT_KEY_DATA_BLOB_VERSION1;
	keyBlob.cbKeyData = 32;
	memcpy(keyBlob.pbData, pbKey32, 32);

	// if(m_lpBCryptGenerateSymmetricKey(hAes, &hKey, (PUCHAR)pKeyObj.get(),
	//	dwKeyObjLen, const_cast<PUCHAR>(pbKey32), 32, 0) != 0) KTBC_FAIL;
	if(m_lpBCryptImportKey(hAes, NULL, BCRYPT_KEY_DATA_BLOB, &hKey,
		pKeyObj.get(), dwKeyObjLen, (PUCHAR)&keyBlob,
		sizeof(BCRYPT_KEY_DATA_BLOB_32), 0) != 0) KTBC_FAIL;

#ifdef _DEBUG
	DWORD dwKeyLen = 0;
	VERIFY(m_lpBCryptGetProperty(hKey, BCRYPT_KEY_STRENGTH, (PUCHAR)&dwKeyLen,
		sizeof(DWORD), &uResult, 0) == 0);
	VERIFY(dwKeyLen == 256);

	BCRYPT_ALG_HANDLE hRef = NULL;
	VERIFY(m_lpBCryptGetProperty(hKey, BCRYPT_PROVIDER_HANDLE, (PUCHAR)&hRef,
		sizeof(BCRYPT_ALG_HANDLE), &uResult, 0) == 0);
	VERIFY(hRef == hAes);
#endif

	return S_OK;
}

void CKeyTransformBCrypt::_DestroyBCrypt(BCRYPT_ALG_HANDLE& hAes, BCRYPT_KEY_HANDLE& hKey)
{
	if(hKey != NULL) { VERIFY(m_lpBCryptDestroyKey(hKey) == 0); hKey = NULL; }
	if(hAes != NULL) { VERIFY(m_lpBCryptCloseAlgorithmProvider(hAes, 0) == 0); hAes = NULL; }
}

HRESULT CKeyTransformBCrypt::TransformKey(const BYTE* pbKey32, BYTE* pbData16,
	UINT64 qwRounds)
{
	if((pbKey32 == NULL) || (pbData16 == NULL)) return E_POINTER;
	if(qwRounds == 0) return S_OK;
	if(m_hLib == NULL) return E_NOINTERFACE;

	BCRYPT_ALG_HANDLE hAes = NULL;
	BCRYPT_KEY_HANDLE hKey = NULL;
	boost::scoped_array<UCHAR> pKeyObj;

	if(_InitBCrypt(hAes, hKey, pKeyObj, pbKey32) != S_OK) return E_FAIL;

	ULONG uResult = 0;
	if(m_lpBCryptEncrypt(hKey, pbData16, 16, NULL, NULL, 0, pbData16, 16, &uResult, 0) != 0)
	{
		ASSERT(FALSE);
		_DestroyBCrypt(hAes, hKey);
		return E_FAIL;
	}

#ifdef _DEBUG
	for(UINT64 qw = 1; qw < qwRounds; ++qw)
	{
		VERIFY(m_lpBCryptEncrypt(hKey, pbData16, 16, NULL, NULL, 0, pbData16,
			16, &uResult, 0) == 0);
	}
#else
	for(UINT64 qw = 1; qw < qwRounds; ++qw)
		m_lpBCryptEncrypt(hKey, pbData16, 16, NULL, NULL, 0, pbData16,
			16, &uResult, 0);
#endif

	_DestroyBCrypt(hAes, hKey);
	return S_OK;
}

HRESULT CKeyTransformBCrypt::Benchmark(const BYTE* pbKey32, BYTE* pbData16,
	UINT64* pqwRounds, DWORD dwTimeMs)
{
	if((pbKey32 == NULL) || (pbData16 == NULL)) return E_POINTER;
	if(pqwRounds == NULL) return E_POINTER;
	if(m_hLib == NULL) return E_NOINTERFACE;

	BCRYPT_ALG_HANDLE hAes = NULL;
	BCRYPT_KEY_HANDLE hKey = NULL;
	boost::scoped_array<UCHAR> pKeyObj;

	if(_InitBCrypt(hAes, hKey, pKeyObj, pbKey32) != S_OK) return E_FAIL;

	ULONG uResult = 0;
	UINT64 qwRounds = 0;
	const DWORD dwStartTime = timeGetTime();

	while(true)
	{
#ifdef _DEBUG
		for(DWORD dw = 0; dw < 512; ++dw)
		{
			VERIFY(m_lpBCryptEncrypt(hKey, pbData16, 16, NULL, NULL, 0, pbData16,
				16, &uResult, 0) == 0);
		}
#else
		for(DWORD dw = 0; dw < 512; ++dw)
			m_lpBCryptEncrypt(hKey, pbData16, 16, NULL, NULL, 0, pbData16,
				16, &uResult, 0);
#endif

		qwRounds += 512;
		if(qwRounds < 512) // Overflow?
		{
			qwRounds = (UINT64_MAX - 8);
			break;
		}

		if((timeGetTime() - dwStartTime) >= dwTimeMs) break;
	}

	_DestroyBCrypt(hAes, hKey);
	*pqwRounds = qwRounds;
	return S_OK;
}
