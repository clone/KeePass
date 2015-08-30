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

#ifndef ___KEEPASS_API_H___
#define ___KEEPASS_API_H___

#include "../../KeePassLibCpp/PwManager.h"
#include "APIDefEx.h"

KP_SHARE void InitManager(void **pMgr, BOOL bIsFirstInstance);
KP_SHARE void DeleteManager(void *pMgr);

KP_SHARE INT SetMasterKey(void *pMgr, LPCTSTR pszMasterKey, BOOL bDiskDrive, LPCTSTR pszSecondKey, const CNewRandomInterface *pARI, BOOL bOverwrite);

KP_SHARE DWORD GetNumberOfEntries(void *pMgr); // Returns number of entries in database
KP_SHARE DWORD GetNumberOfGroups(void *pMgr); // Returns number of groups in database

// Count items in groups
KP_SHARE DWORD GetNumberOfItemsInGroup(void *pMgr, LPCTSTR pszGroup);
KP_SHARE DWORD GetNumberOfItemsInGroupN(void *pMgr, DWORD idGroup);

// Access entry information
KP_SHARE PW_ENTRY *GetEntry(void *pMgr, DWORD dwIndex);
KP_SHARE BOOL GetEntryStruct(void *pMgr, DWORD dwIndex, PW_ENTRY *pe);
KP_SHARE PW_ENTRY *GetEntryByGroup(void *pMgr, DWORD idGroup, DWORD dwIndex);
KP_SHARE DWORD GetEntryByGroupN(void *pMgr, DWORD idGroup, DWORD dwIndex);
KP_SHARE PW_ENTRY *GetEntryByUuid(void *pMgr, const BYTE *pUuid);
KP_SHARE DWORD GetEntryByUuidN(void *pMgr, const BYTE *pUuid); // Returns the index of the item with pUuid
KP_SHARE DWORD GetEntryPosInGroup(void *pMgr, const PW_ENTRY *pEntry);
KP_SHARE PW_ENTRY *GetLastEditedEntry(void *pMgr);

// Access group information
KP_SHARE PW_GROUP *GetGroup(void *pMgr, DWORD dwIndex);
KP_SHARE BOOL GetGroupStruct(void *pMgr, DWORD dwIndex, PW_GROUP *pGroup);
KP_SHARE PW_GROUP *GetGroupById(void *pMgr, DWORD idGroup);
KP_SHARE DWORD GetGroupByIdN(void *pMgr, DWORD idGroup);
KP_SHARE DWORD GetGroupId(void *pMgr, const TCHAR *pszGroupName);
KP_SHARE DWORD GetGroupIdByIndex(void *pMgr, DWORD uGroupIndex);
KP_SHARE DWORD GetLastChildGroup(void *pMgr, DWORD dwParentGroupIndex);
KP_SHARE BOOL GetGroupTree(void *pMgr, DWORD idGroup, DWORD *pGroupIndexes);

// Add entries and groups
KP_SHARE BOOL AddGroup(void *pMgr, const PW_GROUP *pTemplate);
KP_SHARE BOOL AddEntry(void *pMgr, const PW_ENTRY *pTemplate);
KP_SHARE BOOL BackupEntry(void *pMgr, const PW_ENTRY *pe, BOOL *pbGroupCreated); // pe must be unlocked already, pbGroupCreated is optional

// Delete entries and groups
KP_SHARE BOOL DeleteEntry(void *pMgr, DWORD dwIndex);
KP_SHARE BOOL DeleteGroupById(void *pMgr, DWORD uGroupId);

KP_SHARE BOOL SetGroup(void *pMgr, DWORD dwIndex, const PW_GROUP *pTemplate);
KP_SHARE BOOL SetEntry(void *pMgr, DWORD dwIndex, const PW_ENTRY *pTemplate);
// DWORD MakeGroupTree(LPCTSTR lpTreeString, TCHAR tchSeparator);

// Use these functions to make passwords in PW_ENTRY structures readable
KP_SHARE void LockEntryPassword(void *pMgr, PW_ENTRY *pEntry); // Lock password, encrypt it
KP_SHARE void UnlockEntryPassword(void *pMgr, PW_ENTRY *pEntry); // Make password readable

KP_SHARE void NewDatabase(void *pMgr);
KP_SHARE int OpenDatabase(void *pMgr, const TCHAR *pszFile, PWDB_REPAIR_INFO *pRepair);
KP_SHARE int SaveDatabase(void *pMgr, const TCHAR *pszFile);

// Move entries and groups
KP_SHARE void MoveInternal(void *pMgr, DWORD dwFrom, DWORD dwTo);
KP_SHARE void MoveInGroup(void *pMgr, DWORD idGroup, DWORD dwFrom, DWORD dwTo);
KP_SHARE BOOL MoveGroup(void *pMgr, DWORD dwFrom, DWORD dwTo);

// Sort entry and group lists
KP_SHARE void SortGroup(void *pMgr, DWORD idGroup, DWORD dwSortByField);
KP_SHARE void SortGroupList(void *pMgr);

KP_SHARE BOOL MemAllocCopyEntry(const PW_ENTRY *pExisting, PW_ENTRY *pDestination);
KP_SHARE void MemFreeEntry(PW_ENTRY *pEntry);

KP_SHARE void MergeIn(void *pMgr, VPA_MODIFY CPwManager *pDataSource, BOOL bCreateNewUUIDs, BOOL bCompareTimes);

// Find an item
KP_SHARE DWORD Find(void *pMgr, const TCHAR *pszFindString, BOOL bCaseSensitive, DWORD fieldFlags, DWORD nStart);

// Get and set the algorithm used to encrypt the database
KP_SHARE BOOL SetAlgorithm(void *pMgr, int nAlgorithm);
KP_SHARE int GetAlgorithm(void *pMgr);

KP_SHARE DWORD GetKeyEncRounds(void *pMgr);
KP_SHARE void SetKeyEncRounds(void *pMgr, DWORD dwRounds);

// Convert PW_TIME to 5-byte compressed structure and the other way round
KP_SHARE void TimeToPwTime(const BYTE *pCompressedTime, PW_TIME *pPwTime);
KP_SHARE void PwTimeToTime(const PW_TIME *pPwTime, BYTE *pCompressedTime);

// Get the never-expire time
KP_SHARE void GetNeverExpireTime(PW_TIME *pPwTime);

// Checks and corrects the group tree (level order, etc.)
KP_SHARE void FixGroupTree(void *pMgr);
KP_SHARE int DeleteLostEntries(void *pMgr);

KP_SHARE void SubstEntryGroupIds(void *pMgr, DWORD dwExistingId, DWORD dwNewId);

KP_SHARE BOOL AttachFileAsBinaryData(PW_ENTRY *pEntry, const TCHAR *lpFile);
KP_SHARE BOOL SaveBinaryData(const PW_ENTRY *pEntry, const TCHAR *lpFile);
KP_SHARE BOOL RemoveBinaryData(PW_ENTRY *pEntry);

KP_SHARE BOOL IsAllowedStoreGroup(void *pMgr, LPCTSTR lpGroupName, LPCTSTR lpSearchGroupName);

KP_SHARE void GetRawMasterKey(void *pMgr, BYTE *pStorage);
KP_SHARE void SetRawMasterKey(void *pMgr, const BYTE *pNewKey);

KP_SHARE BOOL IsZeroUUID(const BYTE *pUUID);

KP_SHARE PW_GROUP *CreateGroup(void *pMgr, LPCTSTR lpName, DWORD dwImageID);
KP_SHARE PW_ENTRY *CreateEntry(void *pMgr, DWORD dwGroupID, LPCTSTR lpTitle, LPCTSTR lpUserName, LPCTSTR lpURL, LPCTSTR lpPassword, LPCTSTR lpNotes);

#endif
