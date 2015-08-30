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
#include "OptionsDlg.h"

#include "NewGUI/TranslateEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

COptionsDlg::COptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsDlg)
	m_nNewlineSequence = -1;
	m_uClipboardSeconds = 0;
	m_bOpenLastDb = FALSE;
	m_bImgButtons = FALSE;
	m_bEntryGrid = FALSE;
	m_bAutoSave = FALSE;
	//}}AFX_DATA_INIT
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDlg)
	DDX_Control(pDX, IDC_BTN_SELFONT, m_btSelFont);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Radio(pDX, IDC_RADIO_NEWLINE_0, m_nNewlineSequence);
	DDX_Text(pDX, IDC_EDIT_CLIPBOARDTIME, m_uClipboardSeconds);
	DDX_Check(pDX, IDC_CHECK_AUTOOPENLASTDB, m_bOpenLastDb);
	DDX_Check(pDX, IDC_CHECK_IMGBUTTONS, m_bImgButtons);
	DDX_Check(pDX, IDC_CHECK_ENTRYGRID, m_bEntryGrid);
	DDX_Check(pDX, IDC_CHECK_AUTOSAVE, m_bAutoSave);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	//{{AFX_MSG_MAP(COptionsDlg)
	ON_BN_CLICKED(IDC_BTN_SELFONT, OnBtnSelFont)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL COptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	NewGUI_Button(&m_btOK, IDB_OK, IDB_OK);
	NewGUI_Button(&m_btCancel, IDB_CANCEL, IDB_CANCEL);
	NewGUI_Button(&m_btSelFont, IDB_DOCUMENT_SMALL, IDB_DOCUMENT_SMALL);

	m_banner.Attach(this, KCSB_ATTACH_TOP);
	m_banner.SetColBkg(RGB(255,255,255));
	m_banner.SetColBkg2(NewGUI_GetBgColor());
	m_banner.SetColEdge(RGB(0,0,0));
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_OPTIONS),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(TRL("Settings"));
	m_banner.SetCaption(TRL("Here you can configure KeePass."));

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	UpdateData(FALSE);

	return TRUE;
}

void COptionsDlg::OnOK() 
{
	UpdateData(TRUE);
	CDialog::OnOK();
}

void COptionsDlg::OnCancel() 
{
	CDialog::OnCancel();
}

void COptionsDlg::OnBtnSelFont() 
{
	CString strFontSpec = m_strFontSpec;
	CString strFace, strSize, strFlags;
	int nChars = strFontSpec.ReverseFind(';');
	int nSizeEnd = strFontSpec.ReverseFind(',');
	strFace = strFontSpec.Left(nChars);
	strSize = strFontSpec.Mid(nChars + 1, nSizeEnd - nChars - 1);
	strFlags = strFontSpec.Right(4);
	int nSize = atoi((LPCTSTR)strSize);
	int nWeight = FW_NORMAL;
	if(strFlags.GetAt(0) == '1') nWeight = FW_BOLD;
	BYTE bItalic = (strFlags.GetAt(1) == '1') ? TRUE : FALSE;
	BYTE bUnderlined = (strFlags.GetAt(2) == '1') ? TRUE : FALSE;
	BYTE bStrikeOut = (strFlags.GetAt(3) == '1') ? TRUE : FALSE;

	LOGFONT lf;
	HDC hDC = GetDC()->m_hDC;
	ASSERT(hDC != NULL);
	if(hDC != NULL) lf.lfHeight = -MulDiv(nSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	else { ASSERT(FALSE); lf.lfHeight = -nSize; }
	lf.lfWidth = 0; lf.lfEscapement = 0; lf.lfOrientation = 0;
	lf.lfWeight = nWeight; lf.lfItalic = bItalic; lf.lfUnderline = bUnderlined;
	lf.lfStrikeOut = bStrikeOut; lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS; lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY; lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	strcpy(lf.lfFaceName, (LPCTSTR)strFace);

	CFontDialog dlg(&lf);
	CString strTemp;

	if(dlg.DoModal() == IDOK)
	{
		int dSize = dlg.GetSize();
		dSize = (dSize >= 0) ? dSize : -dSize;
		m_strFontSpec = dlg.GetFaceName();
		m_strFontSpec += ";";
		strTemp.Format("%d", dSize / 10);
		m_strFontSpec += strTemp;
		m_strFontSpec += ",";
		m_strFontSpec += (dlg.IsBold() == TRUE) ? '1' : '0';
		m_strFontSpec += (dlg.IsItalic() == TRUE) ? '1' : '0';
		m_strFontSpec += (dlg.IsUnderline() == TRUE) ? '1' : '0';
		m_strFontSpec += (dlg.IsStrikeOut() == TRUE) ? '1' : '0';
	}
}
