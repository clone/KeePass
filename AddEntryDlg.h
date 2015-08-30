/*
  Copyright (c) 2003-2005, Dominik Reichl <dominik.reichl@t-online.de>
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

#ifndef AFX_ADDENTRYDLG_H__C5D4C3E0_0BAE_11D8_BF16_0050BF14F5CC__INCLUDED_
#define AFX_ADDENTRYDLG_H__C5D4C3E0_0BAE_11D8_BF16_0050BF14F5CC__INCLUDED_

#include "Util/SysDefEx.h"
#include "NewGUI/KCSideBannerWnd.h"
#include "NewGUI/XHyperLink.h"
#include "NewGUI/HyperEdit.h"
#include "NewGUI/amsEdit.h"
#include "NewGUI/AutoRichEditCtrl.h"
#include "NewGUI/GradientProgressCtrl.h"
#include "NewGUI/XPStyleButtonST.h"

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

	CFont m_fStyle;
	CImageList *m_pParentIcons;
	CKCSideBannerWnd m_banner;

	BOOL m_bEditMode;
	CPwManager *m_pMgr;
	DWORD m_dwEntryIndex;
	int m_nGroupId;
	int m_nIconId;

	CString m_strPassword;
	CString m_strRepeatPw;
	CString m_strNotes;
	PW_TIME m_tExpire;

	BCMenu m_popmenu;

	//{{AFX_DATA(CAddEntryDlg)
	enum { IDD = IDD_ADDENTRY_DLG };
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
	CHyperEdit	m_pURL;
	CEdit	m_pRepeatPw;
	CEdit	m_pEditPw;
	BOOL	m_bStars;
	CString	m_strTitle;
	CString	m_strURL;
	CString	m_strUserName;
	CAutoRichEditCtrl	m_reNotes;
	CString	m_strAttachment;
	BOOL	m_bExpires;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CAddEntryDlg)
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
	//}}AFX_MSG

	afx_msg LRESULT OnXHyperLinkClicked(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // AFX_ADDENTRYDLG_H__C5D4C3E0_0BAE_11D8_BF16_0050BF14F5CC__INCLUDED_
