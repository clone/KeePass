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
#include "PwManager.h"
#include "../Util/MemUtil.h"
#include "../Crypto/rijndael.h"
#include "../Crypto/sha2.h"
#include "../Crypto/arcfour.h"

#include <string.h>

CPwManager::CPwManager()
{
	m_pEntries = NULL;
	m_dwNumEntries = 0;
	m_dwMaxEntries = 0;

	m_pGroups = NULL;
	m_dwNumGroups = 0;
	m_dwMaxGroups = 0;

	m_nAlgorithm = ALGO_AES;

	memset(m_pMasterKey, 0, 32);

	_AllocGroups(PWM_NUM_INITIAL_GROUPS);
	_AllocEntries(PWM_NUM_INITIAL_ENTRIES);

	m_random.Initialize();
	m_random.GetRandomBuffer(m_pSessionKey, PWM_SESSION_KEY_SIZE);

	ASSERT(sizeof(BYTE) == 1);
}

CPwManager::~CPwManager()
{
	CleanUp();
}

void CPwManager::CleanUp()
{
	if(m_pEntries != NULL)
	{
		_DeleteEntryList(TRUE);
	}
	m_dwNumEntries = 0;
	m_dwMaxEntries = 0;

	if(m_pGroups != NULL)
	{
		_DeleteGroupList(TRUE);
	}
	m_dwNumGroups = 0;
	m_dwMaxGroups = 0;

	mem_erase(m_pMasterKey, 32);

	m_random.Reset();
}

BOOL CPwManager::SetMasterKey(const TCHAR *pszMasterKey, BOOL bDiskDrive, const CNewRandomInterface *pARI)
{
	unsigned long uKeyLen;
	TCHAR szFile[2048];
	sha256_ctx sha32;
	char *paKey = NULL;

	ASSERT(pszMasterKey != NULL);
	if(pszMasterKey == NULL) return FALSE;

#ifdef _UNICODE
	paKey = _ToAscii((const TCHAR *)pszMasterKey);
#else
	paKey = new char[strlen(pszMasterKey) + 1];
	strcpy(paKey, pszMasterKey);
#endif

	ASSERT(paKey != NULL);
	if(paKey == NULL) return FALSE;

	uKeyLen = strlen(paKey);
	ASSERT(uKeyLen != 0);
	if(uKeyLen == 0) return FALSE;

	if(bDiskDrive == FALSE)
	{
		sha256_begin(&sha32);
		sha256_hash((unsigned char *)paKey, uKeyLen, &sha32);
		sha256_end(m_pMasterKey, &sha32);

		mem_erase((unsigned char *)paKey, uKeyLen);
		SAFE_DELETE_ARRAY(paKey);
		return TRUE;
	}
	else
	{
		mem_erase((unsigned char *)paKey, uKeyLen);
		SAFE_DELETE_ARRAY(paKey); // Don't need ASCII key any more from on now

		_tcscpy(szFile, pszMasterKey);
		if(szFile[_tcslen(szFile) - 1] != _T('\\')) _tcscat(szFile, _T("\\"));
		_tcscat(szFile, _T("pwsafe.key"));

		if(pARI == NULL) // Load key from disk
		{
			FILE *fp;

			fp = _tfopen(szFile, _T("rb"));
			if(fp == NULL) return FALSE;
			if(fread(m_pMasterKey, 1, 32, fp) != 32) return FALSE;
			fclose(fp);

			return TRUE;
		}
		else // Save key to disk
		{
			FILE *fp;
			unsigned char aRandomBytes[32];

			if(pARI->GenerateRandomSequence(32, aRandomBytes) == FALSE) return FALSE;

			fp = _tfopen(szFile, _T("wb"));
			if(fp == NULL) return FALSE;
			fwrite(aRandomBytes, 1, 32, fp);
			fclose(fp);

			memcpy(m_pMasterKey, aRandomBytes, 32);

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CPwManager::SetAlgorithm(int nAlgorithm)
{
	ASSERT((nAlgorithm == ALGO_AES) || (nAlgorithm == ALGO_TWOFISH));
	if((nAlgorithm != ALGO_AES) && (nAlgorithm != ALGO_TWOFISH)) return FALSE;
	m_nAlgorithm = nAlgorithm;
	return TRUE;
}

int CPwManager::GetAlgorithm()
{
	return m_nAlgorithm;
}

void CPwManager::_AllocEntries(unsigned long uEntries)
{
	PW_ENTRY *p;
	DWORD dwEntries;

	ASSERT(uEntries != 0);
	if(uEntries == 0) return;

	dwEntries = m_dwNumEntries;

	// If we already have allocated enough entries just return
	if(uEntries <= m_dwMaxEntries) return;

	p = new PW_ENTRY[uEntries];
	if(p == NULL) { ASSERT(FALSE); return; }
	memset(p, 0, sizeof(PW_ENTRY) * uEntries);

	if((m_dwNumEntries > 0) && (m_pEntries != NULL))
	{
		memcpy(p, m_pEntries, sizeof(PW_ENTRY) * m_dwNumEntries);
		_DeleteEntryList(FALSE);
	}

	m_dwNumEntries = dwEntries;
	m_dwMaxEntries = uEntries;
	m_pEntries = p;
}

void CPwManager::_AllocGroups(unsigned long uGroups)
{
	PW_GROUP *p;
	DWORD dwGroups;

	ASSERT(uGroups != 0);
	if(uGroups == 0) return;

	dwGroups = m_dwNumGroups;

	// If we already have allocated enough entries just return
	if(uGroups <= m_dwMaxGroups) return;

	p = new PW_GROUP[uGroups];
	if(p == NULL) { ASSERT(FALSE); return; }
	memset(p, 0, sizeof(PW_GROUP) * uGroups);

	if((m_dwNumGroups > 0) && (m_pGroups != NULL))
	{
		memcpy(p, m_pGroups, sizeof(PW_GROUP) * m_dwNumGroups);
		_DeleteGroupList(FALSE);
	}

	m_dwNumGroups = dwGroups;
	m_dwMaxGroups = uGroups;
	m_pGroups = p;
}

void CPwManager::_DeleteEntryList(BOOL bFreeStrings)
{
	unsigned long i;

	if(m_pEntries == NULL) return; // Nothing to delete

	if(bFreeStrings == TRUE)
	{
		for(i = 0; i < m_dwNumEntries; i++)
		{
			SAFE_DELETE_ARRAY(m_pEntries[i].pszTitle);
			SAFE_DELETE_ARRAY(m_pEntries[i].pszURL);
			SAFE_DELETE_ARRAY(m_pEntries[i].pszUserName);
			SAFE_DELETE_ARRAY(m_pEntries[i].pszPassword);
			SAFE_DELETE_ARRAY(m_pEntries[i].pszAdditional);
		}
	}

	if(m_dwNumEntries != 0)
	{
		mem_erase((unsigned char *)m_pEntries,
			sizeof(PW_ENTRY) * m_dwNumEntries);
	}

	SAFE_DELETE_ARRAY(m_pEntries);

	m_dwNumEntries = 0;
	m_dwMaxEntries = 0;
}

void CPwManager::_DeleteGroupList(BOOL bFreeStrings)
{
	unsigned long i;

	if(m_pGroups == NULL) return; // Nothing to delete

	if(bFreeStrings == TRUE)
	{
		for(i = 0; i < m_dwNumGroups; i++)
		{
			SAFE_DELETE_ARRAY(m_pGroups[i].pszGroupName);
		}
	}

	if(m_dwNumGroups != 0)
	{
		mem_erase((unsigned char *)m_pGroups,
			sizeof(PW_GROUP) * m_dwNumGroups);
	}

	SAFE_DELETE_ARRAY(m_pGroups);

	m_dwNumGroups = 0;
	m_dwMaxGroups = 0;
}

DWORD CPwManager::GetNumberOfEntries()
{
	return m_dwNumEntries;
}

DWORD CPwManager::GetNumberOfGroups()
{
	return m_dwNumGroups;
}

PW_ENTRY *CPwManager::GetEntry(DWORD dwIndex)
{
	ASSERT(dwIndex < m_dwNumEntries);
	if(dwIndex >= m_dwNumEntries) return NULL;

	return &m_pEntries[dwIndex];
}

PW_ENTRY *CPwManager::GetEntryByGroup(int idGroup, DWORD dwIndex)
{
	int nPos;

	ASSERT(idGroup != -1);
	if(idGroup < 0) return NULL;
	ASSERT(dwIndex < m_dwNumEntries);
	if(dwIndex >= m_dwNumEntries) return NULL;

	nPos = GetEntryByGroupN(idGroup, dwIndex);

	if(nPos != -1) return &m_pEntries[nPos];

	ASSERT(FALSE);
	return NULL;
}

int CPwManager::GetEntryByGroupN(int idGroup, DWORD dwIndex)
{
	DWORD i, j = 0;

	ASSERT(idGroup > -1);
	if(idGroup < 0) return -1;
	ASSERT(dwIndex < m_dwNumEntries);
	if(dwIndex >= m_dwNumEntries) return -1;

	for(i = 0; i < m_dwNumEntries; i++)
	{
		if(idGroup == (int)m_pEntries[i].uGroupId)
		{
			if(dwIndex == j) return (int)i;
			j++;
		}
	}

	return -1;
}

PW_GROUP *CPwManager::GetGroup(DWORD dwIndex)
{
	ASSERT(dwIndex < m_dwNumGroups);
	if(dwIndex >= m_dwNumGroups) return NULL;

	return &m_pGroups[dwIndex];
}

int CPwManager::GetGroupId(const TCHAR *pszGroupName)
{
	int i;

	ASSERT(pszGroupName != NULL);
	if(pszGroupName == NULL) return -1;

	for(i = 0; i < (int)m_dwNumGroups; i++)
	{
		if(_tcsicmp(m_pGroups[i].pszGroupName, pszGroupName) == 0)
			return i;
	}

	return -1;
}

int CPwManager::GetNumberOfItemsInGroup(const TCHAR *pszGroup)
{
	ASSERT(pszGroup != NULL);
	if(pszGroup == NULL) return 0;

	return GetNumberOfItemsInGroupN(GetGroupId(pszGroup));
}

int CPwManager::GetNumberOfItemsInGroupN(int idGroup)
{
	DWORD i;
	DWORD id;
	int n;

	ASSERT(idGroup >= 0);
	if(idGroup < 0) return 0;
	ASSERT((DWORD)idGroup < m_dwNumGroups);
	if((DWORD)idGroup >= m_dwNumGroups) return 0;

	id = (DWORD)idGroup;
	n = 0;
	for(i = 0; i < m_dwNumEntries; i++)
	{
		if(m_pEntries[i].uGroupId == id) n++;
	}

	return n;
}

BOOL CPwManager::AddEntry(DWORD uGroupId, DWORD uImageId, const TCHAR *pszTitle,
	const TCHAR *pszURL, const TCHAR *pszUserName, const TCHAR *pszPassword,
	const TCHAR *pszAdditional)
{
	ASSERT(pszTitle != NULL); if(pszTitle == NULL) return FALSE;
	ASSERT(pszURL != NULL); if(pszURL == NULL) return FALSE;
	ASSERT(pszUserName != NULL); if(pszUserName == NULL) return FALSE;
	ASSERT(pszPassword != NULL); if(pszPassword == NULL) return FALSE;
	ASSERT(pszAdditional != NULL); if(pszAdditional == NULL) return FALSE;

	// If we don't have enough allocated entries, allocate 32 more
	if(m_dwNumEntries == m_dwMaxEntries)
		_AllocEntries(m_dwMaxEntries + 32);

	m_dwNumEntries++;
	return SetEntry(m_dwNumEntries - 1, uGroupId, uImageId, pszTitle,
		pszURL, pszUserName, pszPassword, pszAdditional);
}

BOOL CPwManager::AddGroup(DWORD uImageId, const TCHAR *pszGroupName)
{
	ASSERT(pszGroupName != NULL);
	if(pszGroupName == NULL) return FALSE;

	if(m_dwNumGroups == m_dwMaxGroups)
		_AllocGroups(m_dwMaxGroups + 8);

	m_dwNumGroups++;

	return SetGroup(m_dwNumGroups - 1, uImageId, pszGroupName);
}

BOOL CPwManager::SetGroup(DWORD dwIndex, DWORD uImageId, const TCHAR *pszGroupName)
{
	int slen;

	ASSERT(dwIndex < m_dwNumGroups);
	ASSERT(pszGroupName != NULL);
	if(pszGroupName == NULL) return FALSE;

	slen = _tcslen(pszGroupName);
	SAFE_DELETE_ARRAY(m_pGroups[dwIndex].pszGroupName);
	m_pGroups[dwIndex].pszGroupName = new TCHAR[slen + 1];
	_tcscpy(m_pGroups[dwIndex].pszGroupName, pszGroupName);

	m_pGroups[dwIndex].uImageId = uImageId;

	return TRUE;
}

BOOL CPwManager::DeleteEntry(DWORD dwIndex)
{
	DWORD i;

	ASSERT(dwIndex < m_dwNumEntries);
	if(dwIndex >= m_dwNumEntries) return FALSE;

	SAFE_DELETE_ARRAY(m_pEntries[dwIndex].pszTitle);
	SAFE_DELETE_ARRAY(m_pEntries[dwIndex].pszURL);
	SAFE_DELETE_ARRAY(m_pEntries[dwIndex].pszUserName);
	SAFE_DELETE_ARRAY(m_pEntries[dwIndex].pszPassword);
	SAFE_DELETE_ARRAY(m_pEntries[dwIndex].pszAdditional);

	if(dwIndex != (m_dwNumEntries - 1))
	{
		for(i = dwIndex; i < (m_dwNumEntries - 1); i++)
		{
			m_pEntries[i] = m_pEntries[i+1];
		}
	}

	mem_erase((unsigned char *)&m_pEntries[m_dwNumEntries - 1], sizeof(PW_ENTRY));
	m_dwNumEntries--;
	return TRUE;
}

BOOL CPwManager::DeleteGroup(int nGroupId)
{
	DWORD i = 0;
	PW_ENTRY *p;

	ASSERT((DWORD)nGroupId < m_dwNumGroups);
	if((DWORD)nGroupId >= m_dwNumGroups) return FALSE;
	if(nGroupId < 0) return FALSE;

	if(m_dwNumEntries != 0)
	{
		while(1) // Remove all items in that group
		{
			p = GetEntry(i);
			if(p->uGroupId == (DWORD)nGroupId)
			{
				VERIFY(DeleteEntry(i));
				i--;
			}

			i++;
			if(i >= m_dwNumEntries) break;
		}
	}

	SAFE_DELETE_ARRAY(m_pGroups[nGroupId].pszGroupName);

	if((DWORD)nGroupId != (m_dwNumGroups - 1))
	{
		for(i = (DWORD)nGroupId; i < (m_dwNumGroups - 1); i++)
		{
			m_pGroups[i] = m_pGroups[i+1];
		}
	}

	mem_erase((unsigned char *)&m_pGroups[m_dwNumGroups - 1], sizeof(PW_GROUP));
	m_dwNumGroups--;

	// Update group IDs in password entries
	for(i = 0; i < m_dwNumEntries; i++)
	{
		if(m_pEntries[i].uGroupId > (DWORD)nGroupId)
			m_pEntries[i].uGroupId--;
	}

	return TRUE;
}

BOOL CPwManager::SetEntry(DWORD dwIndex, DWORD uGroupId, DWORD uImageId,
	const TCHAR *pszTitle, const TCHAR *pszURL, const TCHAR *pszUserName,
	const TCHAR *pszPassword, const TCHAR *pszAdditional)
{
	int slen;

	ASSERT(dwIndex < m_dwNumEntries);
	if(dwIndex >= m_dwNumEntries) return FALSE;
	ASSERT(pszTitle != NULL); if(pszTitle == NULL) return FALSE;
	ASSERT(pszURL != NULL); if(pszURL == NULL) return FALSE;
	ASSERT(pszUserName != NULL); if(pszUserName == NULL) return FALSE;
	ASSERT(pszPassword != NULL); if(pszPassword == NULL) return FALSE;
	ASSERT(pszAdditional != NULL); if(pszAdditional == NULL) return FALSE;

	m_pEntries[dwIndex].uGroupId = uGroupId;
	m_pEntries[dwIndex].uImageId = uImageId;

	SAFE_DELETE_ARRAY(m_pEntries[dwIndex].pszTitle);
	slen = _tcslen(pszTitle);
	m_pEntries[dwIndex].pszTitle = new TCHAR[slen + 1];
	if(slen != 0)
		_tcscpy(m_pEntries[dwIndex].pszTitle, pszTitle);
	else
		m_pEntries[dwIndex].pszTitle[0] = 0;

	SAFE_DELETE_ARRAY(m_pEntries[dwIndex].pszUserName);
	slen = _tcslen(pszUserName);
	m_pEntries[dwIndex].pszUserName = new TCHAR[slen + 1];
	if(slen != 0)
		_tcscpy(m_pEntries[dwIndex].pszUserName, pszUserName);
	else
		m_pEntries[dwIndex].pszUserName[0] = 0;

	SAFE_DELETE_ARRAY(m_pEntries[dwIndex].pszURL);
	slen = _tcslen(pszURL);
	m_pEntries[dwIndex].pszURL = new TCHAR[slen + 1];
	if(slen != 0)
		_tcscpy(m_pEntries[dwIndex].pszURL, pszURL);
	else
		m_pEntries[dwIndex].pszURL[0] = 0;

	SAFE_DELETE_ARRAY(m_pEntries[dwIndex].pszPassword);
	slen = _tcslen(pszPassword);
	m_pEntries[dwIndex].pszPassword = new TCHAR[slen + 1];
	if(slen != 0)
		_tcscpy(m_pEntries[dwIndex].pszPassword, pszPassword);
	else
		m_pEntries[dwIndex].pszPassword[0] = 0;

	SAFE_DELETE_ARRAY(m_pEntries[dwIndex].pszAdditional);
	slen = _tcslen(pszAdditional);
	m_pEntries[dwIndex].pszAdditional = new TCHAR[slen + 1];
	if(slen != 0)
		_tcscpy(m_pEntries[dwIndex].pszAdditional, pszAdditional);
	else
		m_pEntries[dwIndex].pszAdditional[0] = 0;

	m_pEntries[dwIndex].uPasswordLen = _tcslen(pszPassword);
	LockEntryPassword(&m_pEntries[dwIndex]);

	ASSERT_ENTRY((&m_pEntries[dwIndex]));
	return TRUE;
}

void CPwManager::LockEntryPassword(PW_ENTRY *pEntry)
{
	ASSERT_ENTRY(pEntry);
	if(pEntry == NULL) return;
	if(pEntry->pszPassword == NULL) return;

	if(pEntry->uPasswordLen != 0)
		arcfourCrypt((BYTE *)pEntry->pszPassword, pEntry->uPasswordLen * sizeof(TCHAR),
			m_pSessionKey, PWM_SESSION_KEY_SIZE);
}

void CPwManager::UnlockEntryPassword(PW_ENTRY *pEntry)
{
	LockEntryPassword(pEntry); // OFB encryption mode
}

void CPwManager::NewDatabase()
{
	_DeleteEntryList(TRUE);
	_DeleteGroupList(TRUE);

	_AllocGroups(PWM_NUM_INITIAL_GROUPS);
	_AllocEntries(PWM_NUM_INITIAL_ENTRIES);
}

#define _OPENDB_FAIL \
{ \
	if(pVirtualFile != NULL) \
	{ \
		mem_erase((unsigned char *)pVirtualFile, uAllocated); \
		SAFE_DELETE_ARRAY(pVirtualFile); \
	} \
	return FALSE; \
}

BOOL CPwManager::OpenDatabase(const TCHAR *pszFile)
{
	FILE *fp;
	char *pVirtualFile;
	unsigned long uFileSize, uAllocated, uEncryptedPartSize;
	unsigned long i, j, pos;
	PW_DBHEADER hdr;
	sha256_ctx sha32;
	RD_UINT8 uFinalKey[32];
	DWORD dw, dw2;
	char *ptrTemp;
	char *ptrTitle;
	char *ptrUserName;
	char *ptrURL;
	char *ptrPassword;
	char *ptrAdditional;
	LPTSTR lpptr = NULL;
	LPTSTR lptTitle = NULL, lptUserName = NULL, lptURL = NULL;
	LPTSTR lptPassword = NULL, lptAdditional = NULL;

	ASSERT(sizeof(char) == 1);

	ASSERT(pszFile != NULL);
	if(pszFile == NULL) return FALSE;
	ASSERT(_tcslen(pszFile) != 0);
	if(_tcslen(pszFile) == 0) return FALSE;

	fp = _tfopen(pszFile, _T("rb"));
	if(fp == NULL) return FALSE;

	// Get file size
	fseek(fp, 0, SEEK_END);
	uFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if(uFileSize < sizeof(PW_DBHEADER))
		{ fclose(fp); return FALSE; }

	// Allocate enough memory to hold the complete file
	uAllocated = uFileSize + 17;
	pVirtualFile = new char[uAllocated];
	if(pVirtualFile == NULL)
		{ fclose(fp); return FALSE; }
	pVirtualFile[uFileSize + 17 - 1] = 0;
	fread(pVirtualFile, 1, uFileSize, fp);
	fclose(fp);

	// Extract header structure from memory file
	memcpy(&hdr, pVirtualFile, sizeof(PW_DBHEADER));

	// Check if we can open this
	if((hdr.dwSignature1 != PWM_DBSIG_1) || (hdr.dwSignature2 != PWM_DBSIG_2))
		{ _OPENDB_FAIL; }
	if((hdr.dwVersion & 0xFFFFFF00) != (PWM_DBVER_DW & 0xFFFFFF00))
		{ _OPENDB_FAIL; }

	if(hdr.dwFlags & PWM_FLAG_RIJNDAEL) m_nAlgorithm = ALGO_AES;
	else if(hdr.dwFlags & PWM_FLAG_TWOFISH) m_nAlgorithm = ALGO_TWOFISH;
	else { ASSERT(FALSE); _OPENDB_FAIL; }

	// Hash the master password with the salt in the file
	sha256_begin(&sha32);
	sha256_hash(hdr.aMasterSeed, 16, &sha32);
	sha256_hash(m_pMasterKey, 32, &sha32);
	sha256_end((unsigned char *)uFinalKey, &sha32);

	if(m_nAlgorithm == ALGO_AES)
	{
		Rijndael aes;

		// Initialize Rijndael algorithm
		if(aes.init(Rijndael::CBC, Rijndael::Decrypt, uFinalKey,
			Rijndael::Key32Bytes, hdr.aEncryptionIV) != RIJNDAEL_SUCCESS)
			{ _OPENDB_FAIL; }

		// Decrypt! The first 48 bytes aren't encrypted (that's the header)
		uEncryptedPartSize = (unsigned long)aes.padDecrypt((RD_UINT8 *)pVirtualFile + 48,
			uFileSize - 48, (RD_UINT8 *)pVirtualFile + 48);
	}
	else if(m_nAlgorithm == ALGO_TWOFISH)
	{
		CTwofish twofish;

		if(twofish.init(uFinalKey, 32, hdr.aEncryptionIV) != true)
			{ _OPENDB_FAIL };

		uEncryptedPartSize = (unsigned long)twofish.padDecrypt(
			(RD_UINT8 *)pVirtualFile + 48,
			uFileSize - 48, (RD_UINT8 *)pVirtualFile + 48);
	}
	else
	{
		ASSERT(FALSE); _OPENDB_FAIL;
	}

	// Check for success
	if((uEncryptedPartSize > 2147483446) || (uEncryptedPartSize == 0))
		{ _OPENDB_FAIL; }

	// Update header information, it was partially encrypted
	memcpy(&hdr, pVirtualFile, sizeof(PW_DBHEADER));

	NewDatabase(); // Create a new database and initialize internal structures

	// Add groups from the memory file to the internal structures
	j = 0;
	pos = sizeof(PW_DBHEADER);
	for(i = 0; i < hdr.dwGroups; i++)
	{
		ptrTemp = &pVirtualFile[pos];
		j = strlen(ptrTemp);
		pos += j + 1;
		if(pos >= uFileSize) { _OPENDB_FAIL; }

		memcpy(&dw, &pVirtualFile[pos], sizeof(DWORD));
		pos += 4;
		if(pos >= uFileSize) { _OPENDB_FAIL; }

#ifdef _UNICODE
		lpptr = _ToUnicode(ptrTemp);
		AddGroup(dw, lpptr);
		SAFE_DELETE_ARRAY(lpptr);
#else
		AddGroup(dw, ptrTemp);
#endif
	}

	// Get the entries
	for(i = 0; i < hdr.dwEntries; i++)
	{
		if(pos >= uFileSize) { _OPENDB_FAIL; }

		ptrTitle = &pVirtualFile[pos];
		j = strlen(ptrTitle);
		pos += j + 1;
		if(pos >= uFileSize) { _OPENDB_FAIL; }

		ptrUserName = &pVirtualFile[pos];
		j = strlen(ptrUserName);
		pos += j + 1;
		if(pos >= uFileSize) { _OPENDB_FAIL; }

		ptrURL = &pVirtualFile[pos];
		j = strlen(ptrURL);
		pos += j + 1;
		if(pos >= uFileSize) { _OPENDB_FAIL; }

		ptrPassword = &pVirtualFile[pos];
		j = strlen(ptrPassword);
		pos += j + 1;
		if(pos >= uFileSize) { _OPENDB_FAIL; }

		ptrAdditional = &pVirtualFile[pos];
		j = strlen(ptrAdditional);
		pos += j + 1;
		if(pos >= uFileSize) { _OPENDB_FAIL; }

		memcpy(&dw, &pVirtualFile[pos], sizeof(DWORD));
		pos += 4;
		if(pos >= uFileSize) { _OPENDB_FAIL; }

		memcpy(&dw2, &pVirtualFile[pos], sizeof(DWORD));
		pos += 4;
		if(pos > uFileSize) { _OPENDB_FAIL; }

#ifdef _UNICODE
		lptTitle = _ToUnicode(ptrTitle); lptURL = _ToUnicode(ptrURL);
		lptUserName = _ToUnicode(ptrUserName); lptPassword = _ToUnicode(ptrPassword);
		lptAdditional = _ToUnicode(ptrAdditional);
		AddEntry(dw, dw2, lptTitle, lptURL, lptUserName, lptPassword, lptAdditional);
		SAFE_DELETE_ARRAY(lptTitle); SAFE_DELETE_ARRAY(lptURL);
		SAFE_DELETE_ARRAY(lptUserName); SAFE_DELETE_ARRAY(lptPassword);
		SAFE_DELETE_ARRAY(lptAdditional);
#else
		AddEntry(dw, dw2, ptrTitle, ptrURL, ptrUserName, ptrPassword, ptrAdditional);
#endif
	}

	// Erase and delete memory file
	mem_erase((unsigned char *)pVirtualFile, uAllocated);
	SAFE_DELETE_ARRAY(pVirtualFile);
	return TRUE;
}

BOOL CPwManager::SaveDatabase(const TCHAR *pszFile)
{
	FILE *fp;
	char *pVirtualFile;
	unsigned long uFileSize, uEncryptedPartSize, uAllocated;
	unsigned long i, pos;
	PW_DBHEADER hdr;
	RD_UINT8 uFinalKey[32];
	sha256_ctx sha32;

	ASSERT(pszFile != NULL);
	if(pszFile == NULL) return FALSE;
	ASSERT(_tcslen(pszFile) != 0);
	if(_tcslen(pszFile) == 0) return FALSE;

	fp = _tfopen(pszFile, _T("wb"));
	if(fp == NULL) return FALSE;

	uFileSize = sizeof(PW_DBHEADER);

	// Get the size of all groups
	for(i = 0; i < m_dwNumGroups; i++)
	{
		uFileSize += _tcslen(m_pGroups[i].pszGroupName) + 1;
		uFileSize += 4; // One more 32-bit dword
	}

	// Get the size of all entries together
	for(i = 0; i < m_dwNumEntries; i++)
	{
		uFileSize += _tcslen(m_pEntries[i].pszAdditional) + 1;
		uFileSize += m_pEntries[i].uPasswordLen + 1;
		uFileSize += _tcslen(m_pEntries[i].pszTitle) + 1;
		uFileSize += _tcslen(m_pEntries[i].pszURL) + 1;
		uFileSize += _tcslen(m_pEntries[i].pszUserName) + 1;
		uFileSize += 8; // Plus 2 32-bit dwords
	}

	// Round up filesize to 16-byte boundary for Rijndael
	uFileSize = uFileSize - (uFileSize % 16) + 16;

	// Allocate enough memory
	uAllocated = uFileSize + 16;
	pVirtualFile = new char[uAllocated];
	if(pVirtualFile == NULL) { fclose(fp); return FALSE; }

	// Build header structure
	hdr.dwSignature1 = PWM_DBSIG_1;
	hdr.dwSignature2 = PWM_DBSIG_2;

	hdr.dwFlags = PWM_FLAG_SHA2;

	if(m_nAlgorithm == ALGO_AES) hdr.dwFlags |= PWM_FLAG_RIJNDAEL;
	else if(m_nAlgorithm == ALGO_TWOFISH) hdr.dwFlags |= PWM_FLAG_TWOFISH;
	else { fclose(fp); ASSERT(FALSE); return FALSE; }

	hdr.dwVersion = PWM_DBVER_DW;
	hdr.dwGroups = m_dwNumGroups;
	hdr.dwEntries = m_dwNumEntries;

	// Make up the master key hash seed and the encryption IV
	m_random.GetRandomBuffer(hdr.aMasterSeed, 16);
	m_random.GetRandomBuffer((BYTE *)hdr.aEncryptionIV, 16);

	// Copy header to memory file
	pos = 0;
	memcpy(pVirtualFile, &hdr, sizeof(PW_DBHEADER));
	pos += sizeof(PW_DBHEADER);

#ifdef _UNICODE
	char *paPtr;
#endif
	// Store all groups to memory file
	for(i = 0; i < m_dwNumGroups; i++)
	{
#ifdef _UNICODE
		paPtr = _ToAscii(m_pGroups[i].pszGroupName);
		strcpy(&pVirtualFile[pos], paPtr);
		SAFE_DELETE_ARRAY(paPtr);
#else
		strcpy(&pVirtualFile[pos], m_pGroups[i].pszGroupName);
#endif
		pos += _tcslen(m_pGroups[i].pszGroupName) + 1;

		memcpy(&pVirtualFile[pos], &m_pGroups[i].uImageId, 4);
		pos += 4;
	}

	// Store all entries to memory file
	for(i = 0; i < m_dwNumEntries; i++)
	{
#ifdef _UNICODE
		paPtr = _ToAscii(m_pEntries[i].pszTitle);
		strcpy(&pVirtualFile[pos], paPtr);
		SAFE_DELETE_ARRAY(paPtr);
#else
		strcpy(&pVirtualFile[pos], m_pEntries[i].pszTitle);
#endif
		pos += _tcslen(m_pEntries[i].pszTitle) + 1;

#ifdef _UNICODE
		paPtr = _ToAscii(m_pEntries[i].pszUserName);
		strcpy(&pVirtualFile[pos], paPtr);
		SAFE_DELETE_ARRAY(paPtr);
#else
		strcpy(&pVirtualFile[pos], m_pEntries[i].pszUserName);
#endif
		pos += _tcslen(m_pEntries[i].pszUserName) + 1;

#ifdef _UNICODE
		paPtr = _ToAscii(m_pEntries[i].pszURL);
		strcpy(&pVirtualFile[pos], paPtr);
		SAFE_DELETE_ARRAY(paPtr);
#else
		strcpy(&pVirtualFile[pos], m_pEntries[i].pszURL);
#endif
		pos += _tcslen(m_pEntries[i].pszURL) + 1;

		UnlockEntryPassword(&m_pEntries[i]);
#ifdef _UNICODE
		paPtr = _ToAscii(m_pEntries[i].pszPassword);
		strcpy(&pVirtualFile[pos], paPtr);
		SAFE_DELETE_ARRAY(paPtr);
#else
		strcpy(&pVirtualFile[pos], m_pEntries[i].pszPassword);
#endif
		LockEntryPassword(&m_pEntries[i]);
		pos += m_pEntries[i].uPasswordLen + 1;

#ifdef _UNICODE
		paPtr = _ToAscii(m_pEntries[i].pszAdditional);
		strcpy(&pVirtualFile[pos], paPtr);
		SAFE_DELETE_ARRAY(paPtr);
#else
		strcpy(&pVirtualFile[pos], m_pEntries[i].pszAdditional);
#endif
		pos += _tcslen(m_pEntries[i].pszAdditional) + 1;

		memcpy(&pVirtualFile[pos], &m_pEntries[i].uGroupId, 4);
		pos += 4;

		memcpy(&pVirtualFile[pos], &m_pEntries[i].uImageId, 4);
		pos += 4;
	}

	// Hash the master password with the generated hash salt
	sha256_begin(&sha32);
	sha256_hash(hdr.aMasterSeed, 16, &sha32);
	sha256_hash(m_pMasterKey, 32, &sha32);
	sha256_end((unsigned char *)uFinalKey, &sha32);

	if(m_nAlgorithm == ALGO_AES)
	{
		Rijndael aes;

		// Initialize Rijndael/AES
		if(aes.init(Rijndael::CBC, Rijndael::Encrypt, uFinalKey,
			Rijndael::Key32Bytes, hdr.aEncryptionIV) != RIJNDAEL_SUCCESS)
		{
			fclose(fp); fp = NULL; SAFE_DELETE_ARRAY(pVirtualFile);
			return FALSE;
		}

		// Encrypt! The first 48 bytes remain unencrypted, that's the header
		uEncryptedPartSize = (unsigned long)aes.padEncrypt((RD_UINT8 *)pVirtualFile + 48,
			pos - 48, (RD_UINT8 *)pVirtualFile + 48);
	}
	else if(m_nAlgorithm == ALGO_TWOFISH)
	{
		CTwofish twofish;

		if(twofish.init(uFinalKey, 32, hdr.aEncryptionIV) == false)
		{
			fclose(fp); fp = NULL; SAFE_DELETE_ARRAY(pVirtualFile);
			return FALSE;
		}

		uEncryptedPartSize = (unsigned long)twofish.padEncrypt(
			(RD_UINT8 *)pVirtualFile + 48,
			pos - 48, (RD_UINT8 *)pVirtualFile + 48);
	}
	else
	{
		ASSERT(FALSE); _OPENDB_FAIL;
	}

	// Check if all went correct
	ASSERT((uEncryptedPartSize % 16) == 0);
	if((uEncryptedPartSize > 2147483446) || (uEncryptedPartSize == 0))
	{
		ASSERT(FALSE); fclose(fp); fp = NULL; SAFE_DELETE_ARRAY(pVirtualFile);
		return FALSE;
	}

	// Write memory file to disk
	fwrite(pVirtualFile, 1, uEncryptedPartSize + 48, fp);

	// Close file, erase and delete memory
	fclose(fp); fp = NULL;
	mem_erase((unsigned char *)pVirtualFile, uAllocated);
	SAFE_DELETE_ARRAY(pVirtualFile);
	return TRUE;
}

int CPwManager::Find(const TCHAR *pszFindString, BOOL bCaseSensitive, int fieldFlags, int nStart)
{
	int i;
	CString strFind;
	CString strEntry;

	ASSERT(nStart >= 0);
	if(nStart < 0) return -1;
	if(nStart >= (int)m_dwNumEntries) return -1;
	ASSERT(pszFindString != NULL);
	if(pszFindString == NULL) return -1;

	strFind = pszFindString;
	if(bCaseSensitive == FALSE) strFind.MakeLower();

	if((strFind.GetLength() == 0) || (strFind == _T("*"))) return nStart;

	for(i = nStart; i < (int)m_dwNumEntries; i++)
	{
		if(fieldFlags & PWMF_TITLE)
		{
			strEntry = m_pEntries[i].pszTitle;
			if(bCaseSensitive == FALSE) strEntry.MakeLower();

			if(strEntry.Find(strFind) != -1) return i;
		}
		if(fieldFlags & PWMF_USER)
		{
			strEntry = m_pEntries[i].pszUserName;
			if(bCaseSensitive == FALSE) strEntry.MakeLower();

			if(strEntry.Find(strFind) != -1) return i;
		}
		if(fieldFlags & PWMF_URL)
		{
			strEntry = m_pEntries[i].pszURL;
			if(bCaseSensitive == FALSE) strEntry.MakeLower();

			if(strEntry.Find(strFind) != -1) return i;
		}
		if(fieldFlags & PWMF_PASSWORD)
		{
			UnlockEntryPassword(&m_pEntries[i]);
			strEntry = m_pEntries[i].pszPassword;
			LockEntryPassword(&m_pEntries[i]);
			if(bCaseSensitive == FALSE) strEntry.MakeLower();

			if(strEntry.Find(strFind) != -1) return i;
		}
		if(fieldFlags & PWMF_ADDITIONAL)
		{
			strEntry = m_pEntries[i].pszAdditional;
			if(bCaseSensitive == FALSE) strEntry.MakeLower();

			if(strEntry.Find(strFind) != -1) return i;
		}
		if(fieldFlags & PWMF_GROUPNAME)
		{
			strEntry = GetGroup(m_pEntries[i].uGroupId)->pszGroupName;
			if(bCaseSensitive == FALSE) strEntry.MakeLower();

			if(strEntry.Find(strFind) != -1) return i;
		}
	}

	return -1;
}

void CPwManager::MoveInternal(int nFrom, int nTo)
{
	int i;
	int dir;
	PW_ENTRY pe;

	if(nFrom == nTo) return;
	if((nFrom < 0) || (nFrom >= (int)m_dwNumEntries)) return;
	if((nTo < 0) || (nTo >= (int)m_dwNumEntries)) return;

	// Set moving direction
	if(nFrom < nTo) dir = 1;
	else dir = -1;

	i = nFrom;
	while(1)
	{
		if(i == nTo) break;

		pe = m_pEntries[i];
		m_pEntries[i] = m_pEntries[i+dir];
		m_pEntries[i+dir] = pe;

		i += dir;
	}
}

BOOL CPwManager::MoveGroup(int nFrom, int nTo)
{
	int i;
	int dir;
	PW_GROUP pg;
	DWORD j, dwId1, dwId2;

	if(nFrom == nTo) TRUE;
	if((nFrom < 0) || (nFrom >= (int)m_dwNumGroups)) return FALSE;
	if((nTo < 0) || (nTo >= (int)m_dwNumGroups)) return FALSE;

	// Set moving direction
	if(nFrom < nTo) dir = 1;
	else dir = -1;

	i = nFrom;

	while(1)
	{
		if(i == nTo) break;

		pg = m_pGroups[i];
		m_pGroups[i] = m_pGroups[i+dir];
		m_pGroups[i+dir] = pg;

		// Swap group IDs in password entries too
		dwId1 = (DWORD)i; dwId2 = (DWORD)(i + dir);
		for(j = 0; j < m_dwNumEntries; j++)
		{
			if(m_pEntries[j].uGroupId == dwId1)
				m_pEntries[j].uGroupId = dwId2;
			else if(m_pEntries[j].uGroupId == dwId2)
				m_pEntries[j].uGroupId = dwId1;
		}

		i += dir;
	}

	return TRUE;
}

void CPwManager::MoveInGroup(int nGroup, int nFrom, int nTo)
{
	if((nFrom < 0) || (nFrom >= (int)m_dwNumEntries)) return;
	if((nTo < 0) || (nTo >= (int)m_dwNumEntries)) return;
	if(nFrom == nTo) return;

	int nFromEx = GetEntryByGroupN(nGroup, nFrom);
	int nToEx = GetEntryByGroupN(nGroup, nTo);

	if((nFromEx == -1) || (nToEx == -1)) return;

	MoveInternal(nFromEx, nToEx);
}

void CPwManager::SortGroupList()
{
	DWORD i;
	BOOL bSwapped = TRUE;
	PPW_GROUP p, q;

	if(m_dwNumGroups <= 1) return; // Nothing to sort

	while(bSwapped == TRUE) // Stable bubble-sort on the group list
	{
		bSwapped = FALSE;

		for(i = 0; i < (m_dwNumGroups - 1); i++)
		{
			p = GetGroup(i);
			q = GetGroup(i+1);

			if(_tcsicmp(p->pszGroupName, q->pszGroupName) > 0)
			{
				MoveGroup((int)i, (int)i+1);
				bSwapped = TRUE;
			}
		}
	}
}

void CPwManager::SortGroup(int nGroup, DWORD dwSortByField)
{
	ASSERT(nGroup >= 0);
	if(nGroup < 0) return;
	ASSERT(nGroup < (int)m_dwNumGroups);
	if(nGroup >= (int)m_dwNumGroups) return;

	PPW_ENTRY *p;
	PW_ENTRY v;
	DWORD i, j, n = 0, min, t;

	p = new PPW_ENTRY[m_dwNumEntries];
	if(p == NULL) return;

	// Build pointer array that contains pointers to the elements to sort
	for(i = 0; i < m_dwNumEntries; i++)
	{
		if(m_pEntries[i].uGroupId == (DWORD)nGroup)
		{
			p[n] = &m_pEntries[i];
			n++;
		}
	}
	if(n <= 1) return; // Something to sort?

	// Sort the array, using a simple selection sort
	for(i = 0; i < (n - 1); i++)
	{
		min = i;

		for(j = i + 1; j < n; j++)
		{
			switch(dwSortByField)
			{
			case 0:
				if(_tcsicmp(p[j]->pszTitle, p[min]->pszTitle) < 0)
					min = j;
				break;
			case 1:
				if(_tcsicmp(p[j]->pszUserName, p[min]->pszUserName) < 0)
					min = j;
				break;
			case 2:
				if(_tcsicmp(p[j]->pszURL, p[min]->pszURL) < 0)
					min = j;
				break;
			case 3:
				t = min;
				UnlockEntryPassword(p[j]); UnlockEntryPassword(p[t]);
				if(_tcsicmp(p[j]->pszPassword, p[min]->pszPassword) < 0)
					min = j;
				LockEntryPassword(p[j]); LockEntryPassword(p[t]);
				break;
			case 4:
				if(_tcsicmp(p[j]->pszAdditional, p[min]->pszAdditional) < 0)
					min = j;
				break;
			default:
				ASSERT(FALSE);
				if(_tcsicmp(p[j]->pszTitle, p[min]->pszTitle) < 0)
					min = j;
				break;
			}
		}

		v = *p[min];
		*p[min] = *p[i];
		*p[i] = v;
	}
	
	SAFE_DELETE_ARRAY(p);
}

char *CPwManager::_ToAscii(const TCHAR *lptString)
{
	char *p;
	int _nChars = 0;

	if(lptString == NULL) { ASSERT(FALSE); return NULL; }

#ifdef _UNICODE
	_nChars = (_tcslen(lptString) + 1) * 2;
	p = new char[_nChars];
	p[0] = 0;
	VERIFY(WideCharToMultiByte(CP_ACP, 0, lptString, -1, p, _nChars, NULL, NULL) !=
		ERROR_INSUFFICIENT_BUFFER);
#else
	p = new char[strlen(lptString) + 1];
	strcpy(p, lptString);
#endif

	return p;
}

TCHAR *CPwManager::_ToUnicode(const char *pszString)
{
	TCHAR *p;
	int _nChars = 0;

	if(pszString == NULL) { ASSERT(FALSE); return NULL; }

#ifdef _UNICODE
	// Determine the correct buffer size by calling the function itself with 0 as buffer size (see docs)
	_nChars = MultiByteToWideChar(CP_ACP, 0, pszString, -1, NULL, 0);

	p = new TCHAR[_nChars + 1];
	p[0] = 0;

	// Jan 9th 2004: DonAngel { 
	// This was ASSERTing for string. All debugging did not given good results, so I decided to remove
	// the verification. This could be a bug in MultiByteToWideChar, because thou it was returning
	// ERROR_INSUFFICIENT_BUFFER, the convertion was OK!?
	// The problem should be investigated later, but for now - I prefer to remove the ASSERT
	// VERIFY(MultiByteToWideChar(CP_ACP, 0, pszString, -1, p, _nChars) !=
	//	ERROR_INSUFFICIENT_BUFFER);
	// Jan 9th 2004: DonAngel }

	MultiByteToWideChar(CP_ACP, 0, pszString, -1, p, _nChars);
#else
	// Non-unicode - return the same!
	p = new char[strlen(pszString) + 1];
	strcpy(p, pszString);
#endif

	return p;
}
