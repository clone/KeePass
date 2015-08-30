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

#include "StdAfx.h"
#include "PwSafe.h"
#include "OptionsDlg.h"

#include "NewGUI/NewGUICommon.h"
#include "Util/WinUtil.h"
#include "Util/CmdLine/Executable.h"
#include "../KeePassLibCpp/Util/TranslateEx.h"
#include "../KeePassLibCpp/Crypto/MemoryProtectionEx.h"
#include "../KeePassLibCpp/Crypto/KeyTransform_BCrypt.h"
#include "OptionsAutoTypeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

COptionsDlg::COptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsDlg)
	m_nNewlineSequence = -1;
	m_uClipboardSeconds = 0;
	m_bImgButtons = FALSE;
	m_bEntryGrid = FALSE;
	m_bLockOnMinimize = FALSE;
	m_bMinimizeToTray = FALSE;
	m_bLockAfterTime = FALSE;
	m_nLockAfter = 0;
	m_bColAutoSize = FALSE;
	m_bCloseMinimizes = FALSE;
	m_bDisableUnsafe = FALSE;
	m_bUsePuttyForURLs = FALSE;
	m_bSaveOnLATMod = FALSE;
	m_nClipboardMethod = -1;
	m_bSecureEdits = FALSE;
	m_bDefaultExpire = FALSE;
	m_dwDefaultExpire = 0;
	//}}AFX_DATA_INIT

	m_pParentDlg = NULL;
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDlg)
	DDX_Control(pDX, IDC_LIST_ADVANCED, m_olAdvanced);
	DDX_Control(pDX, IDC_BTN_DELETEASSOC, m_btnDeleteAssoc);
	DDX_Control(pDX, IDC_BTN_CREATEASSOC, m_btnCreateAssoc);
	DDX_Control(pDX, IDC_BTN_ROWHIGHLIGHTSEL, m_btnColorRowHighlight);
	DDX_Control(pDX, IDC_TAB_MENU, m_tabMenu);
	DDX_Control(pDX, IDC_BTN_SELFONT, m_btSelFont);
	DDX_Control(pDX, IDC_BTN_SELNOTESFONT, m_btSelNotesFont);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Radio(pDX, IDC_RADIO_NEWLINE_0, m_nNewlineSequence);
	DDX_Text(pDX, IDC_EDIT_CLIPBOARDTIME, m_uClipboardSeconds);
	DDX_Check(pDX, IDC_CHECK_IMGBUTTONS, m_bImgButtons);
	DDX_Check(pDX, IDC_CHECK_ENTRYGRID, m_bEntryGrid);
	DDX_Check(pDX, IDC_CHECK_LOCKMIN, m_bLockOnMinimize);
	DDX_Check(pDX, IDC_CHECK_MINTRAY, m_bMinimizeToTray);
	DDX_Check(pDX, IDC_CHECK_LOCKAFTERTIME, m_bLockAfterTime);
	DDX_Text(pDX, IDC_EDIT_LOCKSECONDS, m_nLockAfter);
	DDX_Check(pDX, IDC_CHECK_COLAUTOSIZE, m_bColAutoSize);
	DDX_Check(pDX, IDC_CHECK_CLOSEMIN, m_bCloseMinimizes);
	DDX_Check(pDX, IDC_CHECK_DISABLEUNSAFE, m_bDisableUnsafe);
	DDX_Check(pDX, IDC_CHECK_PUTTYURLS, m_bUsePuttyForURLs);
	DDX_Check(pDX, IDC_CHECK_SAVEONLATMOD, m_bSaveOnLATMod);
	DDX_Radio(pDX, IDC_RADIO_CLIPMETHOD_TIMED, m_nClipboardMethod);
	DDX_Check(pDX, IDC_CHECK_SECUREEDITS, m_bSecureEdits);
	DDX_Check(pDX, IDC_CHECK_DEFAULTEXPIRE, m_bDefaultExpire);
	DDX_Text(pDX, IDC_EDIT_DEFAULTEXPIRE, m_dwDefaultExpire);
	DDX_Check(pDX, IDC_CHECK_LOCKONWINLOCK, m_bLockOnWinLock);
	DDX_Control(pDX, IDC_BTN_AUTOTYPE, m_btnAutoType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	//{{AFX_MSG_MAP(COptionsDlg)
	ON_BN_CLICKED(IDC_BTN_SELFONT, OnBtnSelFont)
	ON_BN_CLICKED(IDC_BTN_SELNOTESFONT, OnBtnSelNotesFont)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MENU, OnSelChangeTabMenu)
	ON_BN_CLICKED(IDC_BTN_CREATEASSOC, OnBtnCreateAssoc)
	ON_BN_CLICKED(IDC_BTN_DELETEASSOC, OnBtnDeleteAssoc)
	ON_BN_CLICKED(IDC_RADIO_CLIPMETHOD_SECURE, OnRadioClipMethodSecure)
	ON_BN_CLICKED(IDC_RADIO_CLIPMETHOD_TIMED, OnRadioClipMethodTimed)
	ON_BN_CLICKED(IDC_CHECK_DEFAULTEXPIRE, OnCheckDefaultExpire)
	ON_BN_CLICKED(IDC_CHECK_LOCKAFTERTIME, OnCheckLockAfterTime)
	ON_BN_CLICKED(IDC_BTN_AUTOTYPE, &COptionsDlg::OnBtnAutoType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL COptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	ASSERT(m_pParentDlg != NULL);

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	NewGUI_XPButton(m_btOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(m_btCancel, IDB_CANCEL, IDB_CANCEL);
	NewGUI_XPButton(m_btSelFont, IDB_DOCUMENT_SMALL, IDB_DOCUMENT_SMALL);
	NewGUI_XPButton(m_btSelNotesFont, IDB_DOCUMENT_SMALL, IDB_DOCUMENT_SMALL);
	// NewGUI_XPButton(m_btnCreateAssoc, IDB_FILE, IDB_FILE);
	// NewGUI_XPButton(m_btnDeleteAssoc, IDB_CANCEL, IDB_CANCEL);
	NewGUI_XPButton(m_btnAutoType, IDB_AUTOTYPE, IDB_AUTOTYPE);

	NewGUI_SetShield(m_btnCreateAssoc, TRUE);
	NewGUI_SetShield(m_btnDeleteAssoc, TRUE);

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_OPTIONS),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(TRL("Settings"));
	m_banner.SetCaption(TRL("Here you can configure KeePass."));

	m_wndgrp.AddWindow(GetDlgItem(IDC_STATIC_NEWLINETEXT), OPTGRP_FILES, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_RADIO_NEWLINE_0), OPTGRP_FILES, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_RADIO_NEWLINE_1), OPTGRP_FILES, TRUE);
	m_wndgrp.AddWindow(NULL, OPTGRP_FILES, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_SAVEONLATMOD), OPTGRP_FILES, TRUE);

	// m_wndgrp.AddWindow(GetDlgItem(IDC_STATIC_CLIPBOARDMETHOD), OPTGRP_MEMORY, TRUE);
	// m_wndgrp.AddWindow(GetDlgItem(IDC_RADIO_CLIPMETHOD_TIMED), OPTGRP_MEMORY, TRUE);
	// m_wndgrp.AddWindow(GetDlgItem(IDC_RADIO_CLIPMETHOD_SECURE), OPTGRP_MEMORY, TRUE);
	// m_wndgrp.AddWindow(NULL, OPTGRP_MEMORY, TRUE);
	GetDlgItem(IDC_STATIC_CLIPBOARDMETHOD)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RADIO_CLIPMETHOD_TIMED)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RADIO_CLIPMETHOD_SECURE)->ShowWindow(SW_HIDE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_STATIC_CLIPCLEARTXT), OPTGRP_MEMORY, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_EDIT_CLIPBOARDTIME), OPTGRP_MEMORY, TRUE);

	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_IMGBUTTONS), OPTGRP_GUI, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_ENTRYGRID), OPTGRP_GUI, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_COLAUTOSIZE), OPTGRP_GUI, TRUE);
	m_wndgrp.AddWindow(NULL, OPTGRP_GUI, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_MINTRAY), OPTGRP_GUI, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_CLOSEMIN), OPTGRP_GUI, TRUE);
	m_wndgrp.AddWindow(NULL, OPTGRP_GUI, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_STATIC_SELFONTTXT), OPTGRP_GUI, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_BTN_SELFONT), OPTGRP_GUI, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_BTN_SELNOTESFONT), OPTGRP_GUI, FALSE);
	m_wndgrp.AddWindow(NULL, OPTGRP_GUI, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_STATIC_SELROWHIGHLIGHT), OPTGRP_GUI, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_BTN_ROWHIGHLIGHTSEL), OPTGRP_GUI, TRUE);

	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_LOCKMIN), OPTGRP_SECURITY, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_LOCKONWINLOCK), OPTGRP_SECURITY, TRUE);
	m_wndgrp.AddWindow(NULL, OPTGRP_SECURITY, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_LOCKAFTERTIME), OPTGRP_SECURITY, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_EDIT_LOCKSECONDS), OPTGRP_SECURITY, TRUE);
	m_wndgrp.AddWindow(NULL, OPTGRP_SECURITY, TRUE);
	m_wndgrp.AddWindow(NULL, OPTGRP_SECURITY, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_DISABLEUNSAFE), OPTGRP_SECURITY, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_SECUREEDITS), OPTGRP_SECURITY, TRUE);
	m_wndgrp.AddWindow(NULL, OPTGRP_SECURITY, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_DEFAULTEXPIRE), OPTGRP_SECURITY, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_EDIT_DEFAULTEXPIRE), OPTGRP_SECURITY, TRUE);

	m_wndgrp.AddWindow(GetDlgItem(IDC_STATIC_ASSOC), OPTGRP_SETUP, TRUE);
	m_wndgrp.AddWindow(NULL, OPTGRP_SETUP, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_BTN_CREATEASSOC), OPTGRP_SETUP, TRUE);
	m_wndgrp.AddWindow(NULL, OPTGRP_SETUP, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_BTN_DELETEASSOC), OPTGRP_SETUP, TRUE);
	m_wndgrp.AddWindow(NULL, OPTGRP_SETUP, TRUE);
	m_wndgrp.AddWindow(NULL, OPTGRP_SETUP, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_PUTTYURLS), OPTGRP_SETUP, TRUE);
	m_wndgrp.AddWindow(NULL, OPTGRP_SETUP, TRUE);
	m_wndgrp.AddWindow(NULL, OPTGRP_SETUP, TRUE);
	// m_wndgrp.AddWindow(GetDlgItem(IDC_STATIC_AUTOTYPEHK), OPTGRP_SETUP, TRUE);
	// m_wndgrp.AddWindow(GetDlgItem(IDC_HOTKEY_AUTOTYPE), OPTGRP_SETUP, TRUE);

	m_wndgrp.AddWindow(GetDlgItem(IDC_LIST_ADVANCED), OPTGRP_ADVANCED, FALSE);
	m_wndgrp.AddWindow(&m_btnAutoType, OPTGRP_ADVANCED, FALSE);

	m_wndgrp.HideAllExcept(OPTGRP_SECURITY);
	m_wndgrp.ArrangeWindows(this);

	// m_ilIcons.Create(CPwSafeApp::GetClientIconsResourceID(), 16, 1, RGB(255,0,255));
	CPwSafeApp::CreateHiColorImageList(&m_ilIcons, IDB_CLIENTICONS_EX, 16);
	m_tabMenu.SetImageList(&m_ilIcons);

	// m_ilOptionIcons.Create(IDR_OPTIONICONS, 16, 1, RGB(255,0,255));
	CPwSafeApp::CreateHiColorImageList(&m_ilOptionIcons, IDB_OPTIONICONS_EX, 16);
	m_olAdvanced.InitOptionListEx(&m_ilOptionIcons, FALSE);

	m_olAdvanced.AddGroupText(TRL("Integration"), 9);
	m_olAdvanced.AddCheckItem(TRL("Start KeePass at Windows startup (for current user)"), &m_bStartWithWindows, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Single left click instead of double-click for default tray icon action"), &m_bSingleClickTrayIcon, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Show tray icon only if main window has been sent to tray"), &m_bShowTrayOnlyIfTrayed, NULL, OL_LINK_NULL);
	// m_olAdvanced.AddCheckItem(TRL("Use alternative auto-type method (minimize window)"), &m_bMinimizeBeforeAT, NULL, OL_LINK_NULL);
	// m_olAdvanced.AddCheckItem(TRL("Disable all auto-type features"), &m_bDisableAutoType, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Copy URLs to clipboard instead of launching them (exception: cmd:// URLs)"), &m_bCopyURLs, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Drop to background when copying data to the clipboard"), &m_bDropToBackOnCopy, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Enable remote control (allow applications to communicate with KeePass)"), &m_bEnableRemoteCtrl, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Always grant full access through remote control (not recommended)"), &m_bAlwaysAllowIpc, NULL, OL_LINK_NULL);

	m_olAdvanced.AddGroupText(_T(""), 0);
	m_olAdvanced.AddGroupText(TRL("Start and exit"), 7);
	m_olAdvanced.AddCheckItem(TRL("Remember last opened file"), &m_bRememberLast, &m_bOpenLastDb, OL_LINK_SAME_TRIGGER_FALSE);
	m_olAdvanced.AddCheckItem(TRL("Automatically open last used database on startup"), &m_bOpenLastDb, &m_bRememberLast, OL_LINK_SAME_TRIGGER_TRUE);
	m_olAdvanced.AddCheckItem(TRL("Start minimized and locked"), &m_bStartMinimized, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Automatically save when closing/locking the database"), &m_bAutoSave, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Limit to single instance"), &m_bSingleInstance, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Check for updates at KeePass startup"), &m_bCheckForUpdate, NULL, OL_LINK_NULL);

	m_olAdvanced.AddGroupText(_T(""), 0);
	m_olAdvanced.AddGroupText(TRL("Immediately after opening a database"), 8);
	m_olAdvanced.AddCheckItem(TRL("Show expired entries (if any)"), &m_bAutoShowExpired, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Show entries that will expire soon (if any)"), &m_bAutoShowExpiredSoon, NULL, OL_LINK_NULL);

	m_olAdvanced.AddGroupText(_T(""), 0);
	m_olAdvanced.AddGroupText(TRL("Backup"), 10);
	m_olAdvanced.AddCheckItem(TRL("Save backups of modified entries into the 'Backup' group"), &m_bBackupEntries, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Delete all backup entries before saving the database"), &m_bDeleteBackupsOnSave, NULL, OL_LINK_NULL);

	m_olAdvanced.AddGroupText(_T(""), 0);
	m_olAdvanced.AddGroupText(TRL("Quick search (toolbar)"), 12);
	m_olAdvanced.AddCheckItem(TRL("Include backup entries in quick searches"), &m_bQuickFindIncBackup, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Include expired entries in quick searches"), &m_bQuickFindIncExpired, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Focus entry list after a successful quick search"), &m_bFocusResAfterQuickFind, NULL, OL_LINK_NULL);

	m_olAdvanced.AddGroupText(_T(""), 0);
	m_olAdvanced.AddGroupText(TRL("Advanced"), 11);
	// m_olAdvanced.AddCheckItem(TRL("Automatically generate random passwords for new entries"), &m_bAutoPwGen, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Remember key sources (key file paths, provider names, ...)"), &m_bRememberKeySources, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Minimize main window after locking the workspace"), &m_bMinimizeOnLock, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Exit program instead of locking the workspace after the specified time"), &m_bExitInsteadOfLockAT, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Show full path in the title bar (instead of file name only)"), &m_bShowFullPath, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Disable 'Save' button if the database hasn't been modified"), &m_bAllowSaveIfModifiedOnly, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Use local date/time format instead of ISO notation"), &m_bUseLocalTimeFormat, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Register Ctrl-Alt-K hot key (brings the KeePass window to front)"), &m_bRegisterRestoreHotKey, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Delete TAN entries immediately after using them"), &m_bDeleteTANsAfterUse, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Clear clipboard when closing/locking the database"), &m_bClearClipOnDbClose, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Use file transactions for writing databases"), &m_bUseTransactedFileWrites, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Use advanced memory protection (DPAPI, only Windows Vista and higher)"), CMemoryProtectionEx::GetEnabledPtr(), NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Use CNG/BCrypt for key transformations (only Windows Vista and higher)"), CKeyTransformBCrypt::GetEnabledPtr(), NULL, OL_LINK_NULL);

	AddTcItem(TRL(OPTSZ_SECURITY), 29);
	AddTcItem(TRL(OPTSZ_GUI), 6);
	AddTcItem(TRL(OPTSZ_FILES), 26);
	AddTcItem(TRL(OPTSZ_MEMORY), 42);
	AddTcItem(TRL(OPTSZ_SETUP), 30);
	AddTcItem(TRL(OPTSZ_ADVANCED), 21);

	m_tabMenu.SetCurSel(0);

	m_btnColorRowHighlight.SetDefaultColor(RGB(238,238,255));
	m_btnColorRowHighlight.SetColor(m_rgbRowHighlight);

	m_btnColorRowHighlight.SetCustomText(TRL("More Colors..."));
	m_btnColorRowHighlight.SetDefaultText(TRL("Default"));

	if(m_dwDefaultExpire == 0)
	{
		m_bDefaultExpire = FALSE;
		GetDlgItem(IDC_EDIT_DEFAULTEXPIRE)->EnableWindow(FALSE);
	}
	else m_bDefaultExpire = TRUE;

	UpdateData(FALSE);
	OnCheckLockAfterTime();

	return TRUE;
}

void COptionsDlg::AddTcItem(LPCTSTR lpName, int iImageIndex)
{
	TCITEM tci;
	ZeroMemory(&tci, sizeof(TCITEM));
	tci.mask = TCIF_TEXT | TCIF_IMAGE;

	tci.cchTextMax = static_cast<int>(_tcslen(lpName));
	tci.pszText = const_cast<LPTSTR>(lpName);
	tci.iImage = iImageIndex;
	
	m_tabMenu.InsertItem(m_tabMenu.GetItemCount(), &tci);
}

void COptionsDlg::OnOK() 
{
	UpdateData(TRUE);

	m_rgbRowHighlight = m_btnColorRowHighlight.GetColor();
	if((m_bLockAfterTime == TRUE) && (m_nLockAfter < 5)) m_nLockAfter = 5;

	if(m_bDefaultExpire == FALSE) m_dwDefaultExpire = 0;

	m_ilIcons.DeleteImageList();
	m_ilOptionIcons.DeleteImageList();
	CDialog::OnOK();
}

void COptionsDlg::OnCancel() 
{
	m_ilIcons.DeleteImageList();
	m_ilOptionIcons.DeleteImageList();
	CDialog::OnCancel();
}

void COptionsDlg::_ChangeFont(CString& rSpec)
{
	CString strFontSpec = rSpec;
	if(strFontSpec.GetLength() == 0) strFontSpec = m_strFontSpec;
	if(strFontSpec.GetLength() == 0) { ASSERT(FALSE); return; }

	CString strFace, strSize, strFlags;
	int nChars = strFontSpec.ReverseFind(_T(';'));
	int nSizeEnd = strFontSpec.ReverseFind(_T(','));
	strFace = strFontSpec.Left(nChars);
	strSize = strFontSpec.Mid(nChars + 1, nSizeEnd - nChars - 1);
	strFlags = strFontSpec.Right(4);
	int nSize = _ttoi((LPCTSTR)strSize);
	int nWeight = FW_NORMAL;
	if(strFlags.GetAt(0) == _T('1')) nWeight = FW_BOLD;
	BYTE bItalic = (BYTE)((strFlags.GetAt(1) == _T('1')) ? TRUE : FALSE);
	BYTE bUnderlined = (BYTE)((strFlags.GetAt(2) == _T('1')) ? TRUE : FALSE);
	BYTE bStrikeOut = (BYTE)((strFlags.GetAt(3) == _T('1')) ? TRUE : FALSE);

	LOGFONT lf;
	CDC *pDC = GetDC();
	HDC hDC = pDC->m_hDC;
	ASSERT(hDC != NULL);
	if(hDC != NULL) lf.lfHeight = -MulDiv(nSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	else { ASSERT(FALSE); lf.lfHeight = -nSize; }
	ReleaseDC(pDC); pDC = NULL;

	lf.lfWidth = 0; lf.lfEscapement = 0; lf.lfOrientation = 0;
	lf.lfWeight = nWeight; lf.lfItalic = bItalic; lf.lfUnderline = bUnderlined;
	lf.lfStrikeOut = bStrikeOut; lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS; lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY; lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	_tcscpy_s(lf.lfFaceName, _countof(lf.lfFaceName), strFace);

	CFontDialog dlg(&lf);
	dlg.m_cf.Flags |= CF_NOSCRIPTSEL;

	if(dlg.DoModal() == IDOK)
	{
		int dSize = dlg.GetSize();
		dSize = ((dSize >= 0) ? dSize : -dSize);

		CString strNewFontSpec = dlg.GetFaceName(), strTemp;
		strNewFontSpec += _T(";");
		strTemp.Format(_T("%d"), dSize / 10);
		strNewFontSpec += strTemp;
		strNewFontSpec += _T(",");
		strNewFontSpec += ((dlg.IsBold() == TRUE) ? _T('1') : _T('0'));
		strNewFontSpec += ((dlg.IsItalic() == TRUE) ? _T('1') : _T('0'));
		strNewFontSpec += ((dlg.IsUnderline() == TRUE) ? _T('1') : _T('0'));
		strNewFontSpec += ((dlg.IsStrikeOut() == TRUE) ? _T('1') : _T('0'));

		rSpec = strNewFontSpec;
	}
}

void COptionsDlg::OnBtnSelFont() 
{
	_ChangeFont(m_strFontSpec);
}

void COptionsDlg::OnBtnSelNotesFont() 
{
	_ChangeFont(m_strNotesFontSpec);
}

void COptionsDlg::OnSelChangeTabMenu(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int nCurSel = m_tabMenu.GetCurSel();

	UNREFERENCED_PARAMETER(pNMHDR);

	UpdateData(TRUE);

	switch(nCurSel)
	{
	case OPTGRP_SECURITY:
		m_wndgrp.HideAllExcept(OPTGRP_SECURITY);
		if(m_bLockAfterTime == FALSE)
			GetDlgItem(IDC_EDIT_LOCKSECONDS)->EnableWindow(FALSE);
		if(m_bDefaultExpire == FALSE)
			GetDlgItem(IDC_EDIT_DEFAULTEXPIRE)->EnableWindow(FALSE);
		break;
	case OPTGRP_GUI:
		m_wndgrp.HideAllExcept(OPTGRP_GUI);
		break;
	case OPTGRP_FILES:
		m_wndgrp.HideAllExcept(OPTGRP_FILES);
		break;
	case OPTGRP_MEMORY:
		m_wndgrp.HideAllExcept(OPTGRP_MEMORY);
		if(m_nClipboardMethod != CM_TIMED)
			GetDlgItem(IDC_EDIT_CLIPBOARDTIME)->EnableWindow(FALSE);
		break;
	case OPTGRP_SETUP:
		m_wndgrp.HideAllExcept(OPTGRP_SETUP);
		// if(m_bDisableAutoType == TRUE)
		//	m_hkAutoType.EnableWindow(FALSE);
		break;
	case OPTGRP_ADVANCED:
		m_wndgrp.HideAllExcept(OPTGRP_ADVANCED);
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	*pResult = 0;
}

void COptionsDlg::OnBtnCreateAssoc() 
{
	// CPwSafeApp::ChangeKdbShellAssociation(TRUE, this->m_hWnd);
	WU_RunElevated(Executable::instance().getFullPathName().c_str(),
		KPCLOPT_FILEEXT_REG, this->m_hWnd);
}

void COptionsDlg::OnBtnDeleteAssoc() 
{
	// CPwSafeApp::ChangeKdbShellAssociation(FALSE, this->m_hWnd);
	WU_RunElevated(Executable::instance().getFullPathName().c_str(),
		KPCLOPT_FILEEXT_UNREG, this->m_hWnd);
}

void COptionsDlg::OnRadioClipMethodSecure() 
{
	CString str = TRL("Warning! It is possible that this option won't work correctly on your system, especially if you are using any clipboard enhancing or clipboard backup tools.");
	str += _T("\r\n\r\n");
	str += TRL("If you notice any problems with the enhanced method, switch back to the timed clipboard clearing method.");
	MessageBox((LPCTSTR)str, PWM_PRODUCT_NAME_SHORT, MB_ICONINFORMATION | MB_OK);

	UpdateData(TRUE);
	if(m_nClipboardMethod == CM_TIMED) GetDlgItem(IDC_EDIT_CLIPBOARDTIME)->EnableWindow(TRUE);
	else GetDlgItem(IDC_EDIT_CLIPBOARDTIME)->EnableWindow(FALSE);
}

void COptionsDlg::OnRadioClipMethodTimed() 
{
	UpdateData(TRUE);
	if(m_nClipboardMethod == CM_TIMED) GetDlgItem(IDC_EDIT_CLIPBOARDTIME)->EnableWindow(TRUE);
	else GetDlgItem(IDC_EDIT_CLIPBOARDTIME)->EnableWindow(FALSE);
}

void COptionsDlg::OnCheckDefaultExpire() 
{
	UpdateData(TRUE);

	if(m_bDefaultExpire == TRUE) GetDlgItem(IDC_EDIT_DEFAULTEXPIRE)->EnableWindow(TRUE);
	else GetDlgItem(IDC_EDIT_DEFAULTEXPIRE)->EnableWindow(FALSE);
}

void COptionsDlg::OnCheckLockAfterTime() 
{
	UpdateData(TRUE);

	if(m_bLockAfterTime == FALSE) GetDlgItem(IDC_EDIT_LOCKSECONDS)->EnableWindow(FALSE);
	else GetDlgItem(IDC_EDIT_LOCKSECONDS)->EnableWindow(TRUE);
}

void COptionsDlg::OnBtnAutoType()
{
	COptionsAutoTypeDlg dlg;

	dlg.m_pParentDlg = m_pParentDlg;
	dlg.m_bDisableAutoType = m_bDisableAutoType;
	dlg.m_bMinimizeBeforeAT = m_bMinimizeBeforeAT;
	dlg.m_dwATHotKey = m_dwATHotKey;
	dlg.m_strDefaultAutoTypeSequence = m_strDefaultAutoTypeSequence;
	dlg.m_bAutoTypeIEFix = m_bAutoTypeIEFix;
	dlg.m_bSameKL = m_bAutoTypeSameKL;
	dlg.m_bSortAutoTypeSelItems = m_bSortAutoTypeSelItems;

	if(dlg.DoModal() == IDOK)
	{
		m_bDisableAutoType = dlg.m_bDisableAutoType;
		m_bMinimizeBeforeAT = dlg.m_bMinimizeBeforeAT;
		m_dwATHotKey = dlg.m_dwATHotKey;
		m_strDefaultAutoTypeSequence = dlg.m_strDefaultAutoTypeSequence;
		m_bAutoTypeIEFix = dlg.m_bAutoTypeIEFix;
		m_bAutoTypeSameKL = dlg.m_bSameKL;
		m_bSortAutoTypeSelItems = ((dlg.m_bSortAutoTypeSelItems == FALSE) ? FALSE : TRUE);
	}
}
