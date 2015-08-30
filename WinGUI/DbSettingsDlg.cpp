/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2012 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "DbSettingsDlg.h"
#include "../KeePassLibCpp/Crypto/KeyTransform.h"
#include "../KeePassLibCpp/PwManager.h"
#include "../KeePassLibCpp/Util/TranslateEx.h"
#include "Util/WinUtil.h"
#include "NewGUI/NewGUICommon.h"
#include "NewGUI/NewColorizerEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CDbSettingsDlg::CDbSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDbSettingsDlg::IDD, pParent),
	m_strDefaultUserName(_T(""))
	, m_bCustomColor(FALSE)
{
	//{{AFX_DATA_INIT(CDbSettingsDlg)
	m_nAlgorithm = -1;
	m_dwNumKeyEnc = 0;
	//}}AFX_DATA_INIT

	m_clr = DWORD_MAX;
}

void CDbSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDbSettingsDlg)
	DDX_Control(pDX, IDC_BTN_CALCROUNDS, m_btCalcRounds);
	DDX_Control(pDX, IDC_COMBO_ENCALGOS, m_cEncAlgos);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_CBIndex(pDX, IDC_COMBO_ENCALGOS, m_nAlgorithm);
	DDX_Text(pDX, IDC_EDIT_KEYENC, m_dwNumKeyEnc);
	DDX_Text(pDX, IDC_EDIT_DEFAULTUSER, m_strDefaultUserName);
	DDX_Control(pDX, IDC_BTN_SECHELP, m_btHelp);
	DDX_Control(pDX, IDC_STATIC_COLORVIEW, m_stcColor);
	DDX_Control(pDX, IDC_SLIDER_COLOR, m_sldColor);
	DDX_Check(pDX, IDC_CHECK_CUSTOMCOLOR, m_bCustomColor);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDbSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(CDbSettingsDlg)
	ON_BN_CLICKED(IDC_BTN_CALCROUNDS, OnBtnCalcRounds)
	ON_BN_CLICKED(IDC_BTN_SECHELP, &CDbSettingsDlg::OnBtnClickedHelp)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_COLOR, &CDbSettingsDlg::OnSliderColorChanged)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_CUSTOMCOLOR, &CDbSettingsDlg::OnCheckCustomColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CDbSettingsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	NewGUI_XPButton(m_btOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(m_btCancel, IDB_CANCEL, IDB_CANCEL);
	NewGUI_XPButton(m_btCalcRounds, IDB_TB_DEFAULTEXPIRE, IDB_TB_DEFAULTEXPIRE, TRUE);
	NewGUI_XPButton(m_btHelp, IDB_HELP_SMALL, IDB_HELP_SMALL);

	CString str;
	m_btCalcRounds.GetWindowText(str);
	m_btCalcRounds.SetTooltipText(str + _T("."));
	m_btCalcRounds.SetWindowText(_T(""));

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_OPTIONS),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(TRL("Database Settings"));
	m_banner.SetCaption(TRL("Here you can configure the current database."));

	m_cEncAlgos.ResetContent();
	m_cEncAlgos.AddString(TRL("Advanced Encryption Standard (AES) (128-bit block cipher using 256-bit key)"));
	m_cEncAlgos.AddString(TRL("Twofish (128-bit block cipher using 256-bit key)"));
	m_cEncAlgos.SetCurSel(m_nAlgorithm);

	if(m_clr == DWORD_MAX)
	{
		m_bCustomColor = FALSE;
		m_stcColor.m_clr = RGB(0, 0, 255);
	}
	else
	{
		m_bCustomColor = TRUE;
		m_stcColor.m_clr = m_clr;
	}

	float fHue = NewGUI_GetHue(m_stcColor.m_clr);
	m_sldColor.SetRange(0, 360);
	m_sldColor.SetPos((int)fHue);

	UpdateData(FALSE);
	EnableControlsEx();

	return TRUE; // Return TRUE unless you set the focus to a control
}

void CDbSettingsDlg::EnableControlsEx()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_SLIDER_COLOR)->EnableWindow(m_bCustomColor);
}

void CDbSettingsDlg::OnOK() 
{
	UpdateData(TRUE);

	// Check if the user has entered something valid, otherwise fix it
	CString str;
	GetDlgItem(IDC_EDIT_KEYENC)->GetWindowText(str);
	__int64 l = _ttoi64(str);
	if(l < 0) l = -l;
	if(l > 0xFFFFFFFELL) m_dwNumKeyEnc = 0xFFFFFFFE;
	else if(l != static_cast<__int64>(m_dwNumKeyEnc))
	{
		ASSERT(FALSE);
		m_dwNumKeyEnc = static_cast<DWORD>(l);
	}

	if(m_bCustomColor == FALSE) m_clr = DWORD_MAX;
	else m_clr = m_stcColor.m_clr;

	UpdateData(FALSE);

	CDialog::OnOK();
}

void CDbSettingsDlg::OnCancel() 
{
	UpdateData(TRUE);
	CDialog::OnCancel();
}

void CDbSettingsDlg::OnBtnCalcRounds() 
{
	UpdateData(TRUE);

	const UINT64 u = CKeyTransform::Benchmark(1000);
	m_dwNumKeyEnc = ((u <= static_cast<UINT64>(DWORD_MAX - 8)) ?
		static_cast<DWORD>(u) : (DWORD_MAX - 8));

	UpdateData(FALSE);
}

void CDbSettingsDlg::OnBtnClickedHelp()
{
	WU_OpenAppHelp(PWM_HELP_SECURITY, m_hWnd);
}

void CDbSettingsDlg::OnSliderColorChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	UNREFERENCED_PARAMETER(pNMHDR);
	if(pResult != NULL) *pResult = 0;

	float fHue = (float)m_sldColor.GetPos();
	m_stcColor.m_clr = NewGUI_ColorFromHsv(fHue, 1.0f, 1.0f);
	m_stcColor.RedrawWindow();
}

void CDbSettingsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(pScrollBar->GetDlgCtrlID() == IDC_SLIDER_COLOR)
		OnSliderColorChanged(NULL, NULL);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDbSettingsDlg::OnCheckCustomColor()
{
	EnableControlsEx();
}
