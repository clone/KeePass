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

#ifndef ___KPUTILITIESIMPL_H___
#define ___KPUTILITIESIMPL_H___

#include "../../KeePassLibCpp/SDK/KpSDK.h"

#pragma pack(1)

class CKpUtilitiesImpl : public IKpUtilities
{
private:
	CKpUtilitiesImpl();

public:
	static CKpUtilitiesImpl& Instance();

	KP_DECL_IUNKNOWN;

	STDMETHODIMP ShowFileDialog(BOOL bOpenMode, LPCTSTR lpSuffix, LPTSTR lpStoreBuf,
		DWORD dwBufLen);

	STDMETHODIMP_(char*) UnicodeToMultiByte(const WCHAR* lpwString);
	STDMETHODIMP_(WCHAR*) MultiByteToUnicode(const char* lpString);

	STDMETHODIMP_(UTF8_BYTE*) StringToUTF8(LPCTSTR lpSourceString);
	STDMETHODIMP_(DWORD) UTF8NumChars(const UTF8_BYTE* pUTF8String);
	STDMETHODIMP_(DWORD) UTF8BytesNeeded(LPCTSTR lpString);
	STDMETHODIMP_(LPTSTR) UTF8ToString(const UTF8_BYTE* pUTF8String);
	STDMETHODIMP_(BOOL) IsUTF8String(const UTF8_BYTE* pUTF8String);

	STDMETHODIMP UuidToString(const BYTE* pUuid, LPTSTR lpOutBuf);
	STDMETHODIMP StringToUuid(LPCTSTR lpSource, BYTE* pUuid);

	STDMETHODIMP_(INT) ShellOpenLocalFile(LPCTSTR lpFile, INT nMode);
	STDMETHODIMP OpenUrl(LPCTSTR lpURL, HWND hParent);
	STDMETHODIMP OpenAppHelp(LPCTSTR lpTopicFile);

	STDMETHODIMP Base64Encode(const BYTE* pbIn, DWORD cbInLen, BYTE* pbOut, DWORD* pcbOutLen);
	STDMETHODIMP Base64Decode(const BYTE* pbIn, DWORD cbInLen, BYTE* pbOut, DWORD* pcbOutLen);

	STDMETHODIMP GetApplicationDirectory(LPTSTR lpStoreBuf, DWORD dwBufLen,
		BOOL bFilterSpecial, BOOL bMakeURL);
	STDMETHODIMP_(LPTSTR) MakeRelativePath(LPCTSTR lpBaseFile, LPCTSTR lpTargetFile);
	STDMETHODIMP_(LPTSTR) GetShortestAbsolutePath(LPCTSTR lpFilePath);
	STDMETHODIMP_(BOOL) IsAbsolutePath(LPCTSTR lpPath);
	STDMETHODIMP_(BOOL) ValidatePath(LPCTSTR lpPath, DWORD dwOptions);
	STDMETHODIMP_(LPTSTR) GetQuotedPath(LPCTSTR lpPath);
	STDMETHODIMP CreateDirectoryTree(LPCTSTR lpDirPath, DWORD dwOptions);

	STDMETHODIMP FlushStorageBuffers(LPCTSTR lpFileOnStorage, BOOL bOnlyIfRemovable);
	STDMETHODIMP SecureDeleteFile(LPCTSTR lpFilePath);
	STDMETHODIMP WriteFile(LPCTSTR lpFilePath, const BYTE* pData, DWORD dwDataSize);

	STDMETHODIMP_(INT) CompareTimes(const PW_TIME* pTime1, const PW_TIME* pTime2);

	STDMETHODIMP HashFileSHA256(LPCTSTR lpFile, BYTE* pHashBuf);

	STDMETHODIMP_(DWORD) EstimatePasswordBits(LPCTSTR lpPassword);

	STDMETHODIMP_(BOOL) IsTANEntry(const PW_ENTRY* pEntry);

	STDMETHODIMP SHA256CreateContext(void** pOutNewContext);
	STDMETHODIMP SHA256Init(void* pContext);
	STDMETHODIMP SHA256Hash(void* pContext, const BYTE* pData, DWORD dwDataLength);
	STDMETHODIMP SHA256Final(void* pContext, BYTE* pOutHashBuf);

	STDMETHODIMP EncryptMemory(BYTE* pbBuf, DWORD dwBufLen, const BYTE* pbKey,
		DWORD dwKeyLen);
	STDMETHODIMP DecryptMemory(BYTE* pbBuf, DWORD dwBufLen, const BYTE* pbKey,
		DWORD dwKeyLen);

private:
	KP_DECL_STDREFIMPL;
};

#pragma pack()

#endif // ___KPUTILITIESIMPL_H___
