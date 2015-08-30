/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2008 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___OPTIONS_AUTO_TYPE_DLG_H___
#define ___OPTIONS_AUTO_TYPE_DLG_H___

#include <afxwin.h>
#include <afxcmn.h>

#include "NewGUI/NewGUICommon.h"
#include "NewGUI/KCWndUtil.h"
#include "NewGUI/KCSideBannerWnd.h"
#include "NewGUI/XPStyleButtonST.h"
#include "PwSafeDlg.h"

class COptionsAutoTypeDlg : public CDialog
{
	DECLARE_DYNAMIC(COptionsAutoTypeDlg)

public:
	COptionsAutoTypeDlg(CWnd* pParent = NULL);
	virtual ~COptionsAutoTypeDlg();

	enum { IDD = IDD_OPTIONS_AUTOTYPE_DLG };

	CPwSafeDlg *m_pParentDlg;

	BOOL m_bDisableAutoType;
	BOOL m_bMinimizeBeforeAT;
	DWORD m_dwATHotKey;
	CString m_strDefaultAutoTypeSequence;
	BOOL m_bAutoTypeIEFix;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBtnOK();
	afx_msg void OnBtnCancel();
	afx_msg void OnCheckEnableAT();
	afx_msg void OnChangeDefaultSeq();

	BOOL m_bEnableAT;
	BOOL m_bAlternative;
	CString m_strDefaultSeq;
	BOOL m_bIEFix;
	CButton m_cbAlternative;
	CEdit m_tbDefaultSeq;
	CButton m_cbIEFix;
	CButton m_cbEnableAT;
	CXPStyleButtonST m_btnOK;
	CXPStyleButtonST m_btnCancel;
	CHotKeyCtrl m_hkAutoType;
	CButton m_cbSortATItems;
	BOOL m_bSortAutoTypeSelItems;

private:
	void EnableChildControls();

	CKCSideBannerWnd m_banner;
};

#endif // ___OPTIONS_AUTO_TYPE_DLG_H___
