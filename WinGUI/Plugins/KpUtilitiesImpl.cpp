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
#include <tchar.h>
#include <assert.h>
#include "KpUtilitiesImpl.h"
#include "../../KeePassLibCpp/PwManager.h"
#include "../../KeePassLibCpp/Crypto/ARCFour.h"
#include "../../KeePassLibCpp/Util/AppUtil.h"
#include "../../KeePassLibCpp/Util/StrUtil.h"
#include "../../KeePassLibCpp/Util/MemUtil.h"
#include "../../KeePassLibCpp/Util/PwUtil.h"
#include "../../KeePassLibCpp/Util/Base64.h"
#include "../Util/WinUtil.h"
#include "../Util/CmdLine/FullPathName.h"

KP_IMPL_STDREFIMPL_NODELETE(CKpUtilitiesImpl)

CKpUtilitiesImpl::CKpUtilitiesImpl()
{
	KP_IMPL_CONSTRUCT;
}

CKpUtilitiesImpl& CKpUtilitiesImpl::Instance()
{
	static CKpUtilitiesImpl singletonObject;
	return singletonObject;
}

STDMETHODIMP CKpUtilitiesImpl::QueryInterface(REFIID riid, void** ppvObject)
{
	KP_REQ_OUT_PTR(ppvObject);

	KP_SUPPORT_INTERFACE(IID_IKpUnknown, IKpUnknown);
	KP_SUPPORT_INTERFACE(IID_IKpUtilities, IKpUtilities);

	*ppvObject = NULL;
	return E_NOINTERFACE;
}

STDMETHODIMP CKpUtilitiesImpl::ShowFileDialog(BOOL bOpenMode, LPCTSTR lpSuffix,
	LPTSTR lpStoreBuf, DWORD dwBufLen)
{
	return ((WU_GetFileNameSz(bOpenMode, lpSuffix, lpStoreBuf, dwBufLen) ==
		FALSE) ? E_ABORT : S_OK);
}

STDMETHODIMP_(char*) CKpUtilitiesImpl::UnicodeToMultiByte(const WCHAR* lpwString)
{
	return _StringToAnsi(lpwString);
}

STDMETHODIMP_(WCHAR*) CKpUtilitiesImpl::MultiByteToUnicode(const char* lpString)
{
	return _StringToUnicode(lpString);
}

STDMETHODIMP_(UTF8_BYTE*) CKpUtilitiesImpl::StringToUTF8(LPCTSTR lpSourceString)
{
	return _StringToUTF8(lpSourceString);
}

STDMETHODIMP_(DWORD) CKpUtilitiesImpl::UTF8NumChars(const UTF8_BYTE* pUTF8String)
{
	return _UTF8NumChars(pUTF8String);
}

STDMETHODIMP_(DWORD) CKpUtilitiesImpl::UTF8BytesNeeded(LPCTSTR lpString)
{
	return _UTF8BytesNeeded(lpString);
}

STDMETHODIMP_(LPTSTR) CKpUtilitiesImpl::UTF8ToString(const UTF8_BYTE* pUTF8String)
{
	return _UTF8ToString(pUTF8String);
}

STDMETHODIMP_(BOOL) CKpUtilitiesImpl::IsUTF8String(const UTF8_BYTE* pUTF8String)
{
	return _IsUTF8String(pUTF8String);
}

#pragma warning(push)
#pragma warning(disable: 4996) // _tcscpy unsafe
STDMETHODIMP CKpUtilitiesImpl::UuidToString(const BYTE* pUuid, LPTSTR lpOutBuf)
{
	if((pUuid == NULL) || (lpOutBuf == NULL)) return E_POINTER;

	CString str;
	_UuidToString(pUuid, &str);

	_tcscpy(lpOutBuf, (LPCTSTR)str);
	return S_OK;
}
#pragma warning(pop)

STDMETHODIMP CKpUtilitiesImpl::StringToUuid(LPCTSTR lpSource, BYTE* pUuid)
{
	_StringToUuid(lpSource, pUuid);
	return S_OK;
}

STDMETHODIMP_(INT) CKpUtilitiesImpl::ShellOpenLocalFile(LPCTSTR lpFile, INT nMode)
{
	return _OpenLocalFile(lpFile, nMode);
}

STDMETHODIMP CKpUtilitiesImpl::OpenUrl(LPCTSTR lpURL, HWND hParent)
{
	if(lpURL == NULL) return E_POINTER;
	OpenUrlEx(lpURL, hParent);
	return S_OK;
}

STDMETHODIMP CKpUtilitiesImpl::OpenAppHelp(LPCTSTR lpTopicFile)
{
	return ((WU_OpenAppHelp(lpTopicFile) == FALSE) ? E_FAIL : S_OK);
}

STDMETHODIMP CKpUtilitiesImpl::Base64Encode(const BYTE* pbIn, DWORD cbInLen,
	BYTE* pbOut, DWORD* pcbOutLen)
{
	if(pbIn == NULL) return E_POINTER;
	if(cbInLen == 0) return E_INVALIDARG;
	if((pbOut == NULL) || (pcbOutLen == NULL)) return E_POINTER;

	return (CBase64Codec::Encode(pbIn, cbInLen, pbOut, pcbOutLen) ? S_OK : E_FAIL);
}

STDMETHODIMP CKpUtilitiesImpl::Base64Decode(const BYTE* pbIn, DWORD cbInLen,
	BYTE* pbOut, DWORD* pcbOutLen)
{
	if(pbIn == NULL) return E_POINTER;
	if(cbInLen == 0) return E_INVALIDARG;
	if((pbOut == NULL) || (pcbOutLen == NULL)) return E_POINTER;

	return (CBase64Codec::Decode(pbIn, cbInLen, pbOut, pcbOutLen) ? S_OK : E_FAIL);
}

STDMETHODIMP CKpUtilitiesImpl::GetApplicationDirectory(LPTSTR lpStoreBuf,
	DWORD dwBufLen, BOOL bFilterSpecial, BOOL bMakeURL)
{
	if(lpStoreBuf == NULL) return E_POINTER;
	return ((AU_GetApplicationDirectory(lpStoreBuf, dwBufLen, bFilterSpecial,
		bMakeURL) == FALSE) ? E_FAIL : S_OK);
}

STDMETHODIMP_(LPTSTR) CKpUtilitiesImpl::MakeRelativePath(LPCTSTR lpBaseFile,
	LPCTSTR lpTargetFile)
{
	if((lpBaseFile == NULL) || (lpTargetFile == NULL)) return NULL;
	CString str = MakeRelativePathEx(lpBaseFile, lpTargetFile);
	return _TcsSafeDupAlloc((LPCTSTR)str);
}

STDMETHODIMP_(LPTSTR) CKpUtilitiesImpl::GetShortestAbsolutePath(LPCTSTR lpFilePath)
{
	if(lpFilePath == NULL) return NULL;
	CString str = ::GetShortestAbsolutePath(lpFilePath);
	return _TcsSafeDupAlloc((LPCTSTR)str);
}

STDMETHODIMP_(BOOL) CKpUtilitiesImpl::IsAbsolutePath(LPCTSTR lpPath)
{
	if(lpPath == NULL) return FALSE;
	return WU_IsAbsolutePath(lpPath);
}

STDMETHODIMP_(BOOL) CKpUtilitiesImpl::ValidatePath(LPCTSTR lpPath, DWORD dwOptions)
{
	if(lpPath == NULL) return FALSE;

	std_string str = lpPath;
	const FullPathName fpn(str);
	if(fpn.getState() == FullPathName::INVALID_PATH) return FALSE;

	if((dwOptions & KPVPF_MUST_EXIST) != 0)
	{
		const DWORD dwAttrib = ::GetFileAttributes(lpPath);
		if(dwAttrib == INVALID_FILE_ATTRIBUTES) return FALSE;

		if(((dwOptions & KPVPF_TYPE_DIRECTORY) != 0) &&
			((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == 0))
			return FALSE;

		if(((dwOptions & KPVPF_TYPE_FILE) != 0) &&
			((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) != 0))
			return FALSE;
	}

	if((dwOptions & KPVPF_REGULAR_NAME) != 0)
	{
		LPCTSTR lpScan = lpPath;
		while(*lpScan != 0)
		{
			const TCHAR tch = *lpScan;

			if((tch == _T('<')) || (tch == _T('>')) || (tch == _T('\"')) ||
				(tch == _T('/')) || (tch == _T('|')) || (tch == _T('?')) ||
				(tch == _T('*')))
				return FALSE;

			if((tch >= 0) && (tch <= 31)) return FALSE;

			++lpScan;
		}
	}

	return TRUE;
}

STDMETHODIMP_(LPTSTR) CKpUtilitiesImpl::GetQuotedPath(LPCTSTR lpPath)
{
	if(lpPath == NULL) return NULL;

	const std::basic_string<TCHAR> str = lpPath;
	const std::basic_string<TCHAR> strPath = SU_GetQuotedPath(str);
	return _TcsSafeDupAlloc(strPath.c_str());
}

STDMETHODIMP CKpUtilitiesImpl::CreateDirectoryTree(LPCTSTR lpDirPath, DWORD dwOptions)
{
	UNREFERENCED_PARAMETER(dwOptions);
	return WU_CreateDirectoryTree(lpDirPath);
}

STDMETHODIMP CKpUtilitiesImpl::FlushStorageBuffers(LPCTSTR lpFileOnStorage,
	BOOL bOnlyIfRemovable)
{
	if(lpFileOnStorage == NULL) return E_POINTER;
	return ((WU_FlushStorageBuffersEx(lpFileOnStorage, bOnlyIfRemovable) ==
		FALSE) ? E_FAIL : S_OK);
}

STDMETHODIMP CKpUtilitiesImpl::SecureDeleteFile(LPCTSTR lpFilePath)
{
	if(lpFilePath == NULL) return E_POINTER;
	return ((AU_SecureDeleteFile(lpFilePath) == FALSE) ? E_FAIL : S_OK);
}

STDMETHODIMP CKpUtilitiesImpl::WriteFile(LPCTSTR lpFilePath, const BYTE* pData,
	DWORD dwDataSize)
{
	if(lpFilePath == NULL) return E_POINTER;
	if(pData == NULL) return E_POINTER;
	
	const int nResult = AU_WriteBigFile(lpFilePath, pData, dwDataSize, FALSE);

	if(nResult == PWE_SUCCESS) return S_OK;
	if(nResult == PWE_NOFILEACCESS_WRITE) return E_ACCESSDENIED;
	return E_FAIL;
}

STDMETHODIMP_(INT) CKpUtilitiesImpl::CompareTimes(const PW_TIME* pTime1,
	const PW_TIME* pTime2)
{
	if((pTime1 == NULL) || (pTime2 == NULL)) return 0;
	return _pwtimecmp(pTime1, pTime2);
}

STDMETHODIMP CKpUtilitiesImpl::HashFileSHA256(LPCTSTR lpFile, BYTE* pHashBuf)
{
	if((lpFile == NULL) || (pHashBuf == NULL)) return E_POINTER;
	return ((SHA256_HashFile(lpFile, pHashBuf) == FALSE) ? E_FAIL : S_OK);
}

STDMETHODIMP_(DWORD) CKpUtilitiesImpl::EstimatePasswordBits(LPCTSTR lpPassword)
{
	return CPwUtil::EstimatePasswordBits(lpPassword);
}

STDMETHODIMP_(BOOL) CKpUtilitiesImpl::IsTANEntry(const PW_ENTRY* pEntry)
{
	return CPwUtil::IsTANEntry(pEntry);
}

STDMETHODIMP CKpUtilitiesImpl::SHA256CreateContext(void** pOutNewContext)
{
	if(pOutNewContext == NULL) return E_POINTER;

	sha256_ctx* pContext = new sha256_ctx;
	sha256_begin(pContext);
	*pOutNewContext = pContext;
	return S_OK;
}

STDMETHODIMP CKpUtilitiesImpl::SHA256Init(void* pContext)
{
	if(pContext == NULL) return E_POINTER;
	sha256_begin((sha256_ctx*)pContext);
	return S_OK;
}

STDMETHODIMP CKpUtilitiesImpl::SHA256Hash(void* pContext, const BYTE* pData, DWORD dwDataLength)
{
	if((pContext == NULL) || (pData == NULL)) return E_POINTER;
	sha256_hash(pData, dwDataLength, (sha256_ctx*)pContext);
	return S_OK;
}

STDMETHODIMP CKpUtilitiesImpl::SHA256Final(void* pContext, BYTE* pOutHashBuf)
{
	if((pContext == NULL) || (pOutHashBuf == NULL)) return E_POINTER;
	sha256_end(pOutHashBuf, (sha256_ctx*)pContext);
	return S_OK;
}

STDMETHODIMP CKpUtilitiesImpl::EncryptMemory(BYTE* pbBuf, DWORD dwBufLen,
	const BYTE* pbKey, DWORD dwKeyLen)
{
	if((pbBuf == NULL) || (pbKey == NULL)) return E_POINTER;
	ARCFourCrypt(pbBuf, dwBufLen, pbKey, dwKeyLen);
	return S_OK;
}

STDMETHODIMP CKpUtilitiesImpl::DecryptMemory(BYTE* pbBuf, DWORD dwBufLen,
	const BYTE* pbKey, DWORD dwKeyLen)
{
	// ARCFour is self-inverse
	return CKpUtilitiesImpl::EncryptMemory(pbBuf, dwBufLen, pbKey, dwKeyLen);
}
