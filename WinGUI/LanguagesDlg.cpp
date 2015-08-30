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

#include "StdAfx.h"
#include "PwSafe.h"
#include "LanguagesDlg.h"

#include "../KeePassLibCpp/PwManager.h"
#include "../KeePassLibCpp/Util/TranslateEx.h"
#include "Util/PrivateConfigEx.h"
#include "NewGUI/NewGUICommon.h"
#include "NewGUI/TaskDialog/VistaTaskDialog.h"
#include "Util/CmdLine/Executable.h"

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
	
	NewGUI_XPButton(m_btClose, IDB_CANCEL, IDB_CANCEL);
	NewGUI_XPButton(m_btGetLang, IDB_LANGUAGE, IDB_LANGUAGE);

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_WORLD),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(TRL("Load a Language File"));
	m_banner.SetCaption(TRL("Select one of the languages in the list below."));

	RECT rcList;
	m_listLang.GetWindowRect(&rcList);
	const int nColSize = (rcList.right - rcList.left - GetSystemMetrics(SM_CXVSCROLL) - 8) / 4;
	m_listLang.InsertColumn(0, TRL("Available Languages"), LVCFMT_LEFT, nColSize, 0);
	m_listLang.InsertColumn(1, TRL("Language File Version"), LVCFMT_LEFT, nColSize, 1);
	m_listLang.InsertColumn(2, TRL("Author"), LVCFMT_LEFT, nColSize, 2);
	m_listLang.InsertColumn(3, TRL("Translation Author Contact"), LVCFMT_LEFT, nColSize, 3);

	// m_ilIcons.Create(CPwSafeApp::GetClientIconsResourceID(), 16, 1, RGB(255,0,255));
	CPwSafeApp::CreateHiColorImageList(&m_ilIcons, IDB_CLIENTICONS_EX, 16);
	m_listLang.SetImageList(&m_ilIcons, LVSIL_SMALL);

	m_listLang.PostMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_SI_REPORT |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE |
		LVS_EX_UNDERLINEHOT | LVS_EX_INFOTIP);

	m_listLang.DeleteAllItems();

	LV_ITEM lvi;
	ZeroMemory(&lvi, sizeof(LV_ITEM));
	lvi.iItem = m_listLang.InsertItem(LVIF_TEXT | LVIF_IMAGE, m_listLang.GetItemCount(),
		_T("English"), 0, 0, 1, NULL);

	CString strTemp;
	
	strTemp = PWM_VERSION_STR;
	lvi.iSubItem = 1; lvi.mask = LVIF_TEXT;
	lvi.pszText = (LPTSTR)(LPCTSTR)strTemp;
	m_listLang.SetItem(&lvi);

	strTemp = PWMX_ENGLISH_AUTHOR;
	lvi.iSubItem = 2; lvi.mask = LVIF_TEXT;
	lvi.pszText = (LPTSTR)(LPCTSTR)strTemp;
	m_listLang.SetItem(&lvi);

	strTemp = PWMX_ENGLISH_CONTACT;
	lvi.iSubItem = 3; lvi.mask = LVIF_TEXT;
	lvi.pszText = (LPTSTR)(LPCTSTR)strTemp;
	m_listLang.SetItem(&lvi);

	TCHAR szCurrentlyLoaded[MAX_PATH * 2];
	_tcscpy_s(szCurrentlyLoaded, _countof(szCurrentlyLoaded), GetCurrentTranslationTable());

	std_string strFilter = Executable::instance().getPathOnly();
	strFilter += _T("*.lng");

	CFileFind ff;
	BOOL chk_w = ff.FindFile(strFilter.c_str(), 0);
	while(chk_w != FALSE)
	{
		chk_w = ff.FindNextFile();

		// Ignore KeePass 2.x LNGX files (these are found even though
		// "*.lng" is specified as file mask)
		CString strFileName = ff.GetFileName();
		strFileName = strFileName.MakeLower();
		if((strFileName.GetLength() >= 5) && (strFileName.Right(5) ==
			_T(".lngx")))
			continue;

		CString strID = ff.GetFileTitle();
		strID = strID.MakeLower();
		if((strID != _T("standard")) && (strID != _T("english")))
		{
			VERIFY(LoadTranslationTable((LPCTSTR)ff.GetFileTitle()));

			strTemp = (LPCTSTR)ff.GetFileTitle();
			// strTemp += _T(" - ");
			// strTemp += TRL("~LANGUAGENAME");

			lvi.iItem = m_listLang.InsertItem(LVIF_TEXT | LVIF_IMAGE,
				m_listLang.GetItemCount(), strTemp, 0, 0, 1, NULL);

			strTemp = TRL("~LANGUAGEVERSION");
			if(strTemp == _T("~LANGUAGEVERSION")) strTemp.Empty();
			lvi.iSubItem = 1; lvi.mask = LVIF_TEXT;
			lvi.pszText = (LPTSTR)(LPCTSTR)strTemp;
			m_listLang.SetItem(&lvi);

			strTemp = TRL("~LANGUAGEAUTHOR");
			if(strTemp == _T("~LANGUAGEAUTHOR")) strTemp.Empty();
			lvi.iSubItem = 2; lvi.mask = LVIF_TEXT;
			lvi.pszText = (LPTSTR)(LPCTSTR)strTemp;
			m_listLang.SetItem(&lvi);

			strTemp = TRL("~LANGUAGEAUTHOREMAIL");
			if(strTemp == _T("~LANGUAGEAUTHOREMAIL")) strTemp.Empty();
			lvi.iSubItem = 3; lvi.mask = LVIF_TEXT;
			lvi.pszText = (LPTSTR)(LPCTSTR)strTemp;
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
	UNREFERENCED_PARAMETER(pNMHDR);

	CPoint mousePoint;
	GetCursorPos(&mousePoint);
	m_listLang.ScreenToClient(&mousePoint);

	UINT nFlags = 0;
	const int nHitItem = m_listLang.HitTest(mousePoint, &nFlags);

	TCHAR tszItem[MAX_PATH];
	ZeroMemory(tszItem, MAX_PATH * sizeof(TCHAR));
	m_listLang.GetItemText(nHitItem, 0, tszItem, 254);

	if((nFlags & LVHT_ONITEM) != 0) _LoadLanguage(tszItem);

	*pResult = 0;
}

void CLanguagesDlg::_LoadLanguage(LPCTSTR szLang)
{
	FILE *fp = NULL;
	CPrivateConfigEx cConfig(TRUE);

	// GetModuleFileName(NULL, szFile, MAX_PATH * 2);
	// for(i = _tcslen(szFile)-1; i > 1; i--) // Extract dir
	// {
	//	if(szFile[i] == _T('\\')) { szFile[i] = 0; break; }
	// }
	// _tcscat_s(szFile, _countof(szFile), _T("\\"));
	// _tcscat_s(szFile, _countof(szFile), szLang);
	std_string strFile =  Executable::instance().getPathOnly();
	strFile += szLang;

	if(_tcscmp(szLang, _T("English")) != 0)
	{
		// _tcscat_s(szFile, _countof(szFile), _T(".lng"));
		strFile += _T(".lng");

		_tfopen_s(&fp, strFile.c_str(), _T("rb"));
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
	else cConfig.Set(PWMKEY_LANG, _T("Standard"));

	CString str = TRL("The language file has been installed.");
	str += _T("\r\n\r\n");
	str += TRL("You must restart KeePass in order to use the new language.");
	str += _T("\r\n\r\n");
	str += TRL("Do you wish to restart KeePass now?");

	int iResult = CVistaTaskDialog::ShowMessageBox(this->m_hWnd, TRL("Restart KeePass?"),
		str, MTDI_QUESTION, TRL("&Yes"), IDOK, TRL("&No"), IDCANCEL);
	if(iResult < 0)
		iResult = MessageBox(str, TRL("Restart KeePass?"), MB_YESNO | MB_ICONQUESTION);
	if((iResult == IDOK) || (iResult == IDYES)) CDialog::OnOK();
}

void CLanguagesDlg::OnBtnGetLanguage() 
{
	ShellExecute(GetSafeHwnd(), NULL, PWM_URL_TRL, NULL, NULL, SW_SHOW);
	OnCancel();
}
