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

#ifndef AFX_LANGUAGESDLG_H__339B6A40_1777_11D8_BF16_0050BF14F5CC__INCLUDED_
#define AFX_LANGUAGESDLG_H__339B6A40_1777_11D8_BF16_0050BF14F5CC__INCLUDED_

/////////////////////////////////////////////////////////////////////////////

#include "Util/SysDefEx.h"
#include "NewGUI/KCSideBannerWnd.h"
#include "NewGUI/XPStyleButtonST.h"

class CPP_CLASS_SHARE CLanguagesDlg : public CDialog
{
public:
	CLanguagesDlg(CWnd* pParent = NULL);

	void _LoadLanguage(char *szLang);

	CKCSideBannerWnd m_banner;
	CImageList m_ilIcons;

	//{{AFX_DATA(CLanguagesDlg)
	enum { IDD = IDD_LANGUAGES_DLG };
	CXPStyleButtonST	m_btGetLang;
	CListCtrl	m_listLang;
	CXPStyleButtonST	m_btClose;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CLanguagesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CLanguagesDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClickLanguagesList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnGetLanguage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // AFX_LANGUAGESDLG_H__339B6A40_1777_11D8_BF16_0050BF14F5CC__INCLUDED_
