/*
  Copyright (c) 2003, Dominik Reichl <dominik.reichl@t-online.de>
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
#include "NewGUICommon.h"
#include "BCMenu.h"
#include "ShadeButtonST.h"
#include "TranslateEx.h"

COLORREF NewGUI_GetBgColor()
{
	HDC hDC = NULL;
	int nBitsPerPixel = 0;

	hDC = GetDC(NULL);
	nBitsPerPixel = GetDeviceCaps(hDC, BITSPIXEL);
	ReleaseDC(NULL, hDC);

	if(nBitsPerPixel <= 8) return GetSysColor(COLOR_BTNFACE);

	return(CR_BACK);
}

COLORREF NewGUI_GetBtnColor()
{
	COLORREF clr = 0;

	clr = GetSysColor(COLOR_BTNFACE);

	return clr + 10;
}

void NewGUI_Button(void *pButton, int nBitmapIn, int nBitmapOut)
{
	CShadeButtonST *p = (CShadeButtonST *)pButton;

	p->SetFlat(TRUE);
	p->SetShade(CShadeButtonST::SHS_SOFTBUMP);
	p->SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0, 0, 0), TRUE);
	p->SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 0, 255), TRUE);

	if((nBitmapIn != -1) && (nBitmapOut != -1))
		p->SetBitmaps(nBitmapIn, RGB(255, 0, 255), nBitmapOut, RGB(255, 0, 255));

	p->DrawFlatFocus(TRUE);
}

void NewGUI_TranslateCWnd(CWnd *pWnd)
{
	CString str;
	pWnd->GetWindowText(str);
	pWnd->SetWindowText(TRL((LPCTSTR)str));
}

BOOL CALLBACK NewGUI_TranslateWindowCb(HWND hwnd, LPARAM lParam)
{
	char sz[1024];
	GetWindowText(hwnd, sz, 1023);
	SetWindowText(hwnd, TRL(sz));
	return TRUE;
}
