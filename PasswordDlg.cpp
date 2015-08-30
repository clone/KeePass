/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2006 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "PwSafeDlg.h"
#include "PasswordDlg.h"

#include "Util/StrUtil.h"
#include "PwGeneratorDlg.h"
#include "Util/base64.h"
#include "NewGUI/TranslateEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(push)
// Cast truncates constant value
#pragma warning(disable: 4310)

/////////////////////////////////////////////////////////////////////////////

CPasswordDlg::CPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPasswordDlg)
	m_bStars = TRUE;
	m_bKeyMethod = FALSE;
	//}}AFX_DATA_INIT

	m_bLoadMode = TRUE;
	m_bConfirm = FALSE;
	m_bChanging = FALSE;
	m_strDescriptiveName = _T("");
	m_bOnce = FALSE;
	m_hWindowIcon = NULL;
	m_lpPreSelectPath = m_lpKey = m_lpKey2 = NULL;
}

void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswordDlg)
	DDX_Control(pDX, IDC_COMBO_DISKLIST, m_cbDiskList);
	DDX_Control(pDX, IDC_HL_SELECTFILE, m_hlSelFile);
	DDX_Control(pDX, IDC_PROGRESS_PASSQUALITY, m_cPassQuality);
	DDX_Control(pDX, IDC_CHECK_STARS, m_btStars);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_MAKEPASSWORD_BTN, m_btMakePw);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_pEditPw);
	DDX_Check(pDX, IDC_CHECK_STARS, m_bStars);
	DDX_Check(pDX, IDC_CHECK_KEYMETHOD_AND, m_bKeyMethod);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPasswordDlg, CDialog)
	//{{AFX_MSG_MAP(CPasswordDlg)
	ON_BN_CLICKED(IDC_CHECK_STARS, OnCheckStars)
	ON_BN_CLICKED(IDC_MAKEPASSWORD_BTN, OnMakePasswordBtn)
	ON_EN_CHANGE(IDC_EDIT_PASSWORD, OnChangeEditPassword)
	ON_CBN_SELCHANGE(IDC_COMBO_DISKLIST, OnSelChangeComboDiskList)
	ON_BN_CLICKED(IDC_CHECK_KEYMETHOD_AND, OnCheckKeymethodAnd)
	//}}AFX_MSG_MAP

	ON_REGISTERED_MESSAGE(WM_XHYPERLINK_CLICKED, OnXHyperLinkClicked)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CPasswordDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	ASSERT(m_bOnce == FALSE);

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_APPWINDOW);

	ASSERT(m_hWindowIcon != NULL);
	if(m_hWindowIcon != NULL)
	{
		SetIcon(m_hWindowIcon, TRUE);
		SetIcon(m_hWindowIcon, FALSE);
	}

	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	m_fStyle.CreateFont(-12, 0, 0, 0, 0, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, _T("Tahoma"));
	m_fSymbol.CreateFont(-13, 0, 0, 0, 0, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, CPwSafeApp::GetPasswordFont());

	NewGUI_XPButton(&m_btOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(&m_btCancel, IDB_CANCEL, IDB_CANCEL);
	NewGUI_XPButton(&m_btMakePw, IDB_KEY_SMALL, IDB_KEY_SMALL);
	NewGUI_XPButton(&m_btStars, -1, -1);

	m_btStars.SetFont(&m_fSymbol, TRUE);
	m_pEditPw.SetFont(&m_fSymbol, TRUE);

	m_btStars.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 0, 255), TRUE);
	CString strTT = TRL("Hide &Passwords Behind Asterisks (***)"); strTT.Remove(_T('&'));
	m_btStars.SetTooltipText(strTT, TRUE);

	NewGUI_ConfigQualityMeter(&m_cPassQuality);

	// m_ilIcons.Create(IDR_INFOICONS, 16, 1, RGB(255,0,255)); // Purple is transparent
	CPwSafeApp::CreateHiColorImageList(&m_ilIcons, IDB_INFOICONS_EX, 16);
	m_cbDiskList.SetImageList(&m_ilIcons);

	COMBOBOXEXITEM cbi;
	ZeroMemory(&cbi, sizeof(COMBOBOXEXITEM));
	cbi.mask = CBEIF_IMAGE | CBEIF_TEXT | CBEIF_INDENT | CBEIF_SELECTEDIMAGE;
	cbi.iItem = 0;
	cbi.pszText = (LPTSTR)TRL("<no drive selected>");
	cbi.cchTextMax = (int)_tcslen(cbi.pszText);
	cbi.iImage = cbi.iSelectedImage = ICOIDX_NODRIVE;
	cbi.iIndent = 0;
	m_cbDiskList.InsertItem(&cbi);

	int i; TCHAR c; UINT uStat; CString str; BYTE idxImage;
	int j = 1;
	for(i = 0; i < 26; i++)
	{
		c = (TCHAR)(i + _T('A'));
		str = CString(c) + _T(":\\");
		uStat = GetDriveType((LPCTSTR)str);
		if(uStat != DRIVE_NO_ROOT_DIR)
		{
			idxImage = 0;
			if(uStat == DRIVE_REMOVABLE) idxImage = ICOIDX_REMOVABLE;
			if(uStat == DRIVE_FIXED) idxImage = ICOIDX_FIXED;
			if(uStat == DRIVE_REMOTE) idxImage = ICOIDX_REMOTE;
			if(uStat == DRIVE_CDROM) idxImage = ICOIDX_CDROM;
			if(uStat == DRIVE_RAMDISK) idxImage = ICOIDX_RAMDISK;

			ZeroMemory(&cbi, sizeof(COMBOBOXEXITEM));
			cbi.mask = CBEIF_IMAGE | CBEIF_TEXT | CBEIF_INDENT | CBEIF_SELECTEDIMAGE;
			cbi.iItem = j; j++;
			cbi.pszText = (LPTSTR)(LPCTSTR)str;
			cbi.cchTextMax = (int)_tcslen(cbi.pszText);
			cbi.iImage = cbi.iSelectedImage = (int)idxImage;
			cbi.iIndent = 0;
			m_cbDiskList.InsertItem(&cbi);
		}
	}
	m_cbDiskList.SetCurSel(0);

	DWORD dw = m_cbDiskList.GetExtendedStyle();
	dw &= ~CBES_EX_NOEDITIMAGE;
	m_cbDiskList.SetExtendedStyle(0, dw);

	NewGUI_MakeHyperLink(&m_hlSelFile);
	m_hlSelFile.EnableTooltip(FALSE);
	m_hlSelFile.SetNotifyParent(TRUE);
	m_hlSelFile.EnableURL(FALSE);

	NewGUI_ConfigSideBanner(&m_banner, this);
	if(m_bLoadMode == FALSE)
		m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_KEY),
			KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	else
		m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_KEYHOLE),
			KCSB_ICON_LEFT | KCSB_ICON_VCENTER);

	LPCTSTR lp;

	lp = TRL("Enter the passphrase and/or select the key-disk's drive.");
	GetDlgItem(IDC_STATIC_INTRO)->SetWindowText(lp);

	lp = TRL("Enter password:");
	GetDlgItem(IDC_STATIC_ENTERPW)->SetWindowText(lp);

	if(m_bConfirm == FALSE)
	{
		if(m_bLoadMode == FALSE)
		{
			lp = TRL("Select the password disk drive where the key will be stored:");
			GetDlgItem(IDC_STATIC_SELDISK)->SetWindowText(lp);

			lp = TRL("Save key-file manually to...");
			m_hlSelFile.SetWindowText(lp);

			CString str;
			str = TRL("Set master key");
			if(m_strDescriptiveName.GetLength() != 0)
			{
				str += _T(" - ");
				str += m_strDescriptiveName;
			}
			m_banner.SetTitle(str);

			if(m_bChanging == FALSE)
			{
				SetWindowText(TRL("Create a new password database - Enter master key"));
				m_banner.SetCaption(TRL("Enter the master key for the new database."));
			}
			else
			{
				SetWindowText(TRL("Change password of this database - Enter new master key"));
				m_banner.SetCaption(TRL("Enter the new master key for this database."));
			}
		}
		else // m_bConfirm == FALSE, m_bLoadMode == TRUE
		{
			lp = TRL("Select the password disk drive to load the key from:");
			GetDlgItem(IDC_STATIC_SELDISK)->SetWindowText(lp);

			lp = TRL("Select key-file manually...");
			m_hlSelFile.SetWindowText(lp);

			SetWindowText(TRL("Open database - Enter master key"));

			CString str;
			str = TRL("Enter master key");
			if(m_strDescriptiveName.GetLength() != 0)
			{
				str += _T(" - ");
				str += m_strDescriptiveName;
			}
			m_banner.SetTitle(str);

			m_banner.SetCaption(TRL("Enter the master key for this database."));

			m_btMakePw.ShowWindow(SW_HIDE);
			m_cPassQuality.ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STATIC_PASSBITS)->ShowWindow(SW_HIDE);
		}
	}
	else // m_bConfirm == TRUE
	{
		if(m_bLoadMode == FALSE)
		{
			GetDlgItem(IDC_STATIC_INTRO)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STATIC_SELDISK)->ShowWindow(SW_HIDE);
			m_cbDiskList.ShowWindow(SW_HIDE);
			m_btMakePw.EnableWindow(FALSE);
			m_hlSelFile.ShowWindow(SW_HIDE);

			if(m_bChanging == FALSE)
			{
				SetWindowText(TRL("Create a new password database - Repeat master key"));
				m_banner.SetCaption(TRL("Repeat the master key for the new database."));
			}
			else
			{
				SetWindowText(TRL("Repeat master key"));
				m_banner.SetCaption(TRL("Repeat the master key for this database."));
			}
			m_banner.SetTitle(TRL("Repeat master password"));

			m_cPassQuality.ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STATIC_PASSBITS)->ShowWindow(SW_HIDE);

			GetDlgItem(IDC_CHECK_KEYMETHOD_AND)->ShowWindow(SW_HIDE);
		}
		else
		{
			// This combination isn't possible
			ASSERT(FALSE);

			// Just in case... assume we want to load something
			lp = TRL("Select the password disk drive to load the key from:");
			GetDlgItem(IDC_STATIC_SELDISK)->SetWindowText(lp);

			SetWindowText(TRL("Open database - Enter master key"));

			CString str;
			str = TRL("Enter master key");
			if(m_strDescriptiveName.GetLength() != 0)
			{
				str += _T(" - ");
				str += m_strDescriptiveName;
			}
			m_banner.SetTitle(str);
			m_banner.SetCaption(TRL("Enter the master key for this database."));

			m_btMakePw.ShowWindow(SW_HIDE);
		}
	}

	m_tipSecClear.Create(this, 0x40);
	m_tipSecClear.AddTool(&m_pEditPw, CPwSafeDlg::_GetSecureEditTipText(_T("Enter password:")));
	m_tipSecClear.SetMaxTipWidth(630);
	m_tipSecClear.Activate(m_pEditPw.IsSecureModeEnabled());

	// Alternative password asterisk character: 0xB7 (smaller dot)
	// TCHAR tchDot = (TCHAR)(_T('z') + 27);
	TCHAR tchDot = CPwSafeApp::GetPasswordCharacter();
	CString strStars; strStars += tchDot; strStars += tchDot; strStars += tchDot;
	m_btStars.SetWindowText(strStars);
	m_bStars = TRUE;
	OnCheckStars();

	UpdateData(FALSE);

	NewGUI_ShowQualityMeter(&m_cPassQuality, GetDlgItem(IDC_STATIC_PASSBITS), _T(""));

	if(m_lpPreSelectPath != NULL)
	{
		int i, nSpecLen = _tcslen(m_lpPreSelectPath);
		CString strCBI;
		BOOL bFound = FALSE;

		for(i = 0; i < m_cbDiskList.GetCount(); i++)
		{
			m_cbDiskList.GetLBText(i, strCBI);
			if(strCBI.GetLength() < nSpecLen) continue;

			if(_tcsnicmp((LPCTSTR)strCBI, m_lpPreSelectPath, nSpecLen) == 0)
			{
				m_cbDiskList.SetCurSel(i);
				OnSelChangeComboDiskList();
				bFound = TRUE;
				break;
			}
		}

		if(bFound == FALSE)
		{
			COMBOBOXEXITEM cbi;
			ZeroMemory(&cbi, sizeof(COMBOBOXEXITEM));
			cbi.mask = CBEIF_IMAGE | CBEIF_TEXT | CBEIF_INDENT | CBEIF_SELECTEDIMAGE;
			cbi.iItem = m_cbDiskList.GetCount();
			cbi.pszText = (LPTSTR)m_lpPreSelectPath;
			cbi.cchTextMax = (int)_tcslen(cbi.pszText);
			cbi.iImage = cbi.iSelectedImage = 28;
			cbi.iIndent = 0;
			int nx = m_cbDiskList.InsertItem(&cbi);

			m_cbDiskList.SetCurSel(nx);
			OnSelChangeComboDiskList();
		}

		m_bKeyMethod = TRUE;
		UpdateData(FALSE);
		EnableClientWindows();
	}

	m_pEditPw.SetFocus();
	return FALSE; // Return TRUE unless you set the focus to a control
}

void CPasswordDlg::CleanUp()
{
	m_cbDiskList.ResetContent();
	m_ilIcons.DeleteImageList();
	m_fStyle.DeleteObject();
	m_fSymbol.DeleteObject();
}

void CPasswordDlg::FreePasswords()
{
	ASSERT(m_lpKey != NULL);

	if(m_lpKey != NULL)
	{
		m_pEditPw.DeletePassword(m_lpKey);
		m_lpKey = NULL;
	}

	if(m_lpKey2 != NULL)
	{
		m_pEditPw.DeletePassword(m_lpKey2);
		m_lpKey2 = NULL;
	}
}

void CPasswordDlg::OnOK() 
{
	CString strTemp;
	ULARGE_INTEGER aBytes[3];
	LPTSTR lpPassword;

	UpdateData(TRUE);

	ASSERT((m_lpKey == NULL) && (m_lpKey2 == NULL));
	m_lpKey = m_pEditPw.GetPassword();
	lpPassword = m_lpKey;

	if(m_bConfirm == FALSE)
	{
		// Validate input
		if(m_bKeyMethod == PWM_KEYMETHOD_OR)
		{
			if(!((_tcslen(m_lpKey) == 0) ^ (m_cbDiskList.GetCurSel() == 0)))
			{
				MessageBox(TRL("EITHER enter a password/passphrase OR select a key disk drive."),
					TRL("Password Safe"), MB_OK | MB_ICONINFORMATION);
				FreePasswords();
				return;
			}
		}
		else // m_bKeyMethod == PWM_KEYMETHOD_AND
		{
			if((_tcslen(m_lpKey) == 0) || (m_cbDiskList.GetCurSel() == 0))
			{
				MessageBox(TRL("You've selected the AND key mode, so you must enter a password AND select a key-file."),
					TRL("Password Safe"), MB_OK | MB_ICONINFORMATION);
				FreePasswords();
				return;
			}
		}
	}

	if((m_bKeyMethod == PWM_KEYMETHOD_OR) && (m_cbDiskList.GetCurSel() == 0))
	{
		m_bKeyFile = FALSE;
	}
	else
	{
		m_cbDiskList.GetLBText(m_cbDiskList.GetCurSel(), strTemp);

		if(m_bLoadMode == FALSE)
		{
			if(strTemp.GetAt(strTemp.GetLength() - 1) == _T('\\'))
			{
				if(GetDiskFreeSpaceEx((LPCTSTR)strTemp, &aBytes[0], &aBytes[1], &aBytes[2]) == FALSE)
				{
					strTemp = TRL("Cannot access the selected drive.");
					strTemp += _T("\r\n\r\n");
					strTemp += TRL("Make sure a writable medium is inserted.");
					MessageBox(strTemp, TRL("Stop"), MB_OK | MB_ICONWARNING);
					FreePasswords();
					return;
				}

				if(aBytes[2].QuadPart < 128)
				{
					MessageBox(TRL("Not enough free disk space!"), TRL("Stop"), MB_OK | MB_ICONWARNING);
					FreePasswords();
					return;
				}
			}

			FILE *fpTest;
			CString strTemp2 = strTemp;
			strTemp2 += _T("t2ub8rf6.tmp"); // Just a random filename, will work for directories and files
			fpTest = _tfopen((LPCTSTR)strTemp2, _T("wb"));
			if(fpTest == NULL)
			{
				strTemp = TRL("Cannot access the selected drive.");
				strTemp += _T("\r\n\r\n");
				strTemp += TRL("Make sure a writable medium is inserted.");
				MessageBox(strTemp, TRL("Stop"), MB_OK | MB_ICONWARNING);
				FreePasswords();
				return;
			}
			fclose(fpTest); Sleep(100);
			DeleteFile(strTemp2);
		}

		m_bKeyFile = TRUE;

		if(m_bKeyMethod == PWM_KEYMETHOD_OR) m_pEditPw.DeletePassword(m_lpKey);
		else m_lpKey2 = m_lpKey;

		m_lpKey = new TCHAR[strTemp.GetLength() + 1];
		ASSERT(m_lpKey != NULL); if(m_lpKey == NULL) { FreePasswords(); return; }
		_tcscpy(m_lpKey, (LPCTSTR)strTemp);
	}

	CleanUp();
	ASSERT(m_bOnce == FALSE); m_bOnce = TRUE;
	CDialog::OnOK();
}

void CPasswordDlg::OnCancel() 
{
	CleanUp();
	ASSERT(m_bOnce == FALSE); m_bOnce = TRUE;
	CDialog::OnCancel();
}

void CPasswordDlg::OnCheckStars() 
{
	UpdateData(TRUE);

	if(m_bStars == FALSE)
	{
		m_pEditPw.EnableSecureMode(FALSE);
		m_pEditPw.SetPasswordChar(0);
		m_pEditPw.SetFont(&m_fStyle, TRUE);
	}
	else
	{
		// TCHAR tchDot = (TCHAR)(_T('z') + 27);
		TCHAR tchDot = CPwSafeApp::GetPasswordCharacter();
		m_pEditPw.EnableSecureMode(CPwSafeDlg::m_bSecureEdits);
		m_pEditPw.SetPasswordChar(tchDot);
		m_pEditPw.SetFont(&m_fSymbol, TRUE);
	}

	m_tipSecClear.Activate(m_pEditPw.IsSecureModeEnabled());

	UpdateData(FALSE);
	m_pEditPw.RedrawWindow();
	m_pEditPw.SetFocus();
}

void CPasswordDlg::OnMakePasswordBtn() 
{
	CPwGeneratorDlg dlg;

	UpdateData(TRUE);

	dlg.m_bCanAccept = TRUE;
	dlg.m_bHidePw = m_bStars;

	if(dlg.DoModal() == IDOK)
	{
		if(dlg.m_lpPassword != NULL)
		{
			m_pEditPw.SetPassword(dlg.m_lpPassword);
			NewGUI_ShowQualityMeter(&m_cPassQuality, GetDlgItem(IDC_STATIC_PASSBITS), dlg.m_lpPassword);
			m_pEditPw.DeletePassword(dlg.m_lpPassword); dlg.m_lpPassword = NULL;
		}

		UpdateData(FALSE);
		OnCheckStars();
	}

	EnableClientWindows();
}

void CPasswordDlg::OnChangeEditPassword() 
{
	UpdateData(TRUE);
	LPTSTR lp = m_pEditPw.GetPassword();
	NewGUI_ShowQualityMeter(&m_cPassQuality, GetDlgItem(IDC_STATIC_PASSBITS), lp);
	m_pEditPw.DeletePassword(lp); lp = NULL;
	EnableClientWindows();
}

LRESULT CPasswordDlg::OnXHyperLinkClicked(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	if(wParam == IDC_HL_SELECTFILE)
	{
		CString strFile;
		DWORD dwFlags;
		CString strFilter;

		UpdateData(TRUE);

		strFilter = TRL("All files");
		strFilter += _T(" (*.*)|*.*||");

		dwFlags = OFN_LONGNAMES | OFN_EXTENSIONDIFFERENT;
		// OFN_EXPLORER = 0x00080000, OFN_ENABLESIZING = 0x00800000
		dwFlags |= 0x00080000 | 0x00800000;

		if(m_bLoadMode == TRUE)
			dwFlags |= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		else
			dwFlags |= OFN_HIDEREADONLY;

		CFileDialog dlg(m_bLoadMode, NULL, NULL, dwFlags, strFilter, this);

		if(dlg.DoModal() == IDOK)
		{
			strFile = dlg.GetPathName();

			COMBOBOXEXITEM cbi;
			ZeroMemory(&cbi, sizeof(COMBOBOXEXITEM));
			cbi.mask = CBEIF_IMAGE | CBEIF_TEXT | CBEIF_INDENT | CBEIF_SELECTEDIMAGE;
			cbi.iItem = m_cbDiskList.GetCount();
			cbi.pszText = (LPTSTR)(LPCTSTR)strFile;
			cbi.cchTextMax = (int)_tcslen(cbi.pszText);
			cbi.iImage = cbi.iSelectedImage = 28;
			cbi.iIndent = 0;
			int nx = m_cbDiskList.InsertItem(&cbi);

			// m_cbDiskList.SelectString(-1, (LPCTSTR)strFile);
			m_cbDiskList.SetCurSel(nx);
		}

		UpdateData(FALSE);
	}

	return 0;
}

void CPasswordDlg::OnSelChangeComboDiskList() 
{
	EnableClientWindows();
}

void CPasswordDlg::EnableClientWindows()
{
	UpdateData(TRUE);

	int nPwLength = m_pEditPw.GetWindowTextLength();
	int nComboSel = m_cbDiskList.GetCurSel();

	if(m_bKeyMethod == PWM_KEYMETHOD_OR)
	{
		if(nPwLength != 0)
		{
			m_hlSelFile.EnableWindow(FALSE);
			m_btStars.EnableWindow(TRUE);
			if(m_bConfirm == FALSE) m_btMakePw.EnableWindow(TRUE);
			m_pEditPw.EnableWindow(TRUE);
			m_cbDiskList.EnableWindow(FALSE);
			return;
		}
		else if(nComboSel != 0)
		{
			m_hlSelFile.EnableWindow(TRUE);
			m_btStars.EnableWindow(FALSE);
			if(m_bConfirm == FALSE) m_btMakePw.EnableWindow(FALSE);
			m_pEditPw.EnableWindow(FALSE);
			m_cbDiskList.EnableWindow(TRUE);
			return;
		}
	}

	m_btStars.EnableWindow(TRUE);
	if(m_bConfirm == FALSE) m_btMakePw.EnableWindow(TRUE);
	m_pEditPw.EnableWindow(TRUE);
	m_cbDiskList.EnableWindow(TRUE);

	if(nComboSel == 0) m_hlSelFile.EnableWindow(TRUE);
	else m_hlSelFile.EnableWindow(FALSE);
}

void CPasswordDlg::OnCheckKeymethodAnd() 
{
	EnableClientWindows();
}

BOOL CPasswordDlg::PreTranslateMessage(MSG* pMsg) 
{
	m_tipSecClear.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}

#pragma warning(pop)
