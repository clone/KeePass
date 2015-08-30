/*
  Copyright (c) 2003-2005, Dominik Reichl <dominik.reichl@t-online.de>
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

#ifndef ___PW_EXPORT_H___
#define ___PW_EXPORT_H___

#include "PwManager.h"
#include "../Util/SysDefEx.h"

#define PWEXP_NULL 0
#define PWEXP_TXT  1
#define PWEXP_HTML 2
#define PWEXP_XML  3
#define PWEXP_CSV  4
#define PWEXP_LAST 5

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

class CPwExport
{
public:
	CPwExport();
	virtual ~CPwExport();

	void SetManager(CPwManager *pMgr);
	void SetFormat(int nFormat);
	void SetNewLineSeq(BOOL bWindows);

	BOOL ExportAll(const TCHAR *pszFile, const PWEXPORT_OPTIONS *pOptions);
	BOOL ExportGroup(const TCHAR *pszFile, DWORD dwGroupId, const PWEXPORT_OPTIONS *pOptions);

	CString MakeGroupTreeString(DWORD dwGroupId);

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
