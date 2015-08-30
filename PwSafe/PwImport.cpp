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
#include "PwImport.h"
#include "../Util/MemUtil.h"
#include "../Util/StrUtil.h"

static char g_pNullString[4] = { 0, 0, 0, 0 };

CPwImport::CPwImport()
{
}

CPwImport::~CPwImport()
{
}

char *CPwImport::_FileToMemory(const TCHAR *pszFile, unsigned long *pFileSize)
{
	FILE *fp;
	unsigned long uFileSize;
	char *pData;

	ASSERT(pszFile != NULL);
	if(pszFile == NULL) return NULL;
	if(_tcslen(pszFile) == 0) return NULL;

	fp = _tfopen(pszFile, _T("rb"));
	if(fp == NULL) return NULL;

	fseek(fp, 0, SEEK_END);
	uFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	pData = new char[uFileSize + 1];
	if(pData == NULL) { fclose(fp); fp = NULL; return NULL; }
	pData[uFileSize] = 0; // Terminate buffer

	fread(pData, 1, uFileSize, fp);
	fclose(fp);

	*pFileSize = uFileSize; // Store file size
	return pData;
}

BOOL CPwImport::ImportCsvToDb(const TCHAR *pszFile, CPwManager *pMgr, DWORD dwGroupId)
{
	unsigned long uFileSize, i, j;
	char *pData;
	char *pProcessed;
	BOOL bInField;

	ASSERT(pMgr != NULL);

	pData = _FileToMemory(pszFile, &uFileSize);
	if(pData == NULL) return FALSE;

	pProcessed = new char[uFileSize+1];
	if(pProcessed == NULL) { SAFE_DELETE_ARRAY(pData); return FALSE; }

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

BOOL CPwImport::ImportCWalletToDb(const TCHAR *pszFile, CPwManager *pMgr)
{
	char *pData;
	CString strTitle, strURL, strUserName, strPassword, strNotes;
	DWORD uFileSize, i, b;
	CString str;
	CString strLastCategory = _T("General");
	DWORD dwLastGroupId;
	BOOL bInNotes = FALSE;

	ASSERT(pMgr != NULL);

	pData = _FileToMemory(pszFile, &uFileSize);
	if(pData == NULL) return FALSE;

	strTitle.Empty(); strURL.Empty(); strUserName.Empty();
	strPassword.Empty(); strNotes.Empty();

	i = DWORD_MAX;
	while(1) // Processing the file
	{
		str.Empty();

		while(1) // Loading one line to CString
		{
			i++;
			if(i >= uFileSize) break;

			if(pData[i] == '\n') break;
			if(pData[i] != '\r') str += pData[i];
		}

		// Add the entry
		if(((str.Left(1) == _T("[")) && (str.Right(1) == _T("]"))) ||
			(str == DEF_CW_CATEGORY) || (i >= uFileSize))
		{
			if((strTitle.IsEmpty() == FALSE) || (strUserName.IsEmpty() == FALSE) ||
				(strURL.IsEmpty() == FALSE) || (strPassword.IsEmpty() == FALSE))
			{
				strTitle.TrimLeft(); strTitle.TrimRight();
				strURL.TrimLeft(); strURL.TrimRight();
				strUserName.TrimLeft(); strUserName.TrimRight();
				strPassword.TrimLeft(); strPassword.TrimRight();
				strNotes.TrimLeft(); strNotes.TrimRight();

				PW_ENTRY pwTemplate;
				PW_TIME tNow;

				_GetCurrentPwTime(&tNow);
				memset(&pwTemplate, 0, sizeof(PW_ENTRY));
				pwTemplate.pszAdditional = (TCHAR *)(LPCTSTR)strNotes;
				pwTemplate.pszPassword = (TCHAR *)(LPCTSTR)strPassword;
				pwTemplate.pszTitle = (TCHAR *)(LPCTSTR)strTitle;
				pwTemplate.pszURL = (TCHAR *)(LPCTSTR)strURL;
				pwTemplate.pszUserName = (TCHAR *)(LPCTSTR)strUserName;
				pwTemplate.tCreation = tNow; CPwManager::_GetNeverExpireTime(&pwTemplate.tExpire);
				pwTemplate.tLastAccess = tNow; pwTemplate.tLastMod = tNow;
				pwTemplate.uGroupId = dwLastGroupId;
				pwTemplate.uImageId = _GetPreferredIcon((LPCTSTR)strTitle);
				pwTemplate.uPasswordLen = strPassword.GetLength();

				pMgr->AddEntry(&pwTemplate);
			}

			strTitle.Empty(); strURL.Empty(); strUserName.Empty();
			strPassword.Empty(); strNotes.Empty();
			bInNotes = FALSE;
		}

		if(i >= uFileSize) break;

		if((str.Left(1) == _T("[")) && (str.Right(1) == _T("]")))
		{
			strTitle = str;
			strTitle = strTitle.Left(strTitle.GetLength() - 1);
			strTitle = strTitle.Right(strTitle.GetLength() - 1);
			continue;
		}

		if(bInNotes == TRUE)
		{
			if(strNotes.GetLength() != 0) strNotes += _T("\r\n");
			strNotes += str;
		}

		if(str.Left(10) == _T("Category: "))
		{
			strLastCategory = str.Right(str.GetLength() - 10);
			strLastCategory.TrimLeft(); strLastCategory.TrimRight();

			while(1)
			{
				strLastCategory = strLastCategory.Left(strLastCategory.GetLength() - 1);
				if(strLastCategory.GetLength() == 0) break;
				if(strLastCategory.Right(1) == _T("("))
				{
					strLastCategory = strLastCategory.Left(strLastCategory.GetLength() - 2);
					break;
				}
				if(strLastCategory.GetLength() == 0) break;
			}

			if(strLastCategory.GetLength() == 0)
				strLastCategory = _T("General");
			dwLastGroupId = pMgr->GetGroupId((LPCTSTR)strLastCategory);
			if(dwLastGroupId == DWORD_MAX)
			{
				PW_GROUP pwT;
				PW_TIME tNow;
				_GetCurrentPwTime(&tNow);
				memset(&pwT, 0, sizeof(PW_GROUP));
				pwT.pszGroupName = (TCHAR *)(LPCTSTR)strLastCategory;
				pwT.tCreation = tNow; CPwManager::_GetNeverExpireTime(&pwT.tExpire);
				pwT.tLastAccess = tNow; pwT.tLastMod = tNow;
				pwT.uGroupId = 0; // 0 = create new group ID
				pwT.uImageId = _GetPreferredIcon((LPCTSTR)strLastCategory);
				pMgr->AddGroup(&pwT);
				dwLastGroupId = pMgr->GetGroupId((LPCTSTR)strLastCategory);
			}
			ASSERT(dwLastGroupId != DWORD_MAX);
		}

		if((str.Left(6) == _T("Notes:")) && (bInNotes == FALSE))
		{
			bInNotes = TRUE;
			str = str.Right(str.GetLength() - 6);
			if(str.GetLength() != 0) strNotes = str;
			continue;
		}

		b=0;
		if(str.Left(9) == _T("User ID: ")) {strUserName = str.Right(str.GetLength() - 9); b=1;}
		if(str.Left(7) == _T("Login: ")) {strUserName = str.Right(str.GetLength() - 7); b=1;}
		if(str.Left(10) == _T("Access #: ")) {strUserName = str.Right(str.GetLength() - 10); b=1;}
		if(str.Left(8) == _T("System: ")) {strUserName = str.Right(str.GetLength() - 8); b=1;}
		if(str.Left(9) == _T("Content: ")) {strUserName = str.Right(str.GetLength() - 9); b=1;}
		if(strUserName.GetLength() == 0)
		{
			if(str.Left(6) == _T("Date: ")) {strUserName = str.Right(str.GetLength() - 6); b=1;}
			if(str.Left(8) == _T("Issuer: ")) {strUserName = str.Right(str.GetLength() - 8); b=1;}
			if(str.Left(8) == _T("Number: ")) {strUserName = str.Right(str.GetLength() - 8); b=1;}
			if(str.Left(9) == _T("Network: ")) {strUserName = str.Right(str.GetLength() - 9); b=1;}
			if(str.Left(11) == _T("Ftp login: ")) {strUserName = str.Right(str.GetLength() - 11); b=1;}
		}

		if(str.Left(5) == _T("URL: ")) {strURL = str.Right(str.GetLength() - 5); b=1;}
		if(str.Left(10) == _T("Web site: ")) {strURL = str.Right(str.GetLength() - 10); b=1;}
		if(strURL.GetLength() == 0)
		{
			if(str.Left(19) == _T("Registered e-mail: ")) {strURL = str.Right(str.GetLength() - 19); b=1;}
		}

		if(str.Left(10) == _T("Password: ")) {strPassword = str.Right(str.GetLength() - 10); b=1;}
		if(strPassword.GetLength() == 0)
		{
			if(str.Left(5) == _T("PIN: ")) {strPassword = str.Right(str.GetLength() - 5); b=1;}
		}

		if((b == 0) && (bInNotes == FALSE))
		{
			if(strNotes.GetLength() != 0) strNotes += _T("\r\n");
			strNotes += str;
		}
	}

	SAFE_DELETE_ARRAY(pData);

	return TRUE;
}

BOOL CPwImport::ImportPwSafeToDb(const TCHAR *pszFile, CPwManager *pMgr)
{
	char *pData;
	unsigned long uFileSize, i;
	int nField, j;
	CString strGroup, strTitle, strUserName, strPassword, strNotes;
	DWORD dwGroupId;
	BOOL bInNotes = FALSE;

	ASSERT(pszFile != NULL); if(pszFile == NULL) return FALSE;
	ASSERT(pMgr != NULL); if(pMgr == NULL) return FALSE;

	pData = _FileToMemory(pszFile, &uFileSize);
	if(pData == NULL) return FALSE;

	nField = 0;
	i = 0; j = 0;
	while(1)
	{
		if(pData[i] == '\t')
		{
			nField++;

			if(nField == 1)
			{
				j = strGroup.ReverseFind('.');
				if(j != -1)
				{
					strTitle = strGroup.Right(strGroup.GetLength() - j - 1);
					strGroup = strGroup.Left(j);
				}
				else strTitle.Empty();
			}
		}
		else if((pData[i] == '\"') && (bInNotes == FALSE) && (nField == 3))
		{
			bInNotes = TRUE;
		}
		else if((pData[i] == '\"') && (bInNotes == TRUE) && (nField == 3))
		{
			bInNotes = FALSE;

			if(strNotes.GetAt(0) == '\"') strNotes = strNotes.Right(strNotes.GetLength() - 1);
			if(strNotes.Right(1) == "\"") strNotes = strNotes.Left(strNotes.GetLength() - 1);
		}
		else if((pData[i] == '\r') && (bInNotes == FALSE))
		{
			// Ignore all \r
		}
		else if((pData[i] == '\n') && (bInNotes == FALSE))
		{
			dwGroupId = pMgr->GetGroupId((LPCTSTR)strGroup);
			if(dwGroupId == DWORD_MAX)
			{
				PW_GROUP pwT;
				PW_TIME tNow;
				_GetCurrentPwTime(&tNow);
				pwT.pszGroupName = (TCHAR *)(LPCTSTR)strGroup;
				pwT.tCreation = tNow; CPwManager::_GetNeverExpireTime(&pwT.tExpire);
				pwT.tLastAccess = tNow; pwT.tLastMod = tNow;
				pwT.uGroupId = 0; // 0 = create new group ID
				pwT.uImageId = _GetPreferredIcon((LPCTSTR)strGroup);
				pMgr->AddGroup(&pwT);
				dwGroupId = pMgr->GetGroupId((LPCTSTR)strGroup);
			}
			ASSERT(dwGroupId != DWORD_MAX);

			PW_ENTRY pwTemplate;
			PW_TIME tNow;

			memset(&pwTemplate, 0, sizeof(PW_ENTRY));
			_GetCurrentPwTime(&tNow);
			pwTemplate.pszAdditional = (TCHAR *)(LPCTSTR)strNotes;
			pwTemplate.pszPassword = (TCHAR *)(LPCTSTR)strPassword;
			pwTemplate.pszTitle = (TCHAR *)(LPCTSTR)strTitle;
			pwTemplate.pszURL = g_pNullString;
			pwTemplate.pszUserName = (TCHAR *)(LPCTSTR)strUserName;
			pwTemplate.tCreation = tNow; CPwManager::_GetNeverExpireTime(&pwTemplate.tExpire);
			pwTemplate.tLastAccess = tNow; pwTemplate.tLastMod = tNow;
			pwTemplate.uImageId = _GetPreferredIcon((LPCTSTR)strTitle);
			pwTemplate.uPasswordLen = strPassword.GetLength();
			pwTemplate.uGroupId = dwGroupId;
			// UUID is zero -> create new UUID

			pMgr->AddEntry(&pwTemplate);

			strGroup.Empty(); strTitle.Empty(); strUserName.Empty();
			strPassword.Empty(); strNotes.Empty();
			nField = 0;
		}
		else
		{
			if(nField == 0) strGroup += pData[i];
			else if(nField == 1) strUserName += pData[i];
			else if(nField == 2) strPassword += pData[i];
			else if(nField == 3) strNotes += pData[i];
		}

		i++;
		if(i >= uFileSize) break;
	}

	EraseCString(&strGroup); EraseCString(&strTitle); EraseCString(&strUserName);
	EraseCString(&strPassword); EraseCString(&strNotes);

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
		{
			PW_ENTRY pwTemplate;
			PW_TIME tNow;

			memset(&pwTemplate, 0, sizeof(PW_ENTRY));
			_GetCurrentPwTime(&tNow);
			pwTemplate.pszAdditional = pNotes;
			pwTemplate.pszPassword = pPassword;
			pwTemplate.pszTitle = pTitle;
			pwTemplate.pszURL = pURL;
			pwTemplate.pszUserName = pUserName;
			pwTemplate.tCreation = tNow; CPwManager::_GetNeverExpireTime(&pwTemplate.tExpire);
			pwTemplate.tLastAccess = tNow; pwTemplate.tLastMod = tNow;
			pwTemplate.uGroupId = m_dwLastGroupId;
			pwTemplate.uImageId = _GetPreferredIcon(pTitle);
			pwTemplate.uPasswordLen = _tcslen(pPassword);
			// UUID is zero -> create new UUID

			m_pLastMgr->AddEntry(&pwTemplate);
		}
	}
}

unsigned long CPwImport::_GetPreferredIcon(const char *pszGroup)
{
	CString str = pszGroup;

	if(str.Find(_T("Windows")) != -1) return 38;
	if(str.Find(_T("System")) != -1) return 38;

	if(str.Find(_T("Network")) != -1) return 3;
	if(str.Find(_T("Connection")) != -1) return 3;
	if(str.Find(_T("VPN")) != -1) return 3;
	if(str.Find(_T("LAN")) != -1) return 3;

	if(str.Find(_T("Internet")) != -1) return 1;
	if(str.Find(_T("internet")) != -1) return 1;
	if(str.Find(_T("Web")) != -1) return 1;
	if(str.Find(_T("www")) != -1) return 1;
	if(str.Find(_T("WWW")) != -1) return 1;
	if(str.Find(_T("http")) != -1) return 1;
	if(str.Find(_T("HTTP")) != -1) return 1;
	if(str.Find(_T("ftp")) != -1) return 1;
	if(str.Find(_T("FTP")) != -1) return 1;

	if(str.Find(_T("eMail")) != -1) return 19;
	if(str.Find(_T("mail")) != -1) return 19;
	if(str.Find(_T("Post")) != -1) return 19;
	if(str.Find(_T("@")) != -1) return 19;

	if(str.Find(_T("Homebanking")) != -1) return 37;
	if(str.Find(_T("Banking")) != -1) return 37;
	if(str.Find(_T("Archive")) != -1) return 37;
	if(str.Find(_T("Collection")) != -1) return 37;
	if(str.Find(_T("Cash")) != -1) return 37;

	if(str.Find(_T("Explorer")) != -1) return 9;
	if(str.Find(_T("Netscape")) != -1) return 9;
	if(str.Find(_T("Mozilla")) != -1) return 9;
	if(str.Find(_T("Opera")) != -1) return 9;

	return 0;
}
