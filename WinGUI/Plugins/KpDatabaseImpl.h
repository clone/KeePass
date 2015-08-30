/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2010 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___KPDATABASEIMPL_H___
#define ___KPDATABASEIMPL_H___

#include "../../KeePassLibCpp/SDK/KpSDK.h"

#pragma pack(1)

class CKpDatabaseImpl : public IKpDatabase
{
private:
	CKpDatabaseImpl();

public:
	static CKpDatabaseImpl& Instance();

	KP_DECL_IUNKNOWN;

	STDMETHODIMP InitPrimaryInstance();

	STDMETHODIMP_(INT) SetMasterKey(LPCTSTR lpMasterKey, BOOL bDiskDrive,
		LPCTSTR lpSecondKey);

	STDMETHODIMP_(DWORD) GetEntryCount();
	STDMETHODIMP_(DWORD) GetGroupCount();

	STDMETHODIMP_(DWORD) GetEntryCountInGroup(LPCTSTR lpGroup);
	STDMETHODIMP_(DWORD) GetEntryCountInGroupN(DWORD dwGroupId);

	STDMETHODIMP_(PW_ENTRY*) GetEntry(DWORD dwIndex);
	STDMETHODIMP_(PW_ENTRY*) GetEntryByGroup(DWORD dwGroupId, DWORD dwIndex);
	STDMETHODIMP_(DWORD) GetEntryByGroupN(DWORD dwGroupId, DWORD dwIndex);
	STDMETHODIMP_(PW_ENTRY*) GetEntryByUuid(const BYTE* pUuid);
	STDMETHODIMP_(DWORD) GetEntryByUuidN(const BYTE* pUuid);
	STDMETHODIMP_(DWORD) GetEntryPosInGroup(const PW_ENTRY* pEntry);
	STDMETHODIMP_(PW_ENTRY*) GetLastEditedEntry();

	STDMETHODIMP_(PW_GROUP*) GetGroup(DWORD dwIndex);
	STDMETHODIMP_(PW_GROUP*) GetGroupById(DWORD dwGroupId);
	STDMETHODIMP_(DWORD) GetGroupByIdN(DWORD dwGroupId);
	STDMETHODIMP_(DWORD) GetGroupId(LPCTSTR lpGroupName);
	STDMETHODIMP_(DWORD) GetGroupIdByIndex(DWORD dwIndex);
	STDMETHODIMP_(DWORD) GetLastChildGroup(DWORD dwParentIndex);
	STDMETHODIMP_(BOOL) GetGroupTree(DWORD dwGroupId, DWORD* pvIndices);

	STDMETHODIMP_(BOOL) AddGroup(const PW_GROUP* pTemplate);
	STDMETHODIMP_(BOOL) AddEntry(const PW_ENTRY* pTemplate);
	STDMETHODIMP_(BOOL) BackupEntry(const PW_ENTRY* pe, BOOL* pbGroupCreated);

	STDMETHODIMP_(BOOL) DeleteEntry(DWORD dwIndex);
	STDMETHODIMP_(BOOL) DeleteGroupById(DWORD dwGroupId, BOOL bCreateBackupEntries);

	STDMETHODIMP_(BOOL) SetGroup(DWORD dwIndex, const PW_GROUP* pTemplate);
	STDMETHODIMP_(BOOL) SetEntry(DWORD dwIndex, const PW_ENTRY* pTemplate);

	STDMETHODIMP LockEntryPassword(PW_ENTRY* pEntry);
	STDMETHODIMP UnlockEntryPassword(PW_ENTRY* pEntry);

	STDMETHODIMP NewDatabase();
	STDMETHODIMP_(INT) OpenDatabase(LPCTSTR lpFile, PWDB_REPAIR_INFO* pRepair);
	STDMETHODIMP_(INT) SaveDatabase(LPCTSTR lpFile, BYTE* pWrittenDataHash32);

	STDMETHODIMP MoveEntry(DWORD dwGroupId, DWORD dwFrom, DWORD dwTo);
	STDMETHODIMP_(BOOL) MoveGroup(DWORD dwFrom, DWORD dwTo);
	STDMETHODIMP_(BOOL) MoveGroupEx(DWORD dwFromId, DWORD dwToId);
	STDMETHODIMP_(BOOL) MoveGroupExDir(DWORD dwGroupId, INT iDirection);

	STDMETHODIMP SortGroup(DWORD dwGroupId, DWORD dwSortByField);
	STDMETHODIMP SortGroupList();

	STDMETHODIMP_(DWORD) Find(LPCTSTR lpFindString, BOOL bCaseSensitive,
		DWORD dwSearchFlags, DWORD dwStart);

	STDMETHODIMP_(BOOL) SetEncryptionAlgorithm(INT nAlgorithm);
	STDMETHODIMP_(INT) GetEncryptionAlgorithm();

	STDMETHODIMP_(DWORD) GetKeyEncRounds();
	STDMETHODIMP SetKeyEncRounds(DWORD dwRounds);

	STDMETHODIMP_(const PW_DBHEADER*) GetLastDatabaseHeader();

	STDMETHODIMP FixGroupTree();

	STDMETHODIMP SubstEntryGroupIds(DWORD dwExistingId, DWORD dwNewId);

	STDMETHODIMP GetRawMasterKey(BYTE* pStorage);
	STDMETHODIMP SetRawMasterKey(const BYTE* pNewKey);

	STDMETHODIMP GetPropertyString(DWORD dwPropertyId, LPTSTR lpBuf,
		DWORD dwBufMaxChars);
	STDMETHODIMP_(BOOL) SetPropertyString(DWORD dwPropertyId, LPCTSTR lpValue);

	STDMETHODIMP_(BOOL) SetCustomKvp(LPCTSTR lpKey, LPCTSTR lpValue);
	STDMETHODIMP_(LPCTSTR) GetCustomKvp(LPCTSTR lpKey);

	STDMETHODIMP_(LPCTSTR) GetBackupGroupName(BOOL bTranslated);

private:
	KP_DECL_STDREFIMPL;
};

#pragma pack()

void KPImplInternal_SetDatabase(void* pDatabase);

#endif // ___KPDATABASEIMPL_H___
