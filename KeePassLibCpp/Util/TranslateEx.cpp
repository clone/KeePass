/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2014 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "TranslateEx.h"
#include <vector>
#include <string>

#include "StrUtil.h"
#include "MemUtil.h"

static LPTSTR m_lpTranslationData = NULL;

static std::vector<LPTSTR> m_vDefStrings;
static std::vector<LPTSTR> m_vTrlStrings;

static std::basic_string<TCHAR> m_strTableName;

BOOL LoadTranslationTable(LPCTSTR pszTableName)
{
	FreeCurrentTranslationTable();

	if(pszTableName == NULL) { ASSERT(FALSE); return TRUE; }
	if(pszTableName[0] == 0) return TRUE;

	if((_tcsicmp(pszTableName, _T("Standard")) == 0) ||
		(_tcsicmp(pszTableName, _T("English")) == 0))
	{
		m_strTableName = pszTableName;
		return TRUE;
	}

	TCHAR szPath[MAX_PATH * 3];
	ZeroMemory(&szPath[0], MAX_PATH * 3 * sizeof(TCHAR));
	GetModuleFileName(NULL, &szPath[0], MAX_PATH * 3 - 2);

	size_t uPathPos = _tcslen(szPath) - 1;
	while(uPathPos != 0)
	{
		if((szPath[uPathPos] == _T('\\')) || (szPath[uPathPos] == _T('/')))
		{
			szPath[uPathPos + 1] = 0;
			break;
		}
		--uPathPos;
	}
	_tcscat_s(szPath, _countof(szPath), pszTableName);
	_tcscat_s(szPath, _countof(szPath), _T(".lng"));

	FILE *fp = NULL;
	_tfopen_s(&fp, szPath, _T("rb"));
	if(fp == NULL) return FALSE;

	fseek(fp, 0, SEEK_END);
	const long lFileLength = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	UTF8_BYTE *pTranslation8 = new UTF8_BYTE[lFileLength + 1];
	if(pTranslation8 == NULL) { fclose(fp); fp = NULL; return FALSE; }
	pTranslation8[lFileLength] = 0;

	fread(pTranslation8, 1, static_cast<size_t>(lFileLength), fp);
	fclose(fp); fp = NULL;

	m_lpTranslationData = _UTF8ToString(pTranslation8);
	SAFE_DELETE_ARRAY(pTranslation8);
	if(m_lpTranslationData == NULL) { ASSERT(FALSE); return FALSE; }
	
	const size_t dwLength = _tcslen(m_lpTranslationData);
	BOOL bMode = TRL_MODE_DEF;
	BOOL bScanning = FALSE;

	size_t i = 0;
	if(dwLength > 3)
	{
		if((m_lpTranslationData[0] == 0xEF) && (m_lpTranslationData[1] == 0xBB) &&
			(m_lpTranslationData[2] == 0xBF))
		{
			i += 3; // Skip UTF-8 initialization characters
		}
	}

	for( ; i < dwLength; ++i)
	{
		if(m_lpTranslationData[i] == _T('|'))
		{
			m_lpTranslationData[i] = 0;

			if((bMode == TRL_MODE_DEF) && (bScanning == FALSE))
			{
				m_vDefStrings.push_back(&m_lpTranslationData[i + 1]);
				bScanning = TRUE;
			}
			else if((bMode == TRL_MODE_DEF) && (bScanning == TRUE))
			{
				bMode = TRL_MODE_TRL;
				bScanning = FALSE;
			}
			else if((bMode == TRL_MODE_TRL) && (bScanning == FALSE))
			{
				m_vTrlStrings.push_back(&m_lpTranslationData[i + 1]);
				bScanning = TRUE;
			}
			else // if((bMode == TRL_MODE_TRL) && (bScanning == TRUE))
			{
				bMode = TRL_MODE_DEF;
				bScanning = FALSE;
			}
		}
	}

	ASSERT(bMode == TRL_MODE_DEF);
	ASSERT(bScanning == FALSE);

	ASSERT(m_vDefStrings.size() == m_vTrlStrings.size());
	if(m_vDefStrings.size() != m_vTrlStrings.size())
	{
		FreeCurrentTranslationTable();
		return FALSE;
	}

	for(size_t uZeroScan = 0; uZeroScan < m_vDefStrings.size(); ++uZeroScan)
	{
		if(m_vTrlStrings[uZeroScan][0] == 0)
			m_vTrlStrings[uZeroScan] = m_vDefStrings[uZeroScan];
	}

	_SortTrlTable();

#ifdef _DEBUG
	for(int iDupScan = 0; iDupScan < static_cast<int>(m_vDefStrings.size()) - 1; ++iDupScan)
	{
		LPCTSTR lpTestL = m_vDefStrings[iDupScan];
		LPCTSTR lpTestR = m_vDefStrings[iDupScan + 1];
		ASSERT(_tcscmp(lpTestL, lpTestR) != 0); // Test for duplicate definitions
	}
#endif

	m_strTableName = pszTableName;
	return TRUE;
}

BOOL FreeCurrentTranslationTable()
{
	m_vDefStrings.clear();
	m_vTrlStrings.clear();

	SAFE_DELETE_ARRAY(m_lpTranslationData);

	m_strTableName = _T("English");
	return TRUE;
}

void _SortTrlTable()
{
	const size_t uNumStrings = m_vDefStrings.size();
	if(uNumStrings <= 1) return;

	for(size_t i = 0; i < (uNumStrings - 1); ++i)
	{
		size_t uMin = i;

		for(size_t j = i + 1; j < uNumStrings; ++j)
		{
			if(_tcscmp(m_vDefStrings[j], m_vDefStrings[uMin]) < 0)
				uMin = j;
		}

		LPTSTR v = m_vDefStrings[uMin];
		m_vDefStrings[uMin] = m_vDefStrings[i];
		m_vDefStrings[i] = v;

		v = m_vTrlStrings[uMin];
		m_vTrlStrings[uMin] = m_vTrlStrings[i];
		m_vTrlStrings[i] = v;
	}
}

LPCTSTR _TRL(LPCTSTR pszDefString)
{
	ASSERT(pszDefString != NULL); if(pszDefString == NULL) return _T("");

	const size_t uNumStrings = m_vDefStrings.size();
	if(uNumStrings == 0) return pszDefString;

	// Fast binary search on the sorted list of translation strings:
	int l = 0, r = static_cast<int>(uNumStrings) - 1;
	while(l != r)
	{
		const int x = ((l + r) >> 1);
		const int c = _tcscmp(m_vDefStrings[x], pszDefString);
		if(c < 0) l = x + 1;
		else r = x;
	}
	if(_tcscmp(m_vDefStrings[l], pszDefString) == 0) return m_vTrlStrings[l];

	return pszDefString; // English default

	// Previous slow sequencial search (array doesn't have to be sorted):
	/* static unsigned long i;
	for(i = 0; i < m_dwNumTrlStrings; i++)
	{
		if(strcmp(m_pDefString[i], pszDefString) == 0) return m_pTrlString[i];
	}
	// String hasn't been found in the translation table -> return the input string
	return pszDefString; */
}

LPCTSTR GetCurrentTranslationTable()
{
	return m_strTableName.c_str();
}
