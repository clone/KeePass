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

#include "StdAfx.h"
#include "../Util/AppUtil.h"
#include "../Util/MemUtil.h"
#include "../Util/NewRandom.h"

BOOL GetApplicationDirectory(LPTSTR lpStoreBuf, DWORD dwBufLen, BOOL bFilterSpecial, BOOL bMakeURL)
{
	int i, j = 0;

	ASSERT(lpStoreBuf != NULL); if(lpStoreBuf == NULL) return FALSE;
	ASSERT(dwBufLen >= 3); if(dwBufLen < 3) return FALSE;

	lpStoreBuf[0] = 0; lpStoreBuf[1] = 0;
	GetModuleFileName(NULL, lpStoreBuf, dwBufLen);
	for(i = (int)_tcslen(lpStoreBuf) - 1; i > 1; i--) // Extract dir
	{
		if((lpStoreBuf[i] == _T('\\')) || (lpStoreBuf[i] == _T('/')))
		{
			lpStoreBuf[i] = 0;
			break;
		}
	}

	for(i = 0; i < (int)_tcslen(lpStoreBuf); i++)
	{
		if((bMakeURL == TRUE) && (lpStoreBuf[i] == _T('\\'))) lpStoreBuf[i] = _T('/');

		if(bFilterSpecial == TRUE)
		{
			if(lpStoreBuf[i] != _T('\"'))
			{
				lpStoreBuf[j] = lpStoreBuf[i];
				j++;
			}
		}
	}

	return TRUE;
}

#ifndef _WIN32_WCE
BOOL SecureDeleteFile(LPCTSTR pszFilePath)
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
		if(_tremove(pszFilePath) != 0)
			return FALSE;

	return TRUE;
}
#endif
