/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2011 Dominik Reichl <dominik.reichl@t-online.de>

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
#include <tchar.h>
#include <assert.h>
#include "KpDatabaseImpl.h"
#include "../../KeePassLibCpp/PwManager.h"
#include "../../KeePassLibCpp/Util/StrUtil.h"
#include "../../KeePassLibCpp/Util/TranslateEx.h"

static CPwManager* m_pMgr = NULL;

static LPCTSTR g_lpBackupGroupName = PWS_BACKUPGROUP_SRC;

KP_IMPL_STDREFIMPL_NODELETE(CKpDatabaseImpl)

void KPImplInternal_SetDatabase(void* pDatabase)
{
	m_pMgr = (CPwManager*)pDatabase;
}

CKpDatabaseImpl::CKpDatabaseImpl()
{
	KP_IMPL_CONSTRUCT;
}

CKpDatabaseImpl& CKpDatabaseImpl::Instance()
{
	static CKpDatabaseImpl singletonObject;
	return singletonObject;
}

STDMETHODIMP CKpDatabaseImpl::QueryInterface(REFIID riid, void** ppvObject)
{
	KP_REQ_OUT_PTR(ppvObject);

	KP_SUPPORT_INTERFACE(IID_IKpUnknown, IKpUnknown);
	KP_SUPPORT_INTERFACE(IID_IKpDatabase, IKpDatabase);

	*ppvObject = NULL;
	return E_NOINTERFACE;
}

STDMETHODIMP CKpDatabaseImpl::InitPrimaryInstance()
{
	m_pMgr->InitPrimaryInstance();
	return S_OK;
}

// Set the master key for the database
STDMETHODIMP_(INT) CKpDatabaseImpl::SetMasterKey(LPCTSTR lpMasterKey, BOOL bDiskDrive,
	LPCTSTR lpSecondKey)
{
	return m_pMgr->SetMasterKey(lpMasterKey, bDiskDrive, lpSecondKey, NULL, FALSE, NULL);
}

STDMETHODIMP_(DWORD) CKpDatabaseImpl::GetEntryCount()
{
	return m_pMgr->GetNumberOfEntries();
}

STDMETHODIMP_(DWORD) CKpDatabaseImpl::GetGroupCount()
{
	return m_pMgr->GetNumberOfGroups();
}

STDMETHODIMP_(DWORD) CKpDatabaseImpl::GetEntryCountInGroup(LPCTSTR lpGroup)
{
	return m_pMgr->GetNumberOfItemsInGroup(lpGroup);
}

STDMETHODIMP_(DWORD) CKpDatabaseImpl::GetEntryCountInGroupN(DWORD dwGroupId)
{
	return m_pMgr->GetNumberOfItemsInGroupN(dwGroupId);
}

// Access entry information
STDMETHODIMP_(PW_ENTRY*) CKpDatabaseImpl::GetEntry(DWORD dwIndex)
{
	return m_pMgr->GetEntry(dwIndex);
}

STDMETHODIMP_(PW_ENTRY*) CKpDatabaseImpl::GetEntryByGroup(DWORD dwGroupId, DWORD dwIndex)
{
	return m_pMgr->GetEntryByGroup(dwGroupId, dwIndex);
}

STDMETHODIMP_(DWORD) CKpDatabaseImpl::GetEntryByGroupN(DWORD dwGroupId, DWORD dwIndex)
{
	return m_pMgr->GetEntryByGroupN(dwGroupId, dwIndex);
}

STDMETHODIMP_(PW_ENTRY*) CKpDatabaseImpl::GetEntryByUuid(const BYTE* pUuid)
{
	return m_pMgr->GetEntryByUuid(pUuid);
}

STDMETHODIMP_(DWORD) CKpDatabaseImpl::GetEntryByUuidN(const BYTE* pUuid)
{
	return m_pMgr->GetEntryByUuidN(pUuid);
}

STDMETHODIMP_(DWORD) CKpDatabaseImpl::GetEntryPosInGroup(const PW_ENTRY* pEntry)
{
	return m_pMgr->GetEntryPosInGroup(pEntry);
}

STDMETHODIMP_(PW_ENTRY*) CKpDatabaseImpl::GetLastEditedEntry()
{
	return m_pMgr->GetLastEditedEntry();
}

// Access group information
STDMETHODIMP_(PW_GROUP*) CKpDatabaseImpl::GetGroup(DWORD dwIndex)
{
	return m_pMgr->GetGroup(dwIndex);
}

STDMETHODIMP_(PW_GROUP*) CKpDatabaseImpl::GetGroupById(DWORD dwGroupId)
{
	return m_pMgr->GetGroupById(dwGroupId);
}

STDMETHODIMP_(DWORD) CKpDatabaseImpl::GetGroupByIdN(DWORD dwGroupId)
{
	return m_pMgr->GetGroupByIdN(dwGroupId);
}

STDMETHODIMP_(DWORD) CKpDatabaseImpl::GetGroupId(LPCTSTR lpGroupName)
{
	return m_pMgr->GetGroupId(lpGroupName);
}

STDMETHODIMP_(DWORD) CKpDatabaseImpl::GetGroupIdByIndex(DWORD dwIndex)
{
	return m_pMgr->GetGroupIdByIndex(dwIndex);
}

STDMETHODIMP_(DWORD) CKpDatabaseImpl::GetLastChildGroup(DWORD dwParentIndex)
{
	return m_pMgr->GetLastChildGroup(dwParentIndex);
}

STDMETHODIMP_(BOOL) CKpDatabaseImpl::GetGroupTree(DWORD dwGroupId, DWORD* pvIndices)
{
	return m_pMgr->GetGroupTree(dwGroupId, pvIndices);
}

// Add entries and groups
STDMETHODIMP_(BOOL) CKpDatabaseImpl::AddGroup(const PW_GROUP* pTemplate)
{
	return m_pMgr->AddGroup(pTemplate);
}

STDMETHODIMP_(BOOL) CKpDatabaseImpl::AddEntry(const PW_ENTRY* pTemplate)
{
	return m_pMgr->AddEntry(pTemplate);
}

STDMETHODIMP_(BOOL) CKpDatabaseImpl::BackupEntry(const PW_ENTRY* pe, BOOL* pbGroupCreated)
{
	return m_pMgr->BackupEntry(pe, pbGroupCreated);
}

// Delete entries and groups
STDMETHODIMP_(BOOL) CKpDatabaseImpl::DeleteEntry(DWORD dwIndex)
{
	return m_pMgr->DeleteEntry(dwIndex);
}

STDMETHODIMP_(BOOL) CKpDatabaseImpl::DeleteGroupById(DWORD dwGroupId,
	BOOL bCreateBackupEntries)
{
	return m_pMgr->DeleteGroupById(dwGroupId, bCreateBackupEntries);
}

STDMETHODIMP_(BOOL) CKpDatabaseImpl::SetGroup(DWORD dwIndex, const PW_GROUP* pTemplate)
{
	return m_pMgr->SetGroup(dwIndex, pTemplate);
}

STDMETHODIMP_(BOOL) CKpDatabaseImpl::SetEntry(DWORD dwIndex, const PW_ENTRY* pTemplate)
{
	return m_pMgr->SetEntry(dwIndex, pTemplate);
}

// Use these functions to make passwords in PW_ENTRY structures readable
STDMETHODIMP CKpDatabaseImpl::LockEntryPassword(PW_ENTRY* pEntry)
{
	m_pMgr->LockEntryPassword(pEntry);
	return S_OK;
}

STDMETHODIMP CKpDatabaseImpl::UnlockEntryPassword(PW_ENTRY* pEntry)
{
	m_pMgr->UnlockEntryPassword(pEntry);
	return S_OK;
}

STDMETHODIMP CKpDatabaseImpl::NewDatabase()
{
	m_pMgr->NewDatabase();
	return S_OK;
}

STDMETHODIMP_(INT) CKpDatabaseImpl::OpenDatabase(LPCTSTR lpFile, PWDB_REPAIR_INFO* pRepair)
{
	return m_pMgr->OpenDatabase(lpFile, pRepair);
}

STDMETHODIMP_(INT) CKpDatabaseImpl::SaveDatabase(LPCTSTR lpFile, BYTE* pWrittenDataHash32)
{
	return m_pMgr->SaveDatabase(lpFile, pWrittenDataHash32);
}

// Move entries and groups
STDMETHODIMP CKpDatabaseImpl::MoveEntry(DWORD dwGroupId, DWORD dwFrom, DWORD dwTo)
{
	m_pMgr->MoveEntry(dwGroupId, dwFrom, dwTo);
	return S_OK;
}

STDMETHODIMP_(BOOL) CKpDatabaseImpl::MoveGroup(DWORD dwFrom, DWORD dwTo)
{
	return m_pMgr->MoveGroup(dwFrom, dwTo);
}

STDMETHODIMP_(BOOL) CKpDatabaseImpl::MoveGroupEx(DWORD dwFromId, DWORD dwToId)
{
	return m_pMgr->MoveGroupEx(dwFromId, dwToId);
}

STDMETHODIMP_(BOOL) CKpDatabaseImpl::MoveGroupExDir(DWORD dwGroupId, INT iDirection)
{
	return m_pMgr->MoveGroupExDir(dwGroupId, iDirection);
}

// Sort entry and group lists
STDMETHODIMP CKpDatabaseImpl::SortGroup(DWORD dwGroupId, DWORD dwSortByField)
{
	m_pMgr->SortGroup(dwGroupId, dwSortByField);
	return S_OK;
}

STDMETHODIMP CKpDatabaseImpl::SortGroupList()
{
	m_pMgr->SortGroupList();
	return S_OK;
}

// Find an item
STDMETHODIMP_(DWORD) CKpDatabaseImpl::Find(LPCTSTR lpFindString, BOOL bCaseSensitive,
	DWORD dwSearchFlags, DWORD dwStart)
{
	return m_pMgr->Find(lpFindString, bCaseSensitive, dwSearchFlags, dwStart, DWORD_MAX);
}

// Get and set the algorithm used to encrypt the database
STDMETHODIMP_(BOOL) CKpDatabaseImpl::SetEncryptionAlgorithm(INT nAlgorithm)
{
	return m_pMgr->SetAlgorithm(nAlgorithm);
}

STDMETHODIMP_(INT) CKpDatabaseImpl::GetEncryptionAlgorithm()
{
	return m_pMgr->GetAlgorithm();
}

STDMETHODIMP_(DWORD) CKpDatabaseImpl::GetKeyEncRounds()
{
	return m_pMgr->GetKeyEncRounds();
}

STDMETHODIMP CKpDatabaseImpl::SetKeyEncRounds(DWORD dwRounds)
{
	m_pMgr->SetKeyEncRounds(dwRounds);
	return S_OK;
}

STDMETHODIMP_(const PW_DBHEADER*) CKpDatabaseImpl::GetLastDatabaseHeader()
{
	return m_pMgr->GetLastDatabaseHeader();
}

// Checks and corrects the group tree (level order, etc.)
STDMETHODIMP CKpDatabaseImpl::FixGroupTree()
{
	m_pMgr->FixGroupTree();
	return S_OK;
}

STDMETHODIMP CKpDatabaseImpl::SubstEntryGroupIds(DWORD dwExistingId, DWORD dwNewId)
{
	m_pMgr->SubstEntryGroupIds(dwExistingId, dwNewId);
	return S_OK;
}

STDMETHODIMP CKpDatabaseImpl::GetRawMasterKey(BYTE* pStorage)
{
	m_pMgr->GetRawMasterKey(pStorage);
	return S_OK;
}

STDMETHODIMP CKpDatabaseImpl::SetRawMasterKey(const BYTE* pNewKey)
{
	m_pMgr->SetRawMasterKey(pNewKey);
	return S_OK;
}

STDMETHODIMP CKpDatabaseImpl::GetPropertyString(DWORD dwPropertyId, LPTSTR lpBuf,
	DWORD dwBufMaxChars)
{
	std::basic_string<TCHAR> str = m_pMgr->GetPropertyString(dwPropertyId);
	return ((_tcscpy_s(lpBuf, dwBufMaxChars, str.c_str()) == 0) ? S_OK : E_OUTOFMEMORY);
}

STDMETHODIMP_(BOOL) CKpDatabaseImpl::SetPropertyString(DWORD dwPropertyId, LPCTSTR lpValue)
{
	return m_pMgr->SetPropertyString(dwPropertyId, lpValue);
}

STDMETHODIMP_(BOOL) CKpDatabaseImpl::SetCustomKvp(LPCTSTR lpKey, LPCTSTR lpValue)
{
	return m_pMgr->SetCustomKvp(lpKey, lpValue);
}

STDMETHODIMP_(LPCTSTR) CKpDatabaseImpl::GetCustomKvp(LPCTSTR lpKey)
{
	return m_pMgr->GetCustomKvp(lpKey);
}

STDMETHODIMP_(LPCTSTR) CKpDatabaseImpl::GetBackupGroupName(BOOL bTranslated)
{
	return ((bTranslated == FALSE) ? g_lpBackupGroupName : PWS_BACKUPGROUP);
}
