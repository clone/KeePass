/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2007 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "IconPickerDlg.h"

#include "NewGUI/NewGUICommon.h"
#include "../KeePassLibCpp/Util/TranslateEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CIconPickerDlg::CIconPickerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIconPickerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIconPickerDlg)
	//}}AFX_DATA_INIT

	m_pImageList = NULL;
	m_uNumIcons = 0;
	m_nSelectedIcon = 0;
}

void CIconPickerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIconPickerDlg)
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_ICONLIST, m_cList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIconPickerDlg, CDialog)
	//{{AFX_MSG_MAP(CIconPickerDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CIconPickerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	ASSERT(m_pImageList != NULL);
	ASSERT(m_uNumIcons != 0);

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	m_cList.SetImageList(m_pImageList, LVSIL_SMALL);

	ASSERT(LVM_SETEXTENDEDLISTVIEWSTYLE == (0x1000 + 54));
	m_cList.PostMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_SI_REPORT |
		LVS_EX_ONECLICKACTIVATE | LVS_EX_UNDERLINEHOT);

	unsigned long i;
	CString str;
	for(i = 0; i < m_uNumIcons; i++)
	{
		str.Format(_T("%u"), i);
		m_cList.InsertItem(LVIF_IMAGE | LVIF_TEXT, i, str, 0, 0, i, 0);
	}

	if((m_nSelectedIcon >= 0) && (m_nSelectedIcon < m_cList.GetItemCount()))
		m_cList.SetItemState(m_nSelectedIcon, LVIS_SELECTED, LVIS_SELECTED);

	NewGUI_XPButton(&m_btOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(&m_btCancel, IDB_CANCEL, IDB_CANCEL);

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_ICONPIC),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(TRL("Pick an Icon"));
	m_banner.SetCaption(TRL("Pick an icon from the list and click OK to set it."));

	return TRUE; // Return TRUE unless you set the focus to a control
}

void CIconPickerDlg::OnOK() 
{
	int i, f = -1, s = -1;
	UINT uState;

	m_nSelectedIcon = 0;

	for(i = 0; i < m_cList.GetItemCount(); i++)
	{
		uState = m_cList.GetItemState(i, LVIS_SELECTED | LVIS_FOCUSED);
		if(uState & LVIS_SELECTED) s = i;
		if(uState & LVIS_FOCUSED) f = i;
	}

	if(f != -1) m_nSelectedIcon = f;
	if(s != -1) m_nSelectedIcon = s;

	CDialog::OnOK();
}

void CIconPickerDlg::OnCancel() 
{
	CDialog::OnCancel();
}
