/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2005 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "LanguagesDlg.h"

#include "PwSafe/PwManager.h"
#include "NewGUI/TranslateEx.h"
#include "Util/PrivateConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CLanguagesDlg::CLanguagesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLanguagesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLanguagesDlg)
	//}}AFX_DATA_INIT
}

void CLanguagesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLanguagesDlg)
	DDX_Control(pDX, IDC_BTN_GETLANGUAGE, m_btGetLang);
	DDX_Control(pDX, IDC_LANGUAGES_LIST, m_listLang);
	DDX_Control(pDX, IDCANCEL, m_btClose);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLanguagesDlg, CDialog)
	//{{AFX_MSG_MAP(CLanguagesDlg)
	ON_NOTIFY(NM_CLICK, IDC_LANGUAGES_LIST, OnClickLanguagesList)
	ON_BN_CLICKED(IDC_BTN_GETLANGUAGE, OnBtnGetLanguage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CLanguagesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);
	
	NewGUI_XPButton(&m_btClose, IDB_CANCEL, IDB_CANCEL);
	NewGUI_XPButton(&m_btGetLang, IDB_LANGUAGE, IDB_LANGUAGE);

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_WORLD),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(TRL("Load a language file"));
	m_banner.SetCaption(TRL("Select one of the languages in the list below."));

	RECT rcList;
	m_listLang.GetWindowRect(&rcList);
	int nColSize = rcList.right - rcList.left - GetSystemMetrics(SM_CXVSCROLL) - 8;
	nColSize /= 4;
	m_listLang.InsertColumn(0, TRL("Available Languages"), LVCFMT_LEFT, nColSize, 0);
	m_listLang.InsertColumn(1, TRL("Language file version"), LVCFMT_LEFT, nColSize, 1);
	m_listLang.InsertColumn(2, TRL("Author"), LVCFMT_LEFT, nColSize, 2);
	m_listLang.InsertColumn(3, TRL("Translation author contact"), LVCFMT_LEFT, nColSize, 3);

	m_ilIcons.Create(IDR_CLIENTICONS, 16, 1, RGB(255,0,255));
	m_listLang.SetImageList(&m_ilIcons, LVSIL_SMALL);

	m_listLang.PostMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0,
		LVS_EX_SI_REPORT | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE | LVS_EX_UNDERLINEHOT);

	LV_ITEM lvi;

	m_listLang.DeleteAllItems();
	lvi.iItem = m_listLang.InsertItem(LVIF_TEXT | LVIF_IMAGE, m_listLang.GetItemCount(),
		"English", 0, 0, 1, NULL);

	CString strTemp;
	
	strTemp = PWM_VERSION_STR;
	lvi.iSubItem = 1; lvi.mask = LVIF_TEXT;
	lvi.pszText = (LPSTR)(LPCTSTR)strTemp;
	m_listLang.SetItem(&lvi);

	strTemp = _T("Dominik Reichl");
	lvi.iSubItem = 2; lvi.mask = LVIF_TEXT;
	lvi.pszText = (LPSTR)(LPCTSTR)strTemp;
	m_listLang.SetItem(&lvi);

	strTemp = _T("dominik.reichl@t-online.de, http://www.dominik-reichl.de/");
	lvi.iSubItem = 3; lvi.mask = LVIF_TEXT;
	lvi.pszText = (LPSTR)(LPCTSTR)strTemp;
	m_listLang.SetItem(&lvi);

	CFileFind ff;
	TCHAR szThis[1024];
	unsigned long i = 0;
	CString csTmp;
	BOOL chk_w = FALSE;
	TCHAR szCurrentlyLoaded[MAX_PATH * 2];

	_tcscpy(szCurrentlyLoaded, GetCurrentTranslationTable());

	GetModuleFileName(NULL, szThis, 1024);
	for(i = _tcslen(szThis) - 1; i > 1; i--) // Extract dir
	{
		if(szThis[i] == _T('\\')) { szThis[i] = 0; break; }
	}
	_tcscat(szThis, _T("\\*.lng"));

	chk_w = ff.FindFile(szThis, 0);
	while(chk_w == TRUE)
	{
		chk_w = ff.FindNextFile();

		csTmp = ff.GetFileTitle();
		csTmp.MakeLower();
		if((csTmp != _T("standard")) && (csTmp != _T("english")))
		{
			LoadTranslationTable((LPCTSTR)ff.GetFileTitle());

			strTemp = (LPCTSTR)ff.GetFileTitle();
			// strTemp += _T(" - ");
			// strTemp += TRL("~LANGUAGENAME");

			lvi.iItem = m_listLang.InsertItem(LVIF_TEXT | LVIF_IMAGE,
				m_listLang.GetItemCount(), strTemp, 0, 0, 1, NULL);

			strTemp = TRL("~LANGUAGEVERSION");
			if(strTemp == _T("~LANGUAGEVERSION")) strTemp.Empty();
			lvi.iSubItem = 1; lvi.mask = LVIF_TEXT;
			lvi.pszText = (LPSTR)(LPCTSTR)strTemp;
			m_listLang.SetItem(&lvi);

			strTemp = TRL("~LANGUAGEAUTHOR");
			if(strTemp == _T("~LANGUAGEAUTHOR")) strTemp.Empty();
			lvi.iSubItem = 2; lvi.mask = LVIF_TEXT;
			lvi.pszText = (LPSTR)(LPCTSTR)strTemp;
			m_listLang.SetItem(&lvi);

			strTemp = TRL("~LANGUAGEAUTHOREMAIL");
			if(strTemp == _T("~LANGUAGEAUTHOREMAIL")) strTemp.Empty();
			lvi.iSubItem = 3; lvi.mask = LVIF_TEXT;
			lvi.pszText = (LPSTR)(LPCTSTR)strTemp;
			m_listLang.SetItem(&lvi);
		}
	}

	ff.Close();

	VERIFY(LoadTranslationTable(szCurrentlyLoaded));

	return TRUE;
}

void CLanguagesDlg::OnOK() 
{
	CDialog::OnOK();
}

void CLanguagesDlg::OnCancel() 
{
	CDialog::OnCancel();
}

void CLanguagesDlg::OnClickLanguagesList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint mousePoint;
	UINT nFlags = 0;
	int nHitItem = 0;
	TCHAR tszItem[MAX_PATH];

	UNREFERENCED_PARAMETER(pNMHDR);

	GetCursorPos(&mousePoint);

	m_listLang.ScreenToClient(&mousePoint);

	nHitItem = m_listLang.HitTest(mousePoint, &nFlags);

	m_listLang.GetItemText(nHitItem, 0, tszItem, 254);

	if(nFlags & LVHT_ONITEM)
	{
		_LoadLanguage(tszItem);
	}

	*pResult = 0;
}

void CLanguagesDlg::_LoadLanguage(char *szLang)
{
	FILE *fp = NULL;
	TCHAR szFile[MAX_PATH * 2];
	int i = 0;
	CPrivateConfig cConfig(TRUE);

	GetModuleFileName(NULL, szFile, MAX_PATH * 2);
	for(i = _tcslen(szFile)-1; i > 1; i--) // Extract dir
	{
		if(szFile[i] == _T('\\')) { szFile[i] = 0; break; }
	}
	_tcscat(szFile, _T("\\"));
	_tcscat(szFile, szLang);

	if(_tcscmp(szLang, _T("English")) != 0)
	{
		_tcscat(szFile, _T(".lng"));

		fp = _tfopen(szFile, _T("rb"));
		ASSERT(fp != NULL);
		if(fp == NULL)
		{
			MessageBox(TRL("Language file cannot be opened!"), TRL("Loading error"), MB_OK | MB_ICONWARNING);
			return;
		}
		fclose(fp);

		if(cConfig.Set(PWMKEY_LANG, szLang) == FALSE)
		{
			MessageBox(TRL("Language file cannot be registered!"), TRL("Loading error"), MB_OK | MB_ICONWARNING);
			return;
		}
	}
	else
	{
		cConfig.Set(PWMKEY_LANG, _T("Standard"));
	}

	CString str;
	str = TRL("The language file has been installed.");
	str += _T("\r\n\r\n");
	str += TRL("You must restart KeePass in order to use the new language.");
	str += _T("\r\n\r\n");
	str += TRL("Do you wish to restart KeePass now?");
	i = MessageBox(str,
		TRL("Restart KeePass?"), MB_YESNO | MB_ICONQUESTION);
	if(i == IDYES)
	{
		CDialog::OnOK();
	}
}

void CLanguagesDlg::OnBtnGetLanguage() 
{
	ShellExecute(GetSafeHwnd(), _T("open"), PWM_URL_TRL,
		NULL, NULL, SW_SHOW);
	OnCancel();
}
