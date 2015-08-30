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
#include "IconPickerDlg.h"

#include "NewGUI/NewGUICommon.h"
#include "NewGUI/TranslateEx.h"

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
		str.Format("%u", i);
		m_cList.InsertItem(LVIF_IMAGE | LVIF_TEXT, i, str, 0, 0, i, 0);
	}

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
