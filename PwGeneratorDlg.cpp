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

static CString g_strOptions = _T("");
static CString g_strCharSet = _T("");
static UINT g_nChars = 16;

/////////////////////////////////////////////////////////////////////////////

CPwGeneratorDlg::CPwGeneratorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPwGeneratorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPwGeneratorDlg)
	m_nCharacters = 16;
	m_strPassword = _T("");
	m_bCharSpec = FALSE;
	m_strCharSet = _T("");
	m_bGetEntropy = FALSE;
	//}}AFX_DATA_INIT

	m_bCanAccept = TRUE;
}

void CPwGeneratorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPwGeneratorDlg)
	DDX_Control(pDX, IDC_PROGRESS_PASSQUALITY, m_cPassQuality);
	DDX_Control(pDX, IDC_SPIN_NUMCHARS, m_spinNumChars);
	DDX_Control(pDX, IDC_GENERATE_BTN, m_btGenerate);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_LIST_OPTIONS, m_cList);
	DDX_Text(pDX, IDC_EDIT_NUMCHARACTERS, m_nCharacters);
	DDX_Text(pDX, IDC_EDIT_PW, m_strPassword);
	DDX_Check(pDX, IDC_CHECK_CHARSPEC, m_bCharSpec);
	DDX_Text(pDX, IDC_EDIT_ONLYCHARSPEC, m_strCharSet);
	DDX_Check(pDX, IDC_CHECK_GETENTROPY, m_bGetEntropy);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPwGeneratorDlg, CDialog)
	//{{AFX_MSG_MAP(CPwGeneratorDlg)
	ON_BN_CLICKED(IDC_GENERATE_BTN, OnGenerateBtn)
	ON_BN_CLICKED(IDC_CHECK_CHARSPEC, OnCheckCharSpec)
	ON_NOTIFY(NM_CLICK, IDC_LIST_OPTIONS, OnClickListOptions)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_OPTIONS, OnRclickListOptions)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_NUMCHARS, OnDeltaPosSpinNumChars)
	ON_EN_CHANGE(IDC_EDIT_PW, OnChangeEditPw)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CPwGeneratorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	NewGUI_Button(&m_btnOK, IDB_OK, IDB_OK);
	NewGUI_Button(&m_btnCancel, IDB_CANCEL, IDB_CANCEL);
	NewGUI_Button(&m_btGenerate, IDB_KEY_SMALL, IDB_KEY_SMALL);

	NewGUI_ConfigQualityMeter(&m_cPassQuality);

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_KEY),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(TRL("Password Generator"));
	m_banner.SetCaption(TRL("This will generate a random password."));

	m_cList.PostMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_SI_MENU | LVS_EX_FULLROWSELECT);

	m_ilIcons.Create(IDR_INFOICONS, 16, 1, RGB(255,0,255));
	m_cList.SetImageList(&m_ilIcons, LVSIL_SMALL);

	m_cList.DeleteAllItems();
	RECT rect;
	m_cList.GetClientRect(&rect);
	int nWidth = rect.right - rect.left - GetSystemMetrics(SM_CXVSCROLL) - 8;
	m_cList.InsertColumn(0, TRL("Options"), LVCFMT_LEFT, nWidth, 0);

	m_btnOK.EnableWindow(FALSE);

	int j = 0;
	m_cList.InsertItem(LVIF_TEXT | LVIF_IMAGE, j,
		TRL("Upper alphabetic characters (A, B, C, ...)"), 0, 0, 0, 0); j++;
	m_cList.InsertItem(LVIF_TEXT | LVIF_IMAGE, j,
		TRL("Lower alphabetic characters (a, b, c, ...)"), 0, 0, 0, 0); j++;
	m_cList.InsertItem(LVIF_TEXT | LVIF_IMAGE, j,
		TRL("Numerical characters (1, 2, 3, ...)"), 0, 0, 0, 0); j++;
	m_cList.InsertItem(LVIF_TEXT | LVIF_IMAGE, j,
		TRL("Underline character '_'"), 0, 0, 0, 0); j++;
	m_cList.InsertItem(LVIF_TEXT | LVIF_IMAGE, j,
		TRL("Minus '-'"), 0, 0, 0, 0); j++;
	m_cList.InsertItem(LVIF_TEXT | LVIF_IMAGE, j,
		TRL("Space ' '"), 0, 0, 0, 0); j++;
	m_cList.InsertItem(LVIF_TEXT | LVIF_IMAGE, j,
		TRL("Special characters (!, §, $, %, &, ...)"), 0, 0, 0, 0); j++;
	m_cList.InsertItem(LVIF_TEXT | LVIF_IMAGE, j,
		TRL("Higher ANSI characters (Â, ©, É, µ, ½, ...)"), 0, 0, 0, 0); j++;
	m_cList.InsertItem(LVIF_TEXT | LVIF_IMAGE, j,
		TRL("Special brackets ('{', '}', '[', ...)"), 0, 0, 0, 0); j++;

	if(g_strOptions.GetLength() != 11) g_strOptions = _T("11100000001");
	ASSERT(g_strOptions.GetLength() == 11);

	TCHAR tch;
	int nItem;
	for(nItem = 0; nItem < 9; nItem++)
	{
		tch = g_strOptions.GetAt(nItem);
		if(tch == _T('1')) _SetCheck(nItem, TRUE);
		else _SetCheck(nItem, FALSE);
	}

	tch = g_strOptions.GetAt(9);
	if(tch == _T('1')) m_bCharSpec = TRUE;
	else m_bCharSpec = FALSE;

	tch = g_strOptions.GetAt(10);
	if(tch == _T('0')) m_bGetEntropy = FALSE;
	else m_bGetEntropy = TRUE;

	if(m_bCharSpec == FALSE)
	{
		m_cList.EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_ONLYCHARSPEC)->EnableWindow(FALSE);
	}
	else
	{
		m_cList.EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_ONLYCHARSPEC)->EnableWindow(TRUE);
	}

	m_strCharSet = g_strCharSet;
	m_nCharacters = g_nChars;
	UpdateData(FALSE);

	m_spinNumChars.SetBase(0);
	m_spinNumChars.SetRange(0, 1024);
	m_spinNumChars.SetPos(512);

	if(m_bCanAccept == FALSE)
	{
		m_btnOK.ShowWindow(SW_HIDE);
		m_btnCancel.SetWindowText(_T("&Close"));
	}
	else m_btnOK.ShowWindow(SW_SHOW);

	NewGUI_ShowQualityMeter(&m_cPassQuality, GetDlgItem(IDC_STATIC_PASSBITS), m_strPassword);

	return TRUE;
}

void CPwGeneratorDlg::CleanUp()
{
	m_ilIcons.DeleteImageList();
}

void CPwGeneratorDlg::_SaveOptions()
{
	int i;
	g_strOptions.Empty();
	for(i = 0; i < 9; i++)
	{
		if(_GetCheck(i) == FALSE) g_strOptions += _T("0");
		else g_strOptions += _T("1");
	}
	if(m_bCharSpec == FALSE) g_strOptions += _T("0");
	else g_strOptions += _T("1");
	if(m_bGetEntropy == FALSE) g_strOptions += _T("0");
	else g_strOptions += _T("1");

	g_strCharSet = m_strCharSet;
	g_nChars = m_nCharacters;
}

void CPwGeneratorDlg::OnOK() 
{
	UpdateData(TRUE);

	if(m_strPassword.GetLength() == 0) return;

	_SaveOptions();

	CleanUp();
	CDialog::OnOK();
}

void CPwGeneratorDlg::OnCancel() 
{
	UpdateData(TRUE);

	m_strPassword.Empty();

	_SaveOptions();

	CleanUp();
	CDialog::OnCancel();
}

void CPwGeneratorDlg::OnGenerateBtn() 
{
	CGetRandomDlg dlg;
	CNewRandom newrand;
	sha256_ctx ctx;
	unsigned long uCounter = 0, uFinalChars = 0, j;
	BYTE aTemp[32];
	TCHAR t;
	BOOL bUpperAlpha, bLowerAlpha, bNum, bUnderline, bMinus, bSpace, bSpecial, bHigh, bBrackets;

	ASSERT(sizeof(unsigned long) == 4);

	UpdateData(TRUE);

	newrand.Initialize();

	if(m_nCharacters == 0) m_nCharacters = 16;

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

	if(m_bGetEntropy == TRUE)
	{
		if(dlg.DoModal() == IDCANCEL) return;
	}
	else
	{
		newrand.GetRandomBuffer(dlg.m_pFinalRandom, 32);
	}

	j = 32;
	while(1)
	{
		if(uFinalChars >= m_nCharacters) break;

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
			if((t != 0) && (t != _T('\r')) && (t != _T('\n')))
			{
				if(bUpperAlpha == TRUE)
				{
					if((t >= _T('A')) && (t <= _T('Z')))
						{ m_strPassword += t; uFinalChars++; }
				}

				if(bLowerAlpha == TRUE)
				{
					if((t >= _T('a')) && (t <= _T('z')))
						{ m_strPassword += t; uFinalChars++; }
				}

				if(bNum == TRUE)
				{
					if((t >= _T('0')) && (t <= _T('9')))
						{ m_strPassword += t; uFinalChars++; }
				}

				if((bUnderline == TRUE) && (t == _T('_')))
					{ m_strPassword += t; uFinalChars++; }

				if((bMinus == TRUE) && (t == _T('-')))
					{ m_strPassword += t; uFinalChars++; }

				if((bSpace == TRUE) && (t == _T(' ')))
					{ m_strPassword += t; uFinalChars++; }

				if(bSpecial == TRUE)
				{
					if(!((t == _T('[')) || (t == _T(']')) || (t == _T('{')) || (t == _T('}'))))
					{
						if((t >= _T('!')) && (t <= _T('/')))
							{ m_strPassword += t; uFinalChars++; }
						if((t >= _T(':')) && (t <= _T('@')))
							{ m_strPassword += t; uFinalChars++; }
						if((t >= _T('[')) && (t <= _T('`')))
							{ m_strPassword += t; uFinalChars++; }
					}
				}

				if((bHigh == TRUE) && (((BYTE)t) >= _T('~')))
					{ m_strPassword += t; uFinalChars++; }

				if(bBrackets == TRUE)
				{
					if((t == _T('[')) || (t == _T(']')) || (t == _T('{')) || (t == _T('}')))
						{ m_strPassword += t; uFinalChars++; }
				}
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

	NewGUI_ShowQualityMeter(&m_cPassQuality, GetDlgItem(IDC_STATIC_PASSBITS), m_strPassword);
	if(m_bCanAccept == TRUE) m_btnOK.EnableWindow(TRUE);
}

void CPwGeneratorDlg::_SetCheck(int inxItem, BOOL bEnable)
{
	LV_ITEM lviAdjust;

	ASSERT(inxItem < m_cList.GetItemCount());
	if(inxItem >= m_cList.GetItemCount()) return;

	lviAdjust.mask = LVIF_IMAGE;
	lviAdjust.iItem = inxItem;
	lviAdjust.iSubItem = 0;
	lviAdjust.state = 0;
	lviAdjust.stateMask = 0;
	lviAdjust.lParam = 0;
	lviAdjust.iIndent = 0;
	lviAdjust.iImage = (bEnable == FALSE) ? 57 : 58;

	m_cList.SetItem(&lviAdjust);
}

BOOL CPwGeneratorDlg::_GetCheck(int inxItem)
{
	LV_ITEM lvi;

	ZeroMemory(&lvi, sizeof(LV_ITEM));
	lvi.mask = LVIF_IMAGE;
	lvi.iItem = inxItem;
	if(m_cList.GetItem(&lvi) == FALSE) return FALSE;

	return ((lvi.iImage == 57) ? FALSE : TRUE);
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

void CPwGeneratorDlg::OnClickListOptions(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint pointM;

	GetCursorPos(&pointM);

	UNREFERENCED_PARAMETER(pNMHDR);

	m_cList.ScreenToClient(&pointM);

	UINT nFlags = 0;
	int nHitItem = m_cList.HitTest(pointM, &nFlags);

	if(nFlags & LVHT_ONITEM)
	{
		_SetCheck(nHitItem, !_GetCheck(nHitItem));
	}

	*pResult = 0;
}

void CPwGeneratorDlg::OnRclickListOptions(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UNREFERENCED_PARAMETER(pNMHDR);
	*pResult = 0;
}

void CPwGeneratorDlg::SetOptions(CString strOptions, CString strCharSet, UINT nCharacters)
{
	g_strOptions = strOptions;
	g_strCharSet = strCharSet;
	g_nChars = nCharacters;
}

void CPwGeneratorDlg::GetOptions(CString *pstrOptions, CString *pstrCharSet, UINT *pnCharacters)
{
	*pstrOptions = g_strOptions;
	*pstrCharSet = g_strCharSet;
	*pnCharacters = g_nChars;
}

void CPwGeneratorDlg::OnDeltaPosSpinNumChars(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	*pResult = 0;

	UpdateData(TRUE);

	int nPos = (int)m_nCharacters;
	nPos += pNMUpDown->iDelta;
	if(nPos < 0) nPos = 0;
	m_nCharacters = (UINT)nPos;

	m_spinNumChars.SetPos(512);

	UpdateData(FALSE);
}

void CPwGeneratorDlg::OnChangeEditPw() 
{
	UpdateData(TRUE);
	NewGUI_ShowQualityMeter(&m_cPassQuality, GetDlgItem(IDC_STATIC_PASSBITS), m_strPassword);

	if(m_strPassword.GetLength() == 0) m_btnOK.EnableWindow(FALSE);
	else m_btnOK.EnableWindow(TRUE);
}
