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

#ifndef ___KEY_TRANSFORM_BCRYPT_H___

#include <windows.h>
#include <boost/scoped_array.hpp>

#define BCRYPT_DLLNAME _T("BCrypt.dll")

#define BCFN_OAP "BCryptOpenAlgorithmProvider"
#define BCFN_CAP "BCryptCloseAlgorithmProvider"
#define BCFN_GP  "BCryptGetProperty"
#define BCFN_SP  "BCryptSetProperty"
#define BCFN_GSK "BCryptGenerateSymmetricKey"
#define BCFN_IK  "BCryptImportKey"
#define BCFN_DK  "BCryptDestroyKey"
#define BCFN_E   "BCryptEncrypt"

#ifndef BCRYPT_OBJECT_LENGTH
typedef PVOID BCRYPT_HANDLE;
typedef PVOID BCRYPT_ALG_HANDLE;
typedef PVOID BCRYPT_KEY_HANDLE;
#define BCRYPT_AES_ALGORITHM   L"AES"
#define BCRYPT_OBJECT_LENGTH   L"ObjectLength"
#define BCRYPT_KEY_LENGTH      L"KeyLength"
#define BCRYPT_KEY_STRENGTH    L"KeyStrength"
#define BCRYPT_KEY_DATA_BLOB   L"KeyDataBlob"
#define BCRYPT_PROVIDER_HANDLE L"ProviderHandle"
#define BCRYPT_CHAINING_MODE   L"ChainingMode"
#define BCRYPT_CHAIN_MODE_ECB  L"ChainingModeECB"
#define BCRYPT_KEY_DATA_BLOB_MAGIC 0x4d42444b
#define BCRYPT_KEY_DATA_BLOB_VERSION1 0x1
#endif

#ifndef _NTDEF_
typedef LONG NTSTATUS;
#endif

typedef struct _BCRYPT_KEY_DATA_BLOB_32
{
	ULONG dwMagic;
	ULONG dwVersion;
	ULONG cbKeyData;
	BYTE pbData[32];
} BCRYPT_KEY_DATA_BLOB_32;

typedef NTSTATUS(WINAPI *LPBCRYPTOPENALGORITHMPROVIDER)(BCRYPT_ALG_HANDLE* phAlgorithm,
	LPCWSTR pszAlgId, LPCWSTR pszImplementation, ULONG dwFlags);

typedef NTSTATUS(WINAPI *LPBCRYPTCLOSEALGORITHMPROVIDER)(BCRYPT_ALG_HANDLE hAlgorithm,
	ULONG dwFlags);

typedef NTSTATUS(WINAPI *LPBCRYPTGETPROPERTY)(BCRYPT_HANDLE hObject, LPCWSTR pszProperty,
	PUCHAR pbOutput, ULONG cbOutput, ULONG* pcbResult, ULONG dwFlags);

typedef NTSTATUS(WINAPI *LPBCRYPTSETPROPERTY)(BCRYPT_HANDLE hObject, LPCWSTR pszProperty,
	PUCHAR pbInput, ULONG cbInput, ULONG dwFlags);

typedef NTSTATUS(WINAPI *LPBCRYPTGENERATESYMMETRICKEY)(BCRYPT_ALG_HANDLE hAlgorithm,
	BCRYPT_KEY_HANDLE* phKey, PUCHAR pbKeyObject, ULONG cbKeyObject, PUCHAR pbSecret,
	ULONG cbSecret, ULONG dwFlags);

typedef NTSTATUS(WINAPI *LPBCRYPTIMPORTKEY)(BCRYPT_ALG_HANDLE hAlgorithm,
	BCRYPT_KEY_HANDLE hImportKey, LPCWSTR pszBlobType, BCRYPT_KEY_HANDLE *phKey,
	PUCHAR pbKeyObject, ULONG cbKeyObject, PUCHAR pbInput, ULONG cbInput, ULONG dwFlags);

typedef NTSTATUS(WINAPI *LPBCRYPTDESTROYKEY)(BCRYPT_KEY_HANDLE hKey);

typedef NTSTATUS(WINAPI *LPBCRYPTENCRYPT)(BCRYPT_KEY_HANDLE hKey, PUCHAR pbInput,
	ULONG cbInput, VOID* pPaddingInfo, PUCHAR pbIV, ULONG cbIV, PUCHAR pbOutput,
	ULONG cbOutput, ULONG* pcbResult, ULONG dwFlags);

class CKeyTransformBCrypt
{
public:
	CKeyTransformBCrypt();
	virtual ~CKeyTransformBCrypt();

	static BOOL* GetEnabledPtr();

	HRESULT TransformKey(const BYTE* pbKey32, BYTE* pbData16, UINT64 qwRounds);
	HRESULT Benchmark(const BYTE* pbKey32, BYTE* pbData16, UINT64* pqwRounds,
		DWORD dwTimeMs);

private:
	void _FreeLib();
	HRESULT _InitBCrypt(BCRYPT_ALG_HANDLE& hAes, BCRYPT_KEY_HANDLE& hKey,
		boost::scoped_array<UCHAR>& pKeyObj, const BYTE* pbKey32);
	void _DestroyBCrypt(BCRYPT_ALG_HANDLE& hAes, BCRYPT_KEY_HANDLE& hKey);

	static BOOL m_bEnableBCrypt;

	HMODULE m_hLib;
	LPBCRYPTOPENALGORITHMPROVIDER m_lpBCryptOpenAlgorithmProvider;
	LPBCRYPTCLOSEALGORITHMPROVIDER m_lpBCryptCloseAlgorithmProvider;
	LPBCRYPTGETPROPERTY m_lpBCryptGetProperty;
	LPBCRYPTSETPROPERTY m_lpBCryptSetProperty;
	LPBCRYPTGENERATESYMMETRICKEY m_lpBCryptGenerateSymmetricKey;
	LPBCRYPTIMPORTKEY m_lpBCryptImportKey;
	LPBCRYPTDESTROYKEY m_lpBCryptDestroyKey;
	LPBCRYPTENCRYPT m_lpBCryptEncrypt;
};

#endif // ___KEY_TRANSFORM_BCRYPT_H___
