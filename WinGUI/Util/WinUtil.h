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

#ifndef ___WIN_UTIL_EX_H___
#define ___WIN_UTIL_EX_H___

#include "../../KeePassLibCpp/SysDefEx.h"

#include <string>

#define OLF_OPEN 0
#define OLF_PRINT 1
#define OLF_EXPLORE 2

#define KPSW_SHOWDEFAULT SW_SHOWDEFAULT

// The following definitions must be at least 256, maximum 32767
#define WU_MAX_USER_LEN    1024
#define WU_MAX_MACHINE_LEN 1024

#define CFN_CLIPBOARD_VIEWER_IGNORE _T("Clipboard Viewer Ignore")

#ifndef _WIN32_WCE
void CopyStringToClipboard(const TCHAR *lptString);
void ClearClipboardIfOwner();

void RegisterOwnClipboardData(unsigned char* pData, unsigned long dwDataSize);

// Thanks to Gabe Martin for the contribution of the following
// two secure clipboard functions!
// http://sourceforge.net/tracker/index.php?func=detail&aid=1102906&group_id=95013&atid=609910
BOOL MakeClipboardDelayRender(HWND hOwner, HWND *phNextCB);
void CopyDelayRenderedClipboardData(const TCHAR *lptString);

void SetClipboardIgnoreFormat();
#endif

CString MakeRelativePathEx(LPCTSTR lpBaseFile, LPCTSTR lpTargetFile);
CString GetShortestAbsolutePath(LPCTSTR lpFilePath);

BOOL GetRegKeyEx(HKEY hkey, LPCTSTR lpSubKey, LPTSTR lpRetData);
BOOL OpenUrlInNewBrowser(LPCTSTR lpURL);
BOOL OpenUrlUsingPutty(LPCTSTR lpURL, LPCTSTR lpUser);

// If hParent is not NULL, the function will show an error message if
// the URL cannot be opened
void OpenUrlEx(LPCTSTR lpURL, HWND hParent);

// Internal functions
void OpenUrlShellExec(LPCTSTR lpURL, HWND hParent);
void OpenUrlProcess(LPCTSTR lpURL, HWND hParent);

void WU_SysShellExecute(LPCTSTR lpFile, LPCTSTR lpParameters, HWND hParent);

BOOL _FileAccessible(LPCTSTR lpFile);
BOOL _FileWritable(LPCTSTR lpFile);

// Must be exported:
C_FN_SHARE int _OpenLocalFile(LPCTSTR szFile, int nMode);
C_FN_SHARE BOOL WU_GetFileNameSz(BOOL bOpenMode, LPCTSTR lpSuffix, LPTSTR lpStoreBuf, DWORD dwBufLen);

BOOL WU_OpenAppHelp(LPCTSTR lpTopicFile);

UINT TWinExec(LPCTSTR lpCmdLine, WORD uCmdShow);

BOOL WU_IsWin9xSystem();
BOOL WU_SupportsMultiLineTooltips();

std::basic_string<TCHAR> WU_GetTempFile();
std::basic_string<TCHAR> WU_GetUserName();

// BOOL ContainsChildWindow(HWND hWndContainer, LPCTSTR lpChildWindowText);
// BOOL CALLBACK CcwEnumChildProc(HWND hWnd, LPARAM lParam);

void SafeActivateNextWindow(HWND hWndBase);

HWND WU_ShowWindowInTaskbar(HWND hWndShow, HWND hParent, BOOL bShow);

std::basic_string<TCHAR> WU_FormatSystemMessage(DWORD dwLastErrorCode);

#endif
