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
#include "PwSafeDlg.h"
#include "AddEntryDlg.h"

#include "IconPickerDlg.h"
#include "PwGeneratorExDlg.h"
#include "../KeePassLibCpp/Util/MemUtil.h"
#include "../KeePassLibCpp/Util/StrUtil.h"
#include "Util/WinUtil.h"
#include "NewGUI/NewGUICommon.h"
#include "../KeePassLibCpp/Util/TranslateEx.h"
#include "../KeePassLibCpp/Util/Base64.h"
#include "../KeePassLibCpp/Util/PwUtil.h"
#include "../KeePassLibCpp/PasswordGenerator/PasswordGenerator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(push)
// Disable warning: "cast truncates constant value"
#pragma warning(disable: 4310)

/////////////////////////////////////////////////////////////////////////////

CAddEntryDlg::CAddEntryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddEntryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddEntryDlg)
	m_bStars = TRUE;
	m_strTitle = _T("");
	m_strURL = _T(PWAE_STDURL_A);
	m_strUserName = _T("");
	m_strAttachment = _T("");
	m_bExpires = FALSE;
	//}}AFX_DATA_INIT

	m_pMgr = NULL;
	m_nGroupId = -1;
	m_nIconId = 0;
	m_bEditMode = FALSE;
	m_pParentIcons = NULL;
	m_strNotes = _T("");
	m_lpPassword = m_lpRepeatPw = NULL;
	m_dwDefaultExpire = 0;
}

void CAddEntryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddEntryDlg)
	DDX_Control(pDX, IDC_ENTRYHELP_BTN, m_btHelp);
	DDX_Control(pDX, IDC_SELDEFEXPIRES_BTN, m_btSelDefExpires);
	DDX_Control(pDX, IDC_SETDEFAULTEXPIRE_BTN, m_btSetToDefaultExpire);
	DDX_Control(pDX, IDC_COMBO_GROUPS, m_cbGroups);
	DDX_Control(pDX, IDC_PROGRESS_PASSQUALITY, m_cPassQuality);
	DDX_Control(pDX, IDC_REMOVEATTACH_BTN, m_btRemoveAttachment);
	DDX_Control(pDX, IDC_SAVEATTACH_BTN, m_btSaveAttachment);
	DDX_Control(pDX, IDC_SETATTACH_BTN, m_btSetAttachment);
	DDX_Control(pDX, IDC_EDIT_EXPIRE_TIME, m_editTime);
	DDX_Control(pDX, IDC_EDIT_EXPIRE_DATE, m_editDate);
	DDX_Control(pDX, IDC_CHECK_HIDEPW, m_btHidePw);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_RANDOMPW_BTN, m_btRandomPw);
	DDX_Control(pDX, IDC_PICKICON_BTN, m_btPickIcon);
	DDX_Control(pDX, IDC_EDIT_URL, m_pURL);
	DDX_Control(pDX, IDC_EDIT_REPEATPW, m_pRepeatPw);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_pEditPw);
	DDX_Check(pDX, IDC_CHECK_HIDEPW, m_bStars);
	DDX_Text(pDX, IDC_EDIT_TITLE, m_strTitle);
	DDX_Text(pDX, IDC_EDIT_URL, m_strURL);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_strUserName);
	DDX_Control(pDX, IDC_RE_NOTES, m_reNotes);
	DDX_Text(pDX, IDC_EDIT_ATTACHMENT, m_strAttachment);
	DDX_Check(pDX, IDC_CHECK_EXPIRES, m_bExpires);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAddEntryDlg, CDialog)
	//{{AFX_MSG_MAP(CAddEntryDlg)
	ON_COMMAND(ID_POPUP_AUTOTYPE, OnHelpAutoType)
	ON_COMMAND(ID_POPUP_URLFIELDFEATURES, OnHelpURLFieldFeatures)
	ON_COMMAND(ID_POPUP_OPENHELPFILE, OnHelpOpenFile)
	ON_BN_CLICKED(IDC_CHECK_HIDEPW, OnCheckHidePw)
	ON_BN_CLICKED(IDC_PICKICON_BTN, OnPickIconBtn)
	ON_BN_CLICKED(IDC_RANDOMPW_BTN, OnRandomPwBtn)
	ON_COMMAND(ID_RE_COPYALL, OnReCopyAll)
	ON_COMMAND(ID_RE_COPYSEL, OnReCopySel)
	ON_COMMAND(ID_RE_DELETE, OnReDelete)
	ON_COMMAND(ID_RE_PASTE, OnRePaste)
	ON_COMMAND(ID_RE_SELECTALL, OnReSelectAll)
	ON_COMMAND(ID_RE_CUT, OnReCut)
	ON_COMMAND(ID_RE_UNDO, OnReUndo)
	ON_BN_CLICKED(IDC_SETATTACH_BTN, OnSetAttachBtn)
	ON_BN_CLICKED(IDC_SAVEATTACH_BTN, OnSaveAttachBtn)
	ON_BN_CLICKED(IDC_REMOVEATTACH_BTN, OnRemoveAttachBtn)
	ON_EN_CHANGE(IDC_EDIT_PASSWORD, OnChangeEditPassword)
	ON_BN_CLICKED(IDC_CHECK_EXPIRES, OnCheckExpires)
	ON_BN_CLICKED(IDC_SETDEFAULTEXPIRE_BTN, OnSetDefaultExpireBtn)
	ON_WM_CTLCOLOR()
	ON_COMMAND(ID_EXPIRES_1WEEK, OnExpires1Week)
	ON_COMMAND(ID_EXPIRES_2WEEKS, OnExpires2Weeks)
	ON_COMMAND(ID_EXPIRES_1MONTH, OnExpires1Month)
	ON_COMMAND(ID_EXPIRES_3MONTHS, OnExpires3Months)
	ON_COMMAND(ID_EXPIRES_6MONTHS, OnExpires6Months)
	ON_COMMAND(ID_EXPIRES_12MONTHS, OnExpires12Months)
	ON_COMMAND(ID_EXPIRES_NOW, OnExpiresNow)
	ON_NOTIFY(EN_LINK, IDC_RE_NOTES, OnReNotesClickLink)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CAddEntryDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	ASSERT(m_pParentIcons != NULL); // Parent must set image list first!

	// Translate all windows
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	// The password dots font
	m_fStyle.CreateFont(-12, 0, 0, 0, 0, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, _T("Tahoma"));
	m_fSymbol.CreateFont(-13, 0, 0, 0, 0, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, CPwSafeApp::GetPasswordFont());

	if(m_bStars == FALSE)
	{
		// GetDlgItem(IDC_EDIT_PASSWORD)->SetFont(&m_fStyle, TRUE);
		m_pEditPw.SetFont(&m_fStyle, TRUE);
		// GetDlgItem(IDC_EDIT_REPEATPW)->SetFont(&m_fStyle, TRUE);
		m_pRepeatPw.SetFont(&m_fStyle, TRUE);

		m_pEditPw.EnableSecureMode(FALSE);
		m_pRepeatPw.EnableSecureMode(FALSE);
	}
	else
	{
		// GetDlgItem(IDC_EDIT_PASSWORD)->SetFont(&m_fSymbol, TRUE);
		m_pEditPw.SetFont(&m_fSymbol, TRUE);
		// GetDlgItem(IDC_EDIT_REPEATPW)->SetFont(&m_fSymbol, TRUE);
		m_pRepeatPw.SetFont(&m_fSymbol, TRUE);

		m_pEditPw.EnableSecureMode(CPwSafeDlg::m_bSecureEdits);
		m_pRepeatPw.EnableSecureMode(CPwSafeDlg::m_bSecureEdits);
	}

	// GetDlgItem(IDC_CHECK_HIDEPW)->SetFont(&m_fSymbol, TRUE);
	m_btHidePw.SetFont(&m_fSymbol, TRUE);

	NewGUI_ConfigQualityMeter(&m_cPassQuality);

	NewGUI_XPButton(m_btOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(m_btCancel, IDB_CANCEL, IDB_CANCEL);
	NewGUI_XPButton(m_btRandomPw, IDB_RANDOM_KEY, IDB_RANDOM_KEY);
	NewGUI_XPButton(m_btPickIcon, -1, -1);
	NewGUI_XPButton(m_btHidePw, -1, -1);
	NewGUI_XPButton(m_btSetAttachment, IDB_FILE, IDB_FILE, TRUE);
	NewGUI_XPButton(m_btSaveAttachment, IDB_DISK, IDB_DISK, TRUE);
	NewGUI_XPButton(m_btRemoveAttachment, IDB_TB_DELETEENTRY, IDB_TB_DELETEENTRY, TRUE);
	NewGUI_XPButton(m_btSetToDefaultExpire, IDB_TB_DEFAULTEXPIRE, IDB_TB_DEFAULTEXPIRE, TRUE);
	NewGUI_XPButton(m_btSelDefExpires, IDB_CLOCK, IDB_CLOCK, TRUE);
	NewGUI_XPButton(m_btHelp, IDB_HELP_SMALL_POPUP, IDB_HELP_SMALL_POPUP, TRUE);

	m_btHidePw.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 0, 255), TRUE);
	m_btHidePw.SetTooltipText(TRL("Hide passwords behind asterisks (***)."), TRUE);

	m_btRandomPw.SetTooltipText(TRL("Generate a random password..."));
	m_btSetAttachment.SetTooltipText(TRL("Open file and set as attachment..."));
	m_btSaveAttachment.SetTooltipText(TRL("Save attached file to disk..."));
	CString strRemoveTT = TRL("Remove the currently attached file"); strRemoveTT += _T(".");
	m_btRemoveAttachment.SetTooltipText(strRemoveTT);

	CString strTT = TRL("Change expiration time:");
	strTT.Remove(_T(':')); strTT += _T(".");
	m_btSelDefExpires.SetTooltipText(strTT);

	m_btSelDefExpires.SetMenu(IDR_EXPIRESMENU, this->m_hWnd, TRUE, NULL, CSize(16, 15));
	m_btHelp.SetMenu(IDR_ENTRYHELP_MENU, this->m_hWnd, TRUE, NULL, CSize(16, 16));

	m_btPickIcon.SetTooltipText(TRL("Choose an icon."), TRUE);
	if((m_nIconId >= 0) && (m_pParentIcons != NULL))
		m_btPickIcon.SetIcon(m_pParentIcons->ExtractIcon(m_nIconId));

	if(m_dwDefaultExpire != 0)
	{
		CString str, strTemp;
		str.Format(TRL("Click to expire the entry in %u days"), m_dwDefaultExpire);

		CTime t = CTime::GetCurrentTime();
		t += CTimeSpan((LONG)m_dwDefaultExpire, 0, 0, 0);

		strTemp.Format(_T(" (%04d-%02d-%02d)"), t.GetYear(), t.GetMonth(), t.GetDay());
		str += strTemp + _T(".");

		m_btSetToDefaultExpire.SetTooltipText(str);
	}

	m_cbGroups.SetImageList(m_pParentIcons);

	ASSERT(m_pMgr != NULL); // Must have been initialized by parent
	unsigned int i; PW_GROUP *p;
	COMBOBOXEXITEM cbi;
	for(i = 0; i < (unsigned int)m_pMgr->GetNumberOfGroups(); i++)
	{
		p = m_pMgr->GetGroup(i);
		ASSERT(p != NULL); if(p == NULL) continue;

		ZeroMemory(&cbi, sizeof(COMBOBOXEXITEM));
		cbi.mask = CBEIF_IMAGE | CBEIF_TEXT | CBEIF_INDENT | CBEIF_SELECTEDIMAGE;
		cbi.iItem = (int)i;
		cbi.pszText = (LPTSTR)p->pszGroupName;
		cbi.cchTextMax = (int)_tcslen(p->pszGroupName);
		cbi.iImage = cbi.iSelectedImage = (int)p->uImageId;
		cbi.iIndent = (int)p->usLevel;
		m_cbGroups.InsertItem(&cbi);
	}

	ASSERT(m_nGroupId != -1); // Must have been initialized by parent
	if(m_nGroupId != -1) m_cbGroups.SetCurSel(m_nGroupId);

	CString strGroupTest;
	m_cbGroups.GetLBText(m_cbGroups.GetCurSel(), strGroupTest);
	if(CPwUtil::IsAllowedStoreGroup((LPCTSTR)strGroupTest, PWS_SEARCHGROUP) == FALSE)
	{
		for(i = 0; i < (unsigned int)m_pMgr->GetNumberOfGroups(); i++)
		{
			if(CPwUtil::IsAllowedStoreGroup(m_pMgr->GetGroup(i)->pszGroupName, PWS_SEARCHGROUP))
			{
				m_cbGroups.SetCurSel(i);
				m_nGroupId = i;
				break;
			}
		}
	}

	RECT rectWindow, rectWork; // Save space by removing the banner, if needed
	this->GetWindowRect(&rectWindow);
	BOOL bSpi = SystemParametersInfo(SPI_GETWORKAREA, 0, &rectWork, 0);
	if(((rectWindow.bottom - rectWindow.top) <= (rectWork.bottom -
		rectWork.top)) || (bSpi == FALSE))
	{
		// Configure banner control
		NewGUI_ConfigSideBanner(&m_banner, this);
		m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_ENTRY_EDIT), KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	}

	if(m_bEditMode == FALSE)
	{
		m_banner.SetTitle(TRL("Add Entry"));
		m_banner.SetCaption(TRL("Create a new password entry."));
		SetWindowText(TRL("Add Entry"));
	}
	else
	{
		m_banner.SetTitle(TRL("Edit Entry"));
		m_banner.SetCaption(TRL("Modify existing password entry."));
		SetWindowText(TRL("Edit Entry"));
	}

	// 'z' + 27 is that black dot in Tahoma
	// TCHAR tchDot = (TCHAR)(_T('z') + 27);
	TCHAR tchDot = CPwSafeApp::GetPasswordCharacter();
	CString strStars; strStars += tchDot; strStars += tchDot; strStars += tchDot;
	GetDlgItem(IDC_CHECK_HIDEPW)->SetWindowText(strStars);

	// Configure link edit control
	// m_pURL.SetLinkOption(HEOL_AUTO);
	// m_pURL.SetUnderlineOption(HEOU_ALWAYS);
	// m_pURL.SetVisited(FALSE);
	// m_pURL.SetIEColours();
	// m_pURL.SetDblClkToJump(TRUE);
	// m_pURL.SetWindowText(m_strURL);

	COleDateTime oleMin = AMS_MIN_OLEDATETIME;
	COleDateTime oleMax(2999, 12, 31, 23, 59, 59);
	m_editDate.SetRange(oleMin, oleMax);
	m_editDate.SetDate((int)(unsigned int)m_tExpire.shYear,
		(int)(unsigned int)m_tExpire.btMonth, (int)(unsigned int)m_tExpire.btDay);
	m_editTime.SetRange(oleMin, oleMax);
	m_editTime.SetAMPM(true);
	m_editTime.Show24HourFormat(true);
	m_editTime.ShowSeconds(true);
	m_editTime.SetTime((int)(unsigned int)m_tExpire.btHour,
		(int)(unsigned int)m_tExpire.btMinute, (int)(unsigned int)m_tExpire.btSecond);

	PW_TIME tNever;
	m_pMgr->GetNeverExpireTime(&tNever);

	if(_pwtimecmp(&tNever, &m_tExpire) == 0) m_bExpires = FALSE;
	else m_bExpires = TRUE;

	// m_reNotes.LimitText(0);
	m_reNotes.SetEventMask(ENM_MOUSEEVENTS | ENM_LINK);
	m_reNotes.SendMessage(EM_AUTOURLDETECT, TRUE, 0);

#ifdef _UNICODE
	m_reNotes.SetRTF(m_strNotes, SF_TEXT | SF_UNICODE);
#else
	m_reNotes.SetRTF(m_strNotes, SF_TEXT);
#endif

	m_tipSecClear.Create(this, 0x40);
	m_tipSecClear.AddTool(&m_pEditPw, CPwSafeDlg::_GetSecureEditTipText(_T("Enter Password:")));
	m_tipSecClear.AddTool(&m_pRepeatPw, CPwSafeDlg::_GetSecureEditTipText(_T("Enter Password:")));
	m_tipSecClear.SetMaxTipWidth(630);
	m_tipSecClear.Activate(m_pEditPw.IsSecureModeEnabled());

	UpdateData(FALSE);

	// removed m_bStars = TRUE; -> Parent can decide to show the password or not
	OnCheckHidePw(); // Update GUI based on m_bStars flag

	if(m_bEditMode == FALSE) // Generate a pseudo-random password
	{
		/* CNewRandom *pRand = new CNewRandom();
		DWORD dwSize = 32;
		BYTE pbRandom[16], pbString[32];

		pRand->GetRandomBuffer(pbRandom, 16);
		VERIFY(CBase64Codec::Encode(pbRandom, 16, pbString, &dwSize));
		SAFE_DELETE(pRand);
		pbString[strlen((char *)pbString) - 3] = 0;

		TCHAR *pbStringT = _UTF8ToString((UTF8_BYTE *)(pbString + 1));
		m_pEditPw.SetPassword(pbStringT);
		m_pRepeatPw.SetPassword(pbStringT);

		mem_erase((BYTE *)pbStringT, _tcslen(pbStringT) * sizeof(TCHAR));
		SAFE_DELETE_ARRAY(pbStringT);

		mem_erase(pbRandom, 16); mem_erase(pbString, 32); */

		m_pEditPw.SetPassword(_T(""));
		m_pRepeatPw.SetPassword(_T(""));

		std::vector<TCHAR> strPassword;
		if(PwgGenerateEx(strPassword, &CPwSafeDlg::m_pgsAutoProfile,
			NULL) == PWGE_SUCCESS)
		{
			if(strPassword.size() > 0)
			{
				m_pEditPw.SetPassword(&strPassword[0]);
				m_pRepeatPw.SetPassword(&strPassword[0]);
			}
		}
		else { ASSERT(FALSE); }

		EraseTCharVector(strPassword);

		UpdateData(FALSE);
	}

	UpdateControlsStatus();

	if(m_bEditMode == TRUE)
	{
		if(m_lpPassword != NULL)
		{
			m_pEditPw.SetPassword(m_lpPassword);
			m_pEditPw.DeletePassword(m_lpPassword); m_lpPassword = NULL;
		}
		if(m_lpRepeatPw != NULL)
		{
			m_pRepeatPw.SetPassword(m_lpRepeatPw);
			m_pRepeatPw.DeletePassword(m_lpRepeatPw); m_lpRepeatPw = NULL;
		}
	}

	PerformMiniModeAdjustments();

	if(m_strTitle == PWS_TAN_ENTRY)
	{
		GetDlgItem(IDC_EDIT_TITLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_USERNAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_URL)->EnableWindow(FALSE);
		// GetDlgItem(IDC_RANDOMPW_BTN)->EnableWindow(FALSE);
		m_btRandomPw.EnableWindow(FALSE);
		// GetDlgItem(IDC_PICKICON_BTN)->EnableWindow(FALSE);
		m_btPickIcon.EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();
	}
	else
	{
		GetDlgItem(IDC_EDIT_TITLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_USERNAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_URL)->EnableWindow(TRUE);
		GetDlgItem(IDC_RANDOMPW_BTN)->EnableWindow(TRUE);
		GetDlgItem(IDC_PICKICON_BTN)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_TITLE)->SetFocus();
	}

	LPTSTR lpTemp = m_pEditPw.GetPassword();
	NewGUI_ShowQualityMeter(&m_cPassQuality, GetDlgItem(IDC_STATIC_PASSBITS), lpTemp);
	CSecureEditEx::DeletePassword(lpTemp); lpTemp = NULL;

	CString strTest;
	m_btRandomPw.GetWindowText(strTest);
	if(strTest.Find(_T("(&G)"), 0) >= 0) // Extend buttons if using a language that has special accelerator tips
	{
		RECT rcMover;

		m_pEditPw.GetWindowRect(&rcMover);
		ScreenToClient(&rcMover);
		rcMover.right -= 25;
		m_pEditPw.MoveWindow(&rcMover);

		m_pRepeatPw.GetWindowRect(&rcMover);
		ScreenToClient(&rcMover);
		rcMover.right -= 25;
		m_pRepeatPw.MoveWindow(&rcMover);

		m_btRandomPw.GetWindowRect(&rcMover);
		ScreenToClient(&rcMover);
		rcMover.left -= 25;
		m_btRandomPw.MoveWindow(&rcMover);

		m_btHidePw.GetWindowRect(&rcMover);
		ScreenToClient(&rcMover);
		rcMover.left -= 25;
		m_btHidePw.MoveWindow(&rcMover);
	}

	return FALSE; // Return TRUE unless you set the focus to a control
}

void CAddEntryDlg::CleanUp()
{
	m_reNotes.EmptyUndoBuffer();
	m_cbGroups.ResetContent();
	m_fStyle.DeleteObject();
	m_fSymbol.DeleteObject();
}

void CAddEntryDlg::OnOK() 
{
	UpdateData(TRUE);

	if(m_editDate.GetWindowTextLength() == 0) m_editDate.SetDate(2999, 12, 28);
	if(m_editTime.GetWindowTextLength() == 0) m_editTime.SetTime(23, 59, 59);

	if(m_lpPassword != NULL) CSecureEditEx::DeletePassword(m_lpPassword);
	m_lpPassword = m_pEditPw.GetPassword();
	if(m_lpRepeatPw != NULL) CSecureEditEx::DeletePassword(m_lpRepeatPw);
	m_lpRepeatPw = m_pRepeatPw.GetPassword();

	CString strGroupTest;
	m_cbGroups.GetLBText(m_cbGroups.GetCurSel(), strGroupTest);
	if(CPwUtil::IsAllowedStoreGroup((LPCTSTR)strGroupTest, PWS_SEARCHGROUP) == FALSE)
	{
		MessageBox(TRL("The group you selected cannot store entries. Please select a different group."),
			TRL("Stop"), MB_ICONWARNING | MB_OK);
		return;
	}

	if(m_bExpires == TRUE)
	{
		m_tExpire.shYear = (USHORT)m_editDate.GetYear();
		m_tExpire.btMonth = (BYTE)m_editDate.GetMonth();
		m_tExpire.btDay = (BYTE)m_editDate.GetDay();
		m_tExpire.btHour = (BYTE)m_editTime.GetHour();
		m_tExpire.btMinute = (BYTE)m_editTime.GetMinute();
		m_tExpire.btSecond = (BYTE)m_editTime.GetSecond();
	}
	else m_pMgr->GetNeverExpireTime(&m_tExpire);

	m_reNotes.GetWindowText(m_strNotes);

	m_nGroupId = m_cbGroups.GetCurSel();

	if(_tcscmp(m_lpPassword, m_lpRepeatPw) != 0)
	{
		CSecureEditEx::DeletePassword(m_lpPassword); m_lpPassword = NULL;
		CSecureEditEx::DeletePassword(m_lpRepeatPw); m_lpRepeatPw = NULL;
		MessageBox(TRL("Password and repeated password aren't identical!"),
			TRL("Stop"), MB_OK | MB_ICONWARNING);
		return;
	}

	// If the user hasn't changed the standard URL assume no URL
	if(m_strURL == PWAE_STDURL) m_strURL.Empty();

	UpdateData(FALSE);

	CleanUp();
	CDialog::OnOK();
}

void CAddEntryDlg::OnCancel() 
{
	CleanUp();
	CDialog::OnCancel();
}

void CAddEntryDlg::OnCheckHidePw() 
{
	UpdateData(TRUE);

	if(m_bStars == FALSE)
	{
		m_pEditPw.EnableSecureMode(FALSE);
		m_pRepeatPw.EnableSecureMode(FALSE);

		m_pEditPw.SetPasswordChar(0);
		m_pRepeatPw.SetPasswordChar(0);

		m_pEditPw.SetFont(&m_fStyle, TRUE);
		m_pRepeatPw.SetFont(&m_fStyle, TRUE);
	}
	else // m_bStars == TRUE
	{
		m_pEditPw.EnableSecureMode(CPwSafeDlg::m_bSecureEdits);
		m_pRepeatPw.EnableSecureMode(CPwSafeDlg::m_bSecureEdits);

		// TCHAR tchDot = (TCHAR)(_T('z') + 27);
		TCHAR tchDot = CPwSafeApp::GetPasswordCharacter();
		m_pEditPw.SetPasswordChar(tchDot);
		m_pRepeatPw.SetPasswordChar(tchDot);

		m_pEditPw.SetFont(&m_fSymbol, TRUE);
		m_pRepeatPw.SetFont(&m_fSymbol, TRUE);
	}

	m_tipSecClear.Activate(m_pEditPw.IsSecureModeEnabled());

	UpdateData(FALSE);
	m_pRepeatPw.SetFocus();
	m_pEditPw.SetFocus();
}

void CAddEntryDlg::OnPickIconBtn() 
{
	CIconPickerDlg dlg;

	dlg.m_pImageList = m_pParentIcons;
	dlg.m_uNumIcons = (UINT)m_pParentIcons->GetImageCount();
	dlg.m_nSelectedIcon = m_nIconId;

	if(dlg.DoModal() == IDOK)
	{
		m_nIconId = dlg.m_nSelectedIcon;

		if((m_nIconId >= 0) && (m_pParentIcons != NULL))
			m_btPickIcon.SetIcon(m_pParentIcons->ExtractIcon(m_nIconId));
	}
}

void CAddEntryDlg::OnRandomPwBtn() 
{
	CPwGeneratorExDlg dlg;

	UpdateData(TRUE);

	dlg.InitEx(1, m_bStars, FALSE);

	if(dlg.DoModal() == IDOK)
	{
		LPTSTR lpPassword = dlg.GetGeneratedPassword();
		m_pEditPw.SetPassword(lpPassword); // Allows NULL
		m_pRepeatPw.SetPassword(lpPassword);
		CSecureEditEx::DeletePassword(lpPassword); lpPassword = NULL;

		UpdateData(FALSE);

		LPTSTR lpCur = m_pEditPw.GetPassword();
		NewGUI_ShowQualityMeter(&m_cPassQuality, GetDlgItem(IDC_STATIC_PASSBITS),
			lpCur);
		CSecureEditEx::DeletePassword(lpCur); lpCur = NULL;
	}
}

void CAddEntryDlg::OnReCopyAll() 
{
	long lStart, lEnd;
	m_reNotes.GetSel(lStart, lEnd);
	m_reNotes.SetSel(0, -1);
	m_reNotes.Copy();
	m_reNotes.SetSel(lStart, lEnd);
}

void CAddEntryDlg::OnReCopySel() 
{
	m_reNotes.Copy();
}

void CAddEntryDlg::OnReDelete() 
{
	m_reNotes.Clear();
}

void CAddEntryDlg::OnRePaste() 
{
	m_reNotes.Paste();
}

void CAddEntryDlg::OnReSelectAll() 
{
	m_reNotes.SetSel(0, -1);
}

void CAddEntryDlg::OnReCut() 
{
	m_reNotes.Cut();
}

void CAddEntryDlg::OnReUndo() 
{
	m_reNotes.Undo();
}

BOOL CAddEntryDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	MSGFILTER *lpMsgFilter = (MSGFILTER *)lParam;

	if((wParam == IDC_RE_NOTES) && (lpMsgFilter->nmhdr.code == EN_MSGFILTER)
		&& (lpMsgFilter->msg == WM_RBUTTONDOWN))
	{
		POINT pt;
		GetCursorPos(&pt);

		m_popmenu.LoadMenu(IDR_RECTX_MENU);

		m_popmenu.SetMenuDrawMode(BCMENU_DRAWMODE_XP); // <<<!=>>> BCMENU_DRAWMODE_ORIGINAL
		m_popmenu.SetSelectDisableMode(FALSE);
		m_popmenu.SetXPBitmap3D(TRUE);
		m_popmenu.SetBitmapBackground(RGB(255, 0, 255));
		m_popmenu.SetIconSize(16, 16);

		m_popmenu.LoadToolbar(IDR_INFOICONS, IDB_INFOICONS_EX);

		BCMenu *pSub = NewGUI_GetBCMenu(m_popmenu.GetSubMenu(0));
		if(pSub == NULL) { ASSERT(FALSE); pSub = &m_popmenu; }
		CPwSafeDlg::_TranslateMenu(pSub, TRUE, NULL);
		
		pSub->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
		m_popmenu.DestroyMenu();
	}
	
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CAddEntryDlg::OnSetAttachBtn() 
{
	DWORD dwFlags;
	CString strFilter;

	UpdateData(TRUE);

	strFilter = TRL("All Files");
	strFilter += _T(" (*.*)|*.*||");

	dwFlags = OFN_LONGNAMES | OFN_EXTENSIONDIFFERENT;
	// OFN_EXPLORER = 0x00080000, OFN_ENABLESIZING = 0x00800000
	dwFlags |= 0x00080000 | 0x00800000;
	dwFlags |= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	CFileDialog dlg(TRUE, NULL, NULL, dwFlags, strFilter, this);

	if(dlg.DoModal() == IDOK)
	{
		int nRet;

		if(m_strAttachment == CString(PWS_NEW_ATTACHMENT)) nRet = IDYES;
		else if(m_strAttachment.IsEmpty() == FALSE)
		{
			CString strMsg = TRL("There already is a file attached with this entry.");
			strMsg += _T("\r\n\r\n");
			strMsg += TRL("Do you want to overwrite the current attachment?");
			nRet = MessageBox(strMsg, TRL("Overwrite?"), MB_YESNO | MB_ICONQUESTION);
		}
		else nRet = IDYES;

		if(nRet == IDYES)
			m_strAttachment = CString(PWS_NEW_ATTACHMENT) + dlg.GetPathName();
	}

	UpdateData(FALSE);
	UpdateControlsStatus();
}

void CAddEntryDlg::OnSaveAttachBtn() 
{
	UpdateData(TRUE);

	DWORD dwFlags;
	CString strSample;
	CString strFilter;

	if(m_dwEntryIndex == DWORD_MAX) return;

	PW_ENTRY *pEntry = m_pMgr->GetEntry(m_dwEntryIndex);
	ASSERT(pEntry != NULL); if(pEntry == NULL) return;

	if(_tcslen(pEntry->pszBinaryDesc) == 0)
	{
		MessageBox(TRL("There is no file attached with this entry."),
			PWM_PRODUCT_NAME_SHORT, MB_ICONINFORMATION | MB_OK);
		return;
	}

	strSample = pEntry->pszBinaryDesc;

	strFilter = TRL("All Files");
	strFilter += _T(" (*.*)|*.*||");

	dwFlags = OFN_LONGNAMES | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	dwFlags |= OFN_EXTENSIONDIFFERENT;
	// OFN_EXPLORER = 0x00080000, OFN_ENABLESIZING = 0x00800000
	dwFlags |= 0x00080000 | 0x00800000 | OFN_NOREADONLYRETURN;
	CFileDialog dlg(FALSE, NULL, strSample, dwFlags, strFilter, this);

	if(dlg.DoModal() == IDOK)
		CPwUtil::SaveBinaryData(pEntry, dlg.GetPathName());

	UpdateData(FALSE);
	UpdateControlsStatus();
}

void CAddEntryDlg::OnRemoveAttachBtn() 
{
	UpdateData(TRUE);
	m_strAttachment = CString(PWS_NEW_ATTACHMENT);
	UpdateData(FALSE);
	UpdateControlsStatus();
}

void CAddEntryDlg::UpdateControlsStatus()
{
	UpdateData(TRUE);

	if((m_strAttachment.GetLength() == 0) || (m_strAttachment == CString(PWS_NEW_ATTACHMENT)))
	{
		m_btSetAttachment.EnableWindow(TRUE);
		m_btSaveAttachment.EnableWindow(FALSE);
		m_btRemoveAttachment.EnableWindow(FALSE);
	}
	else
	{
		m_btSetAttachment.EnableWindow(TRUE);

		if(m_strAttachment.Left((int)_tcslen(PWS_NEW_ATTACHMENT)) != CString(PWS_NEW_ATTACHMENT))
			m_btSaveAttachment.EnableWindow(TRUE);
		else
			m_btSaveAttachment.EnableWindow(FALSE);

		m_btRemoveAttachment.EnableWindow(TRUE);
	}

	if(m_strTitle == PWS_TAN_ENTRY) m_btSetAttachment.EnableWindow(FALSE); // Unchangable

	if(m_bExpires == TRUE)
	{
		m_editDate.EnableWindow(TRUE);
		m_editTime.EnableWindow(TRUE);
	}
	else
	{
		m_editDate.EnableWindow(FALSE);
		m_editTime.EnableWindow(FALSE);
	}

	if(m_dwDefaultExpire == 0) m_btSetToDefaultExpire.EnableWindow(FALSE); // Unchangable
}

void CAddEntryDlg::OnChangeEditPassword() 
{
	UpdateData(TRUE);

	LPTSTR lp = m_pEditPw.GetPassword();
	NewGUI_ShowQualityMeter(&m_cPassQuality, GetDlgItem(IDC_STATIC_PASSBITS), lp);
	CSecureEditEx::DeletePassword(lp); lp = NULL;
}

void CAddEntryDlg::OnCheckExpires() 
{
	UpdateControlsStatus();
}

void CAddEntryDlg::OnSetDefaultExpireBtn() 
{
	if(m_dwDefaultExpire == 0) return;
	SetExpireDays(m_dwDefaultExpire, FALSE);
}

HBRUSH CAddEntryDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if((nCtlColor == CTLCOLOR_EDIT) && (pWnd != NULL))
		if((pWnd->GetDlgCtrlID() == IDC_EDIT_URL) && (pDC != NULL))
			pDC->SetTextColor(RGB(0, 0, 255));

	return hbr;
}

void CAddEntryDlg::SetExpireDays(DWORD dwDays, BOOL bSetTime)
{
	UpdateData(TRUE);

	m_bExpires = TRUE;

	CTime t = CTime::GetCurrentTime();
	t += CTimeSpan((LONG)dwDays, 0, 0, 0);

	m_editDate.SetDate(t); // Beware of daylight saving
	if(bSetTime == TRUE) m_editTime.SetTime(t);

	UpdateData(FALSE);
	UpdateControlsStatus();
}

void CAddEntryDlg::OnExpiresNow() 
{
	SetExpireDays(0, TRUE);
}

void CAddEntryDlg::OnExpires1Week() 
{
	SetExpireDays(7, FALSE);
}

void CAddEntryDlg::OnExpires2Weeks() 
{
	SetExpireDays(14, FALSE);
}

void CAddEntryDlg::OnExpires1Month() 
{
	SetExpireDays(30, FALSE);
}

void CAddEntryDlg::OnExpires3Months() 
{
	SetExpireDays(91, FALSE);
}

void CAddEntryDlg::OnExpires6Months() 
{
	SetExpireDays(182, FALSE);
}

void CAddEntryDlg::OnExpires12Months() 
{
	SetExpireDays(365, FALSE);
}

BOOL CAddEntryDlg::PreTranslateMessage(MSG* pMsg) 
{
	m_tipSecClear.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}

void CAddEntryDlg::OnReNotesClickLink(NMHDR* pNMHDR, LRESULT* pResult) 
{
	ENLINK *pEnLink = reinterpret_cast<ENLINK *>(pNMHDR);
	if(pEnLink == NULL) return;

	if((pEnLink->nmhdr.idFrom == IDC_RE_NOTES) && (pEnLink->msg == WM_LBUTTONDOWN))
	{
		CHARRANGE cr;
		m_reNotes.GetSel(cr); // Push current user selection

		m_reNotes.SetSel(pEnLink->chrg);
		CString strSelectedURL = m_reNotes.GetSelText();

		m_reNotes.SetSel(cr); // Pop current user selection

		if(strSelectedURL.GetLength() != 0)
			OpenUrlEx(strSelectedURL, this->m_hWnd);
	}

	*pResult = 0;
}

void CAddEntryDlg::OnHelpOpenFile()
{
	_OpenLocalFile(PWM_README_FILE, OLF_OPEN);
}

void CAddEntryDlg::OnHelpURLFieldFeatures()
{
	WU_OpenAppHelp(PWM_HELP_URLS);
}

void CAddEntryDlg::OnHelpAutoType()
{
	WU_OpenAppHelp(PWM_HELP_AUTOTYPE);
}

void CAddEntryDlg::PerformMiniModeAdjustments()
{
	if(CPwSafeDlg::m_bMiniMode == FALSE) return;

	NewGUI_DisableHideWnd(GetDlgItem(IDC_STATIC_ATTACH));
	NewGUI_DisableHideWnd(GetDlgItem(IDC_EDIT_ATTACHMENT));
	NewGUI_DisableHideWnd(&m_btSetAttachment);
	NewGUI_DisableHideWnd(&m_btSaveAttachment);
	NewGUI_DisableHideWnd(&m_btRemoveAttachment);
	NewGUI_DisableHideWnd(&m_btHelp);

	long lMoveY = -NewGUI_GetWndBasePosDiff(GetDlgItem(IDC_STATIC_DLGSEP), &m_btSetAttachment).cy;
	NewGUI_MoveWnd(GetDlgItem(IDC_STATIC_DLGSEP), 0, lMoveY, this);
	// NewGUI_MoveWnd(&m_btHelp, 0, lMoveY, this);
	NewGUI_MoveWnd(&m_btOK, 0, lMoveY, this);
	NewGUI_MoveWnd(&m_btCancel, 0, lMoveY, this);

	NewGUI_Resize(this, 0, lMoveY, NULL);
}

#pragma warning(pop)
