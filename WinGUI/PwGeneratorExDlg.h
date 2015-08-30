/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2007 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___PW_GENERATOR_EX_DLG_H___
#define ___PW_GENERATOR_EX_DLG_H___

#include "afxwin.h"

#pragma once

#include "afxcmn.h"

#include "../KeePassLibCpp/SysDefEx.h"
#include "../KeePassLibCpp/PasswordGenerator/PasswordGenerator.h"

#include "NewGUI/XPStyleButtonST.h"
#include "NewGUI/KCSideBannerWnd.h"
#include "NewGUI/SecureEditEx.h"
#include "NewGUI/GradientProgressCtrl.h"

#define STR_CUSTOM_PROFILE TRL("(Custom)")
#define STR_AUTO_PROFILE TRL("(Automatically generated passwords for new entries)")

#define PWGD_MAX_PASSWORD_LENGTH 30000

#define PWGD_MPL (((SI_REGSIZE * 3) / 4) - 4)
#define PWGD_MPVL (PWGD_MPL - 16)

#define PWGD_MAX_PROFILE_NAME_LENGTH ((PWGD_MPVL / 4) / 2)
#define PWGD_MAX_PROFILE_CHARSET_LENGTH ((PWGD_MPVL / 2) / 2)
#define PWGD_MAX_PROFILE_PATTERN_LENGTH ((PWGD_MPVL / 4) / 2)

class CPwGeneratorExDlg : public CDialog
{
	DECLARE_DYNAMIC(CPwGeneratorExDlg)

private:
	void CleanUp();

	void UpdateDialogDataEx(BOOL bDialogToInternal, PW_GEN_SETTINGS_EX* pSettings);
	void EnableControlsEx(BOOL bSelectCustom);
	void ShowGenProfile(CString strProfileName);

	void RecreateProfilesList();

	void LoadGenProfiles();
	void SaveGenProfiles();
	PW_GEN_SETTINGS_EX* FindGenProfile(CString strProfileName);

	CString GetCurrentGenProfile();

	CKCSideBannerWnd m_banner;

	CFont m_fStyle;
	CFont m_fSymbol;
	CFont m_fBold;
	CToolTipCtrl m_tipSecClear;
	HWND m_hPrevParent;

	DWORD m_dwRequestedPasswords;

	std::vector<PW_GEN_SETTINGS_EX> m_vProfiles;
	PW_GEN_SETTINGS_EX m_pgsLast;

	LPTSTR m_lpPassword;

	BOOL m_bBlockUIUpdate;
	BOOL m_bShowInTaskbar;

public:
	CPwGeneratorExDlg(CWnd* pParent = NULL);
	virtual ~CPwGeneratorExDlg();

	void InitEx(DWORD dwRequestedPasswords, BOOL bInitialHidePw,
		BOOL bForceInTaskbar);

	LPTSTR GetGeneratedPassword() const;

	enum { IDD = IDD_PW_GENERATOR_EX_DLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnCancel();
	afx_msg void OnOK();
	CComboBox m_cmbProfile;
	CXPStyleButtonST m_btnProfileCreate;
	CXPStyleButtonST m_btnProfileDelete;
	DWORD m_dwPasswordLength;
	BOOL m_bCollectEntropy;
	BOOL m_bCsUpperCase;
	BOOL m_bCsLowerCase;
	BOOL m_bCsNumeric;
	BOOL m_bCsMinus;
	BOOL m_bCsUnderline;
	BOOL m_bCsSpace;
	BOOL m_bCsSpecial;
	BOOL m_bCsBrackets;
	BOOL m_bCsHighAnsi;
	CButton m_cbUpperCase;
	CButton m_cbLowerCase;
	CButton m_cbNumeric;
	CButton m_cbMinus;
	CButton m_cbUnderline;
	CButton m_cbSpace;
	CButton m_cbSpecial;
	CButton m_cbBrackets;
	CButton m_cbHighAnsi;
	CString m_strCustomCharSet;
	CString m_strPattern;
	BOOL m_bNoConfusing;
	CXPStyleButtonST m_btnOK;
	CXPStyleButtonST m_btnCancel;
	CStatic m_stcCustomCharSet;
	CEdit m_tbCustomCharSet;
	CEdit m_tbPattern;
	afx_msg void OnRadioCharsetBased();
	afx_msg void OnRadioPatternBased();
	afx_msg void OnBnClickedBtnHelp();
	CXPStyleButtonST m_btnHelp;
	afx_msg void OnCbnSelChangeComboProfiles();
	afx_msg void OnBnClickedBtnProfileCreate();
	afx_msg void OnBnClickedBtnProfileDelete();
	CButton m_rbCharSetBased;
	CButton m_rbPatternBased;
	CButton m_cbCollectEntropy;
	afx_msg void OnBnClickedGenerateBtn();
	CSecureEditEx m_cEditPw;
	CXPStyleButtonST m_btnHidePw;
	CSpinButtonCtrl m_spinNumChars;
	afx_msg void OnCheckHidePw();
	CXPStyleButtonST m_btnGenerate;
	CGradientProgressCtrl m_cPassQuality;
	afx_msg void OnDeltaPosSpinLength(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditPw();
	BOOL m_bHidePw;
	CStatic m_stcLength;
	CEdit m_tbLength;
	afx_msg void OnEnChangeEditLength();
	afx_msg void OnEnChangeEditCustomCharSet();
	afx_msg void OnEnChangeEditPattern();
	afx_msg void OnBnClickedCheckCsUppercase();
	afx_msg void OnBnClickedCheckCsLowercase();
	afx_msg void OnBnClickedCheckCsNumeric();
	afx_msg void OnBnClickedCheckCsMinus();
	afx_msg void OnBnClickedCheckCsUnderline();
	afx_msg void OnBnClickedCheckCsSpace();
	afx_msg void OnBnClickedCheckCsSpecial();
	afx_msg void OnBnClickedCheckCsBrackets();
	afx_msg void OnBnClickedCheckCsHighansi();
	afx_msg void OnBnClickedCheckNoConfusing();
	afx_msg void OnBnClickedCheckCollectEntropy();
	CButton m_cbPatternPermute;
	BOOL m_bPatternPermute;
};

#endif // ___PW_GENERATOR_EX_DLG_H___
