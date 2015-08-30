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
#include "PwExport.h"
#include <vector>

#include "../Util/MemUtil.h"
#include "../Util/StrUtil.h"
#include "../Util/Base64.h"
#include "../Util/TranslateEx.h"

CPwExport::CPwExport()
{
	m_pMgr = NULL;
	m_nFormat = 0;
	m_fp = NULL;

	SetNewLineSeq(FALSE);

	ZeroMemory(&m_aDefaults[PWEXP_TXT], sizeof(PWEXPORT_OPTIONS));
	m_aDefaults[PWEXP_TXT].bGroup = TRUE;
	m_aDefaults[PWEXP_TXT].bTitle = TRUE;
	m_aDefaults[PWEXP_TXT].bUserName = TRUE;
	m_aDefaults[PWEXP_TXT].bURL = TRUE;
	m_aDefaults[PWEXP_TXT].bPassword = TRUE;
	m_aDefaults[PWEXP_TXT].bNotes = TRUE;

	ZeroMemory(&m_aDefaults[PWEXP_HTML], sizeof(PWEXPORT_OPTIONS));
	m_aDefaults[PWEXP_HTML].bGroup = TRUE;
	m_aDefaults[PWEXP_HTML].bTitle = TRUE;
	m_aDefaults[PWEXP_HTML].bUserName = TRUE;
	m_aDefaults[PWEXP_HTML].bURL = TRUE;
	m_aDefaults[PWEXP_HTML].bPassword = TRUE;
	m_aDefaults[PWEXP_HTML].bNotes = TRUE;

	ZeroMemory(&m_aDefaults[PWEXP_XML], sizeof(PWEXPORT_OPTIONS));
	m_aDefaults[PWEXP_XML].bGroup = TRUE;
	m_aDefaults[PWEXP_XML].bGroupTree = TRUE;
	m_aDefaults[PWEXP_XML].bTitle = TRUE;
	m_aDefaults[PWEXP_XML].bUserName = TRUE;
	m_aDefaults[PWEXP_XML].bURL = TRUE;
	m_aDefaults[PWEXP_XML].bPassword = TRUE;
	m_aDefaults[PWEXP_XML].bNotes = TRUE;
	m_aDefaults[PWEXP_XML].bUUID = TRUE;
	m_aDefaults[PWEXP_XML].bImage = TRUE;
	m_aDefaults[PWEXP_XML].bCreationTime = TRUE;
	m_aDefaults[PWEXP_XML].bLastAccTime = TRUE;
	m_aDefaults[PWEXP_XML].bLastModTime = TRUE;
	m_aDefaults[PWEXP_XML].bExpireTime = TRUE;
	m_aDefaults[PWEXP_XML].bAttachment = TRUE;

	ZeroMemory(&m_aDefaults[PWEXP_CSV], sizeof(PWEXPORT_OPTIONS));
	m_aDefaults[PWEXP_CSV].bTitle = TRUE;
	m_aDefaults[PWEXP_CSV].bUserName = TRUE;
	m_aDefaults[PWEXP_CSV].bURL = TRUE;
	m_aDefaults[PWEXP_CSV].bPassword = TRUE;
	m_aDefaults[PWEXP_CSV].bNotes = TRUE;

	ZeroMemory(&m_aDefaults[PWEXP_KEEPASS], sizeof(PWEXPORT_OPTIONS));
	m_aDefaults[PWEXP_KEEPASS].bGroup = TRUE;
	m_aDefaults[PWEXP_KEEPASS].bGroupTree = TRUE;
	m_aDefaults[PWEXP_KEEPASS].bTitle = TRUE;
	m_aDefaults[PWEXP_KEEPASS].bUserName = TRUE;
	m_aDefaults[PWEXP_KEEPASS].bURL = TRUE;
	m_aDefaults[PWEXP_KEEPASS].bPassword = TRUE;
	m_aDefaults[PWEXP_KEEPASS].bNotes = TRUE;
	m_aDefaults[PWEXP_KEEPASS].bUUID = TRUE;
	m_aDefaults[PWEXP_KEEPASS].bImage = TRUE;
	m_aDefaults[PWEXP_KEEPASS].bCreationTime = TRUE;
	m_aDefaults[PWEXP_KEEPASS].bLastAccTime = TRUE;
	m_aDefaults[PWEXP_KEEPASS].bLastModTime = TRUE;
	m_aDefaults[PWEXP_KEEPASS].bExpireTime = TRUE;
	m_aDefaults[PWEXP_KEEPASS].bAttachment = TRUE;
	m_aDefaults[PWEXP_KEEPASS].bExportBackups = TRUE;
}

CPwExport::~CPwExport()
{
	m_pMgr = NULL;
	m_nFormat = 0;
	m_fp = NULL;
}

void CPwExport::SetManager(CPwManager *pMgr)
{
	ASSERT(pMgr != NULL);
	m_pMgr = pMgr;
}

void CPwExport::SetFormat(int nFormat)
{
	ASSERT((nFormat > PWEXP_NULL) && (nFormat < PWEXP_LAST));
	if((nFormat > PWEXP_NULL) && (nFormat < PWEXP_LAST))
		m_nFormat = nFormat;
}

void CPwExport::SetNewLineSeq(BOOL bWindows)
{
	if(bWindows == TRUE) m_pszNewLine = _T("\r\n");
	else m_pszNewLine = _T("\n");
}

void CPwExport::_ExpStr(LPCTSTR lpString)
{
	if(_tcslen(lpString) > 0)
	{
		UTF8_BYTE *pUtf8String = _StringToUTF8(lpString);

		ASSERT(pUtf8String != NULL);
		if(pUtf8String != NULL)
		{
			fwrite(pUtf8String, 1, strlen((char *)pUtf8String), m_fp);
			SAFE_DELETE_ARRAY(pUtf8String);
		}
	}
}

void CPwExport::_ExpXmlStr(LPCTSTR lpString)
{
	if(_tcslen(lpString) > 0)
	{
		TCHAR *pXmlString = MakeSafeXmlString(lpString);
		
		ASSERT(pXmlString != NULL);
		if(pXmlString != NULL)
		{
			UTF8_BYTE *pUtf8String = _StringToUTF8(pXmlString);
			fwrite(pUtf8String, 1, strlen((char *)pUtf8String), m_fp);
			SAFE_DELETE_ARRAY(pUtf8String);
			SAFE_DELETE_ARRAY(pXmlString);
		}
	}
}

void CPwExport::_ExpHtmlStr(LPCTSTR lpString)
{
	if(lpString[0] == 0) _ExpStr(_T("&nbsp;"));
	else _ExpXmlStr(lpString);
}

void CPwExport::_ExpStrIf(BOOL bCondition, LPCTSTR lpString)
{
	if(bCondition == TRUE)
	{
		if(m_bOneSkipped == TRUE) _ExpStr(m_lpSep);
		else m_bOneSkipped = TRUE;

		if(m_pOptions->bEncodeNewlines == TRUE)
		{
			CString str = lpString;
			str.Replace(_T("\r"), _T("\\r"));
			str.Replace(_T("\n"), _T("\\n"));
			_ExpStr(str);
		}
		else _ExpStr(lpString);
	}
}

void CPwExport::_ExpXmlStrIf(BOOL bCondition, LPCTSTR lpString)
{
	if(bCondition == TRUE)
	{
		if(m_bOneSkipped == TRUE) _ExpStr(m_lpSep);
		else m_bOneSkipped = TRUE;

		if(m_pOptions->bEncodeNewlines == TRUE)
		{
			CString str = lpString;
			str.Replace(_T("\r"), _T("\\r"));
			str.Replace(_T("\n"), _T("\\n"));
			_ExpXmlStr(str);
		}
		else _ExpXmlStr(lpString);
	}
}

void CPwExport::_ExpHtmlStrIf(BOOL bCondition, LPCTSTR lpString)
{
	if(bCondition == TRUE)
	{
		if(m_bOneSkipped == TRUE) _ExpStr(m_lpSep);
		else m_bOneSkipped = TRUE;

		if(m_pOptions->bEncodeNewlines == TRUE)
		{
			CString str = lpString;
			str.Replace(_T("\r"), _T("\\r"));
			str.Replace(_T("\n"), _T("\\n"));
			_ExpHtmlStr(str);
		}
		else _ExpHtmlStr(lpString);
	}
}

void CPwExport::_ExpCsvStrIf(BOOL bCondition, LPCTSTR lpString)
{
	if(bCondition == TRUE)
	{
		if(m_bOneSkipped == TRUE) _ExpStr(m_lpSep);
		else m_bOneSkipped = TRUE;

		CString str = lpString;
		str.Replace(_T("\\"), _T("\\\\"));
		str.Replace(_T("\""), _T("\\\""));

		if(m_pOptions->bEncodeNewlines == TRUE)
		{
			str.Replace(_T("\r"), _T("\\r"));
			str.Replace(_T("\n"), _T("\\n"));
			_ExpStr(str);
		}
		else _ExpStr(str);
	}
}

void CPwExport::_ExpResetSkip()
{
	m_bOneSkipped = FALSE;
}

void CPwExport::_ExpSetSep(LPCTSTR lpSep)
{
	if(lpSep == NULL) m_lpSep = _T("");
	else m_lpSep = lpSep;
}

BOOL CPwExport::ExportAll(const TCHAR *pszFile, const PWEXPORT_OPTIONS *pOptions, CPwManager *pStoreMgr)
{
	ASSERT(pszFile != NULL);
	return ExportGroup(pszFile, DWORD_MAX, pOptions, pStoreMgr);
}

CString CPwExport::MakeGroupTreeString(DWORD dwGroupId, bool bXmlEncode) const
{
	CString str;

	PW_GROUP *pg = m_pMgr->GetGroupById(dwGroupId);
	ASSERT(pg != NULL); if(pg == NULL) return str;

	const USHORT usLevel = pg->usLevel;
	ASSERT(usLevel != 0xFFFF); if(usLevel == 0xFFFF) return str;
	if(usLevel == 0) return str;
	DWORD *pdwIndices = new DWORD[usLevel + 2];
	ASSERT(pdwIndices != NULL); if(pdwIndices == NULL) return str;

	if(m_pMgr->GetGroupTree(dwGroupId, pdwIndices) == TRUE)
	{
		for(USHORT i = 0; i < usLevel; ++i)
		{
			pg = m_pMgr->GetGroup(pdwIndices[i]);

			if(pg != NULL)
			{
				if(i != 0) str += _T("\\");

				CString strTemp;
				if(bXmlEncode)
				{
					TCHAR *tszSafeGroup = MakeSafeXmlString(pg->pszGroupName);
					strTemp = tszSafeGroup;
					SAFE_DELETE_ARRAY(tszSafeGroup);
				}
				else strTemp = pg->pszGroupName;

				strTemp.Replace(_T("\\"), _T("/")); // Avoid ambiguity with tree separator
				str += strTemp;
			}
		}
	}

	SAFE_DELETE_ARRAY(pdwIndices);
	return str;
}

BOOL CPwExport::ExportGroup(const TCHAR *pszFile, DWORD dwGroupId, const PWEXPORT_OPTIONS *pOptions, CPwManager *pStoreMgr)
{
	FILE *fp = NULL;
	DWORD i, j, dwThisId, dwNumberOfGroups;
	PW_ENTRY *p;
	PW_GROUP *pg;
	BYTE aInitUTF8[3] = { 0xEF, 0xBB, 0xBF };
	CString str, strUUID, strImage, strCreationTime, strLastAccTime, strLastModTime;
	CString strExpireTime, strGroupTree;
	BYTE *pbEncodedAttachment = NULL;
	PW_TIME tNever;
	std::vector<DWORD> aGroupIds;
	USHORT usLevel = 0;
	BOOL bReturn = TRUE;

	ASSERT(m_pMgr != NULL); if(m_pMgr == NULL) return FALSE;
	m_pMgr->GetNeverExpireTime(&tNever);

	ASSERT(pszFile != NULL); if(pszFile == NULL) return FALSE;

	if(m_nFormat == PWEXP_KEEPASS) { ASSERT(pStoreMgr != NULL); if(pStoreMgr == NULL) return FALSE; }

	if(pOptions == NULL)
	{
		if(m_nFormat == PWEXP_TXT) pOptions = &m_aDefaults[PWEXP_TXT];
		else if(m_nFormat == PWEXP_HTML) pOptions = &m_aDefaults[PWEXP_HTML];
		else if(m_nFormat == PWEXP_XML) pOptions = &m_aDefaults[PWEXP_XML];
		else if(m_nFormat == PWEXP_CSV) pOptions = &m_aDefaults[PWEXP_CSV];
		else if(m_nFormat == PWEXP_KEEPASS) pOptions = &m_aDefaults[PWEXP_KEEPASS];
		else { ASSERT(FALSE); return FALSE; }
	}
	m_pOptions = pOptions;

	dwNumberOfGroups = m_pMgr->GetNumberOfGroups();
	const DWORD dwInvalidId1 = m_pMgr->GetGroupId(PWS_BACKUPGROUP_SRC);
	const DWORD dwInvalidId2 = m_pMgr->GetGroupId(PWS_BACKUPGROUP);

	if(dwGroupId != DWORD_MAX)
	{
		i = m_pMgr->GetGroupByIdN(dwGroupId);
		ASSERT(i != DWORD_MAX); if(i == DWORD_MAX) return FALSE;

		usLevel = m_pMgr->GetGroup(i)->usLevel;

		while(true)
		{
			pg = m_pMgr->GetGroup(i);
			ASSERT(pg != NULL); if(pg == NULL) break;

			if((pg->uGroupId != dwGroupId) && (pg->usLevel <= usLevel)) break;

			const DWORD dwToAdd = pg->uGroupId;
			aGroupIds.push_back(dwToAdd);

			++i;
			if(i == dwNumberOfGroups) break;
		}
	}
	else
	{
		for(i = 0; i < dwNumberOfGroups; ++i)
		{
			pg = m_pMgr->GetGroup(i);
			ASSERT(pg != NULL); if(pg == NULL) continue;

			const DWORD dwToAdd = pg->uGroupId;
			aGroupIds.push_back(dwToAdd);
		}
	}

	if(m_nFormat != PWEXP_KEEPASS)
	{
		fp = NULL;
		_tfopen_s(&fp, pszFile, _T("wb"));
		if(fp == NULL) { aGroupIds.clear(); return FALSE; }
		m_fp = fp;

		fwrite(aInitUTF8, 1, 3, fp);
	}

	if(m_nFormat == PWEXP_TXT)
	{
	}
	else if(m_nFormat == PWEXP_HTML)
	{
		_ExpStr(_T("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">"));
		_ExpStr(m_pszNewLine);
		_ExpStr(_T("<html><head>"));
		_ExpStr(_T("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">"));
		_ExpStr(_T("<title>"));
		_ExpXmlStr(TRL("Password List"));
		_ExpStr(_T("</title></head><body>"));
		_ExpStr(m_pszNewLine);
		_ExpStr(_T("<h1>"));
		_ExpXmlStr(TRL("Password List"));
		_ExpStr(_T("</h1>"));
		_ExpStr(m_pszNewLine);
		_ExpStr(_T("<table width=\"100%\" border=\"1px\" cellspacing=\"0\" cellpadding=\"1\"><tr><th>"));

		_ExpSetSep(_T("</th><th>"));
		_ExpResetSkip();
		_ExpXmlStrIf(pOptions->bGroup, TRL("Password Groups"));
		_ExpXmlStrIf(pOptions->bGroupTree, TRL("Group Tree"));
		_ExpXmlStrIf(pOptions->bTitle, TRL("Title"));
		_ExpXmlStrIf(pOptions->bUserName, TRL("User Name"));
		_ExpXmlStrIf(pOptions->bURL, TRL("URL"));
		_ExpXmlStrIf(pOptions->bPassword, TRL("Password"));
		_ExpXmlStrIf(pOptions->bNotes, TRL("Notes"));
		_ExpXmlStrIf(pOptions->bUUID, TRL("UUID"));
		_ExpXmlStrIf(pOptions->bImage, TRL("Icon"));
		_ExpXmlStrIf(pOptions->bCreationTime, TRL("Creation Time"));
		_ExpXmlStrIf(pOptions->bLastAccTime, TRL("Last Access"));
		_ExpXmlStrIf(pOptions->bLastModTime, TRL("Last Modification"));
		_ExpXmlStrIf(pOptions->bExpireTime, TRL("Expires"));
		_ExpXmlStrIf(pOptions->bAttachment, TRL("Attachment Description"));
		_ExpXmlStrIf(pOptions->bAttachment, TRL("Attachment"));

		_ExpStr(_T("</th></tr>"));
		_ExpStr(m_pszNewLine);
	}
	else if(m_nFormat == PWEXP_XML)
	{
		_ExpStr(_T("<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>"));
		_ExpStr(m_pszNewLine);
		_ExpStr(_T("<pwlist>"));
		_ExpStr(m_pszNewLine);
	}
	else if(m_nFormat == PWEXP_CSV)
	{
		_ExpStr(_T("\""));

		_ExpSetSep(_T("\",\""));
		_ExpResetSkip();
		_ExpStrIf(pOptions->bGroup, _T("Password Groups"));
		_ExpStrIf(pOptions->bGroupTree, _T("Group Tree"));
		_ExpStrIf(pOptions->bTitle, _T("Account"));
		_ExpStrIf(pOptions->bUserName, _T("Login Name"));
		_ExpStrIf(pOptions->bPassword, _T("Password"));
		_ExpStrIf(pOptions->bURL, _T("Web Site"));
		_ExpStrIf(pOptions->bNotes, _T("Comments"));
		_ExpStrIf(pOptions->bUUID, _T("UUID"));
		_ExpStrIf(pOptions->bImage, _T("Icon"));
		_ExpStrIf(pOptions->bCreationTime, _T("Creation Time"));
		_ExpStrIf(pOptions->bLastAccTime, _T("Last Access"));
		_ExpStrIf(pOptions->bLastModTime, _T("Last Modification"));
		_ExpStrIf(pOptions->bExpireTime, _T("Expires"));
		_ExpStrIf(pOptions->bAttachment, _T("Attachment Description"));
		_ExpStrIf(pOptions->bAttachment, _T("Attachment"));

		_ExpStr(_T("\""));
		_ExpStr(m_pszNewLine);
	}
	else if(m_nFormat == PWEXP_KEEPASS)
	{
		DWORD dwGroupEnum;
		PW_GROUP pwgTemplate;
		for(dwGroupEnum = 0; dwGroupEnum < (DWORD)aGroupIds.size(); dwGroupEnum++)
		{
			PW_GROUP *pgCopy = m_pMgr->GetGroupById(aGroupIds[dwGroupEnum]);
			ASSERT(pgCopy != NULL); if(pgCopy == NULL) return FALSE;
			pwgTemplate = *pgCopy;
			pwgTemplate.usLevel = (USHORT)(pwgTemplate.usLevel - usLevel);
			pStoreMgr->AddGroup(&pwgTemplate);
		}
	}
	else { ASSERT(FALSE); }

	DWORD uNumEntries = m_pMgr->GetNumberOfEntries();

	for(j = 0; j < static_cast<DWORD>(aGroupIds.size()); ++j)
	{
		dwThisId = aGroupIds[j];

		if(m_pOptions->bExportBackups == FALSE)
		{
			if(dwThisId == dwInvalidId1) continue;
			if(dwThisId == dwInvalidId2) continue;
		}

		for(i = 0; i < uNumEntries; ++i)
		{
			p = m_pMgr->GetEntry(i);
			ASSERT_ENTRY(p);

			// Are we exporting this group?
			if(p->uGroupId != dwThisId) continue;

			pg = m_pMgr->GetGroupById(p->uGroupId);
			ASSERT(pg != NULL); if(pg == NULL) continue;

			m_pMgr->UnlockEntryPassword(p);

			_UuidToString(p->uuid, &strUUID);
			strImage.Format(_T("%u"), p->uImageId);

			if((m_nFormat == PWEXP_CSV) || (m_nFormat == PWEXP_XML))
			{
				_PwTimeToXmlTime(p->tCreation, &strCreationTime);
				_PwTimeToXmlTime(p->tLastAccess, &strLastAccTime);
				_PwTimeToXmlTime(p->tLastMod, &strLastModTime);
				_PwTimeToXmlTime(p->tExpire, &strExpireTime);
			}
			else
			{
				_PwTimeToString(p->tCreation, &strCreationTime);
				_PwTimeToString(p->tLastAccess, &strLastAccTime);
				_PwTimeToString(p->tLastMod, &strLastModTime);
				_PwTimeToString(p->tExpire, &strExpireTime);
			}

			strGroupTree = MakeGroupTreeString(pg->uGroupId, ((m_nFormat == PWEXP_XML) ||
				(m_nFormat == PWEXP_HTML)));

			ASSERT(p->pszBinaryDesc != NULL);

			if((pOptions->bAttachment == TRUE) && (p->uBinaryDataLen != 0) && (p->pBinaryData != NULL))
			{
				DWORD dwBufSize = (((p->uBinaryDataLen + 2) / 3) << 2) + 1;

				pbEncodedAttachment = new BYTE[dwBufSize + 1];
				ASSERT(pbEncodedAttachment != NULL);
				if(pbEncodedAttachment != NULL)
				{
					if(CBase64Codec::Encode(p->pBinaryData, p->uBinaryDataLen,
						pbEncodedAttachment, &dwBufSize) == false)
					{
						SAFE_DELETE_ARRAY(pbEncodedAttachment);
					}
				}
			}
			else pbEncodedAttachment = NULL;

			if(m_nFormat == PWEXP_TXT)
			{
				_ExpSetSep(NULL);

				_ExpStrIf(pOptions->bTitle, _T("["));
				_ExpStrIf(pOptions->bTitle, p->pszTitle);
				_ExpStrIf(pOptions->bTitle, _T("]"));
				if(pOptions->bTitle == TRUE) _ExpStr(m_pszNewLine);

				_ExpStrIf(pOptions->bGroup, TRL("Group:"));
				_ExpStrIf(pOptions->bGroup, _T(" "));
				_ExpStrIf(pOptions->bGroup, pg->pszGroupName);
				if(pOptions->bGroup == TRUE) _ExpStr(m_pszNewLine);

				_ExpStrIf(pOptions->bGroupTree, TRL("Group Tree"));
				_ExpStrIf(pOptions->bGroupTree, _T(": "));
				_ExpStrIf(pOptions->bGroupTree, strGroupTree);
				if(pOptions->bGroupTree == TRUE) _ExpStr(m_pszNewLine);

				_ExpStrIf(pOptions->bUserName, TRL("User Name"));
				_ExpStrIf(pOptions->bUserName, _T(": "));
				_ExpStrIf(pOptions->bUserName, p->pszUserName);
				if(pOptions->bUserName == TRUE) _ExpStr(m_pszNewLine);

				_ExpStrIf(pOptions->bURL, TRL("URL:"));
				_ExpStrIf(pOptions->bURL, _T(" "));
				_ExpStrIf(pOptions->bURL, p->pszURL);
				if(pOptions->bURL == TRUE) _ExpStr(m_pszNewLine);

				_ExpStrIf(pOptions->bPassword, TRL("Password:"));
				_ExpStrIf(pOptions->bPassword, _T(" "));
				_ExpStrIf(pOptions->bPassword, p->pszPassword);
				if(pOptions->bPassword == TRUE) _ExpStr(m_pszNewLine);

				_ExpStrIf(pOptions->bNotes, TRL("Notes:"));
				_ExpStrIf(pOptions->bNotes, _T(" "));
				CString strNotesConv = SU_ConvertNewLines(p->pszAdditional, m_pszNewLine);
				_ExpStrIf(pOptions->bNotes, strNotesConv);
				if(pOptions->bNotes == TRUE) _ExpStr(m_pszNewLine);

				_ExpStrIf(pOptions->bUUID, TRL("UUID"));
				_ExpStrIf(pOptions->bUUID, _T(": "));
				_ExpStrIf(pOptions->bUUID, strUUID);
				if(pOptions->bUUID == TRUE) _ExpStr(m_pszNewLine);

				_ExpStrIf(pOptions->bImage, TRL("Icon"));
				_ExpStrIf(pOptions->bImage, _T(": "));
				_ExpStrIf(pOptions->bImage, strImage);
				if(pOptions->bImage == TRUE) _ExpStr(m_pszNewLine);

				_ExpStrIf(pOptions->bCreationTime, TRL("Creation Time"));
				_ExpStrIf(pOptions->bCreationTime, _T(": "));
				_ExpStrIf(pOptions->bCreationTime, strCreationTime);
				if(pOptions->bCreationTime == TRUE) _ExpStr(m_pszNewLine);

				_ExpStrIf(pOptions->bLastAccTime, TRL("Last Access"));
				_ExpStrIf(pOptions->bLastAccTime, _T(": "));
				_ExpStrIf(pOptions->bLastAccTime, strLastAccTime);
				if(pOptions->bLastAccTime == TRUE) _ExpStr(m_pszNewLine);

				_ExpStrIf(pOptions->bLastModTime, TRL("Last Modification"));
				_ExpStrIf(pOptions->bLastModTime, _T(": "));
				_ExpStrIf(pOptions->bLastModTime, strLastModTime);
				if(pOptions->bLastModTime == TRUE) _ExpStr(m_pszNewLine);

				_ExpStrIf(pOptions->bExpireTime, TRL("Expires"));
				_ExpStrIf(pOptions->bExpireTime, _T(": "));
				_ExpStrIf(pOptions->bExpireTime, strExpireTime);
				if(pOptions->bExpireTime == TRUE) _ExpStr(m_pszNewLine);

				if(p->pszBinaryDesc[0] != 0)
				{
					_ExpStrIf(pOptions->bAttachment, TRL("Attachment Description"));
					_ExpStrIf(pOptions->bAttachment, _T(": "));
					_ExpStrIf(pOptions->bAttachment, p->pszBinaryDesc);
					if(pOptions->bAttachment == TRUE) _ExpStr(m_pszNewLine);
				}

				if((p->uBinaryDataLen != 0) && (pbEncodedAttachment != NULL))
				{
					_ExpStrIf(pOptions->bAttachment, TRL("Attachment"));
					_ExpStrIf(pOptions->bAttachment, _T(": "));
					_ExpStrIf(pOptions->bAttachment, (LPCTSTR)pbEncodedAttachment);
					if(pOptions->bAttachment == TRUE) _ExpStr(m_pszNewLine);
				}

				_ExpStr(m_pszNewLine);
			}
			else if(m_nFormat == PWEXP_HTML)
			{
				_ExpStr(_T("<tr><td>"));

				_ExpSetSep(_T("</td><td>"));
				_ExpResetSkip();

				_ExpHtmlStrIf(pOptions->bGroup, pg->pszGroupName);
				_ExpStrIf(pOptions->bGroupTree, strGroupTree); // Is encoded already
				_ExpHtmlStrIf(pOptions->bTitle, p->pszTitle);
				_ExpHtmlStrIf(pOptions->bUserName, p->pszUserName);

				if((pOptions->bURL == TRUE) && (_tcslen(p->pszURL) != 0))
				{
					_ExpStrIf(pOptions->bURL, _T("<a href=\""));
					_ExpXmlStr(p->pszURL); // Use XML encoding, no &nbsp; when empty
					_ExpStr(_T("\">"));
					_ExpHtmlStr(p->pszURL);
					_ExpStr(_T("</a>"));
				}
				else _ExpHtmlStrIf(pOptions->bURL, _T(""));

				_ExpHtmlStrIf(pOptions->bPassword, p->pszPassword);
				CString strNotesConv = SU_ConvertNewLines(p->pszAdditional, m_pszNewLine);
				_ExpHtmlStrIf(pOptions->bNotes, strNotesConv);
				_ExpHtmlStrIf(pOptions->bUUID, strUUID);
				_ExpHtmlStrIf(pOptions->bImage, strImage);
				_ExpHtmlStrIf(pOptions->bCreationTime, strCreationTime);
				_ExpHtmlStrIf(pOptions->bLastAccTime, strLastAccTime);
				_ExpHtmlStrIf(pOptions->bLastModTime, strLastModTime);
				_ExpHtmlStrIf(pOptions->bExpireTime, strExpireTime);

				if(p->pszBinaryDesc[0] != 0)
					_ExpHtmlStrIf(pOptions->bAttachment, p->pszBinaryDesc);
				else
					_ExpHtmlStrIf(pOptions->bAttachment, _T(""));

				if((p->uBinaryDataLen != 0) && (pbEncodedAttachment != NULL))
					_ExpHtmlStrIf(pOptions->bAttachment, (LPCTSTR)pbEncodedAttachment);
				else
					_ExpHtmlStrIf(pOptions->bAttachment, _T(""));

				_ExpStr(_T("</td></tr>"));
				_ExpStr(m_pszNewLine);
			}
			else if(m_nFormat == PWEXP_XML)
			{
				_ExpSetSep(NULL);

				_ExpStr(_T("<pwentry>")); _ExpStr(m_pszNewLine);

				_ExpStrIf(pOptions->bGroup, _T("\t<group"));
				if((pOptions->bGroup == TRUE) && (strGroupTree.GetLength() != 0))
				{
					_ExpStrIf(pOptions->bGroupTree, _T(" tree=\""));
					_ExpStrIf(pOptions->bGroupTree, (LPCTSTR)strGroupTree); // Is encoded
					_ExpStrIf(pOptions->bGroupTree, _T("\""));
				}
				_ExpStrIf(pOptions->bGroup, _T(">"));
				_ExpXmlStrIf(pOptions->bGroup, pg->pszGroupName);
				_ExpStrIf(pOptions->bGroup, _T("</group>"));
				if(pOptions->bGroup == TRUE) _ExpStr(m_pszNewLine);

				_ExpStrIf(pOptions->bTitle, _T("\t<title>"));
				_ExpXmlStrIf(pOptions->bTitle, p->pszTitle);
				_ExpStrIf(pOptions->bTitle, _T("</title>"));
				if(pOptions->bTitle == TRUE) _ExpStr(m_pszNewLine);

				_ExpStrIf(pOptions->bUserName, _T("\t<username>"));
				_ExpXmlStrIf(pOptions->bUserName, p->pszUserName);
				_ExpStrIf(pOptions->bUserName, _T("</username>"));
				if(pOptions->bUserName == TRUE) _ExpStr(m_pszNewLine);

				_ExpStrIf(pOptions->bURL, _T("\t<url>"));
				_ExpXmlStrIf(pOptions->bURL, p->pszURL);
				_ExpStrIf(pOptions->bURL, _T("</url>"));
				if(pOptions->bURL == TRUE) _ExpStr(m_pszNewLine);

				_ExpStrIf(pOptions->bPassword, _T("\t<password>"));
				_ExpXmlStrIf(pOptions->bPassword, p->pszPassword);
				_ExpStrIf(pOptions->bPassword, _T("</password>"));
				if(pOptions->bPassword == TRUE) _ExpStr(m_pszNewLine);

				_ExpStrIf(pOptions->bNotes, _T("\t<notes>"));
				CString strNotesConv = SU_ConvertNewLines(p->pszAdditional, m_pszNewLine);
				_ExpXmlStrIf(pOptions->bNotes, strNotesConv);
				_ExpStrIf(pOptions->bNotes, _T("</notes>"));
				if(pOptions->bNotes == TRUE) _ExpStr(m_pszNewLine);

				_ExpStrIf(pOptions->bUUID, _T("\t<uuid>"));
				_ExpXmlStrIf(pOptions->bUUID, strUUID);
				_ExpStrIf(pOptions->bUUID, _T("</uuid>"));
				if(pOptions->bUUID == TRUE) _ExpStr(m_pszNewLine);

				_ExpStrIf(pOptions->bImage, _T("\t<image>"));
				_ExpXmlStrIf(pOptions->bImage, (LPCTSTR)strImage);
				_ExpStrIf(pOptions->bImage, _T("</image>"));
				if(pOptions->bImage == TRUE) _ExpStr(m_pszNewLine);

				_ExpStrIf(pOptions->bCreationTime, _T("\t<creationtime>"));
				_ExpXmlStrIf(pOptions->bCreationTime, (LPCTSTR)strCreationTime);
				_ExpStrIf(pOptions->bCreationTime, _T("</creationtime>"));
				if(pOptions->bCreationTime == TRUE) _ExpStr(m_pszNewLine);

				_ExpStrIf(pOptions->bLastModTime, _T("\t<lastmodtime>"));
				_ExpXmlStrIf(pOptions->bLastModTime, (LPCTSTR)strLastModTime);
				_ExpStrIf(pOptions->bLastModTime, _T("</lastmodtime>"));
				if(pOptions->bLastModTime == TRUE) _ExpStr(m_pszNewLine);

				_ExpStrIf(pOptions->bLastAccTime, _T("\t<lastaccesstime>"));
				_ExpXmlStrIf(pOptions->bLastAccTime, (LPCTSTR)strLastAccTime);
				_ExpStrIf(pOptions->bLastAccTime, _T("</lastaccesstime>"));
				if(pOptions->bLastAccTime == TRUE) _ExpStr(m_pszNewLine);

				if(memcmp(&p->tExpire, &tNever, sizeof(PW_TIME)) == 0)
					_ExpStrIf(pOptions->bExpireTime, _T("\t<expiretime expires=\"false\">"));
				else
					_ExpStrIf(pOptions->bExpireTime, _T("\t<expiretime expires=\"true\">"));

				_ExpXmlStrIf(pOptions->bExpireTime, (LPCTSTR)strExpireTime);
				_ExpStrIf(pOptions->bExpireTime, _T("</expiretime>"));
				if(pOptions->bExpireTime == TRUE) _ExpStr(m_pszNewLine);

				if(p->pszBinaryDesc[0] != 0)
				{
					_ExpStrIf(pOptions->bAttachment, _T("\t<attachdesc>"));
					_ExpXmlStrIf(pOptions->bAttachment, p->pszBinaryDesc);
					_ExpStrIf(pOptions->bAttachment, _T("</attachdesc>"));
					if(pOptions->bAttachment == TRUE) _ExpStr(m_pszNewLine);
				}

				if((p->uBinaryDataLen != 0) && (pbEncodedAttachment != NULL))
				{
					_ExpStrIf(pOptions->bAttachment, _T("\t<attachment>"));
					_ExpStrIf(pOptions->bAttachment, (LPCTSTR)pbEncodedAttachment);
					_ExpStrIf(pOptions->bAttachment, _T("</attachment>"));
					if(pOptions->bAttachment == TRUE) _ExpStr(m_pszNewLine);
				}

				_ExpStr(_T("</pwentry>"));
				_ExpStr(m_pszNewLine);
			}
			else if(m_nFormat == PWEXP_CSV)
			{
				_ExpStr(_T("\""));

				_ExpSetSep(_T("\",\""));
				_ExpResetSkip();

				_ExpCsvStrIf(pOptions->bGroup, pg->pszGroupName);
				_ExpCsvStrIf(pOptions->bGroupTree, strGroupTree);
				_ExpCsvStrIf(pOptions->bTitle, p->pszTitle);
				_ExpCsvStrIf(pOptions->bUserName, p->pszUserName);
				_ExpCsvStrIf(pOptions->bPassword, p->pszPassword);
				_ExpCsvStrIf(pOptions->bURL, p->pszURL);
				CString strNotesConv = SU_ConvertNewLines(p->pszAdditional, m_pszNewLine);
				_ExpCsvStrIf(pOptions->bNotes, strNotesConv);
				_ExpCsvStrIf(pOptions->bUUID, strUUID);
				_ExpCsvStrIf(pOptions->bImage, strImage);
				_ExpCsvStrIf(pOptions->bCreationTime, strCreationTime);
				_ExpCsvStrIf(pOptions->bLastAccTime, strLastAccTime);
				_ExpCsvStrIf(pOptions->bLastModTime, strLastModTime);
				_ExpCsvStrIf(pOptions->bExpireTime, strExpireTime);

				if(p->pszBinaryDesc[0] != 0)
					_ExpCsvStrIf(pOptions->bAttachment, p->pszBinaryDesc);
				else
					_ExpCsvStrIf(pOptions->bAttachment, _T(""));

				if((p->uBinaryDataLen != 0) && (pbEncodedAttachment != NULL))
					_ExpCsvStrIf(pOptions->bAttachment, (LPCTSTR)pbEncodedAttachment);
				else
					_ExpCsvStrIf(pOptions->bAttachment, _T(""));

				_ExpStr(_T("\""));
				_ExpStr(m_pszNewLine);
			}
			else if(m_nFormat == PWEXP_KEEPASS)
			{
				VERIFY(pStoreMgr->AddEntry(p));
			}
			else { ASSERT(FALSE); }

			SAFE_DELETE_ARRAY(pbEncodedAttachment);

			m_pMgr->LockEntryPassword(p);
		}
	}

	if(m_nFormat == PWEXP_TXT)
	{ // Nothing to do to finalize TXT
	}
	else if(m_nFormat == PWEXP_HTML)
	{
		_ExpStr(_T("</table></body></html>"));
		_ExpStr(m_pszNewLine);
	}
	else if(m_nFormat == PWEXP_XML)
	{
		_ExpStr(_T("</pwlist>"));
		_ExpStr(m_pszNewLine);
	}
	else if(m_nFormat == PWEXP_CSV)
	{ // Nothing to do to finalize CSV
	}
	else if(m_nFormat == PWEXP_KEEPASS)
	{
		if(pStoreMgr->SaveDatabase(pszFile, NULL) != PWE_SUCCESS) bReturn = FALSE;
	}
	else { ASSERT(FALSE); } // Unknown format, should never happen

	if(m_nFormat != PWEXP_KEEPASS)
	{
		ASSERT(fp != NULL);
		if(fp != NULL) fclose(fp);
		fp = NULL; m_fp = NULL;
	}

	aGroupIds.clear();
	return bReturn;
}
