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

#ifndef AFX_ENTRYLISTDLG_H__3D6534C0_AFD4_11D8_BF16_0050BF14F5CC__INCLUDED_
#define AFX_ENTRYLISTDLG_H__3D6534C0_AFD4_11D8_BF16_0050BF14F5CC__INCLUDED_

#include "Util/SysDefEx.h"
#include "NewGUI/XPStyleButtonST.h"
#include "NewGUI/KCSideBannerWnd.h"
#include "PwSafe/PwManager.h"

#include <vector>

#define ELDMODE_UNKNOWN    0
#define ELDMODE_EXPIRED    1
#define ELDMODE_SOONTOEXP  2
#define ELDMODE_EXPSOONEXP 3
#define ELDMODE_LIST       4

/////////////////////////////////////////////////////////////////////////////

class CEntryListDlg : public CDialog
{
// Konstruktion
public:
	CEntryListDlg(CWnd* pParent = NULL);

	void SaveSelectedEntry();

	CKCSideBannerWnd m_banner;

	// You must initialize the following variables before using the CEntryListDlg class
	int m_nDisplayMode;
	CPwManager *m_pMgr;
	CImageList *m_pImgList;
	BOOL m_bPasswordStars;
	BOOL m_bUserStars;

	BYTE m_aUuid[16];

	CString m_strBannerTitle;
	CString m_strBannerCaption;

	std::vector<PW_UUID_STRUCT> m_vEntryList;

	//{{AFX_DATA(CEntryListDlg)
	enum { IDD = IDD_ENTRYLIST_DLG };
	CStatic	m_stcProductName;
	CXPStyleButtonST	m_btClose;
	CListCtrl	m_cList;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CEntryListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	void _AddEntryToList(PW_ENTRY *p, BOOL bExpiredIcon);

	//{{AFX_MSG(CEntryListDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClickEntryList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReturnEntryList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // AFX_ENTRYLISTDLG_H__3D6534C0_AFD4_11D8_BF16_0050BF14F5CC__INCLUDED_
