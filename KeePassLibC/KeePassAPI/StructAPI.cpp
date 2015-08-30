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
#include "StructAPI.h"
#include "../../KeePassLibCpp/Util/StrUtil.h"
#include "../../KeePassLibCpp/Util/MemUtil.h"

KP_SHARE DWORD PG_GetID(PW_GROUP *pGroup)
{
	ASSERT(pGroup != NULL); if(pGroup == NULL) return 0;
	return pGroup->uGroupId;
}

KP_SHARE DWORD PG_GetImageID(PW_GROUP *pGroup)
{
	ASSERT(pGroup != NULL); if(pGroup == NULL) return 0;
	return pGroup->uGroupId;
}

KP_SHARE LPCTSTR PG_GetName(PW_GROUP *pGroup)
{
	ASSERT(pGroup != NULL); if(pGroup == NULL) return NULL;
	return pGroup->pszGroupName;
}

KP_SHARE const PW_TIME *PG_GetCreationTime(PW_GROUP *pGroup)
{
	ASSERT(pGroup != NULL); if(pGroup == NULL) return NULL;
	return &pGroup->tCreation;
}

KP_SHARE const PW_TIME *PG_GetLastModTime(PW_GROUP *pGroup)
{
	ASSERT(pGroup != NULL); if(pGroup == NULL) return NULL;
	return &pGroup->tLastMod;
}

KP_SHARE const PW_TIME *PG_GetLastAccessTime(PW_GROUP *pGroup)
{
	ASSERT(pGroup != NULL); if(pGroup == NULL) return NULL;
	return &pGroup->tLastAccess;
}

KP_SHARE const PW_TIME *PG_GetExpireTime(PW_GROUP *pGroup)
{
	ASSERT(pGroup != NULL); if(pGroup == NULL) return NULL;
	return &pGroup->tExpire;
}

KP_SHARE USHORT PG_GetLevel(PW_GROUP *pGroup)
{
	ASSERT(pGroup != NULL); if(pGroup == NULL) return 0;
	return pGroup->usLevel;
}

KP_SHARE BOOL PG_SetID(PW_GROUP *pGroup, DWORD dwID)
{
	ASSERT(pGroup != NULL); if(pGroup == NULL) return FALSE;
	pGroup->uGroupId = dwID;
	return TRUE;
}

KP_SHARE BOOL PG_SetImageID(PW_GROUP *pGroup, DWORD dwImageID)
{
	ASSERT(pGroup != NULL); if(pGroup == NULL) return FALSE;
	pGroup->uImageId = dwImageID;
	return TRUE;
}

KP_SHARE BOOL PG_SetName(PW_GROUP *pGroup, LPCTSTR lpName)
{
	ASSERT(pGroup != NULL); if(pGroup == NULL) return FALSE;
	SAFE_DELETE_ARRAY(pGroup->pszGroupName);
	pGroup->pszGroupName = _TcsSafeDupAlloc(lpName);
	return TRUE;
}

KP_SHARE BOOL PG_SetCreationTime(PW_GROUP *pGroup, const PW_TIME *pTime)
{
	ASSERT(pGroup != NULL); if(pGroup == NULL) return FALSE;
	ASSERT(pTime != NULL); if(pTime == NULL) return FALSE;
	pGroup->tCreation = *pTime;
	return TRUE;
}

KP_SHARE BOOL PG_SetLastModTime(PW_GROUP *pGroup, const PW_TIME *pTime)
{
	ASSERT(pGroup != NULL); if(pGroup == NULL) return FALSE;
	ASSERT(pTime != NULL); if(pTime == NULL) return FALSE;
	pGroup->tLastMod = *pTime;
	return TRUE;
}

KP_SHARE BOOL PG_SetLastAccessTime(PW_GROUP *pGroup, const PW_TIME *pTime)
{
	ASSERT(pGroup != NULL); if(pGroup == NULL) return FALSE;
	ASSERT(pTime != NULL); if(pTime == NULL) return FALSE;
	pGroup->tLastAccess = *pTime;
	return TRUE;
}

KP_SHARE BOOL PG_SetExpireTime(PW_GROUP *pGroup, const PW_TIME *pTime)
{
	ASSERT(pGroup != NULL); if(pGroup == NULL) return FALSE;
	ASSERT(pTime != NULL); if(pTime == NULL) return FALSE;
	pGroup->tExpire = *pTime;
	return TRUE;
}

KP_SHARE BOOL PG_SetLevel(PW_GROUP *pGroup, USHORT usLevel)
{
	ASSERT(pGroup != NULL); if(pGroup == NULL) return FALSE;
	pGroup->usLevel = usLevel;
	return TRUE;
}

KP_SHARE const BYTE *PE_GetUUID(PW_ENTRY *pEntry)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return FALSE;
	return pEntry->uuid;
}

KP_SHARE DWORD PE_GetGroupID(PW_ENTRY *pEntry)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return 0;
	return pEntry->uGroupId;
}

KP_SHARE DWORD PE_GetImageID(PW_ENTRY *pEntry)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return 0;
	return pEntry->uImageId;
}

KP_SHARE LPCTSTR PE_GetTitle(PW_ENTRY *pEntry)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return NULL;
	return pEntry->pszTitle;
}

KP_SHARE LPCTSTR PE_GetURL(PW_ENTRY *pEntry)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return NULL;
	return pEntry->pszURL;
}

KP_SHARE LPCTSTR PE_GetUserName(PW_ENTRY *pEntry)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return NULL;
	return pEntry->pszUserName;
}

KP_SHARE LPCTSTR PE_GetPasswordPtr(PW_ENTRY *pEntry)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return NULL;
	return pEntry->pszPassword;
}

KP_SHARE LPCTSTR PE_GetNotes(PW_ENTRY *pEntry)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return NULL;
	return pEntry->pszAdditional;
}

KP_SHARE const PW_TIME *PE_GetCreationTime(PW_ENTRY *pEntry)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return NULL;
	return &pEntry->tCreation;
}

KP_SHARE const PW_TIME *PE_GetLastModTime(PW_ENTRY *pEntry)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return NULL;
	return &pEntry->tLastMod;
}

KP_SHARE const PW_TIME *PE_GetLastAccessTime(PW_ENTRY *pEntry)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return NULL;
	return &pEntry->tLastAccess;
}

KP_SHARE const PW_TIME *PE_GetExpireTime(PW_ENTRY *pEntry)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return NULL;
	return &pEntry->tExpire;
}

KP_SHARE LPCTSTR PE_GetBinaryDesc(PW_ENTRY *pEntry)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return NULL;
	return pEntry->pszBinaryDesc;
}

KP_SHARE const BYTE *PE_GetBinaryData(PW_ENTRY *pEntry)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return NULL;
	return pEntry->pBinaryData;

}

KP_SHARE DWORD PE_GetBinaryDataLength(PW_ENTRY *pEntry)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return 0;
	return pEntry->uBinaryDataLen;
}

KP_SHARE BOOL PE_SetUUID(PW_ENTRY *pEntry, const BYTE *pUUID)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return FALSE;
	ASSERT(pUUID != NULL); if(pUUID == NULL) return FALSE;
	memcpy(pEntry->uuid, pUUID, 16);
	return TRUE;
}

KP_SHARE BOOL PE_SetGroupID(PW_ENTRY *pEntry, DWORD dwGroupID)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return FALSE;
	pEntry->uGroupId = dwGroupID;
	return TRUE;
}

KP_SHARE BOOL PE_SetImageID(PW_ENTRY *pEntry, DWORD dwImageID)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return FALSE;
	pEntry->uImageId = dwImageID;
	return TRUE;
}

KP_SHARE BOOL PE_SetTitle(PW_ENTRY *pEntry, LPCTSTR lpTitle)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return FALSE;
	SAFE_DELETE_ARRAY(pEntry->pszTitle);
	pEntry->pszTitle = _TcsSafeDupAlloc(lpTitle);
	return TRUE;
}

KP_SHARE BOOL PE_SetURL(PW_ENTRY *pEntry, LPCTSTR lpURL)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return FALSE;
	SAFE_DELETE_ARRAY(pEntry->pszURL);
	pEntry->pszURL = _TcsSafeDupAlloc(lpURL);
	return TRUE;
}

KP_SHARE BOOL PE_SetUserName(PW_ENTRY *pEntry, LPCTSTR lpUserName)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return FALSE;
	SAFE_DELETE_ARRAY(pEntry->pszUserName);
	pEntry->pszUserName = _TcsSafeDupAlloc(lpUserName);
	return TRUE;
}

KP_SHARE BOOL PE_SetPasswordAndLock(void *pMgr, PW_ENTRY *pEntry, LPCTSTR lpPassword)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return FALSE;
	CPwManager *p = (CPwManager *)pMgr;
	ASSERT(p != NULL); if(p == NULL) return FALSE;

	p->UnlockEntryPassword(pEntry);

	SAFE_DELETE_ARRAY(pEntry->pszPassword);
	pEntry->pszPassword = _TcsCryptDupAlloc(lpPassword);

	p->LockEntryPassword(pEntry);
	return TRUE;
}

KP_SHARE BOOL PE_SetNotes(PW_ENTRY *pEntry, LPCTSTR lpNotes)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return FALSE;
	SAFE_DELETE_ARRAY(pEntry->pszAdditional);
	pEntry->pszAdditional = _TcsSafeDupAlloc(lpNotes);
	return TRUE;
}

KP_SHARE BOOL PE_SetCreationTime(PW_ENTRY *pEntry, const PW_TIME *pTime)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return FALSE;
	pEntry->tCreation = *pTime;
	return TRUE;
}

KP_SHARE BOOL PE_SetLastModTime(PW_ENTRY *pEntry, const PW_TIME *pTime)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return FALSE;
	pEntry->tLastMod = *pTime;
	return TRUE;
}

KP_SHARE BOOL PE_SetLastAccessTime(PW_ENTRY *pEntry, const PW_TIME *pTime)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return FALSE;
	pEntry->tLastAccess = *pTime;
	return TRUE;
}

KP_SHARE BOOL PE_SetExpireTime(PW_ENTRY *pEntry, const PW_TIME *pTime)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return FALSE;
	pEntry->tExpire = *pTime;
	return TRUE;
}

KP_SHARE BOOL PE_SetBinaryDesc(PW_ENTRY *pEntry, LPCTSTR lpDesc)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return FALSE;
	SAFE_DELETE_ARRAY(pEntry->pszBinaryDesc);
	pEntry->pszBinaryDesc = _TcsSafeDupAlloc(lpDesc);
	return TRUE;
}

KP_SHARE BOOL PE_SetBinaryData(PW_ENTRY *pEntry, const BYTE *lpData, DWORD dwDataLength)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return FALSE;

	SAFE_DELETE_ARRAY(pEntry->pBinaryData);
	pEntry->uBinaryDataLen = dwDataLength;

	if((lpData != NULL) && (dwDataLength != 0))
	{
		pEntry->pBinaryData = new BYTE[dwDataLength];
		ASSERT(pEntry->pBinaryData != NULL); if(pEntry->pBinaryData == NULL) return FALSE;

		memcpy(pEntry->pBinaryData, lpData, dwDataLength);
	}

	return TRUE;
}
