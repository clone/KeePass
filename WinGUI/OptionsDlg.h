/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2010 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef AFX_OPTIONSDLG_H__97A7BE00_1851_11D8_BF16_0050BF14F5CC__INCLUDED_
#define AFX_OPTIONSDLG_H__97A7BE00_1851_11D8_BF16_0050BF14F5CC__INCLUDED_

#include <afxwin.h>

#include "NewGUI/KCSideBannerWnd.h"
#include "NewGUI/WindowGroups.h"
#include "NewGUI/ColourPickerXP.h"
#include "NewGUI/XPStyleButtonST.h"
#include "NewGUI/OptionsList.h"
#include "PwSafeDlg.h"

#define OPTGRP_SECURITY  0
#define OPTGRP_GUI       1
#define OPTGRP_FILES     2
#define OPTGRP_MEMORY    3
#define OPTGRP_SETUP     4
#define OPTGRP_ADVANCED  5

// No _T, will be TRL-ed
#define OPTSZ_FILES     "Files"
#define OPTSZ_MEMORY    "Memory"
#define OPTSZ_GUI       "Interface (GUI)"
#define OPTSZ_SECURITY  "Security"
#define OPTSZ_SETUP     "Setup"
#define OPTSZ_ADVANCED  "Advanced"

/////////////////////////////////////////////////////////////////////////////

class COptionsDlg : public CDialog
{
public:
	COptionsDlg(CWnd* pParent = NULL);
	CPwSafeDlg *m_pParentDlg;

	CKCSideBannerWnd m_banner;

	CString m_strFontSpec;
	CWindowGroups m_wndgrp;
	CImageList m_ilIcons;
	CImageList m_ilOptionIcons;

	COLORREF m_rgbRowHighlight;

	DWORD m_dwATHotKey;
	CString m_strDefaultAutoTypeSequence;
	BOOL m_bAutoTypeIEFix;
	BOOL m_bSortAutoTypeSelItems;

private:
	void AddTcItem(LPCTSTR lpName, int iImageIndex);

public:
	BOOL m_bRememberLast;
	BOOL m_bAutoSave;
	BOOL m_bOpenLastDb;
	BOOL m_bStartMinimized;
	BOOL m_bAutoShowExpired;
	BOOL m_bAutoShowExpiredSoon;
	BOOL m_bStartWithWindows;
	BOOL m_bBackupEntries;
	BOOL m_bSingleInstance;
	BOOL m_bSingleClickTrayIcon;
	BOOL m_bQuickFindIncBackup;
	BOOL m_bQuickFindIncExpired;
	BOOL m_bMinimizeBeforeAT;
	BOOL m_bDeleteBackupsOnSave;
	BOOL m_bShowFullPath;
	BOOL m_bDisableAutoType;
	BOOL m_bCopyURLs;
	BOOL m_bExitInsteadOfLockAT;
	BOOL m_bAllowSaveIfModifiedOnly;
	BOOL m_bCheckForUpdate;
	BOOL m_bLockOnWinLock;
	BOOL m_bEnableRemoteCtrl;
	BOOL m_bUseLocalTimeFormat;
	BOOL m_bRegisterRestoreHotKey;
	BOOL m_bFocusResAfterQuickFind;
	BOOL m_bAlwaysAllowIpc;
	BOOL m_bDropToBackOnCopy;
	BOOL m_bClearClipOnDbClose;
	BOOL m_bDeleteTANsAfterUse;
	BOOL m_bUseTransactedFileWrites;

	//{{AFX_DATA(COptionsDlg)
	enum { IDD = IDD_OPTIONS_DLG };
	COptionsList	m_olAdvanced;
	CButton	m_btnDeleteAssoc;
	CButton	m_btnCreateAssoc;
	CColourPickerXP	m_btnColorRowHighlight;
	CTabCtrl	m_tabMenu;
	CXPStyleButtonST	m_btSelFont;
	CXPStyleButtonST	m_btCancel;
	CXPStyleButtonST	m_btOK;
	CXPStyleButtonST	m_btnAutoType;
	int		m_nNewlineSequence;
	UINT	m_uClipboardSeconds;
	BOOL	m_bImgButtons;
	BOOL	m_bEntryGrid;
	BOOL	m_bLockOnMinimize;
	BOOL	m_bMinimizeToTray;
	BOOL	m_bLockAfterTime;
	UINT	m_nLockAfter;
	BOOL	m_bColAutoSize;
	BOOL	m_bCloseMinimizes;
	BOOL	m_bDisableUnsafe;
	BOOL	m_bUsePuttyForURLs;
	BOOL	m_bSaveOnLATMod;
	int		m_nClipboardMethod;
	BOOL	m_bSecureEdits;
	BOOL	m_bDefaultExpire;
	DWORD	m_dwDefaultExpire;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(COptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(COptionsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBtnSelFont();
	afx_msg void OnSelChangeTabMenu(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnCreateAssoc();
	afx_msg void OnBtnDeleteAssoc();
	afx_msg void OnRadioClipMethodSecure();
	afx_msg void OnRadioClipMethodTimed();
	afx_msg void OnCheckDefaultExpire();
	afx_msg void OnCheckLockAfterTime();
	afx_msg void OnBtnAutoType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // AFX_OPTIONSDLG_H__97A7BE00_1851_11D8_BF16_0050BF14F5CC__INCLUDED_
