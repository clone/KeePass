/*
  Copyright (c) 2003-2005, Dominik Reichl <dominik.reichl@t-online.de>
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer. 
  - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  - Neither the name of ReichlSoft nor the names of its contributors may be
    used to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
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
}

void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswordDlg)
	DDX_Control(pDX, IDC_HL_SELECTFILE, m_hlSelFile);
	DDX_Control(pDX, IDC_PROGRESS_PASSQUALITY, m_cPassQuality);
	DDX_Control(pDX, IDC_CHECK_STARS, m_btStars);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_MAKEPASSWORD_BTN, m_btMakePw);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_pEditPw);
	DDX_Control(pDX, IDC_COMBO_DISKLIST, m_cbDiskList);
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

	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	m_fStyle.CreateFont(-12, 0, 0, 0, 0, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, "Tahoma");
	GetDlgItem(IDC_EDIT_PASSWORD)->SetFont(&m_fStyle, TRUE);
	GetDlgItem(IDC_CHECK_STARS)->SetFont(&m_fStyle, TRUE);

	NewGUI_XPButton(&m_btOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(&m_btCancel, IDB_CANCEL, IDB_CANCEL);
	NewGUI_XPButton(&m_btMakePw, IDB_KEY_SMALL, IDB_KEY_SMALL);
	NewGUI_XPButton(&m_btStars, -1, -1);
	m_btStars.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 0, 255), TRUE);

	NewGUI_ConfigQualityMeter(&m_cPassQuality);

	m_ilIcons.Create(IDR_INFOICONS, 16, 1, RGB(255,0,255)); // Purple is transparent
	m_cbDiskList.SetXImageList(&m_ilIcons);

	m_cbDiskList.SetBkGndColor(RGB(255,255,255));
	m_cbDiskList.SetTextColor(RGB(0,0,128));
	m_cbDiskList.SetHiLightTextColor(RGB(0, 0, 128));

	HDC hDC = ::GetDC(NULL);
	if(GetDeviceCaps(hDC, BITSPIXEL) <= 8)
	{
		m_cbDiskList.SetHiLightBkGndColor(RGB(192,192,192));
		m_cbDiskList.SetHiLightFrameColor(RGB(192,192,192));
	}
	else
	{
		m_cbDiskList.SetHiLightBkGndColor(RGB(230,230,255));
		m_cbDiskList.SetHiLightFrameColor(RGB(230,230,255));
	}
	::ReleaseDC(NULL, hDC);

	m_cbDiskList.AddCTString(WZ_ROOT_INDEX, ICOIDX_NODRIVE, TRL("<no drive selected>"));
	int i; TCHAR c; UINT uStat; CString str; BYTE idxImage;
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
			m_cbDiskList.AddCTString(WZ_ROOT_INDEX, idxImage, str);
		}
	}
	m_cbDiskList.SetCurSel(0);

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

	lp = TRL("Either enter the password/passphrase or select the password disk's drive.");
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
			GetDlgItem(IDC_HL_SELECTFILE)->SetWindowText(lp);

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
			GetDlgItem(IDC_HL_SELECTFILE)->SetWindowText(lp);

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

			GetDlgItem(IDC_MAKEPASSWORD_BTN)->ShowWindow(SW_HIDE);
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
			GetDlgItem(IDC_COMBO_DISKLIST)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_MAKEPASSWORD_BTN)->EnableWindow(FALSE);
			GetDlgItem(IDC_HL_SELECTFILE)->ShowWindow(SW_HIDE);

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

			GetDlgItem(IDC_MAKEPASSWORD_BTN)->ShowWindow(SW_HIDE);
		}
	}

	// Alternative password asterisk character: 0xB7 (smaller dot)
	TCHAR tchDot = (TCHAR)(_T('z') + 27);
	// TCHAR tchDot = (TCHAR)0xB7;
	CString strStars = _T("");
	strStars += tchDot; strStars += tchDot; strStars += tchDot;
	GetDlgItem(IDC_CHECK_STARS)->SetWindowText(strStars);
	m_bStars = TRUE;
	OnCheckStars();

	UpdateData(FALSE);

	NewGUI_ShowQualityMeter(&m_cPassQuality, GetDlgItem(IDC_STATIC_PASSBITS), (LPCTSTR)m_strPassword);

	m_pEditPw.SetWindowText((LPCTSTR)m_strPassword);
	m_pEditPw.SetFocus();
	return FALSE; // Return TRUE unless you set the focus to a control
}

void CPasswordDlg::CleanUp()
{
	EraseCString(&m_strPassword);
	ASSERT(m_strPassword.GetLength() == 0);

	m_cbDiskList.ResetContent();
	m_ilIcons.DeleteImageList();
	m_fStyle.DeleteObject();
}

void CPasswordDlg::OnOK() 
{
	CString strTemp;
	ULARGE_INTEGER aBytes[3];

	UpdateData(TRUE);

	EraseCString(&m_strPassword);
	m_pEditPw.GetWindowText(m_strPassword);

	// Validate input
	if(m_bKeyMethod == PWM_KEYMETHOD_OR)
	{
		if(((m_strPassword.GetLength() == 0) ^ (m_cbDiskList.GetCurSel() == 0)) == 0)
		{
			MessageBox(TRL("EITHER enter a password/passphrase OR select a key disk drive."),
				TRL("Password Safe"), MB_OK | MB_ICONINFORMATION);
			return;
		}
	}
	else // m_bKeyMethod == PWM_KEYMETHOD_AND
	{
		if((m_strPassword.GetLength() == 0) || (m_cbDiskList.GetCurSel() == 0))
		{
			MessageBox(TRL("You've selected the AND key mode, so you must enter a password AND select a key-file."),
				TRL("Password Safe"), MB_OK | MB_ICONINFORMATION);
			return;
		}
	}

	if((m_bKeyMethod == PWM_KEYMETHOD_OR) && (m_cbDiskList.GetCurSel() == 0))
	{
		m_bKeyFile = FALSE;
		EraseCString(&m_strRealKey);
		m_strRealKey = m_strPassword;
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
					return;
				}

				if(aBytes[2].QuadPart < 128)
				{
					MessageBox(TRL("Not enough free disk space!"), TRL("Stop"), MB_OK | MB_ICONWARNING);
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
				return;
			}
			fclose(fpTest); Sleep(100);
			DeleteFile(strTemp2);
		}

		m_bKeyFile = TRUE;
		EraseCString(&m_strRealKey);
		m_strRealKey = strTemp;
		ASSERT(m_strRealKey.GetLength() != 0);
	}

	if(m_bKeyMethod == PWM_KEYMETHOD_AND)
	{
		EraseCString(&m_strRealKey2);
		m_strRealKey2 = m_strPassword;
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
		m_pEditPw.SetPasswordChar(0);
	else
	{
		TCHAR tchDot = (TCHAR)(_T('z') + 27);
		// TCHAR tchDot = (TCHAR)0xB7;
		m_pEditPw.SetPasswordChar(tchDot);
	}

	UpdateData(FALSE);
	m_pEditPw.RedrawWindow();
	m_pEditPw.SetFocus();
}

void CPasswordDlg::OnMakePasswordBtn() 
{
	CPwGeneratorDlg dlg;

	UpdateData(TRUE);

	dlg.m_bCanAccept = TRUE;
	if(dlg.DoModal() == IDOK)
	{
		EraseCString(&m_strPassword);
		m_strPassword = dlg.m_strPassword;
		EraseCString(&dlg.m_strPassword);

		m_bStars = FALSE;
		UpdateData(FALSE);
		OnCheckStars();

		NewGUI_ShowQualityMeter(&m_cPassQuality, GetDlgItem(IDC_STATIC_PASSBITS), (LPCTSTR)m_strPassword);
		m_pEditPw.SetWindowText((LPCTSTR)m_strPassword);
	}

	EnableClientWindows();
}

void CPasswordDlg::OnChangeEditPassword() 
{
	UpdateData(TRUE);
	EraseCString(&m_strPassword);
	m_pEditPw.GetWindowText(m_strPassword);
	NewGUI_ShowQualityMeter(&m_cPassQuality, GetDlgItem(IDC_STATIC_PASSBITS), (LPCTSTR)m_strPassword);
	EraseCString(&m_strPassword);
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

			m_cbDiskList.AddCTString(WZ_ROOT_INDEX, 28, (LPCTSTR)strFile);
			m_cbDiskList.SelectString(-1, (LPCTSTR)strFile);
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
			GetDlgItem(IDC_HL_SELECTFILE)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_STARS)->EnableWindow(TRUE);
			GetDlgItem(IDC_MAKEPASSWORD_BTN)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(TRUE);
			GetDlgItem(IDC_COMBO_DISKLIST)->EnableWindow(FALSE);
			return;
		}
		else if(nComboSel != 0)
		{
			GetDlgItem(IDC_HL_SELECTFILE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_STARS)->EnableWindow(FALSE);
			GetDlgItem(IDC_MAKEPASSWORD_BTN)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(FALSE);
			GetDlgItem(IDC_COMBO_DISKLIST)->EnableWindow(TRUE);
			return;
		}
	}

	GetDlgItem(IDC_CHECK_STARS)->EnableWindow(TRUE);
	GetDlgItem(IDC_MAKEPASSWORD_BTN)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_DISKLIST)->EnableWindow(TRUE);

	if(nComboSel == 0)
		GetDlgItem(IDC_HL_SELECTFILE)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_HL_SELECTFILE)->EnableWindow(FALSE);
}

void CPasswordDlg::OnCheckKeymethodAnd() 
{
	EnableClientWindows();
}

#pragma warning(pop)
