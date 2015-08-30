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
	BOOL bUseUserDir = FALSE;
	BOOL bSuccess = FALSE;

	ASSERT(SI_REGSIZE >= (MAX_PATH * 2));

	GetApplicationDirectory(m_szFile, MAX_PATH * 2 - 2, TRUE, FALSE);
	_tcscat(m_szFile, _T("\\"));
	_tcscat(m_szFile, PWM_EXENAME);
	_tcscat(m_szFile, _T(".ini"));

	if(bReqWriteAccess == TRUE)
	{
		if(_FileWritable(m_szFile) == FALSE) bUseUserDir = TRUE;
	}
	else
	{
		if(_FileAccessible(m_szFile) == FALSE) bUseUserDir = TRUE;
	}

	if(bUseUserDir == TRUE)
	{
		m_szFile[0] = 0; m_szFile[1] = 0;
		SHGetSpecialFolderPath(NULL, m_szFile, CSIDL_APPDATA, TRUE);
		unsigned long uLen = (unsigned long)_tcslen(m_szFile);

		if(uLen != 0)
		{
			if(m_szFile[uLen - 1] != _T('\\'))
				_tcscat(m_szFile, _T("\\"));

			_tcscat(m_szFile, PWM_EXENAME);
			_tcscat(m_szFile, _T(".ini"));

			bSuccess = TRUE;
			if(bReqWriteAccess == TRUE)
			{
				if(_FileWritable(m_szFile) == FALSE) bSuccess = FALSE;
			}
			else
			{
				if(_FileAccessible(m_szFile) == FALSE) bSuccess = FALSE;
			}
		}

		if(bSuccess == FALSE)
		{
			_tcscpy(m_szFile, PWM_EXENAME);
			_tcscat(m_szFile, _T(".ini"));
		}
	}
}

CPrivateConfig::~CPrivateConfig()
{
}

BOOL CPrivateConfig::Set(const TCHAR *pszField, PCFG_IN TCHAR *pszValue)
{
	BOOL bRet;

	ASSERT(pszField != NULL); if(pszField == NULL) return FALSE;
	ASSERT(pszValue != NULL); if(pszValue == NULL) return FALSE;

	ASSERT(_tcslen(pszField) > 0); if(_tcslen(pszField) == 0) return FALSE;

#ifndef _WIN32_WCE
	bRet = WritePrivateProfileString(PWM_EXENAME, pszField, pszValue, m_szFile);
#else
	bRet = FALSE; // This will cause an assertion, implement before using on WinCE
#endif

	return bRet;
}

BOOL CPrivateConfig::Get(const TCHAR *pszField, PCFG_OUT TCHAR *pszValue)
{
	BOOL bRet;
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
