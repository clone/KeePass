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

#ifndef AFX_ADDENTRYDLG_H__C5D4C3E0_0BAE_11D8_BF16_0050BF14F5CC__INCLUDED_
#define AFX_ADDENTRYDLG_H__C5D4C3E0_0BAE_11D8_BF16_0050BF14F5CC__INCLUDED_

#include "Util/SysDefEx.h"
#include "NewGUI/KCSideBannerWnd.h"
#include "NewGUI/XHyperLink.h"
#include "NewGUI/amsEdit.h"
#include "NewGUI/AutoRichEditCtrlFx.h"
#include "NewGUI/GradientProgressCtrl.h"
#include "NewGUI/XPStyleButtonST.h"
#include "NewGUI/SecureEditEx.h"

#include "PwSafe/PwManager.h"

#define PWAE_STDURL_A ""
#define PWAE_STDURL   _T(PWAE_STDURL_A)

/////////////////////////////////////////////////////////////////////////////

class CAddEntryDlg : public CDialog
{
public:
	CAddEntryDlg(CWnd* pParent = NULL);

	void CleanUp();

	void UpdateControlsStatus();
	void SetExpireDays(DWORD dwDays, BOOL bSetTime);

	CFont m_fStyle;
	CFont m_fSymbol;
	CImageList *m_pParentIcons;
	CKCSideBannerWnd m_banner;
	CToolTipCtrl m_tipSecClear;

	BOOL m_bEditMode;
	CPwManager *m_pMgr;
	DWORD m_dwEntryIndex;
	int m_nGroupId;
	int m_nIconId;
	BOOL m_bAutoPwGen;

	CString m_strNotes;
	PW_TIME m_tExpire;
	DWORD m_dwDefaultExpire;

	BCMenu m_popmenu;

	LPTSTR m_lpPassword;
	LPTSTR m_lpRepeatPw;

	//{{AFX_DATA(CAddEntryDlg)
	enum { IDD = IDD_ADDENTRY_DLG };
	CXPStyleButtonST	m_btSelDefExpires;
	CXPStyleButtonST	m_btSetToDefaultExpire;
	CComboBoxEx	m_cbGroups;
	CXHyperLink	m_hlHelpURL;
	CXHyperLink	m_hlHelpAutoType;
	CGradientProgressCtrl	m_cPassQuality;
	CXPStyleButtonST	m_btRemoveAttachment;
	CXPStyleButtonST	m_btSaveAttachment;
	CXPStyleButtonST	m_btSetAttachment;
	CAMSTimeEdit	m_editTime;
	CAMSDateEdit	m_editDate;
	CXPStyleButtonST	m_btHidePw;
	CXPStyleButtonST	m_btOK;
	CXPStyleButtonST	m_btCancel;
	CXPStyleButtonST	m_btRandomPw;
	CXPStyleButtonST	m_btPickIcon;
	CEdit	m_pURL;
	CSecureEditEx	m_pRepeatPw;
	CSecureEditEx	m_pEditPw;
	BOOL	m_bStars;
	CString	m_strTitle;
	CString	m_strURL;
	CString	m_strUserName;
	CAutoRichEditCtrlFx	m_reNotes;
	CString	m_strAttachment;
	BOOL	m_bExpires;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CAddEntryDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CAddEntryDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnCheckHidePw();
	afx_msg void OnPickIconBtn();
	afx_msg void OnRandomPwBtn();
	afx_msg void OnReCopyAll();
	afx_msg void OnReCopySel();
	afx_msg void OnReDelete();
	afx_msg void OnRePaste();
	afx_msg void OnReSelectAll();
	afx_msg void OnReCut();
	afx_msg void OnReUndo();
	afx_msg void OnSetAttachBtn();
	afx_msg void OnSaveAttachBtn();
	afx_msg void OnRemoveAttachBtn();
	afx_msg void OnChangeEditPassword();
	afx_msg void OnCheckExpires();
	afx_msg void OnSetDefaultExpireBtn();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnExpires1Week();
	afx_msg void OnExpires2Weeks();
	afx_msg void OnExpires1Month();
	afx_msg void OnExpires3Months();
	afx_msg void OnExpires6Months();
	afx_msg void OnExpires12Months();
	afx_msg void OnExpiresNow();
	afx_msg void OnReNotesClickLink(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	afx_msg LRESULT OnXHyperLinkClicked(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // AFX_ADDENTRYDLG_H__C5D4C3E0_0BAE_11D8_BF16_0050BF14F5CC__INCLUDED_
