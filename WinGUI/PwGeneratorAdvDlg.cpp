/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2011 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "PwGeneratorAdvDlg.h"

#include "Util/WinUtil.h"
#include "NewGUI/NewGUICommon.h"
#include "../KeePassLibCpp/Util/MemUtil.h"
#include "../KeePassLibCpp/Util/StrUtil.h"
#include "../KeePassLibCpp/Util/TranslateEx.h"

IMPLEMENT_DYNAMIC(CPwGeneratorAdvDlg, CDialog)

CPwGeneratorAdvDlg::CPwGeneratorAdvDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPwGeneratorAdvDlg::IDD, pParent)
	, m_bNoRepeat(FALSE)
	, m_bNoConfusing(FALSE)
	, m_strExcludeChars(_T(""))
{
	m_pOpt = NULL;
}

CPwGeneratorAdvDlg::~CPwGeneratorAdvDlg()
{
}

void CPwGeneratorAdvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Check(pDX, IDC_CHECK_NO_REPEAT, m_bNoRepeat);
	DDX_Check(pDX, IDC_CHECK_EXCLUDE_LOOKALIKE, m_bNoConfusing);
	DDX_Control(pDX, IDC_BTN_HELP, m_btHelp);
	DDX_Text(pDX, IDC_EDIT_EXCLUDECHARS, m_strExcludeChars);
}

BEGIN_MESSAGE_MAP(CPwGeneratorAdvDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CPwGeneratorAdvDlg::OnBtnOK)
	ON_BN_CLICKED(IDCANCEL, &CPwGeneratorAdvDlg::OnBtnCancel)
	ON_BN_CLICKED(IDC_BTN_HELP, &CPwGeneratorAdvDlg::OnBtnHelp)
END_MESSAGE_MAP()

BOOL CPwGeneratorAdvDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT(m_pOpt != NULL); if(m_pOpt == NULL) return TRUE;

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	NewGUI_XPButton(m_btOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(m_btCancel, IDB_CANCEL, IDB_CANCEL);
	NewGUI_XPButton(m_btHelp, IDB_HELP_SMALL, IDB_HELP_SMALL);

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_KEY),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(TRL("Options"));
	m_banner.SetCaption(TRL("Advanced Password Generator Options"));

	CString str;
	GetDlgItem(IDC_CHECK_EXCLUDE_LOOKALIKE)->GetWindowText(str);
	GetDlgItem(IDC_CHECK_EXCLUDE_LOOKALIKE)->SetWindowText(str + _T(" (l|1I, O0) *"));

	GetDlgItem(IDC_CHECK_NO_REPEAT)->GetWindowText(str);
	GetDlgItem(IDC_CHECK_NO_REPEAT)->SetWindowText(str + _T(" *"));

	GetDlgItem(IDC_STATIC_EXCLUDECHARS)->GetWindowText(str);
	GetDlgItem(IDC_STATIC_EXCLUDECHARS)->SetWindowText(str + _T(" *"));

	GetDlgItem(IDC_STATIC_SECREDUCINFO)->GetWindowText(str);
	GetDlgItem(IDC_STATIC_SECREDUCINFO)->SetWindowText(_T("* ") + str);

	m_bNoConfusing = m_pOpt->bNoConfusing;
	m_bNoRepeat = m_pOpt->bNoRepeat;

#ifdef _UNICODE
	m_strExcludeChars = m_pOpt->strExcludeChars.c_str();
#else
	char *pExcl = _StringToAnsi(m_pOpt->strExcludeChars.c_str());
	m_strExcludeChars = pExcl;
	SAFE_DELETE_ARRAY(pExcl);
#endif

	UpdateData(FALSE);
	return TRUE;
}

void CPwGeneratorAdvDlg::InitEx(PW_GEN_SETTINGS_EX* pWorkingStruct)
{
	m_pOpt = pWorkingStruct;
}

void CPwGeneratorAdvDlg::CopyAdvancedOptions(const PW_GEN_SETTINGS_EX* pSource,
	PW_GEN_SETTINGS_EX* pDest)
{
	ASSERT(pSource != NULL); if(pSource == NULL) return;
	ASSERT(pDest != NULL); if(pDest == NULL) return;

	pDest->bNoConfusing = pSource->bNoConfusing;
	pDest->bNoRepeat = pSource->bNoRepeat;
	pDest->strExcludeChars = pSource->strExcludeChars;
}

void CPwGeneratorAdvDlg::OnBtnOK()
{
	UpdateData(TRUE);

	m_pOpt->bNoConfusing = ((m_bNoConfusing == FALSE) ? FALSE : TRUE);
	m_pOpt->bNoRepeat = ((m_bNoRepeat == FALSE) ? FALSE : TRUE);

#ifdef _UNICODE
	m_pOpt->strExcludeChars = m_strExcludeChars;
#else
	WCHAR *pExcl = _StringToUnicode(m_strExcludeChars);
	m_pOpt->strExcludeChars = pExcl;
	SAFE_DELETE_ARRAY(pExcl);
#endif

	OnOK();
}

void CPwGeneratorAdvDlg::OnBtnCancel()
{
	OnCancel();
}

void CPwGeneratorAdvDlg::OnBtnHelp()
{
	WU_OpenAppHelp(PWM_HELP_PWGEN_ADV, m_hWnd);
}
