/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2014 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef AFX_TANWIZARDDLG_H__C738F700_624E_11D8_BF16_0050BF14F5CC__INCLUDED_
#define AFX_TANWIZARDDLG_H__C738F700_624E_11D8_BF16_0050BF14F5CC__INCLUDED_

#include "NewGUI/XPStyleButtonST.h"
#include "NewGUI/KCSideBannerWnd.h"
#include "../KeePassLibCpp/SysDefEx.h"

#define TW_DEFAULTCHARS _T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-")

/////////////////////////////////////////////////////////////////////////////

class CTanWizardDlg : public CDialog
{
public:
	CTanWizardDlg(CWnd* pParent = NULL);

	CString m_strGroupName;

private:
	void CleanUpEx();

	CKCSideBannerWnd m_banner;

public:
	//{{AFX_DATA(CTanWizardDlg)
	enum { IDD = IDD_TANWIZARD_DLG };
	CXPStyleButtonST	m_btHelp;
	CXPStyleButtonST	m_btCancel;
	CXPStyleButtonST	m_btOK;
	CString	m_strTans;
	BOOL	m_bAssignNumbers;
	DWORD	m_dwStartNumber;
	CString m_strTANChars;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CTanWizardDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CTanWizardDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnTANHelpBtn();
	afx_msg void OnCheckNumbering();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // AFX_TANWIZARDDLG_H__C738F700_624E_11D8_BF16_0050BF14F5CC__INCLUDED_
