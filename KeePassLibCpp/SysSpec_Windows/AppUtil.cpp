/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2010 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "../Util/PwUtil.h"
#include "../Util/FileTransactionEx.h"
#include "../PwManager.h"

// #ifndef _WIN32_WCE
// #include <objbase.h>
// #include <atlconv.h>
// #endif // _WIN32_WCE

static bool g_bAuInitialized = false;

void AU_EnsureInitialized()
{
	if(g_bAuInitialized) return;

	HMODULE hKernel = ::LoadLibrary(_T("Kernel32.dll")); // Native, not AU
	if(hKernel != NULL)
	{
		// Do not load libraries from the current working directory
		LPSETDLLDIRECTORY lpSetDllDirectory = (LPSETDLLDIRECTORY)
			::GetProcAddress(hKernel, SETDLLDIRECTORY_FNNAME);
		if(lpSetDllDirectory != NULL) lpSetDllDirectory(_T(""));

		// Enable data execution prevention (DEP)
		LPSETPROCESSDEPPOLICY lpSetProcessDEPPolicy = (LPSETPROCESSDEPPOLICY)
			::GetProcAddress(hKernel, SETPROCESSDEPPOLICY_FNNAME);
		if(lpSetProcessDEPPolicy != NULL) lpSetProcessDEPPolicy(PROCESS_DEP_ENABLE);

		::FreeLibrary(hKernel);
	}
	else { ASSERT(FALSE); }

	g_bAuInitialized = true;
}

HMODULE AU_LoadLibrary(LPCTSTR lpFileName)
{
	return ::LoadLibrary(lpFileName);
}

BOOL AU_GetApplicationDirectory(LPTSTR lpStoreBuf, DWORD dwBufLen, BOOL bFilterSpecial, BOOL bMakeURL)
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
BOOL AU_SecureDeleteFile(LPCTSTR pszFilePath)
{
	DWORD dwDummy = 0, dwTmp = 0;

	HANDLE hFile = CreateFile(pszFilePath, GENERIC_WRITE, FILE_SHARE_READ |
		FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE) return FALSE;

	const DWORD dwSizeLo = GetFileSize(hFile, NULL);
	if(dwSizeLo == 0) { CloseHandle(hFile); return DeleteFile(pszFilePath); }

	SetFilePointer(hFile, 0, NULL, FILE_END);
	WriteFile(hFile, &dwDummy, 4, &dwTmp, NULL);
	FlushFileBuffers(hFile);

	BYTE* pBuf = new BYTE[SDF_BUF_SIZE];
	if(pBuf == NULL) { CloseHandle(hFile); return FALSE; }

	// Four rounds: zeros, ones, random, random
	for(int n = 0; n < 4; ++n)
	{
		if(n == 0) memset(pBuf, 0, SDF_BUF_SIZE);
		else if(n == 1) memset(pBuf, 0xFF, SDF_BUF_SIZE);
		else
		{
			for(DWORD j = 0; j < SDF_BUF_SIZE; ++j)
				pBuf[j] = static_cast<BYTE>(randXorShift());
		}

		VERIFY(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == 0);
		DWORD i = (dwSizeLo + static_cast<DWORD>(n)); // Each round one more character at the end

		while(i != 0)
		{
			const DWORD m = ((i < SDF_BUF_SIZE) ? i : SDF_BUF_SIZE);

			const BOOL bRet = WriteFile(hFile, pBuf, m, &dwTmp, NULL);
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

int AU_WriteBigFile(LPCTSTR lpFilePath, const BYTE* pData, DWORD dwDataSize,
	BOOL bTransacted)
{
	// const bool bMadeUnhidden = CPwUtil::UnhideFile(lpFilePath);

	CFileTransactionEx ft(lpFilePath, (bTransacted == FALSE) ? false : true);
	std_string strBufFile;
	if(!ft.OpenWrite(strBufFile)) return PWE_GETLASTERROR;

	HANDLE hFile = CreateFile(strBufFile.c_str(), GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		// if(bMadeUnhidden) CPwUtil::HideFile(lpFilePath, true);
		return PWE_NOFILEACCESS_WRITE;
	}

	int nResult = PWE_SUCCESS;
	DWORD dwRemaining = dwDataSize, dwPosition = 0;

	while(dwRemaining != 0)
	{
		const DWORD dwWrite = ((dwRemaining > AU_MAX_WRITE_BLOCK) ?
			AU_MAX_WRITE_BLOCK : dwRemaining);

		DWORD dwWritten = 0;
		if(WriteFile(hFile, &pData[dwPosition], dwWrite, &dwWritten, NULL) == FALSE)
		{
			nResult = PWE_FILEERROR_WRITE;
			break;
		}
		if(dwWritten == 0) { nResult = PWE_FILEERROR_WRITE; break; }

		dwPosition += dwWritten;
		dwRemaining -= dwWritten;
	}

	VERIFY(FlushFileBuffers(hFile));
	VERIFY(CloseHandle(hFile));

	if(!ft.CommitWrite()) return PWE_GETLASTERROR;

	// if(bMadeUnhidden) CPwUtil::HideFile(lpFilePath, true);
	return nResult;
}

BOOL AU_IsWin9xSystem()
{
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);

	return ((osvi.dwMajorVersion <= 4) ? TRUE : FALSE);
}

BOOL AU_IsAtLeastWinVistaSystem()
{
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);

	return ((osvi.dwMajorVersion >= 6) ? TRUE : FALSE);
}

/*
#ifndef _WIN32_WCE
BOOL _AU_RemoveZoneIdentifier(LPCTSTR lpFile)
{
	USES_CONVERSION;

	IPersistFile* pf = NULL;
	if(CoCreateInstance(CLSID_PersistentZoneIdentifier, NULL, CLSCTX_INPROC_SERVER,
		IID_IPersistFile, (LPVOID*)&pf) != S_OK) return FALSE;

	LPCOLESTR lpOleFile = T2OLE(lpFile);
	if(pf->Load(lpOleFile, STGM_READWRITE | STGM_SHARE_EXCLUSIVE) != S_OK) return FALSE;

	IZoneIdentifier* pz = NULL;
	if(pf->QueryInterface(IID_IZoneIdentifier, (void**)&pz) != S_OK) return FALSE;

	DWORD dwZone = 0;
	VERIFY(pz->GetId(&dwZone) == S_OK);
	if(dwZone == URLZONE_INTERNET)
	{
		VERIFY(pz->Remove() == S_OK);
		VERIFY(pf->Save(NULL, TRUE) == S_OK);
	}

	pz->Release();
	pf->Release();
	return TRUE;
}

BOOL AU_RemoveZoneIdentifier(LPCTSTR lpFile)
{
	if(lpFile == NULL) { ASSERT(FALSE); return FALSE; }

	HRESULT hrCo = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if((hrCo != S_OK) && (hrCo != S_FALSE))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	const BOOL bResult = _AU_RemoveZoneIdentifier(lpFile);

	CoUninitialize();
	return bResult;
}
#endif // _WIN32_WCE
*/
