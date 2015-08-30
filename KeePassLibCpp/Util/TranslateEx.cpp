/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2007 Dominik Reichl <dominik.reichl@t-online.de>

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

#include "StrUtil.h"
#include "MemUtil.h"

static BOOL m_bTableLoaded = FALSE;
static LPTSTR m_pTranslationStrings = NULL;

static DWORD m_dwNumTrlStrings = 0;

static LPTSTR m_pDefString[MAX_TRANSLATION_STRINGS];
static LPTSTR m_pTrlString[MAX_TRANSLATION_STRINGS];

C_FN_SHARE void _SortTrlTable();

static TCHAR m_szCurrentTranslationTable[2 * MAX_PATH];

C_FN_SHARE BOOL LoadTranslationTable(LPCTSTR pszTableName)
{
	FILE *fp = NULL;
	TCHAR szPath[2 * MAX_PATH];
	size_t i = 0;
	size_t dwLength = 0;
	BOOL bMode = FALSE;
	BOOL bScanning = FALSE;

	if(m_bTableLoaded == TRUE) FreeCurrentTranslationTable();

	ASSERT(pszTableName != NULL); if(pszTableName == NULL) return FALSE;
	if(_tcslen(pszTableName) == 0) return TRUE;
	if((_tcscmp(pszTableName, _T("Standard")) == 0) || (_tcscmp(pszTableName, _T("English")) == 0))
	{
		_tcscpy_s(m_szCurrentTranslationTable, _countof(m_szCurrentTranslationTable), pszTableName);
		return TRUE;
	}

	GetModuleFileName(NULL, szPath, 2 * MAX_PATH);

	i = _tcslen(szPath) - 1;
	while(1)
	{
		if((szPath[i] == _T('\\')) || (szPath[i] == _T('/')))
		{
			szPath[i+1] = 0;
			break;
		}
		i--;
		if(i == 0) break;
	}
	_tcscat_s(szPath, _countof(szPath), pszTableName);
	_tcscat_s(szPath, _countof(szPath), _T(".lng"));

	_tfopen_s(&fp, szPath, _T("rb"));
	if(fp == NULL) return FALSE;

	fseek(fp, 0, SEEK_END);
	dwLength = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	UTF8_BYTE *pTranslationStrings8 = new UTF8_BYTE[dwLength + 1];
	if(pTranslationStrings8 == NULL) { fclose(fp); fp = NULL; return FALSE; }
	pTranslationStrings8[dwLength] = 0;

	fread(pTranslationStrings8, 1, dwLength, fp);
	fclose(fp); fp = NULL;

	m_pTranslationStrings = _UTF8ToString(pTranslationStrings8);
	SAFE_DELETE_ARRAY(pTranslationStrings8);

	if(m_pTranslationStrings == NULL) return FALSE;
	dwLength = _tcslen(m_pTranslationStrings);

	m_dwNumTrlStrings = 0;

	bMode = TRL_MODE_DEF;
	bScanning = FALSE;

	i = 0;
	if(dwLength > 3)
		if((m_pTranslationStrings[0] == 0xEF) && (m_pTranslationStrings[1] == 0xBB) &&
			(m_pTranslationStrings[2] == 0xBF))
		{
			i += 3; // Skip UTF-8 initialization characters
		}

	for( ; i < dwLength; i++)
	{
		if(m_pTranslationStrings[i] == _T('|'))
		{
			if((bMode == TRL_MODE_DEF) && (bScanning == FALSE))
			{
				m_pDefString[m_dwNumTrlStrings] = &m_pTranslationStrings[i+1];
				m_pTranslationStrings[i] = 0;
				bScanning = TRUE;

				continue;
			}

			if((bMode == TRL_MODE_DEF) && (bScanning == TRUE))
			{
				m_pTranslationStrings[i] = 0;
				bMode = TRL_MODE_TRL;
				bScanning = FALSE;

				continue;
			}
			if((bMode == TRL_MODE_TRL) && (bScanning == FALSE))
			{
				m_pTrlString[m_dwNumTrlStrings] = &m_pTranslationStrings[i+1];
				m_pTranslationStrings[i] = 0;
				bScanning = TRUE;

				continue;
			}
			if((bMode == TRL_MODE_TRL) && (bScanning == TRUE))
			{
				m_pTranslationStrings[i] = 0;
				bMode = TRL_MODE_DEF;
				bScanning = FALSE;

				m_dwNumTrlStrings++;
				if(m_dwNumTrlStrings == MAX_TRANSLATION_STRINGS) break;

				continue;
			}

			// We should never get here
			ASSERT(FALSE);
		}
	}

	ASSERT(m_dwNumTrlStrings < MAX_TRANSLATION_STRINGS);
	ASSERT(bMode == TRL_MODE_DEF);
	ASSERT(bScanning == FALSE);

	for(i = 0; i < m_dwNumTrlStrings; i++)
	{
		if(_tcslen(m_pTrlString[i]) == 0)
		{
			m_pTrlString[i] = m_pDefString[i];
		}
	}

	_tcscpy_s(m_szCurrentTranslationTable, _countof(m_szCurrentTranslationTable), pszTableName);

	_SortTrlTable();
	m_bTableLoaded = TRUE;
	return TRUE;
}

C_FN_SHARE BOOL FreeCurrentTranslationTable()
{
	SAFE_DELETE_ARRAY(m_pTranslationStrings);
	m_dwNumTrlStrings = 0;

	_tcscpy_s(m_szCurrentTranslationTable, _countof(m_szCurrentTranslationTable), _T("English"));

	if(m_bTableLoaded == FALSE) return FALSE;

	return TRUE;
}

C_FN_SHARE void _SortTrlTable()
{
	unsigned long i, j = 0, min;
	TCHAR *v;

	if(m_dwNumTrlStrings <= 1) return;

	for(i = 0; i < (m_dwNumTrlStrings - 1); i++)
	{
		min = i;

		for(j = i + 1; j < m_dwNumTrlStrings; j++)
		{
			if(_tcscmp(m_pDefString[j], m_pDefString[min]) < 0)
				min = j;
		}

		v = m_pDefString[min];
		m_pDefString[min] = m_pDefString[i];
		m_pDefString[i] = v;
		v = m_pTrlString[min];
		m_pTrlString[min] = m_pTrlString[i];
		m_pTrlString[i] = v;
	}
}

C_FN_SHARE LPCTSTR _TRL(LPCTSTR pszDefString)
{
	if(m_dwNumTrlStrings == 0) return pszDefString;
	ASSERT(pszDefString != NULL); if(pszDefString == NULL) return _T("");

	// Fast binary search on the sorted list of translation strings:
	static int l, r, x, c;
	l = 0; r = ((int)m_dwNumTrlStrings) - 1;
	while(l != r)
	{
		x = (l + r) >> 1;
		c = _tcscmp(m_pDefString[x], pszDefString);
		if(c < 0) l = x + 1;
		else r = x;
	}
	if(_tcscmp(m_pDefString[l], pszDefString) == 0) return m_pTrlString[l];

	return pszDefString;

	// Previous slow sequencial search (array doesn't have to be sorted):
	/* static unsigned long i;
	for(i = 0; i < m_dwNumTrlStrings; i++)
	{
		if(strcmp(m_pDefString[i], pszDefString) == 0) return m_pTrlString[i];
	}
	// String hasn't been found in the translation table -> return the input string
	return pszDefString; */
}

C_FN_SHARE LPCTSTR GetCurrentTranslationTable()
{
	return m_szCurrentTranslationTable;
}
