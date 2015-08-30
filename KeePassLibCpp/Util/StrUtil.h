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

#ifndef ___STR_UTIL_EX_H___
#define ___STR_UTIL_EX_H___

#pragma once

#include "../SysDefEx.h"

#define BOOST_REGEX_NON_RECURSIVE
#include <boost/regex.hpp>

#include "../PwManager.h"

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
char* _StringToAnsi(const WCHAR* lptString);
WCHAR* _StringToUnicode(const char* pszString);

std::basic_string<WCHAR> _StringToUnicodeStl(const TCHAR *pszString);

// Convert UCS-2 to UTF-8 and the other way round
UTF8_BYTE *_StringToUTF8(const TCHAR *pszSourceString);
TCHAR *_UTF8ToString(const UTF8_BYTE *pUTF8String);

DWORD _UTF8NumChars(const UTF8_BYTE *pUTF8String);

// This returns the needed bytes to represent the string, without terminating NULL character
DWORD _UTF8BytesNeeded(const TCHAR *pszString);

BOOL _IsUTF8String(const UTF8_BYTE *pUTF8String);

// Convert PW_TIME structure to a CString
void _PwTimeToString(PW_TIME t, CString *pstrDest);
void _PwTimeToStringEx(const PW_TIME& t, CString& strDest, BOOL bUseLocalFormat);

void _PwTimeToXmlTime(PW_TIME t, CString *pstrDest);

// Convert UUID to string
void _UuidToString(const BYTE *pUuid, CString *pstrDest);

void _StringToUuid(const TCHAR *ptszSource, BYTE *pUuid);

// Get the filename of the file in psFilePath
CString CsFileOnly(const CString *psFilePath);

// Convert ptString into a XML string
TCHAR *MakeSafeXmlString(const TCHAR *ptString);

// Our own simple string functions which do some additional memory checks
DWORD szlen(const char *pszString);
char *szcpy(char *szDestination, const char *szSource);

CString ExtractParameterFromString(LPCTSTR lpstr, LPCTSTR lpStart, DWORD dwInstance);

// void _GetPathFromFile(TCHAR *pszFile, TCHAR *pszPath);

// Allocate enough memory and clone the parameter string
TCHAR *_TcsSafeDupAlloc(const TCHAR *tszSource);

// Allocate enough memory and clone the parameter string,
// suitable for memory protection using DPAPI
LPTSTR _TcsCryptDupAlloc(LPCTSTR lpSource);

void RemoveAcceleratorTip(CString *pString);
CString RemoveAcceleratorTipEx(LPCTSTR lpString);

// Assumes that lpSearch is lower-case when bCaseSensitive == FALSE
// If pUseRegex is not NULL, a regular expression search will be
// performed, otherwise a simple substring matching.
bool StrMatchText(LPCTSTR lpEntryData, LPCTSTR lpSearch,
	BOOL bCaseSensitive, const boost::basic_regex<TCHAR>* pUseRegex);

std::vector<std::basic_string<TCHAR> > SU_SplitSearchTerms(LPCTSTR lpSearch);

std::basic_string<TCHAR> SU_GetQuotedPath(const std::basic_string<TCHAR>& strPath);

CString SU_ConvertNewLines(LPCTSTR lpText, LPCTSTR lpNewLineSeq);

void SU_Split(std::vector<std::basic_string<TCHAR>>& vOut, const std::basic_string<TCHAR>& strData,
	LPCTSTR lpSplitChars);

std::basic_string<TCHAR> SU_CompactWith3Dots(LPCTSTR lpString, size_t uMaxChars);

int SU_FindUnescapedCharW(LPCWSTR lpw, WCHAR wch);

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
// CStringBuilderEx class

class CStringBuilderEx : boost::noncopyable
{
public:
	CStringBuilderEx();

	void Clear() { m_vBuf.clear(); }

	void Append(TCHAR tch);
	void Append(LPCTSTR lpString);

	size_t GetLength() const { return m_vBuf.size(); }

	std::basic_string<TCHAR> ToString() const;

	CStringBuilderEx& operator+=(TCHAR tch) { this->Append(tch); return *this; }
	CStringBuilderEx& operator+=(LPCTSTR lp) { this->Append(lp); return *this; }

private:
	std::vector<TCHAR> m_vBuf;
};

/////////////////////////////////////////////////////////////////////////////
// Set of strings

class CStringSetEx : boost::noncopyable
{
public:
	CStringSetEx();
	virtual ~CStringSetEx();

	void Clear();

	LPCTSTR Add(LPCTSTR lpString);

private:
	std::vector<LPTSTR> m_vStrings;
};

#endif
