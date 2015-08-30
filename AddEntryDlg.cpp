/*
  Copyright (c) 2003/2004, Dominik Reichl <dominik.reichl@t-online.de>
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
#include "AddEntryDlg.h"

#include "IconPickerDlg.h"
#include "PwGeneratorDlg.h"
#include "Util/MemUtil.h"
#include "Util/StrUtil.h"
#include "NewGUI/NewGUICommon.h"
#include "NewGUI/TranslateEx.h"
#include "Util/base64.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(push)
// Cast truncates constant value
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
	//}}AFX_DATA_INIT

	m_pMgr = NULL;
	m_nGroupId = -1;
	m_nIconId = 0;
	m_bEditMode = FALSE;
	m_pParentIcons = NULL;
	m_strNotes = _T("");
	m_strPassword = _T("");
	m_strRepeatPw = _T("");
}

void CAddEntryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddEntryDlg)
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
	DDX_Control(pDX, IDC_COMBO_GROUPS, m_pGroups);
	DDX_Check(pDX, IDC_CHECK_HIDEPW, m_bStars);
	DDX_Text(pDX, IDC_EDIT_TITLE, m_strTitle);
	DDX_Text(pDX, IDC_EDIT_URL, m_strURL);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_strUserName);
	DDX_Control(pDX, IDC_RE_NOTES, m_reNotes);
	DDX_Text(pDX, IDC_EDIT_ATTACHMENT, m_strAttachment);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAddEntryDlg, CDialog)
	//{{AFX_MSG_MAP(CAddEntryDlg)
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
	GetDlgItem(IDC_EDIT_PASSWORD)->SetFont(&m_fStyle, TRUE);
	GetDlgItem(IDC_EDIT_REPEATPW)->SetFont(&m_fStyle, TRUE);
	GetDlgItem(IDC_CHECK_HIDEPW)->SetFont(&m_fStyle, TRUE);

	NewGUI_ConfigQualityMeter(&m_cPassQuality);

	// Make the buttons look cool
	NewGUI_Button(&m_btOK, IDB_OK, IDB_OK);
	NewGUI_Button(&m_btCancel, IDB_CANCEL, IDB_CANCEL);
	NewGUI_Button(&m_btRandomPw, -1, -1);
	NewGUI_Button(&m_btPickIcon, -1, -1);
	NewGUI_Button(&m_btHidePw, -1, -1);
	NewGUI_Button(&m_btSetAttachment, IDB_FILE, IDB_FILE, TRUE);
	NewGUI_Button(&m_btSaveAttachment, IDB_DISK, IDB_DISK, TRUE);
	NewGUI_Button(&m_btRemoveAttachment, IDB_TB_DELETEENTRY, IDB_TB_DELETEENTRY, TRUE);
	m_btHidePw.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 0, 255), TRUE);

	m_btRandomPw.SetTooltipText(TRL("Generate a random password..."));
	m_btSetAttachment.SetTooltipText(TRL("Open file and set as attachment..."));
	m_btSaveAttachment.SetTooltipText(TRL("Save attached file to disk..."));
	m_btRemoveAttachment.SetTooltipText(TRL("Remove the currently attached file"));

	// Set the imagelist for the group selector combo box
	m_pGroups.SetXImageList(m_pParentIcons);

	m_pGroups.SetBkGndColor(RGB(255,255,255));
	m_pGroups.SetTextColor(RGB(0,0,128));
	m_pGroups.SetHiLightTextColor(RGB(0, 0, 128));

	HDC hDC = ::GetDC(NULL);
	if(GetDeviceCaps(hDC, BITSPIXEL) <= 8)
	{
		m_pGroups.SetHiLightBkGndColor(RGB(192,192,192));
		m_pGroups.SetHiLightFrameColor(RGB(192,192,192));
	}
	else
	{
		m_pGroups.SetHiLightBkGndColor(RGB(230,230,255));
		m_pGroups.SetHiLightFrameColor(RGB(230,230,255));
	}
	::ReleaseDC(NULL, hDC);

	ASSERT(m_pMgr != NULL); // Must have been initialized by parent
	unsigned int i, j; PW_GROUP *p; USHORT uLevel;
	CString strAdd;
	for(i = 0; i < m_pMgr->GetNumberOfGroups(); i++) // Add groups to combo box
	{
		p = m_pMgr->GetGroup(i);
		ASSERT(p != NULL);

		strAdd.Empty();
		for(uLevel = 0; uLevel < p->usLevel; uLevel++) strAdd += _T("        ");

		for(j = 0; j < (unsigned int)_tcslen(p->pszGroupName); j++)
		{
			if(p->pszGroupName[j] != _T('&')) strAdd += p->pszGroupName[j];
			else strAdd += _T("&&");
		}

		m_pGroups.AddCTString(WZ_ROOT_INDEX, (BYTE)p->uImageId, strAdd);
	}

	ASSERT(m_nGroupId != -1); // Must have been initialized by parent
	m_pGroups.SetCurSel(m_nGroupId);

	// Configure banner control
	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_KEY),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);

	if(m_bEditMode == FALSE)
	{
		m_banner.SetTitle(TRL("Add Entry"));
		m_banner.SetCaption(TRL("Add a new password entry"));
		SetWindowText(TRL("Add Entry"));
	}
	else
	{
		m_banner.SetTitle(TRL("Edit Entry"));
		m_banner.SetCaption(TRL("Modify a password entry"));
		SetWindowText(TRL("Edit Entry"));
	}

	// 'z' + 27 is that black dot in Tahoma
	TCHAR tchDot = (TCHAR)(_T('z') + 27);
	CString strStars = _T("");
	strStars += tchDot; strStars += tchDot; strStars += tchDot;
	GetDlgItem(IDC_CHECK_HIDEPW)->SetWindowText(strStars);

	// Configure link edit control
	m_pURL.SetLinkOption(HEOL_AUTO);
	m_pURL.SetUnderlineOption(HEOU_ALWAYS);
	m_pURL.SetVisited(FALSE);
	m_pURL.SetIEColours();
	m_pURL.SetDblClkToJump(TRUE);

	m_pURL.SetWindowText(m_strURL);

	COleDateTime oleMin = AMS_MIN_OLEDATETIME;
	COleDateTime oleMax(2999, 12, 28, 23, 59, 59);
	m_editDate.SetRange(oleMin, oleMax);
	m_editTime.SetRange(oleMin, oleMax);
	m_editDate.SetDate((int)(unsigned int)m_tExpire.shYear,
		(int)(unsigned int)m_tExpire.btMonth, (int)(unsigned int)m_tExpire.btDay);
	m_editTime.SetAMPM(true);
	m_editTime.Show24HourFormat(true);
	m_editTime.ShowSeconds(true);
	m_editTime.SetTime((int)(unsigned int)m_tExpire.btHour,
		(int)(unsigned int)m_tExpire.btMinute, (int)(unsigned int)m_tExpire.btSecond);

	// m_reNotes.LimitText(0);
	m_reNotes.SetEventMask(ENM_MOUSEEVENTS);
	m_reNotes.SetRTF(m_strNotes, SF_TEXT);

	UpdateData(FALSE);

	// removed m_bStars = TRUE; -> Parent can decide to show the password or not
	OnCheckHidePw(); // Update GUI based on m_bStars flag

	if(m_bEditMode == FALSE) // Generate a pseudo-random password
	{
		CNewRandom *pRand = new CNewRandom();
		CBase64Codec base64;
		ASSERT(pRand != NULL);
		DWORD dwSize = 32;
		BYTE pbRandom[16], pbString[32];
		pRand->Initialize(); // Get system entropy
		pRand->GetRandomBuffer(pbRandom, 16);
		VERIFY(base64.Encode(pbRandom, 16, pbString, &dwSize));
		SAFE_DELETE(pRand);
		pbString[strlen((char *)pbString) - 3] = 0;
		EraseCString(&m_strPassword);
		m_strPassword = (char *)(pbString + 1);
		EraseCString(&m_strRepeatPw);
		m_strRepeatPw = (char *)(pbString + 1);
		UpdateData(FALSE);
		m_pEditPw.SetWindowText((LPCTSTR)m_strPassword);
		m_pRepeatPw.SetWindowText((LPCTSTR)m_strRepeatPw);
	}

	UpdateControlsStatus();

	m_pEditPw.SetWindowText((LPCTSTR)m_strRepeatPw);
	m_pRepeatPw.SetWindowText((LPCTSTR)m_strRepeatPw);

	if(m_strTitle == PWS_TAN_ENTRY)
	{
		GetDlgItem(IDC_EDIT_TITLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_USERNAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_URL)->EnableWindow(FALSE);
		GetDlgItem(IDC_RANDOMPW_BTN)->EnableWindow(FALSE);
		GetDlgItem(IDC_PICKICON_BTN)->EnableWindow(FALSE);
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

	NewGUI_ShowQualityMeter(&m_cPassQuality, GetDlgItem(IDC_STATIC_PASSBITS), (LPCTSTR)m_strPassword);

	return FALSE; // Return TRUE unless you set the focus to a control
}

void CAddEntryDlg::CleanUp()
{
	m_pGroups.ResetContent();
	m_fStyle.DeleteObject();
}

void CAddEntryDlg::OnOK() 
{
	UpdateData(TRUE);

	EraseCString(&m_strPassword);
	m_pEditPw.GetWindowText(m_strPassword);
	EraseCString(&m_strRepeatPw);
	m_pRepeatPw.GetWindowText(m_strRepeatPw);

	CString strGroupTest;
	m_pGroups.GetLBText(m_pGroups.GetCurSel(), strGroupTest);
	if(CPwManager::IsAllowedStoreGroup((LPCTSTR)strGroupTest, PWS_SEARCHGROUP) == FALSE)
	{
		MessageBox(TRL("The group you selected cannot store entries. Please select an other group."),
			TRL("Stop"), MB_ICONWARNING | MB_OK);
		return;
	}

	m_tExpire.shYear = (USHORT)m_editDate.GetYear();
	m_tExpire.btMonth = (BYTE)m_editDate.GetMonth();
	m_tExpire.btDay = (BYTE)m_editDate.GetDay();
	m_tExpire.btHour = (BYTE)m_editTime.GetHour();
	m_tExpire.btMinute = (BYTE)m_editTime.GetMinute();
	m_tExpire.btSecond = (BYTE)m_editTime.GetSecond();

	m_reNotes.GetWindowText(m_strNotes);

	m_nGroupId = m_pGroups.GetCurSel();

	if(m_strPassword != m_strRepeatPw)
	{
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
		m_pEditPw.SetPasswordChar(0);
		m_pRepeatPw.SetPasswordChar(0);
	}
	else
	{
		TCHAR tchDot = (TCHAR)(_T('z') + 27);
		m_pEditPw.SetPasswordChar(tchDot);
		m_pRepeatPw.SetPasswordChar(tchDot);
	}

	UpdateData(FALSE);
	m_pRepeatPw.RedrawWindow();
	m_pEditPw.RedrawWindow();
	m_pRepeatPw.SetFocus();
	m_pEditPw.SetFocus();
}

void CAddEntryDlg::OnPickIconBtn() 
{
	CIconPickerDlg dlg;

	dlg.m_pImageList = m_pParentIcons;
	dlg.m_uNumIcons = (UINT)m_pParentIcons->GetImageCount();

	if(dlg.DoModal() == IDOK)
	{
		m_nIconId = dlg.m_nSelectedIcon;
	}
}

void CAddEntryDlg::OnRandomPwBtn() 
{
	CPwGeneratorDlg dlg;

	UpdateData(TRUE);

	dlg.m_bCanAccept = TRUE;
	if(dlg.DoModal() == IDOK)
	{
		EraseCString(&m_strPassword);
		m_strPassword = dlg.m_strPassword;
		EraseCString(&m_strRepeatPw);
		m_strRepeatPw = dlg.m_strPassword;

		EraseCString(&dlg.m_strPassword);

		UpdateData(FALSE);

		m_pEditPw.SetWindowText((LPCTSTR)m_strPassword);
		m_pRepeatPw.SetWindowText((LPCTSTR)m_strRepeatPw);
		NewGUI_ShowQualityMeter(&m_cPassQuality, GetDlgItem(IDC_STATIC_PASSBITS), (LPCTSTR)m_strPassword);
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

		m_popmenu.LoadToolbar(IDR_INFOICONS);

		BCMenu *psub = (BCMenu *)m_popmenu.GetSubMenu(0);
		CPwSafeDlg::_TranslateMenu(psub);
		psub->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
		m_popmenu.DestroyMenu();
	}
	
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CAddEntryDlg::OnSetAttachBtn() 
{
	DWORD dwFlags;
	CString strFilter;

	UpdateData(TRUE);

	strFilter = TRL("All files");
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
			CString strMsg;
			
			strMsg = TRL("There already is a file attached with this entry.");
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
	PW_ENTRY *pEntry;

	if(m_dwEntryIndex == DWORD_MAX) return;

	pEntry = m_pMgr->GetEntry(m_dwEntryIndex);
	ASSERT(pEntry != NULL); if(pEntry == NULL) return;

	if(_tcslen(pEntry->pszBinaryDesc) == 0)
	{
		MessageBox(TRL("There is no file attached with this entry."), TRL("Password Safe"), MB_ICONINFORMATION);
		return;
	}

	strSample = pEntry->pszBinaryDesc;

	strFilter = TRL("All files");
	strFilter += _T(" (*.*)|*.*||");

	dwFlags = OFN_LONGNAMES | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	dwFlags |= OFN_EXTENSIONDIFFERENT;
	// OFN_EXPLORER = 0x00080000, OFN_ENABLESIZING = 0x00800000
	dwFlags |= 0x00080000 | 0x00800000 | OFN_NOREADONLYRETURN;
	CFileDialog dlg(FALSE, NULL, strSample, dwFlags, strFilter, this);

	if(dlg.DoModal() == IDOK) { m_pMgr->SaveBinaryData(pEntry, dlg.GetPathName()); }

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

	if(m_strTitle == CString(PWS_TAN_ENTRY)) m_btSetAttachment.EnableWindow(FALSE);
}

void CAddEntryDlg::OnChangeEditPassword() 
{
	UpdateData(TRUE);

	EraseCString(&m_strPassword);
	m_pEditPw.GetWindowText(m_strPassword);
	NewGUI_ShowQualityMeter(&m_cPassQuality, GetDlgItem(IDC_STATIC_PASSBITS), (LPCTSTR)m_strPassword);
}

#pragma warning(pop)
