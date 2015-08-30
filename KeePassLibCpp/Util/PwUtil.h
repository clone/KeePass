/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2007 Dominik Reichl <dominik.reichl@t-online.de>

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

#include "../SysDefEx.h"
#include <string>

// Very simple password quality estimation function
DWORD EstimatePasswordBits(LPCTSTR pszPassword);

BOOL LoadHexKey32(FILE *fp, BYTE *pBuf);
BOOL SaveHexKey32(FILE *fp, BYTE *pBuf);

BOOL ConvertStrToHexEx(char ch1, char ch2, BYTE& bt);
void ConvertHexToStrEx(BYTE bt, char& ch1, char& ch2);

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

CString PWM_FormatStaticError(int nErrorCode, DWORD dwFlags);

#endif // ___PW_UTIL_H___
