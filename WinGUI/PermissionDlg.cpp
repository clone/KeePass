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
#include "PermissionDlg.h"
#include "Util/RemoteControl.h"
#include "NewGUI/NewGUICommon.h"
#include "../KeePassLibCpp/Util/TranslateEx.h"

IMPLEMENT_DYNAMIC(CPermissionDlg, CDialog)

CPermissionDlg::CPermissionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPermissionDlg::IDD, pParent)
{
	m_nPermission = RC_PERMISSION_DENYACCESS;
	m_nActivationCountdown = 3;
}

CPermissionDlg::~CPermissionDlg()
{
}

void CPermissionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_READONLY, m_btnReadOnly);
	DDX_Text(pDX, IDC_STC_APP, m_strApp);
}

BEGIN_MESSAGE_MAP(CPermissionDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CPermissionDlg::OnOK)
	ON_BN_CLICKED(IDCANCEL, &CPermissionDlg::OnCancel)
	ON_BN_CLICKED(IDC_READONLY, &CPermissionDlg::OnBnClickedReadOnly)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CPermissionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	NewGUI_XPButton(&m_btnOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(&m_btnCancel, IDB_CANCEL, IDB_CANCEL);
	NewGUI_XPButton(&m_btnReadOnly, IDB_OK, IDB_OK);

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_KEYHOLE),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(TRL("Incoming IPC Request"));
	CString strIPCCap = TRL("An application requests access through inter-process communication");
	strIPCCap += _T(".");
	m_banner.SetCaption(strIPCCap);

	m_btnOK.EnableWindow(FALSE);
	m_btnReadOnly.EnableWindow(FALSE);

	m_btnOK.GetWindowText(m_strBtnOK);
	m_btnReadOnly.GetWindowText(m_strBtnReadOnly);

	CString strApp = TRL("Application");
	strApp += _T(": ");
	if(m_strApp.GetLength() == 0) strApp += TRL("Unknown");
	else strApp += m_strApp;
	strApp += _T(".");
	m_strApp = strApp;
	UpdateData(FALSE);

	m_fBold.CreateFont(-10, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("MS Shell Dlg"));
	GetDlgItem(IDC_STC_APP)->SetFont(&m_fBold);

	OnTimer(INITIAL_TIMER_UPDATE_ID);
	SetTimer(1, 1000, NULL);

	GetDlgItem(IDCANCEL)->SetFocus();
	return FALSE;
}

void CPermissionDlg::OnOK()
{
	if(m_nActivationCountdown != -1) return;
	
	m_nPermission = RC_PERMISSION_FULLACCESS;

	KillTimer(1);
	m_fBold.DeleteObject();
	CDialog::OnOK();
}

void CPermissionDlg::OnCancel()
{
	m_nPermission = RC_PERMISSION_DENYACCESS;

	KillTimer(1);
	m_fBold.DeleteObject();
	CDialog::OnCancel();
}

void CPermissionDlg::OnBnClickedReadOnly()
{
	if(m_nActivationCountdown != -1) return;

	m_nPermission = RC_PERMISSION_READONLYACCESS;

	KillTimer(1);
	m_fBold.DeleteObject();
	CDialog::OnOK();
}

void CPermissionDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent != INITIAL_TIMER_UPDATE_ID) CDialog::OnTimer(nIDEvent);

	if((nIDEvent == 1) || (nIDEvent == INITIAL_TIMER_UPDATE_ID))
	{
		this->ShowWindow(SW_SHOW);

		if(m_nActivationCountdown == 0)
		{
			m_nActivationCountdown = -1;

			m_btnOK.SetWindowText(m_strBtnOK);
			m_btnReadOnly.SetWindowText(m_strBtnReadOnly);
			m_btnOK.EnableWindow(TRUE);
			m_btnReadOnly.EnableWindow(TRUE);
		}
		else if(m_nActivationCountdown > 0)
		{
			CString str;
			str.Format(_T(" (%d)"), m_nActivationCountdown);
			m_btnOK.SetWindowText(m_strBtnOK + str);
			m_btnReadOnly.SetWindowText(m_strBtnReadOnly + str);

			m_nActivationCountdown--;
		}
	}
}
