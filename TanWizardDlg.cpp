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

#include "StdAfx.h"
#include "PwSafe.h"
#include "TanWizardDlg.h"

#include "NewGUI/TranslateEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CTanWizardDlg::CTanWizardDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTanWizardDlg::IDD, pParent)
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
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Text(pDX, IDC_EDIT_TANS, m_strTans);
	DDX_Check(pDX, IDC_CHECK_NUMBERING, m_bAssignNumbers);
	DDX_Text(pDX, IDC_EDIT_NUMBERING_START, m_dwStartNumber);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTanWizardDlg, CDialog)
	//{{AFX_MSG_MAP(CTanWizardDlg)
	ON_BN_CLICKED(IDC_CHECK_NUMBERING, OnCheckNumbering)
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

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_OPTIONS),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(TRL("TAN Wizard"));
	m_banner.SetCaption(TRL("Using this wizard you can easily add TAN entries."));

	GetDlgItem(IDC_EDIT_NUMBERING_START)->EnableWindow(FALSE);

	return TRUE;
}

void CTanWizardDlg::OnOK() 
{
	UpdateData(TRUE);

	CDialog::OnOK();
}

void CTanWizardDlg::OnCancel() 
{
	CDialog::OnCancel();
}

void CTanWizardDlg::OnCheckNumbering() 
{
	UpdateData(TRUE);

	if(m_bAssignNumbers == TRUE)
		GetDlgItem(IDC_EDIT_NUMBERING_START)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_EDIT_NUMBERING_START)->EnableWindow(FALSE);
}
