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

#ifndef ___PW_GENERATOR_ADV_OPTIONS_DLG_H___
#define ___PW_GENERATOR_ADV_OPTIONS_DLG_H___

#pragma once

#include "../KeePassLibCpp/SysDefEx.h"
#include "../KeePassLibCpp/PasswordGenerator/PasswordGenerator.h"

#include "NewGUI/XPStyleButtonST.h"
#include "NewGUI/KCSideBannerWnd.h"
#include <afxwin.h>

class CPwGeneratorAdvDlg : public CDialog
{
	DECLARE_DYNAMIC(CPwGeneratorAdvDlg)

public:
	CPwGeneratorAdvDlg(CWnd* pParent = NULL);
	virtual ~CPwGeneratorAdvDlg();

	enum { IDD = IDD_PW_GEN_ADV_DLG };

	void InitEx(PW_GEN_SETTINGS_EX* pWorkingStruct);

	static void CopyAdvancedOptions(const PW_GEN_SETTINGS_EX* pSource,
		PW_GEN_SETTINGS_EX* pDest);

private:
	CKCSideBannerWnd m_banner;

	PW_GEN_SETTINGS_EX* m_pOpt;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnOK();
	afx_msg void OnBtnCancel();
	CXPStyleButtonST m_btOK;
	CXPStyleButtonST m_btCancel;
	BOOL m_bNoRepeat;
	BOOL m_bNoConfusing;
	afx_msg void OnBtnHelp();
	CXPStyleButtonST m_btHelp;
	CString m_strExcludeChars;
};

#endif // ___PW_GENERATOR_ADV_OPTIONS_DLG_H___
