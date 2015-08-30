/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2014 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef AFX_ADDGROUPDLG_H__40759B81_0ED2_11D8_BF16_0050BF14F5CC__INCLUDED_
#define AFX_ADDGROUPDLG_H__40759B81_0ED2_11D8_BF16_0050BF14F5CC__INCLUDED_

#include "NewGUI/XPStyleButtonST.h"
#include "NewGUI/KCSideBannerWnd.h"

/////////////////////////////////////////////////////////////////////////////

class CAddGroupDlg : public CDialog
{
public:
	CAddGroupDlg(CWnd* pParent = NULL);

	CKCSideBannerWnd m_banner;

	CImageList *m_pParentImageList;

	BOOL m_bEditMode;
	int m_nIconId;

	//{{AFX_DATA(CAddGroupDlg)
	enum { IDD = IDD_ADDGROUP_DLG };
	CXPStyleButtonST	m_btOK;
	CXPStyleButtonST	m_btCancel;
	CXPStyleButtonST	m_btSetIcon;
	CString	m_strGroupName;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CAddGroupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

private:
	CString m_strInitialName;

protected:
	//{{AFX_MSG(CAddGroupDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSetIconBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // AFX_ADDGROUPDLG_H__40759B81_0ED2_11D8_BF16_0050BF14F5CC__INCLUDED_
