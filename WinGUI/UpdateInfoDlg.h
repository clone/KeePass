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

#ifndef ___UPDATE_INFO_DLG_H___
#define ___UPDATE_INFO_DLG_H___

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NewGUI/XPStyleButtonST.h"
#include "NewGUI/XHyperLink.h"
#include <afxcmn.h>
#include "Resource.h"

#include "Util/UpdateCheckEx.h"
#include <afxwin.h>

class CUpdateInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CUpdateInfoDlg)

public:
	CUpdateInfoDlg(CWnd* pParent = NULL);
	virtual ~CUpdateInfoDlg();

	void InitEx(CImageList *pImgList, const UC_COMPONENTS_LIST* pcInfo,
		LPCTSTR lpStatus);

	enum { IDD = IDD_UPDATEINFO_DLG };

	CXHyperLink	m_hlVisitWebsite;

private:
	CFont m_fBold;
	CImageList *m_pImgList;
	const UC_COMPONENTS_LIST* m_pcInfo;

	void CleanUp();
	void FillComponentList();

	static CString FormatVersion(UINT64 qwVersion, LPCTSTR lpName);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnXHyperLinkClicked(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnCancel();
	afx_msg void OnOK();
	afx_msg void OnBnClickedOK();
	CListCtrl m_cList;
	CString m_strStatus;
	CXPStyleButtonST m_btClose;
	CStatic m_lblStatus;
};

#endif // ___UPDATE_INFO_DLG_H___
