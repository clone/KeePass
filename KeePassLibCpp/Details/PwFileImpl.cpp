/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2012 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "../PwManager.h"
#include "../Crypto/TwofishClass.h"
#include "../Crypto/SHA2/SHA2.h"
#include "../Crypto/ARCFour.h"
#include "../Util/AppUtil.h"
#include "../Util/Base64.h"
#include "../Util/PwUtil.h"
#include "../Util/MemUtil.h"
#include "../Util/StrUtil.h"
#include "../Util/TranslateEx.h"
#include "PwCompatImpl.h"

#include <boost/static_assert.hpp>

#define _OPENDB_FAIL_LIGHT \
{ \
	if(pVirtualFile != NULL) \
	{ \
		mem_erase((unsigned char *)pVirtualFile, uAllocated); \
		SAFE_DELETE_ARRAY(pVirtualFile); \
	} \
	m_dwKeyEncRounds = PWM_STD_KEYENCROUNDS; \
}
#define _OPENDB_FAIL \
{ \
	_OPENDB_FAIL_LIGHT; \
	SAFE_DELETE_ARRAY(pwGroupTemplate.pszGroupName); \
	SAFE_DELETE_ARRAY(pwEntryTemplate.pszTitle); \
	SAFE_DELETE_ARRAY(pwEntryTemplate.pszURL); \
	SAFE_DELETE_ARRAY(pwEntryTemplate.pszUserName); \
	SAFE_DELETE_ARRAY(pwEntryTemplate.pszPassword); \
	SAFE_DELETE_ARRAY(pwEntryTemplate.pszAdditional); \
	SAFE_DELETE_ARRAY(pwEntryTemplate.pszBinaryDesc); \
	SAFE_DELETE_ARRAY(pwEntryTemplate.pBinaryData); \
	return PWE_INVALID_FILESTRUCTURE; \
}

#define RESET_TIME_FIELD_NORMAL(pTimeEx) { \
	(pTimeEx)->btDay = 1; (pTimeEx)->btHour = 0; (pTimeEx)->btMinute = 0; \
	(pTimeEx)->btMonth = 1; (pTimeEx)->btSecond = 0; (pTimeEx)->shYear = 2004; }
#define RESET_TIME_FIELD_EXPIRE(pTimeEx) { \
	(pTimeEx)->btDay = 28; (pTimeEx)->btHour = 23; (pTimeEx)->btMinute = 59; \
	(pTimeEx)->btMonth = 12; (pTimeEx)->btSecond = 59; (pTimeEx)->shYear = 4092; }

#define RESET_PWG_TEMPLATE(ptrx) { \
	memset(ptrx, 0, sizeof(PW_GROUP)); \
	RESET_TIME_FIELD_NORMAL(&(ptrx)->tCreation); RESET_TIME_FIELD_NORMAL(&(ptrx)->tLastMod); \
	RESET_TIME_FIELD_NORMAL(&(ptrx)->tLastAccess); RESET_TIME_FIELD_EXPIRE(&(ptrx)->tExpire); }
#define RESET_PWE_TEMPLATE(ptrx) { \
	memset(ptrx, 0, sizeof(PW_ENTRY)); \
	RESET_TIME_FIELD_NORMAL(&(ptrx)->tCreation); RESET_TIME_FIELD_NORMAL(&(ptrx)->tLastMod); \
	RESET_TIME_FIELD_NORMAL(&(ptrx)->tLastAccess); RESET_TIME_FIELD_EXPIRE(&(ptrx)->tExpire); }

// int CPwManager::OpenDatabase(const TCHAR *pszFile, _Out_opt_ PWDB_REPAIR_INFO *pRepair)
// {
//	return this->OpenDatabaseEx(pszFile, pRepair, NULL);
// }

#define PWMOD_CHECK_AVAIL(w_Cnt_p) { if((static_cast<UINT64>(pos) + static_cast<UINT64>( \
	w_Cnt_p)) > static_cast<UINT64>(uFileSize)) { ASSERT(FALSE); _OPENDB_FAIL; } }

// If bIgnoreCorrupted is TRUE the manager will try to ignore all database file
// errors, i.e. try to read as much as possible instead of breaking out at the
// first error.
// To open a file normally, set bIgnoreCorrupted to FALSE (default).
// To open a file in rescue mode, set it to TRUE.
int CPwManager::OpenDatabase(const TCHAR *pszFile, _Out_opt_ PWDB_REPAIR_INFO *pRepair)
{
	char *pVirtualFile;
	unsigned long uFileSize, uAllocated, uEncryptedPartSize;
	unsigned long pos;
	PW_DBHEADER hdr;
	sha256_ctx sha32;
	UINT8 uFinalKey[32];
	char *p;
	USHORT usFieldType;
	DWORD dwFieldSize;
	PW_GROUP pwGroupTemplate;
	PW_ENTRY pwEntryTemplate;

	BOOST_STATIC_ASSERT(sizeof(char) == 1);

	ASSERT(pszFile != NULL); if(pszFile == NULL) return PWE_INVALID_PARAM;
	ASSERT(pszFile[0] != 0); if(pszFile[0] == 0) return PWE_INVALID_PARAM; // Length != 0

	RESET_PWG_TEMPLATE(&pwGroupTemplate);
	RESET_PWE_TEMPLATE(&pwEntryTemplate);

	if(pRepair != NULL) { ZeroMemory(pRepair, sizeof(PWDB_REPAIR_INFO)); }

	FILE *fp = NULL;
	_tfopen_s(&fp, pszFile, _T("rb"));
	if(fp == NULL) return PWE_NOFILEACCESS_READ;

	// Get file size
	fseek(fp, 0, SEEK_END);
	uFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if(uFileSize < sizeof(PW_DBHEADER))
		{ fclose(fp); return PWE_INVALID_FILEHEADER; }

	// Allocate enough memory to hold the complete file
	uAllocated = uFileSize + 16 + 1 + 64 + 4; // 16 = encryption buffer space, 1+64 = string terminating NULLs, 4 unused
	pVirtualFile = new char[uAllocated];
	if(pVirtualFile == NULL) { fclose(fp); return PWE_NO_MEM; }
	memset(&pVirtualFile[uFileSize + 16], 0, 1 + 64);

	fread(pVirtualFile, 1, uFileSize, fp);
	fclose(fp);

	// Extract header structure from memory file
	memcpy(&hdr, pVirtualFile, sizeof(PW_DBHEADER));

	// Check if it's a KDBX file created by KeePass 2.x
	if((hdr.dwSignature1 == PWM_DBSIG_1_KDBX_P) && (hdr.dwSignature2 == PWM_DBSIG_2_KDBX_P))
		{ _OPENDB_FAIL_LIGHT; return PWE_UNSUPPORTED_KDBX; }
	if((hdr.dwSignature1 == PWM_DBSIG_1_KDBX_R) && (hdr.dwSignature2 == PWM_DBSIG_2_KDBX_R))
		{ _OPENDB_FAIL_LIGHT; return PWE_UNSUPPORTED_KDBX; }

	// Check if we can open this
	if((hdr.dwSignature1 != PWM_DBSIG_1) || (hdr.dwSignature2 != PWM_DBSIG_2))
		{ _OPENDB_FAIL_LIGHT; return PWE_INVALID_FILESIGNATURE; }

	if((hdr.dwVersion & 0xFFFFFF00) != (PWM_DBVER_DW & 0xFFFFFF00))
	{
		if((hdr.dwVersion == 0x00020000) || (hdr.dwVersion == 0x00020001) || (hdr.dwVersion == 0x00020002))
		{
			if(pVirtualFile != NULL)
			{
				mem_erase((unsigned char *)pVirtualFile, uAllocated);
				SAFE_DELETE_ARRAY(pVirtualFile);
			}

			return ((CPwCompatImpl::OpenDatabaseV2(this, pszFile) != FALSE) ?
				PWE_SUCCESS : PWE_UNKNOWN);
		}
		else if(hdr.dwVersion <= 0x00010002)
		{
			if(pVirtualFile != NULL)
			{
				mem_erase((unsigned char *)pVirtualFile, uAllocated);
				SAFE_DELETE_ARRAY(pVirtualFile);
			}
			
			return ((CPwCompatImpl::OpenDatabaseV1(this, pszFile) != FALSE) ?
				PWE_SUCCESS : PWE_UNKNOWN);
		}
		else { ASSERT(FALSE); _OPENDB_FAIL; }
	}

	// Select algorithm
	if((hdr.dwFlags & PWM_FLAG_RIJNDAEL) != 0) m_nAlgorithm = ALGO_AES;
	else if((hdr.dwFlags & PWM_FLAG_TWOFISH) != 0) m_nAlgorithm = ALGO_TWOFISH;
	else { ASSERT(FALSE); _OPENDB_FAIL; }

	m_dwKeyEncRounds = hdr.dwKeyEncRounds;

	// Generate m_pTransformedMasterKey from m_pMasterKey
	if(_TransformMasterKey(hdr.aMasterSeed2) == FALSE) { ASSERT(FALSE); _OPENDB_FAIL; }

	ProtectTransformedMasterKey(false);

	// Hash the master password with the salt in the file
	sha256_begin(&sha32);
	sha256_hash(hdr.aMasterSeed, 16, &sha32);
	sha256_hash(m_pTransformedMasterKey, 32, &sha32);
	sha256_end((unsigned char *)uFinalKey, &sha32);

	ProtectTransformedMasterKey(true);

	if(pRepair == NULL)
	{
		// ASSERT(((uFileSize - sizeof(PW_DBHEADER)) % 16) == 0);
		if(((uFileSize - sizeof(PW_DBHEADER)) % 16) != 0)
		{
			_OPENDB_FAIL_LIGHT;
			return PWE_INVALID_FILESIZE;
		}
	}
	else // Repair the database
	{
		if(((uFileSize - sizeof(PW_DBHEADER)) % 16) != 0)
		{
			uFileSize -= sizeof(PW_DBHEADER); ASSERT((uFileSize & 0xF) != 0);
			uFileSize &= ~0xFUL;
			uFileSize += sizeof(PW_DBHEADER);
		}

		ASSERT(((uFileSize - sizeof(PW_DBHEADER)) % 16) == 0);

		pRepair->dwOriginalGroupCount = hdr.dwGroups;
		pRepair->dwOriginalEntryCount = hdr.dwEntries;
	}

	if(m_nAlgorithm == ALGO_AES)
	{
		CRijndael aes;

		// Initialize Rijndael algorithm
		if(aes.Init(CRijndael::CBC, CRijndael::DecryptDir, uFinalKey,
			CRijndael::Key32Bytes, hdr.aEncryptionIV) != RIJNDAEL_SUCCESS)
			{ _OPENDB_FAIL_LIGHT; return PWE_CRYPT_ERROR; }

		// Decrypt! The first bytes aren't encrypted (that's the header)
		uEncryptedPartSize = (unsigned long)aes.PadDecrypt((UINT8 *)pVirtualFile + sizeof(PW_DBHEADER),
			uFileSize - sizeof(PW_DBHEADER), (UINT8 *)pVirtualFile + sizeof(PW_DBHEADER));
	}
	else if(m_nAlgorithm == ALGO_TWOFISH)
	{
		CTwofish twofish;

		if(twofish.Init(uFinalKey, 32, hdr.aEncryptionIV) != true)
			{ _OPENDB_FAIL };

		uEncryptedPartSize = (unsigned long)twofish.PadDecrypt((UINT8 *)pVirtualFile + sizeof(PW_DBHEADER),
			uFileSize - sizeof(PW_DBHEADER), (UINT8 *)pVirtualFile + sizeof(PW_DBHEADER));
	}
	else
	{
		ASSERT(FALSE); _OPENDB_FAIL; // This should never happen
	}

	mem_erase(uFinalKey, 32);

#if 0
	// For debugging purposes, a file containing the plain text is created.
	// This code of course must not be compiled into the final binary.
#pragma message("PLAIN TEXT OUTPUT IS ENABLED!")
#pragma message("DO NOT DISTRIBUTE THIS BINARY!")
	// std::basic_string<TCHAR> tstrClear = pszFile;
	// tstrClear += _T(".plaintext.bin");
	// FILE *fpClear = NULL;
	// _tfopen_s(&fpClear, tstrClear.c_str(), _T("wb"));
	// fwrite(pVirtualFile, 1, uFileSize, fpClear);
	// fclose(fpClear); fpClear = NULL;
#endif

	// Check for success (non-repair mode only)
	if(pRepair == NULL)
	{
		if((uEncryptedPartSize > 2147483446) || ((uEncryptedPartSize == 0) &&
			((hdr.dwGroups != 0) || (hdr.dwEntries != 0))))
		{
			_OPENDB_FAIL_LIGHT;
			return PWE_INVALID_KEY;
		}
	}

	// Check if key is correct (with very high probability)
	if(pRepair == NULL)
	{
		unsigned char vContentsHash[32];
		sha256_begin(&sha32);
		sha256_hash((unsigned char *)pVirtualFile + sizeof(PW_DBHEADER), uEncryptedPartSize, &sha32);
		sha256_end(vContentsHash, &sha32);
		if(memcmp(hdr.aContentsHash, vContentsHash, 32) != 0)
			{ _OPENDB_FAIL_LIGHT; return PWE_INVALID_KEY; }
	}

	NewDatabase(); // Create a new database and initialize internal structures

	// Add groups from the memory file to the internal structures
	pos = sizeof(PW_DBHEADER);
	// char *pStart = &pVirtualFile[pos];
	DWORD uCurGroup = 0;
	while(uCurGroup < hdr.dwGroups)
	{
		p = &pVirtualFile[pos];

		PWMOD_CHECK_AVAIL(2);
		// if(pRepair != NULL) { if(IsBadReadPtr(p, 2) != FALSE) { _OPENDB_FAIL; } }
		memcpy(&usFieldType, p, 2);
		p += 2; pos += 2;
		// if(pos >= uFileSize) { _OPENDB_FAIL; }

		PWMOD_CHECK_AVAIL(4);
		// if(pRepair != NULL) { if(IsBadReadPtr(p, 4) != FALSE) { _OPENDB_FAIL; } }
		memcpy(&dwFieldSize, p, 4);
		p += 4; pos += 4;
		// if(pos >= (uFileSize + dwFieldSize)) { _OPENDB_FAIL; }

		PWMOD_CHECK_AVAIL(dwFieldSize);
		// if(pRepair != NULL) { if(IsBadReadPtr(p, dwFieldSize) != FALSE) { _OPENDB_FAIL; } }
		if(!ReadGroupField(usFieldType, dwFieldSize, (BYTE *)p,
			&pwGroupTemplate)) { _OPENDB_FAIL; }
		if(usFieldType == 0xFFFF)
			++uCurGroup; // Now and ONLY now the counter gets increased

		p += dwFieldSize;
		// if(p < pStart) { _OPENDB_FAIL; }
		pos += dwFieldSize;
		// if(pos >= uFileSize) { _OPENDB_FAIL; }
	}
	SAFE_DELETE_ARRAY(pwGroupTemplate.pszGroupName);

	// Get the entries
	DWORD uCurEntry = 0;
	while(uCurEntry < hdr.dwEntries)
	{
		p = &pVirtualFile[pos];

		PWMOD_CHECK_AVAIL(2);
		// if(pRepair != NULL) { if(IsBadReadPtr(p, 2) != FALSE) { _OPENDB_FAIL; } }
		memcpy(&usFieldType, p, 2);
		p += 2; pos += 2;
		// if(pos >= uFileSize) { _OPENDB_FAIL; }

		PWMOD_CHECK_AVAIL(4);
		// if(pRepair != NULL) { if(IsBadReadPtr(p, 4) != FALSE) { _OPENDB_FAIL; } }
		memcpy(&dwFieldSize, p, 4);
		p += 4; pos += 4;
		// if(pos >= (uFileSize + dwFieldSize)) { _OPENDB_FAIL; }

		PWMOD_CHECK_AVAIL(dwFieldSize);
		// if(pRepair != NULL) { if(IsBadReadPtr(p, dwFieldSize) != FALSE) { _OPENDB_FAIL; } }
		if(!ReadEntryField(usFieldType, dwFieldSize, (BYTE *)p,
			&pwEntryTemplate)) { _OPENDB_FAIL; }
		if(usFieldType == 0xFFFF)
			++uCurEntry; // Now and ONLY now the counter gets increased

		p += dwFieldSize;
		// if(p < pStart) { _OPENDB_FAIL; }
		pos += dwFieldSize;
		// if(pos >= uFileSize) { _OPENDB_FAIL; }
	}
	SAFE_DELETE_ARRAY(pwEntryTemplate.pszTitle);
	SAFE_DELETE_ARRAY(pwEntryTemplate.pszURL);
	SAFE_DELETE_ARRAY(pwEntryTemplate.pszUserName);
	SAFE_DELETE_ARRAY(pwEntryTemplate.pszPassword);
	SAFE_DELETE_ARRAY(pwEntryTemplate.pszAdditional);
	SAFE_DELETE_ARRAY(pwEntryTemplate.pszBinaryDesc);
	SAFE_DELETE_ARRAY(pwEntryTemplate.pBinaryData);

	memcpy(&m_dbLastHeader, &hdr, sizeof(PW_DBHEADER));

	// Erase and delete memory file
	mem_erase((unsigned char *)pVirtualFile, uAllocated);
	SAFE_DELETE_ARRAY(pVirtualFile);

	const DWORD dwRemovedStreams = _LoadAndRemoveAllMetaStreams(true);
	if(pRepair != NULL) pRepair->dwRecognizedMetaStreamCount = dwRemovedStreams;
	VERIFY(DeleteLostEntries() == 0);
	FixGroupTree();

	return PWE_SUCCESS;
}

int CPwManager::SaveDatabase(const TCHAR *pszFile, BYTE *pWrittenDataHash32)
{
	DWORD uEncryptedPartSize, uAllocated, i, pos, dwFieldSize;
	PW_DBHEADER hdr;
	UINT8 uFinalKey[32];
	sha256_ctx sha32;
	USHORT usFieldType;
	BYTE aCompressedTime[5];

	ASSERT(pszFile != NULL);
	if(pszFile == NULL) return PWE_INVALID_PARAM;
	ASSERT(pszFile[0] != 0);
	if(pszFile[0] == 0) return PWE_INVALID_PARAM;

	_AddAllMetaStreams();

	DWORD uFileSize = sizeof(PW_DBHEADER);
	BYTE *pbt;

	// Get the size of all groups
	for(i = 0; i < m_dwNumGroups; ++i)
	{
		uFileSize += 94; // 6+4+6+6+5+6+5+6+5+6+5+6+4+6+6+2+6+4 = 94

		pbt = _StringToUTF8(m_pGroups[i].pszGroupName);
		uFileSize += szlen((char *)pbt) + 1;
		SAFE_DELETE_ARRAY(pbt);
	}

	// Get the size of all entries together
	for(i = 0; i < m_dwNumEntries; ++i)
	{
		ASSERT_ENTRY(&m_pEntries[i]);

		UnlockEntryPassword(&m_pEntries[i]);

		uFileSize += 134; // 6+16+6+4+6+4+6+6+6+6+6+6+5+6+5+6+5+6+5+6 = 122

		pbt = _StringToUTF8(m_pEntries[i].pszTitle);
		uFileSize += szlen((char *)pbt) + 1;
		SAFE_DELETE_ARRAY(pbt);

		pbt = _StringToUTF8(m_pEntries[i].pszUserName);
		uFileSize += szlen((char *)pbt) + 1;
		SAFE_DELETE_ARRAY(pbt);

		pbt = _StringToUTF8(m_pEntries[i].pszURL);
		uFileSize += szlen((char *)pbt) + 1;
		SAFE_DELETE_ARRAY(pbt);

		pbt = _StringToUTF8(m_pEntries[i].pszPassword);
		uFileSize += szlen((char *)pbt) + 1;
		SAFE_DELETE_ARRAY(pbt);

		pbt = _StringToUTF8(m_pEntries[i].pszAdditional);
		uFileSize += szlen((char *)pbt) + 1;
		SAFE_DELETE_ARRAY(pbt);

		pbt = _StringToUTF8(m_pEntries[i].pszBinaryDesc);
		uFileSize += szlen((char *)pbt) + 1;
		SAFE_DELETE_ARRAY(pbt);

		uFileSize += m_pEntries[i].uBinaryDataLen;

		LockEntryPassword(&m_pEntries[i]);
	}

	// Round up filesize to 16-byte boundary for Rijndael/Twofish
	uFileSize = (uFileSize + 16) - (uFileSize % 16);

	// Allocate enough memory
	uAllocated = uFileSize + 16;
	char *pVirtualFile = new char[uAllocated];
	ASSERT(pVirtualFile != NULL);
	if(pVirtualFile == NULL) { _LoadAndRemoveAllMetaStreams(false); return PWE_NO_MEM; }

	// Build header structure
	hdr.dwSignature1 = PWM_DBSIG_1;
	hdr.dwSignature2 = PWM_DBSIG_2;

	hdr.dwFlags = PWM_FLAG_SHA2; // The one and only hash algorithm available currently

	if(m_nAlgorithm == ALGO_AES) hdr.dwFlags |= PWM_FLAG_RIJNDAEL;
	else if(m_nAlgorithm == ALGO_TWOFISH) hdr.dwFlags |= PWM_FLAG_TWOFISH;
	else { ASSERT(FALSE); _LoadAndRemoveAllMetaStreams(false); return PWE_INVALID_PARAM; }

	hdr.dwVersion = PWM_DBVER_DW;
	hdr.dwGroups = m_dwNumGroups;
	hdr.dwEntries = m_dwNumEntries;
	hdr.dwKeyEncRounds = m_dwKeyEncRounds;

	// Make up the master key hash seed and the encryption IV
	m_random.GetRandomBuffer(hdr.aMasterSeed, 16);
	m_random.GetRandomBuffer((BYTE *)hdr.aEncryptionIV, 16);
	m_random.GetRandomBuffer(hdr.aMasterSeed2, 32);

	// Skip the header, it will be written later
	pos = sizeof(PW_DBHEADER);

	BYTE *pb;

	// Store all groups to memory file
	for(i = 0; i < m_dwNumGroups; i++)
	{
		usFieldType = 0x0001; dwFieldSize = 4;
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;
		memcpy(&pVirtualFile[pos], &m_pGroups[i].uGroupId, 4); pos += 4;

		pb = _StringToUTF8(m_pGroups[i].pszGroupName);
		usFieldType = 0x0002; dwFieldSize = szlen((char *)pb) + 1;
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;
		ASSERT((pb != NULL) && (szlen((char *)pb) == (dwFieldSize - 1)) && ((pos + dwFieldSize) <= uAllocated));
		szcpy(&pVirtualFile[pos], (char *)pb); pos += dwFieldSize;
		SAFE_DELETE_ARRAY(pb);

		usFieldType = 0x0003; dwFieldSize = 5;
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;
		CPwUtil::PwTimeToTime(&m_pGroups[i].tCreation, aCompressedTime);
		memcpy(&pVirtualFile[pos], aCompressedTime, 5); pos += 5;

		usFieldType = 0x0004; dwFieldSize = 5;
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;
		CPwUtil::PwTimeToTime(&m_pGroups[i].tLastMod, aCompressedTime);
		memcpy(&pVirtualFile[pos], aCompressedTime, 5); pos += 5;

		usFieldType = 0x0005; dwFieldSize = 5;
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;
		CPwUtil::PwTimeToTime(&m_pGroups[i].tLastAccess, aCompressedTime);
		memcpy(&pVirtualFile[pos], aCompressedTime, 5); pos += 5;

		usFieldType = 0x0006; dwFieldSize = 5;
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;
		CPwUtil::PwTimeToTime(&m_pGroups[i].tExpire, aCompressedTime);
		memcpy(&pVirtualFile[pos], aCompressedTime, 5); pos += 5;

		usFieldType = 0x0007; dwFieldSize = 4;
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;
		memcpy(&pVirtualFile[pos], &m_pGroups[i].uImageId, 4); pos += 4;

		usFieldType = 0x0008; dwFieldSize = 2;
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;
		memcpy(&pVirtualFile[pos], &m_pGroups[i].usLevel, 2); pos += 2;

		usFieldType = 0x0009; dwFieldSize = 4;
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;
		memcpy(&pVirtualFile[pos], &m_pGroups[i].dwFlags, 4); pos += 4;

		usFieldType = 0xFFFF; dwFieldSize = 0;
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;
	}

	// Store all entries to memory file
	for(i = 0; i < m_dwNumEntries; i++)
	{
		UnlockEntryPassword(&m_pEntries[i]);

		usFieldType = 0x0001; dwFieldSize = 16;
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;
		memcpy(&pVirtualFile[pos], &m_pEntries[i].uuid, 16); pos += 16;

		usFieldType = 0x0002; dwFieldSize = 4;
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;
		memcpy(&pVirtualFile[pos], &m_pEntries[i].uGroupId, 4); pos += 4;

		usFieldType = 0x0003; dwFieldSize = 4;
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;
		memcpy(&pVirtualFile[pos], &m_pEntries[i].uImageId, 4); pos += 4;

		pb = _StringToUTF8(m_pEntries[i].pszTitle);
		usFieldType = 0x0004;
		dwFieldSize = szlen((char *)pb) + 1; // Add terminating NULL character space
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;
		ASSERT((pb != NULL) && (szlen((char *)pb) == (dwFieldSize - 1)) && ((pos + dwFieldSize) <= uAllocated));
		szcpy(&pVirtualFile[pos], (char *)pb); pos += dwFieldSize;
		SAFE_DELETE_ARRAY(pb);

		pb = _StringToUTF8(m_pEntries[i].pszURL);
		usFieldType = 0x0005;
		dwFieldSize = szlen((char *)pb) + 1; // Add terminating NULL character space
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;
		ASSERT((pb != NULL) && (szlen((char *)pb) == (dwFieldSize - 1)) && ((pos + dwFieldSize) <= uAllocated));
		szcpy(&pVirtualFile[pos], (char *)pb); pos += dwFieldSize;
		SAFE_DELETE_ARRAY(pb);

		pb = _StringToUTF8(m_pEntries[i].pszUserName);
		usFieldType = 0x0006;
		dwFieldSize = szlen((char *)pb) + 1; // Add terminating NULL character space
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;
		ASSERT((pb != NULL) && (szlen((char *)pb) == (dwFieldSize - 1)) && ((pos + dwFieldSize) <= uAllocated));
		szcpy(&pVirtualFile[pos], (char *)pb); pos += dwFieldSize;
		SAFE_DELETE_ARRAY(pb);

		pb = _StringToUTF8(m_pEntries[i].pszPassword);
		usFieldType = 0x0007;
		dwFieldSize = szlen((char *)pb) + 1; // Add terminating NULL character space
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;
		ASSERT((pb != NULL) && (szlen((char *)pb) == (dwFieldSize - 1)) && ((pos + dwFieldSize) <= uAllocated));
		szcpy(&pVirtualFile[pos], (char *)pb); pos += dwFieldSize;
		if(pb != NULL) mem_erase((unsigned char *)pb, szlen((char *)pb));
		SAFE_DELETE_ARRAY(pb);

		pb = _StringToUTF8(m_pEntries[i].pszAdditional);
		usFieldType = 0x0008;
		dwFieldSize = szlen((char *)pb) + 1; // Add terminating NULL character space
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;
		ASSERT((pb != NULL) && (szlen((char *)pb) == (dwFieldSize - 1)) && ((pos + dwFieldSize) <= uAllocated));
		szcpy(&pVirtualFile[pos], (char *)pb); pos += dwFieldSize;
		SAFE_DELETE_ARRAY(pb);

		usFieldType = 0x0009; dwFieldSize = 5;
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;
		CPwUtil::PwTimeToTime(&m_pEntries[i].tCreation, aCompressedTime);
		memcpy(&pVirtualFile[pos], aCompressedTime, 5); pos += 5;

		usFieldType = 0x000A; dwFieldSize = 5;
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;
		CPwUtil::PwTimeToTime(&m_pEntries[i].tLastMod, aCompressedTime);
		memcpy(&pVirtualFile[pos], aCompressedTime, 5); pos += 5;

		usFieldType = 0x000B; dwFieldSize = 5;
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;
		CPwUtil::PwTimeToTime(&m_pEntries[i].tLastAccess, aCompressedTime);
		memcpy(&pVirtualFile[pos], aCompressedTime, 5); pos += 5;

		usFieldType = 0x000C; dwFieldSize = 5;
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;
		CPwUtil::PwTimeToTime(&m_pEntries[i].tExpire, aCompressedTime);
		memcpy(&pVirtualFile[pos], aCompressedTime, 5); pos += 5;

		pb = _StringToUTF8(m_pEntries[i].pszBinaryDesc);
		usFieldType = 0x000D;
		dwFieldSize = szlen((char *)pb) + 1;
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;
		ASSERT((pb != NULL) && (szlen((char *)pb) == (dwFieldSize - 1)) && ((pos + dwFieldSize) <= uAllocated));
		szcpy(&pVirtualFile[pos], (char *)pb); pos += dwFieldSize;
		SAFE_DELETE_ARRAY(pb);

		usFieldType = 0x000E; dwFieldSize = m_pEntries[i].uBinaryDataLen;
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;
		if((m_pEntries[i].pBinaryData != NULL) && (dwFieldSize != 0))
			memcpy(&pVirtualFile[pos], m_pEntries[i].pBinaryData, dwFieldSize);
		pos += dwFieldSize;

		usFieldType = 0xFFFF; dwFieldSize = 0;
		memcpy(&pVirtualFile[pos], &usFieldType, 2); pos += 2;
		memcpy(&pVirtualFile[pos], &dwFieldSize, 4); pos += 4;

		LockEntryPassword(&m_pEntries[i]);
	}
	ASSERT((pos <= uFileSize) && ((pos + 33) > uAllocated));

	sha256_begin(&sha32);
	sha256_hash((unsigned char *)pVirtualFile + sizeof(PW_DBHEADER), pos - sizeof(PW_DBHEADER), &sha32);
	sha256_end((unsigned char *)hdr.aContentsHash, &sha32);

	// Now we have everything required to build up the header
	memcpy(pVirtualFile, &hdr, sizeof(PW_DBHEADER));

	// Generate m_pTransformedMasterKey from m_pMasterKey
	if(_TransformMasterKey(hdr.aMasterSeed2) == FALSE)
		{ ASSERT(FALSE); SAFE_DELETE_ARRAY(pVirtualFile); _LoadAndRemoveAllMetaStreams(false); return PWE_CRYPT_ERROR; }

	ProtectTransformedMasterKey(false);

	// Hash the master password with the generated hash salt
	sha256_begin(&sha32);
	sha256_hash(hdr.aMasterSeed, 16, &sha32);
	sha256_hash(m_pTransformedMasterKey, 32, &sha32);
	sha256_end((unsigned char *)uFinalKey, &sha32);

	ProtectTransformedMasterKey(true);

#if 0
	// For debugging purposes, a file containing the plain text is created.
	// This code of course must not be compiled into the final binary.
#pragma message("PLAIN TEXT OUTPUT IS ENABLED!")
#pragma message("DO NOT DISTRIBUTE THIS BINARY!")
	// std::basic_string<TCHAR> tstrClear = pszFile;
	// tstrClear += _T(".plaintext.bin");
	// FILE *fpClear = NULL;
	// _tfopen_s(&fpClear, tstrClear.c_str(), _T("wb"));
	// fwrite(pVirtualFile, 1, uFileSize, fpClear);
	// fclose(fpClear); fpClear = NULL;
#endif

	if(m_nAlgorithm == ALGO_AES)
	{
		CRijndael aes;
		if(aes.Init(CRijndael::CBC, CRijndael::EncryptDir, uFinalKey,
			CRijndael::Key32Bytes, hdr.aEncryptionIV) != RIJNDAEL_SUCCESS)
		{
			SAFE_DELETE_ARRAY(pVirtualFile);
			return PWE_CRYPT_ERROR;
		}

		uEncryptedPartSize = static_cast<unsigned long>(aes.PadEncrypt(
			(UINT8 *)pVirtualFile + sizeof(PW_DBHEADER), pos - sizeof(PW_DBHEADER),
			(UINT8 *)pVirtualFile + sizeof(PW_DBHEADER)));
	}
	else if(m_nAlgorithm == ALGO_TWOFISH)
	{
		CTwofish twofish;
		if(twofish.Init(uFinalKey, 32, hdr.aEncryptionIV) == false)
		{
			SAFE_DELETE_ARRAY(pVirtualFile);
			_LoadAndRemoveAllMetaStreams(false);
			return PWE_CRYPT_ERROR;
		}

		uEncryptedPartSize = static_cast<unsigned long>(twofish.PadEncrypt(
			(UINT8 *)pVirtualFile + sizeof(PW_DBHEADER),
			pos - sizeof(PW_DBHEADER), (UINT8 *)pVirtualFile + sizeof(PW_DBHEADER)));
	}
	else
	{
		ASSERT(FALSE);
		_OPENDB_FAIL_LIGHT;
		_LoadAndRemoveAllMetaStreams(false);
		return PWE_INVALID_PARAM;
	}

	mem_erase(uFinalKey, 32);

	// Check if all went correct
	ASSERT((uEncryptedPartSize % 16) == 0);
	if((uEncryptedPartSize > 2147483446) || ((uEncryptedPartSize == 0) &&
		(this->GetNumberOfGroups() != 0)))
	{
		ASSERT(FALSE);
		SAFE_DELETE_ARRAY(pVirtualFile);
		_LoadAndRemoveAllMetaStreams(false);
		return PWE_CRYPT_ERROR;
	}

	const DWORD dwToWrite = uEncryptedPartSize + sizeof(PW_DBHEADER);
	const int nWriteRes = AU_WriteBigFile(pszFile, (BYTE *)pVirtualFile, dwToWrite,
		m_bUseTransactedFileWrites);
	if(nWriteRes != PWE_SUCCESS)
	{
		mem_erase((unsigned char *)pVirtualFile, uAllocated);
		SAFE_DELETE_ARRAY(pVirtualFile);
		_LoadAndRemoveAllMetaStreams(false);
		return nWriteRes;
	}

	if(pWrittenDataHash32 != NULL) // Caller requests hash of written data
	{
		sha256_ctx shaWritten;
		sha256_begin(&shaWritten);
		sha256_hash((unsigned char *)pVirtualFile, dwToWrite, &shaWritten);
		sha256_end(pWrittenDataHash32, &shaWritten);
	}

	memcpy(&m_dbLastHeader, &hdr, sizeof(PW_DBHEADER)); // Backup last database header

	mem_erase((unsigned char *)pVirtualFile, uAllocated);
	SAFE_DELETE_ARRAY(pVirtualFile);
	_LoadAndRemoveAllMetaStreams(false);

	return PWE_SUCCESS;
}

#define PWMRF_CHECK_AVAIL(w_Cnt_q) { if(dwFieldSize < static_cast<DWORD>(w_Cnt_q)) { \
	ASSERT(FALSE); return false; } else { ASSERT(dwFieldSize == static_cast<DWORD>(w_Cnt_q)); } }

bool CPwManager::ReadGroupField(USHORT usFieldType, DWORD dwFieldSize,
	const BYTE *pData, PW_GROUP *pGroup)
{
	BYTE aCompressedTime[5];

	switch(usFieldType)
	{
	case 0x0000:
		// Ignore field
		break;
	case 0x0001:
		PWMRF_CHECK_AVAIL(4);
		memcpy(&pGroup->uGroupId, pData, 4);
		break;
	case 0x0002:
		ASSERT(dwFieldSize != 0);
		SAFE_DELETE_ARRAY(pGroup->pszGroupName);
		pGroup->pszGroupName = _UTF8ToString((UTF8_BYTE *)pData);
		break;
	case 0x0003:
		PWMRF_CHECK_AVAIL(5);
		memcpy(aCompressedTime, pData, 5);
		CPwUtil::TimeToPwTime(aCompressedTime, &pGroup->tCreation);
		break;
	case 0x0004:
		PWMRF_CHECK_AVAIL(5);
		memcpy(aCompressedTime, pData, 5);
		CPwUtil::TimeToPwTime(aCompressedTime, &pGroup->tLastMod);
		break;
	case 0x0005:
		PWMRF_CHECK_AVAIL(5);
		memcpy(aCompressedTime, pData, 5);
		CPwUtil::TimeToPwTime(aCompressedTime, &pGroup->tLastAccess);
		break;
	case 0x0006:
		PWMRF_CHECK_AVAIL(5);
		memcpy(aCompressedTime, pData, 5);
		CPwUtil::TimeToPwTime(aCompressedTime, &pGroup->tExpire);
		break;
	case 0x0007:
		PWMRF_CHECK_AVAIL(4);
		memcpy(&pGroup->uImageId, pData, 4);
		break;
	case 0x0008:
		PWMRF_CHECK_AVAIL(2);
		memcpy(&pGroup->usLevel, pData, 2);
		break;
	case 0x0009:
		PWMRF_CHECK_AVAIL(4);
		memcpy(&pGroup->dwFlags, pData, 4);
		break;
	case 0xFFFF:
		AddGroup(pGroup);
		SAFE_DELETE_ARRAY(pGroup->pszGroupName);
		RESET_PWG_TEMPLATE(pGroup);
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	return true;
}

bool CPwManager::ReadEntryField(USHORT usFieldType, DWORD dwFieldSize,
	const BYTE *pData, PW_ENTRY *pEntry)
{
	BYTE aCompressedTime[5];

	switch(usFieldType)
	{
	case 0x0000:
		// Ignore field
		break;
	case 0x0001:
		PWMRF_CHECK_AVAIL(16);
		memcpy(pEntry->uuid, pData, 16);
		break;
	case 0x0002:
		PWMRF_CHECK_AVAIL(4);
		memcpy(&pEntry->uGroupId, pData, 4);
		break;
	case 0x0003:
		PWMRF_CHECK_AVAIL(4);
		memcpy(&pEntry->uImageId, pData, 4);
		break;
	case 0x0004:
		ASSERT(dwFieldSize != 0);
		SAFE_DELETE_ARRAY(pEntry->pszTitle);
		pEntry->pszTitle = _UTF8ToString((UTF8_BYTE *)pData);
		break;
	case 0x0005:
		ASSERT(dwFieldSize != 0);
		SAFE_DELETE_ARRAY(pEntry->pszURL);
		pEntry->pszURL = _UTF8ToString((UTF8_BYTE *)pData);
		break;
	case 0x0006:
		ASSERT(dwFieldSize != 0);
		SAFE_DELETE_ARRAY(pEntry->pszUserName);
		pEntry->pszUserName = _UTF8ToString((UTF8_BYTE *)pData);
		break;
	case 0x0007:
		ASSERT(dwFieldSize != 0);
		if(pEntry->pszPassword != NULL)
			mem_erase((unsigned char *)pEntry->pszPassword, _tcslen(pEntry->pszPassword) * sizeof(TCHAR));
		SAFE_DELETE_ARRAY(pEntry->pszPassword);
		pEntry->pszPassword = _UTF8ToString((UTF8_BYTE *)pData);
		break;
	case 0x0008:
		ASSERT(dwFieldSize != 0);
		SAFE_DELETE_ARRAY(pEntry->pszAdditional);
		pEntry->pszAdditional = _UTF8ToString((UTF8_BYTE *)pData);
		break;
	case 0x0009:
		PWMRF_CHECK_AVAIL(5);
		memcpy(aCompressedTime, pData, 5);
		CPwUtil::TimeToPwTime(aCompressedTime, &pEntry->tCreation);
		break;
	case 0x000A:
		PWMRF_CHECK_AVAIL(5);
		memcpy(aCompressedTime, pData, 5);
		CPwUtil::TimeToPwTime(aCompressedTime, &pEntry->tLastMod);
		break;
	case 0x000B:
		PWMRF_CHECK_AVAIL(5);
		memcpy(aCompressedTime, pData, 5);
		CPwUtil::TimeToPwTime(aCompressedTime, &pEntry->tLastAccess);
		break;
	case 0x000C:
		PWMRF_CHECK_AVAIL(5);
		memcpy(aCompressedTime, pData, 5);
		CPwUtil::TimeToPwTime(aCompressedTime, &pEntry->tExpire);
		break;
	case 0x000D:
		ASSERT(dwFieldSize != 0);
		SAFE_DELETE_ARRAY(pEntry->pszBinaryDesc);
		pEntry->pszBinaryDesc = _UTF8ToString((UTF8_BYTE *)pData);
		break;
	case 0x000E:
		SAFE_DELETE_ARRAY(pEntry->pBinaryData);
		if(dwFieldSize != 0)
		{
			pEntry->pBinaryData = new BYTE[dwFieldSize];
			memcpy(pEntry->pBinaryData, pData, dwFieldSize);
			pEntry->uBinaryDataLen = dwFieldSize;
		}
		break;
	case 0xFFFF:
		ASSERT(dwFieldSize == 0);
		AddEntry(pEntry);
		SAFE_DELETE_ARRAY(pEntry->pszTitle);
		SAFE_DELETE_ARRAY(pEntry->pszURL);
		SAFE_DELETE_ARRAY(pEntry->pszUserName);
		if(pEntry->pszPassword != NULL)
			mem_erase((unsigned char *)pEntry->pszPassword, _tcslen(pEntry->pszPassword) * sizeof(TCHAR));
		SAFE_DELETE_ARRAY(pEntry->pszPassword);
		SAFE_DELETE_ARRAY(pEntry->pszAdditional);
		SAFE_DELETE_ARRAY(pEntry->pszBinaryDesc);
		SAFE_DELETE_ARRAY(pEntry->pBinaryData);
		RESET_PWE_TEMPLATE(pEntry);
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	return true;
}
