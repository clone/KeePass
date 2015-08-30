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
#include "PwGeneratorExDlg.h"
#include "PwSafeDlg.h"
#include "SingleLineEditDlg.h"

#include "NewGUI/NewGUICommon.h"
#include "Util/WinUtil.h"
#include "Util/PrivateConfigEx.h"

#include "../KeePassLibCpp/PwManager.h"
#include "../KeePassLibCpp/Util/MemUtil.h"
#include "../KeePassLibCpp/Util/StrUtil.h"
#include "../KeePassLibCpp/Util/TranslateEx.h"

#include "GetRandomDlg.h"
#include "PwGeneratorAdvDlg.h"

IMPLEMENT_DYNAMIC(CPwGeneratorExDlg, CDialog)

CPwGeneratorExDlg::CPwGeneratorExDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPwGeneratorExDlg::IDD, pParent)
	, m_dwPasswordLength(0)
	, m_bCollectEntropy(FALSE)
	, m_bCsUpperCase(FALSE)
	, m_bCsLowerCase(FALSE)
	, m_bCsNumeric(FALSE)
	, m_bCsMinus(FALSE)
	, m_bCsUnderline(FALSE)
	, m_bCsSpace(FALSE)
	, m_bCsSpecial(FALSE)
	, m_bCsBrackets(FALSE)
	, m_bCsHighAnsi(FALSE)
	, m_strCustomCharSet(_T(""))
	, m_strPattern(_T(""))
	, m_bHidePw(FALSE)
	, m_bPatternPermute(FALSE)
{
	m_dwRequestedPasswords = 0;
	m_lpPassword = NULL;
	m_bBlockUIUpdate = FALSE;
	m_bShowInTaskbar = FALSE;
}

CPwGeneratorExDlg::~CPwGeneratorExDlg()
{
}

void CPwGeneratorExDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PROFILES, m_cmbProfile);
	DDX_Control(pDX, IDC_BTN_PROFILE_CREATE, m_btnProfileCreate);
	DDX_Control(pDX, IDC_BTN_PROFILE_DELETE, m_btnProfileDelete);
	DDX_Text(pDX, IDC_EDIT_LENGTH, m_dwPasswordLength);
	DDX_Check(pDX, IDC_CHECK_COLLECT_ENTROPY, m_bCollectEntropy);
	DDX_Check(pDX, IDC_CHECK_CS_UPPERCASE, m_bCsUpperCase);
	DDX_Check(pDX, IDC_CHECK_CS_LOWERCASE, m_bCsLowerCase);
	DDX_Check(pDX, IDC_CHECK_CS_NUMERIC, m_bCsNumeric);
	DDX_Check(pDX, IDC_CHECK_CS_MINUS, m_bCsMinus);
	DDX_Check(pDX, IDC_CHECK_CS_UNDERLINE, m_bCsUnderline);
	DDX_Check(pDX, IDC_CHECK_CS_SPACE, m_bCsSpace);
	DDX_Check(pDX, IDC_CHECK_CS_SPECIAL, m_bCsSpecial);
	DDX_Check(pDX, IDC_CHECK_CS_BRACKETS, m_bCsBrackets);
	DDX_Check(pDX, IDC_CHECK_CS_HIGHANSI, m_bCsHighAnsi);
	DDX_Control(pDX, IDC_CHECK_CS_UPPERCASE, m_cbUpperCase);
	DDX_Control(pDX, IDC_CHECK_CS_LOWERCASE, m_cbLowerCase);
	DDX_Control(pDX, IDC_CHECK_CS_NUMERIC, m_cbNumeric);
	DDX_Control(pDX, IDC_CHECK_CS_MINUS, m_cbMinus);
	DDX_Control(pDX, IDC_CHECK_CS_UNDERLINE, m_cbUnderline);
	DDX_Control(pDX, IDC_CHECK_CS_SPACE, m_cbSpace);
	DDX_Control(pDX, IDC_CHECK_CS_SPECIAL, m_cbSpecial);
	DDX_Control(pDX, IDC_CHECK_CS_BRACKETS, m_cbBrackets);
	DDX_Control(pDX, IDC_CHECK_CS_HIGHANSI, m_cbHighAnsi);
	DDX_Text(pDX, IDC_EDIT_CUSTOMCHARSET, m_strCustomCharSet);
	DDX_Text(pDX, IDC_EDIT_PATTERN, m_strPattern);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_STATIC_CUSTOMCHARSET, m_stcCustomCharSet);
	DDX_Control(pDX, IDC_EDIT_CUSTOMCHARSET, m_tbCustomCharSet);
	DDX_Control(pDX, IDC_EDIT_PATTERN, m_tbPattern);
	DDX_Control(pDX, IDC_BTN_HELP, m_btnHelp);
	DDX_Control(pDX, IDC_RADIO_CHARSET_BASED, m_rbCharSetBased);
	DDX_Control(pDX, IDC_RADIO_PATTERN_BASED, m_rbPatternBased);
	DDX_Control(pDX, IDC_CHECK_COLLECT_ENTROPY, m_cbCollectEntropy);
	DDX_Control(pDX, IDC_EDIT_PW, m_cEditPw);
	DDX_Control(pDX, IDC_CHECK_HIDEPW, m_btnHidePw);
	DDX_Control(pDX, IDC_SPIN_LENGTH, m_spinNumChars);
	DDX_Control(pDX, IDC_GENERATE_BTN, m_btnGenerate);
	DDX_Control(pDX, IDC_PROGRESS_PASSQUALITY, m_cPassQuality);
	DDX_Check(pDX, IDC_CHECK_HIDEPW, m_bHidePw);
	DDX_Control(pDX, IDC_STATIC_PWLENGTH, m_stcLength);
	DDX_Control(pDX, IDC_EDIT_LENGTH, m_tbLength);
	DDX_Control(pDX, IDC_CHECK_PATTERN_PERMUTE, m_cbPatternPermute);
	DDX_Check(pDX, IDC_CHECK_PATTERN_PERMUTE, m_bPatternPermute);
	DDX_Control(pDX, IDC_BTN_ADVANCED, m_btnAdvanced);
}

BEGIN_MESSAGE_MAP(CPwGeneratorExDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CPwGeneratorExDlg::OnCancel)
	ON_BN_CLICKED(IDOK, &CPwGeneratorExDlg::OnOK)
	ON_BN_CLICKED(IDC_RADIO_CHARSET_BASED, &CPwGeneratorExDlg::OnRadioCharsetBased)
	ON_BN_CLICKED(IDC_RADIO_PATTERN_BASED, &CPwGeneratorExDlg::OnRadioPatternBased)
	ON_BN_CLICKED(IDC_BTN_HELP, &CPwGeneratorExDlg::OnBnClickedBtnHelp)
	ON_CBN_SELCHANGE(IDC_COMBO_PROFILES, &CPwGeneratorExDlg::OnCbnSelChangeComboProfiles)
	ON_BN_CLICKED(IDC_BTN_PROFILE_CREATE, &CPwGeneratorExDlg::OnBnClickedBtnProfileCreate)
	ON_BN_CLICKED(IDC_BTN_PROFILE_DELETE, &CPwGeneratorExDlg::OnBnClickedBtnProfileDelete)
	ON_BN_CLICKED(IDC_GENERATE_BTN, &CPwGeneratorExDlg::OnBnClickedGenerateBtn)
	ON_BN_CLICKED(IDC_CHECK_HIDEPW, &CPwGeneratorExDlg::OnCheckHidePw)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LENGTH, &CPwGeneratorExDlg::OnDeltaPosSpinLength)
	ON_EN_CHANGE(IDC_EDIT_PW, &CPwGeneratorExDlg::OnEnChangeEditPw)
	ON_EN_CHANGE(IDC_EDIT_LENGTH, &CPwGeneratorExDlg::OnEnChangeEditLength)
	ON_EN_CHANGE(IDC_EDIT_CUSTOMCHARSET, &CPwGeneratorExDlg::OnEnChangeEditCustomCharSet)
	ON_EN_CHANGE(IDC_EDIT_PATTERN, &CPwGeneratorExDlg::OnEnChangeEditPattern)
	ON_BN_CLICKED(IDC_CHECK_CS_UPPERCASE, &CPwGeneratorExDlg::OnBnClickedCheckCsUppercase)
	ON_BN_CLICKED(IDC_CHECK_CS_LOWERCASE, &CPwGeneratorExDlg::OnBnClickedCheckCsLowercase)
	ON_BN_CLICKED(IDC_CHECK_CS_NUMERIC, &CPwGeneratorExDlg::OnBnClickedCheckCsNumeric)
	ON_BN_CLICKED(IDC_CHECK_CS_MINUS, &CPwGeneratorExDlg::OnBnClickedCheckCsMinus)
	ON_BN_CLICKED(IDC_CHECK_CS_UNDERLINE, &CPwGeneratorExDlg::OnBnClickedCheckCsUnderline)
	ON_BN_CLICKED(IDC_CHECK_CS_SPACE, &CPwGeneratorExDlg::OnBnClickedCheckCsSpace)
	ON_BN_CLICKED(IDC_CHECK_CS_SPECIAL, &CPwGeneratorExDlg::OnBnClickedCheckCsSpecial)
	ON_BN_CLICKED(IDC_CHECK_CS_BRACKETS, &CPwGeneratorExDlg::OnBnClickedCheckCsBrackets)
	ON_BN_CLICKED(IDC_CHECK_CS_HIGHANSI, &CPwGeneratorExDlg::OnBnClickedCheckCsHighansi)
	ON_BN_CLICKED(IDC_CHECK_COLLECT_ENTROPY, &CPwGeneratorExDlg::OnBnClickedCheckCollectEntropy)
	ON_BN_CLICKED(IDC_BTN_ADVANCED, &CPwGeneratorExDlg::OnBnClickedAdvanced)
	ON_BN_CLICKED(IDC_CHECK_PATTERN_PERMUTE, &CPwGeneratorExDlg::OnBnClickedPatternPermute)
END_MESSAGE_MAP()

BOOL CPwGeneratorExDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	NewGUI_XPButton(m_btnOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(m_btnCancel, IDB_CANCEL, IDB_CANCEL);
	NewGUI_XPButton(m_btnProfileCreate, IDB_TB_SAVE_AS, IDB_TB_SAVE_AS, TRUE);
	NewGUI_XPButton(m_btnProfileDelete, IDB_CANCEL, IDB_CANCEL, TRUE);
	NewGUI_XPButton(m_btnHelp, IDB_HELP_SMALL, IDB_HELP_SMALL);
	NewGUI_XPButton(m_btnGenerate, IDB_KEY_SMALL, IDB_KEY_SMALL);
	NewGUI_XPButton(m_btnHidePw, -1, -1);
	NewGUI_XPButton(m_btnAdvanced, -1, -1);

	m_btnAdvanced.GetColor(CButtonST::BTNST_COLOR_FG_OUT, &m_clrControlText);

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	CString strTooltip;
	m_btnProfileCreate.GetWindowText(strTooltip);
	m_btnProfileCreate.SetTooltipText(strTooltip);
	m_btnProfileCreate.SetWindowText(_T(""));
	m_btnProfileDelete.GetWindowText(strTooltip);
	m_btnProfileDelete.SetTooltipText(strTooltip);
	m_btnProfileDelete.SetWindowText(_T(""));

	CString strSpecialChars;
	m_cbSpecial.GetWindowText(strSpecialChars);
	m_cbSpecial.SetWindowText(strSpecialChars + _T(" (!, $, %, &&, ...)"));

	RECT rectWindow, rectWork; // Save space by removing the banner, if needed
	this->GetWindowRect(&rectWindow);
	BOOL bSpi = SystemParametersInfo(SPI_GETWORKAREA, 0, &rectWork, 0);
	if(((rectWindow.bottom - rectWindow.top) <= (rectWork.bottom -
		rectWork.top)) || (bSpi == FALSE))
	{
		NewGUI_ConfigSideBanner(&m_banner, this);
		m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_KEY),
			KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
		CString strBannerTitle = TRL("&Generate Random Password");
		RemoveAcceleratorTip(&strBannerTitle);
		m_banner.SetTitle(strBannerTitle);
		m_banner.SetCaption(TRL("Here you can generate a random password."));
	}

	m_cEditPw.InitEx();

	CFont* pDialogFont = m_rbCharSetBased.GetFont();

	m_fStyle.CreateFont(NewGUI_Scale(-12, this), 0, 0, 0, 0, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, _T("Tahoma"));
	m_fSymbol.CreateFont(NewGUI_Scale(-13, this), 0, 0, 0, 0, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, CPwSafeApp::GetPasswordFont());

	LOGFONT lf;
	pDialogFont->GetLogFont(&lf);
	m_fBold.CreateFont(lf.lfHeight, lf.lfWidth, lf.lfEscapement, lf.lfOrientation,
		FW_BOLD, lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut,
		lf.lfCharSet, lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, lf.lfFaceName);

	m_rbCharSetBased.SetFont(&m_fBold);
	m_rbPatternBased.SetFont(&m_fBold);

	// 'z' + 27 is a black dot in Tahoma
	// TCHAR tchDot = (TCHAR)(_T('z') + 27);
	TCHAR tchDot = CPwSafeApp::GetPasswordCharacter();
	CString strStars; strStars += tchDot; strStars += tchDot; strStars += tchDot;
	m_btnHidePw.SetWindowText(strStars);

	m_cEditPw.SetFont(&m_fStyle, TRUE);
	m_btnHidePw.SetFont(&m_fSymbol, TRUE);

	m_btnHidePw.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 0, 255), TRUE);
	m_btnHidePw.SetTooltipText(TRL("Hide passwords behind asterisks (***)."), TRUE);

	NewGUI_ConfigQualityMeter(&m_cPassQuality);

	m_spinNumChars.SetBase(0);
	m_spinNumChars.SetRange(0, 1024);
	m_spinNumChars.SetPos(512);

	m_tipSecClear.Create(this, 0x40);
	m_tipSecClear.AddTool(&m_cEditPw, CPwSafeDlg::_GetSecureEditTipText(_T("Generated password")));
	m_tipSecClear.SetMaxTipWidth(630);
	m_tipSecClear.Activate(m_cEditPw.IsSecureModeEnabled());

	m_rbCharSetBased.SetCheck(BST_CHECKED);
	m_rbPatternBased.SetCheck(BST_UNCHECKED);

	// m_cmbProfile.LimitText(PWGD_MAX_PROFILE_NAME_LENGTH);
	m_tbCustomCharSet.LimitText(PWGD_MAX_PROFILE_CHARSET_LENGTH);
	m_tbPattern.LimitText(PWGD_MAX_PROFILE_PATTERN_LENGTH);

	CPwSafeDlg::m_pgsAutoProfile.strName = STR_AUTO_PROFILE;

	PwgGetDefaultProfile(&m_pgsLast);
	PwgGetDefaultProfile(&m_pgsAdvanced);

	LoadGenProfiles();
	RecreateProfilesList();

	PW_GEN_SETTINGS_EX pgsDefault;
	PwgGetDefaultProfile(&pgsDefault);
	UpdateDialogDataEx(FALSE, &pgsDefault);

	CString strLast = m_pgsLast.strName.c_str();
	if((m_cmbProfile.SelectString(-1, strLast) != CB_ERR) &&
		(strLast != STR_CUSTOM_PROFILE))
	{
		ShowGenProfile(strLast);
	}
	else
	{
		m_cmbProfile.SelectString(-1, STR_CUSTOM_PROFILE);
		UpdateDialogDataEx(FALSE, &m_pgsLast);
	}

	NewGUI_ShowQualityMeter(&m_cPassQuality, GetDlgItem(IDC_STATIC_PASSBITS), _T(""));
	OnCheckHidePw();

	if(m_bShowInTaskbar == TRUE)
		m_hPrevParent = WU_ShowWindowInTaskbar(this->m_hWnd, NULL, TRUE);

	// if(CPwSafeDlg::m_bMiniMode == TRUE)
	// {
	//	m_btnAdvanced.EnableWindow(FALSE);
	//	m_btnAdvanced.ShowWindow(SW_HIDE);
	// }

	this->EnableControlsEx(FALSE);
	return TRUE;
}

void CPwGeneratorExDlg::InitEx(DWORD dwRequestedPasswords, BOOL bInitialHidePw,
	BOOL bForceInTaskbar)
{
	m_dwRequestedPasswords = dwRequestedPasswords;
	m_bHidePw = bInitialHidePw;
	m_bShowInTaskbar = bForceInTaskbar;
}

LPTSTR CPwGeneratorExDlg::GetGeneratedPassword() const
{
	return m_lpPassword;
}

void CPwGeneratorExDlg::OnOK()
{
	UpdateData(TRUE);

	ASSERT(m_lpPassword == NULL);
	if(m_lpPassword != NULL) CSecureEditEx::DeletePassword(m_lpPassword);
	m_lpPassword = m_cEditPw.GetPassword();
	ASSERT(m_lpPassword != NULL); if(m_lpPassword == NULL) return;

	if(_tcslen(m_lpPassword) == 0)
	{
		CSecureEditEx::DeletePassword(m_lpPassword);
		m_lpPassword = NULL;
		return;
	}

	this->CleanUp();
	CDialog::OnOK();
}

void CPwGeneratorExDlg::OnCancel()
{
	this->CleanUp();
	CDialog::OnCancel();
}

void CPwGeneratorExDlg::CleanUp()
{
	SaveGenProfiles();
	CPwSafeDlg::m_pgsAutoProfile.strName = _T("");

	if(m_bShowInTaskbar == TRUE)
		WU_ShowWindowInTaskbar(this->m_hWnd, m_hPrevParent, FALSE);

	m_fStyle.DeleteObject();
	m_fSymbol.DeleteObject();
	m_fBold.DeleteObject();
}

void CPwGeneratorExDlg::UpdateDialogDataEx(BOOL bDialogToInternal,
	PW_GEN_SETTINGS_EX* pSettings)
{
	ASSERT(pSettings != NULL); if(pSettings == NULL) return;

	std::basic_string<WCHAR> strHighAnsi = PwCharSet::GetHighAnsiChars().ToString();
	std::basic_string<WCHAR> strSpecial = PwCharSet::GetSpecialChars().ToString();

	this->UpdateData(TRUE);

	if(bDialogToInternal == TRUE)
	{
		pSettings->strName = (LPCTSTR)GetCurrentGenProfile();

		const bool bCharSetBased = (m_rbCharSetBased.GetCheck() == BST_CHECKED);

		pSettings->btGeneratorType = (bCharSetBased ? PWGT_CHARSET :
			PWGT_PATTERN);
		pSettings->bCollectUserEntropy = m_bCollectEntropy;

		pSettings->dwLength = m_dwPasswordLength;

		PwCharSet cs;
		if(m_bCsUpperCase == TRUE) cs.Add(PDCS_UPPER_CASE);
		if(m_bCsLowerCase == TRUE) cs.Add(PDCS_LOWER_CASE);
		if(m_bCsNumeric == TRUE) cs.Add(PDCS_NUMERIC);
		if(m_bCsMinus == TRUE) cs.Add(L'-');
		if(m_bCsUnderline == TRUE) cs.Add(L'_');
		if(m_bCsSpace == TRUE) cs.Add(L' ');
		if(m_bCsBrackets == TRUE) cs.Add(PDCS_BRACKETS);
		if(m_bCsSpecial == TRUE) cs.Add(strSpecial.c_str());
		if(m_bCsHighAnsi == TRUE) cs.Add(strHighAnsi.c_str());

#ifdef _UNICODE
		cs.Add(m_strCustomCharSet);
#else
		LPWSTR lpCustom = _StringToUnicode(m_strCustomCharSet);
		cs.Add(lpCustom);
		SAFE_DELETE_ARRAY(lpCustom);
#endif

		pSettings->strCharSet = cs.ToString();

#ifdef _UNICODE
		pSettings->strPattern = m_strPattern;
#else
		LPWSTR lpPattern = _StringToUnicode(m_strPattern);
		pSettings->strPattern = lpPattern;
		SAFE_DELETE_ARRAY(lpPattern);
#endif

		pSettings->bPatternPermute = m_bPatternPermute;

		CPwGeneratorAdvDlg::CopyAdvancedOptions(&m_pgsAdvanced, pSettings);
	}
	else // bSave == FALSE
	{
		m_rbCharSetBased.SetCheck((pSettings->btGeneratorType ==
			PWGT_CHARSET) ? BST_CHECKED : BST_UNCHECKED);
		m_rbPatternBased.SetCheck((pSettings->btGeneratorType !=
			PWGT_CHARSET) ? BST_CHECKED : BST_UNCHECKED);

		m_bCollectEntropy = pSettings->bCollectUserEntropy;

		m_dwPasswordLength = pSettings->dwLength;

		PwCharSet cs(pSettings->strCharSet.c_str());

		m_bCsUpperCase = (cs.RemoveIfAllExist(PDCS_UPPER_CASE) ? TRUE : FALSE);
		m_bCsLowerCase = (cs.RemoveIfAllExist(PDCS_LOWER_CASE) ? TRUE : FALSE);
		m_bCsNumeric = (cs.RemoveIfAllExist(PDCS_NUMERIC) ? TRUE : FALSE);
		m_bCsMinus = (cs.RemoveIfAllExist(L"-") ? TRUE : FALSE);
		m_bCsUnderline = (cs.RemoveIfAllExist(L"_") ? TRUE : FALSE);
		m_bCsSpace = (cs.RemoveIfAllExist(L" ") ? TRUE : FALSE);
		m_bCsBrackets = (cs.RemoveIfAllExist(PDCS_BRACKETS) ? TRUE : FALSE);
		m_bCsSpecial = (cs.RemoveIfAllExist(strSpecial.c_str()) ? TRUE : FALSE);
		m_bCsHighAnsi = (cs.RemoveIfAllExist(strHighAnsi.c_str()) ? TRUE : FALSE);

#ifdef _UNICODE
		m_strCustomCharSet = cs.ToString().c_str();
		m_strPattern = pSettings->strPattern.c_str();
#else
		LPSTR lpCustom = _StringToAnsi(cs.ToString().c_str());
		m_strCustomCharSet = lpCustom;
		SAFE_DELETE_ARRAY(lpCustom);

		LPSTR lpPattern = _StringToAnsi(pSettings->strPattern.c_str());
		m_strPattern = lpPattern;
		SAFE_DELETE_ARRAY(lpPattern);
#endif

		m_bPatternPermute = pSettings->bPatternPermute;

		CPwGeneratorAdvDlg::CopyAdvancedOptions(pSettings, &m_pgsAdvanced);

		this->UpdateData(FALSE);
	}
}

#define ENSURE_ENABLED_STATE(v,b) { if((v).IsWindowEnabled() != (b)) { \
	(v).EnableWindow(b); } }
#define ENSURE_VISIBLE_STATE(v,b) { (v).ShowWindow(((b) == TRUE) ? SW_SHOW : SW_HIDE); }

void CPwGeneratorExDlg::EnableControlsEx(BOOL bSelectCustom)
{
	if(m_bBlockUIUpdate == TRUE) return;

	m_bBlockUIUpdate = TRUE;

	this->UpdateData(TRUE);

	if(bSelectCustom)
		m_cmbProfile.SelectString(-1, STR_CUSTOM_PROFILE);

	BOOL bCharSetBased = ((m_rbCharSetBased.GetCheck() == BST_CHECKED) ? TRUE : FALSE);

	ENSURE_ENABLED_STATE(m_stcLength, bCharSetBased);
	ENSURE_ENABLED_STATE(m_tbLength, bCharSetBased);
	ENSURE_ENABLED_STATE(m_spinNumChars, bCharSetBased);

	ENSURE_ENABLED_STATE(m_cbUpperCase, bCharSetBased);
	ENSURE_ENABLED_STATE(m_cbLowerCase, bCharSetBased);
	ENSURE_ENABLED_STATE(m_cbNumeric, bCharSetBased);
	ENSURE_ENABLED_STATE(m_cbMinus, bCharSetBased);
	ENSURE_ENABLED_STATE(m_cbUnderline, bCharSetBased);
	ENSURE_ENABLED_STATE(m_cbSpace, bCharSetBased);
	ENSURE_ENABLED_STATE(m_cbBrackets, bCharSetBased);
	ENSURE_ENABLED_STATE(m_cbSpecial, bCharSetBased);
	ENSURE_ENABLED_STATE(m_cbHighAnsi, bCharSetBased);

	ENSURE_ENABLED_STATE(m_stcCustomCharSet, bCharSetBased);
	ENSURE_ENABLED_STATE(m_tbCustomCharSet, bCharSetBased);

	BOOL bPattern = ((bCharSetBased == TRUE) ? FALSE : TRUE);

	ENSURE_ENABLED_STATE(m_tbPattern, bPattern);
	ENSURE_ENABLED_STATE(m_cbPatternPermute, bPattern);

	CString strCurrentName = GetCurrentGenProfile();
	BOOL bDelete = (((strCurrentName != STR_CUSTOM_PROFILE) &&
		(strCurrentName != STR_AUTO_PROFILE)) ? TRUE : FALSE);
	ENSURE_ENABLED_STATE(m_btnProfileDelete, bDelete);

	BOOL bNoAutoProfile = ((strCurrentName != STR_AUTO_PROFILE) ? TRUE : FALSE);

	ENSURE_ENABLED_STATE(m_cbCollectEntropy, bNoAutoProfile);

	LPTSTR lpPassword = m_cEditPw.GetPassword();
	NewGUI_ShowQualityMeter(&m_cPassQuality, GetDlgItem(IDC_STATIC_PASSBITS), lpPassword);

	if(m_dwRequestedPasswords > 0)
	{
		ENSURE_VISIBLE_STATE(m_btnOK, TRUE);
		ENSURE_ENABLED_STATE(m_btnOK, (lpPassword[0] != 0) ? TRUE : FALSE);
	}
	else // Can't accept
	{
		ENSURE_ENABLED_STATE(m_btnOK, FALSE);
		ENSURE_VISIBLE_STATE(m_btnOK, FALSE);
	}

	CSecureEditEx::DeletePassword(lpPassword); lpPassword = NULL;

	if(m_dwPasswordLength > PWGD_MAX_PASSWORD_LENGTH)
	{
		m_dwPasswordLength = PWGD_MAX_PASSWORD_LENGTH;
		this->UpdateData(FALSE);
	}

	COLORREF clr = ((PwgHasSecurityReducingOption(&m_pgsAdvanced) == TRUE) ?
		RGB(255, 0, 0) : m_clrControlText);
	m_btnAdvanced.SetColor(CButtonST::BTNST_COLOR_FG_IN, clr, FALSE);
	m_btnAdvanced.SetColor(CButtonST::BTNST_COLOR_FG_OUT, clr, FALSE);
	m_btnAdvanced.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, clr, TRUE);

	m_bBlockUIUpdate = FALSE;
}

void CPwGeneratorExDlg::OnRadioCharsetBased()
{
	this->EnableControlsEx(TRUE);
}

void CPwGeneratorExDlg::OnRadioPatternBased()
{
	this->EnableControlsEx(TRUE);
}

void CPwGeneratorExDlg::OnBnClickedBtnHelp()
{
	WU_OpenAppHelp(PWM_HELP_PWGEN);
}

void CPwGeneratorExDlg::LoadGenProfiles()
{
	TCHAR szTemp[SI_REGSIZE];
	CPrivateConfigEx cConfig(FALSE);

	m_vProfiles.clear();

	for(int nProfile = 0; true; ++nProfile)
	{
		CString strProfileKey;
		strProfileKey.Format(_T("%s%d"), PWMKEY_GENPROFILE, nProfile);
		cConfig.Get(strProfileKey, szTemp);

		if(szTemp[0] == 0) break;
		if(_tcscmp(szTemp, _T("0")) == 0) break;

		std::basic_string<TCHAR> tstr = szTemp;

		PW_GEN_SETTINGS_EX s;
		PwgStringToProfile(tstr, &s);

		m_vProfiles.push_back(s);
	}

	cConfig.Get(PWMKEY_GENPROFILELASTPR, szTemp);
	if((szTemp[0] != 0) && (_tcscmp(szTemp, _T("0")) != 0))
	{
		std::basic_string<TCHAR> strLast = szTemp;
		PwgStringToProfile(strLast, &m_pgsLast);
	}
}

void CPwGeneratorExDlg::SaveGenProfiles()
{
	CPrivateConfigEx cConfig(TRUE);
	CString strProfileKey;

	for(unsigned int uProfile = 0; uProfile < m_vProfiles.size(); ++uProfile)
	{
		strProfileKey.Format(_T("%s%u"), PWMKEY_GENPROFILE, uProfile);

		std::basic_string<TCHAR> str = PwgProfileToString(&m_vProfiles[uProfile]);

		cConfig.Set(strProfileKey, str.c_str());
	}

	strProfileKey.Format(_T("%s%u"), PWMKEY_GENPROFILE, m_vProfiles.size());
	cConfig.Set(strProfileKey, _T("0"));

	UpdateDialogDataEx(TRUE, &m_pgsLast);
	m_pgsLast.strName = (LPCTSTR)GetCurrentGenProfile();
	std::basic_string<TCHAR> strLast = PwgProfileToString(&m_pgsLast);
	cConfig.Set(PWMKEY_GENPROFILELASTPR, strLast.c_str());
}

PW_GEN_SETTINGS_EX* CPwGeneratorExDlg::FindGenProfile(CString strProfileName)
{
	if(strProfileName == STR_AUTO_PROFILE) return &CPwSafeDlg::m_pgsAutoProfile;

	for(DWORD i = 0; i < m_vProfiles.size(); ++i)
	{
		if(m_vProfiles[i].strName == (LPCTSTR)strProfileName)
			return &m_vProfiles[i];
	}

	return NULL;
}

void CPwGeneratorExDlg::ShowGenProfile(CString strProfileName)
{
	PW_GEN_SETTINGS_EX* pProfile = FindGenProfile(strProfileName);
	if(pProfile == NULL) { EnableControlsEx(FALSE); return; }

	UpdateDialogDataEx(FALSE, pProfile);
	EnableControlsEx(FALSE);
}

void CPwGeneratorExDlg::OnCbnSelChangeComboProfiles()
{
	CString strProfile = GetCurrentGenProfile();
	ShowGenProfile(strProfile);
	EnableControlsEx(FALSE);
}

void CPwGeneratorExDlg::OnBnClickedBtnProfileCreate()
{
	this->UpdateData(TRUE);

	std::vector<std::basic_string<TCHAR> > vSelectable;

	vSelectable.push_back(STR_AUTO_PROFILE);
	for(DWORD i = 0; i < m_vProfiles.size(); ++i)
		vSelectable.push_back(m_vProfiles[i].strName);

	CSingleLineEditDlg dlg;
	dlg.InitEx(TRL("Save as Profile"), TRL("Save current settings as password generator profile."),
		TRL("Please enter a name for the new password generator profile, or select an existing profile to overwrite it:"),
		PWGD_MAX_PROFILE_NAME_LENGTH, _T(""), vSelectable);

	if(dlg.DoModal() == IDOK)
	{
		CString strName = dlg.GetEnteredText();

		if(strName == STR_CUSTOM_PROFILE)
		{
			MessageBox(TRL("Path is invalid"), PWM_PRODUCT_NAME_SHORT, MB_OK | MB_ICONWARNING);
			return;
		}

		PW_GEN_SETTINGS_EX s;
		UpdateDialogDataEx(TRUE, &s);
		s.strName = strName;

		if(strName == STR_AUTO_PROFILE)
		{
			s.bCollectUserEntropy = FALSE;
			CPwSafeDlg::m_pgsAutoProfile = s;
		}
		else
		{
			BOOL bExists = FALSE;
			for(DWORD i = 0; i < m_vProfiles.size(); ++i)
			{
				if(m_vProfiles[i].strName == (LPCTSTR)strName)
				{
					m_vProfiles[i] = s;
					bExists = TRUE;
					break;
				}
			}

			if(bExists == FALSE) m_vProfiles.push_back(s);
		}

		RecreateProfilesList();
		m_cmbProfile.SelectString(-1, strName);
		ShowGenProfile(strName);
	}
}

CString CPwGeneratorExDlg::GetCurrentGenProfile()
{
	CString strProfile;
	const int nSel = m_cmbProfile.GetCurSel();
	if(nSel < 0) m_cmbProfile.GetWindowText(strProfile);
	else m_cmbProfile.GetLBText(nSel, strProfile);
	return strProfile;
}

void CPwGeneratorExDlg::RecreateProfilesList()
{
	CString strPrevText = GetCurrentGenProfile();

	m_cmbProfile.ResetContent();

	m_cmbProfile.AddString(STR_CUSTOM_PROFILE);
	m_cmbProfile.AddString(STR_AUTO_PROFILE);

	for(DWORD i = 0; i < m_vProfiles.size(); ++i)
		m_cmbProfile.AddString(m_vProfiles[i].strName.c_str());

	if(m_cmbProfile.SelectString(-1, strPrevText) == CB_ERR)
		m_cmbProfile.SelectString(-1, STR_CUSTOM_PROFILE);

	EnableControlsEx(FALSE);
}

void CPwGeneratorExDlg::OnBnClickedBtnProfileDelete()
{
	this->UpdateData(TRUE);

	CString strProfile = GetCurrentGenProfile();

	for(std::vector<PW_GEN_SETTINGS_EX>::iterator it = m_vProfiles.begin();
		it != m_vProfiles.end(); ++it)
	{
		if(it->strName == (LPCTSTR)strProfile)
		{
			m_vProfiles.erase(it);

			RecreateProfilesList();
			break;
		}
	}
}

void CPwGeneratorExDlg::OnBnClickedGenerateBtn()
{
	PW_GEN_SETTINGS_EX pws;
	this->UpdateDialogDataEx(TRUE, &pws);

	CNewRandom randomSource;
	if(m_bCollectEntropy == TRUE)
	{
		CGetRandomDlg dlg;
		if(dlg.DoModal() == IDCANCEL) return;

		randomSource.AddToUserEntropyPool(dlg.m_pFinalRandom, 32);
	}

	std::vector<TCHAR> strPassword;
	USHORT uError = PwgGenerateWithExtVerify(strPassword, &pws, &randomSource);

	if(uError != PWGE_SUCCESS)
	{
		std::basic_string<TCHAR> strError = _TRL(PwgErrorToString(uError));
		strError += _T(".");
		MessageBox(strError.c_str(), PWM_PRODUCT_NAME_SHORT, MB_OK | MB_ICONWARNING);

		EraseTCharVector(strPassword);
	}

	if(strPassword.size() > PWGD_MAX_PASSWORD_LENGTH)
		strPassword[PWGD_MAX_PASSWORD_LENGTH] = 0; // Truncate

	if(strPassword.size() > 0) m_cEditPw.SetPassword(&strPassword[0]);
	else m_cEditPw.SetPassword(_T(""));

	EraseTCharVector(strPassword);

	this->EnableControlsEx(FALSE);
}

void CPwGeneratorExDlg::OnCheckHidePw()
{
	UpdateData(TRUE);

	if(m_bHidePw == FALSE)
	{
		m_cEditPw.EnableSecureMode(FALSE);
		m_cEditPw.SetPasswordChar(0);
		m_cEditPw.SetFont(&m_fStyle, TRUE);
	}
	else
	{
		// TCHAR tchDot = (TCHAR)(_T('z') + 27);
		TCHAR tchDot = CPwSafeApp::GetPasswordCharacter();
		m_cEditPw.EnableSecureMode(CPwSafeDlg::m_bSecureEdits);
		m_cEditPw.SetPasswordChar(tchDot);
		m_cEditPw.SetFont(&m_fSymbol, TRUE);
	}

	m_tipSecClear.Activate(m_cEditPw.IsSecureModeEnabled());

	UpdateData(FALSE);
	m_cEditPw.RedrawWindow();
	m_cEditPw.SetFocus();
}

void CPwGeneratorExDlg::OnDeltaPosSpinLength(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;

	UpdateData(TRUE);

	int nPos = (int)m_dwPasswordLength;
	nPos += pNMUpDown->iDelta;
	if(nPos < 0) nPos = 0;
	m_dwPasswordLength = static_cast<DWORD>(nPos);

	m_spinNumChars.SetPos(512);

	UpdateData(FALSE);
	this->EnableControlsEx(TRUE);
}

void CPwGeneratorExDlg::OnEnChangeEditPw()
{
	this->EnableControlsEx(FALSE);
}

BOOL CPwGeneratorExDlg::PreTranslateMessage(MSG* pMsg)
{
	m_tipSecClear.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}

void CPwGeneratorExDlg::OnEnChangeEditLength()
{
	this->EnableControlsEx(TRUE);
}

void CPwGeneratorExDlg::OnEnChangeEditCustomCharSet()
{
	this->EnableControlsEx(TRUE);
}

void CPwGeneratorExDlg::OnEnChangeEditPattern()
{
	this->EnableControlsEx(TRUE);
}

void CPwGeneratorExDlg::OnBnClickedCheckCsUppercase()
{
	this->EnableControlsEx(TRUE);
}

void CPwGeneratorExDlg::OnBnClickedCheckCsLowercase()
{
	this->EnableControlsEx(TRUE);
}

void CPwGeneratorExDlg::OnBnClickedCheckCsNumeric()
{
	this->EnableControlsEx(TRUE);
}

void CPwGeneratorExDlg::OnBnClickedCheckCsMinus()
{
	this->EnableControlsEx(TRUE);
}

void CPwGeneratorExDlg::OnBnClickedCheckCsUnderline()
{
	this->EnableControlsEx(TRUE);
}

void CPwGeneratorExDlg::OnBnClickedCheckCsSpace()
{
	this->EnableControlsEx(TRUE);
}

void CPwGeneratorExDlg::OnBnClickedCheckCsSpecial()
{
	this->EnableControlsEx(TRUE);
}

void CPwGeneratorExDlg::OnBnClickedCheckCsBrackets()
{
	this->EnableControlsEx(TRUE);
}

void CPwGeneratorExDlg::OnBnClickedCheckCsHighansi()
{
	this->EnableControlsEx(TRUE);
}

void CPwGeneratorExDlg::OnBnClickedCheckCollectEntropy()
{
	this->EnableControlsEx(TRUE);
}

void CPwGeneratorExDlg::OnBnClickedAdvanced()
{
	CPwGeneratorAdvDlg dlg;
	dlg.InitEx(&m_pgsAdvanced);
	if(dlg.DoModal() == IDOK) this->EnableControlsEx(TRUE);
}

void CPwGeneratorExDlg::OnBnClickedPatternPermute()
{
	this->EnableControlsEx(TRUE);
}
