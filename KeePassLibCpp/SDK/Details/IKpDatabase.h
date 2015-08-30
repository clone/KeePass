/*
  Copyright (c) 2008-2011, Dominik Reichl
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
	the documentation and/or other materials provided with the
	distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef ___IKPDATABASE_H___
#define ___IKPDATABASE_H___

#pragma once

#include "../../SysDefEx.h"
#include "IKpUnknown.h"
#include "../../PwStructs.h"

#pragma pack(1)

/// Interface to a database handling object.
/// Use IKpAPI::QueryInstance to retrieve an interface to a singleton
/// object in KeePass handling database operations.
struct KP_DECL_INTERFACE("29A5C55A-7100-4A1E-B74D-986C0E085AB2") IKpDatabase :
	public IKpUnknown
{
public:
	/// Used by KeePass internally, plugins must not call this method.
	STDMETHOD(InitPrimaryInstance)() = 0;

	/// Set the master key for the database (will be used in the database
	/// loading / saving process).
	STDMETHOD_(INT, SetMasterKey)(LPCTSTR lpMasterKey, BOOL bDiskDrive,
		LPCTSTR lpSecondKey) = 0;

	STDMETHOD_(DWORD, GetEntryCount)() = 0; ///< Returns the total number of entries in database.
	STDMETHOD_(DWORD, GetGroupCount)() = 0; ///< Returns the total number of groups in database.

	/// Count entries in a group (identified by its name).
	STDMETHOD_(DWORD, GetEntryCountInGroup)(LPCTSTR lpGroup) = 0;
	/// Count entries in a group (identified by its ID).
	STDMETHOD_(DWORD, GetEntryCountInGroupN)(DWORD dwGroupId) = 0;

	// Access entry information

	/// Get an entry (by index). @kpstaticobject
	STDMETHOD_(PW_ENTRY*, GetEntry)(DWORD dwIndex) = 0;
	/// Get an entry (by specifying an index of a group). @kpstaticobject
	STDMETHOD_(PW_ENTRY*, GetEntryByGroup)(DWORD dwGroupId, DWORD dwIndex) = 0;
	STDMETHOD_(DWORD, GetEntryByGroupN)(DWORD dwGroupId, DWORD dwIndex) = 0;
	/// Get an entry (by its UUID). @kpstaticobject
	STDMETHOD_(PW_ENTRY*, GetEntryByUuid)(const BYTE* pUuid) = 0;
	STDMETHOD_(DWORD, GetEntryByUuidN)(const BYTE* pUuid) = 0; // Returns the index of the item with pUuid
	STDMETHOD_(DWORD, GetEntryPosInGroup)(const PW_ENTRY* pEntry) = 0;
	/// Get the entry that was last modified. @kpstaticobject
	STDMETHOD_(PW_ENTRY*, GetLastEditedEntry)() = 0;

	// Access group information

	/// Get a group (by index). @kpstaticobject
	STDMETHOD_(PW_GROUP*, GetGroup)(DWORD dwIndex) = 0;
	/// Get a group (by ID). @kpstaticobject
	STDMETHOD_(PW_GROUP*, GetGroupById)(DWORD dwGroupId) = 0;
	STDMETHOD_(DWORD, GetGroupByIdN)(DWORD dwGroupId) = 0;
	STDMETHOD_(DWORD, GetGroupId)(LPCTSTR lpGroupName) = 0;
	STDMETHOD_(DWORD, GetGroupIdByIndex)(DWORD dwIndex) = 0;
	STDMETHOD_(DWORD, GetLastChildGroup)(DWORD dwParentIndex) = 0;
	STDMETHOD_(BOOL, GetGroupTree)(DWORD dwGroupId, DWORD* pvIndices) = 0;

	// Add entries and groups
	STDMETHOD_(BOOL, AddGroup)(const PW_GROUP* pTemplate) = 0;
	STDMETHOD_(BOOL, AddEntry)(const PW_ENTRY* pTemplate) = 0;
	STDMETHOD_(BOOL, BackupEntry)(const PW_ENTRY* pe, BOOL* pbGroupCreated) = 0; // pe must be unlocked already

	// Delete entries and groups
	STDMETHOD_(BOOL, DeleteEntry)(DWORD dwIndex) = 0;
	STDMETHOD_(BOOL, DeleteGroupById)(DWORD dwGroupId, BOOL bCreateBackupEntries) = 0;

	STDMETHOD_(BOOL, SetGroup)(DWORD dwIndex, const PW_GROUP* pTemplate) = 0;
	STDMETHOD_(BOOL, SetEntry)(DWORD dwIndex, const PW_ENTRY* pTemplate) = 0;

	/// In-memory protection: encrypt the password of an entry.
	/// @see UnlockEntryPassword
	STDMETHOD(LockEntryPassword)(PW_ENTRY* pEntry) = 0;

	/// In-memory protection: decrypt the password of an entry.
	/// Passwords of all entries are by default encrypted. In order to make them
	/// readable, use this method. After you've finished using the password, immediately
	/// encrypt it again using the LockEntryPassword method.
	/// @see LockEntryPassword
	STDMETHOD(UnlockEntryPassword)(PW_ENTRY* pEntry) = 0;

	STDMETHOD(NewDatabase)() = 0;
	STDMETHOD_(INT, OpenDatabase)(LPCTSTR lpFile, PWDB_REPAIR_INFO* pRepair) = 0;
	STDMETHOD_(INT, SaveDatabase)(LPCTSTR lpFile, BYTE* pWrittenDataHash32) = 0;

	// Move entries and groups
	STDMETHOD(MoveEntry)(DWORD dwGroupId, DWORD dwFrom, DWORD dwTo) = 0;
	STDMETHOD_(BOOL, MoveGroup)(DWORD dwFrom, DWORD dwTo) = 0;
	STDMETHOD_(BOOL, MoveGroupEx)(DWORD dwFromId, DWORD dwToId) = 0;
	STDMETHOD_(BOOL, MoveGroupExDir)(DWORD dwGroupId, INT iDirection) = 0;

	// Sort entry and group lists
	STDMETHOD(SortGroup)(DWORD dwGroupId, DWORD dwSortByField) = 0;
	STDMETHOD(SortGroupList)() = 0;

	// Find an item
	STDMETHOD_(DWORD, Find)(LPCTSTR lpFindString, BOOL bCaseSensitive,
		DWORD dwSearchFlags, DWORD dwStart) = 0;

	// Get and set the algorithm used to encrypt the database
	STDMETHOD_(BOOL, SetEncryptionAlgorithm)(INT nAlgorithm) = 0;
	STDMETHOD_(INT, GetEncryptionAlgorithm)() = 0;

	STDMETHOD_(DWORD, GetKeyEncRounds)() = 0;
	STDMETHOD(SetKeyEncRounds)(DWORD dwRounds) = 0;

	/// Get the last used database header. @kpstaticobject
	STDMETHOD_(const PW_DBHEADER*, GetLastDatabaseHeader)() = 0;

	/// Checks and corrects the group tree (level order, etc).
	STDMETHOD(FixGroupTree)() = 0;

	STDMETHOD(SubstEntryGroupIds)(DWORD dwExistingId, DWORD dwNewId) = 0;

	STDMETHOD(GetRawMasterKey)(BYTE* pStorage) = 0;
	STDMETHOD(SetRawMasterKey)(const BYTE* pNewKey) = 0;

	STDMETHOD(GetPropertyString)(DWORD dwPropertyId, LPTSTR lpBuf, DWORD dwBufMaxChars) = 0;
	STDMETHOD_(BOOL, SetPropertyString)(DWORD dwPropertyId, LPCTSTR lpValue) = 0;

	/// Set a custom key-value pair, plugins can use this method to store
	/// own data in the database.
	STDMETHOD_(BOOL, SetCustomKvp)(LPCTSTR lpKey, LPCTSTR lpValue) = 0;
	/// Get a custom key-value pair, these are typically set by plugins to
	/// store own data in the database. @kpstaticstring
	STDMETHOD_(LPCTSTR, GetCustomKvp)(LPCTSTR lpKey) = 0;

	/// Get the name of the entry backups group. @kpstaticstring
	STDMETHOD_(LPCTSTR, GetBackupGroupName)(BOOL bTranslated) = 0;
};

#pragma pack()

#endif // ___IKPDATABASE_H___
