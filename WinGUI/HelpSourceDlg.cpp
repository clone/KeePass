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

#include "StdAfx.h"
#include "PwSafe.h"
#include "HelpSourceDlg.h"

#include "Util/WinUtil.h"
#include "NewGUI/NewGUICommon.h"
#include "NewGUI/FontUtil.h"
#include "../KeePassLibCpp/Util/TranslateEx.h"

IMPLEMENT_DYNAMIC(CHelpSourceDlg, CDialog)

CHelpSourceDlg::CHelpSourceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHelpSourceDlg::IDD, pParent)
{
	// m_pBoldFont = NULL;
}

CHelpSourceDlg::~CHelpSourceDlg()
{
}

void CHelpSourceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_HELP_LOCAL, m_rbLocalHelp);
	DDX_Control(pDX, IDC_RADIO_HELP_ONLINE, m_rbOnlineHelp);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
}

BEGIN_MESSAGE_MAP(CHelpSourceDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CHelpSourceDlg::OnBnClickedOK)
	ON_BN_CLICKED(IDCANCEL, &CHelpSourceDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

BOOL CHelpSourceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	NewGUI_XPButton(m_btnOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(m_btnCancel, IDB_CANCEL, IDB_CANCEL);

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_OPTIONS),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(TRL("Help Source"));
	m_banner.SetCaption(TRL("Choose between local help and online help center."));

	if(WU_GetAppHelpSource() == APPHS_LOCAL)
		m_rbLocalHelp.SetCheck(BST_CHECKED);
	else
		m_rbOnlineHelp.SetCheck(BST_CHECKED);

	CFontUtil::SetDefaultFontFrom(&m_rbLocalHelp);

	// CFont* pFont = m_rbLocalHelp.GetFont();
	// if(pFont != NULL)
	// {
	//	LOGFONT lf;
	//	pFont->GetLogFont(&lf);
	//	lf.lfWeight = FW_BOLD;
	//	m_pBoldFont = new CFont();
	//	VERIFY(m_pBoldFont->CreateFontIndirect(&lf));
	//	m_rbLocalHelp.SetFont(m_pBoldFont);
	//	m_rbOnlineHelp.SetFont(m_pBoldFont);
	// }
	CFontUtil::AssignBold(&m_rbLocalHelp, this);
	CFontUtil::AssignBold(&m_rbOnlineHelp, this);

	return TRUE;
}

void CHelpSourceDlg::_CleanUpEx()
{
	// if(m_pBoldFont != NULL)
	// {
	//	m_pBoldFont->DeleteObject();
	//	SAFE_DELETE(m_pBoldFont);
	// }
}

void CHelpSourceDlg::OnBnClickedOK()
{
	WU_SetAppHelpSource((m_rbLocalHelp.GetCheck() == BST_CHECKED) ? APPHS_LOCAL : APPHS_ONLINE);

	this->_CleanUpEx();
	OnOK();
}

void CHelpSourceDlg::OnBnClickedCancel()
{
	this->_CleanUpEx();
	OnCancel();
}
