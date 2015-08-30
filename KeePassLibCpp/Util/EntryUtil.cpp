/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2009 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "EntryUtil.h"

/*
// The entry must be unlocked already!
LPTSTR PwEntryToString(const PW_ENTRY *lpEntry)
{
	ASSERT(lpEntry != NULL); if(lpEntry == NULL) return NULL;

	CString str, strTemp;

	strTemp.Format(_T("%u"), lpEntry->uGroupId);
	str += strTemp + _T("@");

	strTemp.Format(_T("%u"), lpEntry->uImageId);
	str += strTemp + _T("@");

	strTemp.Format(_T("%u"), _tcslen(lpEntry->pszTitle));
	str = strTemp + _T("@");
	str += lpEntry->pszTitle;

	strTemp.Format(_T("%u"), _tcslen(lpEntry->pszUserName));
	str += strTemp + _T("@");
	str += lpEntry->pszUserName;

	strTemp.Format(_T("%u"), _tcslen(lpEntry->pszURL));
	str += strTemp + _T("@");
	str += lpEntry->pszURL;

	ASSERT(lpEntry->uPasswordLen == _tcslen(lpEntry->pszPassword));
	strTemp.Format(_T("%u"), _tcslen(lpEntry->pszPassword));
	str += strTemp + _T("@");
	str += lpEntry->pszPassword;

	strTemp.Format(_T("%u"), _tcslen(lpEntry->pszAdditional));
	str += strTemp + _T("@");
	str += lpEntry->pszAdditional;

	LPTSTR lpRetBuf = new TCHAR[str.GetLength() + 1];
	_tcscpy_s(lpRetBuf, str.GetLength() + 1, (LPCTSTR)str);
	return lpRetBuf;
}

// The returned entry contains the password in plain-text
BOOL StringToPwEntry(PW_ENTRY *pEntry, LPCTSTR lpEntryString)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return FALSE;
	ASSERT(lpEntryString != NULL); if(lpEntryString == NULL) return FALSE;

	CString strTitle, strUserName, strURL, strPassword, strNotes, str;
	DWORD pos = 0;
	TCHAR tch;

	while(1)
	{
		tch = lpEntryString[pos];
		pos++;

		if(tch == 0) return FALSE;
		else if(tch == _T('@')) break;
		else str += tch;
	}
	pEntry->uGroupId = _tcstoul((LPCTSTR)str, NULL, 10);
	str.Empty();

	while(1)
	{
		tch = lpEntryString[pos];
		pos++;

		if(tch == 0) return FALSE;
		else if(tch == _T('@')) break;
		else str += tch;
	}
	pEntry->uImageId = _tcstoul((LPCTSTR)str, NULL, 10);
	str.Empty();

	if(ReadPwStringItem(lpEntryString, &pos, &strTitle) == FALSE) return FALSE;
	if(ReadPwStringItem(lpEntryString, &pos, &strUserName) == FALSE) return FALSE;
	if(ReadPwStringItem(lpEntryString, &pos, &strURL) == FALSE) return FALSE;
	if(ReadPwStringItem(lpEntryString, &pos, &strPassword) == FALSE) return FALSE;
	if(ReadPwStringItem(lpEntryString, &pos, &strNotes) == FALSE) return FALSE;
	ASSERT(lpEntryString[pos] == 0);

	pEntry->pszTitle = _TcsSafeDupAlloc((LPCTSTR)strTitle);
	pEntry->pszUserName = _TcsSafeDupAlloc((LPCTSTR)strUserName);
	pEntry->pszURL = _TcsSafeDupAlloc((LPCTSTR)strURL);
	pEntry->pszPassword = _TcsCryptDupAlloc((LPCTSTR)strPassword);
	pEntry->pszAdditional = _TcsSafeDupAlloc((LPCTSTR)strNotes);
	return TRUE;
}

BOOL ReadPwStringItem(LPCTSTR lpEntryString, DWORD *pdwPos, CString *pStore)
{
	ASSERT(lpEntryString != NULL); if(lpEntryString == NULL) return FALSE;
	ASSERT(pdwPos != NULL); if(pdwPos == NULL) return FALSE;
	ASSERT(pStore != NULL); if(pStore == NULL) return FALSE;

	DWORD i, cnt, pos = *pdwPos;
	TCHAR tch;
	CString str;

	pStore->Empty();

	while(1)
	{
		tch = lpEntryString[pos];
		pos++;

		if(tch == 0) return FALSE;
		else if(tch == _T('@')) break;
		else str += tch;
	}
	cnt = _tcstoul((LPCTSTR)str, NULL, 10);

	for(i = 0; i < cnt; i++)
	{
		tch = lpEntryString[pos];
		if(tch == 0) return FALSE;
		*pStore += tch;
		pos++;
	}

	*pdwPos = pos;
	return TRUE;
}
*/

/*
int _CompareEntriesEx(void *pContext, const void *pEntryX, const void *pEntryY)
{
	if(pContext == NULL) { ASSERT(FALSE); return 0; }
	if(pEntryX == NULL) { ASSERT(FALSE); return 0; }
	if(pEntryY == NULL) { ASSERT(FALSE); return 0; }

	const CEE_CONTEXT *c = (const CEE_CONTEXT *)pContext;
	const PW_ENTRY *x = (const PW_ENTRY *)pEntryX;
	const PW_ENTRY *y = (const PW_ENTRY *)pEntryY;

	int iResult;
	switch(c->dwSortByField)
	{
	case 0:
		iResult = c->lpCompare(x->pszTitle, y->pszTitle);
		break;
	case 1:
		iResult = c->lpCompare(x->pszUserName, y->pszUserName);
		break;
	case 2:
		iResult = c->lpCompare(x->pszURL, y->pszURL);
		break;
	case 3:
		UnlockEntryPassword(x);
		UnlockEntryPassword(y);
		iResult = c->lpCompare(x->pszPassword, y->pszPassword);
		LockEntryPassword(x);
		LockEntryPassword(y);
		break;
	case 4:
		iResult = c->lpCompare(x->pszAdditional, y->pszAdditional);
		break;
	case 5:
		iResult = _pwtimecmp(&x->tCreation, &y->tCreation);
		break;
	case 6:
		iResult = _pwtimecmp(&x->tLastMod, &y->tLastMod);
		break;
	case 7:
		iResult = _pwtimecmp(&x->tLastAccess, &y->tLastAccess);
		break;
	case 8:
		iResult = _pwtimecmp(&x->tExpire, &y->tExpire);
		break;
	case 9:
		iResult = memcmp(&x->uuid[0], &y->uuid[0], 16);
		break;
	default:
		ASSERT(FALSE);
		iResult = c->lpCompare(x->pszTitle, y->pszTitle);
		break;
	}

	return iResult;
}
*/
