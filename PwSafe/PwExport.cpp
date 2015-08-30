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

#ifdef _UNICODE
#define PWEXPSTR(sp) \
{ \
	char *_pa = m_pMgr->_ToAscii(sp); \
	fwrite(_pa, 1, strlen(_pa), fp); \
	SAFE_DELETE_ARRAY(_pa); \
}
#else
#define PWEXPSTR(sp) { fwrite(sp, 1, strlen(sp), fp); }
#endif

BOOL CPwExport::ExportAll(const TCHAR *pszFile)
{
	ASSERT(pszFile != NULL);
	return ExportGroup(pszFile, DWORD_MAX);
}

BOOL CPwExport::ExportGroup(const TCHAR *pszFile, DWORD dwGroupId)
{
	FILE *fp;
	TCHAR sz[256];
	unsigned long i;
	PW_ENTRY *p;
	PW_GROUP *pg;

	ASSERT(pszFile != NULL);
	if(pszFile == NULL) return FALSE;
	fp = _tfopen(pszFile, _T("wb"));
	if(fp == NULL) return FALSE;

	if(m_nFormat == PWEXP_TXT)
	{
		PWEXPSTR(_T("Password List"));
		PWEXPSTR(m_pszNewLine);
		PWEXPSTR(m_pszNewLine);
		PWEXPSTR(_T("Title           UserName      URL              Password       Additional"));
		PWEXPSTR(m_pszNewLine);
		PWEXPSTR(_T("---------------|-------------|----------------|--------------|-----------------"));
		PWEXPSTR(m_pszNewLine);
	}
	else if(m_nFormat == PWEXP_HTML)
	{
		PWEXPSTR(_T("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">"));
		PWEXPSTR(m_pszNewLine);
		PWEXPSTR(_T("<html><head><title>Password List</title></head><body>"));
		PWEXPSTR(m_pszNewLine);
		PWEXPSTR(_T("<table width=\"100%\" border=\"1px\"><tr><td><b>Group</b></td><td><b>Title</b></td><td><b>UserName</b></td>"));
		PWEXPSTR(_T("<td><b>URL</b></td><td><b>Password</b></td><td><b>Notes</b></td></tr>"));
		PWEXPSTR(m_pszNewLine);
	}
	else if(m_nFormat == PWEXP_XML)
	{
		PWEXPSTR(_T("<?xml version=\"1.0\"?>"));
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

	for(i = 0; i < m_pMgr->GetNumberOfEntries(); i++)
	{
		p = m_pMgr->GetEntry(i);
		ASSERT_ENTRY(p);

		if((dwGroupId != DWORD_MAX) && (p->uGroupId != dwGroupId)) continue;

		pg = m_pMgr->GetGroup(m_pMgr->GetGroupByIdN(p->uGroupId));
		ASSERT(pg != NULL);

		m_pMgr->UnlockEntryPassword(p);

		if(m_nFormat == PWEXP_TXT)
		{
			_stprintf(sz, _T("%-15.15s %-13.13s %-16.16s %-14.14s %-17.17s"),
				p->pszTitle, p->pszUserName, p->pszURL, p->pszPassword,
				p->pszAdditional);
			PWEXPSTR(sz);

			PWEXPSTR(m_pszNewLine);
		}
		else if(m_nFormat == PWEXP_HTML)
		{
			PWEXPSTR(_T("<tr><td>"));
			PWEXPSTR(pg->pszGroupName);
			PWEXPSTR(_T("</td><td>"));
			PWEXPSTR(p->pszTitle);
			PWEXPSTR(_T("</td><td>"));
			PWEXPSTR(p->pszUserName);
			PWEXPSTR(_T("</td><td><a href=\""));
			PWEXPSTR(p->pszURL);
			PWEXPSTR(_T("\">"));
			PWEXPSTR(p->pszURL);
			PWEXPSTR(_T("</a></td><td>"));
			PWEXPSTR(p->pszPassword);
			PWEXPSTR(_T("</td><td>"));
			PWEXPSTR(p->pszAdditional);
			PWEXPSTR(_T("</td></tr>"));
			PWEXPSTR(m_pszNewLine);
		}
		else if(m_nFormat == PWEXP_XML)
		{
			PWEXPSTR(_T("<pwentry>")); PWEXPSTR(m_pszNewLine);

			PWEXPSTR(_T("\t<group>"));
			PWEXPSTR(pg->pszGroupName);
			PWEXPSTR(_T("</group>")); PWEXPSTR(m_pszNewLine);

			PWEXPSTR(_T("\t<title>"));
			PWEXPSTR(p->pszTitle);
			PWEXPSTR(_T("</title>")); PWEXPSTR(m_pszNewLine);

			PWEXPSTR(_T("\t<username>"));
			PWEXPSTR(p->pszUserName);
			PWEXPSTR(_T("</username>")); PWEXPSTR(m_pszNewLine);

			PWEXPSTR(_T("\t<url>"));
			PWEXPSTR(p->pszURL);
			PWEXPSTR(_T("</url>")); PWEXPSTR(m_pszNewLine);

			PWEXPSTR(_T("\t<password>"));
			PWEXPSTR(p->pszPassword);
			PWEXPSTR(_T("</password>")); PWEXPSTR(m_pszNewLine);

			PWEXPSTR(_T("\t<notes>"));
			PWEXPSTR(p->pszAdditional);
			PWEXPSTR(_T("</notes>")); PWEXPSTR(m_pszNewLine);

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
