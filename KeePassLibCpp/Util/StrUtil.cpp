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
#include "StrUtil.h"

#include "AppUtil.h"
#include "MemUtil.h"
#include "../Crypto/MemoryProtectionEx.h"

#ifndef _WIN32_WCE
#pragma warning(push)
#pragma warning(disable: 4996) // SCL warning
#include <boost/algorithm/string.hpp>
#pragma warning(pop)
#endif

// Securely erase a CString object
void EraseCString(CString *pString)
{
	ASSERT(pString != NULL); if(pString == NULL) return;

	const int nBufLen = pString->GetLength();
	if(nBufLen <= 0) return; // Nothing to clear

	LPTSTR lpt = pString->GetBuffer(0);

	for(int j = 0; j < nBufLen; ++j)
		lpt[j] = (TCHAR)0;

	pString->ReleaseBuffer();
}

void EraseWCharVector(std::vector<WCHAR>& vBuffer)
{
	const DWORD dwBufSize = static_cast<DWORD>(vBuffer.size());
	for(DWORD i = 0; i < dwBufSize; ++i) vBuffer[i] = 0;

	vBuffer.clear();
}

void EraseTCharVector(std::vector<TCHAR>& vBuffer)
{
	const DWORD dwBufSize = static_cast<DWORD>(vBuffer.size());
	for(DWORD i = 0; i < dwBufSize; ++i) vBuffer[i] = 0;

	vBuffer.clear();
}

void FixURL(CString *pstrURL)
{
	CString strTemp;
	BOOL bPre = FALSE;

	ASSERT(pstrURL != NULL);

	// Load string and make lower
	strTemp = *pstrURL;
	strTemp = strTemp.MakeLower();

	// If the string begins with one of the following prefixes it is an URL
	if(strTemp.Left(5) == _T("http:")) bPre = TRUE;
	else if(strTemp.Left(6) == _T("https:")) bPre = TRUE;
	else if(strTemp.Left(4) == _T("cmd:")) bPre = TRUE;
	else if(strTemp.Left(4) == _T("ftp:")) bPre = TRUE;
	else if(strTemp.Left(5) == _T("file:")) bPre = TRUE;
	else if(strTemp.Left(7) == _T("gopher:")) bPre = TRUE;
	else if(strTemp.Left(7) == _T("mailto:")) bPre = TRUE;
	else if(strTemp.Left(5) == _T("news:")) bPre = TRUE;
	else if(strTemp.Left(5) == _T("nntp:")) bPre = TRUE;
	else if(strTemp.Left(9) == _T("prospero:")) bPre = TRUE;
	else if(strTemp.Left(7) == _T("telnet:")) bPre = TRUE;
	else if(strTemp.Left(5) == _T("wais:")) bPre = TRUE;
	else if(strTemp.Left(4) == _T("irc:")) bPre = TRUE;
	else
	{
		int nIndex = strTemp.Find(_T("://"));
		int nNoIndex = strTemp.FindOneOf(_T("/@;: \t\\"));

		if((nIndex != -1) && ((nNoIndex == -1) || (nNoIndex >= nIndex))) bPre = TRUE;
	}

	if(bPre == FALSE) // The string isn't a valid URL, so assume it's a HTTP
	{
		strTemp = _T("http:");
		if(pstrURL->Left(1) != _T("/")) strTemp += _T("//");
		strTemp += *pstrURL;

		*pstrURL = strTemp;
	}
}

void _PwTimeToString(PW_TIME t, CString *pstrDest)
{
	CString strFormatted;
	_PwTimeToStringEx(t, strFormatted, FALSE);
	*pstrDest = strFormatted;
}

void _PwTimeToStringEx(const PW_TIME& t, CString& strDest, BOOL bUseLocalFormat)
{
	strDest.Empty();

	BOOL bUseIso = FALSE;

	if(bUseLocalFormat == TRUE)
	{
		SYSTEMTIME st;
		ZeroMemory(&st, sizeof(SYSTEMTIME));
		st.wYear = t.shYear;
		st.wMonth = t.btMonth;
		st.wDay = t.btDay;
		st.wHour = t.btHour;
		st.wMinute = t.btMinute;
		st.wSecond = t.btSecond;

		TCHAR tszBuf[32];

		tszBuf[0] = 0; tszBuf[1] = 0;
		if(GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, NULL,
			tszBuf, 31) == 0)
		{
			bUseIso = TRUE; ASSERT(FALSE);
		}
		else strDest += tszBuf;

		strDest += _T(" ");

		tszBuf[0] = 0; tszBuf[1] = 0;
		if(GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, tszBuf, 31) == 0)
		{
			bUseIso = TRUE; ASSERT(FALSE);
		}
		else strDest += tszBuf;
	}
	else bUseIso = TRUE;

	if(bUseIso == TRUE)
	{
		strDest.Format(_T("%04u-%02u-%02u %02u:%02u:%02u"), t.shYear,
			t.btMonth, t.btDay, t.btHour, t.btMinute, t.btSecond);
	}
}

void _PwTimeToXmlTime(PW_TIME t, CString *pstrDest)
{
	ASSERT(pstrDest != NULL);
	pstrDest->Empty();
	pstrDest->Format(_T("%04u-%02u-%02uT%02u:%02u:%02u"), t.shYear, t.btMonth,
		t.btDay, t.btHour, t.btMinute, t.btSecond);
}

void _UuidToString(const BYTE *pUuid, CString *pstrDest)
{
	CString strTemp;
	ASSERT(pstrDest != NULL);

	pstrDest->Empty();
	for(int i = 0; i < 16; ++i)
	{
		strTemp.Format(_T("%02x"), pUuid[i]);
		*pstrDest += strTemp;
	}
}

void _StringToUuid(const TCHAR *ptszSource, BYTE *pUuid)
{
	ASSERT((ptszSource != NULL) && (pUuid != NULL));
	if((ptszSource == NULL) || (pUuid == NULL)) return;

	// Check against invalid UUID
	ASSERT(_tcslen(ptszSource) == 32);
	if(_tcslen(ptszSource) != 32) return;

	for(DWORD i = 0; i < 16; ++i)
	{
		TCHAR ch = ptszSource[i << 1];

		BYTE bt;
		if((ch >= _T('0')) && (ch <= _T('9')))
			bt = static_cast<BYTE>(ch - _T('0'));
		else if((ch >= _T('a')) && (ch <= _T('f')))
			bt = static_cast<BYTE>(ch - _T('a') + 0x0A);
		else
		{
			ASSERT((ch >= _T('A')) && (ch <= _T('F')));
			bt = static_cast<BYTE>(ch - _T('A') + 0x0A);
		}
		
		bt <<= 4;

		ch = ptszSource[(i << 1) + 1];

		if((ch >= _T('0')) && (ch <= _T('9')))
			bt |= static_cast<BYTE>(ch - _T('0'));
		else if((ch >= _T('a')) && (ch <= _T('f')))
			bt |= static_cast<BYTE>(ch - _T('a') + 0x0A);
		else
		{
			ASSERT((ch >= _T('A')) && (ch <= _T('F')));
			bt |= static_cast<BYTE>(ch - _T('A') + 0x0A);
		}

		pUuid[i] = bt;
	}
}

// If pReferenceSource is not NULL, it'll be used to dereference
// lpReplaceWith before replacing lpFind
/* BOOL SeqReplace(CString& str, LPCTSTR lpFind, LPCTSTR lpReplaceWith,
	BOOL bMakeSimString, BOOL bCmdQuotes, BOOL bRemoveMeta, PW_ENTRY* peEntryInfo,
	CPwManager* pReferenceSource, DWORD dwRecursionLevel)
{
	ASSERT(lpFind != NULL); if(lpFind == NULL) return FALSE;
	ASSERT(lpReplaceWith != NULL); if(lpReplaceWith == NULL) return FALSE;

	const int nFindLen = static_cast<int>(_tcslen(lpFind));
	CString strFindLower = lpFind;
	strFindLower = strFindLower.MakeLower();

	BOOL bReplaced = FALSE;
	for(int iLoop = 0; iLoop < 20; ++iLoop)
	{
		const int nPos = str.Find(lpFind);
		if(nPos < 0) break;

		const int nStrLen = str.GetLength();

		CString strTemp = lpReplaceWith;
		bool bDoFilter = true;

		if((pReferenceSource != NULL) && (strTemp.GetLength() > 0))
		{
			CString strInner = strTemp;
			ParseURL(&strInner, peEntryInfo, bMakeSimString, bCmdQuotes, pReferenceSource,
				dwRecursionLevel + 1);

			if(strInner != strTemp)
			{
				strTemp = strInner;
				bDoFilter = false;
			}
		}

		if(bDoFilter)
		{
			if(bRemoveMeta == TRUE) strTemp = CsRemoveMeta(&strTemp);

			if(bCmdQuotes == TRUE) strTemp.Replace(_T("\""), _T("\"\"\""));
		}

		if((bMakeSimString == FALSE) || (bDoFilter == false))
			str = str.Left(nPos) + strTemp + str.Right(nStrLen - nPos - nFindLen);
		else
			str = str.Left(nPos) + TagSimString(strTemp) + str.Right(nStrLen -
				nPos - nFindLen);

		bReplaced = TRUE;
	}

	return bReplaced;
} */

/* void ParseURL(CString *pString, PW_ENTRY *pEntry, BOOL bMakeSimString, BOOL bCmdQuotes,
	CPwManager* pDataSource, DWORD dwRecursionLevel)
{
	ASSERT(pString != NULL); if(pString == NULL) return;
	ASSERT_ENTRY(pEntry); if(pEntry == NULL) return;
	ASSERT(pDataSource != NULL); if(pDataSource == NULL) return;
	if(dwRecursionLevel >= PRL_MAX_DEPTH) return;

	CString str = *pString;

	TCHAR tszBufP[512];
	VERIFY(GetApplicationDirectory(tszBufP, 512 - 1, TRUE, FALSE));

	for(int iLoop = 0; iLoop < 20; ++iLoop)
	{
		BOOL b = FALSE;

		b |= SeqReplace(str, _T("{TITLE}"), pEntry->pszTitle, bMakeSimString,
			bCmdQuotes, FALSE, pEntry, pDataSource, dwRecursionLevel);
		b |= SeqReplace(str, _T("{USERNAME}"), pEntry->pszUserName, bMakeSimString,
			bCmdQuotes, FALSE, pEntry, pDataSource, dwRecursionLevel);
		b |= SeqReplace(str, _T("{URL}"), pEntry->pszURL, bMakeSimString,
			bCmdQuotes, FALSE, pEntry, pDataSource, dwRecursionLevel);

		pDataSource->UnlockEntryPassword(pEntry);
		CString strPwCopy = pEntry->pszPassword;
		pDataSource->LockEntryPassword(pEntry);
		b |= SeqReplace(str, _T("{PASSWORD}"), strPwCopy, bMakeSimString,
			bCmdQuotes, FALSE, pEntry, pDataSource, dwRecursionLevel);
		EraseCString(&strPwCopy);

		CString strNotes = ((pEntry->pszAdditional != NULL) ? pEntry->pszAdditional : _T(""));
		strNotes = CsRemoveMeta(&strNotes);
		b |= SeqReplace(str, _T("{NOTES}"), strNotes, bMakeSimString,
			bCmdQuotes, FALSE, pEntry, pDataSource, dwRecursionLevel);

		b |= SeqReplace(str, _T("{APPDIR}"), &tszBufP[0], bMakeSimString,
			bCmdQuotes, FALSE, NULL, NULL, dwRecursionLevel); // {APPDIR} can't contain reference

		str.Replace(_T("{CLEARFIELD}"), _T("{DELAY 150}{HOME}(+{END}){DEL}{DELAY 150}"));

		b |= FillRefPlaceholders(str, bMakeSimString, bCmdQuotes, pDataSource, dwRecursionLevel);

		if(b == FALSE) break;
	}

	if(bMakeSimString == TRUE)
	{
		CString strSourceCopy = str;
		EraseCString(&str);

		for(int nPos = 0; nPos < strSourceCopy.GetLength(); ++nPos)
		{
			const unsigned char uch = static_cast<unsigned char>(strSourceCopy.GetAt(nPos));

			if(uch > 0x7E)
			{
				str += _T("%({NUMPAD0}");

				CString strTemp;
				strTemp.Format(_T("%u"), uch);
				ASSERT(strTemp.GetLength() == 3);

				for(int i = 0; i < strTemp.GetLength(); i++)
				{
					str += _T("{NUMPAD");
					str += strTemp.GetAt(i);
					str += _T("}");
				}

				str += _T(")");
			}
			else str += static_cast<TCHAR>(uch);
		}
	}

	*pString = str;
} */

/* BOOL FillRefPlaceholders(CString& str, BOOL bMakeSimString, BOOL bCmdQuotes,
	CPwManager* pDataSource, DWORD dwRecursionLevel)
{
	ASSERT(pDataSource != NULL); if(pDataSource == NULL) return FALSE;
	
	UNREFERENCED_PARAMETER(bMakeSimString);

	LPCTSTR lpStart = _T("{REF:");
	const int nStartLen = static_cast<int>(_tcslen(lpStart));
	LPCTSTR lpEnd = _T("}");
	// const int nEndLen = static_cast<int>(_tcslen(lpEnd));

	BOOL bReplaced = FALSE;
	for(int iLoop = 0; iLoop < 20; ++iLoop)
	{
		const int nStart = str.Find(lpStart);
		if(nStart < 0) break;
		const int nEnd = str.Find(lpEnd, nStart);
		if(nEnd < 0) break;

		CString strRef = str.Mid(nStart + nStartLen, nEnd - nStart - nStartLen);
		if(strRef.GetLength() <= 4) break;
		if(strRef.GetAt(1) != _T('@')) break;
		if(strRef.GetAt(3) != _T(':')) break;

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
		else break;

		const DWORD dwIndex = pDataSource->FindEx(strID, FALSE, dwFlags, 0);
		if(dwIndex != DWORD_MAX)
		{
			PW_ENTRY *pFound = pDataSource->GetEntry(dwIndex);
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
			else break;

			ParseURL(&strInsData, pFound, FALSE, bCmdQuotes, pDataSource, dwRecursionLevel + 1);

			str = str.Left(nStart) + strInsData + str.Right(str.GetLength() -
				nEnd - 1);
			bReplaced = TRUE;
		}
		else break;
	}

	return bReplaced;
} */

CString CsRemoveMeta(CString *psString)
{
	CString str = _T(""), strLower;
	LPCTSTR lpRemove = NULL;

	ASSERT(psString != NULL); if(psString == NULL) return str;

	str = *psString;
	strLower = str; strLower = strLower.MakeLower();

	for(int i = 0; i < 2; ++i)
	{
		if(i == 0) lpRemove = _T("auto-type:");
		else if(i == 1) lpRemove = _T("auto-type-window:");

		int nPos = strLower.Find(lpRemove, 0);
		if(nPos != -1)
		{
			if(nPos != 0)
				if(strLower.GetAt(nPos - 1) == _T('\n')) nPos -= 1;
			if(nPos != 0)
				if(strLower.GetAt(nPos - 1) == _T('\r')) nPos -= 1;

			int nCount = strLower.Find(_T('\n'), (int)(nPos + _tcslen(lpRemove) - 1));
			if(nCount == -1) nCount = strLower.GetLength() - nPos;
			else nCount -= nPos - 1;

			strLower.Delete(nPos, nCount);
			str.Delete(nPos, nCount);
		}
	}

	return str;
}

CString CsFileOnly(const CString *psFilePath)
{
	ASSERT(psFilePath != NULL); if(psFilePath == NULL) return CString("");

	const int i = psFilePath->ReverseFind(_T('\\'));
	const int j = psFilePath->ReverseFind(_T('/'));
	const int k = ((i > j) ? i : j);

	if(k <= -1) return *psFilePath;
	
	return psFilePath->Right(psFilePath->GetLength() - k - 1);
}

#define LOCAL_NUMXMLCONV 7

#pragma warning(push)
#pragma warning(disable: 4996) // _tcscpy deprecated

TCHAR *MakeSafeXmlString(const TCHAR *ptString)
{
	size_t i, j;
	size_t dwNeededChars = 0, dwOutPos = 0;
	TCHAR tch;
	BOOL bFound;
	TCHAR *pFinal;

	TCHAR aChar[LOCAL_NUMXMLCONV] = {
		_T('<'), _T('>'), _T('&'), _T('\"'), _T('\''),
		_T('\r'), _T('\n')
	};

	TCHAR *pTrans[LOCAL_NUMXMLCONV] = {
		_T("&lt;"), _T("&gt;"), _T("&amp;"), _T("&quot;"), _T("&#39;"),
		_T("&#xD;"), _T("&#xA;")
	};

	ASSERT(ptString != NULL); if(ptString == NULL) return NULL;

	const size_t dwStringLen = _tcslen(ptString);
	for(i = 0; i < dwStringLen; ++i)
	{
		tch = ptString[i];

		bFound = FALSE;
		for(j = 0; j < LOCAL_NUMXMLCONV; ++j)
		{
			if(tch == aChar[j])
			{
				dwNeededChars += _tcslen(pTrans[j]);
				bFound = TRUE;
			}
		}
		if(bFound == FALSE) ++dwNeededChars;
	}

	pFinal = new TCHAR[dwNeededChars + 4];
	ASSERT(pFinal != NULL); if(pFinal == NULL) return NULL;

	for(i = 0; i < dwStringLen; ++i)
	{
		tch = ptString[i];

		bFound = FALSE;
		for(j = 0; j < LOCAL_NUMXMLCONV; ++j)
		{
			if(tch == aChar[j])
			{
				_tcscpy(&pFinal[dwOutPos], pTrans[j]);
				dwOutPos += _tcslen(pTrans[j]);
				bFound = TRUE;
			}
		}

		if(bFound == FALSE)
		{
			pFinal[dwOutPos] = tch;
			dwOutPos++;
		}
	}

	ASSERT(dwOutPos == dwNeededChars);
	pFinal[dwOutPos] = 0; pFinal[dwOutPos + 1] = 0;
	ASSERT(_tcslen(pFinal) == dwNeededChars);

	return pFinal;
}

char *szcpy(char *szDestination, const char *szSource)
{
	ASSERT(szDestination != NULL); if(szDestination == NULL) return NULL;
	ASSERT(szSource != NULL); if(szSource == NULL) { szDestination[0] = 0; return szDestination; }
	return strcpy(szDestination, szSource);
}

#pragma warning(pop) // _tcscpy / strcpy deprecated

DWORD szlen(const char *pszString)
{
	ASSERT(pszString != NULL); if(pszString == NULL) return 0;

	return static_cast<DWORD>(strlen(pszString));
}

// Extracts a substring from the lpstr string
// Example: to extract the auto-type command, pass "auto-type:" in lpStart
CString ExtractParameterFromString(LPCTSTR lpstr, LPCTSTR lpStart,
	DWORD dwInstance)
{
	CString str;

	ASSERT(lpstr != NULL); if(lpstr == NULL) return str; // _T("")

	CString strSource = lpstr;
	strSource = strSource.MakeLower();
	TCHAR *lp = const_cast<TCHAR *>(lpstr);

	int nPos = -1, nSearchFrom = 0;

	// nPos = strSource.Find(lpStart, 0);
	while(dwInstance != DWORD_MAX)
	{
		nPos = strSource.Find(lpStart, nSearchFrom);

		if(nPos != -1) nSearchFrom = nPos + 1;
		else return str; // _T("")

		--dwInstance;
	}

	if(nPos != -1)
	{
		lp += _tcslen(lpStart);
		lp += nPos;

		while(1)
		{
			const TCHAR tch = *lp;

			if(tch == '\0') break;
			else if(tch == '\n') break;
			else if(tch == '\r') { }
			else str += tch;

			++lp;
		}
	}

	str = str.Trim();
	return str;
}

/* #pragma warning(push)
#pragma warning(disable: 4996) // _tcscpy deprecated

void _GetPathFromFile(TCHAR *pszFile, TCHAR *pszPath)
{
	unsigned int i;

	_tcscpy(pszPath, pszFile);

	for(i = _tcslen(pszFile) - 1; i > 1; i--)
	{
		if((pszFile[i] == _T('\\')) || (pszFile[i] == _T('/')))
		{
			pszPath[i] = 0;
			pszPath[i+1] = 0;
			break;
		}
	}

}

#pragma warning(pop) */

TCHAR *_TcsSafeDupAlloc(const TCHAR *tszSource)
{
	TCHAR *ptsz;

	if(tszSource == NULL)
	{
		ptsz = new TCHAR[1];
		if(ptsz != NULL) ptsz[0] = 0; // Terminate string
	}
	else
	{
		const size_t sizeNewBuffer = (_tcslen(tszSource) + 1);
		ptsz = new TCHAR[sizeNewBuffer];
		if(ptsz != NULL) _tcscpy_s(ptsz, sizeNewBuffer, tszSource); // Clone
	}

	return ptsz;
}

LPTSTR _TcsCryptDupAlloc(LPCTSTR lpSource)
{
	ASSERT((CRYPTPROTECTMEMORY_BLOCK_SIZE % sizeof(TCHAR)) == 0);

	TCHAR *ptsz;
	if(lpSource == NULL)
	{
		ptsz = new TCHAR[CRYPTPROTECTMEMORY_BLOCK_SIZE / sizeof(TCHAR)];
		if(ptsz != NULL) ptsz[0] = 0; // Terminate string
	}
	else
	{
		const DWORD bytesNewText = static_cast<DWORD>(((_tcslen(lpSource) + 1) * sizeof(TCHAR)));
		const DWORD allocSize = CMemoryProtectionEx::ToBlockSize(bytesNewText);
		const DWORD allocChars = (allocSize / sizeof(TCHAR)); // See ASSERT above
		ASSERT(CMemoryProtectionEx::ToBlockSize(allocChars * sizeof(TCHAR)) ==
			(allocChars * sizeof(TCHAR)));

		ptsz = new TCHAR[allocChars];

		ASSERT(allocChars >= static_cast<DWORD>(_tcslen(lpSource) + 1));
		if(ptsz != NULL) _tcscpy_s(ptsz, allocChars, lpSource); // Clone
	}

	return ptsz;
}

void RemoveAcceleratorTip(CString *pString)
{
	ASSERT(pString != NULL); if(pString == NULL) return;

	pString->Replace(_T("(&A)"), _T(""));
	pString->Replace(_T("(&D)"), _T(""));
	pString->Replace(_T("(&E)"), _T(""));
	pString->Replace(_T("(&F)"), _T(""));
	pString->Replace(_T("(&H)"), _T(""));
	pString->Replace(_T("(&L)"), _T(""));
	pString->Replace(_T("(&N)"), _T(""));
	pString->Replace(_T("(&O)"), _T(""));
	pString->Replace(_T("(&P)"), _T(""));
	pString->Replace(_T("(&S)"), _T(""));

	pString->Remove(_T('&'));

	pString->Trim();
}

CString RemoveAcceleratorTipEx(LPCTSTR lpString)
{
	if(lpString == NULL) return CString();

	CString str = lpString;
	RemoveAcceleratorTip(&str);
	return str;
}

// Assumes that lpSearch is lower-case when bCaseSensitive == FALSE
// If pUseRegex is not NULL, a regular expression search will be
// performed, otherwise a simple substring matching.
bool StrMatchText(LPCTSTR lpEntryData, LPCTSTR lpSearch,
	BOOL bCaseSensitive, const boost::basic_regex<TCHAR>* pUseRegex)
{
	ASSERT(lpEntryData != NULL);
	if((lpEntryData == NULL) || (lpEntryData[0] == 0)) return false;

	ASSERT((lpSearch != NULL) && (lpSearch[0] != 0));

	// #ifndef _WIN64
	if(pUseRegex != NULL)
	{
		try
		{
			return boost::regex_match(lpEntryData, *pUseRegex,
				boost::regex_constants::match_any);
		}
		catch(...) { return false; }
	}
	// #else
	// #pragma message("No regular expression support in x64 library.")
	// UNREFERENCED_PARAMETER(pUseRegex);
	// #endif

	if(bCaseSensitive == FALSE)
	{
		CString strEntryData = lpEntryData;
		strEntryData = strEntryData.MakeLower();
		return (strEntryData.Find(lpSearch) != -1);
	}

	return (_tcsstr(lpEntryData, lpSearch) != NULL);
}

std::vector<std::basic_string<TCHAR> > SU_SplitSearchTerms(LPCTSTR lpSearch)
{
	std::vector<std::basic_string<TCHAR> > v;
	if((lpSearch == NULL) || (lpSearch[0] == 0)) return v;

	CStringBuilderEx sbTerm;
	bool bQuoted = false;

	const size_t uLen = _tcslen(lpSearch);
	for(size_t i = 0; i < uLen; ++i)
	{
		TCHAR ch = lpSearch[i];

		if(((ch == _T(' ')) || (ch == _T('\t')) || (ch == _T('\r')) ||
			(ch == _T('\n'))) && !bQuoted)
		{
			if(sbTerm.GetLength() > 0) v.push_back(sbTerm.ToString());

			sbTerm.Clear();
		}
		else if(ch == _T('\"')) bQuoted = !bQuoted;
		else sbTerm.Append(ch);
	}
	if(sbTerm.GetLength() > 0) v.push_back(sbTerm.ToString());

	return v;
}

#ifndef _WIN32_WCE
#pragma warning(push)
#pragma warning(disable: 4996) // SCL warning
std::basic_string<TCHAR> SU_GetQuotedPath(const std::basic_string<TCHAR>& strPath)
{
	std::basic_string<TCHAR> str = strPath;

	LPCTSTR lpTrim = _T(" \t\r\n");
	boost::algorithm::trim_if(str, boost::algorithm::is_any_of(lpTrim));

	if(str.size() <= 1) return str;
	if(str[0] != _T('\"')) return str;

	const std::basic_string<TCHAR>::size_type iSecond = str.find(_T('\"'), 1);
	if((iSecond == str.npos) || (iSecond == 0)) return str;

	return str.substr(1, iSecond - 1);
}
#pragma warning(pop)
#endif

CString SU_ConvertNewLines(LPCTSTR lpText, LPCTSTR lpNewLineSeq)
{
	if(lpText == NULL) { ASSERT(FALSE); return CString(); }
	if(lpNewLineSeq == NULL) { ASSERT(FALSE); return CString(); }

	CString str = lpText;
	str.Replace(_T("\r"), _T(""));
	str.Replace(_T("\n"), lpNewLineSeq);
	return str;
}

#pragma warning(push)
#pragma warning(disable: 4996) // SCL warning
void SU_Split(std::vector<std::basic_string<TCHAR> >& vOut,
	const std::basic_string<TCHAR>& strData, LPCTSTR lpSplitChars)
{
	if((lpSplitChars == NULL) || (lpSplitChars[0] == 0))
	{
		vOut.push_back(strData);
		return;
	}

	boost::algorithm::split(vOut, strData, boost::algorithm::is_any_of(lpSplitChars));
}
#pragma warning(pop)

std::basic_string<TCHAR> SU_CompactWith3Dots(LPCTSTR lpString, size_t uMaxChars)
{
	std::basic_string<TCHAR> str;
	if(lpString == NULL) { ASSERT(FALSE); return str; }
	if(uMaxChars == 0) return str;

	str = lpString;
	if(str.size() <= uMaxChars) return str;

	if(uMaxChars <= 3) return str.substr(0, uMaxChars);

	str = str.substr(0, uMaxChars - 3);
	str += _T("...");
	return str;
}

int SU_FindUnescapedCharW(LPCWSTR lpw, WCHAR wch)
{
	if(lpw == NULL) { ASSERT(FALSE); return -1; }

	const size_t uLen = wcslen(lpw);
	for(size_t i = 0; i < uLen; ++i)
	{
		const WCHAR wchCur = lpw[i];

		if(wchCur == L'\\') ++i; // Next is escaped, skip it
		else if(wchCur == wch) return static_cast<int>(i);
	}

	return -1;
}

/////////////////////////////////////////////////////////////////////////////
// WCharStream class

WCharStream::WCharStream(LPCWSTR lpData)
{
	ASSERT(lpData != NULL); if(lpData == NULL) return;

	m_lpData = lpData;
	m_dwPosition = 0;
}

WCHAR WCharStream::ReadChar()
{
	const WCHAR tValue = m_lpData[m_dwPosition];

	if(tValue == 0) return 0;

	++m_dwPosition;
	return tValue;
}

/////////////////////////////////////////////////////////////////////////////
// CStringBuilderEx class

CStringBuilderEx::CStringBuilderEx()
{
}

void CStringBuilderEx::Append(TCHAR tch)
{
	m_vBuf.push_back(tch);
}

void CStringBuilderEx::Append(LPCTSTR lpString)
{
	ASSERT(lpString != NULL); if(lpString == NULL) return;

	DWORD i = 0;
	while(lpString[i] != 0)
	{
		m_vBuf.push_back(lpString[i]);
		++i;
	}
}

std::basic_string<TCHAR> CStringBuilderEx::ToString() const
{
	std::vector<TCHAR> vCopy(m_vBuf);
	vCopy.push_back(0);

	return std::basic_string<TCHAR>(&vCopy[0]);
}

/////////////////////////////////////////////////////////////////////////////
// CStringSetEx class

CStringSetEx::CStringSetEx()
{
}

CStringSetEx::~CStringSetEx()
{
	this->Clear();
}

void CStringSetEx::Clear()
{
	const size_t uStringCount = m_vStrings.size();
	for(size_t i = 0; i < uStringCount; ++i)
		SAFE_DELETE_ARRAY(m_vStrings[i]);

	m_vStrings.clear();
}

LPCTSTR CStringSetEx::Add(LPCTSTR lpString)
{
	if(lpString == NULL) { ASSERT(FALSE); return NULL; }

	const size_t uStringCount = m_vStrings.size();
	for(size_t i = 0; i < uStringCount; ++i)
	{
		if(_tcscmp(m_vStrings[i], lpString) == 0)
			return m_vStrings[i];
	}

	LPTSTR lp = _TcsSafeDupAlloc(lpString);
	m_vStrings.push_back(lp);
	return lp;
}
