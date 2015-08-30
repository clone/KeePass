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

#ifndef ___STR_UTIL_EX_H___
#define ___STR_UTIL_EX_H___

#include "../PwManager.h"

#define UTF8_BYTE BYTE

#define METAMODE_NULL     0
#define METAMODE_AUTOTYPE 1

#define CLIPBOARD_DELAYED_NONE     0
#define CLIPBOARD_DELAYED_PASSWORD 1
#define CLIPBOARD_DELAYED_USERNAME 2
#define CLIPBOARD_DELAYED_USERPASS 3

typedef BOOL(WINAPI *LPPATHRELATIVEPATHTO)(LPTSTR pszPath, LPCTSTR pszFrom, DWORD dwAttrFrom, LPCTSTR pszTo, DWORD dwAttrTo);

CPP_FN_SHARE void EraseCString(CString *pString);
void EraseWCharVector(std::vector<WCHAR>& vBuffer);
void EraseTCharVector(std::vector<TCHAR>& vBuffer);

// Fix an URL if necessary (check protocol, form, etc.)
CPP_FN_SHARE void FixURL(CString *pstrURL);

// Replace placeholders in pString by data in pEntry
CPP_FN_SHARE void ParseURL(CString *pString, PW_ENTRY *pEntry, BOOL bMakeSimString, BOOL bCmdQuotes);

CPP_FN_SHARE CString CsRemoveMeta(CString *psString);

// String conversion functions
C_FN_SHARE char *_StringToAnsi(const WCHAR *lptString);
C_FN_SHARE WCHAR *_StringToUnicode(const char *pszString);

// Convert UCS-2 to UTF-8 and the other way round
C_FN_SHARE UTF8_BYTE *_StringToUTF8(const TCHAR *pszSourceString);
C_FN_SHARE TCHAR *_UTF8ToString(const UTF8_BYTE *pUTF8String);

C_FN_SHARE DWORD _UTF8NumChars(const UTF8_BYTE *pUTF8String);

// This returns the needed bytes to represent the string, without terminating NULL character
C_FN_SHARE DWORD _UTF8BytesNeeded(const TCHAR *pszString);

C_FN_SHARE BOOL _IsUTF8String(const UTF8_BYTE *pUTF8String);

// Convert PW_TIME structure to a CString
CPP_FN_SHARE void _PwTimeToString(PW_TIME t, CString *pstrDest);
void _PwTimeToStringEx(const PW_TIME& t, CString& strDest, BOOL bUseLocalFormat);

CPP_FN_SHARE void _PwTimeToXmlTime(PW_TIME t, CString *pstrDest);

// Convert UUID to string and the other way round
CPP_FN_SHARE void _UuidToString(const BYTE *pUuid, CString *pstrDest);
C_FN_SHARE void _StringToUuid(const TCHAR *ptszSource, BYTE *pUuid);

// Get the filename of the file in psFilePath
CPP_FN_SHARE CString CsFileOnly(const CString *psFilePath);

// Convert ptString into a XML string
C_FN_SHARE TCHAR *MakeSafeXmlString(const TCHAR *ptString);

// Our own simple string functions which do some additional memory checks
C_FN_SHARE DWORD szlen(const char *pszString);
C_FN_SHARE char *szcpy(char *szDestination, const char *szSource);

CPP_FN_SHARE CString ExtractParameterFromString(LPCTSTR lpstr, LPCTSTR lpStart, DWORD dwInstance);
CPP_FN_SHARE CString TagSimString(LPCTSTR lpString);

// C_FN_SHARE void _GetPathFromFile(TCHAR *pszFile, TCHAR *pszPath);

// Allocate enough memory and clone the parameter string
CPP_FN_SHARE TCHAR *_TcsSafeDupAlloc(const TCHAR *tszSource);

CPP_FN_SHARE void RemoveAcceleratorTip(CString *pString);

class WCharStream
{
public:
	WCharStream(LPCWSTR lpData);

	WCHAR ReadChar();

private:
	LPCWSTR m_lpData;
	DWORD m_dwPosition;
};

#endif
