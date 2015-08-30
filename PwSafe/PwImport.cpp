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
#include "PwImport.h"
#include "../Util/MemUtil.h"

CPwImport::CPwImport()
{
}

CPwImport::~CPwImport()
{
}

BOOL CPwImport::ImportCsvToDb(const char *pszFile, CPwManager *pMgr, DWORD dwGroupId)
{
	FILE *fp;
	unsigned long uFileSize, i, j;
	char *pData;
	char *pProcessed;
	BOOL bInField;

	ASSERT(pszFile != NULL);
	ASSERT(pMgr != NULL);

	fp = fopen(pszFile, "rb");
	if(fp == NULL) return FALSE;

	fseek(fp, 0, SEEK_END);
	uFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	pData = new char[uFileSize+1];
	pProcessed = new char[uFileSize+1];
	if(pData == NULL) { fclose(fp); return FALSE; }
	if(pProcessed == NULL) { SAFE_DELETE_ARRAY(pData); fclose(fp); return FALSE; }

	fread(pData, 1, uFileSize, fp);
	fclose(fp);

	pData[uFileSize] = 0; // Terminate buffer

	// Last character mustn't be an escape character
	if(pData[uFileSize-1] == '\\') pData[uFileSize-1] = 0;

	j = 0; bInField = FALSE;
	for(i = 0; i < uFileSize; i++)
	{
		if(pData[i] == '\\')
		{
			i++; // Skip escape character
			pProcessed[j] = pData[i];
			j++;
		}
		else if(pData[i] == '\"')
		{
			if(bInField == TRUE)
			{
				pProcessed[j] = 0;
				j++;

				bInField = FALSE;
			}
			else
				bInField = TRUE;
		}
		else
		{
			if(bInField == TRUE)
			{
				pProcessed[j] = pData[i];
				j++;
			}
		}
	}
	if(bInField == TRUE) { SAFE_DELETE_ARRAY(pData); SAFE_DELETE_ARRAY(pProcessed); return FALSE; }

	m_pLastMgr = pMgr;
	m_dwLastGroupId = dwGroupId;
	_AddStringStreamToDb(pProcessed, j);

	SAFE_DELETE_ARRAY(pProcessed);
	SAFE_DELETE_ARRAY(pData);
	return TRUE;
}

void CPwImport::_AddStringStreamToDb(const char *pStream, unsigned long uStreamSize)
{
	unsigned long s;
	char *pTitle, *pUserName, *pPassword, *pURL, *pNotes;
	char *p = (char *)pStream;
	char *pEnd = (char *)pStream + uStreamSize;

	ASSERT(pStream != NULL);

	while(1)
	{
		if(p >= pEnd) break;
		pTitle = p;
		s = strlen(p);
		p += s + 1;

		if(p >= pEnd) break;
		pUserName = p;
		s = strlen(p);
		p += s + 1;

		if(p >= pEnd) break;
		pPassword = p;
		s = strlen(p);
		p += s + 1;

		if(p >= pEnd) break;
		pURL = p;
		s = strlen(p);
		p += s + 1;

		if(p >= pEnd) break;
		pNotes = p;
		s = strlen(p);
		p += s + 1;

		if((strcmp(pTitle, "Account") != 0) && (strcmp(pPassword, "Password") != 0))
			m_pLastMgr->AddEntry(m_dwLastGroupId, 0, pTitle, pURL, pUserName,
				pPassword, pNotes);
	}
}
