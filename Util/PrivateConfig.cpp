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

#include "StdAfx.h"
#include "PrivateConfig.h"
#include "WinUtil.h"
#include "../PwSafe/PwManager.h"

CPrivateConfig::CPrivateConfig(BOOL bReqWriteAccess)
{
	ASSERT(SI_REGSIZE >= (MAX_PATH * 2));

	GetApplicationDirectory(m_szFileLocal, MAX_PATH * 2 - 2, TRUE, FALSE);
	_tcscat(m_szFileLocal, _T("\\"));
	_tcscat(m_szFileLocal, PWM_EXENAME);
	_tcscat(m_szFileLocal, _T(".ini"));

	m_szFileUser[0] = 0; m_szFileUser[1] = 0;

	HINSTANCE hShell32 = LoadLibrary("Shell32");
	if(hShell32 != NULL)
	{
		LPSHGETSPECIALFOLDERPATH lpGet = (LPSHGETSPECIALFOLDERPATH)GetProcAddress(hShell32, "SHGetSpecialFolderPathA");

		if(lpGet != NULL) lpGet(NULL, m_szFileUser, CSIDL_APPDATA, TRUE);
		else { ASSERT(FALSE); }

		lpGet = NULL; FreeLibrary(hShell32); hShell32 = NULL;
	} else { ASSERT(FALSE); }

	if(m_szFileUser[0] == 0)
	{
		_tcscpy(m_szFileUser, PWM_EXENAME);
		_tcscat(m_szFileUser, _T(".ini"));
	}
	else
	{
		DWORD uLen = (DWORD)_tcslen(m_szFileUser);
		if(uLen != 0)
		{
			if(m_szFileUser[uLen - 1] != _T('\\'))
				_tcscat(m_szFileUser, _T("\\"));

			_tcscat(m_szFileUser, PWM_EXENAME);
			CreateDirectory(m_szFileUser, NULL);

			_tcscat(m_szFileUser, _T("\\"));
			_tcscat(m_szFileUser, PWM_EXENAME);
			_tcscat(m_szFileUser, _T(".ini"));
		}
	}

	_tcscpy(m_szFileGeneric, PWM_EXENAME);
	_tcscat(m_szFileGeneric, _T(".ini"));

	m_nUseDir = 0;
	if(bReqWriteAccess == TRUE)
	{
		if(_FileWritable(m_szFileLocal) == FALSE)
		{
			m_nUseDir = 1;

			if(_FileWritable(m_szFileUser) == FALSE)
				m_nUseDir = 2;
		}
	}
	else // bReqWriteAccess == FALSE
	{
		if(_FileAccessible(m_szFileLocal) == FALSE)
		{
			m_nUseDir = 1;

			if(_FileAccessible(m_szFileUser) == FALSE)
				m_nUseDir = 2;
		}
	}
}

CPrivateConfig::~CPrivateConfig()
{
}

BOOL CPrivateConfig::Set(const TCHAR *pszField, PCFG_IN TCHAR *pszValue)
{
	BOOL bRet = FALSE;

	ASSERT(pszField != NULL); if(pszField == NULL) return FALSE;
	ASSERT(pszValue != NULL); if(pszValue == NULL) return FALSE;

	ASSERT(_tcslen(pszField) > 0); if(_tcslen(pszField) == 0) return FALSE;

#ifndef _WIN32_WCE
	if(m_nUseDir == 0)
		bRet = WritePrivateProfileString(PWM_EXENAME, pszField, pszValue, m_szFileLocal);

	if(bRet == FALSE)
	{
		if(m_nUseDir <= 1)
			bRet = WritePrivateProfileString(PWM_EXENAME, pszField, pszValue, m_szFileUser);

		if(bRet == FALSE)
			bRet = WritePrivateProfileString(PWM_EXENAME, pszField, pszValue, m_szFileGeneric);
	}
#else
	bRet = FALSE;
	ASSERT(FALSE); // Implement before using on WinCE
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
	if(m_nUseDir == 0)
		bRet = GetPrivateProfileString(PWM_EXENAME, pszField, chEmpty, pszValue, SI_REGSIZE, m_szFileLocal);

	if(bRet == FALSE)
	{
		if(m_nUseDir <= 1)
			bRet = GetPrivateProfileString(PWM_EXENAME, pszField, chEmpty, pszValue, SI_REGSIZE, m_szFileUser);

		if(bRet == FALSE)
			bRet = GetPrivateProfileString(PWM_EXENAME, pszField, chEmpty, pszValue, SI_REGSIZE, m_szFileGeneric);
	}
#else
	bRet = FALSE;
	ASSERT(FALSE); // Implement before using on WinCE
#endif

	return bRet;
}

BOOL CPrivateConfig::SetBool(const TCHAR *pszField, BOOL bValue)
{
	return Set(pszField, (bValue == TRUE) ? _T("True") : _T("False"));
}

BOOL CPrivateConfig::GetBool(const TCHAR *pszField, BOOL bDefault)
{
	TCHAR tszTemp[SI_REGSIZE];

	if(Get(pszField, tszTemp) == FALSE) return bDefault;

	if(_tcscmp(tszTemp, _T("True")) == 0) return TRUE;
	else if(_tcscmp(tszTemp, _T("False")) == 0) return FALSE;

	return bDefault;
}
