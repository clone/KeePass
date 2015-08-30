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

#ifndef ___SINGLE_LINE_EDIT_DLG_H___
#define ___SINGLE_LINE_EDIT_DLG_H___

#pragma once
#include <afxwin.h>
#include "../KeePassLibCpp/SysDefEx.h"
#include <vector>

#include "NewGUI/XPStyleButtonST.h"
#include "NewGUI/KCSideBannerWnd.h"

class CSingleLineEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CSingleLineEditDlg)

public:
	CSingleLineEditDlg(CWnd* pParent = NULL);
	virtual ~CSingleLineEditDlg();

	void InitEx(LPCTSTR lpTitle, LPCTSTR lpDesc,
		LPCTSTR lpPrompt, DWORD dwMaxInputLen, LPCTSTR lpDefaultValue,
		const std::vector<std::basic_string<TCHAR> >& vSelectable);

	CString GetEnteredText() { return m_strEnteredValue; }

	enum { IDD = IDD_SINGLELINEEDIT_DLG };

private:
	CString m_strTitle;
	CString m_strDesc;
	CString m_strPrompt;
	DWORD m_dwMaxInputLen;
	CString m_strDefaultValue;
	std::vector<std::basic_string<TCHAR> > m_vSelectable;

	CString m_strEnteredValue;

	CKCSideBannerWnd m_banner;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBtnOK();
	afx_msg void OnBtnCancel();
	CStatic m_stcPrompt;
	CComboBox m_cmbInput;
	CEdit m_tbInput;
	CXPStyleButtonST m_btnOK;
	CXPStyleButtonST m_btnCancel;
};

#endif // ___SINGLE_LINE_EDIT_DLG_H___
