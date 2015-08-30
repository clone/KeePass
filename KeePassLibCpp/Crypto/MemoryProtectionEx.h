/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2011 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___MEM_PROTECTION_UTIL_H___
#define ___MEM_PROTECTION_UTIL_H___

#pragma once

#include <wincrypt.h>
#include <boost/utility.hpp>

#define MPU_CRYPTLIB_NAME    _T("Crypt32.dll")

#ifndef _WIN32_WCE
#define MPU_CRYPTPROT_NAME   "CryptProtectMemory"
#define MPU_CRYPTUNPROT_NAME "CryptUnprotectMemory"
#else
#define MPU_CRYPTPROT_NAME   _T("CryptProtectMemory")
#define MPU_CRYPTUNPROT_NAME _T("CryptUnprotectMemory")
#endif

typedef BOOL(WINAPI *LPFNCRYPTPROTECTMEMORY)(LPVOID pData, DWORD cbData, DWORD dwFlags);
typedef BOOL(WINAPI *LPFNCRYPTUNPROTECTMEMORY)(LPVOID pData, DWORD cbData, DWORD dwFlags);

class CMemoryProtectionEx : boost::noncopyable
{
public:
	static void SetEnabled(BOOL bEnabled);
	static void SetEnabledAtStart(BOOL bEnabled);
	static BOOL* GetEnabledPtr();

	static void Release(); // Call at application shutdown

	static DWORD ToBlockSize(DWORD dwByteCount);

	static HRESULT EncryptMemory(LPVOID lpBuffer, DWORD cbSize);
	static HRESULT DecryptMemory(LPVOID lpBuffer, DWORD cbSize);

	static HRESULT EncryptText(LPTSTR lpText, DWORD dwCharacters);
	static HRESULT DecryptText(LPTSTR lpText, DWORD dwCharacters);

	// static void Benchmark();

private:
	CMemoryProtectionEx();
	~CMemoryProtectionEx();

	static void LoadCryptLib();
	static void FreeCryptLib();

	static BOOL m_bEnabled;
	static BOOL m_bEnabledAtStart;

	static bool m_bSupportTested;
	static HMODULE m_hCryptLib;
	static LPFNCRYPTPROTECTMEMORY m_lpCryptProtectMemory;
	static LPFNCRYPTUNPROTECTMEMORY m_lpCryptUnprotectMemory;
};

#endif // ___MEM_PROTECTION_UTIL_H___
