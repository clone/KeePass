/*
  Copyright (c) 2003, Dominik Reichl <dominik.reichl@t-online.de>
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
	for(i = strlen(m_szFile) - 1; i > 1; i--) // Extract dir
	{
		if((m_szFile[i] == '\\') || (m_szFile[i] == '/'))
		{
			m_szFile[i] = 0;
			break;
		}
	}
	strcat(m_szFile, "\\");
	strcat(m_szFile, PWM_EXENAME);
	strcat(m_szFile, ".ini");
}

CPrivateConfig::~CPrivateConfig()
{
}

BOOL CPrivateConfig::Set(char *pszField, PCFG_IN char *pszValue)
{
	BOOL bRet = FALSE;

	ASSERT(pszField != NULL);
	ASSERT(pszValue != NULL);
	if(pszField == NULL) return FALSE;
	if(pszValue == NULL) return FALSE;

	ASSERT(strlen(pszField) > 0);
	if(strlen(pszField) == 0) return FALSE;

	bRet = WritePrivateProfileString(PWM_EXENAME, pszField, pszValue, m_szFile);
	ASSERT(bRet == TRUE);
	return bRet;
}

BOOL CPrivateConfig::Get(char *pszField, PCFG_OUT char *pszValue)
{
	BOOL bRet = FALSE;
	char chEmpty[2] = { 0, 0 };

	ASSERT(pszField != NULL);
	if(pszField == NULL) return FALSE;

	pszValue[0] = 0; pszValue[1] = 0;

	bRet = GetPrivateProfileString(PWM_EXENAME, pszField, chEmpty,
		pszValue, SI_REGSIZE, m_szFile);
	return bRet;
}

void _GetPathFromFile(char *pszFile, char *pszPath)
{
	unsigned int i = 0;

	strcpy(pszPath, pszFile);

	for(i = strlen(pszFile) - 1; i > 1; i--)
	{
		if((pszFile[i] == '\\') || (pszFile[i] == '/'))
		{
			pszPath[i] = 0;
			pszPath[i+1] = 0;
			break;
		}
	}

}

HINSTANCE _OpenLocalFile(char *szFile, int nMode)
{
	char szPath[1024];
	char szFileTempl[1024];
	HINSTANCE hInst = NULL;

	GetModuleFileName(NULL, szFileTempl, MAX_PATH + 32);
	_GetPathFromFile(szFileTempl, szPath);
	strcpy(szFileTempl, szPath);
	strcat(szFileTempl, "\\");
	strcat(szFileTempl, szFile);

	if(nMode == OLF_OPEN)
	{
		hInst = ShellExecute(::GetActiveWindow(), "open", szFileTempl,
			NULL, szPath, SW_SHOWNORMAL);
	}
	else if(nMode == OLF_PRINT)
	{
		hInst = ShellExecute(::GetActiveWindow(), "print", szFileTempl,
			NULL, szPath, SW_SHOWNORMAL);
	}
	else if(nMode == OLF_EXPLORE)
	{
		hInst = ShellExecute(::GetActiveWindow(), "explore", szFileTempl,
			NULL, szPath, SW_SHOWNORMAL);
	}
	else
	{
		ASSERT(FALSE);
	}

	return(hInst);
}
