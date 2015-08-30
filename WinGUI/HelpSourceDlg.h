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

#ifndef ___HELP_SOURCE_DLG_H___
#define ___HELP_SOURCE_DLG_H___

#pragma once

#include <afxwin.h>

#include "NewGUI/XPStyleButtonST.h"
#include "NewGUI/KCSideBannerWnd.h"

class CHelpSourceDlg : public CDialog
{
	DECLARE_DYNAMIC(CHelpSourceDlg)

private:
	void _CleanUpEx();

	CKCSideBannerWnd m_banner;
	// CFont* m_pBoldFont;

public:
	CHelpSourceDlg(CWnd* pParent = NULL);
	virtual ~CHelpSourceDlg();

	enum { IDD = IDD_HELPSOURCE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();

public:
	CButton m_rbLocalHelp;
	CButton m_rbOnlineHelp;
	afx_msg void OnBnClickedOK();
	afx_msg void OnBnClickedCancel();
	CXPStyleButtonST m_btnOK;
	CXPStyleButtonST m_btnCancel;
};

#endif // ___HELP_SOURCE_DLG_H___
