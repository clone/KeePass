/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2013 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "CheckOptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CCheckOptionsDlg::CCheckOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCheckOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCheckOptionsDlg)
	//}}AFX_DATA_INIT

	m_bTwoColumns = FALSE;
	m_bForceForeground = FALSE;

	m_dwNumParams = 0; m_pParams = NULL; // Parent must initialize
}

void CCheckOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCheckOptionsDlg)
	DDX_Control(pDX, IDC_LIST_OPTIONS, m_olOptions);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDOK, m_btOK);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCheckOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CCheckOptionsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CCheckOptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	ASSERT(m_dwNumParams != 0); ASSERT(m_pParams != NULL);
	
	// Translate all windows
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	NewGUI_XPButton(m_btOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(m_btCancel, IDB_CANCEL, IDB_CANCEL);

	// Configure banner control
	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_OPTIONS), KCSB_ICON_LEFT | KCSB_ICON_VCENTER);

	m_banner.SetTitle(m_strTitle);
	m_banner.SetCaption(m_strDescription);
	SetWindowText(m_strTitle);

	// m_ilOptionIcons.Create(IDB_OPTIONICONS_EX, 16, 1, RGB(255,0,255));
	CPwSafeApp::CreateHiColorImageList(&m_ilOptionIcons, IDB_OPTIONICONS_EX, 16);
	m_olOptions.InitOptionListEx(&m_ilOptionIcons, m_bTwoColumns);

	DWORD i = 0;
	for(i = 0; i < m_dwNumParams; i++)
	{
		if(m_pParams[i].pbValue == NULL)
			m_olOptions.AddGroupText(m_pParams[i].lpString, m_pParams[i].nIcon);
		else if(m_bTwoColumns)
		{
			ASSERT(m_pParams[i].lpSubString != NULL);
			m_olOptions.AddCheckItemEx(m_pParams[i].lpString, m_pParams[i].lpSubString, m_pParams[i].pbValue, NULL, OL_LINK_NULL);
		}
		else
		{
			ASSERT(m_pParams[i].lpSubString == NULL);
			m_olOptions.AddCheckItem(m_pParams[i].lpString, m_pParams[i].pbValue, NULL, OL_LINK_NULL);
		}
	}

	if(m_bForceForeground == TRUE)
		SetForegroundWindow();

	return TRUE;
}

void CCheckOptionsDlg::CleanUp()
{
	m_ilOptionIcons.DeleteImageList();
}

void CCheckOptionsDlg::OnOK() 
{
	CleanUp();
	CDialog::OnOK();
}

void CCheckOptionsDlg::OnCancel() 
{
	CleanUp();
	CDialog::OnCancel();
}
