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
#include "../PwManager.h"
#include "../Util/StrUtil.h"
#include <algorithm>

using boost::scoped_ptr;

static std_string g_strFindCachedString;
static std::vector<std_string> g_vFindCachedSplitted;

// DWORD CPwManager::Find(const TCHAR *pszFindString, BOOL bCaseSensitive,
//	DWORD searchFlags, DWORD nStart)
// {
//	return this->Find(pszFindString, bCaseSensitive, searchFlags, nStart, DWORD_MAX);
// }

DWORD CPwManager::Find(const TCHAR *pszFindString, BOOL bCaseSensitive,
	DWORD searchFlags, DWORD nStart, DWORD nEndExcl)
{
	if(nEndExcl > m_dwNumEntries) nEndExcl = m_dwNumEntries;

	if(nStart >= nEndExcl) return DWORD_MAX;
	ASSERT(pszFindString != NULL); if(pszFindString == NULL) return DWORD_MAX;

	CString strFind = pszFindString;
	if((strFind.GetLength() == 0) || (strFind == _T("*"))) return nStart;

	scoped_ptr<boost::basic_regex<TCHAR> > spRegex;
	// #ifndef _WIN64
	if((searchFlags & PWMS_REGEX) != 0)
	{
		try
		{
			if(bCaseSensitive == FALSE)
				spRegex.reset(new boost::basic_regex<TCHAR>((LPCTSTR)strFind,
					boost::regex_constants::icase));
			else
				spRegex.reset(new boost::basic_regex<TCHAR>((LPCTSTR)strFind));
		}
		catch(...) { return DWORD_MAX; }
	}
	// #else
	// #pragma message("No regular expression support in x64 library.")
	// #endif

	LPCTSTR lpSearch = strFind;
	if(bCaseSensitive == FALSE)
	{
		strFind = strFind.MakeLower();
		lpSearch = strFind;
	}

	for(DWORD i = nStart; i < nEndExcl; ++i)
	{
		if((searchFlags & PWMF_TITLE) != 0)
		{
			if(StrMatchText(m_pEntries[i].pszTitle, lpSearch, bCaseSensitive, spRegex.get()))
				return i;
		}

		if((searchFlags & PWMF_USER) != 0)
		{
			if(StrMatchText(m_pEntries[i].pszUserName, lpSearch, bCaseSensitive, spRegex.get()))
				return i;
		}

		if((searchFlags & PWMF_URL) != 0)
		{
			if(StrMatchText(m_pEntries[i].pszURL, lpSearch, bCaseSensitive, spRegex.get()))
				return i;
		}

		if((searchFlags & PWMF_PASSWORD) != 0)
		{
			UnlockEntryPassword(&m_pEntries[i]);
			CString strPassword = m_pEntries[i].pszPassword;
			LockEntryPassword(&m_pEntries[i]);

			if(StrMatchText(strPassword, lpSearch, bCaseSensitive, spRegex.get()))
			{
				EraseCString(&strPassword);
				return i;
			}

			EraseCString(&strPassword);
		}

		if((searchFlags & PWMF_ADDITIONAL) != 0)
		{
			if(StrMatchText(m_pEntries[i].pszAdditional, lpSearch, bCaseSensitive, spRegex.get()))
				return i;
		}

		if((searchFlags & PWMF_GROUPNAME) != 0)
		{
			const DWORD dwGroupIndex = GetGroupByIdN(m_pEntries[i].uGroupId);
			ASSERT(dwGroupIndex != DWORD_MAX);
			if(dwGroupIndex == DWORD_MAX) continue;

			if(StrMatchText(GetGroup(dwGroupIndex)->pszGroupName, lpSearch,
				bCaseSensitive, spRegex.get()))
				return i;
		}

		if((searchFlags & PWMF_UUID) != 0)
		{
			CString strUuid;
			_UuidToString(m_pEntries[i].uuid, &strUuid);

			if(StrMatchText(strUuid, lpSearch, bCaseSensitive, spRegex.get()))
				return i;
		}
	}

	return DWORD_MAX;
}

DWORD CPwManager::FindEx(const TCHAR *pszFindString, BOOL bCaseSensitive,
	DWORD searchFlags, DWORD nStart)
{
	if(((searchFlags & PWMS_REGEX) != 0) || (pszFindString == NULL) ||
		(pszFindString[0] == 0))
		return this->Find(pszFindString, bCaseSensitive, searchFlags, nStart, DWORD_MAX);

	const std_string strText = pszFindString;
	if(strText != g_strFindCachedString)
	{
		g_vFindCachedSplitted = SU_SplitSearchTerms(strText.c_str());
		g_strFindCachedString = strText;
	}
	const std::vector<std_string>* pvTerms = &g_vFindCachedSplitted;

	if(pvTerms->size() == 0) return nStart;

	DWORD dwIndex = nStart;
	while(dwIndex != DWORD_MAX)
	{
		bool bAllMatch = true;
		for(size_t i = 0; i < pvTerms->size(); ++i)
		{
			const std_string& strTerm = (*pvTerms)[i];
			const DWORD dwRes = this->Find(strTerm.c_str(), bCaseSensitive,
				searchFlags, dwIndex, DWORD_MAX);

			if(dwRes > dwIndex)
			{
				dwIndex = dwRes;
				bAllMatch = false;
				break;
			}
		}

		if(bAllMatch) break;
	}

	return dwIndex;
}
