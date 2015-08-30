/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2008 Dominik Reichl <dominik.reichl@t-online.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "StdAfx.h"
#include "PwManager.h"
#include "Crypto/ARCFour.h"
#include "Util/Base64.h"
#include "Util/PwUtil.h"
#include "Util/MemUtil.h"
#include "Util/StrUtil.h"
#include "Util/TranslateEx.h"

using boost::scoped_ptr;

static PW_TIME g_pwTimeNever = { 2999, 12, 28, 23, 59, 59 };
static char g_pNullString[4] = { 0, 0, 0, 0 };

#ifdef _UNICODE
// #error Unicode builds are not supported. Switch to ANSI configuration.
#endif

CPwManager::CPwManager()
{
	m_pEntries = NULL;
	m_dwNumEntries = 0;
	m_dwMaxEntries = 0;

	m_pGroups = NULL;
	m_dwNumGroups = 0;
	m_dwMaxGroups = 0;

	m_pLastEditedEntry = NULL;
	m_nAlgorithm = ALGO_AES;
	m_dwKeyEncRounds = PWM_STD_KEYENCROUNDS;

	memset(m_pMasterKey, 0, 32);

	_AllocGroups(PWM_NUM_INITIAL_GROUPS);
	_AllocEntries(PWM_NUM_INITIAL_ENTRIES);

	m_random.GetRandomBuffer(m_pSessionKey, PWM_SESSION_KEY_SIZE);
}

CPwManager::~CPwManager()
{
	this->CleanUp();
}

void CPwManager::InitPrimaryInstance()
{
	ASSERT((m_pLastEditedEntry == NULL) && (memcmp(m_pMasterKey, g_pNullString, 4) == 0));
	ASSERT((m_nAlgorithm == ALGO_AES) && (m_dwKeyEncRounds == PWM_STD_KEYENCROUNDS));

	DWORD dwInitXorShift[4];
	m_random.GetRandomBuffer((BYTE *)&dwInitXorShift, 16);
	srandXorShift(dwInitXorShift);

	ASSERT(sizeof(BYTE) == 1); ASSERT(sizeof(DWORD) == 4);
}

void CPwManager::CleanUp()
{
	_DeleteEntryList(TRUE);
	m_dwNumEntries = 0;
	m_dwMaxEntries = 0;

	_DeleteGroupList(TRUE);
	m_dwNumGroups = 0;
	m_dwMaxGroups = 0;

	m_pLastEditedEntry = NULL;

	mem_erase(m_pMasterKey, 32);

	m_strDefaultUserName.clear();
	m_vSearchHistory.clear();
}

int CPwManager::SetMasterKey(const TCHAR *pszMasterKey, BOOL bDiskDrive, const TCHAR *pszSecondKey, const CNewRandomInterface *pARI, BOOL bOverwrite)
{
	size_t uKeyLen, uKeyLen2 = 0, uFileSize, uRead;
	TCHAR szFile[2048];
	sha256_ctx sha32;
	char *paKey = NULL;
	char *paKey2 = NULL;
	unsigned char aFileKey[32];
	unsigned char aPasswordKey[32];
	BOOL bReadNormal;
	std::vector<BYTE> vExtKey;

	ASSERT(pszMasterKey != NULL); if(pszMasterKey == NULL) return PWE_INVALID_PARAM;

#ifdef _UNICODE
	ASSERT(sizeof(TCHAR) >= 2);
	paKey = _StringToAnsi(pszMasterKey);
#else
	ASSERT(sizeof(TCHAR) == 1);
	size_t sizeANSIKeyBuffer = _tcslen(pszMasterKey) + 1;
	paKey = new char[sizeANSIKeyBuffer];
	ASSERT(paKey != NULL); if(paKey == NULL) return PWE_NO_MEM;
	strcpy_s(paKey, sizeANSIKeyBuffer, pszMasterKey);
#endif

	ASSERT(paKey != NULL); if(paKey == NULL) return PWE_NO_MEM;

	if(pszSecondKey != NULL)
	{
#ifdef _UNICODE
		ASSERT(sizeof(TCHAR) >= 2);
		paKey2 = _StringToAnsi(pszSecondKey);
#else
		ASSERT(sizeof(TCHAR) == 1);
		size_t sizeByteBuffer = _tcslen(pszSecondKey) + 1;
		paKey2 = new char[sizeByteBuffer];
		ASSERT(paKey2 != NULL); if(paKey2 == NULL) return PWE_NO_MEM;
		strcpy_s(paKey2, sizeByteBuffer, pszSecondKey);
#endif

		ASSERT(paKey2 != NULL); if(paKey2 == NULL) return PWE_NO_MEM;

		uKeyLen2 = szlen(paKey2);
		ASSERT(uKeyLen2 != 0);
	}

	uKeyLen = strlen(paKey);
	ASSERT(uKeyLen != 0);
	if(uKeyLen == 0) { SAFE_DELETE_ARRAY(paKey); return PWE_INVALID_KEY; }

	if(bDiskDrive == FALSE)
	{
		sha256_begin(&sha32);
		sha256_hash((unsigned char *)paKey, static_cast<unsigned long>(uKeyLen),
			&sha32);
		sha256_end((unsigned char *)m_pMasterKey, &sha32);

		mem_erase((unsigned char *)paKey, uKeyLen);
		SAFE_DELETE_ARRAY(paKey);
		return PWE_SUCCESS;
	}
	else if(CBase64Codec::IsBase64UrlStringT(pszMasterKey))
	{
		mem_erase((unsigned char *)paKey, uKeyLen);
		SAFE_DELETE_ARRAY(paKey); // Don't need ASCII key any more from on now

		const bool bDec64 = CBase64Codec::DecodeUrlT(pszMasterKey, vExtKey);
		if(bDec64 && (vExtKey.size() > 0))
		{
			sha256_begin(&sha32);
			sha256_hash(&vExtKey[0], static_cast<unsigned long>(vExtKey.size()), &sha32);
			sha256_end((unsigned char *)aFileKey, &sha32);
		}
		else return PWE_KEYPROV_INVALID_KEY;
		mem_erase(&vExtKey[0], vExtKey.size());

		if(pszSecondKey == NULL) // External source only
		{
			memcpy(m_pMasterKey, aFileKey, 32);
			mem_erase((unsigned char *)aFileKey, 32);
			return PWE_SUCCESS;
		}
		else // pszSecondKey != NULL
		{
			sha256_begin(&sha32);
			sha256_hash((unsigned char *)paKey2,
				static_cast<unsigned long>(uKeyLen2), &sha32);
			sha256_end((unsigned char *)aPasswordKey, &sha32);

			mem_erase((unsigned char *)paKey2, uKeyLen2);
			SAFE_DELETE_ARRAY(paKey);

			sha256_begin(&sha32);
			sha256_hash(aPasswordKey, 32, &sha32);
			sha256_hash(aFileKey, 32, &sha32);
			sha256_end((unsigned char *)m_pMasterKey, &sha32);

			mem_erase((unsigned char *)aPasswordKey, 32);
			mem_erase((unsigned char *)aFileKey, 32);
			return PWE_SUCCESS;
		}
	}
	else // With key file
	{
		mem_erase((unsigned char *)paKey, uKeyLen);
		SAFE_DELETE_ARRAY(paKey); // Don't need ASCII key any more from on now

		if(pszSecondKey == NULL) // Key file only
		{
			_tcscpy_s(szFile, _countof(szFile), pszMasterKey);
			if(szFile[_tcslen(szFile) - 1] == _T('\\'))
				_tcscat_s(szFile, _countof(szFile), PWS_DEFAULT_KEY_FILENAME);

			if(pARI == NULL) // If pARI is NULL: load key from disk
			{
				FILE *fp = NULL;
				_tfopen_s(&fp, szFile, _T("rb"));
				if(fp == NULL) return PWE_NOFILEACCESS_READ_KEY;
				fseek(fp, 0, SEEK_END);
				uFileSize = (unsigned long)ftell(fp);
				fseek(fp, 0, SEEK_SET);

				bReadNormal = TRUE;

				if(uFileSize == 32)
				{
					if(fread(m_pMasterKey, 1, 32, fp) != 32)
					{
						fclose(fp); fp = NULL;
						return PWE_FILEERROR_READ;
					}

					bReadNormal = FALSE;
				}
				else if(uFileSize == 64)
				{
					if(CPwUtil::LoadHexKey32(fp, m_pMasterKey) == FALSE)
					{
						fseek(fp, 0, SEEK_SET);
					}
					else bReadNormal = FALSE;
				}

				if(bReadNormal == TRUE)
				{
					sha256_begin(&sha32);
					while(1)
					{
						uRead = (unsigned long)fread((unsigned char *)szFile, 1, 2048, fp);
						if(uRead == 0) break;
						sha256_hash((unsigned char *)szFile,
							static_cast<unsigned long>(uRead), &sha32);
						if(uRead != 2048) break;
					}
					sha256_end((unsigned char *)m_pMasterKey, &sha32);
				}

				fclose(fp); fp = NULL;
				return PWE_SUCCESS;
			}
			else // pARI is not NULL: save key to disk
			{
				FILE *fp = NULL;
				unsigned char aRandomBytes[32];

				_tfopen_s(&fp, szFile, _T("rb")); // Does the file exist already?
				if((fp != NULL) && (bOverwrite == FALSE)) { fclose(fp); fp = NULL; return PWE_NOFILEACCESS_READ_KEY; }
				if(fp != NULL) { fclose(fp); fp = NULL; } // We must close it before opening for write

				if(pARI->GenerateRandomSequence(32, aRandomBytes) == FALSE) return PWE_INVALID_RANDOMSOURCE;

				fp = NULL;
				_tfopen_s(&fp, szFile, _T("wb"));
				if(fp == NULL) return PWE_NOFILEACCESS_WRITE;
				if(CPwUtil::SaveHexKey32(fp, aRandomBytes) == FALSE) { fclose(fp); fp = NULL; return PWE_FILEERROR_WRITE; }
				fclose(fp); fp = NULL;

				memcpy(m_pMasterKey, aRandomBytes, 32);
				return PWE_SUCCESS;
			}
		}
		else // pszSecondKey != NULL
		{
			_tcscpy_s(szFile, _countof(szFile), pszMasterKey);
			if(szFile[_tcslen(szFile) - 1] == _T('\\'))
				_tcscat_s(szFile, _countof(szFile), PWS_DEFAULT_KEY_FILENAME);

			if(pARI == NULL) // If pARI is NULL: load key from disk
			{
				FILE *fp = NULL;
				_tfopen_s(&fp, szFile, _T("rb"));
				if(fp == NULL) return PWE_NOFILEACCESS_READ_KEY;
				fseek(fp, 0, SEEK_END);
				uFileSize = (unsigned long)ftell(fp);
				fseek(fp, 0, SEEK_SET);

				bReadNormal = TRUE;

				if(uFileSize == 32)
				{
					if(fread(aFileKey, 1, 32, fp) != 32)
					{
						fclose(fp); fp = NULL;
						return PWE_FILEERROR_READ;
					}

					bReadNormal = FALSE;
				}
				else if(uFileSize == 64)
				{
					if(CPwUtil::LoadHexKey32(fp, aFileKey) == FALSE)
					{
						fseek(fp, 0, SEEK_SET);
					}
					else bReadNormal = FALSE;
				}

				if(bReadNormal == TRUE)
				{
					sha256_begin(&sha32);
					while(1)
					{
						uRead = (unsigned long)fread((unsigned char *)szFile, 1, 2048, fp);
						if(uRead == 0) break;
						sha256_hash((unsigned char *)szFile,
							static_cast<unsigned long>(uRead), &sha32);
						if(uRead != 2048) break;
					}
					sha256_end((unsigned char *)aFileKey, &sha32);
				}

				fclose(fp); fp = NULL;

				sha256_begin(&sha32);
				sha256_hash((unsigned char *)paKey2,
					static_cast<unsigned long>(uKeyLen2), &sha32);
				sha256_end((unsigned char *)aPasswordKey, &sha32);

				mem_erase((unsigned char *)paKey2, uKeyLen2);
				SAFE_DELETE_ARRAY(paKey);

				sha256_begin(&sha32);
				sha256_hash(aPasswordKey, 32, &sha32);
				sha256_hash(aFileKey, 32, &sha32);
				sha256_end((unsigned char *)m_pMasterKey, &sha32);

				mem_erase((unsigned char *)aPasswordKey, 32);
				mem_erase((unsigned char *)aFileKey, 32);
				return PWE_SUCCESS;
			}
			else // pARI is not NULL: save key to disk
			{
				FILE *fp = NULL;
				unsigned char aRandomBytes[32];

				_tfopen_s(&fp, szFile, _T("rb")); // Does the file exist already?
				if((fp != NULL) && (bOverwrite == FALSE)) { fclose(fp); fp = NULL; return PWE_NOFILEACCESS_READ_KEY; }
				if(fp != NULL) { fclose(fp); fp = NULL; } // We must close it before opening for write

				if(pARI->GenerateRandomSequence(32, aRandomBytes) == FALSE) return PWE_INVALID_RANDOMSOURCE;

				_tfopen_s(&fp, szFile, _T("wb"));
				if(fp == NULL) return PWE_NOFILEACCESS_WRITE;
				if(CPwUtil::SaveHexKey32(fp, aRandomBytes) == FALSE) { fclose(fp); fp = NULL; return PWE_FILEERROR_WRITE; }
				fclose(fp); fp = NULL;

				ASSERT(uKeyLen2 != 0);
				sha256_begin(&sha32);
				sha256_hash((unsigned char *)paKey2,
					static_cast<unsigned long>(uKeyLen2), &sha32);
				sha256_end((unsigned char *)aPasswordKey, &sha32);

				mem_erase((unsigned char *)paKey2, uKeyLen2);
				SAFE_DELETE_ARRAY(paKey);

				sha256_begin(&sha32);
				sha256_hash(aPasswordKey, 32, &sha32);
				sha256_hash(aRandomBytes, 32, &sha32);
				sha256_end((unsigned char *)m_pMasterKey, &sha32);

				mem_erase((unsigned char *)aPasswordKey, 32);
				mem_erase((unsigned char *)aFileKey, 32);
				return PWE_SUCCESS;
			}
		}
	}

	// return PWE_UNKNOWN; // Unreachable anyway
}

BOOL CPwManager::SetAlgorithm(int nAlgorithm)
{
	ASSERT((nAlgorithm == ALGO_AES) || (nAlgorithm == ALGO_TWOFISH));
	if((nAlgorithm != ALGO_AES) && (nAlgorithm != ALGO_TWOFISH)) return FALSE;

	m_nAlgorithm = nAlgorithm;
	return TRUE;
}

int CPwManager::GetAlgorithm() const
{
	return m_nAlgorithm;
}

void CPwManager::_AllocEntries(DWORD uEntries)
{
	ASSERT((uEntries != 0) && (uEntries != DWORD_MAX));
	if(uEntries == 0) return;

	const DWORD dwEntries = m_dwNumEntries;

	// If we already have allocated enough entries just return
	if(uEntries <= m_dwMaxEntries) return;

	PW_ENTRY *p = new PW_ENTRY[uEntries];
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

void CPwManager::_AllocGroups(DWORD uGroups)
{
	ASSERT((uGroups != 0) && (uGroups != DWORD_MAX));
	if((uGroups == 0) || (uGroups == DWORD_MAX)) return;

	const DWORD dwGroups = m_dwNumGroups;

	// If we already have allocated enough entries just return
	if(uGroups <= m_dwMaxGroups) return;

	PW_GROUP *p = new PW_GROUP[uGroups];
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
	if(m_pEntries == NULL) return; // Nothing to delete

	if(bFreeStrings == TRUE)
	{
		for(DWORD uCurrentEntry = 0; uCurrentEntry < m_dwNumEntries; ++uCurrentEntry)
		{
			SAFE_DELETE_ARRAY(m_pEntries[uCurrentEntry].pszTitle);
			SAFE_DELETE_ARRAY(m_pEntries[uCurrentEntry].pszURL);
			SAFE_DELETE_ARRAY(m_pEntries[uCurrentEntry].pszUserName);
			SAFE_DELETE_ARRAY(m_pEntries[uCurrentEntry].pszPassword);
			SAFE_DELETE_ARRAY(m_pEntries[uCurrentEntry].pszAdditional);
			SAFE_DELETE_ARRAY(m_pEntries[uCurrentEntry].pszBinaryDesc);
			SAFE_DELETE_ARRAY(m_pEntries[uCurrentEntry].pBinaryData);
		}
	}

	if(m_dwNumEntries != 0) // Erase ALL data
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
	if(m_pGroups == NULL) return; // Nothing to delete

	if(bFreeStrings == TRUE)
	{
		for(DWORD uCurrentGroup = 0; uCurrentGroup < m_dwNumGroups; ++uCurrentGroup)
		{
			SAFE_DELETE_ARRAY(m_pGroups[uCurrentGroup].pszGroupName);
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

DWORD CPwManager::GetNumberOfEntries() const
{
	return m_dwNumEntries;
}

DWORD CPwManager::GetNumberOfGroups() const
{
	return m_dwNumGroups;
}

PW_ENTRY *CPwManager::GetEntry(DWORD dwIndex)
{
	// ASSERT(dwIndex < m_dwNumEntries);
	if(dwIndex >= m_dwNumEntries) return NULL;

	return &m_pEntries[dwIndex];
}

PW_ENTRY *CPwManager::GetEntryByGroup(DWORD idGroup, DWORD dwIndex)
{
	DWORD dwPos;

	ASSERT(idGroup != DWORD_MAX);
	if(idGroup == DWORD_MAX) return NULL;
	ASSERT(dwIndex < m_dwNumEntries);
	if(dwIndex >= m_dwNumEntries) return NULL;

	dwPos = GetEntryByGroupN(idGroup, dwIndex);

	if(dwPos != DWORD_MAX) return &m_pEntries[dwPos];

	ASSERT(FALSE);
	return NULL;
}

DWORD CPwManager::GetEntryByGroupN(DWORD idGroup, DWORD dwIndex) const
{
	DWORD uCurrentEntry, uEntryInGroupCounter = 0;

	ASSERT(idGroup != DWORD_MAX);
	if(idGroup == DWORD_MAX) return DWORD_MAX;
	ASSERT(dwIndex < m_dwNumEntries);
	if(dwIndex >= m_dwNumEntries) return DWORD_MAX;

	for(uCurrentEntry = 0; uCurrentEntry < m_dwNumEntries; uCurrentEntry++)
		if(idGroup == m_pEntries[uCurrentEntry].uGroupId)
		{
			if(dwIndex == uEntryInGroupCounter) return uCurrentEntry;
			uEntryInGroupCounter++;
		}

	return DWORD_MAX;
}

PW_ENTRY *CPwManager::GetEntryByUuid(const BYTE *pUuid)
{
	ASSERT(pUuid != NULL); if(pUuid == NULL) return NULL;

	DWORD dwEntryIndex = GetEntryByUuidN(pUuid);

	// ASSERT(dwEntryIndex != DWORD_MAX); // Do not assert!
	if(dwEntryIndex == DWORD_MAX) return NULL;

	return &m_pEntries[dwEntryIndex];
}

DWORD CPwManager::GetEntryByUuidN(const BYTE *pUuid) const
{
	ASSERT(pUuid != NULL); if(pUuid == NULL) return DWORD_MAX;

	for(DWORD dw = 0; dw < m_dwNumEntries; dw++)
		if(memcmp(m_pEntries[dw].uuid, pUuid, 16) == 0) return dw;

	// Don't ASSERT here, just return DWORD_MAX
	return DWORD_MAX;
}

DWORD CPwManager::GetEntryPosInGroup(__in_ecount(1) const PW_ENTRY *pEntry) const
{
	DWORD uCurrentEntry, pos = 0;

	ASSERT(pEntry != NULL); if(pEntry == NULL) return DWORD_MAX;

	for(uCurrentEntry = 0; uCurrentEntry < m_dwNumEntries; uCurrentEntry++)
		if(m_pEntries[uCurrentEntry].uGroupId == pEntry->uGroupId)
		{
			if(memcmp(m_pEntries[uCurrentEntry].uuid, pEntry->uuid, 16) == 0)
				return pos;

			pos++;
		}

	return DWORD_MAX;
}

PW_ENTRY *CPwManager::GetLastEditedEntry()
{
	return m_pLastEditedEntry;
}

PW_GROUP *CPwManager::GetGroup(DWORD dwIndex)
{
	ASSERT(dwIndex < m_dwNumGroups);
	if(dwIndex >= m_dwNumGroups) return NULL;

	return &m_pGroups[dwIndex];
}

PW_GROUP *CPwManager::GetGroupById(DWORD idGroup)
{
	DWORD dwIndex = GetGroupByIdN(idGroup);
	if(dwIndex == DWORD_MAX) return NULL;
	return &m_pGroups[dwIndex];
}

DWORD CPwManager::GetGroupByIdN(DWORD idGroup) const
{
	DWORD uCurrentEntry;

	for(uCurrentEntry = 0; uCurrentEntry < m_dwNumGroups; uCurrentEntry++)
		if(m_pGroups[uCurrentEntry].uGroupId == idGroup)
			return uCurrentEntry;

	return DWORD_MAX;
}

DWORD CPwManager::GetGroupId(const TCHAR *pszGroupName) const
{
	DWORD i;

	ASSERT(pszGroupName != NULL); if(pszGroupName == NULL) return DWORD_MAX;

	for(i = 0; i < m_dwNumGroups; i++)
	{
		if(_tcsicmp(m_pGroups[i].pszGroupName, pszGroupName) == 0)
			return m_pGroups[i].uGroupId;
	}

	return DWORD_MAX;
}

DWORD CPwManager::GetGroupIdByIndex(DWORD uGroupIndex) const
{
	if(uGroupIndex >= m_dwNumGroups) return DWORD_MAX;
	return m_pGroups[uGroupIndex].uGroupId;
}

DWORD CPwManager::GetNumberOfItemsInGroup(const TCHAR *pszGroup) const
{
	ASSERT(pszGroup != NULL); if(pszGroup == NULL) return 0;

	return GetNumberOfItemsInGroupN(GetGroupId(pszGroup));
}

DWORD CPwManager::GetNumberOfItemsInGroupN(DWORD idGroup) const
{
	ASSERT(idGroup != DWORD_MAX);
	if(idGroup == DWORD_MAX) return 0;

	DWORD n = 0;
	for(DWORD i = 0; i < m_dwNumEntries; i++)
		if(m_pEntries[i].uGroupId == idGroup) n++;

	return n;
}

BOOL CPwManager::AddEntry(__in_ecount(1) const PW_ENTRY *pTemplate)
{
	PW_ENTRY pT;

	// Don't ASSERT_ENTRY the pTemplate!
	ASSERT(pTemplate != NULL); if(pTemplate == NULL) return FALSE;
	ASSERT((pTemplate->uGroupId != 0) && (pTemplate->uGroupId != DWORD_MAX));
	if((pTemplate->uGroupId == 0) || (pTemplate->uGroupId == DWORD_MAX)) return FALSE;

	// If we don't have enough allocated entries, allocate 32 more
	if(m_dwNumEntries == m_dwMaxEntries)
		_AllocEntries(m_dwMaxEntries + 32);

	pT = *pTemplate; // Copy parameter to local temporary variable

	if(CPwUtil::IsZeroUUID(pT.uuid) == TRUE) // Shall we create a new UUID?
	{
		randCreateUUID(pT.uuid, &m_random); // Create it!
		ASSERT(GetEntryByUuidN(pT.uuid) == DWORD_MAX);
	}

	// Map NULL pointers to valid pointers to a zero-length string
	if(pT.pszTitle == NULL) pT.pszTitle = (TCHAR *)g_pNullString;
	if(pT.pszUserName == NULL) pT.pszUserName = (TCHAR *)g_pNullString;
	if(pT.pszURL == NULL) pT.pszURL = (TCHAR *)g_pNullString;
	if(pT.pszPassword == NULL) pT.pszPassword = (TCHAR *)g_pNullString;
	if(pT.pszAdditional == NULL) pT.pszAdditional = (TCHAR *)g_pNullString;
	if(pT.pszBinaryDesc == NULL) pT.pszBinaryDesc = (TCHAR *)g_pNullString;

	m_dwNumEntries++;
	return SetEntry(m_dwNumEntries - 1, &pT);
}

BOOL CPwManager::AddGroup(__in_ecount(1) const PW_GROUP *pTemplate)
{
	PW_GROUP pT;
	DWORD i, t = 0, b;

	ASSERT(pTemplate != NULL); if(pTemplate == NULL) return FALSE;

	pT = *pTemplate; // Copy parameter to local temporary variable

	if((pT.uGroupId == 0) || (pT.uGroupId == DWORD_MAX))
	{
		while(1) // Generate a new group ID that doesn't exist already
		{
			b = 0;
			t = randXorShift();
			if((t == 0) || (t == DWORD_MAX)) continue;
			for(i = 0; i < m_dwNumGroups; i++)
			{
				if(m_pGroups[i].uGroupId == t) b = 1;
			}

			if(b == 0) break;
		}
	}
	else t = pT.uGroupId;

	pT.uGroupId = t;

	if(m_dwNumGroups == m_dwMaxGroups)
		_AllocGroups(m_dwMaxGroups + 8);

	m_dwNumGroups++;

	return SetGroup(m_dwNumGroups - 1, &pT);
}

BOOL CPwManager::SetGroup(DWORD dwIndex, __in_ecount(1) const PW_GROUP *pTemplate)
{
	ASSERT(dwIndex < m_dwNumGroups);
	ASSERT(pTemplate != NULL);
	ASSERT((pTemplate->uGroupId != 0) && (pTemplate->uGroupId != DWORD_MAX));
	if((pTemplate->uGroupId == 0) || (pTemplate->uGroupId == DWORD_MAX)) return FALSE;

	SAFE_DELETE_ARRAY(m_pGroups[dwIndex].pszGroupName);
	m_pGroups[dwIndex].pszGroupName = _TcsSafeDupAlloc(pTemplate->pszGroupName);

	m_pGroups[dwIndex].uGroupId = pTemplate->uGroupId;
	m_pGroups[dwIndex].uImageId = pTemplate->uImageId;
	m_pGroups[dwIndex].usLevel = pTemplate->usLevel;
	m_pGroups[dwIndex].dwFlags = pTemplate->dwFlags;

	m_pGroups[dwIndex].tCreation = pTemplate->tCreation;
	m_pGroups[dwIndex].tLastMod = pTemplate->tLastMod;
	m_pGroups[dwIndex].tLastAccess = pTemplate->tLastAccess;
	m_pGroups[dwIndex].tExpire = pTemplate->tExpire;

	return TRUE;
}

BOOL CPwManager::DeleteEntry(DWORD dwIndex)
{
	DWORD i;

	ASSERT(dwIndex < m_dwNumEntries); if(dwIndex >= m_dwNumEntries) return FALSE;

	ASSERT_ENTRY(&m_pEntries[dwIndex]);

	SAFE_DELETE_ARRAY(m_pEntries[dwIndex].pszTitle);
	SAFE_DELETE_ARRAY(m_pEntries[dwIndex].pszURL);
	SAFE_DELETE_ARRAY(m_pEntries[dwIndex].pszUserName);
	SAFE_DELETE_ARRAY(m_pEntries[dwIndex].pszPassword);
	SAFE_DELETE_ARRAY(m_pEntries[dwIndex].pszAdditional);
	SAFE_DELETE_ARRAY(m_pEntries[dwIndex].pszBinaryDesc);
	SAFE_DELETE_ARRAY(m_pEntries[dwIndex].pBinaryData);

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

BOOL CPwManager::DeleteGroupById(DWORD uGroupId)
{
	DWORD i = 0, inx;
	PW_ENTRY *p;

	ASSERT(GetGroupById(uGroupId) != NULL);

	if(m_dwNumEntries != 0)
	{
		while(1) // Remove all items in that group
		{
			p = &m_pEntries[i];
			if(p->uGroupId == uGroupId)
			{
				VERIFY(DeleteEntry(i));
				i--;
			}

			i++;
			if(i >= m_dwNumEntries) break;
		}
	}

	inx = GetGroupByIdN(uGroupId);
	SAFE_DELETE_ARRAY(m_pGroups[inx].pszGroupName);

	if(inx != (m_dwNumGroups - 1))
	{
		for(i = inx; i < (m_dwNumGroups - 1); i++)
		{
			m_pGroups[i] = m_pGroups[i + 1];
		}
	}

	mem_erase((unsigned char *)&m_pGroups[m_dwNumGroups - 1], sizeof(PW_GROUP));
	m_dwNumGroups--;

	FixGroupTree();
	return TRUE;
}

BOOL CPwManager::SetEntry(DWORD dwIndex, __in_ecount(1) const PW_ENTRY *pTemplate)
{
	DWORD slen;

	ASSERT(dwIndex < m_dwNumEntries);
	if(dwIndex >= m_dwNumEntries) return FALSE;

	ASSERT_ENTRY(pTemplate);
	if(pTemplate == NULL) return FALSE;

	ASSERT((pTemplate->uGroupId != 0) && (pTemplate->uGroupId != DWORD_MAX));
	if((pTemplate->uGroupId == 0) || (pTemplate->uGroupId == DWORD_MAX)) return FALSE;

	if(pTemplate->pszTitle == NULL) return FALSE;
	if(pTemplate->pszUserName == NULL) return FALSE;
	if(pTemplate->pszURL == NULL) return FALSE;
	if(pTemplate->pszPassword == NULL) return FALSE;
	if(pTemplate->pszAdditional == NULL) return FALSE;

	memcpy(m_pEntries[dwIndex].uuid, pTemplate->uuid, 16);
	m_pEntries[dwIndex].uGroupId = pTemplate->uGroupId;
	m_pEntries[dwIndex].uImageId = pTemplate->uImageId;

	SAFE_DELETE_ARRAY(m_pEntries[dwIndex].pszTitle);
	m_pEntries[dwIndex].pszTitle = _TcsSafeDupAlloc(pTemplate->pszTitle);

	SAFE_DELETE_ARRAY(m_pEntries[dwIndex].pszUserName);
	m_pEntries[dwIndex].pszUserName = _TcsSafeDupAlloc(pTemplate->pszUserName);

	SAFE_DELETE_ARRAY(m_pEntries[dwIndex].pszURL);
	m_pEntries[dwIndex].pszURL = _TcsSafeDupAlloc(pTemplate->pszURL);

	SAFE_DELETE_ARRAY(m_pEntries[dwIndex].pszPassword);
	m_pEntries[dwIndex].pszPassword = _TcsSafeDupAlloc(pTemplate->pszPassword);

	SAFE_DELETE_ARRAY(m_pEntries[dwIndex].pszAdditional);
	m_pEntries[dwIndex].pszAdditional = _TcsSafeDupAlloc(pTemplate->pszAdditional);

	if(!((m_pEntries[dwIndex].pBinaryData == pTemplate->pBinaryData) && (m_pEntries[dwIndex].pszBinaryDesc == pTemplate->pszBinaryDesc)))
	{
		SAFE_DELETE_ARRAY(m_pEntries[dwIndex].pszBinaryDesc);
		m_pEntries[dwIndex].pszBinaryDesc = _TcsSafeDupAlloc(pTemplate->pszBinaryDesc);

		SAFE_DELETE_ARRAY(m_pEntries[dwIndex].pBinaryData);
		slen = pTemplate->uBinaryDataLen;
		if((pTemplate->pBinaryData != NULL) && (slen != 0))
		{
			m_pEntries[dwIndex].pBinaryData = new BYTE[slen];
			memcpy(m_pEntries[dwIndex].pBinaryData, pTemplate->pBinaryData, slen);
		}
		else
			m_pEntries[dwIndex].pBinaryData = NULL;

		m_pEntries[dwIndex].uBinaryDataLen = pTemplate->uBinaryDataLen;
		if(m_pEntries[dwIndex].pBinaryData == NULL) m_pEntries[dwIndex].uBinaryDataLen = 0;
	}

	m_pEntries[dwIndex].uPasswordLen =
		static_cast<DWORD>(_tcslen(m_pEntries[dwIndex].pszPassword));
	LockEntryPassword(&m_pEntries[dwIndex]);

	m_pEntries[dwIndex].tCreation = pTemplate->tCreation;
	m_pEntries[dwIndex].tLastMod = pTemplate->tLastMod;
	m_pEntries[dwIndex].tLastAccess = pTemplate->tLastAccess;
	m_pEntries[dwIndex].tExpire = pTemplate->tExpire;

	if(m_pEntries[dwIndex].pszBinaryDesc == NULL)
	{
		ASSERT(FALSE);
		m_pEntries[dwIndex].pszBinaryDesc = _TcsSafeDupAlloc(NULL);
	}

	ASSERT_ENTRY(&m_pEntries[dwIndex]);
	m_pLastEditedEntry = &m_pEntries[dwIndex];
	return TRUE;
}

void CPwManager::LockEntryPassword(__inout_ecount(1) PW_ENTRY *pEntry)
{
	ASSERT_ENTRY(pEntry); if(pEntry == NULL) return;
	ASSERT(pEntry->pszPassword != NULL); if(pEntry->pszPassword == NULL) return;

	if(pEntry->uPasswordLen != 0)
		ARCFourCrypt((BYTE *)pEntry->pszPassword, pEntry->uPasswordLen * sizeof(TCHAR),
			m_pSessionKey, PWM_SESSION_KEY_SIZE);

	ASSERT(pEntry->pszPassword[pEntry->uPasswordLen] == 0);
}

void CPwManager::UnlockEntryPassword(__inout_ecount(1) PW_ENTRY *pEntry)
{
	ASSERT_ENTRY(pEntry);
	LockEntryPassword(pEntry); // OFB encryption mode

#ifdef _DEBUG
	unsigned int i;
	for(i = 0; i < pEntry->uPasswordLen; i++)
	{
		ASSERT(pEntry->pszPassword[i] != 0);
	}
#endif
}

void CPwManager::NewDatabase()
{
	_DeleteEntryList(TRUE); // Delete really everything, the strings too
	_DeleteGroupList(TRUE);

	m_pLastEditedEntry = NULL;

	_AllocGroups(PWM_NUM_INITIAL_GROUPS); // Allocate some space for the new items
	_AllocEntries(PWM_NUM_INITIAL_ENTRIES);

	m_vUnknownMetaStreams.clear();

	m_strDefaultUserName.clear();
	m_vSearchHistory.clear();
}

DWORD CPwManager::Find(const TCHAR *pszFindString, BOOL bCaseSensitive,
	DWORD searchFlags, DWORD nStart)
{
	if(nStart >= m_dwNumEntries) return DWORD_MAX;
	ASSERT(pszFindString != NULL); if(pszFindString == NULL) return DWORD_MAX;

	CString strFind = pszFindString;
	if((strFind.GetLength() == 0) || (strFind == _T("*"))) return nStart;

	scoped_ptr<boost::basic_regex<TCHAR> > spRegex;
#ifndef _WIN64
	if((searchFlags & PWMS_REGEX) != 0)
	{
		try
		{
			if(bCaseSensitive == FALSE)
				spRegex.reset(new boost::basic_regex<TCHAR>((LPCTSTR)strFind,
					boost::regex_constants::icase));
			else
				spRegex.reset(new boost::basic_regex<TCHAR>((LPCTSTR)strFind));
		}
		catch(...) { return DWORD_MAX; }
	}
#else
#pragma message("No regular expression support in x64 library.")
#endif

	LPCTSTR lpSearch = strFind;
	if(bCaseSensitive == FALSE)
	{
		strFind.MakeLower();
		lpSearch = strFind;
	}

	for(DWORD i = nStart; i < m_dwNumEntries; ++i)
	{
		if((searchFlags & PWMF_TITLE) != 0)
		{
			if(StrMatchText(m_pEntries[i].pszTitle, lpSearch, bCaseSensitive, spRegex.get()))
				return i;
		}

		if((searchFlags & PWMF_USER) != 0)
		{
			if(StrMatchText(m_pEntries[i].pszUserName, lpSearch, bCaseSensitive, spRegex.get()))
				return i;
		}

		if((searchFlags & PWMF_URL) != 0)
		{
			if(StrMatchText(m_pEntries[i].pszURL, lpSearch, bCaseSensitive, spRegex.get()))
				return i;
		}

		if((searchFlags & PWMF_PASSWORD) != 0)
		{
			UnlockEntryPassword(&m_pEntries[i]);
			CString strPassword = m_pEntries[i].pszPassword;
			LockEntryPassword(&m_pEntries[i]);

			if(StrMatchText(strPassword, lpSearch, bCaseSensitive, spRegex.get()))
			{
				EraseCString(&strPassword);
				return i;
			}

			EraseCString(&strPassword);
		}

		if((searchFlags & PWMF_ADDITIONAL) != 0)
		{
			if(StrMatchText(m_pEntries[i].pszAdditional, lpSearch, bCaseSensitive, spRegex.get()))
				return i;
		}

		if((searchFlags & PWMF_GROUPNAME) != 0)
		{
			const DWORD dwGroupIndex = GetGroupByIdN(m_pEntries[i].uGroupId);
			ASSERT(dwGroupIndex != DWORD_MAX);
			if(dwGroupIndex == DWORD_MAX) continue;

			if(StrMatchText(GetGroup(dwGroupIndex)->pszGroupName, lpSearch,
				bCaseSensitive, spRegex.get()))
				return i;
		}
	}

	return DWORD_MAX;
}

void CPwManager::MoveInternal(DWORD dwFrom, DWORD dwTo)
{
	LONG i, dir;
	PW_ENTRY pe;

	if(dwFrom == dwTo) return; // Nothing to do
	if(dwFrom >= m_dwNumEntries) return; // Invalid index
	if(dwTo >= m_dwNumEntries) return; // Invalid index

	// Set moving direction
	if(dwFrom < dwTo) dir = 1;
	else dir = -1;

	i = (LONG)dwFrom;
	while(1)
	{
		if(i == (LONG)dwTo) break;

		pe = m_pEntries[i];
		m_pEntries[i] = m_pEntries[i+dir];
		m_pEntries[i+dir] = pe;

		i += dir;
	}
}

BOOL CPwManager::MoveGroup(DWORD dwFrom, DWORD dwTo)
{
	LONG i, dir;
	PW_GROUP pg;

	ASSERT((dwFrom != DWORD_MAX) && (dwTo != DWORD_MAX));

	if(dwFrom == dwTo) return TRUE;
	if(dwFrom >= m_dwNumGroups) return FALSE;
	if(dwTo >= m_dwNumGroups) return FALSE;

	// Set moving direction
	if(dwFrom < dwTo) dir = 1;
	else dir = -1;

	i = (LONG)dwFrom;

	while(1)
	{
		if(i == (LONG)dwTo) break;

		pg = m_pGroups[i];
		m_pGroups[i] = m_pGroups[i+dir];
		m_pGroups[i+dir] = pg;

		i += dir;
	}

	FixGroupTree();

	return TRUE;
}

void CPwManager::MoveInGroup(DWORD idGroup, DWORD dwFrom, DWORD dwTo)
{
	if((dwFrom >= m_dwNumEntries) || (dwFrom == DWORD_MAX)) return;
	if((dwTo >= m_dwNumEntries) || (dwTo == DWORD_MAX)) return;
	if(dwFrom == dwTo) return;

	DWORD dwFromEx = GetEntryByGroupN(idGroup, dwFrom);
	DWORD dwToEx = GetEntryByGroupN(idGroup, dwTo);

	if((dwFromEx == DWORD_MAX) || (dwToEx == DWORD_MAX)) return;

	MoveInternal(dwFromEx, dwToEx);
}

BOOL CPwManager::GetGroupTree(DWORD idGroup, DWORD *pGroupIndexes) const
{
	DWORD i, dwGroupPos;
	USHORT usLevel;

	ASSERT(pGroupIndexes != NULL); if(pGroupIndexes == NULL) return FALSE;

	dwGroupPos = GetGroupByIdN(idGroup);
	ASSERT(dwGroupPos != DWORD_MAX); if(dwGroupPos == DWORD_MAX) return FALSE;

	i = dwGroupPos;
	usLevel = (USHORT)(m_pGroups[i].usLevel + 1);
	while(1)
	{
		if(m_pGroups[i].usLevel == (usLevel - 1))
		{
			usLevel--;
			pGroupIndexes[usLevel] = i;
			if(usLevel == 0) break;
		}

		if(i == 0) { ASSERT(FALSE); return FALSE; }
		i--;
	}

	return TRUE;
}

void CPwManager::SortGroupList()
{
	DWORD i, j;
	BOOL bSwapped = TRUE;
	PPW_GROUP p = NULL;
	LPTSTR *pList = NULL;
	LPTSTR *pParents = NULL;
	LPTSTR lpTemp = NULL;
	LPTSTR lp = NULL;
	DWORD dwMaxString = 0;
	USHORT usMaxLevel = 0;
	TCHAR tszTemp[16];
	DWORD dw;
	PW_GROUP pwt;

	if(m_dwNumGroups <= 1) return; // Nothing to sort

	// Search list for longest group name and maximum level
	for(i = 0; i < m_dwNumGroups; i++)
	{
		j = (DWORD)_tcslen(m_pGroups[i].pszGroupName);
		if(j > dwMaxString) dwMaxString = j;

		if(m_pGroups[i].usLevel > usMaxLevel) usMaxLevel = m_pGroups[i].usLevel;
	}

	// Allocate pointer list to parent items
	pParents = new LPTSTR[usMaxLevel + 2];
	ASSERT(pParents != NULL); if(pParents == NULL) return;

	// Calculate maximum length of the biggest FQGN (fully qualified group name)
	// (numchars(base10(0xFFFFFFFF)) = 10) + char(NULL) + char(/n) + char(/n) = 13
	dwMaxString = (dwMaxString + 1) * (usMaxLevel + 1) + 13;

	pList = new LPTSTR[m_dwNumGroups]; // Allocate FQGN list (array of pointers)
	ASSERT(pList != NULL);
	lpTemp = new TCHAR[dwMaxString];
	ASSERT(lpTemp != NULL);

	// Build FQGN list
	for(i = 0; i < m_dwNumGroups; i++)
	{
		pList[i] = new TCHAR[dwMaxString];
		ASSERT(pList[i] != NULL);
		if(pList[i] == NULL)
		{
			for(j = 0; j < i; j++) SAFE_DELETE_ARRAY(pList[j]);
			SAFE_DELETE_ARRAY(pList); SAFE_DELETE_ARRAY(pParents); SAFE_DELETE_ARRAY(lpTemp);
			return; // Failed, too few memory available
		}
		*pList[i] = 0;

		p = &m_pGroups[i]; ASSERT(p != NULL);
		if(p->usLevel != 0)
		{
			for(j = 0; j < p->usLevel; j++)
			{
				_tcscat_s(pList[i], dwMaxString, pParents[j]);
				_tcscat_s(pList[i], dwMaxString, _T("\n"));
			}
		}
		pParents[p->usLevel] = p->pszGroupName;
		_tcscat_s(pList[i], dwMaxString, p->pszGroupName);
		_tcscat_s(pList[i], dwMaxString, _T("\n\n"));

		_ltot_s((long)p->uGroupId, tszTemp, _countof(tszTemp), 10);
		_tcscat_s(pList[i], dwMaxString, tszTemp);

		ASSERT(_tcslen(pList[i]) < dwMaxString);
	}

	while(bSwapped == TRUE) // Stable bubble-sort on the group list
	{
		bSwapped = FALSE;

		for(i = 0; i < (m_dwNumGroups - 1); i++)
		{
			if(_tcsicmp(pList[i], pList[i+1]) > 0)
			{
				_tcscpy_s(lpTemp, dwMaxString, pList[i]); // Swap strings
				_tcscpy_s(pList[i], dwMaxString, pList[i+1]);
				_tcscpy_s(pList[i+1], dwMaxString, lpTemp);

				bSwapped = TRUE;
			}
		}
	}

	for(i = 0; i < (m_dwNumGroups - 1); ++i)
	{
		lp = &pList[i][_tcslen(pList[i]) - 1];
		while(1)
		{
			if(*lp == _T('\n')) break;
			--lp;
		}
		++lp;

		dw = (DWORD)_ttol(lp);
		ASSERT(GetGroupById(dw) != NULL);

		for(j = i; j < m_dwNumGroups; j++)
		{
			if(m_pGroups[j].uGroupId == dw) break;
		}

		pwt = m_pGroups[i];
		m_pGroups[i] = m_pGroups[j];
		m_pGroups[j] = pwt;
	}

	for(i = 0; i < m_dwNumGroups; ++i) SAFE_DELETE_ARRAY(pList[i]);

	SAFE_DELETE_ARRAY(pList);
	SAFE_DELETE_ARRAY(pParents);
	SAFE_DELETE_ARRAY(lpTemp);

	FixGroupTree();
}

void CPwManager::SortGroup(DWORD idGroup, DWORD dwSortByField)
{
	PPW_ENTRY *p;
	PW_ENTRY v;
	DWORD i, j, n = 0, min, t;

	if(m_dwNumEntries <= 1) return; // Nothing to sort

	p = new PPW_ENTRY[m_dwNumEntries];
	if(p == NULL) return;

	// Build pointer array that contains pointers to the elements to sort
	for(i = 0; i < m_dwNumEntries; i++)
	{
		if(m_pEntries[i].uGroupId == idGroup)
		{
			p[n] = &m_pEntries[i];
			n++;
		}
	}
	if(n <= 1) { SAFE_DELETE_ARRAY(p); return; } // Something to sort?

	// Sort the array, using a simple selection sort
	for(i = 0; i < (n - 1); ++i)
	{
		min = i;

		for(j = i + 1; j < n; ++j)
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
			case 5:
				if(_pwtimecmp(&p[j]->tCreation, &p[min]->tCreation) > 0)
					min = j;
				break;
			case 6:
				if(_pwtimecmp(&p[j]->tLastMod, &p[min]->tLastMod) > 0)
					min = j;
				break;
			case 7:
				if(_pwtimecmp(&p[j]->tLastAccess, &p[min]->tLastAccess) > 0)
					min = j;
				break;
			case 8:
				if(_pwtimecmp(&p[j]->tExpire, &p[min]->tExpire) > 0)
					min = j;
				break;
			case 9:
				// Cannot sort by UUID
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

void CPwManager::GetNeverExpireTime(__out_ecount(1) PW_TIME *pPwTime)
{
	ASSERT(pPwTime != NULL); if(pPwTime == NULL) return;
	memcpy(pPwTime, &g_pwTimeNever, sizeof(PW_TIME));
}

void CPwManager::FixGroupTree()
{
	m_pGroups[0].usLevel = 0; // First group must be root

	USHORT usLastLevel = 0;
	for(DWORD i = 0; i < m_dwNumGroups; i++)
	{
		if(m_pGroups[i].usLevel > (USHORT)(usLastLevel + 1))
			m_pGroups[i].usLevel = (USHORT)(usLastLevel + 1);

		usLastLevel = m_pGroups[i].usLevel;
	}
}

DWORD CPwManager::GetLastChildGroup(DWORD dwParentGroupIndex) const
{
	DWORD i;
	USHORT usParentLevel;
	USHORT usLevel;
	USHORT usLastLevel;

	if(m_dwNumGroups <= 1) return 0;
	ASSERT(dwParentGroupIndex < m_dwNumGroups);
	if(dwParentGroupIndex == (m_dwNumGroups - 1)) return m_dwNumGroups - 1;
	else if(dwParentGroupIndex > (DWORD)(m_dwNumGroups - 1)) return DWORD_MAX;

	usParentLevel = m_pGroups[dwParentGroupIndex].usLevel;

	i = dwParentGroupIndex + 1;
	usLevel = usParentLevel;
	while(1)
	{
		usLastLevel = usLevel;
		usLevel = m_pGroups[i].usLevel;

		if(usLevel <= usParentLevel) return i - 1;

		i++;
		if(i >= m_dwNumGroups) break;
	}

	return DWORD_MAX;
}

void CPwManager::SubstEntryGroupIds(DWORD dwExistingId, DWORD dwNewId)
{
	DWORD i;

	ASSERT(dwExistingId != DWORD_MAX); ASSERT(dwNewId != DWORD_MAX);
	if(dwExistingId == dwNewId) return; // Nothing to do?

	for(i = 0; i < m_dwNumEntries; i++)
	{
		if(m_pEntries[i].uGroupId == dwExistingId)
			m_pEntries[i].uGroupId = dwNewId;
	}
}

// Encrypt the master key a few times to make brute-force key-search harder
BOOL CPwManager::_TransformMasterKey(const BYTE *pKeySeed)
{
	CRijndael rijndael;
	UINT8 aKey[32];
	UINT8 aTest[16];
	UINT8 aRef[16] = { // The Rijndael class will be tested, that's the expected ciphertext
		0x8e, 0xa2, 0xb7, 0xca, 0x51, 0x67, 0x45, 0xbf,
		0xea, 0xfc, 0x49, 0x90, 0x4b, 0x49, 0x60, 0x89
	};
	DWORD i;
	sha256_ctx sha2;

	ASSERT(pKeySeed != NULL); if(pKeySeed == NULL) return FALSE;

	if(rijndael.Init(CRijndael::ECB, CRijndael::EncryptDir, (const UINT8 *)pKeySeed,
		CRijndael::Key32Bytes, 0) != RIJNDAEL_SUCCESS)
	{
		return FALSE;
	}

	memcpy(m_pTransformedMasterKey, m_pMasterKey, 32);

	for(i = 0; i < m_dwKeyEncRounds; i++)
	{
		rijndael.BlockEncrypt((const UINT8 *)m_pTransformedMasterKey, 256, (UINT8 *)m_pTransformedMasterKey);
	}

	// Do a quick test if the Rijndael class worked correctly
	for(i = 0; i < 32; i++) aKey[i] = (UINT8)i;
	for(i = 0; i < 16; i++) aTest[i] = (UINT8)(((UINT8)i << 4) | (UINT8)i);
	if(rijndael.Init(CRijndael::ECB, CRijndael::EncryptDir, aKey, CRijndael::Key32Bytes, NULL) != RIJNDAEL_SUCCESS)
		{ ASSERT(FALSE); return FALSE; }
	if(rijndael.BlockEncrypt(aTest, 128, aTest) != 128) { ASSERT(FALSE); }
	if(memcmp(aTest, aRef, 16) != 0) { ASSERT(FALSE); return FALSE; }

	// Hash once with SHA-256
	sha256_begin(&sha2);
	sha256_hash(m_pTransformedMasterKey, 32, &sha2);
	sha256_end(m_pTransformedMasterKey, &sha2);

	return TRUE;
}

DWORD CPwManager::GetKeyEncRounds() const
{
	return m_dwKeyEncRounds;
}

void CPwManager::SetKeyEncRounds(DWORD dwRounds)
{
	// All allowed except DWORD_MAX
	if(dwRounds == DWORD_MAX) m_dwKeyEncRounds = DWORD_MAX - 1;
	else m_dwKeyEncRounds = dwRounds;
}

DWORD CPwManager::DeleteLostEntries()
{
	DWORD dwDeletedCount = 0;

	DWORD dwEntryCount = GetNumberOfEntries();
	if(dwEntryCount == 0) return 0;

	BOOL bFixed = TRUE;
	while(bFixed == TRUE)
	{
		bFixed = FALSE;

		for(DWORD i = 0; i < dwEntryCount; ++i)
		{
			PW_ENTRY *pe = GetEntry(i);
			ASSERT(pe != NULL); if(pe == NULL) break;

			PW_GROUP *pg = GetGroupById(pe->uGroupId);
			if(pg == NULL)
			{
				DeleteEntry(i);
				--dwEntryCount;
				bFixed = TRUE;
				++dwDeletedCount;
				break;
			}
		}
	}

	return dwDeletedCount;
}

BOOL CPwManager::BackupEntry(__in_ecount(1) const PW_ENTRY *pe,
	__out_opt BOOL *pbGroupCreated)
{
	PW_ENTRY pwe;
	PW_GROUP pwg;
	DWORD dwGroupId;

	ASSERT_ENTRY(pe); if(pe == NULL) return FALSE;

	if(pbGroupCreated != NULL) *pbGroupCreated = FALSE;

	dwGroupId = GetGroupId(PWS_BACKUPGROUP);
	if(dwGroupId == DWORD_MAX)
	{
		ZeroMemory(&pwg, sizeof(PW_GROUP));
		pwg.pszGroupName = (TCHAR *)PWS_BACKUPGROUP;
		_GetCurrentPwTime(&pwg.tCreation);
		pwg.tLastAccess = pwg.tCreation;
		pwg.tLastMod = pwg.tCreation;
		GetNeverExpireTime(&pwg.tExpire);
		pwg.uImageId = 4;

		if(AddGroup(&pwg) == FALSE) return FALSE;
		if(pbGroupCreated != NULL) *pbGroupCreated = TRUE;
		dwGroupId = GetGroupId(PWS_BACKUPGROUP);
	}
	if(dwGroupId == DWORD_MAX) return FALSE;

	pwe = *pe;
	_GetCurrentPwTime(&pwe.tLastMod);
	pwe.uGroupId = dwGroupId;
	ZeroMemory(&pwe.uuid, 16); // Create new UUID for the backup entry

	return AddEntry(&pwe);
}

/* DWORD CPwManager::MakeGroupTree(LPCTSTR lpTreeString, TCHAR tchSeparator)
{
	DWORD i, j = 0, dwCurGroup = 0, dwTestGroup;
	DWORD dwId = DWORD_MAX;
	TCHAR *ptzTemp;
	DWORD dwStrLen;
	DWORD dwLevel = 0;
	TCHAR tch;

	ASSERT(lpTreeString != NULL); if(lpTreeString == NULL) return DWORD_MAX;

	dwStrLen = (DWORD)_tcslen(lpTreeString);
	ptzTemp = new TCHAR[dwStrLen + 1]; ASSERT(ptzTemp != NULL);
	ptzTemp[0] = 0;

	for(i = 0; i < (dwStrLen + 1); i++)
	{
		tch = lpTreeString[i];

		if((tch == tchSeparator) || (tch == 0))
		{
			ptzTemp[j] = 0;

			j = 0;
		}
		else
		{
			ptzTemp[j] = tch;
			j++;
		}
	}

	SAFE_DELETE_ARRAY(ptzTemp);
	return dwId;
} */

BOOL CPwManager::_AddMetaStream(LPCTSTR lpMetaDataDesc, BYTE *pData, DWORD dwLength)
{
	PW_ENTRY pe;

	ASSERT(lpMetaDataDesc != NULL); ASSERT(_tcslen(lpMetaDataDesc) != 0);
	ASSERT(pData != NULL); if(pData == NULL) return FALSE;
	ASSERT(dwLength != 0); if(dwLength == 0) return TRUE;

	// Database must contain at least one group
	if(GetNumberOfGroups() == 0) return FALSE;

	memset(&pe, 0, sizeof(PW_ENTRY));
	pe.uGroupId = m_pGroups[0].uGroupId;
	pe.pBinaryData = pData; pe.pszAdditional = (TCHAR *)lpMetaDataDesc;
	pe.pszBinaryDesc = PMS_ID_BINDESC; pe.pszPassword = _T("");
	pe.pszTitle = PMS_ID_TITLE; pe.pszURL = PMS_ID_URL;
	pe.pszUserName = PMS_ID_USER;
	pe.tCreation = g_pwTimeNever; pe.tExpire = g_pwTimeNever;
	pe.tLastAccess = g_pwTimeNever; pe.tLastMod = g_pwTimeNever;
	pe.uBinaryDataLen = dwLength;

	return AddEntry(&pe);
}

BOOL CPwManager::_IsMetaStream(const PW_ENTRY *p) const
{
	ASSERT_ENTRY(p); if(p == NULL) return FALSE;

	if(p->pBinaryData == NULL) return FALSE;
	if(p->pszAdditional == NULL) return FALSE;
	if(p->pszBinaryDesc == NULL) return FALSE;
	if(_tcscmp(p->pszBinaryDesc, PMS_ID_BINDESC) != 0) return FALSE;
	if(p->pszTitle == NULL) return FALSE;
	if(_tcscmp(p->pszTitle, PMS_ID_TITLE) != 0) return FALSE;
	if(p->pszUserName == NULL) return FALSE;
	if(_tcscmp(p->pszUserName, PMS_ID_USER) != 0) return FALSE;
	if(p->pszURL == NULL) return FALSE;
	if(_tcscmp(p->pszURL, PMS_ID_URL) != 0) return FALSE;
	if(p->uImageId != 0) return FALSE;

	return TRUE;
}

DWORD CPwManager::_LoadAndRemoveAllMetaStreams(bool bAcceptUnknown)
{
	if(m_pEntries == NULL) return 0;
	if(m_pGroups == NULL) return 0;
	if(GetNumberOfEntries() == 0) return 0;
	if(GetNumberOfGroups() == 0) return 0;

	m_vSearchHistory.clear();

	DWORD dwMetaStreamCount = 0;

	BOOL bChange = TRUE;
	while(bChange == TRUE)
	{
		bChange = FALSE;
		const DWORD dwEntryCount = GetNumberOfEntries();
		if(dwEntryCount == 0) break;

		DWORD i = dwEntryCount - 1;
		while(1)
		{
			PW_ENTRY *p = GetEntry(i);
			if(_IsMetaStream(p) == TRUE)
			{
				_ParseMetaStream(p, bAcceptUnknown);
				VERIFY(DeleteEntry(i));
				dwMetaStreamCount++;
				bChange = TRUE;
				break;
			}

			if(i == 0) break;
			--i;
		}
	}

	return dwMetaStreamCount;
}

BOOL CPwManager::_AddAllMetaStreams()
{
	PMS_SIMPLE_UI_STATE simpState;
	BOOL b = TRUE;

	memset(&simpState, 0, sizeof(PMS_SIMPLE_UI_STATE));

	simpState.uLastSelectedGroupId = m_dwLastSelectedGroupId;
	simpState.uLastTopVisibleGroupId = m_dwLastTopVisibleGroupId;
	memcpy(simpState.aLastSelectedEntryUuid, m_aLastSelectedEntryUuid, 16);
	memcpy(simpState.aLastTopVisibleEntryUuid, m_aLastTopVisibleEntryUuid, 16);

	b &= _AddMetaStream(PMS_STREAM_SIMPLESTATE, (BYTE *)&simpState, sizeof(PMS_SIMPLE_UI_STATE));

	BYTE *pName = _StringToUTF8(m_strDefaultUserName.c_str());
	b &= _AddMetaStream(PMS_STREAM_DEFAULTUSER, pName, szlen((const char *)pName) + 1);
	SAFE_DELETE_ARRAY(pName);

	for(size_t uHItem = 0; uHItem < m_vSearchHistory.size(); ++uHItem)
	{
		const size_t uHIndex = m_vSearchHistory.size() - uHItem - 1;
		BYTE *pHItem = _StringToUTF8(m_vSearchHistory[uHIndex].c_str());
		b &= _AddMetaStream(PMS_STREAM_SEARCHHISTORYITEM, pHItem, szlen((const char *)pHItem) + 1);
		SAFE_DELETE_ARRAY(pHItem);
	}

	// Add back all unknown meta streams
	for(std::vector<PWDB_META_STREAM>::iterator it = m_vUnknownMetaStreams.begin();
		it != m_vUnknownMetaStreams.end(); ++it)
	{
		b &= _AddMetaStream(it->strName.c_str(), &it->vData[0],
			static_cast<DWORD>(it->vData.size()));
	}

	return b;
}

void CPwManager::_ParseMetaStream(PW_ENTRY *p, bool bAcceptUnknown)
{
	ASSERT(_IsMetaStream(p) == TRUE);

	if(_tcscmp(p->pszAdditional, PMS_STREAM_SIMPLESTATE) == 0)
	{
		PMS_SIMPLE_UI_STATE *pState = (PMS_SIMPLE_UI_STATE *)p->pBinaryData;

		if(p->uBinaryDataLen >= 4) // Length checks for backwards compatibility
			m_dwLastSelectedGroupId = pState->uLastSelectedGroupId;

		if(p->uBinaryDataLen >= 8)
			m_dwLastTopVisibleGroupId = pState->uLastTopVisibleGroupId;

		if(p->uBinaryDataLen >= 24)
			memcpy(m_aLastSelectedEntryUuid, pState->aLastSelectedEntryUuid, 16);

		if(p->uBinaryDataLen >= 40)
			memcpy(m_aLastTopVisibleEntryUuid, pState->aLastTopVisibleEntryUuid, 16);
	}
	else if(_tcscmp(p->pszAdditional, PMS_STREAM_DEFAULTUSER) == 0)
	{
		LPTSTR lpName = _UTF8ToString(p->pBinaryData);
		m_strDefaultUserName = (LPCTSTR)lpName;
		SAFE_DELETE_ARRAY(lpName);
	}
	else if(_tcscmp(p->pszAdditional, PMS_STREAM_SEARCHHISTORYITEM) == 0)
	{
		LPTSTR lpItem = _UTF8ToString(p->pBinaryData);
		m_vSearchHistory.push_back(std::basic_string<TCHAR>((LPCTSTR)lpItem));
		SAFE_DELETE_ARRAY(lpItem);
	}
	else // Unknown meta stream -- save it
	{
		if(bAcceptUnknown)
		{
			PWDB_META_STREAM msUnknown;
			msUnknown.strName = p->pszAdditional;
			msUnknown.vData.assign(p->pBinaryData, p->pBinaryData +
				p->uBinaryDataLen);

			if(_CanIgnoreUnknownMetaStream(msUnknown) == FALSE)
				m_vUnknownMetaStreams.push_back(msUnknown);
		}
	}
}

BOOL CPwManager::_CanIgnoreUnknownMetaStream(const PWDB_META_STREAM& msUnknown) const
{
	if(m_vUnknownMetaStreams.size() == 0) return FALSE;
	if(msUnknown.strName != PMS_STREAM_KPXICON2) return FALSE;

	const PWDB_META_STREAM* pLastMs = &m_vUnknownMetaStreams[
		m_vUnknownMetaStreams.size() - 1];

	if(msUnknown.vData.size() != pLastMs->vData.size()) return FALSE;

	for(DWORD dwByte = 0; dwByte < msUnknown.vData.size(); dwByte++)
	{
		if(msUnknown.vData[dwByte] != pLastMs->vData[dwByte])
			return FALSE;
	}

	return TRUE;
}

void CPwManager::MergeIn(__inout_ecount(1) CPwManager *pDataSource,
	BOOL bCreateNewUUIDs, BOOL bCompareTimes)
{
	ASSERT(pDataSource != NULL); if(pDataSource == NULL) return;

	DWORD i, dwModifyIndex, dwOldId, dwNewId;
	PW_GROUP *pgThis, *pgSource;
	PW_ENTRY *peThis, *peSource;
	PW_TIME tNow;
	BOOL bDoReplace;

	_GetCurrentPwTime(&tNow);

	for(i = 0; i < pDataSource->GetNumberOfGroups(); i++)
	{
		pgSource = pDataSource->GetGroup(i);
		ASSERT(pgSource != NULL); if(pgSource == NULL) continue;

		if(bCreateNewUUIDs == TRUE)
		{
			while(1)
			{
				dwOldId = pgSource->uGroupId;
				pgSource->uGroupId = 0; // Create new ID
				VERIFY(AddGroup(pgSource) == TRUE);

				dwNewId = GetGroup(GetNumberOfGroups() - 1)->uGroupId;

				if(pDataSource->GetGroupById(dwNewId) == NULL)
				{
					pDataSource->SubstEntryGroupIds(dwOldId, dwNewId);
					break;
				}

				pgSource->uGroupId = dwOldId;
				VERIFY(DeleteGroupById(dwNewId) == TRUE);
			}
		}
		else // bCreateNewUUIDs == FALSE
		{
			dwModifyIndex = GetGroupByIdN(pgSource->uGroupId);
			if(dwModifyIndex != DWORD_MAX) pgThis = GetGroup(dwModifyIndex);
			else pgThis = NULL;

			if(pgThis == NULL) AddGroup(pgSource); // Group doesn't exist already
			else
			{
				bDoReplace = TRUE;
				if(bCompareTimes == TRUE)
					if(_pwtimecmp(&pgThis->tLastMod, &pgSource->tLastMod) >= 0)
						bDoReplace = FALSE;

				if(bDoReplace == TRUE)
				{
					VERIFY(SetGroup(dwModifyIndex, pgSource) == TRUE);

					pgThis = GetGroup(dwModifyIndex);
					if(pgThis != NULL) pgThis->tLastAccess = tNow;
				}
			}
		}
	}

	FixGroupTree();

	for(i = 0; i < pDataSource->GetNumberOfEntries(); i++)
	{
		peSource = pDataSource->GetEntry(i);
		ASSERT(peSource != NULL); if(peSource == NULL) continue;

		// Don't import meta streams
		if(_IsMetaStream(peSource) == TRUE) continue;

		pDataSource->UnlockEntryPassword(peSource);

		if(bCreateNewUUIDs == TRUE)
		{
			memset(peSource->uuid, 0, 16);
			VERIFY(AddEntry(peSource) == TRUE);
		}
		else
		{
			dwModifyIndex = GetEntryByUuidN(peSource->uuid);
			if(dwModifyIndex != DWORD_MAX) peThis = GetEntry(dwModifyIndex);
			else peThis = NULL;

			if(peThis == NULL) AddEntry(peSource); // Entry doesn't exist already
			else
			{
				bDoReplace = TRUE;
				if(bCompareTimes == TRUE)
					if(_pwtimecmp(&peThis->tLastMod, &peSource->tLastMod) >= 0)
						bDoReplace = FALSE;

				if(bDoReplace == TRUE)
				{
					VERIFY(SetEntry(dwModifyIndex, peSource));

					peThis = GetEntry(dwModifyIndex);
					if(peThis != NULL) peThis->tLastAccess = tNow;
				}
			}
		}

		pDataSource->LockEntryPassword(peSource);
	}

	VERIFY(DeleteLostEntries() == 0);
}

void CPwManager::GetRawMasterKey(__out_ecount(32) BYTE *pStorage) const
{
	ASSERT(pStorage != NULL); if(pStorage == NULL) return;
	memcpy(pStorage, m_pMasterKey, 32);
}

void CPwManager::SetRawMasterKey(__in_ecount(32) const BYTE *pNewKey)
{
	if(pNewKey != NULL) memcpy(m_pMasterKey, pNewKey, 32);
	else memset(m_pMasterKey, 0, 32);
}

const PW_DBHEADER *CPwManager::GetLastDatabaseHeader() const
{
	return &m_dbLastHeader;
}

std::basic_string<TCHAR> CPwManager::GetPropertyString(DWORD dwPropertyId) const
{
	if(dwPropertyId == PWP_DEFAULT_USER_NAME)
		return m_strDefaultUserName;

	ASSERT(FALSE);
	return std::basic_string<TCHAR>();
}

BOOL CPwManager::SetPropertyString(DWORD dwPropertyId, LPCTSTR lpValue)
{
	BOOL bResult = TRUE;

	switch(dwPropertyId)
	{
		case PWP_DEFAULT_USER_NAME:
			m_strDefaultUserName = lpValue;
			break;
		default:
			ASSERT(FALSE);
			bResult = FALSE;
			break;
	}

	return bResult;
}

std::vector<std::basic_string<TCHAR> >* CPwManager::AccessPropertyStrArray(
	DWORD dwPropertyId)
{
	if(dwPropertyId == PWPA_SEARCH_HISTORY)
		return &m_vSearchHistory;

	ASSERT(FALSE);
	return NULL;
}
