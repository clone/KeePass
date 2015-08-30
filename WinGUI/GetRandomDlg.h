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

#ifndef AFX_GETRANDOMDLG_H__28A6C1A0_0FA4_11D8_BF16_0050BF14F5CC__INCLUDED_
#define AFX_GETRANDOMDLG_H__28A6C1A0_0FA4_11D8_BF16_0050BF14F5CC__INCLUDED_

#include "NewGUI/KCSideBannerWnd.h"
#include "NewGUI/XPStyleButtonST.h"

/////////////////////////////////////////////////////////////////////////////

class CGetRandomDlg : public CDialog
{
public:
	CGetRandomDlg(CWnd* pParent = NULL);

	RECT m_rect;

	BOOL m_bMouseActive;
	POINT m_points[100];
	DWORD m_dwCurPoint;

	CKCSideBannerWnd m_banner;

	unsigned char m_pFinalRandom[32];

	//{{AFX_DATA(CGetRandomDlg)
	enum { IDD = IDD_GETRANDOM_DLG };
	CXPStyleButtonST	m_btCancel;
	CXPStyleButtonST	m_btOK;
	CXPStyleButtonST	m_btMouseInput;
	CProgressCtrl	m_cMouseProgress;
	CString	m_strRandom;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CGetRandomDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CGetRandomDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBeginMouseInput();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // AFX_GETRANDOMDLG_H__28A6C1A0_0FA4_11D8_BF16_0050BF14F5CC__INCLUDED_
