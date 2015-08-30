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

#include "StdAfx.h"
#include "../Util/MemUtil.h"
#include "../Util/StrUtil.h"

C_FN_SHARE char *_StringToAnsi(const TCHAR *lptString)
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

C_FN_SHARE TCHAR *_StringToUnicode(const char *pszString)
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

C_FN_SHARE UTF8_BYTE *_StringToUTF8(const TCHAR *pszSourceString)
{
	DWORD i, j = 0;
	DWORD dwLength, dwBytesNeeded;
	BYTE *p = NULL;
	WCHAR ut;
	const WCHAR *pUni = NULL;

#ifndef _UNICODE
	WCHAR *pUniBuffer = NULL;
	DWORD dwUniBufferLength = 0;
#endif

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

		// if(ut == 0) break;

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

C_FN_SHARE DWORD _UTF8NumChars(const UTF8_BYTE *pUTF8String)
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
C_FN_SHARE DWORD _UTF8BytesNeeded(const TCHAR *pszString)
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

C_FN_SHARE TCHAR *_UTF8ToString(const UTF8_BYTE *pUTF8String)
{
	DWORD i, j;
	DWORD dwNumChars, dwMoreBytes, dwPBufLength;
	BYTE b0, b1, b2;
	WCHAR *p;
	WCHAR tch;

#ifndef _UNICODE
	WCHAR *pANSI;
#endif

	ASSERT(pUTF8String != NULL); if(pUTF8String == NULL) return NULL;

	// Count the needed Unicode chars (right counterpart to _StringToUTF8)
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

		if(dwMoreBytes == 0) return NULL; // Invalid UTF-8 string
	}
	// if(dwNumChars == 0) return NULL;

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

	int nErr = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, p, -1, (LPSTR)pANSI, dwNumChars, NULL, NULL);
	if(dwNumChars != 122) { ASSERT(nErr != 122); } // ERROR_INSUFFICIENT_BUFFER is defined as 122...
	else { ASSERT(GetLastError() == 0); nErr = nErr; }

	if(p != NULL) mem_erase((unsigned char *)p, dwPBufLength);
	SAFE_DELETE_ARRAY(p);
	return (TCHAR *)pANSI;
#endif
}

C_FN_SHARE BOOL _IsUTF8String(const UTF8_BYTE *pUTF8String)
{
	DWORD i = 0;
	BYTE b0, b1, b2;

	ASSERT(pUTF8String != NULL); if(pUTF8String == NULL) return FALSE;

	if(pUTF8String[0] == 0xEF)
		if(pUTF8String[1] == 0xBB)
			if(pUTF8String[2] == 0xBF) i += 3;

	while(1)
	{
		b0 = pUTF8String[i]; i++;

		if(b0 >= 0x80)
		{
			b1 = pUTF8String[i]; i++;

			if((b1 & 0xC0) != 0x80) return FALSE;

			if((b0 & 0xE0) != 0xC0)
			{
				b2 = pUTF8String[i]; i++;

				if((b2 & 0xC0) != 0x80) return FALSE;
			}
		}

		if(b0 == 0) break;
	}

	return TRUE;
}
