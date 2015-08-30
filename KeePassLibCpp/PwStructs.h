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

#ifndef ___PW_STRUCTS_H___
#define ___PW_STRUCTS_H___

#pragma once

#include "SysDefEx.h"

#pragma pack(1)

/// Time structure.
typedef struct _PW_TIME
{
	USHORT shYear; ///< Year. 2004 means 2004.
	BYTE btMonth;  ///< Month. Ranges from 1 = Jan to 12 = Dec.
	BYTE btDay;    ///< Day. The first day is 1.
	BYTE btHour;   ///< Hour. Begins with hour 0, maximum value is 23.
	BYTE btMinute; ///< Minutes. Begins at 0, maximum value is 59.
	BYTE btSecond; ///< Seconds. Begins at 0, maximum value is 59.

#ifdef VPF_ALIGN
	BYTE btDummy;
#endif
} PW_TIME, *PPW_TIME;

/// Database header structure. All KeePass KDB files begin with this structure.
typedef struct _PW_DBHEADER
{
	DWORD dwSignature1; ///< File identifier word, set to PWM_DBSIG_1.
	DWORD dwSignature2; ///< File identifier word, set to PWM_DBSIG_2.
	DWORD dwFlags;
	DWORD dwVersion;

	BYTE aMasterSeed[16]; ///< Seed that gets hashed with the user key to form the final key.
	UINT8 aEncryptionIV[16]; ///< IV used for content encryption.

	DWORD dwGroups; ///< Number of groups in the database.
	DWORD dwEntries; ///< Number of entries in the database.

	BYTE aContentsHash[32]; ///< SHA-256 hash of the database, used for integrity checking.

	BYTE aMasterSeed2[32]; ///< Seed used for the dwKeyEncRounds AES transformations.
	DWORD dwKeyEncRounds; ///< Number of master key transformations.
} PW_DBHEADER, *PPW_DBHEADER;

/// Group structure, containing information about one group.
typedef struct _PW_GROUP
{
	DWORD uGroupId; ///< ID of the group. The ID is a unique identifier in one database.
	DWORD uImageId; ///< Index of the icon in the image list to use for this group.
	TCHAR *pszGroupName; ///< Name of the group.

	PW_TIME tCreation; ///< Time when the group was created.
	PW_TIME tLastMod; ///< Time when the group was last modified.
	PW_TIME tLastAccess; ///< Time when the group was last accessed.
	PW_TIME tExpire; ///< Time when the group will expire.

	USHORT usLevel; ///< Indentation/depth level in the group tree.

#ifdef VPF_ALIGN
	USHORT usDummy;
#endif

	DWORD dwFlags; ///< Used by KeePass internally, don't use (set to 0 for a new structure).
} PW_GROUP, *PPW_GROUP;

/// Entry structure, containing information about one entry.
typedef struct _PW_ENTRY
{
	BYTE uuid[16]; ///< Unique GUID identifying this entry (not only in this database).
	DWORD uGroupId; ///< ID of the group that contains this entry.
	DWORD uImageId; ///< Index of the icon in the image list to use for this entry.

	TCHAR *pszTitle; ///< Title.
	TCHAR *pszURL; ///< URL.
	TCHAR *pszUserName; ///< User name.

	DWORD uPasswordLen; ///< Length of the password (required for memory protection).
	TCHAR *pszPassword; ///< Password (may be encrypted, use IKpDatabase::UnlockEntryPassword to decrypt).

	TCHAR *pszAdditional; ///< Notes.

	PW_TIME tCreation; ///< Time when the entry was created.
	PW_TIME tLastMod; ///< Time when the entry was last modified.
	PW_TIME tLastAccess; ///< Time when the entry was last accessed.
	PW_TIME tExpire; ///< Time when the entry will expire.

	TCHAR *pszBinaryDesc; ///< A string describing the contents of pBinaryData.
	BYTE *pBinaryData; ///< Attachment data (of length uBinaryDataLen), may be NULL.
	DWORD uBinaryDataLen; ///< Length of the attachment data in bytes.
} PW_ENTRY, *PPW_ENTRY;

/// Structure wrapping one GUID.
typedef struct _PW_UUID_STRUCT
{
	BYTE uuid[16]; ///< A GUID.
} PW_UUID_STRUCT;

/// Structure containing information about a database repairing process.
typedef struct _PWDB_REPAIR_INFO
{
	DWORD dwOriginalGroupCount;
	DWORD dwOriginalEntryCount;
	DWORD dwRecognizedMetaStreamCount;
} PWDB_REPAIR_INFO, *PPWDB_REPAIR_INFO;

/// Structure containing information about one main menu item provided by a plugin.
typedef struct
{
	/// Flags (enabled state, checkbox, popup, etc).
	/// - If KPMIF_CHECKBOX is set, the menu item has a checkbox.
	/// - If KPMIF_DISABLED is set, the menu item is disabled (grayed out).
	/// - If KPMIF_POPUP_START is set, the menu item has a popup menu (all menu
	///   items following the current one will be shown in the popup).
	/// - If KPMIF_POPUP_END is set, the menu item is invisible and a popup
	///   menu is closed (all following menu items are shown in the parent menu).
	DWORD dwFlags; // KPMIF_XXX flags

	/// State (checkbox, etc).
	/// If KPMIS_CHECKED is set, the checkbox is checked.
	DWORD dwState; // KPMIS_XXX flags

	DWORD dwIcon;
	LPTSTR lpCommandString; ///< The menu item's text.
	DWORD dwCommandID; ///< Set by KeePass, don't modify yourself.

	DWORD_PTR dwReserved; ///< Reserved for future use, must be 0.
} KP_MENU_ITEM;

/// Structure used for entry validations by plugins.
typedef struct
{
	const void* pOriginalEntry; ///< Pointer to the original PW_ENTRY.

	BYTE uuid[16]; ///< Unique GUID identifying this entry (not only in this database).

	/// Index of the group that contains this entry.
	/// Note that the group index is different from the group ID.
	DWORD uGroupIndex;

	DWORD uImageId; ///< Index of the icon in the image list to use for this entry.

	LPCTSTR lpTitle; ///< Title.
	LPCTSTR lpURL; ///< URL.
	LPCTSTR lpUserName; ///< User name.
	LPCTSTR lpPassword; ///< Password (unencrypted).
	LPCTSTR lpAdditional; ///< Notes.

	DWORD_PTR dwReserved; ///< Reserved for future use, must be 0.
} KP_ENTRY, *PKP_ENTRY;

#pragma pack()

/////////////////////////////////////////////////////////////////////////////
// Key provider structures

#pragma pack(1)

/// Information about a key provider.
typedef struct _KP_KEYPROV_INFO
{
	DWORD dwFlags; ///< Reserved for future use, must be 0.
	LPCTSTR lpName; ///< Unique display name of the key provider.
	DWORD dwImageIndex; ///< Index of the icon shown in the combo box.

	DWORD_PTR dwReserved; ///< Reserved for future use, must be 0.
} KP_KEYPROV_INFO;

/// A key returned by a key provider.
typedef struct _KP_KEYPROV_KEY
{
	DWORD dwType; ///< Reserved for future use, must be 0.
	DWORD dwFormat; ///< Reserved for future use, must be 0.
	LPVOID lpData; ///< Key data pointer.
	DWORD dwDataSize; ///< Size of the key (lpData) in bytes.

	DWORD_PTR dwReserved; ///< Reserved for future use, must be 0.
} KP_KEYPROV_KEY;

/// Information structure used when querying keys from key providers.
typedef struct _KP_KEYPROV_CONTEXT
{
	DWORD dwSize; ///< Size of the KP_KEYPROV_CONTEXT structure.

	LPCTSTR lpProviderName; ///< Name of the provider that should generate the key.

	BOOL bCreatingNewKey; ///< Specifies whether a new key is being generated.
	BOOL bConfirming; ///< Specifies whether KeePass asks the user to confirm the key.
	BOOL bChanging;

	LPCTSTR lpDescriptiveName; ///< File name or some other descriptive string.
} KP_KEYPROV_CONTEXT;

#pragma pack()

#endif // ___PW_STRUCTS_H___
