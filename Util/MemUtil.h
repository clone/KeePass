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

#ifndef ___MEMORY_UTILITIES_H___
#define ___MEMORY_UTILITIES_H___

#include "../StdAfx.h"
#include "../PwSafe/PwManager.h"

// Safely delete pointers
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if((p) != NULL) { delete (p); (p) = NULL; } }
#define SAFE_DELETE_ARRAY(p) { if((p) != NULL) { delete [](p); (p) = NULL; } }
#define SAFE_RELEASE(p)      { if((p) != NULL) { (p)->Release(); (p) = NULL; } }
#endif

// Maximum temporary buffer for SecureDeleteFile
#define SDF_BUF_SIZE 4096

// Securely erase memory
C_FN_SHARE void mem_erase(unsigned char *p, unsigned long u);

#ifndef _WIN32_WCE
C_FN_SHARE BOOL SecureDeleteFile(LPCSTR pszFilePath);
#endif

// Time conversion functions
C_FN_SHARE void _PackTimeToStruct(BYTE *pBytes, DWORD dwYear, DWORD dwMonth, DWORD dwDay, DWORD dwHour, DWORD dwMinute, DWORD dwSecond);
C_FN_SHARE void _UnpackStructToTime(BYTE *pBytes, DWORD *pdwYear, DWORD *pdwMonth, DWORD *pdwDay, DWORD *pdwHour, DWORD *pdwMinute, DWORD *pdwSecond);

// Getting the time
C_FN_SHARE void _GetCurrentPwTime(PW_TIME *p);

// Compare two PW_TIME structures, returns -1 if pt1<pt2, returns 1 if pt1>pt2,
// returns 0 if pt1=pt2
C_FN_SHARE int _pwtimecmp(const PW_TIME *pt1, const PW_TIME *pt2);

// Packs an array of integers to a TCHAR string
C_FN_SHARE void ar2str(TCHAR *tszString, INT *pArray, INT nItemCount);

// Unpacks a TCHAR string to an array of integers
C_FN_SHARE void str2ar(TCHAR *tszString, INT *pArray, INT nItemCount);

// Hash a file
C_FN_SHARE BOOL SHA256_HashFile(LPCTSTR lpFile, BYTE *pHash);

#endif
