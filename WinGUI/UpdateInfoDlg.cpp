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

#include "StdAfx.h"
#include "PwSafe.h"
#include "UpdateInfoDlg.h"

#include "NewGUI/NewGUICommon.h"
#include "../KeePassLibCpp/PwManager.h"
#include "../KeePassLibCpp/Util/TranslateEx.h"

IMPLEMENT_DYNAMIC(CUpdateInfoDlg, CDialog)

CUpdateInfoDlg::CUpdateInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUpdateInfoDlg::IDD, pParent)
	, m_strStatus(_T(""))
{
}

CUpdateInfoDlg::~CUpdateInfoDlg()
{
}

void CUpdateInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUpdateInfoDlg)
	DDX_Control(pDX, IDC_HLINK_HOMEPAGE, m_hlVisitWebsite);
	DDX_Control(pDX, IDC_LIST_COMPS, m_cList);
	DDX_Text(pDX, IDC_STATIC_SUMMARY, m_strStatus);
	DDX_Control(pDX, IDOK, m_btClose);
	DDX_Control(pDX, IDC_STATIC_SUMMARY, m_lblStatus);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CUpdateInfoDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CUpdateInfoDlg::OnBnClickedOK)

	ON_REGISTERED_MESSAGE(WM_XHYPERLINK_CLICKED, OnXHyperLinkClicked)
END_MESSAGE_MAP()

BOOL CUpdateInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	CFont* pDialogFont = m_lblStatus.GetFont();
	LOGFONT lf;
	pDialogFont->GetLogFont(&lf);
	m_fBold.CreateFont(lf.lfHeight, lf.lfWidth, lf.lfEscapement, lf.lfOrientation,
		FW_BOLD, lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut,
		lf.lfCharSet, lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, lf.lfFaceName);
	m_lblStatus.SetFont(&m_fBold);

	NewGUI_XPButton(m_btClose, IDB_CANCEL, IDB_CANCEL);

	NewGUI_MakeHyperLink(&m_hlVisitWebsite);
	m_hlVisitWebsite.EnableTooltip(FALSE);
	m_hlVisitWebsite.SetNotifyParent(TRUE);
	m_hlVisitWebsite.EnableURL(FALSE);

	ASSERT(m_pImgList != NULL);
	m_cList.SetImageList(m_pImgList, LVSIL_SMALL);

	// m_cList.PostMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_SI_REPORT |
	//	LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE |
	//	LVS_EX_UNDERLINEHOT | LVS_EX_INFOTIP);
	m_cList.PostMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_SI_REPORT |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP);

	RECT rect;
	m_cList.GetWindowRect(&rect);
	const int nWidth = (rect.right - rect.left - GetSystemMetrics(SM_CXVSCROLL) - 8) / 6;
	m_cList.InsertColumn(0, TRL("Component"), LVCFMT_LEFT, nWidth * 2, 0);
	m_cList.InsertColumn(1, TRL("Status"), LVCFMT_LEFT, nWidth * 2, 1);
	m_cList.InsertColumn(2, TRL("Installed"), LVCFMT_LEFT, nWidth, 2);
	m_cList.InsertColumn(3, TRL("Available"), LVCFMT_LEFT, nWidth, 3);

	FillComponentList();

	UpdateData(FALSE);
	return TRUE;
}

void CUpdateInfoDlg::InitEx(CImageList *pImgList, const UC_COMPONENTS_LIST* pcInfo,
	LPCTSTR lpStatus)
{
	m_pImgList = pImgList;
	m_pcInfo = pcInfo;
	m_strStatus = ((lpStatus != NULL) ? lpStatus : _T(""));
}

void CUpdateInfoDlg::OnOK()
{
	this->CleanUp();
	CDialog::OnOK();
}

void CUpdateInfoDlg::OnCancel()
{
	this->CleanUp();
	CDialog::OnCancel();
}

void CUpdateInfoDlg::CleanUp()
{
	m_cList.SetImageList(NULL, LVSIL_SMALL);
	m_fBold.DeleteObject();
}

void CUpdateInfoDlg::OnBnClickedOK()
{
	OnOK();
}

LRESULT CUpdateInfoDlg::OnXHyperLinkClicked(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	if(wParam == IDC_HLINK_HOMEPAGE)
		ShellExecute(NULL, NULL, PWM_HOMEPAGE, NULL, NULL, SW_SHOW);

	OnCancel();
	return 0;
}

void CUpdateInfoDlg::FillComponentList()
{
	m_cList.DeleteAllItems();

	if(m_pcInfo == NULL) { ASSERT(FALSE); return; }

	for(size_t i = 0; i < m_pcInfo->size(); ++i)
	{
		const UC_COMPONENT_INFO& c = (*m_pcInfo)[i];

		LV_ITEM lvi;
		ZeroMemory(&lvi, sizeof(LV_ITEM));

		lvi.iItem = static_cast<int>(i);
		lvi.mask = (LVIF_TEXT | LVIF_IMAGE);

		lvi.iImage = c.iStatusImage;

		lvi.pszText = const_cast<LPTSTR>(c.strName.c_str());
		m_cList.InsertItem(&lvi);

		lvi.mask = LVIF_TEXT;

		lvi.iSubItem = 1;
		lvi.pszText = const_cast<LPTSTR>(c.strStatus.c_str());
		m_cList.SetItem(&lvi);

		lvi.iSubItem = 2;
		CString strTmp = CUpdateInfoDlg::FormatVersion(c.qwVerInstalled, c.strName.c_str());
		lvi.pszText = const_cast<LPTSTR>((LPCTSTR)strTmp);
		m_cList.SetItem(&lvi);

		lvi.iSubItem = 3;
		strTmp = CUpdateInfoDlg::FormatVersion(c.qwVerAvailable, c.strName.c_str());
		lvi.pszText = const_cast<LPTSTR>((LPCTSTR)strTmp);
		m_cList.SetItem(&lvi);
	}
}

CString CUpdateInfoDlg::FormatVersion(UINT64 qwVersion, LPCTSTR lpName)
{
	CString str;
	if(qwVersion == 0) return str;

	const DWORD dwHi = static_cast<DWORD>(qwVersion >> 32);
	const DWORD dwLo = static_cast<DWORD>(qwVersion & DWORD_MAX);
	if((lpName != NULL) && (_tcscmp(lpName, PWM_PRODUCT_NAME_SHORT) == 0))
	{
		str.Format(_T("%u.%u%u"), dwHi >> 16, dwHi & 0xFFFF, dwLo >> 16);
		if((dwLo & 0xFFFF) != 0)
			str += static_cast<TCHAR>(_T('a') + static_cast<TCHAR>((dwLo - 1) & 0xFFFF));
		return str;
	}

	str.Format(_T("%u.%u.%u.%u"), dwHi >> 16, dwHi & 0xFFFF, dwLo >> 16, dwLo & 0xFFFF);
	return str;
}
