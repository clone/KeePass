/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2015 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "SprEngine.h"
#include "SprEncoding.h"
#include <map>

#include "../../../KeePassLibCpp/Util/AppUtil.h"
#include "../../../KeePassLibCpp/Util/StrUtil.h"

#include "../WinUtil.h"

typedef std::map<std::basic_string<TCHAR>, std::basic_string<TCHAR> > SprRefsCache;

static TCHAR g_tszAppDir[SPRE_MAX_PATH_LEN];

void SprInitializeInternalStatic();
CString SprCompileInternal(LPCTSTR lpText, PW_ENTRY* pEntry, CPwManager* pDatabase,
	const SPR_CONTENT_FLAGS* pcf, DWORD dwRecursionLevel, SprRefsCache& vRefsCache);
void SprFillIfExists(CString& strData, LPCTSTR lpPlaceholder, LPCTSTR lpParsable,
	PW_ENTRY* pEntry, CPwManager* pDatabase, const SPR_CONTENT_FLAGS* pcf,
	DWORD dwRecursionLevel, SprRefsCache& vRefsCache);
void SprFillPlaceholder(CString& strData, LPCTSTR lpPlaceholder, LPCTSTR lpReplaceWith,
	const SPR_CONTENT_FLAGS* pcf);
void SprFillRefPlaceholders(CString& str, CPwManager* pDataSource,
	const SPR_CONTENT_FLAGS* pcf, DWORD dwRecursionLevel, SprRefsCache& vRefsCache);
void SprFillRefsUsingCache(CString& str, const SprRefsCache& vRefsCache);
void SprAddRefToCache(LPCTSTR lpRef, LPCTSTR lpValue, SprRefsCache& vRefs);

void SprInitializeInternalStatic()
{
	VERIFY(AU_GetApplicationDirectory(g_tszAppDir, SPRE_MAX_PATH_LEN - 1, TRUE, FALSE));
}

CString SprCompile(LPCTSTR lpText, bool bIsAutoTypeSequence, PW_ENTRY* pEntry,
	CPwManager* pDatabase, bool bEscapeForAutoType, bool bEscapeQuotesForCommandLine)
{
	ASSERT(lpText != NULL); if(lpText == NULL) return CString();
	if(lpText[0] == 0) return CString();

	SprInitializeInternalStatic();

	CString strText = lpText; // Local copy, lpText may mutate

	SPR_CONTENT_FLAGS cf;
	ZeroMemory(&cf, sizeof(SPR_CONTENT_FLAGS));
	cf.bMakeAutoTypeSequence = (bEscapeForAutoType && bIsAutoTypeSequence);
	cf.bMakeCmdQuotes = bEscapeQuotesForCommandLine;

	SprRefsCache vRefsCache;

	CString strNew = SprCompileInternal(strText, pEntry, pDatabase, &cf, 0, vRefsCache);

	if(bEscapeForAutoType && (bIsAutoTypeSequence == false))
		strNew = SprMakeAutoTypeSequence(strNew);

	return strNew;
}

CString SprCompileInternal(LPCTSTR lpText, PW_ENTRY* pEntry, CPwManager* pDatabase,
	const SPR_CONTENT_FLAGS* pcf, DWORD dwRecursionLevel, SprRefsCache& vRefsCache)
{
	ASSERT(lpText != NULL); if(lpText == NULL) return CString();
	if(dwRecursionLevel >= SPRE_MAX_DEPTH) { ASSERT(FALSE); return CString(); }

	CString str = lpText;

	WU_FillPlaceholders(&str, pcf);

	if(pEntry != NULL)
	{
		SprFillIfExists(str, _T("{TITLE}"), pEntry->pszTitle, pEntry,
			pDatabase, pcf, dwRecursionLevel, vRefsCache);
		SprFillIfExists(str, _T("{USERNAME}"), pEntry->pszUserName, pEntry,
			pDatabase, pcf, dwRecursionLevel, vRefsCache);
		SprFillIfExists(str, _T("{URL}"), pEntry->pszURL, pEntry,
			pDatabase, pcf, dwRecursionLevel, vRefsCache);

		if(pDatabase != NULL)
		{
			pDatabase->UnlockEntryPassword(pEntry);
			CString strPwCopy = pEntry->pszPassword; // Create local copy
			pDatabase->LockEntryPassword(pEntry);

			SprFillIfExists(str, _T("{PASSWORD}"), strPwCopy, pEntry,
				pDatabase, pcf, dwRecursionLevel, vRefsCache);

			EraseCString(&strPwCopy); // Erase local copy
		}

		CString strNotes = ((pEntry->pszAdditional != NULL) ? pEntry->pszAdditional : _T(""));
		strNotes = CsRemoveMeta(&strNotes);
		SprFillIfExists(str, _T("{NOTES}"), strNotes, pEntry,
			pDatabase, pcf, dwRecursionLevel, vRefsCache);
	}

	SprFillPlaceholder(str, _T("{APPDIR}"), &g_tszAppDir[0], pcf);

	// Use Bksp instead of Del (in order to avoid Ctrl+Alt+Del);
	// https://sourceforge.net/p/keepass/discussion/329220/thread/4f1aa6b8/
	SprFillPlaceholder(str, _T("{CLEARFIELD}"),
		_T("{DELAY 150}{HOME}(+{END}){BKSP}{DELAY 150}"), NULL);

	CTime t = CTime::GetCurrentTime();
	CString strT = t.Format(_T("%Y%m%d%H%M%S"));
	SprFillPlaceholder(str, _T("{DT_SIMPLE}"), strT, NULL);
	strT = t.Format(_T("%Y"));
	SprFillPlaceholder(str, _T("{DT_YEAR}"), strT, NULL);
	strT = t.Format(_T("%m"));
	SprFillPlaceholder(str, _T("{DT_MONTH}"), strT, NULL);
	strT = t.Format(_T("%d"));
	SprFillPlaceholder(str, _T("{DT_DAY}"), strT, NULL);
	strT = t.Format(_T("%H"));
	SprFillPlaceholder(str, _T("{DT_HOUR}"), strT, NULL);
	strT = t.Format(_T("%M"));
	SprFillPlaceholder(str, _T("{DT_MINUTE}"), strT, NULL);
	strT = t.Format(_T("%S"));
	SprFillPlaceholder(str, _T("{DT_SECOND}"), strT, NULL);
	strT = t.FormatGmt(_T("%Y%m%d%H%M%S"));
	SprFillPlaceholder(str, _T("{DT_UTC_SIMPLE}"), strT, NULL);
	strT = t.FormatGmt(_T("%Y"));
	SprFillPlaceholder(str, _T("{DT_UTC_YEAR}"), strT, NULL);
	strT = t.FormatGmt(_T("%m"));
	SprFillPlaceholder(str, _T("{DT_UTC_MONTH}"), strT, NULL);
	strT = t.FormatGmt(_T("%d"));
	SprFillPlaceholder(str, _T("{DT_UTC_DAY}"), strT, NULL);
	strT = t.FormatGmt(_T("%H"));
	SprFillPlaceholder(str, _T("{DT_UTC_HOUR}"), strT, NULL);
	strT = t.FormatGmt(_T("%M"));
	SprFillPlaceholder(str, _T("{DT_UTC_MINUTE}"), strT, NULL);
	strT = t.FormatGmt(_T("%S"));
	SprFillPlaceholder(str, _T("{DT_UTC_SECOND}"), strT, NULL);

	SprFillRefPlaceholders(str, pDatabase, pcf, dwRecursionLevel, vRefsCache);

	return str;
}

void SprFillIfExists(CString& strData, LPCTSTR lpPlaceholder, LPCTSTR lpParsable,
	PW_ENTRY* pEntry, CPwManager* pDatabase, const SPR_CONTENT_FLAGS* pcf,
	DWORD dwRecursionLevel, SprRefsCache& vRefsCache)
{
	ASSERT(lpPlaceholder != NULL); if(lpPlaceholder == NULL) return;
	ASSERT(lpPlaceholder[0] != 0); if(lpPlaceholder[0] == 0) return;
	ASSERT(lpParsable != NULL); if(lpParsable == NULL) return;

	if(strData.Find(lpPlaceholder) >= 0)
		SprFillPlaceholder(strData, lpPlaceholder, SprCompileInternal(lpParsable,
			pEntry, pDatabase, NULL, dwRecursionLevel + 1, vRefsCache), pcf);
}

void SprFillPlaceholder(CString& strData, LPCTSTR lpPlaceholder, LPCTSTR lpReplaceWith,
	const SPR_CONTENT_FLAGS* pcf)
{
	ASSERT(lpPlaceholder != NULL); if(lpPlaceholder == NULL) return;
	ASSERT(lpPlaceholder[0] != 0); if(lpPlaceholder[0] == 0) return;
	ASSERT(lpReplaceWith != NULL); if(lpReplaceWith == NULL) return;

	strData.Replace(lpPlaceholder, SprTransformContent(lpReplaceWith, pcf));
}

CString SprTransformContent(LPCTSTR lpContent, const SPR_CONTENT_FLAGS* pcf)
{
	ASSERT(lpContent != NULL); if(lpContent == NULL) return CString();

	CString str = lpContent;

	if(pcf != NULL)
	{
		if(pcf->bMakeCmdQuotes)
			str = SprMakeCmdQuotes(str);

		if(pcf->bMakeAutoTypeSequence)
			str = SprMakeAutoTypeSequence(str);
	}

	return str;
}

#define SPRFRP_CONTINUE_WITH_OFFSET { nOffset = nStart + 1; continue; }

void SprFillRefPlaceholders(CString& str, CPwManager* pDataSource,
	const SPR_CONTENT_FLAGS* pcf, DWORD dwRecursionLevel, SprRefsCache& vRefsCache)
{
	if(pDataSource == NULL) return;

	LPCTSTR lpStart = _T("{REF:");
	const int nStartLen = static_cast<int>(_tcslen(lpStart));
	LPCTSTR lpEnd = _T("}");

	int nOffset = 0;
	for(int iLoop = 0; iLoop < 20; ++iLoop)
	{
		SprFillRefsUsingCache(str, vRefsCache);

		const int nStart = str.Find(lpStart, nOffset);
		if(nStart < 0) break;
		const int nEnd = str.Find(lpEnd, nStart);
		if(nEnd < 0) break;

		CString strFullRef = str.Mid(nStart, nEnd - nStart + 1);

		CString strRef = str.Mid(nStart + nStartLen, nEnd - nStart - nStartLen);
		if(strRef.GetLength() <= 4) SPRFRP_CONTINUE_WITH_OFFSET;
		if(strRef.GetAt(1) != _T('@')) SPRFRP_CONTINUE_WITH_OFFSET;
		if(strRef.GetAt(3) != _T(':')) SPRFRP_CONTINUE_WITH_OFFSET;

		const TCHAR tchScan = static_cast<TCHAR>(toupper(strRef.GetAt(2)));
		const TCHAR tchWanted = static_cast<TCHAR>(toupper(strRef.GetAt(0)));
		CString strID = strRef.Mid(4);

		DWORD dwFlags = 0;
		if(tchScan == _T('T')) dwFlags |= PWMF_TITLE;
		else if(tchScan == _T('U')) dwFlags |= PWMF_USER;
		else if(tchScan == _T('A')) dwFlags |= PWMF_URL;
		else if(tchScan == _T('P')) dwFlags |= PWMF_PASSWORD;
		else if(tchScan == _T('N')) dwFlags |= PWMF_ADDITIONAL;
		else if(tchScan == _T('I')) dwFlags |= PWMF_UUID;
		else SPRFRP_CONTINUE_WITH_OFFSET;

		const DWORD dwIndex = pDataSource->FindEx(strID, FALSE, dwFlags, 0);
		if(dwIndex != DWORD_MAX)
		{
			PW_ENTRY* pFound = pDataSource->GetEntry(dwIndex);
			ASSERT_ENTRY(pFound);

			CString strInsData;
			if(tchWanted == _T('T')) strInsData = pFound->pszTitle;
			else if(tchWanted == _T('U')) strInsData = pFound->pszUserName;
			else if(tchWanted == _T('A')) strInsData = pFound->pszURL;
			else if(tchWanted == _T('P'))
			{
				pDataSource->UnlockEntryPassword(pFound);
				strInsData = pFound->pszPassword;
				pDataSource->LockEntryPassword(pFound);
			}
			else if(tchWanted == _T('N'))
			{
				CString strNotes = pFound->pszAdditional;
				strInsData = CsRemoveMeta(&strNotes);
			}
			else if(tchWanted == _T('I')) _UuidToString(pFound->uuid, &strInsData);
			else SPRFRP_CONTINUE_WITH_OFFSET;

			CString strInnerContent = SprCompileInternal(strInsData, pFound,
				pDataSource, NULL, dwRecursionLevel + 1, vRefsCache);
			strInnerContent = SprTransformContent(strInnerContent, pcf);

			// str = str.Left(nStart) + strInnerContent + str.Right(str.GetLength() - nEnd - 1);
			// str.Replace(strFullRef, strInnerContent);
			SprAddRefToCache(strFullRef, strInnerContent, vRefsCache);
			SprFillRefsUsingCache(str, vRefsCache);
		}
		else SPRFRP_CONTINUE_WITH_OFFSET;
	}
}

void SprFillRefsUsingCache(CString& str, const SprRefsCache& vRefs)
{
	for(SprRefsCache::const_iterator it = vRefs.begin(); it != vRefs.end(); ++it)
	{
		str.Replace(it->first.c_str(), it->second.c_str());
	}
}

void SprAddRefToCache(LPCTSTR lpRef, LPCTSTR lpValue, SprRefsCache& vRefs)
{
	if(lpRef == NULL) { ASSERT(FALSE); return; }
	if(lpValue == NULL) { ASSERT(FALSE); return; }

	std::basic_string<TCHAR> strKey = lpRef;
	std::basic_string<TCHAR> strValue = lpValue;

	if(vRefs.find(strKey) == vRefs.end())
		vRefs[strKey] = strValue;
}
