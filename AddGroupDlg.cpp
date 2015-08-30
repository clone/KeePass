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
#include "AddGroupDlg.h"

#include "IconPickerDlg.h"
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

	NewGUI_Button(&m_btSetIcon, -1, -1);
	NewGUI_Button(&m_btOK, IDB_OK, IDB_OK);
	NewGUI_Button(&m_btCancel, IDB_CANCEL, IDB_CANCEL);

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
	UpdateData(TRUE);

	if(m_strGroupName.GetLength() == 0)
	{
		MessageBox(TRL("Enter a group name!"),
			TRL("Stop"), MB_ICONINFORMATION);
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
	}
}
