/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2005 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "AddGroupDlg.h"

#include "IconPickerDlg.h"
#include "PwSafe/PwManager.h"
#include "NewGUI/BtnST.h"
#include "NewGUI/TranslateEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CAddGroupDlg::CAddGroupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddGroupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddGroupDlg)
	m_strGroupName = _T("");
	//}}AFX_DATA_INIT

	m_nIconId = 0;
	m_pParentImageList = NULL;
	m_bEditMode = FALSE;
}

void CAddGroupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddGroupDlg)
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_SETICON_BTN, m_btSetIcon);
	DDX_Text(pDX, IDC_EDIT_GROUPNAME, m_strGroupName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAddGroupDlg, CDialog)
	//{{AFX_MSG_MAP(CAddGroupDlg)
	ON_BN_CLICKED(IDC_SETICON_BTN, OnSetIconBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CAddGroupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	ASSERT(m_pParentImageList != NULL);

	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	NewGUI_XPButton(&m_btSetIcon, -1, -1);
	NewGUI_XPButton(&m_btOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(&m_btCancel, IDB_CANCEL, IDB_CANCEL);

	CString strTT = TRL("&Pick One"); strTT.Remove(_T('&'));
	m_btSetIcon.SetTooltipText(strTT, TRUE);
	if((m_nIconId >= 0) && (m_pParentImageList != NULL))
		m_btSetIcon.SetIcon(m_pParentImageList->ExtractIcon(m_nIconId));

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_KEY),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);

	if(m_bEditMode == FALSE)
	{
		m_banner.SetTitle(TRL("Add Group"));
		m_banner.SetCaption(TRL("Add a new password group"));
		SetWindowText(TRL("Add Group"));
	}
	else
	{
		m_banner.SetTitle(TRL("Edit Group"));
		m_banner.SetCaption(TRL("Modify a password group"));
		SetWindowText(TRL("Edit Group"));
	}

	UpdateData(FALSE);
	GetDlgItem(IDC_EDIT_GROUPNAME)->SetFocus();
	return FALSE; // Return TRUE unless you set the focus to a control
}

void CAddGroupDlg::OnOK() 
{
	CString strRef = TRL(PWS_SEARCHGROUP), strTest;
	strRef.MakeLower();

	UpdateData(TRUE);

	if(m_strGroupName.GetLength() == 0)
	{
		MessageBox(TRL("Enter a group name!"),
			TRL("Stop"), MB_ICONINFORMATION);
		return;
	}
	strTest = m_strGroupName;
	strTest.MakeLower();
	if(strTest == strRef)
	{
		MessageBox(TRL("The group you selected cannot store entries. Please select an other group."),
			TRL("Stop"), MB_ICONWARNING | MB_OK);
		return;
	}

	CDialog::OnOK();
}

void CAddGroupDlg::OnCancel() 
{
	CDialog::OnCancel();
}

void CAddGroupDlg::OnSetIconBtn() 
{
	CIconPickerDlg dlg;

	dlg.m_pImageList = m_pParentImageList;
	dlg.m_uNumIcons = m_pParentImageList->GetImageCount();

	if(dlg.DoModal() == IDOK)
	{
		m_nIconId = dlg.m_nSelectedIcon;

		if((m_nIconId >= 0) && (m_pParentImageList != NULL))
			m_btSetIcon.SetIcon(m_pParentImageList->ExtractIcon(m_nIconId));
	}
}
