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

#ifndef AFX_ICONPICKERDLG_H__40759B80_0ED2_11D8_BF16_0050BF14F5CC__INCLUDED_
#define AFX_ICONPICKERDLG_H__40759B80_0ED2_11D8_BF16_0050BF14F5CC__INCLUDED_

#include "NewGUI/XPStyleButtonST.h"
#include "NewGUI/KCSideBannerWnd.h"

/////////////////////////////////////////////////////////////////////////////

class CIconPickerDlg : public CDialog
{
public:
	CIconPickerDlg(CWnd* pParent = NULL);

	CImageList *m_pImageList;
	UINT m_uNumIcons;

	int m_nSelectedIcon;

	CKCSideBannerWnd m_banner;

	//{{AFX_DATA(CIconPickerDlg)
	enum { IDD = IDD_ICONPICKER_DLG };
	CXPStyleButtonST	m_btOK;
	CXPStyleButtonST	m_btCancel;
	CListCtrl	m_cList;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CIconPickerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CIconPickerDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // AFX_ICONPICKERDLG_H__40759B80_0ED2_11D8_BF16_0050BF14F5CC__INCLUDED_
