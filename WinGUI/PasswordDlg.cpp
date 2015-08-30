/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2009 Dominik Reichl <dominik.reichl@t-online.de>

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

#include "../KeePassLibCpp/Util/MemUtil.h"
#include "../KeePassLibCpp/Util/StrUtil.h"
#include "../KeePassLibCpp/Util/Base64.h"
#include "../KeePassLibCpp/Util/PwUtil.h"
#include "../KeePassLibCpp/Util/TranslateEx.h"
#include "Util/WinUtil.h"
#include "Util/PrivateConfigEx.h"

#include <boost/algorithm/string.hpp>

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
	DDX_Control(pDX, IDC_PROGRESS_PASSQUALITY, m_cPassQuality);
	DDX_Control(pDX, IDC_CHECK_STARS, m_btStars);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_PWDLG_HELP_BTN, m_btHelp);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_pEditPw);
	DDX_Check(pDX, IDC_CHECK_STARS, m_bStars);
	DDX_Check(pDX, IDC_CHECK_KEYMETHOD_AND, m_bKeyMethod);
	DDX_Control(pDX, IDC_BTN_BROWSE_KEYFILE, m_btBrowseKeyFile);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPasswordDlg, CDialog)
	//{{AFX_MSG_MAP(CPasswordDlg)
	ON_BN_CLICKED(IDC_CHECK_STARS, OnCheckStars)
	ON_BN_CLICKED(IDC_PWDLG_HELP_BTN, OnHelpBtn)
	ON_EN_CHANGE(IDC_EDIT_PASSWORD, OnChangeEditPassword)
	ON_CBN_SELCHANGE(IDC_COMBO_DISKLIST, OnSelChangeComboDiskList)
	ON_BN_CLICKED(IDC_CHECK_KEYMETHOD_AND, OnCheckKeymethodAnd)
	ON_BN_CLICKED(IDC_BTN_BROWSE_KEYFILE, OnBnClickedBrowseKeyFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CPasswordDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	ASSERT(m_bOnce == FALSE);

	m_bStarsMask = ((CPwSafeDlg::m_bDisableUnsafeAtStart == TRUE) ? FALSE : TRUE);

	if(m_bLoadMode == TRUE)
		ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_APPWINDOW); // Show in taskbar

	ASSERT(m_hWindowIcon != NULL);
	if(m_hWindowIcon != NULL)
	{
		SetIcon(m_hWindowIcon, TRUE);
		SetIcon(m_hWindowIcon, FALSE);
	}

	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	m_fStyle.CreateFont(NewGUI_Scale(-12, this), 0, 0, 0, 0, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, _T("Tahoma"));
	m_fSymbol.CreateFont(NewGUI_Scale(-13, this), 0, 0, 0, 0, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, CPwSafeApp::GetPasswordFont());

	NewGUI_XPButton(m_btOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(m_btCancel, IDB_CANCEL, IDB_CANCEL);
	NewGUI_XPButton(m_btHelp, IDB_HELP_SMALL, IDB_HELP_SMALL);
	NewGUI_XPButton(m_btStars, -1, -1);

	m_pEditPw.InitEx();

	m_btStars.SetFont(&m_fSymbol, TRUE);
	m_pEditPw.SetFont(&m_fSymbol, TRUE);

	LOGFONT lf;
	CFont* pDialogFont = GetDlgItem(IDC_STATIC_ENTERPW)->GetFont();
	pDialogFont->GetLogFont(&lf);
	m_fBold.CreateFont(lf.lfHeight, lf.lfWidth, lf.lfEscapement, lf.lfOrientation,
		FW_BOLD, lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut,
		lf.lfCharSet, lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, lf.lfFaceName);

	GetDlgItem(IDC_STATIC_ENTERPW)->SetFont(&m_fBold);
	GetDlgItem(IDC_STATIC_SELDISK)->SetFont(&m_fBold);

	m_btStars.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 0, 255), TRUE);
	m_btStars.SetTooltipText(TRL("Hide passwords behind asterisks (***)."), TRUE);

	NewGUI_ConfigQualityMeter(&m_cPassQuality);

	// m_ilIcons.Create(IDR_INFOICONS, 16, 1, RGB(255,0,255)); // Purple is transparent
	CPwSafeApp::CreateHiColorImageList(&m_ilIcons, IDB_INFOICONS_EX, 16);
	m_cbDiskList.SetImageList(&m_ilIcons);

	COMBOBOXEXITEM cbi;
	ZeroMemory(&cbi, sizeof(COMBOBOXEXITEM));
	cbi.mask = CBEIF_IMAGE | CBEIF_TEXT | CBEIF_INDENT | CBEIF_SELECTEDIMAGE;
	cbi.iItem = 0;
	cbi.pszText = (LPTSTR)TRL("(No key file selected)");
	cbi.cchTextMax = (int)_tcslen(cbi.pszText);
	cbi.iImage = cbi.iSelectedImage = ICOIDX_NODRIVE;
	cbi.iIndent = 0;
	m_cbDiskList.InsertItem(&cbi);

	int i;
	int j = 1;
	
	for(i = 0; i < 26; i++)
	{
		const TCHAR c = (TCHAR)(i + _T('A'));
		CString str = CString(c) + _T(":\\");

		const UINT uStat = GetDriveType((LPCTSTR)str);
		if(uStat != DRIVE_NO_ROOT_DIR)
		{
			BYTE idxImage = 0;
			if(uStat == DRIVE_REMOVABLE) idxImage = ICOIDX_REMOVABLE;
			if(uStat == DRIVE_FIXED) idxImage = ICOIDX_FIXED;
			if(uStat == DRIVE_REMOTE) idxImage = ICOIDX_REMOTE;
			if(uStat == DRIVE_CDROM) idxImage = ICOIDX_CDROM;
			if(uStat == DRIVE_RAMDISK) idxImage = ICOIDX_RAMDISK;

			str += PWS_DEFAULT_KEY_FILENAME;

			ZeroMemory(&cbi, sizeof(COMBOBOXEXITEM));
			cbi.mask = (CBEIF_IMAGE | CBEIF_TEXT | CBEIF_INDENT | CBEIF_SELECTEDIMAGE);
			cbi.iItem = j; j++;
			cbi.pszText = const_cast<LPTSTR>((LPCTSTR)str);
			cbi.cchTextMax = (int)_tcslen(cbi.pszText);
			cbi.iImage = cbi.iSelectedImage = (int)idxImage;
			cbi.iIndent = 0;
			m_cbDiskList.InsertItem(&cbi);
		}
	}

	QueryKeyProviders();
	for(size_t iProv = 0; iProv < m_vKeyProv.size(); ++iProv)
	{
		const KP_KEYPROV_INFO_CPP& keyProv = m_vKeyProv[iProv];

		ZeroMemory(&cbi, sizeof(COMBOBOXEXITEM));
		cbi.mask = (CBEIF_IMAGE | CBEIF_TEXT | CBEIF_INDENT | CBEIF_SELECTEDIMAGE);
		cbi.iItem = j; ++j;
		cbi.pszText = const_cast<TCHAR *>(keyProv.strName.c_str());
		cbi.cchTextMax = static_cast<int>(keyProv.strName.size());
		cbi.iImage = cbi.iSelectedImage = static_cast<int>(keyProv.dwImageIndex);
		cbi.iIndent = 0;
		m_cbDiskList.InsertItem(&cbi);
	}

	CString strStaticKeyFile = TRL("Key File");
	if(m_vKeyProv.size() > 0)
	{
		strStaticKeyFile += _T(" / ");
		strStaticKeyFile += TRL("Provider");
	}
	strStaticKeyFile += _T(":");
	GetDlgItem(IDC_STATIC_SELDISK)->SetWindowText(strStaticKeyFile);

	m_cbDiskList.SetCurSel(0);

	DWORD dw = m_cbDiskList.GetExtendedStyle();
	dw &= ~CBES_EX_NOEDITIMAGE;
	m_cbDiskList.SetExtendedStyle(0, dw);

	NewGUI_ConfigSideBanner(&m_banner, this);
	if(m_bLoadMode == FALSE)
		m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_KEY),
			KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	else
		m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_KEYHOLE),
			KCSB_ICON_LEFT | KCSB_ICON_VCENTER);

	m_btBrowseKeyFile.SetWindowText(_T(""));

	LPCTSTR lp;
	if(m_bConfirm == FALSE)
	{
		if(m_bLoadMode == FALSE)
		{
			NewGUI_XPButton(m_btBrowseKeyFile, IDB_TB_SAVE, IDB_TB_SAVE, TRUE);
			lp = TRL("Save key file manually to...");
			m_btBrowseKeyFile.SetTooltipText(lp);

			CString str = TRL("Set Composite Master Key");
			if(m_strDescriptiveName.GetLength() != 0)
				str = m_strDescriptiveName;
			m_banner.SetTitle(str);

			if(m_bChanging == FALSE)
			{
				SetWindowText(TRL("Create New Password Database"));
				m_banner.SetCaption(TRL("Specify the composite master key."));
			}
			else
			{
				str = TRL("Change Composite Master Key");
				if(m_strDescriptiveName.GetLength() != 0)
				{
					str += _T(" - ");
					str += m_strDescriptiveName;
				}
				SetWindowText(str);

				m_banner.SetCaption(TRL("Enter the new composite master key."));
			}
		}
		else // m_bConfirm == FALSE, m_bLoadMode == TRUE
		{
			NewGUI_XPButton(m_btBrowseKeyFile, IDB_TB_OPEN, IDB_TB_OPEN, TRUE);
			lp = TRL("Select key file manually...");
			m_btBrowseKeyFile.SetTooltipText(lp);

			CString str = TRL("Enter Composite Master Key");
			if(m_strDescriptiveName.GetLength() != 0)
				str = m_strDescriptiveName;
			m_banner.SetTitle(str);

			str = TRL("Open Database");
			if(m_strDescriptiveName.GetLength() != 0)
			{
				str += _T(" - ");
				str += m_strDescriptiveName;
			}
			SetWindowText(str);

			m_banner.SetCaption(TRL("Enter the composite master key."));

			m_cPassQuality.ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STATIC_PASSBITS)->ShowWindow(SW_HIDE);
		}
	}
	else // m_bConfirm == TRUE
	{
		if(m_bLoadMode == FALSE)
		{
			GetDlgItem(IDC_STATIC_SELDISK)->ShowWindow(SW_HIDE);
			m_cbDiskList.ShowWindow(SW_HIDE);
			m_btBrowseKeyFile.ShowWindow(SW_HIDE);

			if(m_bChanging == FALSE)
			{
				SetWindowText(TRL("Create New Password Database"));
				m_banner.SetCaption(TRL("Repeat the master password for the new database."));
			}
			else
			{
				SetWindowText(TRL("Repeat Master Password"));
				m_banner.SetCaption(TRL("Repeat the new master password for this database."));
			}
			m_banner.SetTitle(TRL("Repeat Master Password"));

			m_cPassQuality.ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STATIC_PASSBITS)->ShowWindow(SW_HIDE);

			GetDlgItem(IDC_CHECK_KEYMETHOD_AND)->ShowWindow(SW_HIDE);
		}
		else
		{
			// This combination isn't possible
			ASSERT(FALSE);

			CString str = TRL("Enter Composite Master Key");
			if(m_strDescriptiveName.GetLength() != 0)
				str = m_strDescriptiveName;
			m_banner.SetTitle(str);

			str = TRL("Open Database");
			if(m_strDescriptiveName.GetLength() != 0)
			{
				str += _T(" - ");
				str += m_strDescriptiveName;
			}
			SetWindowText(str);

			m_banner.SetCaption(TRL("Enter the composite master key."));
		}
	}

	m_tipSecClear.Create(this, 0x40);
	m_tipSecClear.AddTool(&m_pEditPw, CPwSafeDlg::_GetSecureEditTipText(_T("Enter Password:")));
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
		const int nSpecLen = static_cast<int>(_tcslen(m_lpPreSelectPath));
		CString strCBI;
		BOOL bFound = FALSE;

		for(i = 0; i < m_cbDiskList.GetCount(); ++i)
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
			cbi.mask = (CBEIF_IMAGE | CBEIF_TEXT | CBEIF_INDENT | CBEIF_SELECTEDIMAGE);
			cbi.iItem = m_cbDiskList.GetCount();
			cbi.pszText = const_cast<LPTSTR>(m_lpPreSelectPath);
			cbi.cchTextMax = (int)_tcslen(cbi.pszText);
			cbi.iImage = cbi.iSelectedImage = 28;
			cbi.iIndent = 0;
			int nx = m_cbDiskList.InsertItem(&cbi);

			m_cbDiskList.SetCurSel(nx);
			OnSelChangeComboDiskList();
		}

		m_bKeyMethod = TRUE;
		UpdateData(FALSE);
	}

	PerformMiniModeAdjustments();

	CPwSafeDlg::SetLastMasterKeyDlg(this->m_hWnd);
	EnableClientWindows();
	m_pEditPw.SetFocus();
	return FALSE; // Return TRUE unless you set the focus to a control
}

void CPasswordDlg::CleanUp()
{
	m_cbDiskList.ResetContent();
	m_ilIcons.DeleteImageList();
	m_fStyle.DeleteObject();
	m_fSymbol.DeleteObject();
	m_fBold.DeleteObject();

	_CallPlugins(KPM_KEYPROV_FINALIZE, 0, 0);
}

void CPasswordDlg::PerformMiniModeAdjustments()
{
	if(CPwSafeDlg::m_bMiniMode == FALSE) return;

	NewGUI_DisableHideWnd(GetDlgItem(IDC_CHECK_KEYMETHOD_AND));
	NewGUI_DisableHideWnd(GetDlgItem(IDC_STATIC_SELDISK));
	NewGUI_DisableHideWnd(&m_cbDiskList);
	NewGUI_DisableHideWnd(&m_btBrowseKeyFile);
	NewGUI_DisableHideWnd(&m_btHelp);

	RECT rectTop, rectBottom;
	GetDlgItem(IDC_CHECK_KEYMETHOD_AND)->GetWindowRect(&rectTop);
	m_btOK.GetWindowRect(&rectBottom);

	long lMoveY = -(rectBottom.top - rectTop.top);
	// NewGUI_MoveWnd(&m_btHelp, 0, lMoveY, this);
	NewGUI_MoveWnd(&m_btOK, 0, lMoveY, this);
	NewGUI_MoveWnd(&m_btCancel, 0, lMoveY, this);

	NewGUI_Resize(this, 0, lMoveY, NULL);
}

void CPasswordDlg::FreePasswords()
{
	ASSERT(m_lpKey != NULL);

	if(m_lpKey != NULL)
	{
		CSecureEditEx::DeletePassword(m_lpKey);
		m_lpKey = NULL;
	}

	if(m_lpKey2 != NULL)
	{
		CSecureEditEx::DeletePassword(m_lpKey2);
		m_lpKey2 = NULL;
	}
}

void CPasswordDlg::OnOK() 
{
	UpdateData(TRUE);

	ASSERT((m_lpKey == NULL) && (m_lpKey2 == NULL));
	m_lpKey = m_pEditPw.GetPassword();

	if(m_bConfirm == FALSE)
	{
		// Validate input
		if(m_bKeyMethod == PWM_KEYMETHOD_OR)
		{
			if(!((_tcslen(m_lpKey) == 0) ^ (m_cbDiskList.GetCurSel() == 0)))
			{
				MessageBox(TRL("EITHER enter a master password OR select a key file."),
					PWM_PRODUCT_NAME_SHORT, MB_OK | MB_ICONINFORMATION);
				FreePasswords();
				return;
			}
		}
		else // m_bKeyMethod == PWM_KEYMETHOD_AND
		{
			if((_tcslen(m_lpKey) == 0) || (m_cbDiskList.GetCurSel() == 0))
			{
				MessageBox(TRL("You've selected the AND key mode, so you must enter a password AND select a key file."),
					PWM_PRODUCT_NAME_SHORT, MB_OK | MB_ICONINFORMATION);
				FreePasswords();
				return;
			}
		}
	}

	// Validate master password properties
	if((m_lpKey[0] != 0) && (m_bConfirm == FALSE) && (m_bLoadMode == FALSE))
	{
		CPrivateConfigEx cfg(FALSE);
		std::basic_string<TCHAR> strMinLen = cfg.GetSafe(PWMKEY_MASTERPW_MINLEN);
		std::basic_string<TCHAR> strMinQuality = cfg.GetSafe(PWMKEY_MASTERPW_MINQUALITY);
		CString strValMsg;

		if(strMinLen.size() > 0)
		{
			const long lMinLen = _ttol(strMinLen.c_str());
			if(static_cast<long>(_tcslen(m_lpKey)) < lMinLen)
			{
				strValMsg.Format(TRL("The master password must be at least %u characters long!"),
					static_cast<unsigned int>(lMinLen));
				MessageBox(strValMsg, PWM_PRODUCT_NAME_SHORT, MB_OK | MB_ICONWARNING);
				FreePasswords();
				return;
			}
		}

		if(strMinQuality.size() > 0)
		{
			const long lMinQuality = _ttol(strMinQuality.c_str());
			const DWORD lCurQuality = CPwUtil::EstimatePasswordBits(m_lpKey);
			if(static_cast<long>(lCurQuality) < lMinQuality)
			{
				strValMsg.Format(TRL("The estimated quality of the master password must be at least %u bits!"),
					static_cast<unsigned int>(lMinQuality));
				MessageBox(strValMsg, PWM_PRODUCT_NAME_SHORT, MB_OK | MB_ICONWARNING);
				FreePasswords();
				return;
			}
		}

		LPTSTR lpValMsg = NULL;
		_CallPlugins(KPM_VALIDATE_MASTERPASSWORD, (LPARAM)m_lpKey, (LPARAM)&lpValMsg);
		if((lpValMsg != NULL) && (lpValMsg[0] != 0))
		{
			MessageBox(lpValMsg, PWM_PRODUCT_NAME_SHORT, MB_OK | MB_ICONWARNING);
			FreePasswords();
			return;
		}
	}

	if((m_bKeyMethod == PWM_KEYMETHOD_OR) && (m_cbDiskList.GetCurSel() == 0))
	{
		m_bKeyFile = FALSE;
	}
	else // Key file provided
	{
		CString strTemp;
		m_cbDiskList.GetLBText(m_cbDiskList.GetCurSel(), strTemp);

		const BOOL bKeyProv = IsKeyProvider(strTemp);

		if((m_bLoadMode == FALSE) && (bKeyProv == FALSE))
		{
			if(strTemp.GetAt(strTemp.GetLength() - 1) == _T('\\'))
			{
				ULARGE_INTEGER aBytes[3];
				if(GetDiskFreeSpaceEx((LPCTSTR)strTemp, &aBytes[0], &aBytes[1], &aBytes[2]) == FALSE)
				{
					strTemp = TRL("Cannot access the selected drive.");
					strTemp += _T("\r\n\r\n");
					strTemp += TRL("Make sure a writable medium is inserted.");
					MessageBox(strTemp, PWM_PRODUCT_NAME_SHORT, MB_OK | MB_ICONWARNING);
					FreePasswords();
					return;
				}

				if(aBytes[2].QuadPart < 128)
				{
					MessageBox(TRL("Not enough free disk space!"), PWM_PRODUCT_NAME_SHORT,
						MB_OK | MB_ICONWARNING);
					FreePasswords();
					return;
				}
			}

			FILE *fpTest = NULL;
			CString strTemp2 = strTemp;
			strTemp2 += _T("t2ub8rf6.tmp"); // Just a random filename, will work for directories and files
			_tfopen_s(&fpTest, (LPCTSTR)strTemp2, _T("wb"));
			if(fpTest == NULL)
			{
				strTemp = TRL("Cannot access the selected drive.");
				strTemp += _T("\r\n\r\n");
				strTemp += TRL("Make sure a writable medium is inserted.");
				MessageBox(strTemp, PWM_PRODUCT_NAME_SHORT, MB_OK | MB_ICONWARNING);
				FreePasswords();
				return;
			}
			fclose(fpTest); Sleep(100);
			DeleteFile(strTemp2);
		}
		else if(bKeyProv == FALSE) // Load the key file
		{
			const DWORD dwDummyErrorCode = 0x6F4B1C80;
			::SetLastError(dwDummyErrorCode);
			HANDLE hKeyFile = CreateFile(strTemp, GENERIC_READ, FILE_SHARE_READ,
				NULL, OPEN_EXISTING, 0, NULL);
			if(hKeyFile != INVALID_HANDLE_VALUE) CloseHandle(hKeyFile);
			else // hKeyFile == INVALID_HANDLE_VALUE
			{
				DWORD dwKeyFileError = ::GetLastError();
				if(dwKeyFileError != dwDummyErrorCode)
				{
					std::basic_string<TCHAR> strKeyError = strTemp;
					strKeyError += _T("\r\n\r\n");
					strKeyError += CPwUtil::FormatSystemMessage(dwKeyFileError);

					MessageBox(strKeyError.c_str(), PWM_PRODUCT_NAME_SHORT, MB_OK | MB_ICONWARNING);
					FreePasswords();
					return;
				}
			}
		}

		// Get data from external key provider
		if(bKeyProv == TRUE)
		{
			strTemp = GetKeyFromProvider(strTemp).c_str();
			if(strTemp.GetLength() == 0)
			{
				FreePasswords();
				return;
			}
		}

		m_bKeyFile = TRUE;

		if(m_bKeyMethod == PWM_KEYMETHOD_OR) m_pEditPw.DeletePassword(m_lpKey);
		else m_lpKey2 = m_lpKey;

		const size_t sizeKeyBuffer = strTemp.GetLength() + 1;
		m_lpKey = new TCHAR[sizeKeyBuffer];
		ASSERT(m_lpKey != NULL); if(m_lpKey == NULL) { FreePasswords(); return; }
		_tcscpy_s(m_lpKey, sizeKeyBuffer, (LPCTSTR)strTemp);
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
	BOOL bPrevState = m_bStars;

	UpdateData(TRUE);

	if((bPrevState == TRUE) && (m_bStars == FALSE) &&
		(CPwSafeDlg::IsUnsafeAllowed(this->m_hWnd) == FALSE))
	{
		m_bStars = TRUE;
		UpdateData(FALSE);
		return;
	}

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

void CPasswordDlg::OnHelpBtn() 
{
	WU_OpenAppHelp(PWM_HELP_KEYS);
}

void CPasswordDlg::OnChangeEditPassword() 
{
	UpdateData(TRUE);
	LPTSTR lp = m_pEditPw.GetPassword();
	NewGUI_ShowQualityMeter(&m_cPassQuality, GetDlgItem(IDC_STATIC_PASSBITS), lp);
	m_pEditPw.DeletePassword(lp); lp = NULL;
	EnableClientWindows();
}

void CPasswordDlg::OnSelChangeComboDiskList() 
{
	EnableClientWindows();
}

void CPasswordDlg::EnableClientWindows()
{
	UpdateData(TRUE);

	const int nPwLength = m_pEditPw.GetWindowTextLength();
	const int nComboSel = m_cbDiskList.GetCurSel();

	if(m_bKeyMethod == PWM_KEYMETHOD_OR)
	{
		if(nPwLength != 0)
		{
			m_btBrowseKeyFile.EnableWindow(FALSE);
			m_btStars.EnableWindow(TRUE & m_bStarsMask);
			m_pEditPw.EnableWindow(TRUE);
			m_cbDiskList.EnableWindow(FALSE);
			return;
		}
		else if(nComboSel != 0)
		{
			m_btBrowseKeyFile.EnableWindow(TRUE);
			m_btStars.EnableWindow(FALSE);
			m_pEditPw.EnableWindow(FALSE);
			m_cbDiskList.EnableWindow(TRUE);
			return;
		}
	}

	m_btBrowseKeyFile.EnableWindow(TRUE);
	m_btStars.EnableWindow(TRUE & m_bStarsMask);
	m_pEditPw.EnableWindow(TRUE);
	m_cbDiskList.EnableWindow(TRUE);

	// if(nComboSel == 0) m_btBrowseKeyFile.EnableWindow(TRUE);
	// else m_btBrowseKeyFile.EnableWindow(FALSE);
}

void CPasswordDlg::UpdateAndCheckBox()
{
	EnableClientWindows();
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

void CPasswordDlg::OnBnClickedBrowseKeyFile()
{
	UpdateData(TRUE);

	CString strFilter = TRL("All Files");
	strFilter += _T(" (*.*)|*.*||");

	DWORD dwFlags = (OFN_LONGNAMES | OFN_EXTENSIONDIFFERENT | OFN_EXPLORER |
		OFN_ENABLESIZING);

	if(m_bLoadMode == TRUE)
		dwFlags |= (OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY);
	else
		dwFlags |= OFN_HIDEREADONLY;

	std::basic_string<TCHAR> strDir = WU_GetCurrentDirectory();

	CFileDialog dlg(m_bLoadMode, NULL, NULL, dwFlags, strFilter, this);
	if(dlg.DoModal() == IDOK)
	{
		CString strFile = dlg.GetPathName();

		COMBOBOXEXITEM cbi;
		ZeroMemory(&cbi, sizeof(COMBOBOXEXITEM));
		cbi.mask = CBEIF_IMAGE | CBEIF_TEXT | CBEIF_INDENT | CBEIF_SELECTEDIMAGE;
		cbi.iItem = m_cbDiskList.GetCount();
		cbi.pszText = (LPTSTR)(LPCTSTR)strFile;
		cbi.cchTextMax = static_cast<int>(_tcslen(cbi.pszText));
		cbi.iImage = cbi.iSelectedImage = 28;
		cbi.iIndent = 0;
		const int nx = m_cbDiskList.InsertItem(&cbi);

		// m_cbDiskList.SelectString(-1, (LPCTSTR)strFile);
		m_cbDiskList.SetCurSel(nx);
	}

	WU_SetCurrentDirectory(strDir.c_str());
	UpdateData(FALSE);
}

void CPasswordDlg::QueryKeyProviders()
{
	m_vKeyProv.clear();

	std::vector<KP_PLUGIN_INSTANCE>& v = CPluginManager::Instance().m_plugins;

	for(size_t i = 0; i < v.size(); ++i)
	{
		KP_PLUGIN_INSTANCE& p = v[i];

		// if(p.bEnabled == FALSE) continue;
		if(p.hinstDLL == NULL) continue;
		if(p.pInterface == NULL) { ASSERT(FALSE); continue; }

		KP_KEYPROV_INFO cInfo;
		ZeroMemory(&cInfo, sizeof(KP_KEYPROV_INFO));
		p.pInterface->OnMessage(KPM_KEYPROV_QUERY_INFO_FIRST, NULL, (LPARAM)&cInfo);

		if(cInfo.lpName != NULL)
		{
			AddKeyProvider(cInfo);

			while(true)
			{
				ZeroMemory(&cInfo, sizeof(KP_KEYPROV_INFO));
				p.pInterface->OnMessage(KPM_KEYPROV_QUERY_INFO_NEXT, NULL, (LPARAM)&cInfo);

				if(cInfo.lpName != NULL) AddKeyProvider(cInfo);
				else break;
			}
		}
	}
}

void CPasswordDlg::AddKeyProvider(const KP_KEYPROV_INFO& keyProvInfo)
{
	if((keyProvInfo.lpName == NULL) || (keyProvInfo.lpName[0] == 0)) return;

	// std::basic_string<TCHAR> strSpec = lpKeyProvInfo;
	// std::basic_string<TCHAR> strSplit = strSpec.substr(0, 1);
	// std::basic_string<TCHAR> strDef = strSpec.substr(1);
	// std::vector<std::basic_string<TCHAR> > vSpec;
	// boost::algorithm::split(vSpec, strDef,
	//	boost::algorithm::is_any_of(strSplit));
	// if(vSpec.size() == 0) return;

	m_vKeyProv.push_back(CPasswordDlg::KeyProvCToS(keyProvInfo));
}

BOOL CPasswordDlg::IsKeyProvider(LPCTSTR lpDisplayName)
{
	for(size_t i = 0; i < m_vKeyProv.size(); ++i)
	{
		if(m_vKeyProv[i].strName == lpDisplayName)
			return TRUE;
	}

	return FALSE;
}

std::basic_string<TCHAR> CPasswordDlg::GetKeyFromProvider(LPCTSTR lpDisplayName)
{
	std::basic_string<TCHAR> str;

	if((lpDisplayName == NULL) || (lpDisplayName[0] == 0)) return str;

	KP_KEYPROV_KEY kpKey;
	ZeroMemory(&kpKey, sizeof(KP_KEYPROV_KEY));

	_CallPlugins(KPM_KEYPROV_QUERY_KEY, (LPARAM)lpDisplayName, (LPARAM)&kpKey);
	if((kpKey.lpData == NULL) || (kpKey.dwDataSize == 0))
	{
		ZeroMemory(&kpKey, sizeof(KP_KEYPROV_KEY));

		KP_KEYPROV_CONTEXT ctx;
		ZeroMemory(&ctx, sizeof(KP_KEYPROV_CONTEXT));

		ctx.dwSize = sizeof(KP_KEYPROV_CONTEXT);
		ctx.lpProviderName = lpDisplayName;
		ctx.bCreatingNewKey = ((m_bLoadMode == FALSE) ? TRUE : FALSE);
		ctx.bConfirming = m_bConfirm;
		ctx.bChanging = m_bChanging;
		ctx.lpDescriptiveName = m_strDescriptiveName;

		_CallPlugins(KPM_KEYPROV_QUERY_KEY_EX, (LPARAM)&ctx, (LPARAM)&kpKey);
	}

	if((kpKey.lpData == NULL) || (kpKey.dwDataSize == 0))
	{
		CString strMsg = TRL("The key provider plugin did not supply a valid key");
		strMsg += _T(".");
		MessageBox(strMsg, PWM_PRODUCT_NAME_SHORT, MB_ICONWARNING | MB_OK);
		return str;
	}

	const DWORD dwBufSizeM = (kpKey.dwDataSize * 3) + 12;
	BYTE* pBase = new BYTE[dwBufSizeM];
	ZeroMemory(pBase, dwBufSizeM);
	DWORD dwBufSize = dwBufSizeM;
	VERIFY(CBase64Codec::Encode((const BYTE *)kpKey.lpData, kpKey.dwDataSize,
		pBase, &dwBufSize));

	str = _T(CB64_PROTOCOL);

#ifndef _UNICODE
	str += (const char *)pBase;
#else // Unicode
	const WCHAR *pUni = _StringToUnicode((const char *)pBase);
	str += pUni;
	SAFE_DELETE_ARRAY(pUni);
#endif

	mem_erase(pBase, dwBufSizeM);
	SAFE_DELETE_ARRAY(pBase);
	return str;
}

KP_KEYPROV_INFO_CPP CPasswordDlg::KeyProvCToS(const KP_KEYPROV_INFO& c)
{
	KP_KEYPROV_INFO_CPP s;

	s.dwFlags = c.dwFlags;
	s.strName = c.lpName;
	s.dwImageIndex = c.dwImageIndex;

	return s;
}

#pragma warning(pop)
