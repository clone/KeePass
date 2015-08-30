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

#include "StdAfx.h"
#include "PwSafe.h"
#include "OptionsDlg.h"

#include "NewGUI/NewGUICommon.h"
#include "NewGUI/TranslateEx.h"

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
	//}}AFX_DATA_INIT

	m_pParentDlg = NULL;
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDlg)
	DDX_Control(pDX, IDC_LIST_ADVANCED, m_olAdvanced);
	DDX_Control(pDX, IDC_HOTKEY_AUTOTYPE, m_hkAutoType);
	DDX_Control(pDX, IDC_BTN_DELETEASSOC, m_btnDeleteAssoc);
	DDX_Control(pDX, IDC_BTN_CREATEASSOC, m_btnCreateAssoc);
	DDX_Control(pDX, IDC_BTN_ROWHIGHLIGHTSEL, m_btnColorRowHighlight);
	DDX_Control(pDX, IDC_TAB_MENU, m_tabMenu);
	DDX_Control(pDX, IDC_BTN_SELFONT, m_btSelFont);
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
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	//{{AFX_MSG_MAP(COptionsDlg)
	ON_BN_CLICKED(IDC_BTN_SELFONT, OnBtnSelFont)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MENU, OnSelChangeTabMenu)
	ON_BN_CLICKED(IDC_BTN_CREATEASSOC, OnBtnCreateAssoc)
	ON_BN_CLICKED(IDC_BTN_DELETEASSOC, OnBtnDeleteAssoc)
	ON_BN_CLICKED(IDC_RADIO_CLIPMETHOD_SECURE, OnRadioClipMethodSecure)
	ON_BN_CLICKED(IDC_RADIO_CLIPMETHOD_TIMED, OnRadioClipMethodTimed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL COptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	ASSERT(m_pParentDlg != NULL);

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	NewGUI_XPButton(&m_btOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(&m_btCancel, IDB_CANCEL, IDB_CANCEL);
	NewGUI_XPButton(&m_btSelFont, IDB_DOCUMENT_SMALL, IDB_DOCUMENT_SMALL);
	NewGUI_XPButton(&m_btnCreateAssoc, IDB_FILE, IDB_FILE);
	NewGUI_XPButton(&m_btnDeleteAssoc, IDB_CANCEL, IDB_CANCEL);

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

	m_wndgrp.AddWindow(GetDlgItem(IDC_STATIC_CLIPBOARDMETHOD), OPTGRP_MEMORY, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_RADIO_CLIPMETHOD_TIMED), OPTGRP_MEMORY, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_RADIO_CLIPMETHOD_SECURE), OPTGRP_MEMORY, TRUE);
	m_wndgrp.AddWindow(NULL, OPTGRP_MEMORY, TRUE);
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
	m_wndgrp.AddWindow(NULL, OPTGRP_GUI, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_STATIC_SELROWHIGHLIGHT), OPTGRP_GUI, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_BTN_ROWHIGHLIGHTSEL), OPTGRP_GUI, TRUE);

	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_LOCKMIN), OPTGRP_SECURITY, TRUE);
	m_wndgrp.AddWindow(NULL, OPTGRP_SECURITY, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_LOCKAFTERTIME), OPTGRP_SECURITY, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_EDIT_LOCKSECONDS), OPTGRP_SECURITY, TRUE);
	m_wndgrp.AddWindow(NULL, OPTGRP_SECURITY, TRUE);
	m_wndgrp.AddWindow(NULL, OPTGRP_SECURITY, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_DISABLEUNSAFE), OPTGRP_SECURITY, TRUE);

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
	m_wndgrp.AddWindow(GetDlgItem(IDC_STATIC_AUTOTYPEHK), OPTGRP_SETUP, TRUE);
	m_wndgrp.AddWindow(GetDlgItem(IDC_HOTKEY_AUTOTYPE), OPTGRP_SETUP, TRUE);

	m_wndgrp.AddWindow(GetDlgItem(IDC_LIST_ADVANCED), OPTGRP_ADVANCED, FALSE);

	m_wndgrp.HideAllExcept(OPTGRP_SECURITY);
	m_wndgrp.ArrangeWindows(this);

	m_ilIcons.Create(IDR_CLIENTICONS, 16, 1, RGB(255,0,255));
	m_tabMenu.SetImageList(&m_ilIcons);

	m_ilOptionIcons.Create(IDR_OPTIONICONS, 16, 1, RGB(255,0,255));
	m_olAdvanced.InitOptionListEx(&m_ilOptionIcons);

	m_olAdvanced.AddGroupText(TRL("Integration"), 9);
	m_olAdvanced.AddCheckItem(TRL("Start KeePass at Windows startup (for current user)"), &m_bStartWithWindows, NULL, OL_LINK_NULL);

	m_olAdvanced.AddGroupText(TRL(""), 0);
	m_olAdvanced.AddGroupText(TRL("Start and Exit"), 7);
	m_olAdvanced.AddCheckItem(TRL("Remember last opened file"), &m_bRememberLast, &m_bOpenLastDb, OL_LINK_SAME_TRIGGER_FALSE);
	m_olAdvanced.AddCheckItem(TRL("Automatically open last used database on startup"), &m_bOpenLastDb, &m_bRememberLast, OL_LINK_SAME_TRIGGER_TRUE);
	m_olAdvanced.AddCheckItem(TRL("Start minimized and locked"), &m_bStartMinimized, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Automatically save database on exit"), &m_bAutoSave, NULL, OL_LINK_NULL);

	m_olAdvanced.AddGroupText(TRL(""), 0);
	m_olAdvanced.AddGroupText(TRL("Immediately after opening a database"), 8);
	m_olAdvanced.AddCheckItem(TRL("Show expired entries (if any)"), &m_bAutoShowExpired, NULL, OL_LINK_NULL);
	m_olAdvanced.AddCheckItem(TRL("Show entries that will expire soon (if any)"), &m_bAutoShowExpiredSoon, NULL, OL_LINK_NULL);

	m_olAdvanced.AddGroupText(TRL(""), 0);
	m_olAdvanced.AddGroupText(TRL("Backup"), 10);
	m_olAdvanced.AddCheckItem(TRL("Save backups of modified entries into the 'Backup' group"), &m_bBackupEntries, NULL, OL_LINK_NULL);

	TCITEM tci;
	ZeroMemory(&tci, sizeof(TCITEM));
	tci.mask = TCIF_TEXT | TCIF_IMAGE;

	tci.cchTextMax = _tcslen(TRL(OPTSZ_SECURITY)); tci.pszText = (char *)TRL(OPTSZ_SECURITY);
	tci.iImage = 29; m_tabMenu.InsertItem(m_tabMenu.GetItemCount(), &tci);
	tci.cchTextMax = _tcslen(TRL(OPTSZ_GUI)); tci.pszText = (char *)TRL(OPTSZ_GUI);
	tci.iImage = 6; m_tabMenu.InsertItem(m_tabMenu.GetItemCount(), &tci);
	tci.cchTextMax = _tcslen(TRL(OPTSZ_FILES)); tci.pszText = (char *)TRL(OPTSZ_FILES);
	tci.iImage = 26; m_tabMenu.InsertItem(m_tabMenu.GetItemCount(), &tci);
	tci.cchTextMax = _tcslen(TRL(OPTSZ_MEMORY)); tci.pszText = (char *)TRL(OPTSZ_MEMORY);
	tci.iImage = 42; m_tabMenu.InsertItem(m_tabMenu.GetItemCount(), &tci);
	tci.cchTextMax = _tcslen(TRL(OPTSZ_SETUP)); tci.pszText = (char *)TRL(OPTSZ_SETUP);
	tci.iImage = 30; m_tabMenu.InsertItem(m_tabMenu.GetItemCount(), &tci);
	tci.cchTextMax = _tcslen(TRL(OPTSZ_ADVANCED)); tci.pszText = (char *)TRL(OPTSZ_ADVANCED);
	tci.iImage = 21; m_tabMenu.InsertItem(m_tabMenu.GetItemCount(), &tci);

	m_tabMenu.SetCurSel(0);

	m_hkAutoType.SetRules(HKCOMB_NONE | HKCOMB_S, HOTKEYF_CONTROL | HOTKEYF_ALT);
	m_hkAutoType.SetHotKey((WORD)(m_dwATHotKey & 0x0000FFFF), (WORD)(m_dwATHotKey >> 16));

	m_btnColorRowHighlight.SetDefaultColor(RGB(238,238,255));
	m_btnColorRowHighlight.SetColor(m_rgbRowHighlight);

	m_btnColorRowHighlight.SetCustomText(TRL("More Colors..."));
	m_btnColorRowHighlight.SetDefaultText(TRL("Default"));

	UpdateData(FALSE);

	return TRUE;
}

void COptionsDlg::OnOK() 
{
	UpdateData(TRUE);

	WORD wVK = 0, wMod = 0;
	m_hkAutoType.GetHotKey(wVK, wMod);
	DWORD dwNewHotKey = ((DWORD)wMod << 16) | (DWORD)wVK;
	if(dwNewHotKey != m_dwATHotKey)
	{
		if(m_pParentDlg->RegisterGlobalHotKey(HOTKEYID_AUTOTYPE, dwNewHotKey, (m_dwATHotKey != 0) ? TRUE : FALSE) == FALSE)
			return;

		m_dwATHotKey = dwNewHotKey;
	}

	m_rgbRowHighlight = m_btnColorRowHighlight.GetColor();
	if((m_bLockAfterTime == TRUE) && (m_nLockAfter < 5)) m_nLockAfter = 5;

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

void COptionsDlg::OnBtnSelFont() 
{
	CString strFontSpec = m_strFontSpec;
	CString strFace, strSize, strFlags;
	int nChars = strFontSpec.ReverseFind(_T(';'));
	int nSizeEnd = strFontSpec.ReverseFind(_T(','));
	strFace = strFontSpec.Left(nChars);
	strSize = strFontSpec.Mid(nChars + 1, nSizeEnd - nChars - 1);
	strFlags = strFontSpec.Right(4);
	int nSize = atoi((LPCTSTR)strSize);
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
	ReleaseDC(pDC);

	lf.lfWidth = 0; lf.lfEscapement = 0; lf.lfOrientation = 0;
	lf.lfWeight = nWeight; lf.lfItalic = bItalic; lf.lfUnderline = bUnderlined;
	lf.lfStrikeOut = bStrikeOut; lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS; lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY; lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	strcpy(lf.lfFaceName, (LPCTSTR)strFace);

	CFontDialog dlg(&lf);
	CString strTemp;

	if(dlg.DoModal() == IDOK)
	{
		int dSize = dlg.GetSize();
		dSize = (dSize >= 0) ? dSize : -dSize;
		m_strFontSpec = dlg.GetFaceName();
		m_strFontSpec += _T(";");
		strTemp.Format(_T("%d"), dSize / 10);
		m_strFontSpec += strTemp;
		m_strFontSpec += _T(",");
		m_strFontSpec += (dlg.IsBold() == TRUE) ? _T('1') : _T('0');
		m_strFontSpec += (dlg.IsItalic() == TRUE) ? _T('1') : _T('0');
		m_strFontSpec += (dlg.IsUnderline() == TRUE) ? _T('1') : _T('0');
		m_strFontSpec += (dlg.IsStrikeOut() == TRUE) ? _T('1') : _T('0');
	}
}

void COptionsDlg::OnSelChangeTabMenu(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int n = m_tabMenu.GetCurSel();

	UNREFERENCED_PARAMETER(pNMHDR);

	switch(n)
	{
	case OPTGRP_SECURITY:
		m_wndgrp.HideAllExcept(OPTGRP_SECURITY);
		break;
	case OPTGRP_GUI:
		m_wndgrp.HideAllExcept(OPTGRP_GUI);
		break;
	case OPTGRP_FILES:
		m_wndgrp.HideAllExcept(OPTGRP_FILES);
		break;
	case OPTGRP_MEMORY:
		m_wndgrp.HideAllExcept(OPTGRP_MEMORY);
		UpdateData(TRUE);
		if(m_nClipboardMethod != CM_TIMED)
			GetDlgItem(IDC_EDIT_CLIPBOARDTIME)->EnableWindow(FALSE);
		break;
	case OPTGRP_SETUP:
		m_wndgrp.HideAllExcept(OPTGRP_SETUP);
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
	if(CPwSafeApp::RegisterShellAssociation() == TRUE)
	{
		NotifyAssocChanged();

		MessageBox(TRL("Successfully associated KeePass with .kdb files! A double-click on a .kdb file will now start KeePass automatically!"),
			TRL("Password Safe"), MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		MessageBox(TRL("Failed to change the .kdb file association. Make sure you have the rights to write to the registry and change file associations."),
			TRL("Password Safe"), MB_OK | MB_ICONWARNING);
	}
}

void COptionsDlg::OnBtnDeleteAssoc() 
{
	if(CPwSafeApp::UnregisterShellAssociation() == TRUE)
	{
		NotifyAssocChanged();

		MessageBox(TRL("Successfully removed association! KeePass won't be started any more when double-clicking on a .kdb file!"),
			TRL("Password Safe"), MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		MessageBox(TRL("Failed to change the .kdb file association. Make sure you have the rights to write to the registry and change file associations."),
			TRL("Password Safe"), MB_OK | MB_ICONWARNING);
	}
}

void COptionsDlg::NotifyAssocChanged()
{
	LPSHCHANGENOTIFY lpSHChangeNotify;
	HINSTANCE hShell32;

	hShell32 = LoadLibrary(_T("Shell32.dll"));
	if(hShell32 != NULL)
	{
		lpSHChangeNotify = (LPSHCHANGENOTIFY)GetProcAddress(hShell32, _T("SHChangeNotify"));

		if(lpSHChangeNotify != NULL)
		{
			lpSHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
		}
		else { ASSERT(FALSE); }

		FreeLibrary(hShell32);
	}
	else { ASSERT(FALSE); }
}

void COptionsDlg::OnRadioClipMethodSecure() 
{
	CString str = TRL("Warning! It's possible that this option won't work correctly on your system, especially if you are using any clipboard enhancing tools or something like this.");
	str += _T("\r\n\r\n");
	str += TRL("If you notice any problems with this enhanced method, just switch back to the timed clipboard clearing method.");
	MessageBox((LPCTSTR)str, TRL("Password Safe"), MB_ICONINFORMATION | MB_OK);

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
