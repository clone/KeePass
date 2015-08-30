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

#ifndef ___STR_UTIL_EX_H___
#define ___STR_UTIL_EX_H___

#pragma once

#include "../SysDefEx.h"

#define BOOST_REGEX_NON_RECURSIVE
#include <boost/regex.hpp>

#include "../PwManager.h"

#define UTF8_BYTE BYTE

#define METAMODE_NULL     0
#define METAMODE_AUTOTYPE 1

#define CLIPBOARD_DELAYED_NONE     0
#define CLIPBOARD_DELAYED_PASSWORD 1
#define CLIPBOARD_DELAYED_USERNAME 2
#define CLIPBOARD_DELAYED_USERPASS 3

#define PRL_MAX_DEPTH 10

typedef BOOL(WINAPI *LPPATHRELATIVEPATHTO)(LPTSTR pszPath, LPCTSTR pszFrom, DWORD dwAttrFrom, LPCTSTR pszTo, DWORD dwAttrTo);

void EraseCString(CString *pString);
void EraseWCharVector(std::vector<WCHAR>& vBuffer);
void EraseTCharVector(std::vector<TCHAR>& vBuffer);

// Fix an URL if necessary (check protocol, form, etc.)
void FixURL(CString *pstrURL);

// If pReferenceSource is not NULL, it'll be used to dereference
// lpReplaceWith before replacing lpFind
// BOOL SeqReplace(CString& str, LPCTSTR lpFind, LPCTSTR lpReplaceWith,
//	BOOL bMakeSimString, BOOL bCmdQuotes, BOOL bRemoveMeta, PW_ENTRY* peEntryInfo,
//	CPwManager* pReferenceSource, DWORD dwRecursionLevel);

// Replace placeholders in pString by data in pEntry
// void ParseURL(CString *pString, PW_ENTRY *pEntry, BOOL bMakeSimString, BOOL bCmdQuotes,
//	CPwManager* pDataSource, DWORD dwRecursionLevel);

// BOOL FillRefPlaceholders(CString& str, BOOL bMakeSimString, BOOL bCmdQuotes,
//	CPwManager* pDataSource, DWORD dwRecursionLevel);

CString CsRemoveMeta(CString *psString);

// String conversion functions
char *_StringToAnsi(const WCHAR *lptString);
WCHAR *_StringToUnicode(const char *pszString);

// Convert UCS-2 to UTF-8 and the other way round
// (must be exported)
C_FN_SHARE UTF8_BYTE *_StringToUTF8(const TCHAR *pszSourceString);
C_FN_SHARE TCHAR *_UTF8ToString(const UTF8_BYTE *pUTF8String);

C_FN_SHARE DWORD _UTF8NumChars(const UTF8_BYTE *pUTF8String);

// This returns the needed bytes to represent the string, without terminating NULL character
C_FN_SHARE DWORD _UTF8BytesNeeded(const TCHAR *pszString);

// Must be exported:
C_FN_SHARE BOOL _IsUTF8String(const UTF8_BYTE *pUTF8String);

// Convert PW_TIME structure to a CString
void _PwTimeToString(PW_TIME t, CString *pstrDest);
void _PwTimeToStringEx(const PW_TIME& t, CString& strDest, BOOL bUseLocalFormat);

void _PwTimeToXmlTime(PW_TIME t, CString *pstrDest);

// Convert UUID to string
void _UuidToString(const BYTE *pUuid, CString *pstrDest);

// Must be exported:
C_FN_SHARE void _StringToUuid(const TCHAR *ptszSource, BYTE *pUuid);

// Get the filename of the file in psFilePath
CString CsFileOnly(const CString *psFilePath);

// Convert ptString into a XML string
TCHAR *MakeSafeXmlString(const TCHAR *ptString);

// Our own simple string functions which do some additional memory checks
DWORD szlen(const char *pszString);
char *szcpy(char *szDestination, const char *szSource);

CString ExtractParameterFromString(LPCTSTR lpstr, LPCTSTR lpStart, DWORD dwInstance);

// C_FN_SHARE void _GetPathFromFile(TCHAR *pszFile, TCHAR *pszPath);

// Allocate enough memory and clone the parameter string
TCHAR *_TcsSafeDupAlloc(const TCHAR *tszSource);

void RemoveAcceleratorTip(CString *pString);

// Assumes that lpSearch is lower-case when bCaseSensitive == FALSE
// If pUseRegex is not NULL, a regular expression search will be
// performed, otherwise a simple substring matching.
bool StrMatchText(LPCTSTR lpEntryData, LPCTSTR lpSearch,
	BOOL bCaseSensitive, const boost::basic_regex<TCHAR>* pUseRegex);

std::basic_string<TCHAR> GetQuotedAppPath(const std::basic_string<TCHAR>& strPath);

/////////////////////////////////////////////////////////////////////////////
// Natural string comparison API

void NSCAPI_Initialize();
bool NSCAPI_Supported();
void NSCAPI_Exit();

typedef int(*LPCTSTRCMPEX)(LPCTSTR x, LPCTSTR y);
typedef int(WINAPI *LPCWSTRCMPEX)(LPCWSTR x, LPCWSTR y);

int StrCmpNaturalEx(LPCTSTR x, LPCTSTR y);
LPCTSTRCMPEX StrCmpGetNaturalMethodOrFallback();

/////////////////////////////////////////////////////////////////////////////
// Wide character stream

class WCharStream : boost::noncopyable
{
public:
	WCharStream(LPCWSTR lpData);

	WCHAR ReadChar();

private:
	LPCWSTR m_lpData;
	DWORD m_dwPosition;
};

/////////////////////////////////////////////////////////////////////////////
// Wide character stream

class CStringBuilderEx : boost::noncopyable
{
public:
	CStringBuilderEx();

	void Append(TCHAR tch);
	void Append(LPCTSTR lpString);

	std::basic_string<TCHAR> ToString() const;

	CStringBuilderEx& operator+=(TCHAR tch) { this->Append(tch); return *this; }
	CStringBuilderEx& operator+=(LPCTSTR lp) { this->Append(lp); return *this; }

private:
	std::vector<TCHAR> m_vBuf;
};

#endif
