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
#include "GetRandomDlg.h"

#include "Util/NewRandom.h"

#include "NewGUI/TranslateEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CGetRandomDlg::CGetRandomDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGetRandomDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGetRandomDlg)
	m_strRandom = _T("");
	//}}AFX_DATA_INIT
}

void CGetRandomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetRandomDlg)
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDC_BEGIN_MOUSEINPUT, m_btMouseInput);
	DDX_Control(pDX, IDC_PROGRESS_MOUSE, m_cMouseProgress);
	DDX_Text(pDX, IDC_EDIT_RANDOMSTRING, m_strRandom);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGetRandomDlg, CDialog)
	//{{AFX_MSG_MAP(CGetRandomDlg)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BEGIN_MOUSEINPUT, OnBeginMouseInput)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CGetRandomDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_cMouseProgress.SetRange(0, 100);
	m_cMouseProgress.SetPos(0);

	m_dwCurPoint = 0;
	m_bMouseActive = FALSE;

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	NewGUI_Button(&m_btOK, IDB_OK, IDB_OK);
	NewGUI_Button(&m_btCancel, IDB_CANCEL, IDB_CANCEL);
	NewGUI_Button(&m_btMouseInput, IDB_MOUSE_SMALL, IDB_MOUSE_SMALL);

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_KEY),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(TRL("Get random data for key generation"));
	m_banner.SetCaption(TRL("Move the mouse and type some random text to generate random values."));

	CNewRandom cRand;
	cRand.Initialize();
	cRand.GetRandomBuffer(m_pFinalRandom, 32);
	cRand.GetRandomBuffer((BYTE *)m_points, sizeof(POINT) * 100);

	return TRUE;
}

void CGetRandomDlg::OnOK() 
{
	sha256_ctx ctx;
	int nStringLen;

	UpdateData(TRUE);

	if((m_dwCurPoint == 0) && (m_strRandom.GetLength() == 0))
	{
		MessageBox(TRL("You must generate some random values using the random mouse input box or the edit field before you can close this dialog."),
			TRL("No random input"), MB_OK | MB_ICONWARNING);
		return;
	}

	sha256_begin(&ctx);
	sha256_hash(m_pFinalRandom, 32, &ctx);

	sha256_hash((unsigned char *)m_points, sizeof(POINT) * 100, &ctx);

	nStringLen = m_strRandom.GetLength();
	if(nStringLen != 0)
		sha256_hash((BYTE *)(LPCTSTR)m_strRandom, (unsigned long)nStringLen, &ctx);

	sha256_end((unsigned char *)m_pFinalRandom, &ctx);

	CDialog::OnOK();
}

void CGetRandomDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(m_bMouseActive == TRUE)
	{
		if(PtInRect(&m_rect, point))
		{
			if(m_dwCurPoint < 100)
			{
				if((rand() % 5) == 0)
				{
					m_points[m_dwCurPoint] = (POINT)point;
					m_dwCurPoint++;
					m_cMouseProgress.SetPos(m_dwCurPoint);
				}
			}
			else
			{
				m_bMouseActive = FALSE;

				GetDlgItem(IDC_BEGIN_MOUSEINPUT)->EnableWindow(TRUE);
				GetDlgItem(IDC_BEGIN_MOUSEINPUT)->SetFocus();
			}
		}
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CGetRandomDlg::OnBeginMouseInput() 
{
	int x, y;

	GetDlgItem(IDC_STATIC_RANDOM)->GetWindowRect(&m_rect);

	x = m_rect.left + ((m_rect.right - m_rect.left) / 2);
	y = m_rect.top + ((m_rect.bottom - m_rect.top) / 2);

	SetCursorPos(x, y);

	ScreenToClient(&m_rect);

	m_cMouseProgress.SetPos(0);

	GetDlgItem(IDC_BEGIN_MOUSEINPUT)->EnableWindow(FALSE);

	m_dwCurPoint = 0;
	m_bMouseActive = TRUE;
}

void CGetRandomDlg::OnCancel() 
{
	CDialog::OnCancel();
}
