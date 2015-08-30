/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2008 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___PW_UTIL_H___
#define ___PW_UTIL_H___

#pragma once

#include "../SysDefEx.h"
#include <string>
#include <boost/utility.hpp>

#include "../PwManager.h"

class CPwUtil : boost::noncopyable
{
private:
	CPwUtil();

public:
	// Very simple password quality estimation function
	static DWORD EstimatePasswordBits(LPCTSTR pszPassword);

	static BOOL LoadHexKey32(FILE *fp, BYTE *pBuf);
	static BOOL SaveHexKey32(FILE *fp, BYTE *pBuf);

	static CString FormatError(int nErrorCode, DWORD dwFlags);

	static BOOL MemAllocCopyEntry(__in_ecount(1) const PW_ENTRY *pExisting,
		__out_ecount(1) PW_ENTRY *pDestination);
	static void MemFreeEntry(__inout_ecount(1) PW_ENTRY *pEntry);

	// Convert PW_TIME to 5-byte compressed structure and the other way round
	static void TimeToPwTime(__in_ecount(5) const BYTE *pCompressedTime,
		__out_ecount(1) PW_TIME *pPwTime);
	static void PwTimeToTime(__in_ecount(1) const PW_TIME *pPwTime,
		__out_ecount(5) BYTE *pCompressedTime);

	static BOOL AttachFileAsBinaryData(__inout_ecount(1) PW_ENTRY *pEntry,
		const TCHAR *lpFile);
	static BOOL SaveBinaryData(__in_ecount(1) const PW_ENTRY *pEntry,
		const TCHAR *lpFile);
	static BOOL RemoveBinaryData(__inout_ecount(1) PW_ENTRY *pEntry);

	static BOOL IsAllowedStoreGroup(LPCTSTR lpGroupName, LPCTSTR lpSearchGroupName);

	static BOOL IsZeroUUID(__in_ecount(16) const BYTE *pUUID);

	static BOOL IsTANEntry(const PW_ENTRY *pe);

private:
	inline static BOOL ConvertStrToHex(char ch1, char ch2, BYTE& bt);
	inline static void ConvertHexToStr(BYTE bt, char& ch1, char& ch2);
};

/* class CPwErrorInfo
{
public:
	CPwErrorInfo();
	void SetInfo(const std::basic_string<TCHAR>& strText, int nPwErrorCode,
		BOOL bUseLastError);

	std::basic_string<TCHAR> ToString() const;

private:
	void FormatInfo();

	std::basic_string<TCHAR> m_strText;
	int m_nPwErrorCode;
	DWORD m_dwLastError;

	std::basic_string<TCHAR> m_strFinal;
}; */

#endif // ___PW_UTIL_H___
