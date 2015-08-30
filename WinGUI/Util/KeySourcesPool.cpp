/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2013 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "KeySourcesPool.h"
#include "../../KeePassLibCpp/PwManager.h"
// #include "../../KeePassLibCpp/Crypto/SHA2/SHA2.h"

BOOL CKeySourcesPool::m_bEnabled = TRUE;
bool CKeySourcesPool::m_bEnabledAtLoad = true;
KspVec CKeySourcesPool::m_vec;

CKeySourcesPool::CKeySourcesPool()
{
}

BOOL CKeySourcesPool::GetEnabled()
{
	return m_bEnabled;
}

void CKeySourcesPool::SetEnabled(BOOL bEnabled)
{
	m_bEnabled = ((bEnabled == FALSE) ? FALSE : TRUE);
}

bool CKeySourcesPool::Load(const CPrivateConfigEx* pSource)
{
	if(pSource == NULL) { ASSERT(FALSE); return false; }

	m_vec.clear();

	std::vector<std_string> vKeys = pSource->GetArray(PWMKEY_KEYSOURCE_ID);
	std::vector<std_string> vValues = pSource->GetArray(PWMKEY_KEYSOURCE_VALUE);
	if(vKeys.size() != vValues.size()) { ASSERT(FALSE); return false; }

	for(size_t i = 0; i < vKeys.size(); ++i)
	{
		ASSERT(vKeys[i].size() > 0);
		ASSERT(vValues[i].size() > 0);

		KspKvp kvp = std::make_pair<std_string, std_string>(vKeys[i], vValues[i]);
		m_vec.push_back(kvp);
	}

	m_bEnabledAtLoad = (m_bEnabled != FALSE);
	return true;
}

bool CKeySourcesPool::Save(CPrivateConfigEx* pOut)
{
	if(pOut == NULL) { ASSERT(FALSE); return false; }

	if(m_bEnabled == FALSE)
	{
		m_vec.clear();

		if(m_bEnabledAtLoad) CKeySourcesPool::_ClearAllConfigItems(pOut);
	}

	while(m_vec.size() > CKSP_MAX_ITEMS)
	{
		m_vec.erase(m_vec.begin());
	}

	DWORD dwIndex = 0;
	for(KspVec::const_iterator it = m_vec.begin(); it != m_vec.end(); ++it)
	{
		CKeySourcesPool::_SetAndInc(pOut, it->first.c_str(), it->second.c_str(), &dwIndex);
	}

	CKeySourcesPool::_SetAndInc(pOut, NULL, NULL, &dwIndex); // Terminate
	return true;
}

void CKeySourcesPool::_SetAndInc(CPrivateConfigEx* pOut, LPCTSTR lpKey,
	LPCTSTR lpValue, DWORD* pdwIndex)
{
	CString strKey;
	strKey.Format(_T("%s%u"), PWMKEY_KEYSOURCE_ID, *pdwIndex);
	CString strValue;
	strValue.Format(_T("%s%u"), PWMKEY_KEYSOURCE_VALUE, *pdwIndex);

	pOut->Set(strKey, lpKey);
	pOut->Set(strValue, lpValue);

	*pdwIndex = ((*pdwIndex) + 1);
}

void CKeySourcesPool::_ClearAllConfigItems(CPrivateConfigEx* pOut)
{
	DWORD dwIndex = 0;
	for(size_t i = 0; i < CKSP_MAX_ITEMS; ++i)
		CKeySourcesPool::_SetAndInc(pOut, NULL, NULL, &dwIndex);
}

std_string CKeySourcesPool::Get(LPCTSTR lpDatabasePath)
{
	if(lpDatabasePath == NULL) { ASSERT(FALSE); return std_string(); }

	if(m_bEnabled == FALSE) return std_string();

	CString strWanted = lpDatabasePath;
	strWanted = strWanted.Trim();

	for(KspVec::iterator it = m_vec.begin(); it != m_vec.end(); ++it)
	{
		CString strKey = it->first.c_str();
		strKey = strKey.Trim();

		if(strKey.CompareNoCase(strWanted) == 0)
		{
			std_string strRet = it->second;
			CKeySourcesPool::_UpdateItem(it);
			return strRet;
		}
	}

	return std_string();
}

void CKeySourcesPool::Set(LPCTSTR lpDatabasePath, LPCTSTR lpKeyFilePath)
{
	if(lpDatabasePath == NULL) { ASSERT(FALSE); return; }

	// if(m_bEnabled == FALSE) return; // Temporarily save even when disabled

	CString strDb = lpDatabasePath;
	strDb = strDb.Trim();

	if((lpKeyFilePath == NULL) || (lpKeyFilePath[0] == 0))
	{
		for(KspVec::iterator it = m_vec.begin(); it != m_vec.end(); ++it)
		{
			CString strItem = it->first.c_str();
			strItem = strItem.Trim();

			if(strItem.CompareNoCase(strDb) == 0)
			{
				m_vec.erase(it);
				break;
			}
		}

		return;
	}

	std_string strKey = lpDatabasePath;
	std_string strValue = lpKeyFilePath;
	KspKvp kvpNew = std::make_pair<std_string, std_string>(strKey, strValue);

	for(size_t i = 0; i < m_vec.size(); ++i)
	{
		const KspKvp kvpItem = m_vec[i];
		CString strItem = kvpItem.first.c_str();
		strItem = strItem.Trim();

		if(strItem.CompareNoCase(strDb) == 0)
		{
			m_vec[i] = kvpNew;
			CKeySourcesPool::Get(strKey.c_str()); // Update MRU order
			return;
		}
	}

	m_vec.push_back(kvpNew);
}

/* std_string CKeySourcesPool::_PathToID(LPCTSTR lpDatabasePath)
{
	if(lpDatabasePath == NULL) { ASSERT(FALSE); return std_string(); }

	CString str = lpDatabasePath;
	str = str.Trim();

	LPCTSTR lp = str;
	return std_string(lp);
} */

/* std_string CKeySourcesPool::_PathToID(LPCTSTR lpDatabasePath)
{
	if(lpDatabasePath == NULL) { ASSERT(FALSE); return std_string(); }

	CString str = lpDatabasePath;
	str = str.Trim();
	str = str.MakeLower();
	LPCTSTR lp = str;

	BYTE vHash[32];
	ZeroMemory(&vHash[0], 32);

	sha256_ctx ctx;
	sha256_begin(&ctx);
	sha256_hash((const BYTE*)lp, _tcslen(lp) * sizeof(TCHAR), &ctx);
	sha256_end(vHash, &ctx);

	CString strHash;
	_UuidToString(&vHash[0], &strHash); // Truncate 32 to 16 bytes

	LPCTSTR lpHash = strHash;
	return std_string(lpHash);
} */

void CKeySourcesPool::_UpdateItem(KspVec::iterator& it)
{
	if(it == m_vec.end()) { ASSERT(FALSE); return; }

	std_string strKey = it->first;
	std_string strValue = it->second;

	m_vec.erase(it);

	KspKvp kvp = std::make_pair<std_string, std_string>(strKey, strValue);
	m_vec.push_back(kvp);
}
