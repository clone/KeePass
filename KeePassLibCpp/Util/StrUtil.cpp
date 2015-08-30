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
#include "StrUtil.h"

#include "AppUtil.h"
#include "MemUtil.h"

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
	for(DWORD i = 0; i < vBuffer.size(); ++i)
		vBuffer[i] = 0;

	vBuffer.clear();
}

void EraseTCharVector(std::vector<TCHAR>& vBuffer)
{
	for(DWORD i = 0; i < vBuffer.size(); ++i)
		vBuffer[i] = 0;

	vBuffer.clear();
}

void FixURL(CString *pstrURL)
{
	CString strTemp;
	BOOL bPre = FALSE;

	ASSERT(pstrURL != NULL);

	// Load string and make lower
	strTemp = *pstrURL;
	strTemp.MakeLower();

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
		st.wYear = t.shYear;
		st.wMonth = t.btMonth;
		st.wDay = t.btDay;
		st.wDayOfWeek = 0;
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
	for(int i = 0; i < 16; i++)
	{
		strTemp.Format(_T("%02x"), pUuid[i]);
		*pstrDest += strTemp;
	}
}

C_FN_SHARE void _StringToUuid(const TCHAR *ptszSource, BYTE *pUuid)
{
	ASSERT((ptszSource != NULL) && (pUuid != NULL));
	if((ptszSource == NULL) || (pUuid == NULL)) return;

	// Check against invalid UUID
	ASSERT(_tcslen(ptszSource) == 32);
	if(_tcslen(ptszSource) != 32) return;

	TCHAR ch;
	BYTE bt;
	DWORD i;
	for(i = 0; i < 16; i++)
	{
		ch = ptszSource[i << 1];

		if((ch >= _T('0')) && (ch <= _T('9')))
			bt = (BYTE)(ch - _T('0'));
		else
			bt = (BYTE)(ch - _T('a') + 0x0A);
		ASSERT(bt < 0x10);
		bt <<= 4;

		ch = ptszSource[(i << 1) + 1];

		if((ch >= _T('0')) && (ch <= _T('9')))
			bt |= (BYTE)(ch - _T('0'));
		else
			bt |= (BYTE)(ch - _T('a') + 0x0A);

		pUuid[i] = bt;
	}
}

void ParseURL(CString *pString, PW_ENTRY *pEntry, BOOL bMakeSimString, BOOL bCmdQuotes)
{
	CString str, strTemp;
	int nPos;

	ASSERT(pString != NULL); if(pString == NULL) return;
	ASSERT_ENTRY(pEntry); if(pEntry == NULL) return;

	str = *pString;

	while(1)
	{
		nPos = str.Find(_T("%TITLE%"));
		if(nPos == -1) nPos = str.Find(_T("{TITLE}"));
		if(nPos == -1) break;

		if(bMakeSimString == FALSE)
		{
			strTemp = pEntry->pszTitle;
			if(bCmdQuotes == TRUE) strTemp.Replace(_T("\""), _T("\"\"\""));
			
			str = str.Left(nPos) + strTemp + str.Right(str.GetLength() - nPos - 7);
		}
		else
			str = str.Left(nPos) + TagSimString(pEntry->pszTitle) + str.Right(str.GetLength() - nPos - 7);
	}

	while(1)
	{
		nPos = str.Find(_T("%USERNAME%"));
		if(nPos == -1) nPos = str.Find(_T("{USERNAME}"));
		if(nPos == -1) break;

		if(bMakeSimString == FALSE)
		{
			strTemp = pEntry->pszUserName;
			if(bCmdQuotes == TRUE) strTemp.Replace(_T("\""), _T("\"\"\""));
			
			str = str.Left(nPos) + strTemp + str.Right(str.GetLength() - nPos - 10);
		}
		else
			str = str.Left(nPos) + TagSimString(pEntry->pszUserName) + str.Right(str.GetLength() - nPos - 10);
	}

	while(1)
	{
		nPos = str.Find(_T("%URL%"));
		if(nPos == -1) nPos = str.Find(_T("{URL}"));
		if(nPos == -1) break;

		if(bMakeSimString == FALSE)
		{
			strTemp = pEntry->pszURL;
			if(bCmdQuotes == TRUE) strTemp.Replace(_T("\""), _T("\"\"\""));

			str = str.Left(nPos) + strTemp + str.Right(str.GetLength() - nPos - 5);
		}
		else
			str = str.Left(nPos) + TagSimString(pEntry->pszURL) + str.Right(str.GetLength() - nPos - 5);
	}

	while(1)
	{
		nPos = str.Find(_T("%PASSWORD%"));
		if(nPos == -1) nPos = str.Find(_T("{PASSWORD}"));
		if(nPos == -1) break;

		if(bMakeSimString == FALSE)
		{
			strTemp = pEntry->pszPassword;
			if(bCmdQuotes == TRUE) strTemp.Replace(_T("\""), _T("\"\"\""));

			str = str.Left(nPos) + strTemp + str.Right(str.GetLength() - nPos - 10);
		}
		else
			str = str.Left(nPos) + TagSimString(pEntry->pszPassword) + str.Right(str.GetLength() - nPos - 10);
	}

	while(1)
	{
		nPos = str.Find(_T("%NOTES%"));
		if(nPos == -1) nPos = str.Find(_T("{NOTES}"));
		if(nPos == -1) break;

		if(bMakeSimString == FALSE)
		{
			strTemp = pEntry->pszAdditional;
			strTemp = CsRemoveMeta(&strTemp);

			if(bCmdQuotes == TRUE) strTemp.Replace(_T("\""), _T("\"\"\""));

			str = str.Left(nPos) + strTemp + str.Right(str.GetLength() - nPos - 7);
			EraseCString(&strTemp);
		}
		else
		{
			strTemp = pEntry->pszAdditional;
			strTemp = CsRemoveMeta(&strTemp);
			str = str.Left(nPos) + TagSimString((LPCTSTR)strTemp) + str.Right(str.GetLength() - nPos - 7);
			EraseCString(&strTemp);
		}
	}

	while(1)
	{
		nPos = str.Find(_T("%APPDIR%"));
		if(nPos == -1) nPos = str.Find(_T("{APPDIR}"));
		if(nPos == -1) break;
		TCHAR tszBufP[512];
		GetApplicationDirectory(tszBufP, 512 - 1, TRUE, FALSE);
		if(bMakeSimString == FALSE)
			str = str.Left(nPos) + tszBufP + str.Right(str.GetLength() - nPos - 8);
		else
			str = str.Left(nPos) + TagSimString(tszBufP) + str.Right(str.GetLength() - nPos - 8);
	}

	if(bMakeSimString == TRUE)
	{
		CString strSourceCopy = str;
		unsigned char uch;
		int i;

		EraseCString(&str);

		for(nPos = 0; nPos < strSourceCopy.GetLength(); nPos++)
		{
			uch = (unsigned char)strSourceCopy.GetAt(nPos);

			if(uch > 0x7E)
			{
				str += _T("(%{NUMPAD0}");
				strTemp.Format(_T("%u"), uch);
				ASSERT(strTemp.GetLength() == 3);

				for(i = 0; i < strTemp.GetLength(); i++)
				{
					str += _T("{NUMPAD");
					str += strTemp.GetAt(i);
					str += _T("}");
				}

				str += _T(")");
			}
			else str += (TCHAR)uch;
		}

		EraseCString(&strTemp);
	}

	str.Replace(_T("{CLEARFIELD}"), _T("{DELAY 150}{HOME}(+{END}){DEL}{DELAY 150}"));

	*pString = str;
	EraseCString(&str);
}

CString CsRemoveMeta(CString *psString)
{
	CString str = _T(""), strLower;
	int nPos, nCount;
	LPCTSTR lpRemove = NULL;
	int i;

	ASSERT(psString != NULL); if(psString == NULL) return str;

	str = *psString;
	strLower = str; strLower.MakeLower();

	for(i = 0; i < 2; i++)
	{
		if(i == 0) lpRemove = _T("auto-type:");
		else if(i == 1) lpRemove = _T("auto-type-window:");

		nPos = strLower.Find(lpRemove, 0);
		if(nPos != -1)
		{
			if(nPos != 0)
				if(strLower.GetAt(nPos - 1) == _T('\n')) nPos -= 1;
			if(nPos != 0)
				if(strLower.GetAt(nPos - 1) == _T('\r')) nPos -= 1;

			nCount = strLower.Find(_T('\n'), (int)(nPos + _tcslen(lpRemove) - 1));
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
	CString str;
	int i, j, k;

	ASSERT(psFilePath != NULL); if(psFilePath == NULL) return CString("");

	i = psFilePath->ReverseFind(_T('\\'));
	j = psFilePath->ReverseFind(_T('/'));
	k = (i > j) ? i : j;

	if(k <= -1) str = *psFilePath;
	else str = psFilePath->Right(psFilePath->GetLength() - k - 1);

	return str;
}

#define LOCAL_NUMXMLCONV 7

#pragma warning(push)
#pragma warning(disable: 4996) // _tcscpy deprecated

TCHAR *MakeSafeXmlString(const TCHAR *ptString)
{
	size_t i, j;
	size_t dwStringLen, dwNeededChars = 0, dwOutPos = 0;
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

	dwStringLen = _tcslen(ptString);

	for(i = 0; i < dwStringLen; i++)
	{
		tch = ptString[i];

		bFound = FALSE;
		for(j = 0; j < LOCAL_NUMXMLCONV; j++)
		{
			if(tch == aChar[j])
			{
				dwNeededChars += _tcslen(pTrans[j]);
				bFound = TRUE;
			}
		}
		if(bFound == FALSE) dwNeededChars++;
	}

	pFinal = new TCHAR[dwNeededChars + 4];
	ASSERT(pFinal != NULL); if(pFinal == NULL) return NULL;

	for(i = 0; i < dwStringLen; i++)
	{
		tch = ptString[i];

		bFound = FALSE;
		for(j = 0; j < LOCAL_NUMXMLCONV; j++)
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
	TCHAR *lp;
	TCHAR tch;
	CString str = _T("");
	int nPos = -1, nSearchFrom = 0;

	ASSERT(lpstr != NULL); if(lpstr == NULL) return str; // _T("")

	CString strSource = lpstr;
	strSource.MakeLower();
	lp = (TCHAR *)lpstr;

	// nPos = strSource.Find(lpStart, 0);
	while(dwInstance != DWORD_MAX)
	{
		nPos = strSource.Find(lpStart, nSearchFrom);

		if(nPos != -1) nSearchFrom = nPos + 1;
		else return str; // _T("")

		dwInstance--;
	}

	if(nPos != -1)
	{
		lp += _tcslen(lpStart);
		lp += nPos;

		while(1)
		{
			tch = *lp;

			if(tch == '\0') break;
			else if(tch == '\n') break;
			else if(tch == '\r') { }
			else str += tch;

			lp++;
		}
	}

	str.TrimLeft(); str.TrimRight();
	return str;
}

CString TagSimString(LPCTSTR lpString)
{
	int i;
	CString str = _T("");
	TCHAR tch;

	ASSERT(lpString != NULL); if(lpString == NULL) return str;

	for(i = 0; i < (int)_tcslen(lpString); i++)
	{
		tch = lpString[i];

		switch(tch)
		{
			case _T('+'): str += _T("{PLUS}"); break;
			case _T('@'): str += _T("{AT}"); break;
			// case _T('~'): str += _T("{TILDE}"); break;
			case _T('~'): str += _T("(%{NUMPAD0}{NUMPAD1}{NUMPAD2}{NUMPAD6})"); break;
			case _T('^'): str += _T("(%{NUMPAD0}{NUMPAD9}{NUMPAD4})"); break;
			case _T('\''): str += _T("(%{NUMPAD0}{NUMPAD3}{NUMPAD9})"); break;
			case _T('"'): str += _T("(%{NUMPAD0}{NUMPAD3}{NUMPAD4})"); break;
			case _T('´'): str += _T("(%{NUMPAD0}{NUMPAD1}{NUMPAD8}{NUMPAD0})"); break;
			case _T('`'): str += _T("(%{NUMPAD0}{NUMPAD9}{NUMPAD6})"); break;
			case _T('%'): str += _T("{PERCENT}"); break;
			case _T('{'): str += _T("{LEFTBRACE}"); break;
			case _T('}'): str += _T("{RIGHTBRACE}"); break;
			case _T('('): str += _T("{LEFTPAREN}"); break;
			case _T(')'): str += _T("{RIGHTPAREN}"); break;
			default: str += tch; break;
		}
	}

	return str;
}

/* #pragma warning(push)
#pragma warning(disable: 4996) // _tcscpy deprecated

C_FN_SHARE void _GetPathFromFile(TCHAR *pszFile, TCHAR *pszPath)
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
		size_t sizeNewBuffer = _tcslen(tszSource) + 1;
		ptsz = new TCHAR[sizeNewBuffer];
		if(ptsz != NULL) _tcscpy_s(ptsz, sizeNewBuffer, tszSource); // Clone
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

	pString->TrimLeft();
	pString->TrimRight();
}

WCharStream::WCharStream(LPCWSTR lpData)
{
	ASSERT(lpData != NULL); if(lpData == NULL) return;

	m_lpData = lpData;
	m_dwPosition = 0;
}

WCHAR WCharStream::ReadChar()
{
	WCHAR tValue = m_lpData[m_dwPosition];

	if(tValue == 0) return 0;

	++m_dwPosition;
	return tValue;
}
