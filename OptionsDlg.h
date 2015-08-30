/*
  Copyright (c) 2003-2005, Dominik Reichl <dominik.reichl@t-online.de>
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer. 
  - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  - Neither the name of ReichlSoft nor the names of its contributors may be
    used to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef AFX_OPTIONSDLG_H__97A7BE00_1851_11D8_BF16_0050BF14F5CC__INCLUDED_
#define AFX_OPTIONSDLG_H__97A7BE00_1851_11D8_BF16_0050BF14F5CC__INCLUDED_

#include "NewGUI/KCSideBannerWnd.h"
#include "NewGUI/WindowGroups.h"
#include "NewGUI/ColourPickerXP.h"
#include "NewGUI/XPStyleButtonST.h"
#include "PwSafeDlg.h"

#define OPTGRP_SECURITY  0
#define OPTGRP_STARTEXIT 1
#define OPTGRP_GUI       2
#define OPTGRP_FILES     3
#define OPTGRP_MEMORY    4
#define OPTGRP_SETUP     5

// No _T, will be TRL-ed
#define OPTSZ_FILES     "Files"
#define OPTSZ_MEMORY    "Memory"
#define OPTSZ_STARTEXIT "Start and Exit"
#define OPTSZ_GUI       "Interface (GUI)"
#define OPTSZ_SECURITY  "Security"
#define OPTSZ_SETUP     "Setup"

/////////////////////////////////////////////////////////////////////////////

class CPP_CLASS_SHARE COptionsDlg : public CDialog
{
public:
	COptionsDlg(CWnd* pParent = NULL);
	CPwSafeDlg *m_pParentDlg;

	CKCSideBannerWnd m_banner;

	CString m_strFontSpec;
	CWindowGroups m_wndgrp;
	CImageList m_ilIcons;

	COLORREF m_rgbRowHighlight;

	DWORD m_dwATHotKey;

	static void NotifyAssocChanged();

	//{{AFX_DATA(COptionsDlg)
	enum { IDD = IDD_OPTIONS_DLG };
	CHotKeyCtrl	m_hkAutoType;
	CXPStyleButtonST	m_btnDeleteAssoc;
	CXPStyleButtonST	m_btnCreateAssoc;
	CColourPickerXP	m_btnColorRowHighlight;
	CTabCtrl	m_tabMenu;
	CXPStyleButtonST	m_btSelFont;
	CXPStyleButtonST	m_btCancel;
	CXPStyleButtonST	m_btOK;
	int		m_nNewlineSequence;
	UINT	m_uClipboardSeconds;
	BOOL	m_bOpenLastDb;
	BOOL	m_bImgButtons;
	BOOL	m_bEntryGrid;
	BOOL	m_bAutoSave;
	BOOL	m_bLockOnMinimize;
	BOOL	m_bMinimizeToTray;
	BOOL	m_bLockAfterTime;
	UINT	m_nLockAfter;
	BOOL	m_bColAutoSize;
	BOOL	m_bCloseMinimizes;
	BOOL	m_bDisableUnsafe;
	BOOL	m_bRememberLast;
	BOOL	m_bUsePuttyForURLs;
	BOOL	m_bSaveOnLATMod;
	int		m_nClipboardMethod;
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
	afx_msg void OnCheckRememberLast();
	afx_msg void OnCheckAutoOpenLastDb();
	afx_msg void OnRadioClipMethodSecure();
	afx_msg void OnRadioClipMethodTimed();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // AFX_OPTIONSDLG_H__97A7BE00_1851_11D8_BF16_0050BF14F5CC__INCLUDED_
