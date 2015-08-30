/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2007 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "PwSafe.h"
#include "TanWizardDlg.h"

#include "NewGUI/NewGUICommon.h"
#include "Util/WinUtil.h"
#include "Util/PrivateConfigEx.h"
#include "../KeePassLibCpp/PwManager.h"
#include "../KeePassLibCpp/Util/TranslateEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CTanWizardDlg::CTanWizardDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTanWizardDlg::IDD, pParent)
	, m_strTANChars(_T(""))
{
	//{{AFX_DATA_INIT(CTanWizardDlg)
	m_strTans = _T("");
	m_bAssignNumbers = FALSE;
	m_dwStartNumber = 0;
	//}}AFX_DATA_INIT
}

void CTanWizardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTanWizardDlg)
	DDX_Control(pDX, IDC_TANHELP_BTN, m_btHelp);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Text(pDX, IDC_EDIT_TANS, m_strTans);
	DDX_Check(pDX, IDC_CHECK_NUMBERING, m_bAssignNumbers);
	DDX_Text(pDX, IDC_EDIT_NUMBERING_START, m_dwStartNumber);
	DDX_Text(pDX, IDC_EDIT_TANCHARS, m_strTANChars);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTanWizardDlg, CDialog)
	//{{AFX_MSG_MAP(CTanWizardDlg)
	ON_BN_CLICKED(IDC_CHECK_NUMBERING, OnCheckNumbering)
	ON_BN_CLICKED(IDC_TANHELP_BTN, OnTANHelpBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CTanWizardDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	NewGUI_XPButton(&m_btOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(&m_btCancel, IDB_CANCEL, IDB_CANCEL);
	NewGUI_XPButton(&m_btHelp, IDB_HELP_SMALL, IDB_HELP_SMALL);

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_OPTIONS),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(TRL("TAN Wizard"));
	m_banner.SetCaption(TRL("Using this wizard you can easily add TAN entries."));

	GetDlgItem(IDC_EDIT_NUMBERING_START)->EnableWindow(FALSE);

	TCHAR lpTANChars[SI_REGSIZE + 1];
	CPrivateConfigEx cfg(FALSE);
	if(cfg.Get(PWMKEY_TANCHARS, lpTANChars) == TRUE) m_strTANChars = lpTANChars;
	else m_strTANChars = TW_DEFAULTCHARS;

	UpdateData(FALSE);

	GetDlgItem(IDC_EDIT_TANS)->SetFocus();
	return FALSE;
}

void CTanWizardDlg::OnOK() 
{
	CleanUpEx();
	CDialog::OnOK();
}

void CTanWizardDlg::OnCancel() 
{
	CleanUpEx();
	m_strTans = _T("");
	CDialog::OnCancel();
}

void CTanWizardDlg::CleanUpEx()
{
	UpdateData(TRUE);

	CPrivateConfigEx cfg(TRUE);
	cfg.Set(PWMKEY_TANCHARS, m_strTANChars);
}

void CTanWizardDlg::OnCheckNumbering() 
{
	UpdateData(TRUE);

	if(m_bAssignNumbers == TRUE)
		GetDlgItem(IDC_EDIT_NUMBERING_START)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_EDIT_NUMBERING_START)->EnableWindow(FALSE);
}

void CTanWizardDlg::OnTANHelpBtn()
{
	WU_OpenAppHelp(PWM_HELP_TANS);
}
