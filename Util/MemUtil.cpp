/*
  Copyright (c) 2003, Dominik Reichl <dominik.reichl@t-online.de>
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
#include "NewRandom.h"

void mem_erase(unsigned char *p, unsigned long u)
{
	unsigned long i;

	ASSERT(p != NULL);
	if(p == NULL) return;
	ASSERT(u != 0);
	if(u == 0) return;

	for(i = 0; i < u; i++)
		p[i] = (unsigned char)(rand() & 0xFF);
	for(i = 0; i < u; i++)
		p[i] = (unsigned char)(rand() & 0xFF);
	for(i = 0; i < u; i++)
		p[i] = (unsigned char)(rand() & 0xFF);

	memset(p, 0, u);
}

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

void CopyStringToClipboard(char *pszString)
{
	unsigned long uDataSize;
	HGLOBAL globalHandle;
	LPVOID globalData;

	if(::OpenClipboard(NULL) == FALSE) return;

	if(::EmptyClipboard() == FALSE) return;

	if(pszString == NULL)
	{
		::CloseClipboard();
		return;
	}

	uDataSize = strlen(pszString); // Get length
	if(uDataSize == 0)
	{
		::CloseClipboard();
		return;
	}
	uDataSize++; // Plus NULL-terminator of string

	globalHandle = ::GlobalAlloc(GHND | GMEM_DDESHARE, uDataSize);
	globalData = ::GlobalLock(globalHandle);
	strcpy((char *)globalData, pszString); // Copy string plus NULL-byte to global memory
	::GlobalUnlock(globalHandle); // Unlock before SetClipboardData!

	::SetClipboardData(CF_TEXT, globalHandle); // Set clipboard data to our global memory block
	::CloseClipboard(); // Close clipboard, done
}

BOOL SecureDeleteFile(LPCSTR pszFilePath)
{
	HANDLE hFile;
	DWORD i, m, dwSizeLo, dwTmp;
	int n;
	BYTE *pBuf;
	BOOL bRet;

	hFile = CreateFile(pszFilePath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE) return FALSE;

	dwSizeLo = GetFileSize(hFile, NULL);
	if(dwSizeLo == 0) { CloseHandle(hFile); return DeleteFile(pszFilePath); }

	SetFilePointer(hFile, 0, NULL, FILE_END);
	WriteFile(hFile, &i, 4, &dwTmp, NULL);
	FlushFileBuffers(hFile);

	pBuf = new BYTE[SDF_BUF_SIZE];
	if(pBuf == NULL) { CloseHandle(hFile); return FALSE; }

	// Four rounds: zeros, ones, random, random
	for(n = 0; n < 4; n++)
	{
		if(n == 0) memset(pBuf, 0, SDF_BUF_SIZE);
		else if(n == 1) memset(pBuf, 0xFF, SDF_BUF_SIZE);
		else
		{
			for(i = 0; i < SDF_BUF_SIZE; i++)
				pBuf[i] = (BYTE)randXorShift();
		}

		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
		i = dwSizeLo + (DWORD)n; // Each round one more character at the end

		while(i != 0)
		{
			m = (i < SDF_BUF_SIZE) ? i : SDF_BUF_SIZE;

			bRet = WriteFile(hFile, pBuf, m, &dwTmp, NULL);
			FlushFileBuffers(hFile);

			ASSERT(bRet == TRUE);
			if(bRet != TRUE) break;
			if(m != dwTmp) { ASSERT(FALSE); break; }

			i -= m;
		}
	}

	CloseHandle(hFile);
	SAFE_DELETE_ARRAY(pBuf);

	if(DeleteFile(pszFilePath) == FALSE)
		if(remove(pszFilePath) != 0)
			return FALSE;

	return TRUE;
}

void FixURL(CString *pstrURL)
{
	CString strTemp;
	BOOL bPre = FALSE;

	ASSERT(pstrURL != NULL);

	strTemp = *pstrURL;
	strTemp.MakeLower();

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

	if(bPre == FALSE)
	{
		strTemp = "http:";
		if(pstrURL->Left(1) != "/") strTemp += "//";
		strTemp += *pstrURL;

		*pstrURL = strTemp;
	}
}
