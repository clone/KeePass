/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2007 Dominik Reichl <dominik.reichl@t-online.de>

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

#if !defined(AFX_IMPORTOPTIONSDLG_H__A254F1CE_C80C_4D9E_9E94_ED6648AFE2EF__INCLUDED_)
#define AFX_IMPORTOPTIONSDLG_H__A254F1CE_C80C_4D9E_9E94_ED6648AFE2EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NewGUI/XPStyleButtonST.h"
#include "NewGUI/KCSideBannerWnd.h"

/////////////////////////////////////////////////////////////////////////////

class CImportOptionsDlg : public CDialog
{
public:
	CImportOptionsDlg(CWnd* pParent = NULL);

	CFont m_fBold;
	CKCSideBannerWnd m_banner;

	//{{AFX_DATA(CImportOptionsDlg)
	enum { IDD = IDD_IMPORT_OPTIONS_DLG };
	CXPStyleButtonST	m_btCancel;
	CXPStyleButtonST	m_btOK;
	int		m_nImportMethod;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CImportOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CImportOptionsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_IMPORTOPTIONSDLG_H__A254F1CE_C80C_4D9E_9E94_ED6648AFE2EF__INCLUDED_)
