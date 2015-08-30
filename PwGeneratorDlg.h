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

#ifndef AFX_PWGENERATORDLG_H__ED008380_12A4_11D8_BF16_0050BF14F5CC__INCLUDED_
#define AFX_PWGENERATORDLG_H__ED008380_12A4_11D8_BF16_0050BF14F5CC__INCLUDED_

#include "Util/SysDefEx.h"
#include "NewGUI/KCSideBannerWnd.h"
#include "NewGUI/XPStyleButtonST.h"
#include "NewGUI/GradientProgressCtrl.h"
#include "NewGUI/SecureEditEx.h"

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
	CFont m_fSymbol;
	CToolTipCtrl m_tipSecClear;

	LPTSTR m_lpPassword;

	static void SetOptions(CString strOptions, CString strCharSet, UINT nCharacters);
	static void GetOptions(CString *pstrOptions, CString *pstrCharSet, UINT *pnCharacters);

	//{{AFX_DATA(CPwGeneratorDlg)
	enum { IDD = IDD_PW_GENERATOR_DLG };
	CXPStyleButtonST	m_btHidePw;
	CSecureEditEx	m_cEditPw;
	CGradientProgressCtrl	m_cPassQuality;
	CSpinButtonCtrl	m_spinNumChars;
	CXPStyleButtonST	m_btGenerate;
	CXPStyleButtonST	m_btnCancel;
	CXPStyleButtonST	m_btnOK;
	CListCtrl	m_cList;
	UINT	m_nCharacters;
	BOOL	m_bCharSpec;
	CString	m_strCharSet;
	BOOL	m_bGetEntropy;
	BOOL	m_bHidePw;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CPwGeneratorDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
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
