/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2010 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___PW_UTIL_H___
#define ___PW_UTIL_H___

#pragma once

#include "../SysDefEx.h"
#include <string>
#include <vector>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include "../PwManager.h"

#define PWU_CVT_EX PMD_HREL_ID
#define PWU_SPN_CONST 0x9AEAA5A3

typedef struct _PG_TREENODE
{
	PW_GROUP g;
	std::vector<boost::shared_ptr<_PG_TREENODE> > vChildNodes;
} PG_TREENODE;

template<typename T>
class CNullableEx : boost::noncopyable
{
public:
	CNullableEx() : m_bHasValue(false), m_tValue() { }
	CNullableEx(const T& t) : m_bHasValue(true), m_tValue(t) { }

	bool HasValue() const { return m_bHasValue; }
	const T& GetValue() const { return m_tValue; }
	T* GetValuePtr() { return &m_tValue; }

	void SetValue(const T& t) { m_bHasValue = true; m_tValue = t; }

private:
	bool m_bHasValue;
	T m_tValue;
};

class CPwUtil : boost::noncopyable
{
private:
	CPwUtil();

public:
	// Very simple password quality estimation function
	static DWORD EstimatePasswordBits(LPCTSTR lpPassword);

	static BOOL LoadHexKey32(FILE *fp, BYTE *pBuf);
	static BOOL SaveHexKey32(FILE *fp, BYTE *pBuf);

	static LPCTSTR GetUniCvtPtr(LPCTSTR lpBase, BOOL bTranslate);
	static CString FormatError(int nErrorCode, DWORD dwFlags);

	static BOOL MemAllocCopyEntry(__in_ecount(1) const PW_ENTRY *pExisting,
		__out_ecount(1) PW_ENTRY *pDestination);
	static void MemFreeEntry(__inout_ecount(1) PW_ENTRY *pEntry);

	// Convert PW_TIME to 5-byte compressed structure and the other way round
	static void TimeToPwTime(__in_ecount(5) const BYTE *pCompressedTime,
		__out_ecount(1) PW_TIME *pPwTime);
	static void PwTimeToTime(__in_ecount(1) const PW_TIME *pPwTime,
		__out_ecount(5) BYTE *pCompressedTime);

	static BOOL AttachFileAsBinaryData(__inout_ecount(1) PW_ENTRY *pEntry,
		const TCHAR *lpFile);
	static BOOL SaveBinaryData(__in_ecount(1) const PW_ENTRY *pEntry,
		const TCHAR *lpFile);
	static BOOL RemoveBinaryData(__inout_ecount(1) PW_ENTRY *pEntry);

	static BOOL IsAllowedStoreGroup(LPCTSTR lpGroupName, LPCTSTR lpSearchGroupName);

	static BOOL IsZeroUUID(__in_ecount(16) const BYTE *pUUID);
	static CString CreateUUIDStr(CNewRandom* pRandomSource);
	static DWORD GetUniCPT(LPCTSTR lp);

	static BOOL IsTANEntry(const PW_ENTRY *pe);

	// static void ProtectMemory(UINT8 *pMem, size_t uBytes, bool bEncrypt);

	static PG_TREENODE GroupsToTree(CPwManager* pMgr);
	static boost::shared_ptr<PG_TREENODE> FindGroupInTree(PG_TREENODE* pRoot,
		DWORD dwGroupId, bool bUnlinkGroup, int iMoveGroup);
	static void FlattenGroupTree(PW_GROUP* pStorage, PG_TREENODE* pRoot,
		DWORD dwStorageCount);

#ifdef _DEBUG
	static void CheckGroupList(CPwManager* pMgr);
#endif

	static std::basic_string<TCHAR> FormatSystemMessage(DWORD dwLastErrorCode);

	static bool UnhideFile(LPCTSTR lpFile);
	static bool HideFile(LPCTSTR lpFile, bool bHide);

	static CNullableEx<FILETIME> GetFileCreationTime(LPCTSTR lpFile);
	static bool SetFileCreationTime(LPCTSTR lpFile, const FILETIME* pTime);

private:
	inline static BOOL ConvertStrToHex(char ch1, char ch2, BYTE& bt);
	inline static void ConvertHexToStr(BYTE bt, char& ch1, char& ch2);

	static boost::shared_ptr<PG_TREENODE> GroupToTreeNode(CPwManager* pMgr,
		DWORD dwIndex, DWORD& dwAllocCount);
	static void FlattenGroupTreeInternal(PW_GROUP* pStorage, PG_TREENODE* pRoot,
		DWORD& dwIndex, DWORD dwStorageCount, USHORT usLevel);
};

/* class CPwErrorInfo
{
public:
	CPwErrorInfo();
	void SetInfo(const std::basic_string<TCHAR>& strText, int nPwErrorCode,
		BOOL bUseLastError);

	std::basic_string<TCHAR> ToString() const;

private:
	void FormatInfo();

	std::basic_string<TCHAR> m_strText;
	int m_nPwErrorCode;
	DWORD m_dwLastError;

	std::basic_string<TCHAR> m_strFinal;
}; */

#endif // ___PW_UTIL_H___
