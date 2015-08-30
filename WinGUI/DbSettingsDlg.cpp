/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2008 Dominik Reichl <dominik.reichl@t-online.de>

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
#include <mmsystem.h>
#include "../KeePassLibCpp/PwManager.h"
#include "../KeePassLibCpp/Crypto/Rijndael.h"
#include "Util/WinUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "NewGUI/NewGUICommon.h"
#include "../KeePassLibCpp/Util/TranslateEx.h"

/////////////////////////////////////////////////////////////////////////////

CDbSettingsDlg::CDbSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDbSettingsDlg::IDD, pParent),
	m_strDefaultUserName(_T(""))
{
	//{{AFX_DATA_INIT(CDbSettingsDlg)
	m_nAlgorithm = -1;
	m_dwNumKeyEnc = 0;
	//}}AFX_DATA_INIT
}

void CDbSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDbSettingsDlg)
	DDX_Control(pDX, IDC_HLINK_HELPFILE, m_hlHelp);
	DDX_Control(pDX, IDC_BTN_CALCROUNDS, m_btCalcRounds);
	DDX_Control(pDX, IDC_COMBO_ENCALGOS, m_cEncAlgos);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_CBIndex(pDX, IDC_COMBO_ENCALGOS, m_nAlgorithm);
	DDX_Text(pDX, IDC_EDIT_KEYENC, m_dwNumKeyEnc);
	DDX_Text(pDX, IDC_EDIT_DEFAULTUSER, m_strDefaultUserName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDbSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(CDbSettingsDlg)
	ON_BN_CLICKED(IDC_BTN_CALCROUNDS, OnBtnCalcRounds)
	//}}AFX_MSG_MAP

	ON_REGISTERED_MESSAGE(WM_XHYPERLINK_CLICKED, OnXHyperLinkClicked)
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

	CString str;
	m_btCalcRounds.GetWindowText(str);
	m_btCalcRounds.SetTooltipText(str + _T("."));
	m_btCalcRounds.SetWindowText(_T(""));

	str = TRL("Open &Help File"); str.Remove(_T('&'));
	m_hlHelp.SetWindowText(str);
	NewGUI_MakeHyperLink(&m_hlHelp);
	m_hlHelp.EnableTooltip(FALSE);
	m_hlHelp.SetNotifyParent(TRUE);
	m_hlHelp.EnableURL(FALSE);

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_OPTIONS),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(TRL("Database Settings"));
	m_banner.SetCaption(TRL("Here you can configure the current database."));

	m_cEncAlgos.ResetContent();
	m_cEncAlgos.AddString(TRL("Advanced Encryption Standard (AES) (128-bit block cipher using 256-bit key)"));
	m_cEncAlgos.AddString(TRL("Twofish (128-bit block cipher using 256-bit key)"));
	m_cEncAlgos.SetCurSel(m_nAlgorithm);

	UpdateData(FALSE);

	return TRUE; // Return TRUE unless you set the focus to a control
}

void CDbSettingsDlg::OnOK() 
{
	UpdateData(TRUE);

	// Check if the user has entered something valid, otherwise fix it
	CString str;
	GetDlgItem(IDC_EDIT_KEYENC)->GetWindowText(str);
	str.TrimLeft(); str.TrimRight();
	if(str.GetLength() > 10) m_dwNumKeyEnc = 0xFFFFFFFE; // Set to max
	else if(str.GetLength() == 10)
	{
		if(str.GetAt(0) >= _T('4')) m_dwNumKeyEnc = 0xFFFFFFFE; // Set to max
	}

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
	CRijndael rijndael;
	UINT8 aKeySeed[32];
	UINT8 aTest[32];
	DWORD iRounds = 0;

	UpdateData(TRUE);

	memset(aKeySeed, 0x4b, 32);
	memset(aTest, 0x8e, 32);

	if(rijndael.Init(CRijndael::ECB, CRijndael::EncryptDir, aKeySeed, CRijndael::Key32Bytes, 0) != RIJNDAEL_SUCCESS)
	{
		MessageBox(TRL("Internal error"), PWM_PRODUCT_NAME_SHORT,
			MB_ICONSTOP | MB_OK);
		return;
	}

	const DWORD dwStartTime = timeGetTime();

	while(true)
	{
		rijndael.BlockEncrypt(aTest, 256, aTest);
		rijndael.BlockEncrypt(aTest, 256, aTest);
		rijndael.BlockEncrypt(aTest, 256, aTest);
		rijndael.BlockEncrypt(aTest, 256, aTest);
		rijndael.BlockEncrypt(aTest, 256, aTest);
		rijndael.BlockEncrypt(aTest, 256, aTest);
		rijndael.BlockEncrypt(aTest, 256, aTest);
		rijndael.BlockEncrypt(aTest, 256, aTest);
		iRounds += 8;

		if(iRounds < 8) { iRounds = DWORD_MAX - 8; break; } // Overflow?
		if((timeGetTime() - dwStartTime) > 1000) break;
	}

	m_dwNumKeyEnc = iRounds;
	UpdateData(FALSE);
}

LRESULT CDbSettingsDlg::OnXHyperLinkClicked(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	if(wParam == IDC_HLINK_HELPFILE) WU_OpenAppHelp(PWM_HELP_SECURITY);

	return 0;
}
