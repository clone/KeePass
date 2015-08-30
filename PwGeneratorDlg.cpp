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
#include "PwGeneratorDlg.h"

#include "GetRandomDlg.h"

#include "NewGUI/NewGUICommon.h"
#include "NewGUI/TranslateEx.h"

#include "Crypto/sha2.h"

#include "Util/NewRandom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CPwGeneratorDlg::CPwGeneratorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPwGeneratorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPwGeneratorDlg)
	m_nCharacters = 12;
	m_strPassword = _T("");
	m_bCharSpec = FALSE;
	m_strCharSet = _T("");
	//}}AFX_DATA_INIT

	m_bCanAccept = TRUE;
}

void CPwGeneratorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPwGeneratorDlg)
	DDX_Control(pDX, IDC_GENERATE_BTN, m_btGenerate);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_LIST_OPTIONS, m_cList);
	DDX_Text(pDX, IDC_EDIT_NUMCHARACTERS, m_nCharacters);
	DDX_Text(pDX, IDC_EDIT_PW, m_strPassword);
	DDX_Check(pDX, IDC_CHECK_CHARSPEC, m_bCharSpec);
	DDX_Text(pDX, IDC_EDIT_ONLYCHARSPEC, m_strCharSet);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPwGeneratorDlg, CDialog)
	//{{AFX_MSG_MAP(CPwGeneratorDlg)
	ON_BN_CLICKED(IDC_GENERATE_BTN, OnGenerateBtn)
	ON_BN_CLICKED(IDC_CHECK_CHARSPEC, OnCheckCharSpec)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CPwGeneratorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	NewGUI_Button(&m_btnOK, IDB_OK, IDB_OK);
	NewGUI_Button(&m_btnCancel, IDB_CANCEL, IDB_CANCEL);
	NewGUI_Button(&m_btGenerate, IDB_KEY_SMALL, IDB_KEY_SMALL);

	m_banner.Attach(this, KCSB_ATTACH_TOP);
	m_banner.SetColBkg(RGB(255,255,255));
	m_banner.SetColBkg2(NewGUI_GetBgColor());
	m_banner.SetColEdge(RGB(0,0,0));
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_KEY),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(TRL("Password Generator"));
	m_banner.SetCaption(TRL("This will generate a random password."));

	m_cList.PostMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_SI_MENU |
		LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);

	m_cList.DeleteAllItems();
	RECT rect;
	m_cList.GetClientRect(&rect);
	int nWidth = rect.right - rect.left - GetSystemMetrics(SM_CXVSCROLL) - 8;
	m_cList.InsertColumn(0, TRL("Options"), LVCFMT_LEFT, nWidth, 0);

	m_btnOK.EnableWindow(FALSE);

	int j = 0;
	m_cList.InsertItem(LVIF_TEXT, j, TRL("Upper alphabetic characters (A, B, C, ...)"),
		0, 0, 0, 0); j++;
	m_cList.InsertItem(LVIF_TEXT, j, TRL("Lower alphabetic characters (a, b, c, ...)"),
		0, 0, 0, 0); j++;
	m_cList.InsertItem(LVIF_TEXT, j, TRL("Numerical characters (1, 2, 3, ...)"),
		0, 0, 0, 0); j++;
	m_cList.InsertItem(LVIF_TEXT, j, TRL("Underline character '_'"),
		0, 0, 0, 0); j++;
	m_cList.InsertItem(LVIF_TEXT, j, TRL("Minus '-'"),
		0, 0, 0, 0); j++;
	m_cList.InsertItem(LVIF_TEXT, j, TRL("Space ' '"),
		0, 0, 0, 0); j++;
	m_cList.InsertItem(LVIF_TEXT, j, TRL("Special characters (!, §, $, %, &, ...)"),
		0, 0, 0, 0); j++;
	m_cList.InsertItem(LVIF_TEXT, j, TRL("Higher ANSI characters (Â, ©, É, µ, ½, ...)"),
		0, 0, 0, 0); j++;
	m_cList.InsertItem(LVIF_TEXT, j, TRL("Special brackets ('{', '}', '[', ...)"),
		0, 0, 0, 0); j++;

	// _SetCheck doesn't seem to work in OnInitDialog?
	_SetCheck(0, FALSE);
	_SetCheck(1, FALSE);
	_SetCheck(2, FALSE);
	_SetCheck(3, FALSE);
	_SetCheck(4, FALSE);
	_SetCheck(5, FALSE);
	_SetCheck(6, FALSE);
	_SetCheck(7, FALSE);
	_SetCheck(8, FALSE);

	GetDlgItem(IDC_EDIT_ONLYCHARSPEC)->EnableWindow(FALSE);
	m_bCharSpec = FALSE;
	UpdateData(FALSE);

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	return TRUE;
}

void CPwGeneratorDlg::OnOK() 
{
	UpdateData(TRUE);

	if(m_strPassword.GetLength() == 0) return;

	CDialog::OnOK();
}

void CPwGeneratorDlg::OnCancel() 
{
	m_strPassword.Empty();

	CDialog::OnCancel();
}

void CPwGeneratorDlg::OnGenerateBtn() 
{
	CGetRandomDlg dlg;
	CNewRandom newrand;
	int nChars = 0;
	sha256_ctx ctx;
	unsigned long uCounter = 0, uFinalChars = 0, j;
	BYTE aTemp[32];
	TCHAR t;
	BOOL bUpperAlpha, bLowerAlpha, bNum, bUnderline, bMinus, bSpace, bSpecial, bHigh, bBrackets;

	ASSERT(sizeof(unsigned long) == 4);

	UpdateData(TRUE);

	newrand.Initialize();

	if(m_nCharacters == 0) m_nCharacters = 12;

	if((m_bCharSpec == TRUE) && (m_strCharSet.GetLength() == 0))
	{
		MessageBox(TRL("The password must consist of at least one character!"), TRL("Password Safe"),
			MB_OK | MB_ICONWARNING);
		return;
	}

	bUpperAlpha = _GetCheck(0);
	bLowerAlpha = _GetCheck(1);
	bNum = _GetCheck(2);
	bUnderline = _GetCheck(3);
	bMinus = _GetCheck(4);
	bSpace = _GetCheck(5);
	bSpecial = _GetCheck(6);
	bHigh = _GetCheck(7);
	bBrackets = _GetCheck(8);
	m_strPassword.Empty();

	if(m_bCharSpec == FALSE)
	{
		if(bUpperAlpha + bLowerAlpha + bNum + bUnderline + bMinus + bSpace + bSpecial +
			bHigh + bBrackets == 0)
		{
			MessageBox(TRL("Select at least one of the options in the list above!"),
				TRL("Password Safe"), MB_OK | MB_ICONWARNING);
			return;
		}
	}

	if(dlg.DoModal() == IDCANCEL) return;

	j = 32;
	while(1)
	{
		if(uFinalChars == m_nCharacters) break;

		if(j == 32)
		{
			sha256_begin(&ctx);
			sha256_hash(dlg.m_pFinalRandom, 32, &ctx);
			sha256_hash((BYTE *)&uCounter, 4, &ctx);
			sha256_end(aTemp, &ctx);

			j = 0;
			uCounter++;
		}

		BYTE bt;
		newrand.GetRandomBuffer(&bt, 1);
		if(bt < 130) // Some additional randomness
		{
			j++;
			continue;
		}

		t = (TCHAR)(BYTE)aTemp[j];

		if(m_bCharSpec == FALSE)
		{
			if(bUpperAlpha == TRUE)
			{
				if((t >= 'A') && (t <= 'Z'))
				{ m_strPassword += t; uFinalChars++; }
			}

			if(bLowerAlpha == TRUE)
			{
				if((t >= 'a') && (t <= 'z'))
				{ m_strPassword += t; uFinalChars++; }
			}

			if(bNum == TRUE)
			{
				if((t >= '0') && (t <= '9'))
				{ m_strPassword += t; uFinalChars++; }
			}

			if((bUnderline == TRUE) && (t == '_'))
				{ m_strPassword += t; uFinalChars++; }

			if((bMinus == TRUE) && (t == '-'))
				{ m_strPassword += t; uFinalChars++; }

			if((bSpace == TRUE) && (t == ' '))
				{ m_strPassword += t; uFinalChars++; }

			if(bSpecial == TRUE)
			{
				if((t >= '!') && (t <= '/'))
				{ m_strPassword += t; uFinalChars++; }
			}

			if((bHigh == TRUE) && (((BYTE)t) > '~'))
				{ m_strPassword += t; uFinalChars++; }

			if(bBrackets == TRUE)
			{
				if((t == '[') || (t == ']') || (t == '{') || (t == '}'))
					{ m_strPassword += t; uFinalChars++; }
			}
		}
		else // m_bCharSpec == TRUE
		{
			bool bFound = false;
			for(int ix = 0; ix < m_strCharSet.GetLength(); ix++)
			{
				if((BYTE)(TCHAR)m_strCharSet[ix] == (BYTE)t) bFound = true;
			}

			if(bFound == true)
			{
				m_strPassword += t;
				uFinalChars++;
			}
		}

		j++;
	}

	UpdateData(FALSE);

	if(m_bCanAccept == TRUE) m_btnOK.EnableWindow(TRUE);
}

void CPwGeneratorDlg::_SetCheck(int inxItem, BOOL bEnable)
{
	LV_ITEM _ms_lvi = {0};
	memset(&_ms_lvi, 0, sizeof(LV_ITEM));
	_ms_lvi.stateMask = LVIS_STATEIMAGEMASK;
	_ms_lvi.state = (UINT)(((int)(bEnable) + 1) << 12);

	m_cList.SendMessage(LVM_SETITEMSTATE, (WPARAM)inxItem,
		(LPARAM)(LV_ITEM FAR *)&_ms_lvi);
}

BOOL CPwGeneratorDlg::_GetCheck(int inxItem)
{
	return ((((UINT)(m_cList.SendMessage(LVM_GETITEMSTATE, (WPARAM)inxItem,
		LVIS_STATEIMAGEMASK))) >> 12) - 1);
}

void CPwGeneratorDlg::OnCheckCharSpec() 
{
	UpdateData(TRUE);

	if(m_bCharSpec == TRUE)
	{
		GetDlgItem(IDC_EDIT_ONLYCHARSPEC)->EnableWindow(TRUE);
		m_cList.EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_ONLYCHARSPEC)->EnableWindow(FALSE);
		m_cList.EnableWindow(TRUE);
	}
}
