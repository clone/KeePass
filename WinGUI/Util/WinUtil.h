/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2006 Dominik Reichl <dominik.reichl@t-online.de>

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

#define OLF_OPEN 0
#define OLF_PRINT 1
#define OLF_EXPLORE 2

#define KPSW_SHOWDEFAULT SW_SHOWDEFAULT

#ifndef _WIN32_WCE
C_FN_SHARE void CopyStringToClipboard(const TCHAR *lptString);
C_FN_SHARE void ClearClipboardIfOwner();

// Thanks to Gabe Martin for the contribution of the following
// two secure clipboard functions!
// http://sourceforge.net/tracker/index.php?func=detail&aid=1102906&group_id=95013&atid=609910
C_FN_SHARE BOOL MakeClipboardDelayRender(HWND hOwner, HWND *phNextCB);
C_FN_SHARE void CopyDelayRenderedClipboardData(const TCHAR *lptString);
#endif

CPP_FN_SHARE CString MakeRelativePathEx(LPCTSTR lpBaseFile, LPCTSTR lpTargetFile);
CPP_FN_SHARE CString GetShortestAbsolutePath(LPCTSTR lpFilePath);

C_FN_SHARE BOOL GetRegKeyEx(HKEY hkey, LPCTSTR lpSubKey, LPTSTR lpRetData);
C_FN_SHARE BOOL OpenUrlInNewBrowser(LPCTSTR lpURL);
C_FN_SHARE BOOL OpenUrlUsingPutty(LPCTSTR lpURL, LPCTSTR lpUser);

C_FN_SHARE void OpenUrlEx(LPCTSTR lpURL);

C_FN_SHARE BOOL _FileAccessible(LPCTSTR lpFile);
C_FN_SHARE BOOL _FileWritable(LPCTSTR lpFile);

C_FN_SHARE int _OpenLocalFile(LPCTSTR szFile, int nMode);

C_FN_SHARE BOOL WU_GetFileNameSz(BOOL bOpenMode, LPCTSTR lpSuffix, LPTSTR lpStoreBuf, DWORD dwBufLen);

C_FN_SHARE BOOL WU_OpenAppHelp(LPCTSTR lpTopicFile);

C_FN_SHARE UINT TWinExec(LPCTSTR lpCmdLine, WORD uCmdShow);

#endif
