/*
  Copyright (c) 2003/2004, Dominik Reichl <dominik.reichl@t-online.de>
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer. 
  - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  - Neither the name of ReichlSoft nor the names of its contributors may be
    used to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef ___STR_UTIL_EX_H___
#define ___STR_UTIL_EX_H___

#include "../StdAfx.h"
#include "../PwSafe/PwManager.h"

#define UTF8_BYTE BYTE

typedef BOOL(WINAPI *LPPATHRELATIVEPATHTO)(LPTSTR pszPath, LPCTSTR pszFrom, DWORD dwAttrFrom, LPCTSTR pszTo, DWORD dwAttrTo);
typedef HWND(WINAPI *LPHTMLHELP)(HWND hwndCaller, LPCSTR pszFile, UINT uCommand, DWORD dwData); 

void EraseCString(CString *pString);

#ifndef _WIN32_WCE
void CopyStringToClipboard(const TCHAR *lptString);
void ClearClipboardIfOwner();
#endif

// Fix an URL if necessary (check protocol, form, etc.)
void FixURL(CString *pstrURL);

// Replace placeholders in pString by data in pEntry
void ParseURL(CString *pString, PW_ENTRY *pEntry, BOOL bMakeSimString = FALSE);

// String conversion functions
char *_StringToAnsi(const TCHAR *lptString);
TCHAR *_StringToUnicode(const char *pszString);

// Convert UCS-2 to UTF-8 and the other way round
UTF8_BYTE *_StringToUTF8(const TCHAR *pszSourceString);
TCHAR *_UTF8ToString(const UTF8_BYTE *pUTF8String);

DWORD _UTF8NumChars(const UTF8_BYTE *pUTF8String);

// This returns the needed bytes to represent the string, without terminating NULL character
DWORD _UTF8BytesNeeded(const TCHAR *pszString);

// Convert PW_TIME structure to a CString
void _PwTimeToString(PW_TIME t, CString *pstrDest);
void _PwTimeToXmlTime(PW_TIME t, CString *pstrDest);

// Convert UUID to string and the other way round
void _UuidToString(const BYTE *pUuid, CString *pstrDest);
void _StringToUuid(const TCHAR *ptszSource, BYTE *pUuid);

// Get the filename of the file in psFilePath
CString CsFileOnly(CString *psFilePath);

// Convert ptString into a XML string
TCHAR *MakeSafeXmlString(TCHAR *ptString);

// Our own simple string functions which do some additional memory checks
size_t szlen(const char *pszString);
char *szcpy(char *szDestination, const char *szSource);

CString MakeRelativePathEx(LPCTSTR lpBaseFile, LPCTSTR lpTargetFile);

BOOL GetRegKeyEx(HKEY hkey, LPCTSTR lpSubKey, LPTSTR lpRetData);
BOOL OpenUrlInNewBrowser(LPCTSTR lpURL);
BOOL OpenUrlUsingPutty(LPCTSTR lpURL, LPCTSTR lpUser);

void OpenUrlEx(LPCTSTR lpURL);

CString ExtractAutoTypeCmd(LPCTSTR lpstr);
CString TagSimString(LPCTSTR lpString);

#endif
