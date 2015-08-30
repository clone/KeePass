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
#include "DbSettingsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "NewGUI/NewGUICommon.h"
#include "NewGUI/TranslateEx.h"

/////////////////////////////////////////////////////////////////////////////

CDbSettingsDlg::CDbSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDbSettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDbSettingsDlg)
	m_nAlgorithm = -1;
	m_dwNumKeyEnc = 0;
	//}}AFX_DATA_INIT
}

void CDbSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDbSettingsDlg)
	DDX_Control(pDX, IDC_COMBO_ENCALGOS, m_cEncAlgos);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_CBIndex(pDX, IDC_COMBO_ENCALGOS, m_nAlgorithm);
	DDX_Text(pDX, IDC_EDIT_KEYENC, m_dwNumKeyEnc);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDbSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(CDbSettingsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CDbSettingsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	NewGUI_XPButton(&m_btOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(&m_btCancel, IDB_CANCEL, IDB_CANCEL);

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_OPTIONS),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(TRL("Database Settings"));
	m_banner.SetCaption(TRL("Here you can configure the current database."));

	m_cEncAlgos.ResetContent();
	m_cEncAlgos.AddString(TRL("Advanced Encryption Standard (AES) (128-bit block cipher using 256-bit key)"));
	m_cEncAlgos.AddString(TRL("Twofish (128-bit block cipher using 256-bit key)"));
	m_cEncAlgos.SetCurSel(m_nAlgorithm);

	UpdateData(FALSE);

	return TRUE; // Return TRUE unless you set the focus to a control
}

void CDbSettingsDlg::OnOK() 
{
	UpdateData(TRUE);

	// Check if the user has entered something valid, otherwise fix it
	CString str;
	GetDlgItem(IDC_EDIT_KEYENC)->GetWindowText(str);
	str.TrimLeft(); str.TrimRight();
	if(str.GetLength() > 10) m_dwNumKeyEnc = 0xFFFFFFFE; // Set to max
	else if(str.GetLength() == 10)
	{
		if(str.GetAt(0) >= _T('4')) m_dwNumKeyEnc = 0xFFFFFFFE; // Set to max
	}

	UpdateData(FALSE);

	CDialog::OnOK();
}

void CDbSettingsDlg::OnCancel() 
{
	UpdateData(TRUE);
	CDialog::OnCancel();
}
