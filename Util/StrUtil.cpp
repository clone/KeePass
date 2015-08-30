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

#include "StdAfx.h"
#include "MemUtil.h"
#include "StrUtil.h"
#include "../Crypto/sha2.h"

static unsigned char g_shaLastString[32];

void EraseCString(CString *pString)
{
	int j, len;
	LPTSTR lpt;
	const TCHAR tcPlus = ' ';
	const TCHAR tcMod = '}' - tcPlus;

	ASSERT(pString != NULL);

	len = pString->GetLength();
	lpt = pString->GetBuffer(0);

	for(j = 0; j < len; j++)
	{
		lpt[j] = (TCHAR)((rand() % tcMod) + tcPlus);
	}

	pString->Empty();
	pString->FreeExtra();
}

#ifndef _WIN32_WCE
void CopyStringToClipboard(const TCHAR *lptString)
{
	unsigned long uDataSize;
	HGLOBAL globalHandle;
	LPVOID globalData;

	if(OpenClipboard(NULL) == FALSE) return;

	if(EmptyClipboard() == FALSE) return;

	if(lptString == NULL) // No string to copy => empty clipboard
	{
		CloseClipboard();
		return;
	}

	uDataSize = _tcslen(lptString) * sizeof(TCHAR); // Get length
	if(uDataSize == 0)
	{
		CloseClipboard();
		return;
	}
	uDataSize += sizeof(TCHAR); // Plus NULL-terminator of string

	globalHandle = GlobalAlloc(GHND | GMEM_DDESHARE, uDataSize);
	globalData = GlobalLock(globalHandle);
	_tcscpy((TCHAR *)globalData, lptString); // Copy string plus NULL-byte to global memory
	GlobalUnlock(globalHandle); // Unlock before SetClipboardData!

	VERIFY(SetClipboardData(CF_TEXT, globalHandle)); // Set clipboard data to our global memory block
	VERIFY(CloseClipboard()); // Close clipboard, and done

	sha256_ctx shactx;
	sha256_begin(&shactx);
	sha256_hash((unsigned char *)lptString, uDataSize - sizeof(TCHAR), &shactx);
	sha256_end(g_shaLastString, &shactx);
}

void ClearClipboardIfOwner()
{
	if(OpenClipboard(NULL) == FALSE) return;

	if((IsClipboardFormatAvailable(CF_TEXT) == FALSE) &&
		(IsClipboardFormatAvailable(CF_OEMTEXT) == FALSE))
	{
		CloseClipboard();
		return;
	}

	HANDLE hClipboardData = GetClipboardData(CF_TEXT);
	TCHAR *lpString = (TCHAR *)GlobalLock(hClipboardData);

	sha256_ctx shactx;
	unsigned char uHash[32];
	sha256_begin(&shactx);
	sha256_hash((unsigned char *)lpString, _tcslen(lpString) * sizeof(TCHAR), &shactx);
	sha256_end(uHash, &shactx);

	GlobalUnlock(hClipboardData);

	// If we have copied the string to the clipboard, delete it
	if(memcmp(uHash, g_shaLastString, 32) == 0)
	{
		VERIFY(EmptyClipboard());
	}

	VERIFY(CloseClipboard());
}
#endif

void FixURL(CString *pstrURL)
{
	CString strTemp;
	BOOL bPre = FALSE;

	ASSERT(pstrURL != NULL);

	// Load string and make lower
	strTemp = *pstrURL;
	strTemp.MakeLower();

	// If the string begins with one of the following prefixes it is an URL
	if(strTemp.Left(5) == "file:") bPre = TRUE;
	if(strTemp.Left(4) == "ftp:") bPre = TRUE;
	if(strTemp.Left(7) == "gopher:") bPre = TRUE;
	if(strTemp.Left(5) == "http:") bPre = TRUE;
	if(strTemp.Left(6) == "https:") bPre = TRUE;
	if(strTemp.Left(7) == "mailto:") bPre = TRUE;
	if(strTemp.Left(5) == "news:") bPre = TRUE;
	if(strTemp.Left(5) == "nntp:") bPre = TRUE;
	if(strTemp.Left(9) == "prospero:") bPre = TRUE;
	if(strTemp.Left(7) == "telnet:") bPre = TRUE;
	if(strTemp.Left(5) == "wais:") bPre = TRUE;
	if(strTemp.Left(4) == "cmd:") bPre = TRUE;

	if(bPre == FALSE) // The string isn't a valid URL, so assume it's a HTTP
	{
		strTemp = "http:";
		if(pstrURL->Left(1) != "/") strTemp += "//";
		strTemp += *pstrURL;

		*pstrURL = strTemp;
	}
}

char *_StringToAnsi(const TCHAR *lptString)
{
	char *p = NULL;
	int _nChars = 0;

	if(lptString == NULL) { ASSERT(FALSE); return NULL; }

#ifdef _UNICODE
	_nChars = lstrlen(lptString) + 1;
	p = new char[_nChars * 2 + 1];
	p[0] = 0;
	VERIFY(WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, lptString, -1, p, _nChars, NULL, NULL) !=
		ERROR_INSUFFICIENT_BUFFER);
#else
	_nChars = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, (LPCWSTR) lptString, -1, NULL, 0, NULL, NULL);
	p = new char[_nChars * 2 + 1];
	p[0] = 0;
	VERIFY(WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, (LPCWSTR) lptString, -1, p, _nChars, NULL, NULL) !=
		ERROR_INSUFFICIENT_BUFFER);
#endif

	return p;
}

TCHAR *_StringToUnicode(const char *pszString)
{
	int _nChars = 0;

	if(pszString == NULL) { ASSERT(FALSE); return NULL; }

	WCHAR *p;

#ifdef _UNICODE
	// Determine the correct buffer size by calling the function itself with 0 as buffer size (see docs)
	_nChars = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszString, -1, NULL, 0);

	p = new WCHAR[_nChars + 1];
	p[0] = 0;

	// Jan 9th 2004: DonAngel {
	// This was ASSERTing for string. All debugging did not given good results, so I decided to remove
	// the verification. This could be a bug in MultiByteToWideChar, because thou it was returning
	// ERROR_INSUFFICIENT_BUFFER, the convertion was OK!?
	// The problem should be investigated later, but for now - I prefer to remove the ASSERT
	// VERIFY(MultiByteToWideChar(CP_ACP, 0, pszString, -1, p, _nChars) !=
	//	ERROR_INSUFFICIENT_BUFFER);
	// Jan 9th 2004: DonAngel }

	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszString, -1, p, _nChars);
#else
	_nChars = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszString, -1, NULL, 0);
	p = new WCHAR[_nChars *2 + 1];
	p[0] = 0; p[1] = 0;
	VERIFY(MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszString, -1, (LPWSTR) p, _nChars) !=
		ERROR_INSUFFICIENT_BUFFER);
#endif

	return (TCHAR *)p;
}

void _PwTimeToString(PW_TIME t, CString *pstrDest)
{
	ASSERT(pstrDest != NULL);
	pstrDest->Empty();
	pstrDest->Format(_T("%04u/%02u/%02u %02u:%02u:%02u"), t.shYear, t.btMonth,
		t.btDay, t.btHour, t.btMinute, t.btSecond);
}

void _PwTimeToXmlTime(PW_TIME t, CString *pstrDest)
{
	ASSERT(pstrDest != NULL);
	pstrDest->Empty();
	pstrDest->Format(_T("%04u-%02u-%02uT%02u:%02u:%02u"), t.shYear, t.btMonth,
		t.btDay, t.btHour, t.btMinute, t.btSecond);
}

void _UuidToString(const BYTE *pUuid, CString *pstrDest)
{
	CString strTemp;
	ASSERT(pstrDest != NULL);

	pstrDest->Empty();
	for(int i = 0; i < 16; i++)
	{
		strTemp.Format(_T("%02x"), pUuid[i]);
		*pstrDest += strTemp;
	}
}

void _StringToUuid(const TCHAR *ptszSource, BYTE *pUuid)
{
	ASSERT((ptszSource != NULL) && (pUuid != NULL));
	if((ptszSource == NULL) || (pUuid == NULL)) return;

	// Check against invalid UUID
	ASSERT(_tcslen(ptszSource) == 32);
	if(_tcslen(ptszSource) != 32) return;

	TCHAR ch;
	BYTE bt;
	DWORD i;
	for(i = 0; i < 16; i++)
	{
		ch = ptszSource[i << 1];

		if((ch >= _T('0')) && (ch <= _T('9')))
			bt = (BYTE)(ch - _T('0'));
		else
			bt = (BYTE)(ch - _T('a') + 0x0A);
		ASSERT(bt < 0x10);
		bt <<= 4;

		ch = ptszSource[(i << 1) + 1];

		if((ch >= _T('0')) && (ch <= _T('9')))
			bt |= (BYTE)(ch - _T('0'));
		else
			bt |= (BYTE)(ch - _T('a') + 0x0A);

		pUuid[i] = bt;
	}
}

UTF8_BYTE *_StringToUTF8(const TCHAR *pszSourceString)
{
	DWORD i, j = 0;
	DWORD dwLength, dwBytesNeeded, dwUniBufferLength = 0;
	BYTE *p = NULL;
	WCHAR ut;
	const WCHAR *pUni = NULL;
	WCHAR *pUniBuffer = NULL;

	ASSERT(pszSourceString != NULL); if(pszSourceString == NULL) return NULL;

#ifdef _UNICODE
	dwLength = lstrlen(pszSourceString) + 1; // In order to be compatible with the code below, add 1 for the zero at the end of the buffer
	pUni = pszSourceString;
#else
	// This returns the new length plus the zero byte - i.e. the whole buffer!
	dwLength = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszSourceString, -1, NULL, 0);
	dwUniBufferLength = dwLength + 2;
	pUniBuffer = new WCHAR[dwUniBufferLength];
	pUniBuffer[0] = 0; pUniBuffer[1] = 0;
	pUni = pUniBuffer;
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszSourceString, -1, pUniBuffer, dwLength);
#endif

	// Both counting and converting routines need update to support surrogates
	// count UTF-8 needed bytes
	dwBytesNeeded = 0;
	for(i = 0; i < dwLength; i++)
	{
		ut = pUni[i];

		if(ut == 0) break;

		if(ut < 0x80) dwBytesNeeded++;
		else if(ut < 0x0800) dwBytesNeeded += 2;
		else dwBytesNeeded += 3;
	}

	p = new BYTE[dwBytesNeeded + 2];
	ASSERT(p != NULL); if(p == NULL) return NULL;

	j = 0;
	for(i = 0; i < dwLength; i++)
	{
		ut = pUni[i];

		if(ut < 0x80) // 7-bit character, store as it is
		{
			p[j] = (BYTE)ut; j++;
		}
		else if(ut < 0x800) // Are 2 bytes enough?
		{
			p[j] = (BYTE)(0xC0 | (ut >> 6)); j++;
			p[j] = (BYTE)(0x80 | (ut & 0x3F)); j++;
		}
		else // Maximum bytes needed for UCS-2 is 3 bytes in UTF-8
		{
			p[j] = (BYTE)(0xE0 | (ut >> 12)); j++;
			p[j] = (BYTE)(0x80 | ((ut >> 6) & 0x3F)); j++;
			p[j] = (BYTE)(0x80 | (ut & 0x3F)); j++;
		}
	}
	p[j] = 0; // Terminate string
	ASSERT(j == (dwBytesNeeded + 1));

#ifndef _UNICODE
	mem_erase((unsigned char *)pUniBuffer, dwUniBufferLength * sizeof(WCHAR));
	SAFE_DELETE_ARRAY(pUniBuffer);
#endif

	return p;
}

DWORD _UTF8NumChars(const UTF8_BYTE *pUTF8String)
{
	DWORD i = 0, dwLength = 0;
	BYTE bt;

	ASSERT(pUTF8String != NULL);
	if(pUTF8String == NULL) return 0;

	while(1)
	{
		bt = pUTF8String[i];
		if(bt == 0) break;
		else if((bt & 0x80) == 0) dwLength++;
		else if((bt & 0xC0) == 0xC0) dwLength++;
		else if((bt & 0xE0) == 0xE0) dwLength++;
		i++;
	}

	return dwLength;
}

// This returns the needed bytes to represent the string, without terminating NULL character
DWORD _UTF8BytesNeeded(const TCHAR *pszString)
{
	DWORD i = 0;
	DWORD dwBytes = 0;
	USHORT us;

	// Don't use this function any more. The _StringToUTF8 function uses some pre-conversion
	// functions that makes a simple length calculation like in this function impossible.
	// If you really need this function, comment out the following ASSERT, but be careful!
	ASSERT(FALSE);

	ASSERT(pszString != NULL);
	if(pszString == NULL) return 0;

	while(1)
	{
#ifdef _UNICODE
		us = (USHORT)pszString[i];
#else
		us = (USHORT)(((USHORT)((BYTE)pszString[i])) & 0x00FF);
#endif

		if(us == 0) break;

		if(us < 0x0080) dwBytes++;
		else if(us < 0x0800) dwBytes += 2;
		else dwBytes += 3;

		i++;
	}

	return dwBytes;
}

TCHAR *_UTF8ToString(const UTF8_BYTE *pUTF8String)
{
	DWORD i, j;
	DWORD dwNumChars, dwMoreBytes, dwPBufLength = 0;
	BYTE b0, b1, b2;
	WCHAR *p, *pANSI;
	WCHAR tch;

	ASSERT(pUTF8String != NULL); if(pUTF8String == NULL) return NULL;

	// Count needed Unicode chars (right counterpart to _StringToUTF8)
	i = 0; dwNumChars = 0;
	while(1)
	{
		b0 = (BYTE)pUTF8String[i];
		dwMoreBytes = 0;
		if(b0 == 0) break;
		else if(b0 < 0xC0) dwMoreBytes++;
		else if(b0 < 0xE0) dwMoreBytes++;
		else if(b0 < 0xF0) dwMoreBytes++;
		dwNumChars++;
		i += dwMoreBytes;
	}
	if(dwNumChars == 0) return NULL;

	dwPBufLength = dwNumChars + 2;
	p = new WCHAR[dwPBufLength];
	ASSERT(p != NULL); if(p == NULL) return NULL;

	i = 0; j = 0;
	while(1)
	{
		b0 = pUTF8String[i]; i++;

		if(b0 < 0x80)
		{
			p[j] = (WCHAR)b0; j++;
		}
		else
		{
			b1 = pUTF8String[i]; i++;

			ASSERT((b1 & 0xC0) == 0x80);
			if((b1 & 0xC0) != 0x80) break;

			if((b0 & 0xE0) == 0xC0)
			{
				tch = (WCHAR)(b0 & 0x1F);
				tch <<= 6;
				tch |= (b1 & 0x3F);
				p[j] = tch; j++;
			}
			else
			{
				b2 = pUTF8String[i]; i++;

				ASSERT((b2 & 0xC0) == 0x80);
				if((b2 & 0xC0) != 0x80) break;

				tch = (WCHAR)(b0 & 0xF);
				tch <<= 6;
				tch |= (b1 & 0x3F);
				tch <<= 6;
				tch |= (b2 & 0x3F);
				p[j] = tch; j++;
			}
		}

		if(b0 == 0) break;
	}

#ifdef _UNICODE
	return (TCHAR *)p;
#else
	// Got Unicode, convert to ANSI
	dwNumChars = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, p, -1, NULL, 0, NULL, NULL);
	pANSI = new WCHAR[dwNumChars + 2];
	pANSI[0] = 0;
	VERIFY(WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, p, -1, (LPSTR)pANSI, dwNumChars, NULL, NULL) !=
		ERROR_INSUFFICIENT_BUFFER);
	if(p != NULL) mem_erase((unsigned char *)p, dwPBufLength);
	SAFE_DELETE_ARRAY(p);
	return (TCHAR *)pANSI;
#endif
}

void ParseURL(CString *pString, PW_ENTRY *pEntry, BOOL bMakeSimString)
{
	CString str;
	int nPos;

	ASSERT(pString != NULL); if(pString == NULL) return;
	ASSERT_ENTRY(pEntry); if(pEntry == NULL) return;

	str = *pString;

	while(1)
	{
		nPos = str.Find(_T("%TITLE%"));
		if(nPos == -1) nPos = str.Find(_T("{TITLE}"));
		if(nPos == -1) break;

		if(bMakeSimString == FALSE)
			str = str.Left(nPos) + pEntry->pszTitle + str.Right(str.GetLength() - nPos - 7);
		else
			str = str.Left(nPos) + TagSimString(pEntry->pszTitle) + str.Right(str.GetLength() - nPos - 7);
	}

	while(1)
	{
		nPos = str.Find(_T("%USERNAME%"));
		if(nPos == -1) nPos = str.Find(_T("{USERNAME}"));
		if(nPos == -1) break;

		if(bMakeSimString == FALSE)
			str = str.Left(nPos) + pEntry->pszUserName + str.Right(str.GetLength() - nPos - 10);
		else
			str = str.Left(nPos) + TagSimString(pEntry->pszUserName) + str.Right(str.GetLength() - nPos - 10);
	}

	while(1)
	{
		nPos = str.Find(_T("%URL%"));
		if(nPos == -1) nPos = str.Find(_T("{URL}"));
		if(nPos == -1) break;

		if(bMakeSimString == FALSE)
			str = str.Left(nPos) + pEntry->pszURL + str.Right(str.GetLength() - nPos - 5);
		else
			str = str.Left(nPos) + TagSimString(pEntry->pszURL) + str.Right(str.GetLength() - nPos - 5);
	}

	while(1)
	{
		nPos = str.Find(_T("%PASSWORD%"));
		if(nPos == -1) nPos = str.Find(_T("{PASSWORD}"));
		if(nPos == -1) break;

		if(bMakeSimString == FALSE)
			str = str.Left(nPos) + pEntry->pszPassword + str.Right(str.GetLength() - nPos - 10);
		else
			str = str.Left(nPos) + TagSimString(pEntry->pszPassword) + str.Right(str.GetLength() - nPos - 10);
	}

	while(1)
	{
		nPos = str.Find(_T("%NOTES%"));
		if(nPos == -1) nPos = str.Find(_T("{NOTES}"));
		if(nPos == -1) break;

		if(bMakeSimString == FALSE)
			str = str.Left(nPos) + pEntry->pszAdditional + str.Right(str.GetLength() - nPos - 7);
		else
			str = str.Left(nPos) + TagSimString(pEntry->pszAdditional) + str.Right(str.GetLength() - nPos - 7);
	}

	*pString = str;
	EraseCString(&str);
}

CString CsFileOnly(CString *psFilePath)
{
	CString str;
	int i;

	ASSERT(psFilePath != NULL); if(psFilePath == NULL) return CString("");

	i = psFilePath->ReverseFind(_T('\\'));
	if(i == -1) i = psFilePath->ReverseFind(_T('/'));

	if(i == -1) str = *psFilePath;
	else
	{
		str = psFilePath->Right(psFilePath->GetLength() - i - 1);
	}

	return str;
}

#define LOCAL_NUMXMLCONV 7

TCHAR *MakeSafeXmlString(TCHAR *ptString)
{
	DWORD i, j;
	DWORD dwStringLen, dwNeededChars = 0, dwOutPos = 0;
	TCHAR tch;
	BOOL bFound;
	TCHAR *pFinal;

	TCHAR aChar[LOCAL_NUMXMLCONV] = {
		_T('<'), _T('>'), _T('&'), _T('\"'), _T('\'')
	};

	TCHAR *pTrans[LOCAL_NUMXMLCONV] = {
		_T("&lt;"), _T("&gt;"), _T("&amp;"), _T("&quot;"), _T("&apos;")
	};

	ASSERT(ptString != NULL); if(ptString == NULL) return NULL;

	dwStringLen = (DWORD)_tcslen(ptString);

	for(i = 0; i < dwStringLen; i++)
	{
		tch = ptString[i];

		bFound = FALSE;
		for(j = 0; j < LOCAL_NUMXMLCONV; j++)
		{
			if(tch == aChar[j])
			{
				dwNeededChars += _tcslen(pTrans[j]);
				bFound = TRUE;
			}
		}
		if(bFound == FALSE) dwNeededChars++;
	}

	pFinal = new TCHAR[dwNeededChars + 4];
	ASSERT(pFinal != NULL); if(pFinal == NULL) return NULL;

	for(i = 0; i < dwStringLen; i++)
	{
		tch = ptString[i];

		bFound = FALSE;
		for(j = 0; j < LOCAL_NUMXMLCONV; j++)
		{
			if(tch == aChar[j])
			{
				_tcscpy(&pFinal[dwOutPos], pTrans[j]);
				dwOutPos += _tcslen(pTrans[j]);
				bFound = TRUE;
			}
		}

		if(bFound == FALSE)
		{
			pFinal[dwOutPos] = tch;
			dwOutPos++;
		}
	}

	ASSERT(dwOutPos == dwNeededChars);
	pFinal[dwOutPos] = 0; pFinal[dwOutPos + 1] = 0;
	ASSERT(_tcslen(pFinal) == dwNeededChars);

	return pFinal;
}

size_t szlen(const char *pszString)
{
	ASSERT(pszString != NULL); if(pszString == NULL) return 0;
	return strlen(pszString);
}

char *szcpy(char *szDestination, const char *szSource)
{
	ASSERT(szDestination != NULL); if(szDestination == NULL) return NULL;
	ASSERT(szSource != NULL); if(szSource == NULL) { szDestination[0] = 0; return szDestination; }
	return strcpy(szDestination, szSource);
}

#ifdef _UNICODE
#define PRPT_API_NAME "PathRelativePathToW"
#else
#define PRPT_API_NAME "PathRelativePathToA"
#endif

#ifndef _WIN32_WCE
CString MakeRelativePathEx(LPCTSTR lpBaseFile, LPCTSTR lpTargetFile)
{
	LPPATHRELATIVEPATHTO lpRel;
	HINSTANCE hShl;
	TCHAR tszPath[MAX_PATH * 2];
	BOOL bResult = FALSE;
	CString str;
	BOOL bMod;

	if((lpBaseFile[1] == _T(':')) && (lpTargetFile[1] == _T(':')) &&
		(lpBaseFile[2] == _T('\\')) && (lpTargetFile[2] == _T('\\')) &&
		(lpBaseFile[0] != lpTargetFile[0]))
	{
		return CString(lpTargetFile);
	}
	else if((lpTargetFile[0] == _T('\\')) && (lpTargetFile[1] == _T('\\')))
	{
		return CString(lpTargetFile);
	}

	hShl = LoadLibrary(_T("ShlWApi.dll"));
	if(hShl == NULL) return CString(lpTargetFile);

	lpRel = (LPPATHRELATIVEPATHTO)GetProcAddress(hShl, _T(PRPT_API_NAME));
	if(lpRel != NULL)
	{
		bResult = lpRel(tszPath, lpBaseFile, 0, lpTargetFile, 0);
	}
	FreeLibrary(hShl); hShl = NULL;
	if(bResult == FALSE) return CString(lpTargetFile);

	str = tszPath;
	while(1) // Remove all .\\ from the left of the path
	{
		bMod = FALSE;

		if(str.Left(2) == ".\\")
		{
			str = str.Right(str.GetLength() - 2);
			bMod = TRUE;
		}

		if(bMod == FALSE) break;
	}

	if(bResult == TRUE) return str;
	else return CString(lpTargetFile);
}
#else
CString MakeRelativePathEx(LPCTSTR lpBaseFile, LPCTSTR lpTargetFile)
{
	return CString(lpTargetFile);
}
#endif

BOOL GetRegKeyEx(HKEY hkeyBase, LPCTSTR lpSubKey, LPTSTR lpRetData)
{
	HKEY hkey = hkeyBase;
	LONG lRetVal = RegOpenKeyEx(hkeyBase, lpSubKey, 0, KEY_QUERY_VALUE, &hkey);

	if(lRetVal == ERROR_SUCCESS)
	{
		LONG lDataSize = MAX_PATH;
		TCHAR tszData[MAX_PATH];

		lRetVal = RegQueryValue(hkey, NULL, tszData, &lDataSize);
		_tcscpy(lpRetData, tszData);
		RegCloseKey(hkey); hkey = (HKEY)NULL;
	}

	return lRetVal == ERROR_SUCCESS ? TRUE : FALSE;
}

BOOL OpenUrlInNewBrowser(LPCTSTR lpURL)
{
	TCHAR tszKey[MAX_PATH << 1];
	UINT uResult = 0;

	ASSERT(lpURL != NULL); if(lpURL == NULL) return FALSE;

	_tcscpy(tszKey, _T("http\\shell\\open\\command"));

	if(GetRegKeyEx(HKEY_CLASSES_ROOT, tszKey, tszKey) == TRUE)
	{
		TCHAR *pos;
		pos = _tcsstr(tszKey, _T("\"%1\""));
		if(pos == NULL) // No quotes found
		{
			pos = _tcsstr(tszKey, _T("%1")); // Check for %1, without quotes 
			if(pos == NULL) // No parameter at all...
				pos = tszKey + _tcslen(tszKey) - 1;
			else
				*pos = '\0'; // Remove the parameter
		}
		else *pos = '\0'; // Remove the parameter

		_tcscat(pos, _T(" "));
		_tcscat(pos, lpURL);

		uResult = WinExec(tszKey, SW_SHOW);
	}

	return uResult > 31 ? TRUE : FALSE;
}

BOOL OpenUrlUsingPutty(LPCTSTR lpURL, LPCTSTR lpUser)
{
	CString strURL;
	BOOL bResult = FALSE;

	ASSERT(lpURL != NULL); if(lpURL == NULL) return FALSE;
	strURL = lpURL;

	if(strURL.Find(_T("ssh:")) >= 0)
	{
		TCHAR tszKey[MAX_PATH << 1];

		// TODO: Make this configurable
		_tcscpy(tszKey, _T("PUTTY.EXE -ssh "));

		// Parse out the "http://" and "ssh://"
		if(strURL.Find(_T("http://")) == 0)
			strURL = strURL.Right(strURL.GetLength() - _tcslen(_T("http://")));

		strURL = strURL.Right(strURL.GetLength() - _tcslen(_T("ssh:")));
		if(strURL.Left(1) == _T("/"))
			strURL = strURL.Right(strURL.GetLength() - 1);
		if(strURL.Left(1) == _T("/"))
			strURL = strURL.Right(strURL.GetLength() - 1);

		// Add pre-URL command-line parameters
		if(lpUser != NULL)
		{
			if(_tcslen(lpUser) > 0)
			{
				_tcscat(tszKey, lpUser);
				_tcscat(tszKey, _T("@"));
			}
		}

		// Add the URL
		_tcscat(tszKey, (LPCTSTR)strURL);

		// Execute the ssh client
		bResult = WinExec(tszKey, SW_SHOW) > 31 ? TRUE : FALSE;
	}
	else if(strURL.Find(_T("telnet:")) >= 0)
	{
		TCHAR tszKey[MAX_PATH << 1];

		// TODO: Make this configurable
		_tcscpy(tszKey, _T("PUTTY.EXE "));

		// Parse out the "http://" and "telnet://"
		if(strURL.Find(_T("http://")) == 0)
			strURL = strURL.Right(strURL.GetLength() - _tcslen(_T("http://")));

		strURL = strURL.Right(strURL.GetLength() - _tcslen(_T("telnet:")));
		if(strURL.Left(1) == _T("/"))
			strURL = strURL.Right(strURL.GetLength() - 1);
		if(strURL.Left(1) == _T("/"))
			strURL = strURL.Right(strURL.GetLength() - 1);

		// Add the url
		_tcscat(tszKey, _T("telnet://"));
		_tcscat(tszKey, strURL.GetBuffer(0));

		// Execute the ssh client
		bResult = WinExec(tszKey, SW_SHOW) > 31 ? TRUE : FALSE;
	}

	return bResult;
}

void OpenUrlEx(LPCTSTR lpURL)
{
	ASSERT(lpURL != NULL); if(lpURL == NULL) return;

	if(_tcslen(lpURL) == 0) return;

	if(_tcsncmp(lpURL, _T("http://"), 7) == 0)
	{
		if(OpenUrlInNewBrowser(lpURL) == FALSE)
			ShellExecute(NULL, _T("open"), lpURL, NULL, NULL, SW_SHOW);
	}
	else if(_tcsncmp(lpURL, _T("https://"), 8) == 0)
	{
		if(OpenUrlInNewBrowser(lpURL) == FALSE)
			ShellExecute(NULL, _T("open"), lpURL, NULL, NULL, SW_SHOW);
	}
	else if(_tcsncmp(lpURL, _T("cmd://"), 6) == 0)
	{
		if(_tcslen(lpURL) != 6)
			WinExec(((LPCSTR)lpURL) + (6 * sizeof(TCHAR)), SW_SHOW);
	}
	else
	{
		ShellExecute(NULL, _T("open"), lpURL, NULL, NULL, SW_SHOW);
	}
}

CString ExtractAutoTypeCmd(LPCTSTR lpstr)
{
	TCHAR *lp;
	TCHAR tch;
	CString str = _T("");
	CString strSource;
	int nPos;

	ASSERT(lpstr != NULL); if(lpstr == NULL) return CString("");

	strSource = lpstr;
	strSource.MakeLower();
	lp = (TCHAR *)lpstr;

	nPos = strSource.Find("auto-type:", 0);
	if(nPos != -1)
	{
		lp += _tcslen("auto-type:");
		lp += nPos;

		while(1)
		{
			tch = *lp;

			if(tch == '\0') break;
			else if(tch == '\n') break;
			else if(tch == '\r') { }
			else str += tch;

			lp++;
		}
	}

	str.TrimLeft(); str.TrimRight();
	return str;
}

CString TagSimString(LPCTSTR lpString)
{
	int i;
	CString str = _T("");
	TCHAR tch;

	ASSERT(lpString != NULL); if(lpString == NULL) return str;

	for(i = 0; i < (int)_tcslen(lpString); i++)
	{
		tch = lpString[i];

		switch(tch)
		{
			case _T('+'): str += _T("{PLUS}"); break;
			case _T('@'): str += _T("{AT}"); break;
			case _T('^'): str += _T("{CARET}"); break;
			case _T('~'): str += _T("{TILDE}"); break;
			case _T('%'): str += _T("{PERCENT}"); break;
			case _T('{'): str += _T("{LEFTBRACE}"); break;
			case _T('}'): str += _T("{RIGHTBRACE}"); break;
			case _T('('): str += _T("{LEFTPAREN}"); break;
			case _T(')'): str += _T("{RIGHTPAREN}"); break;
			default: str += tch; break;
		}
	}

	return str;
}
