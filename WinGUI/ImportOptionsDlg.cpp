/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2006 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "ImportOptionsDlg.h"

#include "NewGUI/NewGUICommon.h"
#include "../KeePassLibCpp/Util/TranslateEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CImportOptionsDlg::CImportOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CImportOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImportOptionsDlg)
	m_nImportMethod = 0;
	//}}AFX_DATA_INIT
}

void CImportOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportOptionsDlg)
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Radio(pDX, IDC_RADIO_CREATENEW, m_nImportMethod);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CImportOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CImportOptionsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CImportOptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	NewGUI_XPButton(&m_btOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(&m_btCancel, IDB_CANCEL, IDB_CANCEL);

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_ICONPIC),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(TRL("Options"));
	m_banner.SetCaption(_T(" "));

	m_fBold.CreateFont(-10, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, _T("MS Shell Dlg"));

	GetDlgItem(IDC_RADIO_CREATENEW)->SetFont(&m_fBold);
	GetDlgItem(IDC_RADIO_OVERWRITE_ALWAYS)->SetFont(&m_fBold);
	GetDlgItem(IDC_RADIO_OVERWRITE_TIMEBASED)->SetFont(&m_fBold);

	return TRUE;
}

void CImportOptionsDlg::OnOK()
{
	UpdateData(TRUE);

	VERIFY(m_fBold.DeleteObject());
	CDialog::OnOK();
}

void CImportOptionsDlg::OnCancel()
{
	UpdateData(TRUE);

	VERIFY(m_fBold.DeleteObject());
	CDialog::OnCancel();
}
