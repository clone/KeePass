/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2009 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef AFX_LANGUAGESDLG_H__339B6A40_1777_11D8_BF16_0050BF14F5CC__INCLUDED_
#define AFX_LANGUAGESDLG_H__339B6A40_1777_11D8_BF16_0050BF14F5CC__INCLUDED_

/////////////////////////////////////////////////////////////////////////////

#include "NewGUI/KCSideBannerWnd.h"
#include "NewGUI/XPStyleButtonST.h"

#define PWMX_ENGLISH_AUTHOR  _T("Dominik Reichl")
#define PWMX_ENGLISH_CONTACT _T("dominik.reichl@t-online.de, http://www.dominik-reichl.de/")

class CLanguagesDlg : public CDialog
{
public:
	CLanguagesDlg(CWnd* pParent = NULL);

	void _LoadLanguage(LPCTSTR szLang);

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
