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

#ifndef AFX_PASSWORDDLG_H__8F02B880_0ADD_11D8_BF16_0050BF14F5CC__INCLUDED_
#define AFX_PASSWORDDLG_H__8F02B880_0ADD_11D8_BF16_0050BF14F5CC__INCLUDED_

#include "../KeePassLibCpp/SysDefEx.h"
#include "NewGUI/NewGUICommon.h"
#include "NewGUI/KCSideBannerWnd.h"
#include "NewGUI/GradientProgressCtrl.h"
#include "NewGUI/XHyperLink.h"
#include "NewGUI/XPStyleButtonST.h"
#include "NewGUI/SecureEditEx.h"

/////////////////////////////////////////////////////////////////////////////

class CPP_CLASS_SHARE CPasswordDlg : public CDialog
{
public:
	CPasswordDlg(CWnd* pParent = NULL);

	void CleanUp();
	void FreePasswords();
	void EnableClientWindows();

	BOOL m_bLoadMode;
	BOOL m_bConfirm;
	BOOL m_bChanging;
	HICON m_hWindowIcon;

	CImageList m_ilIcons;
	CFont m_fStyle;
	CFont m_fSymbol;
	CKCSideBannerWnd m_banner;
	CToolTipCtrl m_tipSecClear;

	LPCTSTR m_lpPreSelectPath;

	LPTSTR m_lpKey;
	LPTSTR m_lpKey2;
	BOOL m_bKeyFile;

	CString m_strDescriptiveName;

	//{{AFX_DATA(CPasswordDlg)
	enum { IDD = IDD_PASSWORD_DLG };
	CComboBoxEx	m_cbDiskList;
	CXHyperLink	m_hlSelFile;
	CGradientProgressCtrl	m_cPassQuality;
	CXPStyleButtonST	m_btStars;
	CXPStyleButtonST	m_btOK;
	CXPStyleButtonST	m_btCancel;
	CXPStyleButtonST	m_btMakePw;
	CSecureEditEx	m_pEditPw;
	BOOL	m_bStars;
	BOOL	m_bKeyMethod;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CPasswordDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

private:
	BOOL m_bOnce;

protected:
	//{{AFX_MSG(CPasswordDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnCheckStars();
	afx_msg void OnMakePasswordBtn();
	afx_msg void OnChangeEditPassword();
	afx_msg void OnSelChangeComboDiskList();
	afx_msg void OnCheckKeymethodAnd();
	//}}AFX_MSG

	afx_msg LRESULT OnXHyperLinkClicked(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // AFX_PASSWORDDLG_H__8F02B880_0ADD_11D8_BF16_0050BF14F5CC__INCLUDED_
