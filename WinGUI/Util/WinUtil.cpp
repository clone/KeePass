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
#include <mmsystem.h>

#ifdef _UNICODE
#include <atlbase.h>
#endif

#include "WinUtil.h"
#include "CmdLine/Executable.h"
#include "PrivateConfig.h"
#include "../../KeePassLibCpp/Util/AppUtil.h"
#include "../../KeePassLibCpp/Util/MemUtil.h"
#include "../../KeePassLibCpp/Util/StrUtil.h"
#include "../../KeePassLibCpp/Util/TranslateEx.h"

static unsigned char g_shaLastString[32];
// static LPCTSTR g_lpChildWindowText = NULL;

static UINT g_uCfIgnoreID = 0; // ID of CFN_CLIPBOARD_VIEWER_IGNORE

#ifndef _WIN32_WCE

#ifdef _UNICODE
#define CF_TTEXTEX CF_UNICODETEXT
#else
#define CF_TTEXTEX CF_TEXT
#endif

void CopyStringToClipboard(const TCHAR *lptString)
{
	if(OpenClipboard(NULL) == FALSE) return;
	if(EmptyClipboard() == FALSE) return;

	if(lptString == NULL) // No string to copy => empty clipboard
	{
		CloseClipboard();
		return;
	}

	size_t uDataSize = _tcslen(lptString) * sizeof(TCHAR); // Get length
	if(uDataSize == 0)
	{
		CloseClipboard();
		return;
	}
	uDataSize += sizeof(TCHAR); // Plus NULL-terminator of string

	SetClipboardIgnoreFormat();

	HGLOBAL globalHandle = GlobalAlloc(GHND | GMEM_DDESHARE, uDataSize);
	if(globalHandle == NULL) { ASSERT(FALSE); CloseClipboard(); return; }
	LPVOID globalData = GlobalLock(globalHandle);
	if(globalData == NULL) { ASSERT(FALSE); CloseClipboard(); return; }
	_tcscpy_s((TCHAR *)globalData, _tcslen(lptString) + 1, lptString); // Copy string plus NULL-byte to global memory
	GlobalUnlock(globalHandle); // Unlock before SetClipboardData!

	VERIFY(SetClipboardData(CF_TTEXTEX, globalHandle)); // Set clipboard data to our global memory block
	VERIFY(CloseClipboard()); // Close clipboard, and done

	RegisterOwnClipboardData((unsigned char *)lptString, uDataSize - sizeof(TCHAR));
}

void RegisterOwnClipboardData(unsigned char* pData, unsigned long dwDataSize)
{
	ASSERT(pData != NULL); if(pData == NULL) return;

	sha256_ctx shactx;

	sha256_begin(&shactx);

	if(dwDataSize > 0)
		sha256_hash(pData, dwDataSize, &shactx);

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

	HANDLE hClipboardData = GetClipboardData(CF_TTEXTEX);
	if(hClipboardData == NULL) { CloseClipboard(); return; }

	TCHAR *lpString = (TCHAR *)GlobalLock(hClipboardData);
	if(lpString == NULL) { CloseClipboard(); return; }

	sha256_ctx shactx;
	unsigned char uHash[32];
	sha256_begin(&shactx);
	sha256_hash((unsigned char *)lpString, (unsigned long)(_tcslen(lpString) *
		sizeof(TCHAR)), &shactx);
	sha256_end(uHash, &shactx);

	GlobalUnlock(hClipboardData);

	// If we have copied the string to the clipboard, delete it
	if(memcmp(uHash, g_shaLastString, 32) == 0)
	{
		VERIFY(EmptyClipboard());
	}

	VERIFY(CloseClipboard());
}

// Thanks to Gabe Martin for the contribution of the following
// two secure clipboard functions!
// http://sourceforge.net/tracker/index.php?func=detail&aid=1102906&group_id=95013&atid=609910

BOOL MakeClipboardDelayRender(HWND hOwner, HWND *phNextCB)
{
	BOOL bResult = OpenClipboard(hOwner);

	if(bResult != FALSE)
	{
		// Add a clipboard listener to the cb chain so we can block any listeners from
		// knowing we are adding sensitive data to the clipboard
		if(phNextCB != NULL)
			if(*phNextCB == NULL)
				*phNextCB = SetClipboardViewer(hOwner);

		EmptyClipboard();
		SetClipboardIgnoreFormat();
		SetClipboardData(CF_TTEXTEX, NULL);
		CloseClipboard();
	}

	return bResult;
}

void CopyDelayRenderedClipboardData(const TCHAR *lptString)
{
	ASSERT(lptString != NULL); if(lptString == NULL) return;

	SetClipboardIgnoreFormat();

	const size_t cch = _tcslen(lptString);
	HGLOBAL hglb = GlobalAlloc(GMEM_MOVEABLE, (cch + 1) * sizeof(TCHAR));
	ASSERT(hglb != NULL); if(hglb == NULL) return;

	// Copy the text from pboxLocalClip
	LPTSTR lptstr = (LPTSTR)GlobalLock(hglb);
	if(cch > 1) memcpy(lptstr, lptString, cch * sizeof(TCHAR));
	lptstr[cch] = (TCHAR)0;
	GlobalUnlock(hglb);

	// Put the delayed clipboard data in the clipboard.
	SetClipboardData(CF_TTEXTEX, hglb);

	RegisterOwnClipboardData((unsigned char *)lptString, cch * sizeof(TCHAR));
}

void SetClipboardIgnoreFormat()
{
	if(g_uCfIgnoreID == 0)
	{
		g_uCfIgnoreID = RegisterClipboardFormat(CFN_CLIPBOARD_VIEWER_IGNORE);
	}

	if(g_uCfIgnoreID != 0) // Registered
	{
		const size_t cch = _tcslen(PWM_PRODUCT_NAME);
		HGLOBAL hglb = GlobalAlloc(GMEM_MOVEABLE, (cch + 1) * sizeof(TCHAR));
		ASSERT(hglb != NULL); if(hglb == NULL) return;
		LPTSTR lptstr = (LPTSTR)GlobalLock(hglb);
		_tcscpy_s(lptstr, cch + 1, PWM_PRODUCT_NAME);
		GlobalUnlock(hglb);

		SetClipboardData(g_uCfIgnoreID, hglb);
	}
}

#endif

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

	lpRel = (LPPATHRELATIVEPATHTO)GetProcAddress(hShl, PRPT_API_NAME);
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

		if(str.Left(2) == _T(".\\"))
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
CPP_FN_SHARE CString MakeRelativePathEx(LPCTSTR lpBaseFile, LPCTSTR lpTargetFile)
{
	return CString(lpTargetFile);
}
#endif

CString GetShortestAbsolutePath(LPCTSTR lpFilePath)
{
	CString str;

	ASSERT(lpFilePath != NULL); if(lpFilePath == NULL) return str;

	DWORD dwBufLen = (DWORD)_tcslen(lpFilePath) + 12, dw;
	LPTSTR lpBuf = new TCHAR[dwBufLen + 1];
	LPTSTR lpFilePart = NULL;

	if(lpBuf != NULL)
	{
		lpBuf[0] = 0;
		dw = GetFullPathName(lpFilePath, dwBufLen, lpBuf, &lpFilePart);

		if((dw != 0) && (dw < dwBufLen)) str = lpBuf;
		else str = lpFilePath;
	}
	else str = lpFilePath;

	SAFE_DELETE_ARRAY(lpBuf);
	return str;
}

#pragma warning(push)
#pragma warning(disable: 4996) // _tcscpy deprecated

BOOL GetRegKeyEx(HKEY hkeyBase, LPCTSTR lpSubKey, LPTSTR lpRetData)
{
	HKEY hkey = hkeyBase;
	LONG lRetVal = RegOpenKeyEx(hkeyBase, lpSubKey, 0, KEY_QUERY_VALUE, &hkey);

	if(lRetVal == ERROR_SUCCESS)
	{
		LONG lDataSize = MAX_PATH * 4;
		TCHAR tszData[MAX_PATH * 4];

		lRetVal = RegQueryValue(hkey, NULL, tszData, &lDataSize);
		_tcscpy(lpRetData, tszData);
		RegCloseKey(hkey); hkey = NULL;
	}

	return (lRetVal == ERROR_SUCCESS) ? TRUE : FALSE;
}

#pragma warning(pop)

BOOL OpenUrlInNewBrowser(LPCTSTR lpURL)
{
	ASSERT(lpURL != NULL); if(lpURL == NULL) return FALSE;

	TCHAR tszKey[MAX_PATH * 4];
	UINT uResult = 0;

	_tcscpy_s(tszKey, _countof(tszKey), _T("http\\shell\\open\\command"));

	if(GetRegKeyEx(HKEY_CLASSES_ROOT, tszKey, tszKey) == TRUE)
	{
		TCHAR *pos = _tcsstr(tszKey, _T("\"%1\""));
		if(pos == NULL) // No quotes found
		{
			pos = _tcsstr(tszKey, _T("%1")); // Check for %1, without quotes
			if(pos != NULL) *pos = _T('\0'); // Remove the parameter
		}
		else *pos = _T('\0'); // Remove the parameter

		std::basic_string<TCHAR> strExec = tszKey;

		if(pos == NULL) strExec += _T(" ");
		strExec += lpURL;

		uResult = TWinExec(strExec.c_str(), KPSW_SHOWDEFAULT);
	}

	return (uResult > 31) ? TRUE : FALSE;
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
		_tcscpy_s(tszKey, _countof(tszKey), _T("PUTTY.EXE -ssh "));

		// Parse out the "http://" and "ssh://"
		if(strURL.Find(_T("http://")) == 0)
			strURL = strURL.Right(strURL.GetLength() - (int)_tcslen(_T("http://")));

		strURL = strURL.Right(strURL.GetLength() - (int)_tcslen(_T("ssh:")));
		if(strURL.Left(1) == _T("/"))
			strURL = strURL.Right(strURL.GetLength() - 1);
		if(strURL.Left(1) == _T("/"))
			strURL = strURL.Right(strURL.GetLength() - 1);

		// Add pre-URL command-line parameters
		if(lpUser != NULL)
		{
			if(_tcslen(lpUser) > 0)
			{
				_tcscat_s(tszKey, _countof(tszKey), lpUser);
				_tcscat_s(tszKey, _countof(tszKey), _T("@"));
			}
		}

		// Add the URL
		_tcscat_s(tszKey, _countof(tszKey), (LPCTSTR)strURL);

		// Execute the ssh client
		bResult = (TWinExec(tszKey, KPSW_SHOWDEFAULT) > 31) ? TRUE : FALSE;
	}
	else if(strURL.Find(_T("telnet:")) >= 0)
	{
		TCHAR tszKey[MAX_PATH << 1];

		// TODO: Make this configurable
		_tcscpy_s(tszKey, _countof(tszKey), _T("PUTTY.EXE "));

		// Parse out the "http://" and "telnet://"
		if(strURL.Find(_T("http://")) == 0)
			strURL = strURL.Right(strURL.GetLength() - (int)_tcslen(_T("http://")));

		strURL = strURL.Right(strURL.GetLength() - (int)_tcslen(_T("telnet:")));
		if(strURL.Left(1) == _T("/"))
			strURL = strURL.Right(strURL.GetLength() - 1);
		if(strURL.Left(1) == _T("/"))
			strURL = strURL.Right(strURL.GetLength() - 1);

		// Add the url
		_tcscat_s(tszKey, _countof(tszKey), _T("telnet://"));
		_tcscat_s(tszKey, _countof(tszKey), strURL.GetBuffer(0));

		// Execute the ssh client
		bResult = (TWinExec(tszKey, KPSW_SHOWDEFAULT) > 31) ? TRUE : FALSE;
	}

	return bResult;
}

// If hParent is not NULL, the function will show an error message if
// the URL cannot be opened
void OpenUrlEx(LPCTSTR lpURL, HWND hParent)
{
	ASSERT(lpURL != NULL); if(lpURL == NULL) return;
	if(_tcslen(lpURL) == 0) return; // Valid, but nothing to do

	CPrivateConfig cfg(FALSE);
	BOOL bPrevMethod = cfg.GetBool(PWMKEY_HTMURLMETHOD, FALSE);
	if(bPrevMethod == FALSE)
	{
		OpenUrlShellExec(lpURL, hParent);
		return;
	}

	if(_tcsncmp(lpURL, _T("http://"), 7) == 0)
	{
		if(OpenUrlInNewBrowser(lpURL) == FALSE)
			ShellExecute(NULL, _T("open"), lpURL, NULL, NULL, KPSW_SHOWDEFAULT);
	}
	else if(_tcsncmp(lpURL, _T("https://"), 8) == 0)
	{
		if(OpenUrlInNewBrowser(lpURL) == FALSE)
			ShellExecute(NULL, _T("open"), lpURL, NULL, NULL, KPSW_SHOWDEFAULT);
	}
	else if(_tcsncmp(lpURL, _T("cmd://"), 6) == 0)
	{
		if(_tcslen(lpURL) > 6)
		{
			TWinExec(&lpURL[6], KPSW_SHOWDEFAULT);
		}
	}
	else ShellExecute(NULL, _T("open"), lpURL, NULL, NULL, KPSW_SHOWDEFAULT);
}

// Internal function
void OpenUrlShellExec(LPCTSTR lpURL, HWND hParent)
{
	ASSERT(lpURL != NULL); if(lpURL == NULL) return;

	CString strURL = lpURL;
	strURL.TrimLeft(_T(" \t\r\n"));
	if(strURL.GetLength() == 0) return;

	if(strURL.Left(6) == _T("cmd://"))
		OpenUrlProcess(strURL.Right(strURL.GetLength() - 6), hParent);
	else // Standard method
		WU_SysShellExecute(strURL, NULL, hParent);
}

// Internal function
void OpenUrlProcess(LPCTSTR lpURL, HWND hParent)
{
	ASSERT(lpURL != NULL); if(lpURL == NULL) return;

	CString strLine = lpURL, strFile, strParam;
	BOOL bFile = FALSE, bParam = FALSE;

	if(strLine.Left(1) == _T("\""))
	{
		int nSecond = strLine.Find(_T('\"'), 1);
		if(nSecond >= 1)
		{
			strFile = strLine.Mid(1, nSecond - 1).Trim();
			bFile = TRUE;

			strParam = strLine.Right(strLine.GetLength() - (nSecond + 1)).Trim();
			bParam = TRUE;
		}
	}

	if(bFile == FALSE)
	{
		int nSpace = strLine.Find(_T(' '));

		if(nSpace >= 0)
		{
			strFile = strLine.Left(nSpace);
			bFile = TRUE;

			strParam = strLine.Right(strLine.GetLength() - nSpace).Trim();
			bParam = TRUE;
		}
		else { strFile = strLine; bFile = TRUE; }
	}

	if((bParam == TRUE) && (strParam.GetLength() > 0))
		WU_SysShellExecute(strFile, strParam, hParent);
	else
		WU_SysShellExecute(strFile, NULL, hParent);
}

void WU_SysShellExecute(LPCTSTR lpFile, LPCTSTR lpParameters, HWND hParent)
{
	ASSERT(lpFile != NULL); if(lpFile == NULL) return;
	ASSERT(lpFile[0] != 0); if(lpFile[0] == 0) return;

	SHELLEXECUTEINFO sei;
	ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));

	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.fMask = SEE_MASK_FLAG_NO_UI;
	sei.hwnd = hParent;
	sei.lpFile = lpFile;
	sei.lpParameters = lpParameters;
	sei.nShow = SW_SHOWNORMAL;

	const DWORD dwDummyErr = 0x19B5A28F;
	const DWORD dwPrevErr = GetLastError();
	SetLastError(dwDummyErr);

	if(ShellExecuteEx(&sei) == FALSE)
	{
		const DWORD dwErr = GetLastError();
		if(dwErr == dwDummyErr) { SetLastError(dwPrevErr); return; }

		std::basic_string<TCHAR> strErr = WU_FormatSystemMessage(dwErr);
		if(strErr.size() == 0) { SetLastError(dwPrevErr); return; }

		CString strMsg = TRL("&File");
		RemoveAcceleratorTip(&strMsg);
		strMsg += _T(": ");
		strMsg += lpFile;

		if(lpParameters != NULL)
		{
			strMsg += _T("\r\n");
			strMsg += TRL("Arguments");
			strMsg += _T(": ");
			strMsg += lpParameters;
		}

		strMsg += _T("\r\n\r\n");
		strMsg += strErr.c_str();

		MessageBox(hParent, strMsg, TRL("Password Safe"), MB_ICONWARNING | MB_OK);
	}

	SetLastError(dwPrevErr);
}

BOOL _FileAccessible(LPCTSTR lpFile)
{
	FILE *fp = NULL;
	_tfopen_s(&fp, lpFile, _T("rb"));
	if(fp == NULL) return FALSE;
	fclose(fp); fp = NULL;
	return TRUE;
}

BOOL _FileWritable(LPCTSTR lpFile)
{
	FILE *fp = NULL;
	_tfopen_s(&fp, lpFile, _T("ab"));
	if(fp == NULL) return FALSE;
	fclose(fp); fp = NULL;
	return TRUE;
}

C_FN_SHARE int _OpenLocalFile(LPCTSTR szFile, int nMode)
{
	// TCHAR szPath[1024];
	// TCHAR szFileTempl[1024];
	int nRet = 0;

	// GetModuleFileName(NULL, szFileTempl, MAX_PATH + 32);
	// _GetPathFromFile(szFileTempl, szPath);
	// _tcscpy_s(szFileTempl, _countof(szFileTempl), szPath);
	// _tcscat_s(szFileTempl, _countof(szFileTempl), _T("\\"));
	std_string strPath = Executable::instance().getPathOnly();

	// _tcscat_s(szFileTempl, _countof(szFileTempl), szFile);
	std_string strFile = strPath;
	strFile += szFile;

#ifndef _WIN32_WCE
	LPCTSTR lpVerb = _T("open");
	if(nMode == OLF_OPEN) { } // Default == OLF_OPEN
	else if(nMode == OLF_PRINT) lpVerb = _T("print");
	else if(nMode == OLF_EXPLORE) lpVerb = _T("explore");
	else { ASSERT(FALSE); }

	nRet = (int)(INT_PTR)ShellExecute(::GetActiveWindow(), lpVerb,
		strFile.c_str(), NULL, strPath.c_str(), KPSW_SHOWDEFAULT);
#else
	ASSERT(FALSE); // Implement before using on WinCE
#endif

	return nRet;
}

BOOL WU_GetFileNameSz(BOOL bOpenMode, LPCTSTR lpSuffix, LPTSTR lpStoreBuf, DWORD dwBufLen)
{
	DWORD dwFlags;
	CString strSample;
	CString strFilter;

	ASSERT(lpSuffix != NULL); if(lpSuffix == NULL) return FALSE;
	ASSERT(lpStoreBuf != NULL); if(lpStoreBuf == NULL) return FALSE;
	ASSERT(dwBufLen != 0); if(dwBufLen == 0) return FALSE;

	strSample = _T("*.");
	strSample += lpSuffix;

	strFilter = TRL("All Files");
	strFilter += _T(" (*.*)|*.*||");

	if(bOpenMode == FALSE)
	{
		dwFlags = OFN_LONGNAMES | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
		dwFlags |= OFN_EXTENSIONDIFFERENT;
		// OFN_EXPLORER = 0x00080000, OFN_ENABLESIZING = 0x00800000
		dwFlags |= 0x00080000 | 0x00800000 | OFN_NOREADONLYRETURN;
	}
	else
	{
		dwFlags = OFN_LONGNAMES | OFN_EXTENSIONDIFFERENT;
		// OFN_EXPLORER = 0x00080000, OFN_ENABLESIZING = 0x00800000
		dwFlags |= 0x00080000 | 0x00800000;
		dwFlags |= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	}

	CFileDialog dlg(bOpenMode, lpSuffix, strSample, dwFlags, strFilter, NULL);

	if(dlg.DoModal() == IDOK)
	{
		strSample = dlg.GetPathName();

		if((DWORD)strSample.GetLength() < dwBufLen)
		{
			_tcscpy_s(lpStoreBuf, dwBufLen, (LPCTSTR)strSample);
			return TRUE;
		}
		else _tcsncpy_s(lpStoreBuf, dwBufLen, (LPCTSTR)strSample, dwBufLen - 1);
	}

	return FALSE;
}

BOOL WU_OpenAppHelp(LPCTSTR lpTopicFile)
{
	ASSERT(lpTopicFile != NULL); if(lpTopicFile == NULL) return FALSE;

	TCHAR tszBuf[MAX_PATH * 2];
	GetApplicationDirectory(tszBuf, MAX_PATH * 2 - 2, TRUE, TRUE);

	CString str = _T("hh.exe ms-its:");
	str += tszBuf;
	str += _T("/");
	str += PWM_README_FILE;
	str += _T("::/");
	str += lpTopicFile;

	TWinExec(str, KPSW_SHOWDEFAULT);
	return TRUE;
}

UINT TWinExec(LPCTSTR lpCmdLine, WORD wCmdShow)
{
    STARTUPINFO sui;
	PROCESS_INFORMATION pi;
	BOOL bResult;

	ASSERT(lpCmdLine != NULL);
	if(lpCmdLine == NULL) return ERROR_PATH_NOT_FOUND;

	ZeroMemory(&sui, sizeof(STARTUPINFO));
	sui.cb = sizeof(STARTUPINFO);
	sui.wShowWindow = wCmdShow;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	const size_t dwCmdLen = _tcslen(lpCmdLine);
	LPTSTR lp = new TCHAR[dwCmdLen + 2];
	_tcscpy_s(lp, dwCmdLen + 2, lpCmdLine);

	bResult = CreateProcess(NULL, lp, NULL, NULL, FALSE, 0, NULL, NULL, &sui, &pi);

	// LPVOID lpMsgBuf;
	// FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL);
	// MessageBox(NULL, (LPCTSTR)lpMsgBuf, _T("Error"), MB_OK | MB_ICONINFORMATION);
	// LocalFree(lpMsgBuf);

	CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

	SAFE_DELETE_ARRAY(lp);

	return (bResult != FALSE) ? 32 : ERROR_FILE_NOT_FOUND;
}

BOOL WU_IsWin9xSystem()
{
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);

	return ((osvi.dwMajorVersion <= 4) ? TRUE : FALSE);
}

BOOL WU_SupportsMultiLineTooltips()
{
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);

	return (((osvi.dwMajorVersion >= 5) && (osvi.dwMinorVersion >= 1)) ?
		TRUE : FALSE);
}

std::basic_string<TCHAR> WU_GetTempFile()
{
	TCHAR szDir[MAX_PATH * 2];

	GetTempPath(MAX_PATH * 2 - 2, szDir);
	if(szDir[_tcslen(szDir) - 1] != _T('\\'))
		_tcscat_s(szDir, _countof(szDir), _T("\\"));

	std::basic_string<TCHAR> tszFile;
	DWORD dwIndex = 0;
	while(true)
	{
		dwIndex += 13;

		DWORD dwTest = ((DWORD)rand() << 16) + (DWORD)rand() + dwIndex;

		CString strTest;
		strTest.Format(_T("%s%s%u%s"), szDir, _T("Tmp"), dwTest, _T(".html"));
		if(_FileAccessible(strTest) == TRUE) continue;
		if(_FileWritable(strTest) == FALSE) continue;

		DeleteFile(strTest);
		tszFile = (LPCTSTR)strTest;
		break;
	}

	return tszFile;
}

std::basic_string<TCHAR> WU_GetUserName()
{
	std::basic_string<TCHAR> tsz;

	LPTSTR lpUser = new TCHAR[WU_MAX_USER_LEN + 1];
	memset(lpUser, 0, (WU_MAX_USER_LEN + 1) * sizeof(TCHAR));
	DWORD dwUserLength = WU_MAX_USER_LEN;
	GetUserName(lpUser, &dwUserLength);
	tsz += lpUser;
	SAFE_DELETE_ARRAY(lpUser);

	if(tsz.size() > 0) tsz += _T(" @ ");

	LPTSTR lpMachine = new TCHAR[WU_MAX_MACHINE_LEN + 1];
	memset(lpMachine, 0, (WU_MAX_MACHINE_LEN + 1) * sizeof(TCHAR));
	DWORD dwMachineLength = WU_MAX_MACHINE_LEN;
	GetComputerName(lpMachine, &dwMachineLength);
	tsz += lpMachine;
	SAFE_DELETE_ARRAY(lpMachine);

	return tsz;
}

/*
// Warning: this function is NOT multithreading-safe!
BOOL ContainsChildWindow(HWND hWndContainer, LPCTSTR lpChildWindowText)
{
	ASSERT(lpChildWindowText != NULL);
	if(lpChildWindowText == NULL) return FALSE;
	ASSERT(lpChildWindowText[0] != 0);
	if(lpChildWindowText[0] == 0) return FALSE;

	g_lpChildWindowText = lpChildWindowText;

	BOOL bWindowFound = FALSE;
	EnumChildWindows(hWndContainer, CcwEnumChildProc, (LPARAM)&bWindowFound);
	return bWindowFound;
}

BOOL CALLBACK CcwEnumChildProc(HWND hWnd, LPARAM lParam)
{
	// int nLength = GetWindowTextLength(hWnd);
	// if(nLength <= 0) return TRUE; // Continue enumeration

	int nLength = _tcslen(g_lpChildWindowText);

	int nAllocated = nLength + 4;
	LPTSTR lpText = new TCHAR[nAllocated];
	memset(lpText, 0, nAllocated * sizeof(TCHAR));

	GetWindowText(hWnd, lpText, nAllocated - 2);

	if(_tcsstr(lpText, g_lpChildWindowText) != NULL)
	{
		*((BOOL *)lParam) = TRUE;

		SAFE_DELETE_ARRAY(lpText);
		return FALSE; // Stop enumeration
	}

	SAFE_DELETE_ARRAY(lpText);
	return TRUE; // Continue enumeration
}
*/

void SafeActivateNextWindow(HWND hWndBase)
{
	HWND hWnd = GetWindow(hWndBase, GW_HWNDNEXT);

	WINDOWPLACEMENT wp;
	wp.length = sizeof(WINDOWPLACEMENT);

	while(1)
	{
		if(hWnd != hWndBase)
		{
			LONG lStyle = GetWindowLong(hWnd, GWL_STYLE);
			GetWindowPlacement(hWnd, &wp);

			if(((lStyle & WS_VISIBLE) == WS_VISIBLE) && (wp.showCmd != SW_SHOWMINIMIZED))
			{
				if(GetWindowTextLength(hWnd) != 0) break;
			}
		}

		hWnd = GetWindow(hWnd, GW_HWNDNEXT);
		if(hWnd == NULL) break;
	}

	SetWindowPos(hWndBase, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);

	if(hWnd != NULL)
	{
		SetForegroundWindow(hWnd);

		DWORD dwStartTime = timeGetTime();
		while(1)
		{
			if(GetForegroundWindow() == hWnd) break;

			if((timeGetTime() - dwStartTime) > 1000) break;
			Sleep(50);
		}
	}
}

HWND WU_ShowWindowInTaskbar(HWND hWndShow, HWND hParent, BOOL bShow)
{
	LONG_PTR sty;

	if(bShow == FALSE) // Hide
	{
		sty = GetWindowLongPtr(hWndShow, GWL_EXSTYLE);
		SetWindowLongPtr(hWndShow, GWL_EXSTYLE, sty & ~(WS_EX_APPWINDOW));

		sty = GetWindowLongPtr(hWndShow, GWL_STYLE);
		SetWindowLongPtr(hWndShow, GWL_STYLE, (sty | WS_CHILD) & ~(WS_POPUP));

		return SetParent(hWndShow, hParent);
	}
	else // Show
	{
		HWND h = SetParent(hWndShow, NULL);

		sty = GetWindowLongPtr(hWndShow, GWL_STYLE);
		SetWindowLongPtr(hWndShow, GWL_STYLE, (sty | WS_POPUP) & ~(WS_CHILD));

		sty = GetWindowLongPtr(hWndShow, GWL_EXSTYLE);
		SetWindowLongPtr(hWndShow, GWL_EXSTYLE, sty | WS_EX_APPWINDOW);

		return h;
	}
}

std::basic_string<TCHAR> WU_FormatSystemMessage(DWORD dwLastErrorCode)
{
	std::basic_string<TCHAR> str;
	LPTSTR lpBuffer = NULL;

	if(FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, dwLastErrorCode, 0, (LPTSTR)&lpBuffer, 1, NULL) != 0)
	{
		str = lpBuffer;
	}

	if(lpBuffer != NULL) { LocalFree(lpBuffer); lpBuffer = NULL; }

	return str;
}
