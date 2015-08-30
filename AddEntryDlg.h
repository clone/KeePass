/*
  Copyright (c) 2003/2004, Dominik Reichl <dominik.reichl@t-online.de>
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

#include "NewGUI/WzComboBox.h"
#include "NewGUI/KCSideBannerWnd.h"
#include "NewGUI/HyperEdit.h"
#include "NewGUI/ShadeButtonST.h"

#include "PwSafe/PwManager.h"

#define PWAE_STDURL_A "http://www."
#define PWAE_STDURL   _T(PWAE_STDURL_A)

/////////////////////////////////////////////////////////////////////////////

class CAddEntryDlg : public CDialog
{
public:
	CAddEntryDlg(CWnd* pParent = NULL);

	void CleanUp();

	CFont m_fStyle;
	CImageList m_ilIcons;
	CKCSideBannerWnd m_banner;

	BOOL m_bEditMode;
	CPwManager *m_pMgr;
	int m_nGroupId;
	int m_nIconId;

	//{{AFX_DATA(CAddEntryDlg)
	enum { IDD = IDD_ADDENTRY_DLG };
	CShadeButtonST	m_btHidePw;
	CShadeButtonST	m_btOK;
	CShadeButtonST	m_btCancel;
	CShadeButtonST	m_btRandomPw;
	CShadeButtonST	m_btPickIcon;
	CHyperEdit	m_pURL;
	CEdit	m_pRepeatPw;
	CEdit	m_pEditPw;
	CWzComboBox	m_pGroups;
	BOOL	m_bStars;
	int		m_idGroup;
	CString	m_strNotes;
	CString	m_strPassword;
	CString	m_strRepeatPw;
	CString	m_strTitle;
	CString	m_strURL;
	CString	m_strUserName;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CAddEntryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CAddEntryDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnCheckHidePw();
	afx_msg void OnPickIconBtn();
	afx_msg void OnRandomPwBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // AFX_ADDENTRYDLG_H__C5D4C3E0_0BAE_11D8_BF16_0050BF14F5CC__INCLUDED_
