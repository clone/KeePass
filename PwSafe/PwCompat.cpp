/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2006 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "PwCompat.h"
#include "../Util/MemUtil.h"
#include "../Util/StrUtil.h"
#include "../Crypto/twoclass.h"
#include "../Crypto/sha2.h"
#include "../Crypto/arcfour.h"

#include <string.h>

static const BYTE g_uuidZero[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
static PW_TIME g_pwTimeNever = { 2999, 12, 28, 23, 59, 59 };
static char g_pNullString[4] = { 0, 0, 0, 0 };

#define _OPENDB_FAIL \
{ \
	if(pVirtualFile != NULL) \
	{ \
		mem_erase((unsigned char *)pVirtualFile, uAllocated); \
		SAFE_DELETE_ARRAY(pVirtualFile); \
	} \
	return FALSE; \
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

BOOL CPwManager::_OpenDatabaseV1(const TCHAR *pszFile)
{
	FILE *fp;
	char *pVirtualFile;
	unsigned long uFileSize, uAllocated, uEncryptedPartSize;
	unsigned long i, j, pos;
	PW_DBHEADER_V1 hdr;
	sha256_ctx sha32;
	RD_UINT8 uFinalKey[32];
	DWORD dw, dw2;
	char *ptrTemp;
	char *ptrTitle;
	char *ptrUserName;
	char *ptrURL;
	char *ptrPassword;
	char *ptrAdditional;
	PW_TIME tNow;
	PW_GROUP pwTG;
	PW_ENTRY pwTE;

	_GetCurrentPwTime(&tNow);
	memset(&pwTG, 0, sizeof(PW_GROUP)); memset(&pwTE, 0, sizeof(PW_ENTRY));
	pwTG.tCreation = tNow; pwTG.tExpire = g_pwTimeNever;
	pwTG.tLastAccess = tNow; pwTG.tLastMod = tNow;
	pwTE.tCreation = tNow; pwTE.tExpire = g_pwTimeNever;
	pwTE.tLastAccess = tNow; pwTE.tLastMod = tNow;

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

	if(uFileSize < sizeof(PW_DBHEADER_V1))
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
	memcpy(&hdr, pVirtualFile, sizeof(PW_DBHEADER_V1));

	// Check if we can open this
	if((hdr.dwSignature1 != PWM_DBSIG_1) || (hdr.dwSignature2 != PWM_DBSIG_2))
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
	memcpy(&hdr, pVirtualFile, sizeof(PW_DBHEADER_V1));

	NewDatabase(); // Create a new database and initialize internal structures

	// Add groups from the memory file to the internal structures
	j = 0;
	pos = sizeof(PW_DBHEADER_V1);
	for(i = 0; i < hdr.dwGroups; i++)
	{
		ptrTemp = &pVirtualFile[pos];
		j = strlen(ptrTemp);
		pos += j + 1;
		if(pos >= uFileSize) { _OPENDB_FAIL; }

		memcpy(&dw, &pVirtualFile[pos], sizeof(DWORD));
		pos += 4;
		if(pos >= uFileSize) { _OPENDB_FAIL; }

		pwTG.uGroupId = i + 1; pwTG.uImageId = dw;
#ifdef _UNICODE
		LPTSTR lpptr = _StringToUnicode(ptrTemp);
		pwTG.pszGroupName = lpptr;
		pwTG.usLevel = 0;
		AddGroup(&pwTG);
		SAFE_DELETE_ARRAY(lpptr);
#else
		pwTG.pszGroupName = ptrTemp;
		AddGroup(&pwTG);
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

		dw++; // Increase group ID, to be compatible with i+1 group ID importing

		ZeroMemory(pwTE.uuid, 16); // 0 = create new UUID

#ifdef _UNICODE
		LPTSTR lptTitle = _StringToUnicode(ptrTitle);
		LPTSTR lptURL = _StringToUnicode(ptrURL);
		LPTSTR lptUserName = _StringToUnicode(ptrUserName);
		LPTSTR lptPassword = _StringToUnicode(ptrPassword);
		LPTSTR lptAdditional = _StringToUnicode(ptrAdditional);
		pwTE.pszAdditional = lptAdditional; pwTE.pszPassword = lptPassword;
		pwTE.pszTitle = lptTitle; pwTE.pszURL = lptURL;
		pwTE.pszUserName = lptUserName; pwTE.uGroupId = dw; pwTE.uImageId = dw2;
		AddEntry(&pwTE);
		SAFE_DELETE_ARRAY(lptTitle); SAFE_DELETE_ARRAY(lptURL);
		SAFE_DELETE_ARRAY(lptUserName); SAFE_DELETE_ARRAY(lptPassword);
		SAFE_DELETE_ARRAY(lptAdditional);
#else
		pwTE.pszAdditional = ptrAdditional; pwTE.pszPassword = ptrPassword;
		pwTE.pszTitle = ptrTitle; pwTE.pszURL = ptrURL;
		pwTE.pszUserName = ptrUserName; pwTE.uGroupId = dw; pwTE.uImageId = dw2;
		AddEntry(&pwTE);
#endif
	}

	// Erase and delete memory file
	mem_erase((unsigned char *)pVirtualFile, uAllocated);
	SAFE_DELETE_ARRAY(pVirtualFile);
	return TRUE;
}

BOOL CPwManager::_OpenDatabaseV2(const TCHAR *pszFile)
{
	FILE *fp;
	char *pVirtualFile;
	unsigned long uFileSize, uAllocated, uEncryptedPartSize;
	unsigned long pos;
	PW_DBHEADER_V2 hdr;
	sha256_ctx sha32;
	RD_UINT8 uFinalKey[32];
	CString strTitle, strUserName, strURL, strPassword, strNotes;
	char *p;
	USHORT usFieldType;
	DWORD dwFieldSize;
	PW_GROUP pwGroupTemplate;
	PW_ENTRY pwEntryTemplate;

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

	if(uFileSize < sizeof(PW_DBHEADER_V2))
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
	memcpy(&hdr, pVirtualFile, sizeof(PW_DBHEADER_V2));

	// Check if we can open this
	if((hdr.dwSignature1 != PWM_DBSIG_1) || (hdr.dwSignature2 != PWM_DBSIG_2))
		{ _OPENDB_FAIL; }

	if(hdr.dwFlags & PWM_FLAG_RIJNDAEL) m_nAlgorithm = ALGO_AES;
	else if(hdr.dwFlags & PWM_FLAG_TWOFISH) m_nAlgorithm = ALGO_TWOFISH;
	else { ASSERT(FALSE); _OPENDB_FAIL; }

	// Hash the master password with the salt in the file
	sha256_begin(&sha32);
	sha256_hash(hdr.aMasterSeed, 16, &sha32);
	sha256_hash(m_pMasterKey, 32, &sha32);
	sha256_end((unsigned char *)uFinalKey, &sha32);

	ASSERT(((uFileSize - sizeof(PW_DBHEADER_V2)) % 16) == 0);
	if(m_nAlgorithm == ALGO_AES)
	{
		Rijndael aes;

		// Initialize Rijndael algorithm
		if(aes.init(Rijndael::CBC, Rijndael::Decrypt, uFinalKey,
			Rijndael::Key32Bytes, hdr.aEncryptionIV) != RIJNDAEL_SUCCESS)
			{ _OPENDB_FAIL; }

		// Decrypt! The first bytes aren't encrypted (that's the header)
		uEncryptedPartSize = (unsigned long)aes.padDecrypt((RD_UINT8 *)pVirtualFile + sizeof(PW_DBHEADER_V2),
			uFileSize - sizeof(PW_DBHEADER_V2), (RD_UINT8 *)pVirtualFile + sizeof(PW_DBHEADER_V2));
	}
	else if(m_nAlgorithm == ALGO_TWOFISH)
	{
		CTwofish twofish;

		if(twofish.init(uFinalKey, 32, hdr.aEncryptionIV) != true)
			{ _OPENDB_FAIL };

		uEncryptedPartSize = (unsigned long)twofish.padDecrypt((RD_UINT8 *)pVirtualFile + sizeof(PW_DBHEADER_V2),
			uFileSize - sizeof(PW_DBHEADER_V2), (RD_UINT8 *)pVirtualFile + sizeof(PW_DBHEADER_V2));
	}
	else
	{
		ASSERT(FALSE); _OPENDB_FAIL; // This should never happen
	}

	// Check for success
	if((uEncryptedPartSize > 2147483446) || (uEncryptedPartSize == 0))
		{ _OPENDB_FAIL; }

	// Check if key is correct (with high probability)
	sha256_begin(&sha32);
	sha256_hash((unsigned char *)pVirtualFile + sizeof(PW_DBHEADER_V2), uEncryptedPartSize, &sha32);
	sha256_end((unsigned char *)uFinalKey, &sha32);
	if(memcmp(hdr.aContentsHash, uFinalKey, 32) != 0)
		{ _OPENDB_FAIL; }

	NewDatabase(); // Create a new database and initialize internal structures

	memset(&pwGroupTemplate, 0, 16);
	RESET_PWG_TEMPLATE(&pwGroupTemplate);

	// Add groups from the memory file to the internal structures
	unsigned long uCurGroup;
	BOOL bRet;
	pos = sizeof(PW_DBHEADER_V2);
	for(uCurGroup = 0; uCurGroup < hdr.dwGroups; )
	{
		p = &pVirtualFile[pos];

		memcpy(&usFieldType, p, 2);
		p += 2; pos += 2;
		if(pos >= uFileSize) { _OPENDB_FAIL; }

		memcpy(&dwFieldSize, p, 4);
		p += 4; pos += 4;
		if(pos >= (uFileSize + dwFieldSize)) { _OPENDB_FAIL; }

		bRet = _ReadGroupFieldV2(usFieldType, dwFieldSize, (BYTE *)p, &pwGroupTemplate);
		if((usFieldType == 0xFFFF) && (bRet == TRUE))
			uCurGroup++; // Now and ONLY now the counter gets increased

		p += dwFieldSize;
		pos += dwFieldSize;
		if(pos >= uFileSize) { _OPENDB_FAIL; }
	}
	SAFE_DELETE_ARRAY(pwGroupTemplate.pszGroupName);

	RESET_PWE_TEMPLATE(&pwEntryTemplate);
	// Get the entries
	unsigned long uCurEntry;
	for(uCurEntry = 0; uCurEntry < hdr.dwEntries; )
	{
		p = &pVirtualFile[pos];

		memcpy(&usFieldType, p, 2);
		p += 2; pos += 2;
		if(pos >= uFileSize) { _OPENDB_FAIL; }

		memcpy(&dwFieldSize, p, 4);
		p += 4; pos += 4;
		if(pos >= (uFileSize + dwFieldSize)) { _OPENDB_FAIL; }

		bRet = _ReadEntryFieldV2(usFieldType, dwFieldSize, (BYTE *)p, &pwEntryTemplate);
		if((usFieldType == 0xFFFF) && (bRet == TRUE))
			uCurEntry++; // Now and ONLY now the counter gets increased

		p += dwFieldSize;
		pos += dwFieldSize;
		if(pos >= uFileSize) { _OPENDB_FAIL; }
	}
	SAFE_DELETE_ARRAY(pwEntryTemplate.pszTitle);
	SAFE_DELETE_ARRAY(pwEntryTemplate.pszURL);
	SAFE_DELETE_ARRAY(pwEntryTemplate.pszUserName);
	SAFE_DELETE_ARRAY(pwEntryTemplate.pszPassword);
	SAFE_DELETE_ARRAY(pwEntryTemplate.pszAdditional);
	SAFE_DELETE_ARRAY(pwEntryTemplate.pszBinaryDesc);
	SAFE_DELETE_ARRAY(pwEntryTemplate.pBinaryData);

	// Erase and delete memory file
	mem_erase((unsigned char *)pVirtualFile, uAllocated);
	SAFE_DELETE_ARRAY(pVirtualFile);
	return TRUE;
}

BOOL CPwManager::_ReadGroupFieldV2(USHORT usFieldType, DWORD dwFieldSize, BYTE *pData, PW_GROUP *pGroup)
{
	BYTE aCompressedTime[5];

	switch(usFieldType)
	{
	case 0x0000:
		// Ignore field
		break;
	case 0x0001:
		memcpy(&pGroup->uGroupId, pData, 4);
		break;
	case 0x0002:
		ASSERT(dwFieldSize != 0);
		SAFE_DELETE_ARRAY(pGroup->pszGroupName);
		pGroup->pszGroupName = _UTF8ToStringV2((UTF8_BYTE *)pData);
		break;
	case 0x0003:
		memcpy(aCompressedTime, pData, 5);
		_TimeToPwTime(aCompressedTime, &pGroup->tCreation);
		break;
	case 0x0004:
		memcpy(aCompressedTime, pData, 5);
		_TimeToPwTime(aCompressedTime, &pGroup->tLastMod);
		break;
	case 0x0005:
		memcpy(aCompressedTime, pData, 5);
		_TimeToPwTime(aCompressedTime, &pGroup->tLastAccess);
		break;
	case 0x0006:
		memcpy(aCompressedTime, pData, 5);
		_TimeToPwTime(aCompressedTime, &pGroup->tExpire);
		break;
	case 0x0007:
		memcpy(&pGroup->uImageId, pData, 4);
		break;
	case 0x0008:
		memcpy(&pGroup->usLevel, pData, 2);
		break;
	case 0x0009:
		memcpy(&pGroup->dwFlags, pData, 4);
		break;
	case 0xFFFF:
		AddGroup(pGroup);
		SAFE_DELETE_ARRAY(pGroup->pszGroupName);
		RESET_PWG_TEMPLATE(pGroup);
		break;
	default:
		return FALSE; // Field unsupported
	}

	return TRUE; // Field supported
}

BOOL CPwManager::_ReadEntryFieldV2(USHORT usFieldType, DWORD dwFieldSize, BYTE *pData, PW_ENTRY *pEntry)
{
	BYTE aCompressedTime[5];

	switch(usFieldType)
	{
	case 0x0000:
		// Ignore field
		break;
	case 0x0001:
		memcpy(pEntry->uuid, pData, 16);
		break;
	case 0x0002:
		memcpy(&pEntry->uGroupId, pData, 4);
		break;
	case 0x0003:
		memcpy(&pEntry->uImageId, pData, 4);
		break;
	case 0x0004:
		ASSERT(dwFieldSize != 0);
		SAFE_DELETE_ARRAY(pEntry->pszTitle);
		pEntry->pszTitle = _UTF8ToStringV2((UTF8_BYTE *)pData);
		break;
	case 0x0005:
		ASSERT(dwFieldSize != 0);
		SAFE_DELETE_ARRAY(pEntry->pszURL);
		pEntry->pszURL = _UTF8ToStringV2((UTF8_BYTE *)pData);
		break;
	case 0x0006:
		ASSERT(dwFieldSize != 0);
		SAFE_DELETE_ARRAY(pEntry->pszUserName);
		pEntry->pszUserName = _UTF8ToStringV2((UTF8_BYTE *)pData);
		break;
	case 0x0007:
		ASSERT(dwFieldSize != 0);
		SAFE_DELETE_ARRAY(pEntry->pszPassword);
		pEntry->pszPassword = _UTF8ToStringV2((UTF8_BYTE *)pData);
		break;
	case 0x0008:
		ASSERT(dwFieldSize != 0);
		SAFE_DELETE_ARRAY(pEntry->pszAdditional);
		pEntry->pszAdditional = _UTF8ToStringV2((UTF8_BYTE *)pData);
		break;
	case 0x0009:
		ASSERT(dwFieldSize == 5);
		memcpy(aCompressedTime, pData, 5);
		_TimeToPwTime(aCompressedTime, &pEntry->tCreation);
		break;
	case 0x000A:
		ASSERT(dwFieldSize == 5);
		memcpy(aCompressedTime, pData, 5);
		_TimeToPwTime(aCompressedTime, &pEntry->tLastMod);
		break;
	case 0x000B:
		ASSERT(dwFieldSize == 5);
		memcpy(aCompressedTime, pData, 5);
		_TimeToPwTime(aCompressedTime, &pEntry->tLastAccess);
		break;
	case 0x000C:
		ASSERT(dwFieldSize == 5);
		memcpy(aCompressedTime, pData, 5);
		_TimeToPwTime(aCompressedTime, &pEntry->tExpire);
		break;
	case 0x000D:
		ASSERT(dwFieldSize != 0);
		SAFE_DELETE_ARRAY(pEntry->pszBinaryDesc);
		pEntry->pszBinaryDesc = _UTF8ToStringV2((UTF8_BYTE *)pData);
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
		SAFE_DELETE_ARRAY(pEntry->pszPassword);
		SAFE_DELETE_ARRAY(pEntry->pszAdditional);
		SAFE_DELETE_ARRAY(pEntry->pszBinaryDesc);
		SAFE_DELETE_ARRAY(pEntry->pBinaryData);
		RESET_PWE_TEMPLATE(pEntry);
		break;
	default:
		return FALSE; // Field unsupported
	}

	return TRUE; // Field processed
}

TCHAR *_UTF8ToStringV2(const UTF8_BYTE *pUTF8String)
{
	DWORD i = 0, j = 0;
	DWORD dwNumChars;
	TCHAR *p;
	BYTE b0, b1, b2;
	TCHAR tch;

	ASSERT(pUTF8String != NULL);
	dwNumChars = _UTF8NumChars(pUTF8String);
	if(dwNumChars == 0) return NULL;

	p = new TCHAR[dwNumChars + 1];
	ASSERT(p != NULL);
	if(p == NULL) return NULL;

	while(1)
	{
		b0 = pUTF8String[i]; i++;

		if(b0 < 0x80)
		{
			p[j] = (TCHAR)b0; j++;
		}
		else
		{
			b1 = pUTF8String[i]; i++;

			ASSERT((b1 & 0xC0) == 0x80);
			if((b1 & 0xC0) != 0x80) break;

			if((b0 & 0xE0) == 0xC0)
			{
				tch = (TCHAR)(b0 & 0x1F);
				tch <<= 6;
				tch |= (b1 & 0x3F);
				p[j] = tch; j++;
			}
			else
			{
				b2 = pUTF8String[i]; i++;

				ASSERT((b2 & 0xC0) == 0x80);
				if((b2 & 0xC0) != 0x80) break;

				tch = (TCHAR)(b0 & 0xF);
				tch <<= 6;
				tch |= (b1 & 0x3F);
				tch <<= 6;
				tch |= (b2 & 0x3F);
				p[j] = tch; j++;
			}
		}

		if(b0 == 0) break;
	}

	return p;
}
