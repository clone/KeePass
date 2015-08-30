/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2011 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "SingleLineEditDlg.h"

#include "NewGUI/NewGUICommon.h"
#include "../KeePassLibCpp/PwManager.h"

IMPLEMENT_DYNAMIC(CSingleLineEditDlg, CDialog)

CSingleLineEditDlg::CSingleLineEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSingleLineEditDlg::IDD, pParent)
{
	m_dwMaxInputLen = DWORD_MAX;
}

CSingleLineEditDlg::~CSingleLineEditDlg()
{
}

void CSingleLineEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PROMPT, m_stcPrompt);
	DDX_Control(pDX, IDC_COMBO_INPUT, m_cmbInput);
	DDX_Control(pDX, IDC_EDIT_INPUT, m_tbInput);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
}

BEGIN_MESSAGE_MAP(CSingleLineEditDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSingleLineEditDlg::OnBtnOK)
	ON_BN_CLICKED(IDCANCEL, &CSingleLineEditDlg::OnBtnCancel)
END_MESSAGE_MAP()

BOOL CSingleLineEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	NewGUI_XPButton(m_btnOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(m_btnCancel, IDB_CANCEL, IDB_CANCEL);

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	SetWindowText(m_strTitle);

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_KEYHOLE),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(m_strTitle);
	m_banner.SetCaption(m_strDesc);

	m_stcPrompt.SetWindowText(m_strPrompt);

	if(m_vSelectable.size() == 0)
	{
		m_cmbInput.EnableWindow(FALSE);
		m_cmbInput.ShowWindow(SW_HIDE);

		if(m_dwMaxInputLen != DWORD_MAX)
			m_tbInput.LimitText(m_dwMaxInputLen);

		m_tbInput.SetWindowText(m_strDefaultValue);
		m_tbInput.SetFocus();
	}
	else // Use combo box
	{
		m_tbInput.EnableWindow(FALSE);
		m_tbInput.ShowWindow(SW_HIDE);

		if(m_dwMaxInputLen != DWORD_MAX)
			m_cmbInput.LimitText(m_dwMaxInputLen);

		m_cmbInput.SetWindowText(m_strDefaultValue);

		for(DWORD dwPre = 0; dwPre < m_vSelectable.size(); ++dwPre)
			m_cmbInput.AddString(m_vSelectable[dwPre].c_str());

		m_cmbInput.SetFocus();
	}

	this->Invalidate();
	return FALSE;
}

void CSingleLineEditDlg::InitEx(LPCTSTR lpTitle, LPCTSTR lpDesc,
	LPCTSTR lpPrompt, DWORD dwMaxInputLen, LPCTSTR lpDefaultValue,
	const std::vector<std::basic_string<TCHAR> >& vSelectable)
{
	m_strTitle = lpTitle;
	m_strDesc = lpDesc;
	m_strPrompt = lpPrompt;
	m_dwMaxInputLen = dwMaxInputLen;
	m_strDefaultValue = lpDefaultValue;
	m_vSelectable = vSelectable;
}

void CSingleLineEditDlg::OnBtnOK()
{
	if(m_vSelectable.size() > 0)
		m_cmbInput.GetWindowText(m_strEnteredValue);
	else
		m_tbInput.GetWindowText(m_strEnteredValue);

	this->OnOK();
}

void CSingleLineEditDlg::OnBtnCancel()
{
	this->OnCancel();
}
