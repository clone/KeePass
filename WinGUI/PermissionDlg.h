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

#pragma once

#include "NewGUI/KCSideBannerWnd.h"
#include "NewGUI/XPStyleButtonST.h"

#define INITIAL_TIMER_UPDATE_ID 14904

class CPermissionDlg : public CDialog
{
	DECLARE_DYNAMIC(CPermissionDlg)

public:
	CPermissionDlg(CWnd* pParent = NULL);
	virtual ~CPermissionDlg();

	int m_nPermission;
	CKCSideBannerWnd m_banner;
	// CFont m_fBold;

	int m_nActivationCountdown;
	CString m_strBtnOK;
	CString m_strBtnReadOnly;
	CString m_strApp;
	BOOL m_bAlwaysAllowFullAccess;
	CXPStyleButtonST m_btnCancel;
	CXPStyleButtonST m_btnOK;
	CXPStyleButtonST m_btnReadOnly;

	enum { IDD = IDD_PERMISSION_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBnClickedReadOnly();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
