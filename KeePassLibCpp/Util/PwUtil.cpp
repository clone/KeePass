/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2014 Dominik Reichl <dominik.reichl@t-online.de>

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
#include <math.h>
#include <map>
#include <set>

#include "PwUtil.h"
#include "MemUtil.h"
#include "StrUtil.h"
#include "TranslateEx.h"
#include "../Crypto/ARCFour.h"

static const BYTE g_uuidZero[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

CPwUtil::CPwUtil()
{
}

BOOL CPwUtil::LoadHexKey32(FILE *fp, BYTE *pBuf)
{
	char buf[65], ch1, ch2;
	BYTE bt;

	ASSERT(fp != NULL); if(fp == NULL) return FALSE;
	ASSERT(pBuf != NULL); if(pBuf == NULL) return FALSE;

	buf[64] = 0;
	if(fread(buf, 1, 64, fp) != 64) { ASSERT(FALSE); return FALSE; }

	for(int i = 0; i < 32; ++i)
	{
		ch1 = buf[i * 2];
		ch2 = buf[i * 2 + 1];

		if(CPwUtil::ConvertStrToHex(ch1, ch2, bt) == FALSE) return FALSE;

		pBuf[i] = bt;
	}

	mem_erase((BYTE *)buf, 64);
	return TRUE;
}

BOOL CPwUtil::SaveHexKey32(FILE *fp, BYTE *pBuf)
{
	char buf[65], ch1, ch2;
	BYTE bt;

	ASSERT(fp != NULL); if(fp == NULL) return FALSE;
	ASSERT(pBuf != NULL); if(pBuf == NULL) return FALSE;

	buf[64] = 0;

	for(int i = 0; i < 32; ++i)
	{
		bt = pBuf[i];

		CPwUtil::ConvertHexToStr(bt, ch1, ch2);

		buf[i * 2] = ch1;
		buf[i * 2 + 1] = ch2;
	}

	fwrite(buf, 1, 64, fp);

	mem_erase((BYTE *)buf, 64);
	return TRUE;
}

LPCTSTR CPwUtil::GetUniCvtPtr(LPCTSTR lpBase, BOOL bTranslate)
{
	if(lpBase == NULL) return PWU_CVT_EX; // NULL is allowed, return unique default pointer

	if(bTranslate != FALSE) return _TRL(lpBase);
	return lpBase; // Untranslated
}

BOOL CPwUtil::ConvertStrToHex(char ch1, char ch2, BYTE& bt)
{
	if((ch1 >= '0') && (ch1 <= '9')) bt = static_cast<BYTE>(ch1 - '0');
	else if((ch1 >= 'a') && (ch1 <= 'f')) bt = static_cast<BYTE>(ch1 - 'a' + 10);
	else if((ch1 >= 'A') && (ch1 <= 'F')) bt = static_cast<BYTE>(ch1 - 'A' + 10);
	else return FALSE;

	bt <<= 4;

	if((ch2 >= '0') && (ch2 <= '9')) bt |= static_cast<BYTE>(ch2 - '0');
	else if((ch2 >= 'a') && (ch2 <= 'f')) bt |= static_cast<BYTE>(ch2 - 'a' + 10);
	else if((ch2 >= 'A') && (ch2 <= 'F')) bt |= static_cast<BYTE>(ch2 - 'A' + 10);
	else return FALSE;

	return TRUE;
}

void CPwUtil::ConvertHexToStr(BYTE bt, char& ch1, char& ch2)
{
	char chq = static_cast<char>(bt >> 4);
	if(chq < 10) ch1 = static_cast<char>(chq + '0');
	else ch1 = static_cast<char>(chq - 10 + 'a');

	chq = static_cast<char>(bt & 0x0F);
	if(chq < 10) ch2 = static_cast<char>(chq + '0');
	else ch2 = static_cast<char>(chq - 10 + 'a');
}

CString CPwUtil::FormatError(int nErrorCode, DWORD dwFlags)
{
	CString str;

	if(nErrorCode != PWE_SUCCESS)
	{
		TCHAR tszTemp[24];
		_stprintf_s(tszTemp, _countof(tszTemp), _T("%08X"),
			static_cast<unsigned int>(nErrorCode));

		if((dwFlags & PWFF_NO_INTRO) == 0)
		{
			str = TRL("An error occurred"); str += _T("!\r\n");
		}

		str += TRL("Error code"); str += _T(": 0x");
		str += tszTemp;
		str += _T(".");

		if((dwFlags & PWFF_NO_INTRO) == 0) str += _T("\r\n\r\n");
		else str += _T("\r\n");
	}

	if(((nErrorCode == PWE_INVALID_KEY) && ((dwFlags & PWFF_INVKEY_WITH_CODE) == 0)) ||
		(nErrorCode == PWE_UNSUPPORTED_KDBX) || ((dwFlags & PWFF_MAIN_TEXT_ONLY) != 0) ||
		(nErrorCode == PWE_GETLASTERROR))
		str.Empty();

	switch(nErrorCode)
	{
	case PWE_UNKNOWN:
		str += TRL("Unknown error");
		break;
	case PWE_SUCCESS:
		str += TRL("Success");
		break;
	case PWE_INVALID_PARAM:
		str += TRL("Invalid parameter");
		break;
	case PWE_NO_MEM:
		str += TRL("Too little memory (RAM) available");
		break;
	case PWE_INVALID_KEY:
		str += TRL("Invalid/wrong key");
		break;
	case PWE_NOFILEACCESS_READ:
		str += TRL("File access error: failed to open file in read mode");
		break;
	case PWE_NOFILEACCESS_WRITE:
		str += TRL("File access error: failed to open file in write mode");
		break;
	case PWE_FILEERROR_READ:
		str += TRL("File error: error while reading from the file");
		break;
	case PWE_FILEERROR_WRITE:
		str += TRL("File error: error while writing to the file");
		break;
	case PWE_INVALID_RANDOMSOURCE:
		str += TRL("Internal error"); str += _T(": ");
		str += TRL("Invalid random source");
		break;
	case PWE_INVALID_FILESTRUCTURE:
		str += TRL("Invalid/corrupted file structure");
		break;
	case PWE_CRYPT_ERROR:
		str += TRL("Encryption/decryption error");
		break;
	case PWE_INVALID_FILESIZE:
		str += TRL("Invalid/corrupted file structure");
		break;
	case PWE_INVALID_FILESIGNATURE:
		str += TRL("Invalid/corrupted file structure");
		break;
	case PWE_INVALID_FILEHEADER:
		str += TRL("Invalid/corrupted file structure");
		break;
	case PWE_NOFILEACCESS_READ_KEY:
		str += TRL("File access error: failed to open file in read mode");
		break;
	case PWE_KEYPROV_INVALID_KEY:
		str += TRL("The key provider plugin did not supply a valid key");
		break;
	case PWE_FILEERROR_VERIFY:
		str += TRL("File error: error while writing to the file"); // Same as write
		break;
	case PWE_UNSUPPORTED_KDBX:
		str += TRL("KeePass 1.x cannot open KDBX files.");
		str += _T("\r\n\r\n");
		str += TRL("Use the KeePass 2.x 'Export' feature to migrate this file");
		break;
	case PWE_GETLASTERROR:
		str += CPwUtil::FormatSystemMessage(GetLastError()).c_str();
		str = str.TrimRight(_T(".!\r\n"));
		break;
	case PWE_DB_EMPTY:
		str += TRL("The database is empty");
		break;
	default:
		ASSERT(FALSE);
		str += TRL("Unknown error");
		break;
	}
	str += _T(".");

	if(nErrorCode == PWE_NOFILEACCESS_READ_KEY)
	{
		str += _T("\r\n\r\n");
		str += TRL("Make sure the media that contains the key file is inserted and that KeePass has the rights to access it (check file access rights, ensure that no other application is blocking the file, ...).");
	}

	if((dwFlags & PWFF_DATALOSS_WITHOUT_SAVE) != 0)
	{
		str += _T("\r\n\r\n");
		str += TRL("The target file might be in a corrupted state. Please try saving again, and if that fails, save the database to a different location.");
	}

	return str;
}

BOOL CPwUtil::MemAllocCopyEntry(_In_ const PW_ENTRY *pExisting,
	_Out_ PW_ENTRY *pDestination)
{
	ASSERT_ENTRY(pExisting); ASSERT(pDestination != NULL);
	if((pExisting == NULL) || (pDestination == NULL)) return FALSE;

	ZeroMemory(pDestination, sizeof(PW_ENTRY));

	pDestination->uBinaryDataLen = pExisting->uBinaryDataLen;
	if(pExisting->pBinaryData != NULL)
	{
		pDestination->pBinaryData = new BYTE[pExisting->uBinaryDataLen + 1];
		ASSERT(pDestination->pBinaryData != NULL); if(pDestination->pBinaryData == NULL) return FALSE;
		pDestination->pBinaryData[pExisting->uBinaryDataLen] = 0;
		memcpy(pDestination->pBinaryData, pExisting->pBinaryData, pExisting->uBinaryDataLen);
	}

	pDestination->pszAdditional = _TcsSafeDupAlloc(pExisting->pszAdditional);
	pDestination->pszBinaryDesc = _TcsSafeDupAlloc(pExisting->pszBinaryDesc);
	pDestination->pszPassword = _TcsCryptDupAlloc(pExisting->pszPassword);
	pDestination->pszTitle = _TcsSafeDupAlloc(pExisting->pszTitle);
	pDestination->pszURL = _TcsSafeDupAlloc(pExisting->pszURL);
	pDestination->pszUserName = _TcsSafeDupAlloc(pExisting->pszUserName);

	pDestination->tCreation = pExisting->tCreation;
	pDestination->tExpire = pExisting->tExpire;
	pDestination->tLastAccess = pExisting->tLastAccess;
	pDestination->tLastMod = pExisting->tLastMod;

	pDestination->uGroupId = pExisting->uGroupId;
	pDestination->uImageId = pExisting->uImageId;
	pDestination->uPasswordLen = pExisting->uPasswordLen;
	memcpy(pDestination->uuid, pExisting->uuid, 16);

	return TRUE;
}

void CPwUtil::MemFreeEntry(_Inout_ PW_ENTRY *pEntry)
{
	ASSERT_ENTRY(pEntry); if(pEntry == NULL) return;

	SAFE_DELETE_ARRAY(pEntry->pBinaryData);
	SAFE_DELETE_ARRAY(pEntry->pszAdditional);
	SAFE_DELETE_ARRAY(pEntry->pszBinaryDesc);
	SAFE_DELETE_ARRAY(pEntry->pszPassword);
	SAFE_DELETE_ARRAY(pEntry->pszTitle);
	SAFE_DELETE_ARRAY(pEntry->pszURL);
	SAFE_DELETE_ARRAY(pEntry->pszUserName);

	ZeroMemory(pEntry, sizeof(PW_ENTRY));
}

void CPwUtil::TimeToPwTime(_In_bytecount_c_(5) const BYTE *pCompressedTime,
	_Out_ PW_TIME *pPwTime)
{
	DWORD dwYear, dwMonth, dwDay, dwHour, dwMinute, dwSecond;

	ASSERT((pCompressedTime != NULL) && (pPwTime != NULL));
	if(pPwTime == NULL) return;

	_UnpackStructToTime(pCompressedTime, &dwYear, &dwMonth, &dwDay, &dwHour, &dwMinute, &dwSecond);
	pPwTime->shYear = (USHORT)dwYear;
	pPwTime->btMonth = (BYTE)dwMonth;
	pPwTime->btDay = (BYTE)dwDay;
	pPwTime->btHour = (BYTE)dwHour;
	pPwTime->btMinute = (BYTE)dwMinute;
	pPwTime->btSecond = (BYTE)dwSecond;
}

void CPwUtil::PwTimeToTime(_In_ const PW_TIME *pPwTime,
	_Out_bytecap_c_(5) BYTE *pCompressedTime)
{
	ASSERT((pPwTime != NULL) && (pCompressedTime != NULL));
	if(pPwTime == NULL) return;

	_PackTimeToStruct(pCompressedTime, (DWORD)pPwTime->shYear, (DWORD)pPwTime->btMonth,
		(DWORD)pPwTime->btDay, (DWORD)pPwTime->btHour, (DWORD)pPwTime->btMinute,
		(DWORD)pPwTime->btSecond);
}

/* void CPwUtil::PwEntryStlToKpEntry(const PW_ENTRY_STL* pIn, KP_ENTRY* pOut,
	const PW_ENTRY* pOriginalEntry, const CPwManager* pMgr)
{
	if((pIn == NULL) || (pOut == NULL)) { ASSERT(FALSE); return; }
	// pOriginalEntry may be NULL
	if(pMgr == NULL) { ASSERT(FALSE); return; }

	ZeroMemory(pOut, sizeof(KP_ENTRY));

	pOut->pOriginalEntry = pOriginalEntry;

	memcpy(pOut->uuid, pIn->uuid, 16);
	pOut->uGroupIndex = pMgr->GetGroupByIdN(pIn->uGroupId);
	pOut->uImageId = pIn->uImageId;
	pOut->lpTitle = _TcsSafeDupAlloc(pIn->strTitle.c_str());
	pOut->lpURL = _TcsSafeDupAlloc(pIn->strURL.c_str());
	pOut->lpUserName = _TcsSafeDupAlloc(pIn->strUserName.c_str());
	pOut->lpPassword = _TcsSafeDupAlloc(pIn->strPassword.c_str());
	pOut->lpAdditional = _TcsSafeDupAlloc(pIn->strAdditional.c_str());
}

void CPwUtil::ClearPwEntryStl(PW_ENTRY_STL* p)
{
	if(p == NULL) { ASSERT(FALSE); return; }

	ZeroMemory(p->uuid, 16);
	p->uGroupId = 0;
	p->uImageId = 0;
	p->strTitle.clear();
	p->strURL.clear();
	p->strUserName.clear();
	p->strPassword.clear();
	p->strAdditional.clear();
	CPwManager::GetNeverExpireTime(&p->tCreation);
	CPwManager::GetNeverExpireTime(&p->tLastMod);
	CPwManager::GetNeverExpireTime(&p->tLastAccess);
	CPwManager::GetNeverExpireTime(&p->tExpire);
	p->strBinaryDesc.clear();
	p->vBinaryData.clear();
} */

void CPwUtil::FreeKpEntry(KP_ENTRY* p)
{
	if(p == NULL) { ASSERT(FALSE); return; }

	SAFE_DELETE_ARRAY(p->lpTitle);
	SAFE_DELETE_ARRAY(p->lpURL);
	SAFE_DELETE_ARRAY(p->lpUserName);
	SAFE_DELETE_ARRAY(p->lpAdditional);

	if(p->lpPassword != NULL)
		mem_erase((unsigned char *)p->lpPassword, _tcslen(p->lpPassword) * sizeof(TCHAR));
	SAFE_DELETE_ARRAY(p->lpPassword);
}

/* bool CPwUtil::EqualsPwEntryStl(const PW_ENTRY_STL* p1, const PW_ENTRY_STL* p2)
{
	if((p1 == NULL) || (p2 == NULL)) { ASSERT(FALSE); return false; }

	if(memcmp(p1->uuid, p2->uuid, 16) != 0) return false;
	if(p1->uGroupId != p2->uGroupId) return false;
	if(p1->uImageId != p2->uImageId) return false;
	if(p1->strTitle != p2->strTitle) return false;
	if(p1->strURL != p2->strURL) return false;
	if(p1->strUserName != p2->strUserName) return false;
	if(p1->strPassword != p2->strPassword) return false;
	if(p1->strAdditional != p2->strAdditional) return false;

	// Do not compare times

	if(p1->strBinaryDesc != p2->strBinaryDesc) return false;
	if(p1->vBinaryData.size() != p2->vBinaryData.size()) return false;
	if(p1->vBinaryData.size() > 0)
	{
		if(memcmp(&p1->vBinaryData[0], &p2->vBinaryData[0],
			p1->vBinaryData.size()) != 0) return false;
	}

	return true;
} */

void CPwUtil::HashKpEntry(const KP_ENTRY* pEntry, BYTE* pHash32)
{
	if((pEntry == NULL) || (pHash32 == NULL)) { ASSERT(FALSE); return; }

	sha256_ctx sha32;
	sha256_begin(&sha32);

	sha256_hash(pEntry->uuid, 16, &sha32);
	sha256_hash((const BYTE*)&pEntry->uGroupIndex, sizeof(DWORD), &sha32);
	sha256_hash((const BYTE*)&pEntry->uImageId, sizeof(DWORD), &sha32);
	HashStringWithTerm(pEntry->lpTitle, sha32);
	HashStringWithTerm(pEntry->lpURL, sha32);
	HashStringWithTerm(pEntry->lpUserName, sha32);
	HashStringWithTerm(pEntry->lpPassword, sha32);
	HashStringWithTerm(pEntry->lpAdditional, sha32);

	sha256_end(pHash32, &sha32);
}

void CPwUtil::HashStringWithTerm(LPCTSTR lp, sha256_ctx& sha32)
{
	if(lp != NULL)
		sha256_hash((BYTE*)lp, static_cast<unsigned long>((_tcslen(lp) + 1) *
			sizeof(TCHAR)), &sha32);
	else // lp == NULL
	{
		const TCHAR tch0 = 0;
		sha256_hash((const BYTE*)&tch0, sizeof(TCHAR), &sha32);
	}
}

BOOL CPwUtil::AttachFileAsBinaryData(_Inout_ PW_ENTRY *pEntry,
	const TCHAR *lpFile)
{
	FILE *fp = NULL;
	LPTSTR pBinaryDesc;

	ASSERT_ENTRY(pEntry); if(pEntry == NULL) return FALSE;
	ASSERT(lpFile != NULL); if(lpFile == NULL) return FALSE;

	_tfopen_s(&fp, lpFile, _T("rb"));
	if(fp == NULL) return FALSE;

	fseek(fp, 0, SEEK_END);
	const DWORD dwFileLen = static_cast<DWORD>(ftell(fp));
	fseek(fp, 0, SEEK_SET);

	if(dwFileLen == 0) { fclose(fp); fp = NULL; return FALSE; }
	ASSERT(dwFileLen > 0);

	SAFE_DELETE_ARRAY(pEntry->pszBinaryDesc);
	SAFE_DELETE_ARRAY(pEntry->pBinaryData);

	DWORD i = static_cast<DWORD>(_tcslen(lpFile)) - 1;
	while(1)
	{
		if(i == (DWORD)(-1)) break;
		if((lpFile[i] == '/') || (lpFile[i] == '\\')) break;
		--i;
	}
	pBinaryDesc = (LPTSTR)&lpFile[i + 1];

	const DWORD dwPathLen = static_cast<DWORD>(_tcslen(pBinaryDesc));

	pEntry->pszBinaryDesc = new TCHAR[dwPathLen + 1];
	_tcscpy_s(pEntry->pszBinaryDesc, dwPathLen + 1, pBinaryDesc);

	pEntry->pBinaryData = new BYTE[dwFileLen];
	fread(pEntry->pBinaryData, 1, dwFileLen, fp);

	pEntry->uBinaryDataLen = dwFileLen;

	fclose(fp); fp = NULL;
	return TRUE;
}

BOOL CPwUtil::SaveBinaryData(_In_ const PW_ENTRY *pEntry,
	const TCHAR *lpFile)
{
	FILE *fp = NULL;

	ASSERT_ENTRY(pEntry); if(pEntry == NULL) return FALSE;
	ASSERT(lpFile != NULL); if(lpFile == NULL) return FALSE;
	if(_tcslen(pEntry->pszBinaryDesc) == 0) return FALSE;

	_tfopen_s(&fp, lpFile, _T("wb"));
	if(fp == NULL) return FALSE;

	if(pEntry->uBinaryDataLen != 0)
		fwrite(pEntry->pBinaryData, 1, pEntry->uBinaryDataLen, fp);

	fclose(fp); fp = NULL;
	return TRUE;
}

BOOL CPwUtil::RemoveBinaryData(_Inout_ PW_ENTRY *pEntry)
{
	ASSERT_ENTRY(pEntry); if(pEntry == NULL) return FALSE;

	SAFE_DELETE_ARRAY(pEntry->pBinaryData);
	SAFE_DELETE_ARRAY(pEntry->pszBinaryDesc);
	pEntry->pszBinaryDesc = new TCHAR[1];
	pEntry->pszBinaryDesc[0] = 0;
	pEntry->uBinaryDataLen = 0;
	return TRUE;
}

BOOL CPwUtil::IsAllowedStoreGroup(LPCTSTR lpGroupName, LPCTSTR lpSearchGroupName)
{
	ASSERT(lpGroupName != NULL); if(lpGroupName == NULL) return FALSE;

	if(_tcscmp(lpGroupName, lpSearchGroupName) == 0) return FALSE;
	return TRUE;
}

BOOL CPwUtil::IsZeroUUID(_In_bytecount_c_(16) const BYTE *pUUID)
{
	if(pUUID == NULL) return TRUE;
	return ((memcmp(pUUID, g_uuidZero, 16) == 0) ? TRUE : FALSE);
}

BOOL CPwUtil::IsTANEntry(const PW_ENTRY *pe)
{
	ASSERT(pe != NULL); if(pe == NULL) return FALSE;

	return ((_tcscmp(pe->pszTitle, PWS_TAN_ENTRY) != 0) ? FALSE : TRUE);
}

/* void CPwUtil::ProtectMemory(UINT8 *pMem, size_t uBytes, bool bEncrypt)
{
#ifdef _WIN32
	DWORD dwBytes = static_cast<DWORD>(uBytes);
	dwBytes += CRYPTPROTECTMEMORY_BLOCK_SIZE - (dwBytes % CRYPTPROTECTMEMORY_BLOCK_SIZE);
	ASSERT((uBytes <= dwBytes) && (dwBytes <= uBytes + CRYPTPROTECTMEMORY_BLOCK_SIZE));
	ASSERT((dwBytes % CRYPTPROTECTMEMORY_BLOCK_SIZE) == 0);

	if(bEncrypt)
	{
		VERIFY(CryptProtectMemory(pMem, dwBytes, CRYPTPROTECTMEMORY_SAME_PROCESS));
	}
	else
	{
		VERIFY(CryptUnprotectMemory(pMem, dwBytes, CRYPTPROTECTMEMORY_SAME_PROCESS));
	}
#endif
} */

/* CPwErrorInfo::CPwErrorInfo()
{
	m_nPwErrorCode = PWE_SUCCESS;
	m_dwLastError = 0;
}

void CPwErrorInfo::SetInfo(const std::basic_string<TCHAR>& strText,
	int nPwErrorCode, BOOL bUseLastError)
{
	m_strText = strText;
	m_nPwErrorCode = nPwErrorCode;

	if(bUseLastError == TRUE) m_dwLastError = GetLastError();
	else m_dwLastError = 0;

	this->FormatInfo();
}

void CPwErrorInfo::FormatInfo()
{
	std::basic_string<TCHAR> str;

	if(m_strText.size() > 0)
	{
		str += m_strText;
		str += _T("\r\n\r\n");
	}

	str += PWM_FormatStaticError(m_nPwErrorCode, 0);
	str += _T("\r\n\r\n");

	if(m_dwLastError != 0)
	{
		LPTSTR lpBuffer = NULL;

		if(FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, m_dwLastError, 0, (LPTSTR)&lpBuffer, 1, NULL) != 0)
		{
			str += lpBuffer;
		}

		if(lpBuffer != NULL) { LocalFree(lpBuffer); lpBuffer = NULL; }
	}

	m_strFinal = str;
}

std::basic_string<TCHAR> CPwErrorInfo::ToString() const
{
	return m_strFinal;
}
*/

CString CPwUtil::CreateUUIDStr(CNewRandom* pRandomSource)
{
	if(pRandomSource == NULL) { ASSERT(FALSE); return CString(); }

	BYTE pbUuid[16];
	randCreateUUID(&pbUuid[0], pRandomSource);

	CString str;
	_UuidToString(&pbUuid[0], &str);
	return str;
}

DWORD CPwUtil::GetUniCPT(LPCTSTR lp)
{
	DWORD dwRel = 0; // See CPT specs
	while(*lp != 0) { dwRel = (ROTL32UE(dwRel, 5) + *lp); ++lp; }
	return dwRel;
}

PG_TREENODE CPwUtil::GroupsToTree(CPwManager* pMgr)
{
	PG_TREENODE v;
	ZeroMemory(&v.g, sizeof(PW_GROUP));

	if(pMgr == NULL) { ASSERT(FALSE); return v; }

	const DWORD dwGroupCount = pMgr->GetNumberOfGroups();
	for(DWORD i = 0; i < dwGroupCount; ++i)
		pMgr->GetGroup(i)->dwFlags &= ~PWGF_TEMP_BIT; // Clear

	DWORD dwAllocCount = 0;
	for(DWORD i = 0; i < dwGroupCount; ++i)
	{
		PW_GROUP* pg = pMgr->GetGroup(i);
		if(pg->usLevel == 0)
		{
			ASSERT((pg->dwFlags & PWGF_TEMP_BIT) == 0);
			v.vChildNodes.push_back(GroupToTreeNode(pMgr, i, dwAllocCount));
		}
		else { ASSERT((pg->dwFlags & PWGF_TEMP_BIT) != 0); }
	}

	ASSERT((pMgr->GetNumberOfGroups() == dwGroupCount) && (dwAllocCount == dwGroupCount));
	return v;
}

boost::shared_ptr<PG_TREENODE> CPwUtil::GroupToTreeNode(CPwManager* pMgr,
	DWORD dwIndex, DWORD& dwAllocCount)
{
	boost::shared_ptr<PG_TREENODE> p(new PG_TREENODE());
	PW_GROUP* pg = pMgr->GetGroup(dwIndex);

	++dwAllocCount;
	p->g = *pg;
	pg->dwFlags |= PWGF_TEMP_BIT;

	while(true)
	{
		++dwIndex;
		if(dwIndex >= pMgr->GetNumberOfGroups()) break;

		PW_GROUP* pgSub = pMgr->GetGroup(dwIndex);
		if(pgSub->usLevel <= pg->usLevel) break;

		if((pgSub->dwFlags & PWGF_TEMP_BIT) == 0)
			p->vChildNodes.push_back(GroupToTreeNode(pMgr, dwIndex, dwAllocCount));
	}

	return p;
}

boost::shared_ptr<PG_TREENODE> CPwUtil::FindGroupInTree(PG_TREENODE* pRoot,
	DWORD dwGroupId, bool bUnlinkGroup, int iMoveGroup)
{
	boost::shared_ptr<PG_TREENODE> pNull;

	if(pRoot == NULL) { ASSERT(FALSE); return pNull; }

	std::vector<boost::shared_ptr<PG_TREENODE> >& v = pRoot->vChildNodes;
	size_t uCurrent = 0;

	for(std::vector<boost::shared_ptr<PG_TREENODE> >::iterator it = v.begin();
		it != v.end(); ++it)
	{
		boost::shared_ptr<PG_TREENODE> p = *it;
		if(p->g.uGroupId == dwGroupId)
		{
			if(bUnlinkGroup) v.erase(it);
			else if(v.size() >= 2)
			{
				if((iMoveGroup == -2) && (uCurrent > 0))
				{
					v.erase(it);
					v.insert(v.begin(), p);
				}
				else if((iMoveGroup == 2) && (uCurrent < (v.size() - 1)))
				{
					v.erase(it);
					v.push_back(p);
				}
				else if((iMoveGroup == -1) && (uCurrent > 0))
				{
					v[uCurrent] = v[uCurrent - 1];
					v[uCurrent - 1] = p;
				}
				else if((iMoveGroup == 1) && (uCurrent < (v.size() - 1)))
				{
					v[uCurrent] = v[uCurrent + 1];
					v[uCurrent + 1] = p;
				}
			}

			return p;
		}
		else
		{
			boost::shared_ptr<PG_TREENODE> pSub = FindGroupInTree(p.get(),
				dwGroupId, bUnlinkGroup, iMoveGroup);

			if(pSub.get() != NULL) return pSub;
		}

		++uCurrent;
	}

	return pNull;
}

void CPwUtil::FlattenGroupTree(PW_GROUP* pStorage, PG_TREENODE* pRoot,
	DWORD dwStorageCount)
{
	DWORD dwIndex = 0;

	FlattenGroupTreeInternal(pStorage, pRoot, dwIndex, dwStorageCount, 0);

	ASSERT(dwIndex == dwStorageCount);
}

void CPwUtil::FlattenGroupTreeInternal(PW_GROUP* pStorage, PG_TREENODE* pRoot,
	DWORD& dwIndex, DWORD dwStorageCount, USHORT usLevel)
{
	for(size_t i = 0; i < pRoot->vChildNodes.size(); ++i)
	{
		if(dwIndex >= dwStorageCount) { ASSERT(FALSE); return; }

		pStorage[dwIndex] = pRoot->vChildNodes[i]->g;
		pStorage[dwIndex].usLevel = usLevel;
		++dwIndex;

		FlattenGroupTreeInternal(pStorage, pRoot->vChildNodes[i].get(), dwIndex,
			dwStorageCount, usLevel + 1);
	}
}

#ifdef _DEBUG
void CPwUtil::CheckGroupList(CPwManager* pMgr)
{
	std::set<DWORD> vIds;
	USHORT usLastLevel = 0;

	for(DWORD i = 0; i < pMgr->GetNumberOfGroups(); ++i)
	{
		PW_GROUP* pg = pMgr->GetGroup(i);
		ASSERT(pg != NULL);

		ASSERT((pg->uGroupId != 0) && (pg->uGroupId != DWORD_MAX));

		const DWORD dwGroupId = pg->uGroupId;
		ASSERT(vIds.find(dwGroupId) == vIds.end());

		ASSERT(pg->usLevel <= (usLastLevel + 1));

		vIds.insert(dwGroupId);
		usLastLevel = pg->usLevel;
	}
}
#endif

std::basic_string<TCHAR> CPwUtil::FormatSystemMessage(DWORD dwLastErrorCode)
{
	std::basic_string<TCHAR> str;
	LPTSTR lpBuffer = NULL;

	if(FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, dwLastErrorCode, 0, (LPTSTR)&lpBuffer, 1, NULL) != 0)
	{
		str = lpBuffer;
	}

	if(lpBuffer != NULL) { LocalFree(lpBuffer); lpBuffer = NULL; }

	return str;
}

bool CPwUtil::UnhideFile(LPCTSTR lpFile)
{
	if(lpFile == NULL) { ASSERT(FALSE); return false; }

	const DWORD dwAttrib = GetFileAttributes(lpFile);
	if(dwAttrib == INVALID_FILE_ATTRIBUTES) return false;

	if((dwAttrib & FILE_ATTRIBUTE_HIDDEN) == 0) return false;

	return CPwUtil::HideFile(lpFile, false);
}

bool CPwUtil::HideFile(LPCTSTR lpFile, bool bHide)
{
	if(lpFile == NULL) { ASSERT(FALSE); return false; }

	DWORD dwAttrib = GetFileAttributes(lpFile);
	if(dwAttrib == INVALID_FILE_ATTRIBUTES) return false;

	if(bHide) dwAttrib = ((dwAttrib & ~FILE_ATTRIBUTE_NORMAL) | FILE_ATTRIBUTE_HIDDEN);
	else // Unhide
	{
		dwAttrib &= ~FILE_ATTRIBUTE_HIDDEN;
		if(dwAttrib == 0) dwAttrib |= FILE_ATTRIBUTE_NORMAL;
	}

	return (SetFileAttributes(lpFile, dwAttrib) != FALSE);
}

CNullableEx<FILETIME> CPwUtil::GetFileCreationTime(LPCTSTR lpFile)
{
	if(lpFile == NULL) { ASSERT(FALSE); return CNullableEx<FILETIME>(); }

	HANDLE h = CreateFile(lpFile, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, 0, NULL);
	if(h == INVALID_HANDLE_VALUE) return CNullableEx<FILETIME>();

	FILETIME vTimes[3];
	const BOOL bResult = GetFileTime(h, &vTimes[0], &vTimes[1], &vTimes[2]);
	VERIFY(CloseHandle(h));

	if(bResult == FALSE) { ASSERT(FALSE); return CNullableEx<FILETIME>(); }

	return CNullableEx<FILETIME>(vTimes[0]);
}

bool CPwUtil::SetFileCreationTime(LPCTSTR lpFile, const FILETIME* pTime)
{
	if(lpFile == NULL) { ASSERT(FALSE); return false; }

	HANDLE h = CreateFile(lpFile, FILE_WRITE_ATTRIBUTES, 0, NULL,
		OPEN_EXISTING, 0, NULL);
	if(h == INVALID_HANDLE_VALUE) return false;

	VERIFY(SetFileTime(h, pTime, NULL, NULL));
	VERIFY(CloseHandle(h));
	return true;
}

bool CPwUtil::EfsEncryptFile(LPCTSTR lpFile)
{
	if(lpFile == NULL) { ASSERT(FALSE); return false; }

	HMODULE hLib = ::LoadLibrary(_T("AdvApi32.dll"));
	if(hLib == NULL) { ASSERT(FALSE); return false; }

	bool bResult = false;
	LPENCRYPTFILE lpEncryptFile = (LPENCRYPTFILE)::GetProcAddress(hLib, ENCRYPTFILE_FNNAME);
	if(lpEncryptFile != NULL) { bResult = (lpEncryptFile(lpFile) != FALSE); }
	else { ASSERT(FALSE); }

	::FreeLibrary(hLib);
	return bResult;
}

bool CPwUtil::IsDatabaseFile(LPCTSTR lpFile)
{
	if(lpFile == NULL) { ASSERT(FALSE); return false; }

	FILE* fp = NULL;
	_tfopen_s(&fp, lpFile, _T("rb"));
	if(fp == NULL) return false;

	fseek(fp, 0, SEEK_END);
	const long lFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	bool bResult = false;
	if(lFileSize >= 8)
	{
		BYTE vStart[8];
		if(fread(vStart, 1, 8, fp) == 8)
		{
			const DWORD dwSig1 = *(DWORD*)&vStart[0];
			const DWORD dwSig2 = *(DWORD*)&vStart[4];

			if(((dwSig1 == PWM_DBSIG_1) && (dwSig2 == PWM_DBSIG_2)) ||
				((dwSig1 == PWM_DBSIG_1_KDBX_P) && (dwSig2 == PWM_DBSIG_2_KDBX_P)) ||
				((dwSig1 == PWM_DBSIG_1_KDBX_R) && (dwSig2 == PWM_DBSIG_2_KDBX_R)))
				bResult = true;
		}
		else { ASSERT(FALSE); }
	}

	fclose(fp);
	return bResult;
}
