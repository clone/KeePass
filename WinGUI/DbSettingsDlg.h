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

#ifndef AFX_DBSETTINGSDLG_H__B913D560_B1A3_11D8_BF16_0050BF14F5CC__INCLUDED_
#define AFX_DBSETTINGSDLG_H__B913D560_B1A3_11D8_BF16_0050BF14F5CC__INCLUDED_

#include "NewGUI/XPStyleButtonST.h"
#include "NewGUI/KCSideBannerWnd.h"
#include "NewGUI/ColorStaticEx.h"
#include <afxwin.h>
#include <afxcmn.h>

/////////////////////////////////////////////////////////////////////////////

class CDbSettingsDlg : public CDialog
{
public:
	CDbSettingsDlg(CWnd* pParent = NULL);

	CKCSideBannerWnd m_banner;

	COLORREF m_clr;

	void EnableControlsEx();

	//{{AFX_DATA(CDbSettingsDlg)
	enum { IDD = IDD_DBSETTINGS_DLG };
	CXPStyleButtonST	m_btHelp;
	CXPStyleButtonST	m_btCalcRounds;
	CComboBox	m_cEncAlgos;
	CXPStyleButtonST	m_btCancel;
	CXPStyleButtonST	m_btOK;
	int		m_nAlgorithm;
	DWORD	m_dwNumKeyEnc;
	CString m_strDefaultUserName;
	CColorStaticEx m_stcColor;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CDbSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CDbSettingsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBtnCalcRounds();
	afx_msg void OnBtnClickedHelp();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	CSliderCtrl m_sldColor;
	afx_msg void OnSliderColorChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	BOOL m_bCustomColor;
	afx_msg void OnCheckCustomColor();
};

//{{AFX_INSERT_LOCATION}}

#endif // AFX_DBSETTINGSDLG_H__B913D560_B1A3_11D8_BF16_0050BF14F5CC__INCLUDED_
