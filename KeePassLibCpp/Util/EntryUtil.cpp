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
#include "EntryUtil.h"
#include "PwUtil.h"
#include "TranslateEx.h"
#include <boost/lexical_cast.hpp>
// #include "Base64.h"
// #include "../SDK/Details/KpDefs.h"

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

/*
#ifdef _MFC_VER
void ArStorePwTime(CArchive& ar, const PW_TIME& rTime)
{
	ar << rTime.shYear;
	ar << rTime.btMonth;
	ar << rTime.btDay;
	ar << rTime.btHour;
	ar << rTime.btMinute;
	ar << rTime.btSecond;
}

void ArLoadPwTime(CArchive& ar, PW_TIME& rTime)
{
	ar >> rTime.shYear;
	ar >> rTime.btMonth;
	ar >> rTime.btDay;
	ar >> rTime.btHour;
	ar >> rTime.btMinute;
	ar >> rTime.btSecond;
}

CString PwEntryToString(PW_ENTRY *lpEntry, CPwManager* lpContext)
{
	if(lpEntry == NULL) { ASSERT(FALSE); return CString(); }
	if(lpContext == NULL) { ASSERT(FALSE); return CString(); }

	CMemFile memFile;
	CArchive ar(&memFile, CArchive::store);

	CString strUuid;
	_UuidToString(lpEntry->uuid, &strUuid);
	ar << strUuid;

	ar << lpEntry->uGroupId;
	ar << lpEntry->uImageId;
	
	ar << lpEntry->pszTitle;
	ar << lpEntry->pszURL;
	ar << lpEntry->pszUserName;

	lpContext->UnlockEntryPassword(lpEntry);
	ar << lpEntry->pszPassword;
	lpContext->LockEntryPassword(lpEntry);

	ar << lpEntry->pszAdditional;

	ArStorePwTime(ar, lpEntry->tCreation);
	ArStorePwTime(ar, lpEntry->tLastMod);
	ArStorePwTime(ar, lpEntry->tLastAccess);
	ArStorePwTime(ar, lpEntry->tExpire);

	if((lpEntry->pszBinaryDesc == NULL) || (lpEntry->pBinaryData == NULL))
	{
		ar << _T("");
		ar << static_cast<DWORD>(0);
	}
	else // Store attachment
	{
		ar << lpEntry->pszBinaryDesc;
		ar << lpEntry->uBinaryDataLen;
		ar.Write(lpEntry->pBinaryData, lpEntry->uBinaryDataLen);
	}

	ar.Close();

	const ULONGLONG uMemLen = memFile.GetLength();
	const BYTE *pbMem = memFile.Detach();

	DWORD dwBase64Len = static_cast<DWORD>(uMemLen * 3 + 12);
	BYTE *pbBase64 = new BYTE[dwBase64Len];
	CBase64Codec::Encode(pbMem, static_cast<DWORD>(uMemLen), pbBase64, &dwBase64Len);

#ifdef _UNICODE
	TCHAR *lpFinal = _StringToUnicode((char *)pbBase64);
#else
	TCHAR *lpFinal = (TCHAR *)pbBase64;
#endif

	CString strFinal = _T(CB64_PROTOCOL);
	strFinal += lpFinal;

	SAFE_DELETE_ARRAY(pbBase64);
#ifdef _UNICODE
	SAFE_DELETE_ARRAY(lpFinal);
#endif

	memFile.Close();
	return strFinal;
}

bool StringToPwEntry(PW_ENTRY *lpEntry, LPCTSTR lpEntryString)
{
	if(lpEntry == NULL) { ASSERT(FALSE); return false; }
	if(lpEntryString == NULL) { ASSERT(FALSE); return false; }

	std::vector<BYTE> vData;
	if(!CBase64Codec::DecodeUrlT(lpEntryString, vData)) { ASSERT(FALSE); return false; }

	CMemFile memFile(&vData[0], vData.size());
	CArchive ar(&memFile, CArchive::load);

	CString strUuid;
	ar >> strUuid;
	_StringToUuid(strUuid, lpEntry->uuid);

	ar >> lpEntry->uGroupId;
	ar >> lpEntry->uImageId;

	CString str;
	ar >> str;
	lpEntry->pszTitle = _TcsSafeDupAlloc((LPCTSTR)str);

	ar >> str;
	lpEntry->pszURL = _TcsSafeDupAlloc((LPCTSTR)str);

	ar >> str;
	lpEntry->pszUserName = _TcsSafeDupAlloc((LPCTSTR)str);

	ar >> str;
	lpEntry->pszPassword = _TcsCryptDupAlloc((LPCTSTR)str);

	ar >> str;
	lpEntry->pszAdditional = _TcsSafeDupAlloc((LPCTSTR)str);

	ArLoadPwTime(ar, lpEntry->tCreation);
	ArLoadPwTime(ar, lpEntry->tLastMod);
	ArLoadPwTime(ar, lpEntry->tLastAccess);
	ArLoadPwTime(ar, lpEntry->tExpire);	

	ar >> str;
	DWORD dwBinLen;
	ar >> dwBinLen;

	if(dwBinLen > 0)
	{
		lpEntry->pszBinaryDesc = _TcsSafeDupAlloc((LPCTSTR)str);
		lpEntry->uBinaryDataLen = dwBinLen;
		lpEntry->pBinaryData = new BYTE[dwBinLen];
		ar.Read(lpEntry->pBinaryData, dwBinLen);
	}
	else
	{
		lpEntry->pszBinaryDesc = _TcsSafeDupAlloc(NULL);
		lpEntry->uBinaryDataLen = 0;
		lpEntry->pBinaryData = NULL;
	}

	ar.Close();
	memFile.Close();
	return true;
}
#endif // _MFC_VER
*/

CEntryUtil::CEntryUtil()
{
}

std::basic_string<TCHAR> CEntryUtil::CreateSummaryList(CPwManager* pMgr,
	const std::vector<DWORD>& vEntryIndices)
{
	std::basic_string<TCHAR> strEmpty;
	if(pMgr == NULL) { ASSERT(FALSE); return strEmpty; }
	if(vEntryIndices.size() == 0) return strEmpty;

	const size_t uMaxEntries = 10;
	size_t uSummaryShow = min(uMaxEntries, vEntryIndices.size());
	if(uSummaryShow == (vEntryIndices.size() - 1)) --uSummaryShow; // Plural msg

	CStringBuilderEx sb;
	for(size_t i = 0; i < uSummaryShow; ++i)
	{
		PW_ENTRY* pe = pMgr->GetEntry(vEntryIndices[i]);
		if(pe == NULL) { ASSERT(FALSE); continue; }

		if(sb.GetLength() > 0) sb.Append(_T("\r\n"));

		sb.Append(_T("- "));
		std::basic_string<TCHAR> strItem = SU_CompactWith3Dots(pe->pszTitle, 39);
		sb.Append(strItem.c_str());

		if(CPwUtil::IsTANEntry(pe) != FALSE)
		{
			std::basic_string<TCHAR> strTanID = SU_CompactWith3Dots(pe->pszUserName, 39);
			if(strTanID.size() > 0)
			{
				sb.Append(_T(" (#"));
				sb.Append(strTanID.c_str());
				sb.Append(_T(")"));
			}
		}
	}

	if(uSummaryShow != vEntryIndices.size())
	{
		const std::basic_string<TCHAR> strCount =
			boost::lexical_cast<std::basic_string<TCHAR> >(static_cast<int>(
			vEntryIndices.size() - uSummaryShow));

		CString strMore = TRL("{PARAM} more entries");
		VERIFY(strMore.Replace(_T("{PARAM}"), strCount.c_str()) == 1);

		sb.Append(_T("\r\n- "));
		sb.Append(strMore);
	}

	return sb.ToString();
}
