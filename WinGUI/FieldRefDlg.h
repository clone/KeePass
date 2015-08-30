/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2013 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___FIELD_REF_DLG_H___
#define ___FIELD_REF_DLG_H___

#include <afxwin.h>
#include <afxcmn.h>
#include "../KeePassLibCpp/PwManager.h"
#include "NewGUI/XPStyleButtonST.h"

class CFieldRefDlg : public CDialog
{
	DECLARE_DYNAMIC(CFieldRefDlg)

public:
	CFieldRefDlg(CWnd* pParent = NULL);
	virtual ~CFieldRefDlg();

	void InitEx(CPwManager* pMgr, CImageList* pImages);

	CString m_strFieldRef;

	enum { IDD = IDD_FIELDREF_DLG };

private:
	void _AddEntryToList(PW_ENTRY *p);
	void EnableChildControls();
	PW_ENTRY* GetSelectedEntry();
	BOOL _IdMatchesMultipleTimes(LPCTSTR lpText, TCHAR tchScan);

	BOOL m_bClosing;
	CPwManager* m_pMgr;
	CImageList* m_pImages;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	CXPStyleButtonST m_btOK;
	CXPStyleButtonST m_btCancel;
	CListCtrl m_lvEntries;
	CButton m_radioId;
	CButton m_radioRef;
	CXPStyleButtonST m_btHelp;

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnHelp();
	afx_msg void OnNMClickListEntries(NMHDR *pNMHDR, LRESULT *pResult);
};

#endif
