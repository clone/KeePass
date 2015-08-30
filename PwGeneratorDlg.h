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

#ifndef AFX_PWGENERATORDLG_H__ED008380_12A4_11D8_BF16_0050BF14F5CC__INCLUDED_
#define AFX_PWGENERATORDLG_H__ED008380_12A4_11D8_BF16_0050BF14F5CC__INCLUDED_

#include "Util/SysDefEx.h"
#include "NewGUI/KCSideBannerWnd.h"
#include "NewGUI/XPStyleButtonST.h"
#include "NewGUI/GradientProgressCtrl.h"

/////////////////////////////////////////////////////////////////////////////

class CPwGeneratorDlg : public CDialog
{
public:
	CPwGeneratorDlg(CWnd* pParent = NULL);

	void CleanUp();

	void _SetCheck(int inxItem, BOOL bEnable);
	BOOL _GetCheck(int inxItem);

	BOOL m_bCanAccept;

	CKCSideBannerWnd m_banner;
	CImageList m_ilIcons;
	CFont m_fStyle;

	static void SetOptions(CString strOptions, CString strCharSet, UINT nCharacters);
	static void GetOptions(CString *pstrOptions, CString *pstrCharSet, UINT *pnCharacters);

	//{{AFX_DATA(CPwGeneratorDlg)
	enum { IDD = IDD_PW_GENERATOR_DLG };
	CXPStyleButtonST	m_btHidePw;
	CEdit	m_cEditPw;
	CGradientProgressCtrl	m_cPassQuality;
	CSpinButtonCtrl	m_spinNumChars;
	CXPStyleButtonST	m_btGenerate;
	CXPStyleButtonST	m_btnCancel;
	CXPStyleButtonST	m_btnOK;
	CListCtrl	m_cList;
	UINT	m_nCharacters;
	CString	m_strPassword;
	BOOL	m_bCharSpec;
	CString	m_strCharSet;
	BOOL	m_bGetEntropy;
	BOOL	m_bHidePw;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CPwGeneratorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	void _SaveOptions();

	//{{AFX_MSG(CPwGeneratorDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnGenerateBtn();
	afx_msg void OnCheckCharSpec();
	afx_msg void OnClickListOptions(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickListOptions(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosSpinNumChars(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditPw();
	afx_msg void OnCheckHidePw();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // AFX_PWGENERATORDLG_H__ED008380_12A4_11D8_BF16_0050BF14F5CC__INCLUDED_
