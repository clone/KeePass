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
#include "EntryPropertiesDlg.h"
#include "IconPickerDlg.h"
#include "NewGUI/TranslateEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CEntryPropertiesDlg::CEntryPropertiesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEntryPropertiesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEntryPropertiesDlg)
	m_bModExpire = FALSE;
	m_bModGroup = FALSE;
	m_bModIcon = FALSE;
	//}}AFX_DATA_INIT

	m_pMgr = NULL;
	m_pParentIcons = NULL;
}

void CEntryPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEntryPropertiesDlg)
	DDX_Control(pDX, IDC_COMBO_GROUPS, m_cbGroups);
	DDX_Control(pDX, IDC_BUTTON_SELECT_ICON, m_btSelectIcon);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDC_EDIT_EXPIRE_TIME, m_editTime);
	DDX_Control(pDX, IDC_EDIT_EXPIRE_DATE, m_editDate);
	DDX_Check(pDX, IDC_CHECK_MODEXPIRE, m_bModExpire);
	DDX_Check(pDX, IDC_CHECK_MODGROUP, m_bModGroup);
	DDX_Check(pDX, IDC_CHECK_MODICON, m_bModIcon);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEntryPropertiesDlg, CDialog)
	//{{AFX_MSG_MAP(CEntryPropertiesDlg)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_ICON, OnButtonSelectIcon)
	ON_BN_CLICKED(IDC_CHECK_MODEXPIRE, OnCheckModExpire)
	ON_BN_CLICKED(IDC_CHECK_MODGROUP, OnCheckModGroup)
	ON_BN_CLICKED(IDC_CHECK_MODICON, OnCheckModIcon)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CEntryPropertiesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	ASSERT(m_pMgr != NULL); // You must set the manager before using this class
	ASSERT(m_pParentIcons != NULL); // You must set the image list first!

	// Translate all windows
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	NewGUI_XPButton(&m_btOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(&m_btCancel, IDB_CANCEL, IDB_CANCEL);
	NewGUI_XPButton(&m_btSelectIcon, -1, -1);

	// Set the imagelist for the group selector combo box
	m_cbGroups.SetImageList(m_pParentIcons);

	ASSERT(m_pMgr != NULL); // Must have been initialized by parent
	unsigned int i; PW_GROUP *p;
	COMBOBOXEXITEM cbi;
	for(i = 0; i < m_pMgr->GetNumberOfGroups(); i++) // Add groups to combo box
	{
		p = m_pMgr->GetGroup(i);
		ASSERT(p != NULL);

		ZeroMemory(&cbi, sizeof(COMBOBOXEXITEM));
		cbi.mask = CBEIF_IMAGE | CBEIF_TEXT | CBEIF_INDENT | CBEIF_SELECTEDIMAGE;
		cbi.iItem = (int)i;
		cbi.pszText = (LPTSTR)p->pszGroupName;
		cbi.cchTextMax = (int)_tcslen(p->pszGroupName);
		cbi.iImage = cbi.iSelectedImage = (int)p->uImageId;
		cbi.iIndent = (int)p->usLevel;
		m_cbGroups.InsertItem(&cbi);
	}

	m_cbGroups.SetCurSel(0);

	// Configure banner control
	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_KEY), KCSB_ICON_LEFT | KCSB_ICON_VCENTER);

	m_banner.SetTitle(TRL("Mass Modify"));
	m_banner.SetCaption(TRL("Change fields of all selected entries"));
	SetWindowText(TRL("Mass Modify"));

	COleDateTime oleMin = AMS_MIN_OLEDATETIME;
	COleDateTime oleMax(2999, 12, 28, 23, 59, 59);
	m_editDate.SetRange(oleMin, oleMax);
	m_editTime.SetRange(oleMin, oleMax);
	m_editDate.SetDate(oleMax);
	m_editTime.SetAMPM(true);
	m_editTime.Show24HourFormat(true);
	m_editTime.ShowSeconds(true);
	m_editTime.SetTime(oleMax);

	m_bModGroup = FALSE;
	m_cbGroups.EnableWindow(FALSE);
	m_bModIcon = FALSE;
	GetDlgItem(IDC_BUTTON_SELECT_ICON)->EnableWindow(FALSE);
	m_bModExpire = FALSE;
	m_editTime.EnableWindow(FALSE);
	m_editDate.EnableWindow(FALSE);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CEntryPropertiesDlg::OnOK() 
{
	UpdateData(TRUE);

	if(m_bModGroup == TRUE)
	{
		CString strGroupTest;
		m_cbGroups.GetLBText(m_cbGroups.GetCurSel(), strGroupTest);
		if(CPwManager::IsAllowedStoreGroup((LPCTSTR)strGroupTest, PWS_SEARCHGROUP) == FALSE)
		{
			MessageBox(TRL("The group you selected cannot store entries. Please select an other group."),
				TRL("Stop"), MB_ICONWARNING | MB_OK);
			return;
		}
	}

	m_tExpire.shYear = (USHORT)m_editDate.GetYear();
	m_tExpire.btMonth = (BYTE)m_editDate.GetMonth();
	m_tExpire.btDay = (BYTE)m_editDate.GetDay();
	m_tExpire.btHour = (BYTE)m_editTime.GetHour();
	m_tExpire.btMinute = (BYTE)m_editTime.GetMinute();
	m_tExpire.btSecond = (BYTE)m_editTime.GetSecond();

	m_nGroupInx = m_cbGroups.GetCurSel();

	CDialog::OnOK();
}

void CEntryPropertiesDlg::OnCancel() 
{
	CDialog::OnCancel();
}

void CEntryPropertiesDlg::OnButtonSelectIcon() 
{
	CIconPickerDlg dlg;

	dlg.m_pImageList = m_pParentIcons;
	dlg.m_uNumIcons = (UINT)m_pParentIcons->GetImageCount();

	if(dlg.DoModal() == IDOK)
	{
		m_nIconId = dlg.m_nSelectedIcon;
	}
}

void CEntryPropertiesDlg::OnCheckModExpire() 
{
	UpdateData(TRUE);
	if(m_bModExpire == TRUE) { m_editTime.EnableWindow(TRUE); m_editDate.EnableWindow(TRUE); }
	else { m_editTime.EnableWindow(FALSE); m_editDate.EnableWindow(FALSE); }
}

void CEntryPropertiesDlg::OnCheckModGroup() 
{
	UpdateData(TRUE);
	if(m_bModGroup == TRUE) m_cbGroups.EnableWindow(TRUE);
	else m_cbGroups.EnableWindow(FALSE);
}

void CEntryPropertiesDlg::OnCheckModIcon() 
{
	UpdateData(TRUE);
	if(m_bModIcon == TRUE) GetDlgItem(IDC_BUTTON_SELECT_ICON)->EnableWindow(TRUE);
	else GetDlgItem(IDC_BUTTON_SELECT_ICON)->EnableWindow(FALSE);
}
