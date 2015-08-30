/*
  Copyright (c) 2003, Dominik Reichl <dominik.reichl@t-online.de>
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
#include "AddEntryDlg.h"

#include "IconPickerDlg.h"
#include "PwGeneratorDlg.h"
#include "Util/MemUtil.h"
#include "NewGUI/TranslateEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CAddEntryDlg::CAddEntryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddEntryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddEntryDlg)
	m_bStars = TRUE;
	m_idGroup = -1;
	m_strNotes = _T("");
	m_strPassword = _T("");
	m_strRepeatPw = _T("");
	m_strTitle = _T("");
	m_strURL = _T(PWAE_STDURL_A);
	m_strUserName = _T("");
	//}}AFX_DATA_INIT

	m_pMgr = NULL;
	m_nGroupId = -1;
	m_nIconId = 0;
	m_bEditMode = FALSE;
}

void CAddEntryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddEntryDlg)
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
	DDX_CBIndex(pDX, IDC_COMBO_GROUPS, m_idGroup);
	DDX_Text(pDX, IDC_EDIT_NOTES, m_strNotes);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPassword);
	DDX_Text(pDX, IDC_EDIT_REPEATPW, m_strRepeatPw);
	DDX_Text(pDX, IDC_EDIT_TITLE, m_strTitle);
	DDX_Text(pDX, IDC_EDIT_URL, m_strURL);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_strUserName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAddEntryDlg, CDialog)
	//{{AFX_MSG_MAP(CAddEntryDlg)
	ON_BN_CLICKED(IDC_CHECK_HIDEPW, OnCheckHidePw)
	ON_BN_CLICKED(IDC_PICKICON_BTN, OnPickIconBtn)
	ON_BN_CLICKED(IDC_RANDOMPW_BTN, OnRandomPwBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CAddEntryDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// The password dots font
	m_fStyle.CreateFont(-12, 0, 0, 0, 0, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, "Tahoma");
	GetDlgItem(IDC_EDIT_PASSWORD)->SetFont(&m_fStyle, TRUE);
	GetDlgItem(IDC_EDIT_REPEATPW)->SetFont(&m_fStyle, TRUE);
	GetDlgItem(IDC_CHECK_HIDEPW)->SetFont(&m_fStyle, TRUE);

	// The available images for the entry
	m_ilIcons.Create(IDR_CLIENTICONS, 16, 1, RGB(255,0,255)); // Purple is transparent

	// Make the buttons look cool
	NewGUI_Button(&m_btOK, IDB_OK, IDB_OK);
	NewGUI_Button(&m_btCancel, IDB_CANCEL, IDB_CANCEL);
	NewGUI_Button(&m_btRandomPw, -1, -1);
	NewGUI_Button(&m_btPickIcon, -1, -1);
	NewGUI_Button(&m_btHidePw, -1, -1);

	// Set the imagelist for the group selector combo box
	m_pGroups.SetXImageList(&m_ilIcons);

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
	unsigned int i; PW_GROUP *p;
	for(i = 0; i < m_pMgr->GetNumberOfGroups(); i++)
	{
		p = m_pMgr->GetGroup(i);
		ASSERT(p != NULL);
		m_pGroups.AddCTString(WZ_ROOT_INDEX, (BYTE)p->uImageId, p->pszGroupName);
	}

	ASSERT(m_nGroupId != -1); // Must have been initialized by parent
	m_pGroups.SetCurSel(m_nGroupId);

	// Configure banner control
	m_banner.Attach(this, KCSB_ATTACH_TOP);
	m_banner.SetColBkg(RGB(255,255,255));
	m_banner.SetColBkg2(NewGUI_GetBgColor());
	m_banner.SetColEdge(RGB(0,0,0));
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
	CString strStars = (TCHAR)('z' + 27);
	strStars += (TCHAR)('z' + 27); strStars += (TCHAR)('z' + 27);
	GetDlgItem(IDC_CHECK_HIDEPW)->SetWindowText(strStars);
	m_bStars = TRUE;
	OnCheckHidePw();

	// Configure link edit control
	m_pURL.SetLinkOption(HEOL_AUTO);
	m_pURL.SetUnderlineOption(HEOU_ALWAYS);
	m_pURL.SetVisited(FALSE);
	m_pURL.SetIEColours();
	m_pURL.SetDblClkToJump(TRUE);

	m_pURL.SetWindowText(m_strURL);

	UpdateData(FALSE);

	// Translate all windows
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	GetDlgItem(IDC_EDIT_TITLE)->SetFocus();
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
		m_pEditPw.SetPasswordChar((TCHAR)('z' + 27));
		m_pRepeatPw.SetPasswordChar((TCHAR)('z' + 27));
	}

	UpdateData(FALSE);
	m_pEditPw.RedrawWindow();
	m_pRepeatPw.RedrawWindow();
	m_pEditPw.SetFocus();
	m_pRepeatPw.SetFocus();
}

void CAddEntryDlg::OnPickIconBtn() 
{
	CIconPickerDlg dlg;

	dlg.m_pImageList = &m_ilIcons;
	dlg.m_uNumIcons = (UINT)m_ilIcons.GetImageCount();

	if(dlg.DoModal() == IDOK)
	{
		m_nIconId = dlg.m_nSelectedIcon;
	}
}

void CAddEntryDlg::OnRandomPwBtn() 
{
	CPwGeneratorDlg dlg;

	UpdateData(TRUE);

	if(dlg.DoModal() == IDOK)
	{
		m_strPassword = dlg.m_strPassword;
		m_strRepeatPw = dlg.m_strPassword;

		EraseCString(&dlg.m_strPassword);

		UpdateData(FALSE);
	}
}
