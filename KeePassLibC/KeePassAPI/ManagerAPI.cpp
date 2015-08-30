/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2007 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "ManagerAPI.h"
#include "../../KeePassLibCpp/Util/MemUtil.h"

static BOOL g_bRandomGenInit = FALSE; // Random generator initialized?

KP_SHARE void InitManager(void **pMgr, BOOL bIsFirstInstance)
{
	ASSERT(pMgr != NULL); if(pMgr == NULL) return;

	CPwManager *pNewMgr = new CPwManager();

	if(bIsFirstInstance != FALSE)
	{
		ASSERT(g_bRandomGenInit == FALSE);

		if(g_bRandomGenInit == FALSE) pNewMgr->InitPrimaryInstance();
		g_bRandomGenInit = TRUE;
	}

	*pMgr = pNewMgr;
}

KP_SHARE void DeleteManager(void *pMgr)
{
	DECL_MGR_V(pMgr);

	delete p; p = NULL;
}

KP_SHARE INT SetMasterKey(void *pMgr, LPCTSTR pszMasterKey, BOOL bDiskDrive, LPCTSTR pszSecondKey, const CNewRandomInterface *pARI, BOOL bOverwrite)
{
	DECL_MGR_N(pMgr);
	return p->SetMasterKey(pszMasterKey, bDiskDrive, pszSecondKey, pARI, bOverwrite);
}

// Returns number of entries in database
KP_SHARE DWORD GetNumberOfEntries(void *pMgr)
{
	DECL_MGR_N(pMgr);
	return p->GetNumberOfEntries();
}

// Returns number of groups in database
KP_SHARE DWORD GetNumberOfGroups(void *pMgr)
{
	DECL_MGR_N(pMgr);
	return p->GetNumberOfGroups();
}

// Count items in groups
KP_SHARE DWORD GetNumberOfItemsInGroup(void *pMgr, LPCTSTR pszGroup)
{
	DECL_MGR_N(pMgr);
	return p->GetNumberOfItemsInGroup(pszGroup);
}

KP_SHARE DWORD GetNumberOfItemsInGroupN(void *pMgr, DWORD idGroup)
{
	DECL_MGR_N(pMgr);
	return p->GetNumberOfItemsInGroupN(idGroup);
}

// Access entry information
KP_SHARE PW_ENTRY *GetEntry(void *pMgr, DWORD dwIndex)
{
	DECL_MGR_P(pMgr);
	return p->GetEntry(dwIndex);
}

KP_SHARE BOOL GetEntryStruct(void *pMgr, DWORD dwIndex, PW_ENTRY *pe)
{
	DECL_MGR_B(pMgr);

	const PW_ENTRY *peSource = p->GetEntry(dwIndex);
	if((peSource != NULL) && (pe != NULL))
	{
		*pe = *peSource;
		return TRUE;
	}

	return FALSE;
}

KP_SHARE PW_ENTRY *GetEntryByGroup(void *pMgr, DWORD idGroup, DWORD dwIndex)
{
	DECL_MGR_P(pMgr);
	return p->GetEntryByGroup(idGroup, dwIndex);
}

KP_SHARE DWORD GetEntryByGroupN(void *pMgr, DWORD idGroup, DWORD dwIndex)
{
	DECL_MGR_N(pMgr);
	return p->GetEntryByGroupN(idGroup, dwIndex);
}

KP_SHARE PW_ENTRY *GetEntryByUuid(void *pMgr, const BYTE *pUuid)
{
	DECL_MGR_P(pMgr);
	return p->GetEntryByUuid(pUuid);
}

// Returns the index of the item with pUuid
KP_SHARE DWORD GetEntryByUuidN(void *pMgr, const BYTE *pUuid)
{
	DECL_MGR_N(pMgr);
	return p->GetEntryByUuidN(pUuid);
}

KP_SHARE DWORD GetEntryPosInGroup(void *pMgr, const PW_ENTRY *pEntry)
{
	DECL_MGR_N(pMgr);
	return p->GetEntryPosInGroup(pEntry);
}

KP_SHARE PW_ENTRY *GetLastEditedEntry(void *pMgr)
{
	DECL_MGR_P(pMgr);
	return p->GetLastEditedEntry();
}

// Access group information
KP_SHARE PW_GROUP *GetGroup(void *pMgr, DWORD dwIndex)
{
	DECL_MGR_P(pMgr);
	return p->GetGroup(dwIndex);
}

KP_SHARE BOOL GetGroupStruct(void *pMgr, DWORD dwIndex, PW_GROUP *pGroup)
{
	DECL_MGR_B(pMgr);

	PW_GROUP *pg = p->GetGroup(dwIndex);
	if((pg != NULL) && (pGroup != NULL))
	{
		*pGroup = *pg;
		return TRUE;
	}

	return FALSE;
}

KP_SHARE PW_GROUP *GetGroupById(void *pMgr, DWORD idGroup)
{
	DECL_MGR_P(pMgr);
	return p->GetGroupById(idGroup);
}

KP_SHARE DWORD GetGroupByIdN(void *pMgr, DWORD idGroup)
{
	DECL_MGR_N(pMgr);
	return p->GetGroupByIdN(idGroup);
}

KP_SHARE DWORD GetGroupId(void *pMgr, const TCHAR *pszGroupName)
{
	DECL_MGR_N(pMgr);
	return p->GetGroupId(pszGroupName);
}

KP_SHARE DWORD GetGroupIdByIndex(void *pMgr, DWORD uGroupIndex)
{
	DECL_MGR_N(pMgr);
	return p->GetGroupIdByIndex(uGroupIndex);
}

KP_SHARE DWORD GetLastChildGroup(void *pMgr, DWORD dwParentGroupIndex)
{
	DECL_MGR_N(pMgr);
	return p->GetLastChildGroup(dwParentGroupIndex);
}

KP_SHARE BOOL GetGroupTree(void *pMgr, DWORD idGroup, DWORD *pGroupIndexes)
{
	DECL_MGR_B(pMgr);
	return p->GetGroupTree(idGroup, pGroupIndexes);
}

// Add entries and groups
KP_SHARE BOOL AddGroup(void *pMgr, const PW_GROUP *pTemplate)
{
	DECL_MGR_B(pMgr);
	return p->AddGroup(pTemplate);
}

KP_SHARE BOOL AddEntry(void *pMgr, const PW_ENTRY *pTemplate)
{
	DECL_MGR_B(pMgr);
	return p->AddEntry(pTemplate);
}

// pe must be unlocked already, pbGroupCreated is optional
KP_SHARE BOOL BackupEntry(void *pMgr, const PW_ENTRY *pe, BOOL *pbGroupCreated)
{
	DECL_MGR_B(pMgr);
	return p->BackupEntry(pe, pbGroupCreated);
}

// Delete entries and groups
KP_SHARE BOOL DeleteEntry(void *pMgr, DWORD dwIndex)
{
	DECL_MGR_B(pMgr);
	return p->DeleteEntry(dwIndex);
}

KP_SHARE BOOL DeleteGroupById(void *pMgr, DWORD uGroupId)
{
	DECL_MGR_B(pMgr);
	return p->DeleteGroupById(uGroupId);
}

KP_SHARE BOOL SetGroup(void *pMgr, DWORD dwIndex, const PW_GROUP *pTemplate)
{
	DECL_MGR_B(pMgr);
	return p->SetGroup(dwIndex, pTemplate);
}

KP_SHARE BOOL SetEntry(void *pMgr, DWORD dwIndex, const PW_ENTRY *pTemplate)
{
	DECL_MGR_B(pMgr);
	return p->SetEntry(dwIndex, pTemplate);
}

// DWORD MakeGroupTree(LPCTSTR lpTreeString, TCHAR tchSeparator);

// Use these functions to make passwords in PW_ENTRY structures readable
KP_SHARE void LockEntryPassword(void *pMgr, PW_ENTRY *pEntry)
{
	DECL_MGR_V(pMgr);
	p->LockEntryPassword(pEntry);
}

KP_SHARE void UnlockEntryPassword(void *pMgr, PW_ENTRY *pEntry)
{
	DECL_MGR_V(pMgr);
	p->UnlockEntryPassword(pEntry);
}

KP_SHARE void NewDatabase(void *pMgr)
{
	DECL_MGR_V(pMgr);
	p->NewDatabase();
}

KP_SHARE int OpenDatabase(void *pMgr, const TCHAR *pszFile, PWDB_REPAIR_INFO *pRepair)
{
	DECL_MGR_N(pMgr);
	return p->OpenDatabase(pszFile, pRepair);
}

KP_SHARE int SaveDatabase(void *pMgr, const TCHAR *pszFile)
{
	DECL_MGR_N(pMgr);
	return p->SaveDatabase(pszFile);
}

// Move entries and groups
KP_SHARE void MoveInternal(void *pMgr, DWORD dwFrom, DWORD dwTo)
{
	DECL_MGR_V(pMgr);
	p->MoveInternal(dwFrom, dwTo);
}

KP_SHARE void MoveInGroup(void *pMgr, DWORD idGroup, DWORD dwFrom, DWORD dwTo)
{
	DECL_MGR_V(pMgr);
	p->MoveInGroup(idGroup, dwFrom, dwTo);
}

KP_SHARE BOOL MoveGroup(void *pMgr, DWORD dwFrom, DWORD dwTo)
{
	DECL_MGR_B(pMgr);
	return p->MoveGroup(dwFrom, dwTo);
}

// Sort entry and group lists
KP_SHARE void SortGroup(void *pMgr, DWORD idGroup, DWORD dwSortByField)
{
	DECL_MGR_V(pMgr);
	p->SortGroup(idGroup, dwSortByField);
}

KP_SHARE void SortGroupList(void *pMgr)
{
	DECL_MGR_V(pMgr);
	p->SortGroupList();
}

KP_SHARE BOOL MemAllocCopyEntry(const PW_ENTRY *pExisting, PW_ENTRY *pDestination)
{
	return CPwManager::MemAllocCopyEntry(pExisting, pDestination);
}

KP_SHARE void MemFreeEntry(PW_ENTRY *pEntry)
{
	CPwManager::MemFreeEntry(pEntry);
}

KP_SHARE void MergeIn(void *pMgr, VPA_MODIFY CPwManager *pDataSource, BOOL bCreateNewUUIDs, BOOL bCompareTimes)
{
	DECL_MGR_V(pMgr);
	p->MergeIn(pDataSource, bCreateNewUUIDs, bCompareTimes);
}

// Find an item
KP_SHARE DWORD Find(void *pMgr, const TCHAR *pszFindString, BOOL bCaseSensitive, DWORD fieldFlags, DWORD nStart)
{
	DECL_MGR_N(pMgr);
	return p->Find(pszFindString, bCaseSensitive, fieldFlags, nStart);
}

// Get and set the algorithm used to encrypt the database
KP_SHARE BOOL SetAlgorithm(void *pMgr, int nAlgorithm)
{
	DECL_MGR_B(pMgr);
	return p->SetAlgorithm(nAlgorithm);
}

KP_SHARE int GetAlgorithm(void *pMgr)
{
	DECL_MGR_N(pMgr);
	return p->GetAlgorithm();
}

KP_SHARE DWORD GetKeyEncRounds(void *pMgr)
{
	DECL_MGR_N(pMgr);
	return p->GetKeyEncRounds();
}

KP_SHARE void SetKeyEncRounds(void *pMgr, DWORD dwRounds)
{
	DECL_MGR_V(pMgr);
	p->SetKeyEncRounds(dwRounds);
}

// Convert PW_TIME to 5-byte compressed structure and the other way round
KP_SHARE void TimeToPwTime(const BYTE *pCompressedTime, PW_TIME *pPwTime)
{
	CPwManager::TimeToPwTime(pCompressedTime, pPwTime);
}

KP_SHARE void PwTimeToTime(const PW_TIME *pPwTime, BYTE *pCompressedTime)
{
	CPwManager::PwTimeToTime(pPwTime, pCompressedTime);
}

// Get the never-expire time
KP_SHARE void GetNeverExpireTime(PW_TIME *pPwTime)
{
	CPwManager::GetNeverExpireTime(pPwTime);
}

// Checks and corrects the group tree (level order, etc.)
KP_SHARE void FixGroupTree(void *pMgr)
{
	DECL_MGR_V(pMgr);
	p->FixGroupTree();
}

KP_SHARE int DeleteLostEntries(void *pMgr)
{
	DECL_MGR_N(pMgr);
	return p->DeleteLostEntries();
}

KP_SHARE void SubstEntryGroupIds(void *pMgr, DWORD dwExistingId, DWORD dwNewId)
{
	DECL_MGR_V(pMgr);
	p->SubstEntryGroupIds(dwExistingId, dwNewId);
}

KP_SHARE BOOL AttachFileAsBinaryData(PW_ENTRY *pEntry, const TCHAR *lpFile)
{
	return CPwManager::AttachFileAsBinaryData(pEntry, lpFile);
}

KP_SHARE BOOL SaveBinaryData(const PW_ENTRY *pEntry, const TCHAR *lpFile)
{
	return CPwManager::SaveBinaryData(pEntry, lpFile);
}

KP_SHARE BOOL RemoveBinaryData(PW_ENTRY *pEntry)
{
	return CPwManager::RemoveBinaryData(pEntry);
}

KP_SHARE BOOL IsAllowedStoreGroup(void *pMgr, LPCTSTR lpGroupName, LPCTSTR lpSearchGroupName)
{
	DECL_MGR_B(pMgr);
	return p->IsAllowedStoreGroup(lpGroupName, lpSearchGroupName);
}

KP_SHARE void GetRawMasterKey(void *pMgr, BYTE *pStorage)
{
	DECL_MGR_V(pMgr);
	p->GetRawMasterKey(pStorage);
}

KP_SHARE void SetRawMasterKey(void *pMgr, const BYTE *pNewKey)
{
	DECL_MGR_V(pMgr);
	p->SetRawMasterKey(pNewKey);
}

KP_SHARE BOOL IsZeroUUID(const BYTE *pUUID)
{
	return CPwManager::IsZeroUUID(pUUID);
}

KP_SHARE PW_GROUP *CreateGroup(void *pMgr, LPCTSTR lpName, DWORD dwImageID)
{
	DECL_MGR_P(pMgr);

	PW_GROUP pg;
	ZeroMemory(&pg, sizeof(PW_GROUP));

	pg.pszGroupName = (LPTSTR)lpName;
	_GetCurrentPwTime(&pg.tCreation);
	pg.tLastAccess = pg.tCreation;
	pg.tLastMod = pg.tCreation;
	CPwManager::GetNeverExpireTime(&pg.tExpire);
	pg.uImageId = dwImageID;

	if(p->AddGroup(&pg) == FALSE) return NULL;

	DWORD dwNewGroupIndex = p->GetNumberOfGroups() - 1;
	return p->GetGroup(dwNewGroupIndex);
}

KP_SHARE PW_ENTRY *CreateEntry(void *pMgr, DWORD dwGroupID, LPCTSTR lpTitle, LPCTSTR lpUserName, LPCTSTR lpURL, LPCTSTR lpPassword, LPCTSTR lpNotes)
{
	DECL_MGR_P(pMgr);

	PW_ENTRY pe;
	ZeroMemory(&pe, sizeof(PW_ENTRY));

	pe.pszAdditional = (LPTSTR)lpNotes;
	pe.pszPassword = (LPTSTR)lpPassword;
	pe.pszTitle = (LPTSTR)lpTitle;
	pe.pszURL = (LPTSTR)lpURL;
	pe.pszUserName = (LPTSTR)lpUserName;
	_GetCurrentPwTime(&pe.tCreation);
	pe.tLastAccess = pe.tCreation;
	pe.tLastMod = pe.tCreation;
	CPwManager::GetNeverExpireTime(&pe.tExpire);
	pe.uGroupId = dwGroupID;
	pe.uPasswordLen = static_cast<DWORD>((lpPassword != NULL) ?
		_tcslen(lpPassword) : 0);

	if(p->AddEntry(&pe) == FALSE) return NULL;

	DWORD dwNewEntryIndex = p->GetNumberOfEntries() - 1;
	return p->GetEntry(dwNewEntryIndex);
}
