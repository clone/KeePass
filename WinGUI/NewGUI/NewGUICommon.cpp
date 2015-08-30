/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2006 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "NewGUICommon.h"
#include "BCMenu.h"
#include "BtnST.h"
#include "../../KeePassLibCpp/Util/TranslateEx.h"
#include "GradientProgressCtrl.h"
#include "KCSideBannerWnd.h"
#include "XHyperLink.h"
#include "XPStyleButtonST.h"

#include "../../KeePassLibCpp/Util/PwUtil.h"
#include "../../KeePassLibCpp/Util/StrUtil.h"

static BOOL g_bImgButtons = 0;
static CThemeHelperST *g_pThemeHelper = NULL;

C_FN_SHARE COLORREF NewGUI_GetBgColor()
{
	// HDC hDC = NULL;
	// int nBitsPerPixel = 0;

	// hDC = GetDC(NULL);
	// nBitsPerPixel = GetDeviceCaps(hDC, BITSPIXEL);
	// ReleaseDC(NULL, hDC);

	// if(nBitsPerPixel <= 8) return GetSysColor(COLOR_BTNFACE);

	// return(CR_BACK);

	return GetSysColor(COLOR_BTNFACE);
}

C_FN_SHARE COLORREF NewGUI_GetBtnColor()
{
	COLORREF clr;

	clr = GetSysColor(COLOR_BTNFACE);

	// return clr + 10;
	return clr;
}

C_FN_SHARE void NewGUI_SetImgButtons(BOOL bImageButtons)
{
	ASSERT((bImageButtons == TRUE) || (bImageButtons == FALSE));
	g_bImgButtons = bImageButtons;
}

C_FN_SHARE void NewGUI_Button(void *pButton, int nBitmapIn, int nBitmapOut, BOOL bForceImage)
{
	CButtonST *p = (CButtonST *)pButton;

	ASSERT(p != NULL); if(p == NULL) return;

	p->SetFlat(FALSE);

	p->SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0, 0, 0), TRUE);
	p->SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 0, 0), TRUE);

	if((g_bImgButtons == FALSE) && (bForceImage == FALSE)) return;

	if((nBitmapIn != -1) && (nBitmapOut != -1))
		p->SetBitmaps(nBitmapIn, RGB(255, 0, 255), nBitmapOut, RGB(255, 0, 255));
}

C_FN_SHARE void NewGUI_SetThemeHelper(void *pThemeHelper)
{
	ASSERT(pThemeHelper != NULL);
	g_pThemeHelper = (CThemeHelperST *)pThemeHelper;

	if(g_pThemeHelper != NULL)
	{
		if(g_pThemeHelper->IsAppThemed() == FALSE)
			g_pThemeHelper = NULL;
	}
}

C_FN_SHARE void NewGUI_XPButton(void *pButton, int nBitmapIn, int nBitmapOut, BOOL bForceImage)
{
	CXPStyleButtonST *p = (CXPStyleButtonST *)pButton;

	ASSERT(p != NULL); if(p == NULL) return;

	if(g_pThemeHelper != NULL) p->SetFlat(TRUE);
	else p->SetFlat(FALSE);

	if(!((g_bImgButtons == FALSE) && (bForceImage == FALSE)))
	{
		if((nBitmapIn != -1) && (nBitmapOut != -1))
			p->SetBitmaps(nBitmapIn, RGB(255, 0, 255), nBitmapOut, RGB(255, 0, 255));
	}

	if(g_pThemeHelper != NULL) p->SetThemeHelper(g_pThemeHelper);
}

/* Old shade button code:

	CShadeButtonST *p = (CShadeButtonST *)pButton;

	p->DrawFlatFocus(TRUE);
	p->SetFlat(FALSE);

	if(g_bImgButtons == TRUE)
		p->SetShade(CShadeButtonST::SHS_DIAGSHADE);
	else
		p->SetShade(CShadeButtonST::SHS_HBUMP, 8, 0, 255, NewGUI_GetBtnColor());

	p->SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0, 0, 0), TRUE);
	p->SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 0, 0), TRUE);

	// Pre 0.86 buttons
	// p->SetFlat(TRUE);
	// p->SetShade(CShadeButtonST::SHS_SOFTBUMP);
	// p->SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0, 0, 0), TRUE);
	// p->SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 0, 255), TRUE);
	// p->DrawFlatFocus(TRUE);

	if((g_bImgButtons == FALSE) && (bForceImage == FALSE)) return;

	// if(g_bImgButtons == FALSE) return;

	if((nBitmapIn != -1) && (nBitmapOut != -1))
		p->SetBitmaps(nBitmapIn, RGB(255, 0, 255), nBitmapOut, RGB(255, 0, 255));
*/

/*
C_FN_SHARE COLORREF NewGUI_LightenColor(COLORREF crColor, double dblFactor)
{
	BYTE byRed, byGreen, byBlue, byLightRed, byLightGreen, byLightBlue;

	byRed = GetRValue(crColor);
	byGreen = GetGValue(crColor);
	byBlue = GetBValue(crColor);

	byLightRed = (BYTE)(dblFactor * (double)byRed);
	byLightGreen = (BYTE)(dblFactor * (double)byGreen);
	byLightBlue = (BYTE)(dblFactor * (double)byBlue);

	crColor = RGB(byLightRed, byLightGreen, byLightBlue);

	return crColor;
}
*/

C_FN_SHARE void NewGUI_ToolBarButton(void *pButton, int nBitmapIn, int nBitmapOut)
{
	CXPStyleButtonST *p = (CXPStyleButtonST *)pButton;
	CString strToolTip;

	ASSERT(p != NULL); if(p == NULL) return;

	p->SetFlat(TRUE);
	// p->DrawTransparent();

	if((nBitmapIn != -1) && (nBitmapOut != -1))
		p->SetBitmaps(nBitmapIn, RGB(255,0,255), nBitmapOut, RGB(255,0,255));

	p->GetWindowText(strToolTip);
	p->SetWindowText(_T(""));

	strToolTip = TRL_VAR(strToolTip);
	RemoveAcceleratorTip(&strToolTip);
	p->SetTooltipText(strToolTip);

	if(g_pThemeHelper == NULL)
	{
		COLORREF crHighlight;
		BYTE byRed, byGreen, byBlue;
		p->GetColor(CButtonST::BTNST_COLOR_BK_IN, &crHighlight);
		byRed = (BYTE)((GetRValue(crHighlight) < 240) ? GetRValue(crHighlight) + 15 : 255);
		byGreen = (BYTE)((GetGValue(crHighlight) < 240) ? GetGValue(crHighlight) + 15 : 255);
		byBlue = (BYTE)((GetBValue(crHighlight) < 240) ? GetBValue(crHighlight) + 15 : 255);
		crHighlight = RGB(byRed, byGreen, byBlue);
		p->SetColor(CButtonST::BTNST_COLOR_BK_IN, crHighlight, TRUE);
	}

	p->SetThemeHelper(g_pThemeHelper);
	p->DrawAsToolbar(TRUE);
}

CPP_FN_SHARE void NewGUI_TranslateCWnd(CWnd *pWnd)
{
	CString str;
	ASSERT(pWnd != NULL); if(pWnd == NULL) return;
	pWnd->GetWindowText(str);
	pWnd->SetWindowText(TRL_VAR(str));
}

C_FN_SHARE BOOL CALLBACK NewGUI_TranslateWindowCb(HWND hwnd, LPARAM lParam)
{
	TCHAR sz[512];

	UNREFERENCED_PARAMETER(lParam);
	ASSERT(hwnd != NULL);

	GetClassName(hwnd, sz, 16);
	if(_tcsicmp(sz, _T("Edit")) == 0) return TRUE;
	if(_tcsicmp(sz, _T("RICHEDIT")) == 0) return TRUE;
	if(_tcsicmp(sz, _T("ComboBox")) == 0) return TRUE;
	if(_tcsicmp(sz, _T("ComboBoxEx32")) == 0) return TRUE;

	sz[0] = 0; sz[1] = 0;
	if(GetWindowText(hwnd, sz, 511) != 0) SetWindowText(hwnd, TRL_VAR(sz));
	return TRUE;
}

C_FN_SHARE void NewGUI_ConfigQualityMeter(void *pWnd)
{
	CGradientProgressCtrl *p = (CGradientProgressCtrl *)pWnd;

	ASSERT(p != NULL); if(p == NULL) return;
	p->SetStartColor(RGB(255,128,0));
	p->SetEndColor(RGB(0,255,0));
	p->SetStep(1);
	p->SetRange(0, 128);
	p->SetPos(0);
}

C_FN_SHARE void NewGUI_ShowQualityMeter(void *pProgressBar, void *pStaticDesc, const TCHAR *pszPassword)
{
	CGradientProgressCtrl *pProgress = (CGradientProgressCtrl *)pProgressBar;
	CStatic *pStatic = (CStatic *)pStaticDesc;

	ASSERT(pProgress != NULL); if(pProgress == NULL) return;
	ASSERT(pStatic != NULL); if(pStatic == NULL) return;

	DWORD dwBits = EstimatePasswordBits(pszPassword);
	if(dwBits > 9999) dwBits = 9999; // 4 characters display limit

	CString strQuality;
	strQuality.Format(_T("%u"), dwBits);
	strQuality += _T(" ");
	strQuality += TRL("bits");
	pStatic->SetWindowText((LPCTSTR)strQuality);
	if(dwBits > 128) dwBits = 128;
	pProgress->SetPos((int)dwBits);
}

C_FN_SHARE void NewGUI_ConfigSideBanner(void *pBanner, void *pParentWnd)
{
	CKCSideBannerWnd *p = (CKCSideBannerWnd *)pBanner;
	CWnd *pParent = (CWnd *)pParentWnd;
	ASSERT(pBanner != NULL); if(pBanner == NULL) return;
	ASSERT(pParentWnd != NULL); if(pParentWnd == NULL) return;

	p->Attach(pParent, KCSB_ATTACH_TOP);

	// Original white-gray
	// p->SetColBkg(RGB(255,255,255));
	// p->SetColBkg2(NewGUI_GetBgColor());

	p->SetColBkg(RGB(235, 235, 255));
	p->SetColBkg2(RGB(192, 192, 255));

	p->SetColEdge(RGB(0,0,0));
}

C_FN_SHARE BOOL NewGUI_GetHeaderOrder(HWND hwListCtrl, INT *pOrder, INT nColumnCount)
{
	HWND hHeader;

	ASSERT(hwListCtrl != NULL); if(hwListCtrl == NULL) return FALSE;
	ASSERT(pOrder != NULL); if(pOrder == NULL) return FALSE;

	hHeader = (HWND)SendMessage(hwListCtrl, LVM_GETHEADER, 0, 0);
	ASSERT(hHeader != NULL); if(hHeader == NULL) return FALSE;

	if(SendMessage(hHeader, HDM_GETORDERARRAY, (WPARAM)nColumnCount, (LPARAM)pOrder) == FALSE)
	{
		ASSERT(FALSE); return FALSE;
	}

	return TRUE;
}

C_FN_SHARE BOOL NewGUI_SetHeaderOrder(HWND hwListCtrl, INT *pOrder, INT nColumnCount)
{
	HWND hHeader;

	ASSERT(hwListCtrl != NULL); if(hwListCtrl == NULL) return FALSE;
	ASSERT(pOrder != NULL); if(pOrder == NULL) return FALSE;

	hHeader = (HWND)SendMessage(hwListCtrl, LVM_GETHEADER, 0, 0);
	ASSERT(hHeader != NULL); if(hHeader == NULL) return FALSE;

	if(SendMessage(hHeader, HDM_SETORDERARRAY, (WPARAM)nColumnCount, (LPARAM)pOrder) == FALSE)
	{
		ASSERT(FALSE); return FALSE;
	}

	return TRUE;
}

C_FN_SHARE void NewGUI_MakeHyperLink(void *pXHyperLink)
{
	CXHyperLink *p = (CXHyperLink *)pXHyperLink;
	ASSERT(p != NULL); if(p == NULL) return;

	p->SetVisited(FALSE);
	p->SetAutoSize(TRUE);
	p->SetUnderline(CXHyperLink::ulAlways);
	p->SetColours(RGB(0,0,255), RGB(0,0,255), RGB(100,100,255));
}
