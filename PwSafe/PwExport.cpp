/*
  Copyright (c) 2003, Dominik Reichl <dominik.reichl@t-online.de>
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
	if(bWindows == TRUE) m_pszNewLine = "\r\n";
	else m_pszNewLine = "\n";
}

#define PWEXPSTR(sp) { fwrite(sp, 1, strlen(sp), fp); }

BOOL CPwExport::ExportAll(const char *pszFile)
{
	ASSERT(pszFile != NULL);
	return ExportGroup(pszFile, -1);
}

BOOL CPwExport::ExportGroup(const char *pszFile, int nGroup)
{
	FILE *fp;
	char sz[256];
	unsigned long i;
	PW_ENTRY *p;
	PW_GROUP *pg;

	ASSERT(pszFile != NULL);
	if(pszFile == NULL) return FALSE;
	ASSERT((nGroup > -2) && (nGroup < (int)m_pMgr->GetNumberOfGroups()));
	fp = fopen(pszFile, "wb");
	if(fp == NULL) return FALSE;

	if(m_nFormat == PWEXP_TXT)
	{
		PWEXPSTR("Password List");
		PWEXPSTR(m_pszNewLine);
		PWEXPSTR(m_pszNewLine);
		PWEXPSTR("Title           UserName      URL              Password       Additional");
		PWEXPSTR(m_pszNewLine);
		PWEXPSTR("---------------|-------------|----------------|--------------|-----------------");
		PWEXPSTR(m_pszNewLine);
	}
	else if(m_nFormat == PWEXP_HTML)
	{
		PWEXPSTR("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">");
		PWEXPSTR(m_pszNewLine);
		PWEXPSTR("<html><head><title>Password List</title></head><body>");
		PWEXPSTR(m_pszNewLine);
		PWEXPSTR("<table width=\"100%\" border=\"1px\"><tr><td><b>Group</b></td><td><b>Title</b></td><td><b>UserName</b></td>");
		PWEXPSTR("<td><b>URL</b></td><td><b>Password</b></td><td><b>Notes</b></td></tr>");
		PWEXPSTR(m_pszNewLine);
	}
	else if(m_nFormat == PWEXP_XML)
	{
		PWEXPSTR("<?xml version=\"1.0\"?>");
		PWEXPSTR(m_pszNewLine);
		PWEXPSTR("<pwlist>");
		PWEXPSTR(m_pszNewLine);
	}
	else if(m_nFormat == PWEXP_CSV)
	{
		PWEXPSTR("\"Group\",\"Title\",\"UserName\",\"URL\",\"Password\",\"Notes\"");
		PWEXPSTR(m_pszNewLine);
	}
	else { ASSERT(FALSE); }

	for(i = 0; i < m_pMgr->GetNumberOfEntries(); i++)
	{
		p = m_pMgr->GetEntry(i);
		ASSERT(p != NULL);

		if((nGroup != -1) && (p->uGroupId != (DWORD)nGroup)) continue;

		pg = m_pMgr->GetGroup(p->uGroupId);
		ASSERT(pg != NULL);

		m_pMgr->UnlockEntryPassword(p);

		if(m_nFormat == PWEXP_TXT)
		{
			sprintf(sz, "%-15.15s %-13.13s %-16.16s %-14.14s %-17.17s",
				p->pszTitle, p->pszUserName, p->pszURL, p->pszPassword,
				p->pszAdditional);
			PWEXPSTR(sz);

			PWEXPSTR(m_pszNewLine);
		}
		else if(m_nFormat == PWEXP_HTML)
		{
			PWEXPSTR("<tr><td>");
			PWEXPSTR(pg->pszGroupName);
			PWEXPSTR("</td><td>");
			PWEXPSTR(p->pszTitle);
			PWEXPSTR("</td><td>");
			PWEXPSTR(p->pszUserName);
			PWEXPSTR("</td><td><a href=\"");
			PWEXPSTR(p->pszURL);
			PWEXPSTR("\">");
			PWEXPSTR(p->pszURL);
			PWEXPSTR("</a></td><td>");
			PWEXPSTR((const char *)p->pszPassword);
			PWEXPSTR("</td><td>");
			PWEXPSTR(p->pszAdditional);
			PWEXPSTR("</td></tr>");
			PWEXPSTR(m_pszNewLine);
		}
		else if(m_nFormat == PWEXP_XML)
		{
			PWEXPSTR("<pwentry>"); PWEXPSTR(m_pszNewLine);

			PWEXPSTR("\t<group>");
			PWEXPSTR(pg->pszGroupName);
			PWEXPSTR("</group>"); PWEXPSTR(m_pszNewLine);

			PWEXPSTR("\t<title>");
			PWEXPSTR(p->pszTitle);
			PWEXPSTR("</title>"); PWEXPSTR(m_pszNewLine);

			PWEXPSTR("\t<username>");
			PWEXPSTR(p->pszUserName);
			PWEXPSTR("</username>"); PWEXPSTR(m_pszNewLine);

			PWEXPSTR("\t<url>");
			PWEXPSTR(p->pszURL);
			PWEXPSTR("</url>"); PWEXPSTR(m_pszNewLine);

			PWEXPSTR("\t<password>");
			PWEXPSTR((const char *)p->pszPassword);
			PWEXPSTR("</password>"); PWEXPSTR(m_pszNewLine);

			PWEXPSTR("\t<notes>");
			PWEXPSTR(p->pszAdditional);
			PWEXPSTR("</notes>"); PWEXPSTR(m_pszNewLine);

			PWEXPSTR("</pwentry>"); PWEXPSTR(m_pszNewLine);
		}
		else if(m_nFormat == PWEXP_CSV)
		{
			PWEXPSTR("\"");
			PWEXPSTR(pg->pszGroupName);
			PWEXPSTR("\",\"");
			PWEXPSTR(p->pszTitle);
			PWEXPSTR("\",\"");
			PWEXPSTR(p->pszUserName);
			PWEXPSTR("\",\"");
			PWEXPSTR(p->pszURL);
			PWEXPSTR("\",\"");
			PWEXPSTR((const char *)p->pszPassword);
			PWEXPSTR("\",\"");
			PWEXPSTR(p->pszAdditional);
			PWEXPSTR("\"");
			PWEXPSTR(m_pszNewLine);
		}
		else { ASSERT(FALSE); }

		m_pMgr->LockEntryPassword(p);
	}

	if(m_nFormat == PWEXP_TXT)
	{ // Nothing to finalize TXT
	}
	else if(m_nFormat == PWEXP_HTML)
	{
		PWEXPSTR("</table></body></html>");
		PWEXPSTR(m_pszNewLine);
	}
	else if(m_nFormat == PWEXP_XML)
	{
		PWEXPSTR("</pwlist>");
		PWEXPSTR(m_pszNewLine);
	}
	else if(m_nFormat == PWEXP_CSV)
	{ // Nothing to finalize CSV
	}
	else { ASSERT(FALSE); }

	fclose(fp);

	return TRUE;
}
