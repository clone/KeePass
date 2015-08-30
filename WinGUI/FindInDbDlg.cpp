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
#include "FindInDbDlg.h"

#include "NewGUI/NewGUICommon.h"
#include "../KeePassLibCpp/Util/TranslateEx.h"

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
	m_bExcludeBackups = TRUE;
	//}}AFX_DATA_INIT

	m_lpGroupName = NULL;
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
	DDX_Check(pDX, IDC_CHECK_EXCLUDEBACKUPS, m_bExcludeBackups);
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

	NewGUI_XPButton(&m_btOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(&m_btCancel, IDB_CANCEL, IDB_CANCEL);

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_SEARCH),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(TRL("Find"));
	m_banner.SetCaption(TRL("Find a string in the password list."));

	GetDlgItem(IDC_EDIT_FINDTEXT)->SetFocus();

	if(m_lpGroupName != NULL)
	{
		CString str;

		str = TRL("Find"); str += _T(" - "); str += TRL("Group:");
		str += _T(" "); str += m_lpGroupName;
		m_banner.SetTitle(str);

		GetDlgItem(IDC_CHECK_GROUPNAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_EXCLUDEBACKUPS)->EnableWindow(FALSE);
	}

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
