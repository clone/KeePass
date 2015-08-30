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

#ifndef AFX_ENTRYPROPERTIESDLG_H__C4740E60_4F39_11D8_BF16_0050BF14F5CC__INCLUDED_
#define AFX_ENTRYPROPERTIESDLG_H__C4740E60_4F39_11D8_BF16_0050BF14F5CC__INCLUDED_

#include "NewGUI/XPStyleButtonST.h"
#include "NewGUI/KCSideBannerWnd.h"
#include "NewGUI/AMSEdit.h"
#include "../KeePassLibCpp/PwManager.h"

/////////////////////////////////////////////////////////////////////////////

class CEntryPropertiesDlg : public CDialog
{
public:
	CEntryPropertiesDlg(CWnd* pParent = NULL);

	void SetExpireDays(DWORD dwDays, BOOL bSetTime);

	CKCSideBannerWnd m_banner;
	CImageList *m_pParentIcons;

	CPwManager *m_pMgr;
	int m_nGroupInx;
	PW_TIME m_tExpire;
	int m_nIconId;
	DWORD m_dwDefaultExpire;

	//{{AFX_DATA(CEntryPropertiesDlg)
	enum { IDD = IDD_ENTRYPROPERTIES_DLG };
	CXPStyleButtonST	m_btSetToDefaultExpire;
	CXPStyleButtonST	m_btSelDefExpires;
	CComboBoxEx	m_cbGroups;
	CXPStyleButtonST	m_btSelectIcon;
	CXPStyleButtonST	m_btCancel;
	CXPStyleButtonST	m_btOK;
	CAMSTimeEdit	m_editTime;
	CAMSDateEdit	m_editDate;
	BOOL	m_bModExpire;
	BOOL	m_bModGroup;
	BOOL	m_bModIcon;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CEntryPropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CEntryPropertiesDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnButtonSelectIcon();
	afx_msg void OnCheckModExpire();
	afx_msg void OnCheckModGroup();
	afx_msg void OnCheckModIcon();
	afx_msg void OnSetDefaultExpireBtn();
	afx_msg void OnExpires1Week();
	afx_msg void OnExpires2Weeks();
	afx_msg void OnExpires1Month();
	afx_msg void OnExpires3Months();
	afx_msg void OnExpires6Months();
	afx_msg void OnExpires12Months();
	afx_msg void OnExpiresNow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // AFX_ENTRYPROPERTIESDLG_H__C4740E60_4F39_11D8_BF16_0050BF14F5CC__INCLUDED_
