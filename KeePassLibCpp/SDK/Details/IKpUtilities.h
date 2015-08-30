/*
  Copyright (c) 2008-2012, Dominik Reichl
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
	the documentation and/or other materials provided with the
	distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef ___IKPUTILITIES_H___
#define ___IKPUTILITIES_H___

#pragma once

#include "../../SysDefEx.h"
#include "IKpUnknown.h"

#pragma pack(1)

/// Interface to an object providing various utility methods.
/// Use IKpAPI::QueryInstance to retrieve an interface to a singleton
/// object in KeePass providing utility methods.
struct KP_DECL_INTERFACE("9700114E-9178-4337-A26F-34A6DA91FBB4") IKpUtilities :
	public IKpUnknown
{
public:
	STDMETHOD(ShowFileDialog)(BOOL bOpenMode, LPCTSTR lpSuffix, LPTSTR lpStoreBuf,
		DWORD dwBufLen) = 0;

	/// Convert a Unicode to a multi-byte string. @return Multi-byte string. @kpdeletestring
	STDMETHOD_(char*, UnicodeToMultiByte)(const WCHAR* lpwString) = 0;
	/// Convert a multi-byte to a Unicode string. @return Unicode string. @kpdeletestring
	STDMETHOD_(WCHAR*, MultiByteToUnicode)(const char* lpString) = 0;

	/// Convert a string to UTF-8. @return UTF-8 string. @kpdeletestring
	STDMETHOD_(UTF8_BYTE*, StringToUTF8)(LPCTSTR lpSourceString) = 0;
	STDMETHOD_(DWORD, UTF8NumChars)(const UTF8_BYTE* pUTF8String) = 0;
	STDMETHOD_(DWORD, UTF8BytesNeeded)(LPCTSTR lpString) = 0;
	/// Convert a UTF-8 string to a TCHAR string. @return TCHAR string. @kpdeletestring
	STDMETHOD_(LPTSTR, UTF8ToString)(const UTF8_BYTE* pUTF8String) = 0;
	STDMETHOD_(BOOL, IsUTF8String)(const UTF8_BYTE* pUTF8String) = 0;

	STDMETHOD(UuidToString)(const BYTE* pUuid, LPTSTR lpOutBuf) = 0;
	STDMETHOD(StringToUuid)(LPCTSTR lpSource, BYTE* pUuid) = 0;

	STDMETHOD_(INT, ShellOpenLocalFile)(LPCTSTR lpFile, INT nMode) = 0;
	STDMETHOD(OpenUrl)(LPCTSTR lpURL, HWND hParent) = 0;
	STDMETHOD(OpenAppHelp)(LPCTSTR lpTopicFile) = 0;

	STDMETHOD(Base64Encode)(const BYTE* pbIn, DWORD cbInLen, BYTE* pbOut, DWORD* pcbOutLen) = 0;
	STDMETHOD(Base64Decode)(const BYTE* pbIn, DWORD cbInLen, BYTE* pbOut, DWORD* pcbOutLen) = 0;

	STDMETHOD(GetApplicationDirectory)(LPTSTR lpStoreBuf, DWORD dwBufLen,
		BOOL bFilterSpecial, BOOL bMakeURL) = 0;

	/// Create a relative path from a given file and a base path. @return Relative path. @kpdeletestring
	STDMETHOD_(LPTSTR, MakeRelativePath)(LPCTSTR lpBaseFile, LPCTSTR lpTargetFile) = 0;

	/// Compact a path (evaluate navigations like two dots, etc). @return Absolute path. @kpdeletestring
	STDMETHOD_(LPTSTR, GetShortestAbsolutePath)(LPCTSTR lpFilePath) = 0;

	STDMETHOD_(BOOL, IsAbsolutePath)(LPCTSTR lpPath) = 0;

	/// Validate a path string.
	/// @param lpPath Path string to validate.
	/// @param dwOptions Validation options flags:
	/// - 0 or KPVPF_DEFAULT. The default validation options. Only valid if specified alone.
	/// - KPVPF_MUST_EXIST. The specified file or directory must exist. If it doesn't exist,
	///   the return value is FALSE.
	/// - KPVPF_TYPE_DIRECTORY. Only valid if KPVPF_MUST_EXIST is also specified. Checks
	///   whether the specified path is a directory or not (will return FALSE if it's a file).
	/// - KPVPF_TYPE_FILE. Only valid if KPVPF_MUST_EXIST is also specified. Checks whether
	///   the specified path is a file or not (will return FALSE if it's a directory).
	/// - KPVPF_REGULAR_NAME. Checks whether or not the given path string contains invalid
	///   characters like '*', '?', '/' and others.
	STDMETHOD_(BOOL, ValidatePath)(LPCTSTR lpPath, DWORD dwOptions) = 0;

	/// Filter quotes of a path. @return Unquoted path. @kpdeletestring
	STDMETHOD_(LPTSTR, GetQuotedPath)(LPCTSTR lpPath) = 0;

	/// Create a directory tree (the specified directory and all intermediate directories).
	/// @param lpDirPath Directory path.
	/// @param dwOptions Reserved for future use, set it to 0.
	/// @return S_OK, if successful.
	/// @return A COM error code (E_XXX), if unsuccessful.
	STDMETHOD(CreateDirectoryTree)(LPCTSTR lpDirPath, DWORD dwOptions) = 0;

	/// Try to flush all write buffers to the device containing the specified file/directory.
	/// If the user doesn't have administrative rights, this might fail.
	STDMETHOD(FlushStorageBuffers)(LPCTSTR lpFileOnStorage, BOOL bOnlyIfRemovable) = 0;

	/// Securely delete a file (overwrite multiple times before unlinking from file system).
	STDMETHOD(SecureDeleteFile)(LPCTSTR lpFilePath) = 0;

	STDMETHOD(WriteFile)(LPCTSTR lpFilePath, const BYTE* pData, DWORD dwDataSize) = 0;

	STDMETHOD_(INT, CompareTimes)(const PW_TIME* pTime1, const PW_TIME* pTime2) = 0;

	STDMETHOD_(DWORD, EstimatePasswordBits)(LPCTSTR lpPassword) = 0;

	STDMETHOD_(BOOL, IsTANEntry)(const PW_ENTRY* pEntry) = 0;

	STDMETHOD(HashFileSHA256)(LPCTSTR lpFile, BYTE* pHashBuf) = 0;

	/// Create a SHA-256 hash context.
	/// The context pointer stored in pOutNewContext can be passed to the
	/// other SHA256XXX methods.
	/// It must be deleted using IKpAPI::DeleteObject. 
	/// The hash context is already initialized, you do not need to call
	/// IKpUtilities::SHA256Init.
	/// @see SHA256Init
	STDMETHOD(SHA256CreateContext)(void** pOutNewContext) = 0;

	/// Initialize a hash context.
	/// Calling this method is only required if you want to use a hash
	/// context multiple times (immediately call it after the
	/// IKpUtilities::SHA256Final method).
	STDMETHOD(SHA256Init)(void* pContext) = 0;

	/// Hash data.
	/// Note this method can be called multiple times consecutively to
	/// hash large data step by step.
	STDMETHOD(SHA256Hash)(void* pContext, const BYTE* pData, DWORD dwDataLength) = 0;

	/// Finalize a hash.
	/// Finish the hashing process and store the final hash value in pOutHashBuf.
	/// Note that you still have to delete the hash context using IKpAPI::DeleteObject.
	/// @param pOutHashBuf Address of a buffer that will receive the hash value.
	/// Must be able to hold at least 32 bytes.
	STDMETHOD(SHA256Final)(void* pContext, BYTE* pOutHashBuf) = 0;

	/// Encrypt a memory block.
	/// This method is only intended for protecting a memory block in order
	/// to make it unreadable by other processes. It is not guaranteed that all
	/// KeePass versions use the same algorithm, therefore do not store data
	/// encrypted with this method.
	/// @see DecryptMemory
	STDMETHOD(EncryptMemory)(BYTE* pbBuf, DWORD dwBufLen, const BYTE* pbKey, DWORD dwKeyLen) = 0;

	/// Decrypt a memory block.
	/// @see EncryptMemory
	STDMETHOD(DecryptMemory)(BYTE* pbBuf, DWORD dwBufLen, const BYTE* pbKey, DWORD dwKeyLen) = 0;
};

#pragma pack()

#endif // ___IKPUTILITIES_H___
