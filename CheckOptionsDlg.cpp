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

	NewGUI_XPButton(&m_btOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(&m_btCancel, IDB_CANCEL, IDB_CANCEL);

	// Configure banner control
	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_OPTIONS), KCSB_ICON_LEFT | KCSB_ICON_VCENTER);

	m_banner.SetTitle(m_strTitle);
	m_banner.SetCaption(m_strDescription);
	SetWindowText(m_strTitle);

	m_ilOptionIcons.Create(IDR_OPTIONICONS, 16, 1, RGB(255,0,255));
	m_olOptions.InitOptionListEx(&m_ilOptionIcons);

	DWORD i = 0;
	for(i = 0; i < m_dwNumParams; i++)
	{
		if(m_pParams[i].pbValue == NULL)
			m_olOptions.AddGroupText(m_pParams[i].lpString, m_pParams[i].nIcon);
		else
			m_olOptions.AddCheckItem(m_pParams[i].lpString, m_pParams[i].pbValue, NULL, OL_LINK_NULL);
	}

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
