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

#ifndef AFX_PASSWORDDLG_H__8F02B880_0ADD_11D8_BF16_0050BF14F5CC__INCLUDED_
#define AFX_PASSWORDDLG_H__8F02B880_0ADD_11D8_BF16_0050BF14F5CC__INCLUDED_

#include "Util/SysDefEx.h"
#include "NewGUI/NewGUICommon.h"
#include "NewGUI/KCSideBannerWnd.h"
#include "NewGUI/GradientProgressCtrl.h"
#include "NewGUI/XHyperLink.h"
#include "NewGUI/XPStyleButtonST.h"

/////////////////////////////////////////////////////////////////////////////

class CPP_CLASS_SHARE CPasswordDlg : public CDialog
{
public:
	CPasswordDlg(CWnd* pParent = NULL);

	void CleanUp();
	void EnableClientWindows();

	BOOL m_bLoadMode;
	BOOL m_bConfirm;
	BOOL m_bChanging;
	HICON m_hWindowIcon;

	CImageList m_ilIcons;
	CFont m_fStyle;
	CKCSideBannerWnd m_banner;

	CString m_strPassword;
	CString m_strRealKey;
	CString m_strRealKey2;
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
	CEdit	m_pEditPw;
	BOOL	m_bStars;
	BOOL	m_bKeyMethod;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CPasswordDlg)
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
