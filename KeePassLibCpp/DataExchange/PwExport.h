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

#ifndef ___PW_EXPORT_H___
#define ___PW_EXPORT_H___

#pragma once

#include "../PwManager.h"
#include "../SysDefEx.h"
#include <stdio.h>
#include <boost/utility.hpp>

#define PWEXP_NULL    0
#define PWEXP_TXT     1
#define PWEXP_HTML    2
#define PWEXP_XML     3
#define PWEXP_CSV     4
#define PWEXP_KEEPASS 5
#define PWEXP_LAST    6

typedef struct
{
	BOOL bGroup;
	BOOL bGroupTree;
	BOOL bTitle;
	BOOL bUserName;
	BOOL bURL;
	BOOL bPassword;
	BOOL bNotes;
	BOOL bUUID;
	BOOL bImage;
	BOOL bCreationTime;
	BOOL bLastAccTime;
	BOOL bLastModTime;
	BOOL bExpireTime;
	BOOL bAttachment;

	BOOL bEncodeNewlines;
	BOOL bExportBackups;
} PWEXPORT_OPTIONS;

class CPwExport : boost::noncopyable
{
public:
	CPwExport();
	virtual ~CPwExport();

	void SetManager(CPwManager *pMgr);
	void SetFormat(int nFormat);
	void SetNewLineSeq(BOOL bWindows);

	BOOL ExportAll(const TCHAR *pszFile, const PWEXPORT_OPTIONS *pOptions, CPwManager *pStoreMgr);
	BOOL ExportGroup(const TCHAR *pszFile, DWORD dwGroupId, const PWEXPORT_OPTIONS *pOptions, CPwManager *pStoreMgr);

	CString MakeGroupTreeString(DWORD dwGroupId, bool bXmlEncode) const;

	PWEXPORT_OPTIONS m_aDefaults[PWEXP_LAST];
	int m_nFormat;

private:
	void _ExpStr(LPCTSTR lpString);
	void _ExpXmlStr(LPCTSTR lpString);
	void _ExpHtmlStr(LPCTSTR lpString);

	void _ExpResetSkip();
	void _ExpSetSep(LPCTSTR lpSep);
	void _ExpStrIf(BOOL bCondition, LPCTSTR lpString);
	void _ExpXmlStrIf(BOOL bCondition, LPCTSTR lpString);
	void _ExpHtmlStrIf(BOOL bCondition, LPCTSTR lpString);
	void _ExpCsvStrIf(BOOL bCondition, LPCTSTR lpString);

	CPwManager *m_pMgr;
	TCHAR *m_pszNewLine;

	FILE *m_fp;
	BOOL m_bOneSkipped;
	LPCTSTR m_lpSep;
	const PWEXPORT_OPTIONS *m_pOptions;
};

#endif
