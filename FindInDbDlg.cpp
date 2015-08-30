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
#include "PwSafe.h"
#include "FindInDbDlg.h"

#include "NewGUI/TranslateEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CFindInDbDlg::CFindInDbDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindInDbDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindInDbDlg)
	m_bAdditional = TRUE;
	m_bPassword = TRUE;
	m_bTitle = TRUE;
	m_bURL = TRUE;
	m_bUserName = TRUE;
	m_strFind = _T("");
	m_bCaseSensitive = FALSE;
	m_bGroupName = FALSE;
	//}}AFX_DATA_INIT
}

void CFindInDbDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindInDbDlg)
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Check(pDX, IDC_CHECK_NOTES, m_bAdditional);
	DDX_Check(pDX, IDC_CHECK_PASSWORD, m_bPassword);
	DDX_Check(pDX, IDC_CHECK_TITLE, m_bTitle);
	DDX_Check(pDX, IDC_CHECK_URL, m_bURL);
	DDX_Check(pDX, IDC_CHECK_USERNAME, m_bUserName);
	DDX_Text(pDX, IDC_EDIT_FINDTEXT, m_strFind);
	DDX_Check(pDX, IDC_CHECK_CASESENSITIVE, m_bCaseSensitive);
	DDX_Check(pDX, IDC_CHECK_GROUPNAME, m_bGroupName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFindInDbDlg, CDialog)
	//{{AFX_MSG_MAP(CFindInDbDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CFindInDbDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	NewGUI_Button(&m_btOK, IDB_OK, IDB_OK);
	NewGUI_Button(&m_btCancel, IDB_CANCEL, IDB_CANCEL);

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_SEARCH),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(TRL("Find"));
	m_banner.SetCaption(TRL("Find a string in the password list"));

	GetDlgItem(IDC_EDIT_FINDTEXT)->SetFocus();
	return FALSE; // Return TRUE unless you set the focus to a control
}

void CFindInDbDlg::OnOK() 
{
	UpdateData(TRUE);
	CDialog::OnOK();
}

void CFindInDbDlg::OnCancel() 
{
	CDialog::OnCancel();
}
