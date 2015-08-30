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

#ifndef ___KEEPASS_STRUCT_API_H___
#define ___KEEPASS_STRUCT_API_H___

#include "APIDefEx.h"
#include "../../KeePassLibCpp/PwManager.h"

// Extract group information
KP_SHARE DWORD PG_GetID(PW_GROUP *pGroup);
KP_SHARE DWORD PG_GetImageID(PW_GROUP *pGroup);
KP_SHARE LPCTSTR PG_GetName(PW_GROUP *pGroup);
KP_SHARE const PW_TIME *PG_GetCreationTime(PW_GROUP *pGroup);
KP_SHARE const PW_TIME *PG_GetLastModTime(PW_GROUP *pGroup);
KP_SHARE const PW_TIME *PG_GetLastAccessTime(PW_GROUP *pGroup);
KP_SHARE const PW_TIME *PG_GetExpireTime(PW_GROUP *pGroup);
KP_SHARE USHORT PG_GetLevel(PW_GROUP *pGroup);

// Set group information
KP_SHARE BOOL PG_SetID(PW_GROUP *pGroup, DWORD dwID);
KP_SHARE BOOL PG_SetImageID(PW_GROUP *pGroup, DWORD dwImageID);
KP_SHARE BOOL PG_SetName(PW_GROUP *pGroup, LPCTSTR lpName);
KP_SHARE BOOL PG_SetCreationTime(PW_GROUP *pGroup, const PW_TIME *pTime);
KP_SHARE BOOL PG_SetLastModTime(PW_GROUP *pGroup, const PW_TIME *pTime);
KP_SHARE BOOL PG_SetLastAccessTime(PW_GROUP *pGroup, const PW_TIME *pTime);
KP_SHARE BOOL PG_SetExpireTime(PW_GROUP *pGroup, const PW_TIME *pTime);
KP_SHARE BOOL PG_SetLevel(PW_GROUP *pGroup, USHORT usLevel);

// Extract entry information
KP_SHARE const BYTE *PE_GetUUID(PW_ENTRY *pEntry);
KP_SHARE DWORD PE_GetGroupID(PW_ENTRY *pEntry);
KP_SHARE DWORD PE_GetImageID(PW_ENTRY *pEntry);
KP_SHARE LPCTSTR PE_GetTitle(PW_ENTRY *pEntry);
KP_SHARE LPCTSTR PE_GetURL(PW_ENTRY *pEntry);
KP_SHARE LPCTSTR PE_GetUserName(PW_ENTRY *pEntry);
KP_SHARE LPCTSTR PE_GetPasswordPtr(PW_ENTRY *pEntry);
KP_SHARE LPCTSTR PE_GetNotes(PW_ENTRY *pEntry);
KP_SHARE const PW_TIME *PE_GetCreationTime(PW_ENTRY *pEntry);
KP_SHARE const PW_TIME *PE_GetLastModTime(PW_ENTRY *pEntry);
KP_SHARE const PW_TIME *PE_GetLastAccessTime(PW_ENTRY *pEntry);
KP_SHARE const PW_TIME *PE_GetExpireTime(PW_ENTRY *pEntry);
KP_SHARE LPCTSTR PE_GetBinaryDesc(PW_ENTRY *pEntry);
KP_SHARE const BYTE *PE_GetBinaryData(PW_ENTRY *pEntry);
KP_SHARE DWORD PE_GetBinaryDataLength(PW_ENTRY *pEntry);

// Set entry information
KP_SHARE BOOL PE_SetUUID(PW_ENTRY *pEntry, const BYTE *pUUID);
KP_SHARE BOOL PE_SetGroupID(PW_ENTRY *pEntry, DWORD dwGroupID);
KP_SHARE BOOL PE_SetImageID(PW_ENTRY *pEntry, DWORD dwImageID);
KP_SHARE BOOL PE_SetTitle(PW_ENTRY *pEntry, LPCTSTR lpTitle);
KP_SHARE BOOL PE_SetURL(PW_ENTRY *pEntry, LPCTSTR lpURL);
KP_SHARE BOOL PE_SetUserName(PW_ENTRY *pEntry, LPCTSTR lpUserName);
KP_SHARE BOOL PE_SetPasswordAndLock(void *pMgr, PW_ENTRY *pEntry, LPCTSTR lpPassword);
KP_SHARE BOOL PE_SetNotes(PW_ENTRY *pEntry, LPCTSTR lpNotes);
KP_SHARE BOOL PE_SetCreationTime(PW_ENTRY *pEntry, const PW_TIME *pTime);
KP_SHARE BOOL PE_SetLastModTime(PW_ENTRY *pEntry, const PW_TIME *pTime);
KP_SHARE BOOL PE_SetLastAccessTime(PW_ENTRY *pEntry, const PW_TIME *pTime);
KP_SHARE BOOL PE_SetExpireTime(PW_ENTRY *pEntry, const PW_TIME *pTime);
KP_SHARE BOOL PE_SetBinaryDesc(PW_ENTRY *pEntry, LPCTSTR lpDesc);
KP_SHARE BOOL PE_SetBinaryData(PW_ENTRY *pEntry, const BYTE *lpData, DWORD dwDataLength);

#endif
