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
#include "PrivateConfig.h"
#include "../PwSafe/PwManager.h"

CPrivateConfig::CPrivateConfig()
{
	int i = 0;

	ASSERT(SI_REGSIZE >= (MAX_PATH * 2));

	GetModuleFileName(NULL, m_szFile, MAX_PATH * 2);
	for(i = _tcslen(m_szFile) - 1; i > 1; i--) // Extract dir
	{
		if((m_szFile[i] == _T('\\')) || (m_szFile[i] == _T('/')))
		{
			m_szFile[i] = 0;
			break;
		}
	}
	_tcscat(m_szFile, _T("\\"));
	_tcscat(m_szFile, PWM_EXENAME);
	_tcscat(m_szFile, _T(".ini"));
}

CPrivateConfig::~CPrivateConfig()
{
}

BOOL CPrivateConfig::Set(const TCHAR *pszField, PCFG_IN TCHAR *pszValue)
{
	BOOL bRet = FALSE;

	ASSERT(pszField != NULL);
	ASSERT(pszValue != NULL);
	if(pszField == NULL) return FALSE;
	if(pszValue == NULL) return FALSE;

	ASSERT(_tcslen(pszField) > 0);
	if(_tcslen(pszField) == 0) return FALSE;

#ifndef _WIN32_WCE
	bRet = WritePrivateProfileString(PWM_EXENAME, pszField, pszValue, m_szFile);
#else
	bRet = FALSE; // This will cause an assertion, implement before using on WinCE
#endif

	return bRet;
}

BOOL CPrivateConfig::Get(const TCHAR *pszField, PCFG_OUT TCHAR *pszValue)
{
	BOOL bRet = FALSE;
	TCHAR chEmpty[2] = { 0, 0 };

	ASSERT((pszField != NULL) && (pszValue != NULL));
	if((pszField == NULL) || (pszValue == NULL)) return FALSE;

	pszValue[0] = 0; pszValue[1] = 0; // Reset string

#ifndef _WIN32_WCE
	bRet = GetPrivateProfileString(PWM_EXENAME, pszField, chEmpty,
		pszValue, SI_REGSIZE, m_szFile);
#else
	bRet = FALSE;
	ASSERT(FALSE); // Implement before using on WinCE
#endif

	return bRet;
}

void _GetPathFromFile(TCHAR *pszFile, TCHAR *pszPath)
{
	unsigned int i;

	_tcscpy(pszPath, pszFile);

	for(i = _tcslen(pszFile) - 1; i > 1; i--)
	{
		if((pszFile[i] == _T('\\')) || (pszFile[i] == _T('/')))
		{
			pszPath[i] = 0;
			pszPath[i+1] = 0;
			break;
		}
	}

}

HINSTANCE _OpenLocalFile(TCHAR *szFile, int nMode)
{
	TCHAR szPath[1024];
	TCHAR szFileTempl[1024];
	HINSTANCE hInst = NULL;

	GetModuleFileName(NULL, szFileTempl, MAX_PATH + 32);
	_GetPathFromFile(szFileTempl, szPath);
	_tcscpy(szFileTempl, szPath);
	_tcscat(szFileTempl, "\\");
	_tcscat(szFileTempl, szFile);

#ifndef _WIN32_WCE
	if(nMode == OLF_OPEN)
	{
		hInst = ShellExecute(::GetActiveWindow(), _T("open"), szFileTempl,
			NULL, szPath, SW_SHOWNORMAL);
	}
	else if(nMode == OLF_PRINT)
	{
		hInst = ShellExecute(::GetActiveWindow(), _T("print"), szFileTempl,
			NULL, szPath, SW_SHOWNORMAL);
	}
	else if(nMode == OLF_EXPLORE)
	{
		hInst = ShellExecute(::GetActiveWindow(), _T("explore"), szFileTempl,
			NULL, szPath, SW_SHOWNORMAL);
	}
	else
	{
		ASSERT(FALSE);
	}
#else
	hInst = NULL;
	ASSERT(FALSE); // Implement before using on WinCE
#endif

	return(hInst);
}
