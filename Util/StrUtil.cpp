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
#include "StrUtil.h"

void EraseCString(CString *pString)
{
	int i, j, len;
	const TCHAR tcPlus = ' ';
	const TCHAR tcMod = '}' - tcPlus;

	ASSERT(pString != NULL);

	len = pString->GetLength();

	for(i = 0; i < 3; i++) // 3 rounds overwrite
	{
		for(j = 0; j < len; j++)
		{
			pString->SetAt(j, (TCHAR)((rand() % tcMod) + tcPlus));
		}
	}

	pString->Empty();
}

#ifndef _WIN32_WCE
void CopyStringToClipboard(const TCHAR *lptString)
{
	unsigned long uDataSize;
	HGLOBAL globalHandle;
	LPVOID globalData;

	if(::OpenClipboard(NULL) == FALSE) return;

	if(::EmptyClipboard() == FALSE) return;

	if(lptString == NULL) // No string to copy => empty clipboard
	{
		::CloseClipboard();
		return;
	}

	uDataSize = _tcslen(lptString) * sizeof(TCHAR); // Get length
	if(uDataSize == 0)
	{
		::CloseClipboard();
		return;
	}
	uDataSize += sizeof(TCHAR); // Plus NULL-terminator of string

	globalHandle = ::GlobalAlloc(GHND | GMEM_DDESHARE, uDataSize);
	globalData = ::GlobalLock(globalHandle);
	_tcscpy((TCHAR *)globalData, lptString); // Copy string plus NULL-byte to global memory
	::GlobalUnlock(globalHandle); // Unlock before SetClipboardData!

	::SetClipboardData(CF_TEXT, globalHandle); // Set clipboard data to our global memory block
	::CloseClipboard(); // Close clipboard, and done
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

	if(bPre == FALSE) // The string isn't a valid URL, so assume it's a HTTP
	{
		strTemp = "http:";
		if(pstrURL->Left(1) != "/") strTemp += "//";
		strTemp += *pstrURL;

		*pstrURL = strTemp;
	}
}

char *_StringToAscii(const TCHAR *lptString)
{
	char *p;
	int _nChars = 0;

	if(lptString == NULL) { ASSERT(FALSE); return NULL; }

#ifdef _UNICODE
	_nChars = (_tcslen(lptString) + 1) * 2;
	p = new char[_nChars];
	p[0] = 0;
	VERIFY(WideCharToMultiByte(CP_ACP, 0, lptString, -1, p, _nChars, NULL, NULL) !=
		ERROR_INSUFFICIENT_BUFFER);
#else
	p = new char[strlen(lptString) + 1];
	strcpy(p, lptString);
#endif

	return p;
}

TCHAR *_StringToUnicode(const char *pszString)
{
	TCHAR *p;
	int _nChars = 0;

	if(pszString == NULL) { ASSERT(FALSE); return NULL; }

#ifdef _UNICODE
	// Determine the correct buffer size by calling the function itself with 0 as buffer size (see docs)
	_nChars = MultiByteToWideChar(CP_ACP, 0, pszString, -1, NULL, 0);

	p = new TCHAR[_nChars + 1];
	p[0] = 0;

	// Jan 9th 2004: DonAngel { 
	// This was ASSERTing for string. All debugging did not given good results, so I decided to remove
	// the verification. This could be a bug in MultiByteToWideChar, because thou it was returning
	// ERROR_INSUFFICIENT_BUFFER, the convertion was OK!?
	// The problem should be investigated later, but for now - I prefer to remove the ASSERT
	// VERIFY(MultiByteToWideChar(CP_ACP, 0, pszString, -1, p, _nChars) !=
	//	ERROR_INSUFFICIENT_BUFFER);
	// Jan 9th 2004: DonAngel }

	MultiByteToWideChar(CP_ACP, 0, pszString, -1, p, _nChars);
#else
	// Non-unicode - return the same!
	p = new char[strlen(pszString) + 1];
	strcpy(p, pszString);
#endif

	return p;
}

void _PwTimeToString(PW_TIME t, CString *pstrDest)
{
	ASSERT(pstrDest != NULL);
	pstrDest->Empty();
	pstrDest->Format(_T("%04u/%02u/%02u %02u:%02u:%02u"), t.shYear, t.btMonth,
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
	DWORD dwLength, dwBytesNeeded;
	USHORT ut;
	BYTE *p;

	ASSERT(pszSourceString != NULL);
	if(pszSourceString == NULL) return NULL;

	dwLength = _tcslen(pszSourceString);
	dwBytesNeeded = _UTF8BytesNeeded(pszSourceString);

	p = new BYTE[dwBytesNeeded + 1];
	ASSERT(p != NULL);
	if(p == NULL) return NULL;

	for(i = 0; i < dwLength; i++)
	{
#ifdef _UNICODE
		ut = (USHORT)pszSourceString[i];
#else
		ut = (USHORT)(BYTE)pszSourceString[i];
#endif

		if(ut < 0x80) // 7-bit character, store as it is
		{
			p[j] = (TCHAR)ut; j++;
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
	ASSERT(j == dwBytesNeeded);

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

DWORD _UTF8BytesNeeded(const TCHAR *pszString)
{
	DWORD i = 0;
	DWORD dwBytes = 0;
	USHORT us;

	ASSERT(pszString != NULL);
	if(pszString == NULL) return 0;

	while(1)
	{
#ifdef _UNICODE
		us = (USHORT)pszString[i];
#else
		us = (USHORT)(BYTE)pszString[i];
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
	DWORD i = 0, j = 0;
	DWORD dwNumChars;
	TCHAR *p;
	BYTE b0, b1, b2;
	TCHAR tch;

	ASSERT(pUTF8String != NULL);
	dwNumChars = _UTF8NumChars(pUTF8String);
	if(dwNumChars == 0) return NULL;

	p = new TCHAR[dwNumChars + 1];
	ASSERT(p != NULL);
	if(p == NULL) return NULL;

	while(1)
	{
		b0 = pUTF8String[i]; i++;

		if(b0 < 0x80)
		{
			p[j] = (TCHAR)b0; j++;
		}
		else
		{
			b1 = pUTF8String[i]; i++;

			ASSERT((b1 & 0xC0) == 0x80);
			if((b1 & 0xC0) != 0x80) break;

			if((b0 & 0xE0) == 0xC0)
			{
				tch = (TCHAR)(b0 & 0x1F);
				tch <<= 6;
				tch |= (b1 & 0x3F);
				p[j] = tch; j++;
			}
			else
			{
				b2 = pUTF8String[i]; i++;

				ASSERT((b2 & 0xC0) == 0x80);
				if((b2 & 0xC0) != 0x80) break;

				tch = (TCHAR)(b0 & 0xF);
				tch <<= 6;
				tch |= (b1 & 0x3F);
				tch <<= 6;
				tch |= (b2 & 0x3F);
				p[j] = tch; j++;
			}
		}

		if(b0 == 0) break;
	}

	return p;
}
