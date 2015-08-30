/*
  Copyright (c) 2003/2004, Dominik Reichl <dominik.reichl@t-online.de>
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer. 
  - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  - Neither the name of ReichlSoft nor the names of its contributors may be
    used to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#include "StdAfx.h"
#include "PwExport.h"

#include "../Util/MemUtil.h"
#include "../Util/StrUtil.h"
#include "../Util/base64.h"
#include "../NewGUI/TranslateEx.h"

CPwExport::CPwExport()
{
	m_pMgr = NULL;
	m_nFormat = 0;

	SetNewLineSeq(false);
}

CPwExport::~CPwExport()
{
	m_pMgr = NULL;
	m_nFormat = 0;
}

void CPwExport::SetManager(CPwManager *pMgr)
{
	ASSERT(pMgr != NULL);
	if(pMgr != NULL) m_pMgr = pMgr;
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

#define PWEXPSTR(sp) \
{ \
	ASSERT(sp != NULL); if(sp != NULL) { \
	UTF8_BYTE *_pUtf8String = _StringToUTF8(sp); \
	fwrite(_pUtf8String, 1, strlen((char *)_pUtf8String), fp); \
	SAFE_DELETE_ARRAY(_pUtf8String); } \
}

#define PWEXPSTRXML(sp) \
{ \
	ASSERT(sp != NULL); if(sp != NULL) { \
	TCHAR *_pXmlString = MakeSafeXmlString(sp); \
	UTF8_BYTE *_pUtf8String = _StringToUTF8(_pXmlString); \
	fwrite(_pUtf8String, 1, strlen((char *)_pUtf8String), fp); \
	SAFE_DELETE_ARRAY(_pUtf8String); \
	SAFE_DELETE_ARRAY(_pXmlString); } \
}

BOOL CPwExport::ExportAll(const TCHAR *pszFile)
{
	ASSERT(pszFile != NULL);
	return ExportGroup(pszFile, DWORD_MAX);
}

BOOL CPwExport::ExportGroup(const TCHAR *pszFile, DWORD dwGroupId)
{
	FILE *fp;
	unsigned long i;
	PW_ENTRY *p;
	PW_GROUP *pg;
	BYTE aInitUTF8[3] = { 0xEF, 0xBB, 0xBF };
	CString str;
	CBase64Codec base64;

	ASSERT(pszFile != NULL);
	if(pszFile == NULL) return FALSE;
	fp = _tfopen(pszFile, _T("wb"));
	if(fp == NULL) return FALSE;

	fwrite(aInitUTF8, 1, 3, fp);

	if(m_nFormat == PWEXP_TXT)
	{
	}
	else if(m_nFormat == PWEXP_HTML)
	{
		PWEXPSTR(_T("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">"));
		PWEXPSTR(m_pszNewLine);
		PWEXPSTR(_T("<html><head>"));
		PWEXPSTR(_T("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">"));
		PWEXPSTR(_T("<title>"));
		PWEXPSTR(TRL("Password List"));
		PWEXPSTR(_T("</title></head><body>"));
		PWEXPSTR(m_pszNewLine);
		PWEXPSTR(_T("<h1>"));
		PWEXPSTR(TRL("Password List"));
		PWEXPSTR(_T("</h1>"));
		PWEXPSTR(m_pszNewLine);
		PWEXPSTR(_T("<table width=\"100%\" border=\"1px\" cellspacing=\"0\" cellpadding=\"1\"><tr><th>"));
		PWEXPSTR(TRL("Group:"));
		PWEXPSTR(_T("</th><th>"));
		PWEXPSTR(TRL("Title"));
		PWEXPSTR(_T(":</th><th>"));
		PWEXPSTR(TRL("UserName"));
		PWEXPSTR(_T(":</th><th>"));
		PWEXPSTR(TRL("URL"));
		PWEXPSTR(_T(":</th><th>"));
		PWEXPSTR(TRL("Password"));
		PWEXPSTR(_T(":</th><th>"));
		PWEXPSTR(TRL("Notes"));
		PWEXPSTR(_T(":</th></tr>"));
		PWEXPSTR(m_pszNewLine);
	}
	else if(m_nFormat == PWEXP_XML)
	{
		PWEXPSTR(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));
		PWEXPSTR(m_pszNewLine);
		PWEXPSTR(_T("<pwlist>"));
		PWEXPSTR(m_pszNewLine);
	}
	else if(m_nFormat == PWEXP_CSV)
	{
		PWEXPSTR(_T("\"Account\",\"Login Name\",\"Password\",\"Web Site\",\"Comments\""));
		PWEXPSTR(m_pszNewLine);
	}
	else { ASSERT(FALSE); }

	unsigned long uNumEntries = (unsigned long)m_pMgr->GetNumberOfEntries();

	for(i = 0; i < uNumEntries; i++)
	{
		p = m_pMgr->GetEntry(i);
		ASSERT_ENTRY(p);

		// Are we exporting this group?
		if((dwGroupId != DWORD_MAX) && (p->uGroupId != dwGroupId)) continue;

		pg = m_pMgr->GetGroupById(p->uGroupId);
		ASSERT(pg != NULL); if(pg == NULL) continue;

		m_pMgr->UnlockEntryPassword(p);

		if(m_nFormat == PWEXP_TXT)
		{
			PWEXPSTR(_T("["));
			PWEXPSTR(p->pszTitle);
			PWEXPSTR(_T("]"));
			PWEXPSTR(m_pszNewLine);
			PWEXPSTR(TRL("Group:"));
			PWEXPSTR(_T(" "));
			PWEXPSTR(pg->pszGroupName);
			PWEXPSTR(m_pszNewLine);
			PWEXPSTR(TRL("UserName:"));
			PWEXPSTR(_T(" "));
			PWEXPSTR(p->pszUserName);
			PWEXPSTR(m_pszNewLine);
			PWEXPSTR(TRL("URL:"));
			PWEXPSTR(_T(" "));
			PWEXPSTR(p->pszURL);
			PWEXPSTR(m_pszNewLine);
			PWEXPSTR(TRL("Password:"));
			PWEXPSTR(_T(" "));
			PWEXPSTR(p->pszPassword);
			PWEXPSTR(m_pszNewLine);
			PWEXPSTR(TRL("Notes:"));
			PWEXPSTR(_T(" "));
			PWEXPSTR(p->pszAdditional);
			PWEXPSTR(m_pszNewLine);
			PWEXPSTR(m_pszNewLine);
		}
		else if(m_nFormat == PWEXP_HTML)
		{
			PWEXPSTR(_T("<tr><td>"));
			if(_tcslen(pg->pszGroupName) != 0) PWEXPSTRXML(pg->pszGroupName)
			else PWEXPSTR(_T("&nbsp;"));
			PWEXPSTR(_T("</td><td>"));
			if(_tcslen(p->pszTitle) != 0) PWEXPSTRXML(p->pszTitle)
			else PWEXPSTR(_T("&nbsp;"));
			PWEXPSTR(_T("</td><td>"));
			if(_tcslen(p->pszUserName) != 0) PWEXPSTRXML(p->pszUserName)
			else PWEXPSTR(_T("&nbsp;"));
			PWEXPSTR(_T("</td><td>"));
			if(_tcslen(p->pszURL) != 0)
			{
				PWEXPSTR(_T("<a href=\""));
				PWEXPSTRXML(p->pszURL);
				PWEXPSTR(_T("\">"));
				PWEXPSTRXML(p->pszURL);
				PWEXPSTR(_T("</a>"));
			}
			else PWEXPSTR(_T("&nbsp;"));
			PWEXPSTR(_T("</td><td>"));
			if(_tcslen(p->pszPassword) != 0) PWEXPSTRXML(p->pszPassword)
			else PWEXPSTR(_T("&nbsp;"));
			PWEXPSTR(_T("</td><td>"));
			if(_tcslen(p->pszAdditional) != 0) PWEXPSTRXML(p->pszAdditional)
			else PWEXPSTR(_T("&nbsp;"));
			PWEXPSTR(_T("</td></tr>"));
			PWEXPSTR(m_pszNewLine);
		}
		else if(m_nFormat == PWEXP_XML)
		{
			PWEXPSTR(_T("<pwentry>")); PWEXPSTR(m_pszNewLine);

			PWEXPSTR(_T("\t<group>"));
			PWEXPSTRXML(pg->pszGroupName);
			PWEXPSTR(_T("</group>")); PWEXPSTR(m_pszNewLine);

			PWEXPSTR(_T("\t<title>"));
			PWEXPSTRXML(p->pszTitle);
			PWEXPSTR(_T("</title>")); PWEXPSTR(m_pszNewLine);

			PWEXPSTR(_T("\t<username>"));
			PWEXPSTRXML(p->pszUserName);
			PWEXPSTR(_T("</username>")); PWEXPSTR(m_pszNewLine);

			PWEXPSTR(_T("\t<url>"));
			PWEXPSTRXML(p->pszURL);
			PWEXPSTR(_T("</url>")); PWEXPSTR(m_pszNewLine);

			PWEXPSTR(_T("\t<password>"));
			PWEXPSTRXML(p->pszPassword);
			PWEXPSTR(_T("</password>")); PWEXPSTR(m_pszNewLine);

			PWEXPSTR(_T("\t<notes>"));
			PWEXPSTRXML(p->pszAdditional);
			PWEXPSTR(_T("</notes>")); PWEXPSTR(m_pszNewLine);

			PWEXPSTR(_T("\t<uuid>"));
			_UuidToString(p->uuid, &str);
			PWEXPSTR((LPCTSTR)str);
			PWEXPSTR(_T("</uuid>")); PWEXPSTR(m_pszNewLine);

			PWEXPSTR(_T("\t<image>"));
			str.Format(_T("%u"), p->uImageId);
			PWEXPSTR((LPCTSTR)str);
			PWEXPSTR(_T("</image>")); PWEXPSTR(m_pszNewLine);

			PWEXPSTR(_T("\t<creationtime>"));
			_PwTimeToXmlTime(p->tCreation, &str);
			PWEXPSTR((LPCTSTR)str);
			PWEXPSTR(_T("</creationtime>")); PWEXPSTR(m_pszNewLine);

			PWEXPSTR(_T("\t<lastmodtime>"));
			_PwTimeToXmlTime(p->tLastMod, &str);
			PWEXPSTR((LPCTSTR)str);
			PWEXPSTR(_T("</lastmodtime>")); PWEXPSTR(m_pszNewLine);

			PWEXPSTR(_T("\t<lastaccesstime>"));
			_PwTimeToXmlTime(p->tLastAccess, &str);
			PWEXPSTR((LPCTSTR)str);
			PWEXPSTR(_T("</lastaccesstime>")); PWEXPSTR(m_pszNewLine);

			PWEXPSTR(_T("\t<expiretime>"));
			_PwTimeToXmlTime(p->tExpire, &str);
			PWEXPSTR((LPCTSTR)str);
			PWEXPSTR(_T("</expiretime>")); PWEXPSTR(m_pszNewLine);

			if(_tcslen(p->pszBinaryDesc) != 0)
			{
				PWEXPSTR(_T("\t<attachdesc>"));
				PWEXPSTRXML(p->pszBinaryDesc);
				PWEXPSTR(_T("</attachdesc>")); PWEXPSTR(m_pszNewLine);
			}

			if(p->uBinaryDataLen != 0)
			{
				DWORD dwBufSize = (((p->uBinaryDataLen + 2) / 3) << 2) + 1;
				BYTE *pTempBuf = new BYTE[dwBufSize + 1];

				if(base64.Encode(p->pBinaryData, p->uBinaryDataLen, pTempBuf, &dwBufSize) == true)
				{
					PWEXPSTR(_T("\t<attachment>"));
					PWEXPSTR((TCHAR *)pTempBuf);
					PWEXPSTR(_T("</attachment>")); PWEXPSTR(m_pszNewLine);
				}
				else { ASSERT(FALSE); }

				SAFE_DELETE_ARRAY(pTempBuf);
			}

			PWEXPSTR(_T("</pwentry>")); PWEXPSTR(m_pszNewLine);
		}
		else if(m_nFormat == PWEXP_CSV)
		{
			PWEXPSTR(_T("\""));
			PWEXPSTR(p->pszTitle);
			PWEXPSTR(_T("\",\""));
			PWEXPSTR(p->pszUserName);
			PWEXPSTR(_T("\",\""));
			PWEXPSTR(p->pszPassword);
			PWEXPSTR(_T("\",\""));
			PWEXPSTR(p->pszURL);
			PWEXPSTR(_T("\",\""));
			PWEXPSTR(p->pszAdditional);
			PWEXPSTR(_T("\""));
			PWEXPSTR(m_pszNewLine);
		}
		else { ASSERT(FALSE); }

		m_pMgr->LockEntryPassword(p);
	}

	if(m_nFormat == PWEXP_TXT)
	{ // Nothing to do to finalize TXT
	}
	else if(m_nFormat == PWEXP_HTML)
	{
		PWEXPSTR(_T("</table></body></html>"));
		PWEXPSTR(m_pszNewLine);
	}
	else if(m_nFormat == PWEXP_XML)
	{
		PWEXPSTR(_T("</pwlist>"));
		PWEXPSTR(m_pszNewLine);
	}
	else if(m_nFormat == PWEXP_CSV)
	{ // Nothing to finalize CSV
	}
	else { ASSERT(FALSE); } // Unknown format, should never happen

	fclose(fp); fp = NULL;
	return TRUE;
}
