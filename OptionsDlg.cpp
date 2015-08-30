/*
  Copyright (c) 2003/2004, Dominik Reichl <dominik.reichl@t-online.de>
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
	m_bOpenLastDb = FALSE;
	m_bImgButtons = FALSE;
	m_bEntryGrid = FALSE;
	m_bAutoSave = FALSE;
	m_bLockOnMinimize = FALSE;
	m_bMinimizeToTray = FALSE;
	m_bLockAfterTime = FALSE;
	m_nLockAfter = 0;
	m_bColAutoSize = FALSE;
	m_bCloseMinimizes = FALSE;
	m_bDisableUnsafe = FALSE;
	m_bRememberLast = FALSE;
	m_bUsePuttyForURLs = FALSE;
	m_bSaveOnLATMod = FALSE;
	//}}AFX_DATA_INIT
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDlg)
	DDX_Control(pDX, IDC_BTN_DELETEASSOC, m_btnDeleteAssoc);
	DDX_Control(pDX, IDC_BTN_CREATEASSOC, m_btnCreateAssoc);
	DDX_Control(pDX, IDC_BTN_ROWHIGHLIGHTSEL, m_btnColorRowHighlight);
	DDX_Control(pDX, IDC_TAB_MENU, m_tabMenu);
	DDX_Control(pDX, IDC_BTN_SELFONT, m_btSelFont);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Radio(pDX, IDC_RADIO_NEWLINE_0, m_nNewlineSequence);
	DDX_Text(pDX, IDC_EDIT_CLIPBOARDTIME, m_uClipboardSeconds);
	DDX_Check(pDX, IDC_CHECK_AUTOOPENLASTDB, m_bOpenLastDb);
	DDX_Check(pDX, IDC_CHECK_IMGBUTTONS, m_bImgButtons);
	DDX_Check(pDX, IDC_CHECK_ENTRYGRID, m_bEntryGrid);
	DDX_Check(pDX, IDC_CHECK_AUTOSAVE, m_bAutoSave);
	DDX_Check(pDX, IDC_CHECK_LOCKMIN, m_bLockOnMinimize);
	DDX_Check(pDX, IDC_CHECK_MINTRAY, m_bMinimizeToTray);
	DDX_Check(pDX, IDC_CHECK_LOCKAFTERTIME, m_bLockAfterTime);
	DDX_Text(pDX, IDC_EDIT_LOCKSECONDS, m_nLockAfter);
	DDX_Check(pDX, IDC_CHECK_COLAUTOSIZE, m_bColAutoSize);
	DDX_Check(pDX, IDC_CHECK_CLOSEMIN, m_bCloseMinimizes);
	DDX_Check(pDX, IDC_CHECK_DISABLEUNSAFE, m_bDisableUnsafe);
	DDX_Check(pDX, IDC_CHECK_REMEMBERLAST, m_bRememberLast);
	DDX_Check(pDX, IDC_CHECK_PUTTYURLS, m_bUsePuttyForURLs);
	DDX_Check(pDX, IDC_CHECK_SAVEONLATMOD, m_bSaveOnLATMod);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	//{{AFX_MSG_MAP(COptionsDlg)
	ON_BN_CLICKED(IDC_BTN_SELFONT, OnBtnSelFont)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MENU, OnSelChangeTabMenu)
	ON_BN_CLICKED(IDC_BTN_CREATEASSOC, OnBtnCreateAssoc)
	ON_BN_CLICKED(IDC_BTN_DELETEASSOC, OnBtnDeleteAssoc)
	ON_BN_CLICKED(IDC_CHECK_REMEMBERLAST, OnCheckRememberLast)
	ON_BN_CLICKED(IDC_CHECK_AUTOOPENLASTDB, OnCheckAutoOpenLastDb)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL COptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	NewGUI_Button(&m_btOK, IDB_OK, IDB_OK);
	NewGUI_Button(&m_btCancel, IDB_CANCEL, IDB_CANCEL);
	NewGUI_Button(&m_btSelFont, IDB_DOCUMENT_SMALL, IDB_DOCUMENT_SMALL);
	NewGUI_Button(&m_btnCreateAssoc, IDB_FILE, IDB_FILE);
	NewGUI_Button(&m_btnDeleteAssoc, IDB_CANCEL, IDB_CANCEL);

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_OPTIONS),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(TRL("Settings"));
	m_banner.SetCaption(TRL("Here you can configure KeePass."));

	m_wndgrp.AddWindow(GetDlgItem(IDC_STATIC_NEWLINETEXT), OPTGRP_FILES);
	m_wndgrp.AddWindow(GetDlgItem(IDC_RADIO_NEWLINE_0), OPTGRP_FILES);
	m_wndgrp.AddWindow(GetDlgItem(IDC_RADIO_NEWLINE_1), OPTGRP_FILES);
	m_wndgrp.AddWindow(NULL, OPTGRP_FILES);
	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_SAVEONLATMOD), OPTGRP_FILES);

	m_wndgrp.AddWindow(GetDlgItem(IDC_STATIC_CLIPCLEARTXT), OPTGRP_MEMORY);
	m_wndgrp.AddWindow(GetDlgItem(IDC_EDIT_CLIPBOARDTIME), OPTGRP_MEMORY);

	m_wndgrp.AddWindow(GetDlgItem(IDC_STATIC_START), OPTGRP_STARTEXIT);
	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_AUTOOPENLASTDB), OPTGRP_STARTEXIT);
	m_wndgrp.AddWindow(NULL, OPTGRP_STARTEXIT);
	m_wndgrp.AddWindow(GetDlgItem(IDC_STATIC_EXIT), OPTGRP_STARTEXIT);
	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_AUTOSAVE), OPTGRP_STARTEXIT);
	m_wndgrp.AddWindow(NULL, OPTGRP_STARTEXIT);
	m_wndgrp.AddWindow(NULL, OPTGRP_STARTEXIT);
	m_wndgrp.AddWindow(NULL, OPTGRP_STARTEXIT);
	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_REMEMBERLAST), OPTGRP_STARTEXIT);

	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_IMGBUTTONS), OPTGRP_GUI);
	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_ENTRYGRID), OPTGRP_GUI);
	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_COLAUTOSIZE), OPTGRP_GUI);
	m_wndgrp.AddWindow(NULL, OPTGRP_GUI);
	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_MINTRAY), OPTGRP_GUI);
	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_CLOSEMIN), OPTGRP_GUI);
	m_wndgrp.AddWindow(NULL, OPTGRP_GUI);
	m_wndgrp.AddWindow(GetDlgItem(IDC_STATIC_SELFONTTXT), OPTGRP_GUI);
	m_wndgrp.AddWindow(GetDlgItem(IDC_BTN_SELFONT), OPTGRP_GUI);
	m_wndgrp.AddWindow(NULL, OPTGRP_GUI);
	m_wndgrp.AddWindow(GetDlgItem(IDC_STATIC_SELROWHIGHLIGHT), OPTGRP_GUI);
	m_wndgrp.AddWindow(GetDlgItem(IDC_BTN_ROWHIGHLIGHTSEL), OPTGRP_GUI);

	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_LOCKMIN), OPTGRP_SECURITY);
	m_wndgrp.AddWindow(NULL, OPTGRP_SECURITY);
	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_LOCKAFTERTIME), OPTGRP_SECURITY);
	m_wndgrp.AddWindow(GetDlgItem(IDC_EDIT_LOCKSECONDS), OPTGRP_SECURITY);
	m_wndgrp.AddWindow(NULL, OPTGRP_SECURITY);
	m_wndgrp.AddWindow(NULL, OPTGRP_SECURITY);
	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_DISABLEUNSAFE), OPTGRP_SECURITY);

	m_wndgrp.AddWindow(GetDlgItem(IDC_STATIC_ASSOC), OPTGRP_SETUP);
	m_wndgrp.AddWindow(NULL, OPTGRP_SETUP);
	m_wndgrp.AddWindow(GetDlgItem(IDC_BTN_CREATEASSOC), OPTGRP_SETUP);
	m_wndgrp.AddWindow(NULL, OPTGRP_SETUP);
	m_wndgrp.AddWindow(GetDlgItem(IDC_BTN_DELETEASSOC), OPTGRP_SETUP);
	m_wndgrp.AddWindow(NULL, OPTGRP_SETUP);
	m_wndgrp.AddWindow(NULL, OPTGRP_SETUP);
	m_wndgrp.AddWindow(GetDlgItem(IDC_CHECK_PUTTYURLS), OPTGRP_SETUP);

	m_wndgrp.HideAllExcept(OPTGRP_SECURITY);
	m_wndgrp.ArrangeWindows(this);

	m_ilIcons.Create(IDR_CLIENTICONS, 16, 1, RGB(255,0,255));
	m_tabMenu.SetImageList(&m_ilIcons);

	TCITEM tci;
	ZeroMemory(&tci, sizeof(TCITEM));
	tci.mask = TCIF_TEXT | TCIF_IMAGE;

	tci.cchTextMax = _tcslen(TRL(OPTSZ_SECURITY)); tci.pszText = (char *)TRL(OPTSZ_SECURITY);
	tci.iImage = 29; m_tabMenu.InsertItem(m_tabMenu.GetItemCount(), &tci);
	tci.cchTextMax = _tcslen(TRL(OPTSZ_STARTEXIT)); tci.pszText = (char *)TRL(OPTSZ_STARTEXIT);
	tci.iImage = 34; m_tabMenu.InsertItem(m_tabMenu.GetItemCount(), &tci);
	tci.cchTextMax = _tcslen(TRL(OPTSZ_GUI)); tci.pszText = (char *)TRL(OPTSZ_GUI);
	tci.iImage = 6; m_tabMenu.InsertItem(m_tabMenu.GetItemCount(), &tci);
	tci.cchTextMax = _tcslen(TRL(OPTSZ_FILES)); tci.pszText = (char *)TRL(OPTSZ_FILES);
	tci.iImage = 26; m_tabMenu.InsertItem(m_tabMenu.GetItemCount(), &tci);
	tci.cchTextMax = _tcslen(TRL(OPTSZ_MEMORY)); tci.pszText = (char *)TRL(OPTSZ_MEMORY);
	tci.iImage = 42; m_tabMenu.InsertItem(m_tabMenu.GetItemCount(), &tci);
	tci.cchTextMax = _tcslen(TRL(OPTSZ_SETUP)); tci.pszText = (char *)TRL(OPTSZ_SETUP);
	tci.iImage = 30; m_tabMenu.InsertItem(m_tabMenu.GetItemCount(), &tci);

	m_tabMenu.SetCurSel(0);

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
	m_rgbRowHighlight = m_btnColorRowHighlight.GetColor();
	m_ilIcons.DeleteImageList();
	if((m_bLockAfterTime == TRUE) && (m_nLockAfter < 5)) m_nLockAfter = 5;
	CDialog::OnOK();
}

void COptionsDlg::OnCancel() 
{
	m_ilIcons.DeleteImageList();
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
	case OPTGRP_STARTEXIT:
		m_wndgrp.HideAllExcept(OPTGRP_STARTEXIT);
		break;
	case OPTGRP_GUI:
		m_wndgrp.HideAllExcept(OPTGRP_GUI);
		break;
	case OPTGRP_FILES:
		m_wndgrp.HideAllExcept(OPTGRP_FILES);
		break;
	case OPTGRP_MEMORY:
		m_wndgrp.HideAllExcept(OPTGRP_MEMORY);
		break;
	case OPTGRP_SECURITY:
		m_wndgrp.HideAllExcept(OPTGRP_SECURITY);
		break;
	case OPTGRP_SETUP:
		m_wndgrp.HideAllExcept(OPTGRP_SETUP);
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
}

void COptionsDlg::OnBtnDeleteAssoc() 
{
	if(CPwSafeApp::UnregisterShellAssociation() == TRUE)
	{
		NotifyAssocChanged();

		MessageBox(TRL("Successfully removed association! KeePass won't be started any more when double-clicking on a .kdb file!"),
			TRL("Password Safe"), MB_OK | MB_ICONINFORMATION);
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

void COptionsDlg::OnCheckRememberLast() 
{
	UpdateData(TRUE);
	if(m_bRememberLast == FALSE) m_bOpenLastDb = FALSE;
	UpdateData(FALSE);
}

void COptionsDlg::OnCheckAutoOpenLastDb() 
{
	UpdateData(TRUE);
	if(m_bOpenLastDb == TRUE) m_bRememberLast = TRUE;
	UpdateData(FALSE);
}
