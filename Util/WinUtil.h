/*
  Copyright (c) 2003-2005, Dominik Reichl <dominik.reichl@t-online.de>
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

#ifndef ___WIN_UTIL_EX_H___
#define ___WIN_UTIL_EX_H___

#include "../StdAfx.h"
#include "../PwSafe/PwManager.h"

#define OLF_OPEN 0
#define OLF_PRINT 1
#define OLF_EXPLORE 2

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

C_FN_SHARE BOOL GetRegKeyEx(HKEY hkey, LPCTSTR lpSubKey, LPTSTR lpRetData);
C_FN_SHARE BOOL OpenUrlInNewBrowser(LPCTSTR lpURL);
C_FN_SHARE BOOL OpenUrlUsingPutty(LPCTSTR lpURL, LPCTSTR lpUser);

C_FN_SHARE void OpenUrlEx(LPCTSTR lpURL);

C_FN_SHARE BOOL _FileAccessible(LPCTSTR lpFile);
C_FN_SHARE BOOL _FileWritable(LPCTSTR lpFile);

C_FN_SHARE HINSTANCE _OpenLocalFile(TCHAR *szFile, int nMode);

// Get the application's directory; without \\ at the end
C_FN_SHARE BOOL GetApplicationDirectory(LPTSTR lpStoreBuf, DWORD dwBufLen, BOOL bFilterSpecial, BOOL bMakeURL);

C_FN_SHARE BOOL WU_GetFileNameSz(BOOL bOpenMode, LPCTSTR lpSuffix, LPTSTR lpStoreBuf, DWORD dwBufLen);

C_FN_SHARE BOOL WU_OpenAppHelp(LPCTSTR lpTopicFile);

#endif
