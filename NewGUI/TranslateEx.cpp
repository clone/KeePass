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
#include "../Util/MemUtil.h"
#include "TranslateEx.h"

static BOOL m_bTableLoaded = FALSE;
static char *m_pTranslationStrings = NULL;

static DWORD m_dwNumTrlStrings = 0;

static char *m_pDefString[MAX_TRANSLATION_STRINGS];
static char *m_pTrlString[MAX_TRANSLATION_STRINGS];

BOOL LoadTranslationTable(const char *pszTableName)
{
	FILE *fp = NULL;
	char szPath[2 * MAX_PATH];
	DWORD i = 0;
	DWORD dwLength = 0;
	BOOL bMode = FALSE;
	BOOL bScanning = FALSE;

	if(m_bTableLoaded == TRUE) FreeCurrentTranslationTable();

	ASSERT(pszTableName != NULL);
	if(pszTableName == NULL) return FALSE;
	if(strlen(pszTableName) == 0) return TRUE;
	if((strcmp(pszTableName, "Standard") == 0) || (strcmp(pszTableName, "English") == 0))
		return TRUE;

	GetModuleFileName(NULL, szPath, 2 * MAX_PATH);

	i = strlen(szPath) - 1;
	while(1)
	{
		if((szPath[i] == '\\') || (szPath[i] == '/'))
		{
			szPath[i+1] = 0;
			break;
		}
		i--;
		if(i == 0) break;
	}
	strcat(szPath, pszTableName);
	strcat(szPath, ".lng");

	fp = fopen(szPath, "rb");
	if(fp == NULL) return FALSE;

	fseek(fp, 0, SEEK_END);
	dwLength = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	m_pTranslationStrings = new char[dwLength+1];
	if(m_pTranslationStrings == NULL) { fclose(fp); return FALSE; }
	m_pTranslationStrings[dwLength] = 0;

	fread(m_pTranslationStrings, 1, dwLength, fp);

	fclose(fp);
	fp = NULL;

	m_dwNumTrlStrings = 0;

	bMode = TRL_MODE_DEF;
	bScanning = FALSE;
	for(i = 0; i < dwLength; i++)
	{
		if(m_pTranslationStrings[i] == '|')
		{
			if((bMode == TRL_MODE_DEF) && (bScanning == FALSE))
			{
				m_pDefString[m_dwNumTrlStrings] = &m_pTranslationStrings[i+1];
				m_pTranslationStrings[i] = 0;
				bScanning = TRUE;

				continue;
			}
			
			if((bMode == TRL_MODE_DEF) && (bScanning == TRUE))
			{
				m_pTranslationStrings[i] = 0;
				bMode = TRL_MODE_TRL;
				bScanning = FALSE;

				continue;
			}
			if((bMode == TRL_MODE_TRL) && (bScanning == FALSE))
			{
				m_pTrlString[m_dwNumTrlStrings] = &m_pTranslationStrings[i+1];
				m_pTranslationStrings[i] = 0;
				bScanning = TRUE;

				continue;
			}
			if((bMode == TRL_MODE_TRL) && (bScanning == TRUE))
			{
				m_pTranslationStrings[i] = 0;
				bMode = TRL_MODE_DEF;
				bScanning = FALSE;

				m_dwNumTrlStrings++;
				if(m_dwNumTrlStrings == MAX_TRANSLATION_STRINGS) break;

				continue;
			}

			// We should never get here
			ASSERT(FALSE);
		}
	}

	ASSERT(bMode == TRL_MODE_DEF);
	ASSERT(bScanning == FALSE);

	for(i = 0; i < m_dwNumTrlStrings; i++)
	{
		if(strlen(m_pTrlString[i]) == 0)
		{
			m_pTrlString[i] = m_pDefString[i];
		}
	}

	m_bTableLoaded = TRUE;
	return TRUE;
}

BOOL FreeCurrentTranslationTable()
{
	SAFE_DELETE_ARRAY(m_pTranslationStrings);

	if(m_bTableLoaded == FALSE) return FALSE;

	return TRUE;
}

const char *_TRL(const char *pszDefString)
{
	static unsigned long i;

	for(i = 0; i < m_dwNumTrlStrings; i++)
	{
		if(strcmp(m_pDefString[i], pszDefString) == 0) return m_pTrlString[i];
	}

	return pszDefString;
}
