/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2005 Dominik Reichl <dominik.reichl@t-online.de>

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

#if !defined(AFX_CHECKOPTIONSDLG_H__5F6F5781_A123_11D9_BF17_0050BF14F5CC__INCLUDED_)
#define AFX_CHECKOPTIONSDLG_H__5F6F5781_A123_11D9_BF17_0050BF14F5CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NewGUI/NewGUICommon.h"
#include "NewGUI/KCSideBannerWnd.h"
#include "NewGUI/XPStyleButtonST.h"
#include "NewGUI/OptionsList.h"

typedef struct
{
	LPCTSTR lpString;
	BOOL *pbValue;
	int nIcon;
} CHKOPT_PARAM;

/////////////////////////////////////////////////////////////////////////////

class CCheckOptionsDlg : public CDialog
{
public:
	CCheckOptionsDlg(CWnd* pParent = NULL);

	void CleanUp();

	DWORD m_dwNumParams;
	CHKOPT_PARAM *m_pParams;

	CString m_strTitle;
	CString m_strDescription;

	CKCSideBannerWnd m_banner;
	CImageList m_ilOptionIcons;

	//{{AFX_DATA(CCheckOptionsDlg)
	enum { IDD = IDD_CHECKOPTIONS_DLG };
	COptionsList	m_olOptions;
	CXPStyleButtonST	m_btCancel;
	CXPStyleButtonST	m_btOK;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CCheckOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CCheckOptionsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_CHECKOPTIONSDLG_H__5F6F5781_A123_11D9_BF17_0050BF14F5CC__INCLUDED_)
