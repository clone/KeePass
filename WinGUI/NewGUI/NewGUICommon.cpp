/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2012 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "../Util/WinUtil.h"
#include <algorithm>
#include <gdiplus.h>

#include "../../KeePassLibCpp/Util/PwUtil.h"
#include "../../KeePassLibCpp/Util/StrUtil.h"
#include "../../KeePassLibCpp/Util/MemUtil.h"
#include "../../KeePassLibCpp/Util/AppUtil.h"

static bool g_gdiplusInitialized = false;
static ULONG_PTR g_gdiplusToken = 0;

static BOOL g_bImgButtons = 0;
static CThemeHelperST* g_pThemeHelper = NULL;

static COLORREF m_crBannerStart = RGB(151, 154, 173); // = RGB(235, 235, 255);
static COLORREF m_crBannerEnd = RGB(27, 27, 37); // = RGB(192, 192, 255);
static COLORREF m_crBannerText = RGB(255, 255, 255); // RGB(0, 0, 0);
static bool m_bBannerFlip = true; // = false;
static CFont* m_pfBannerTitleFont = NULL;

void NewGUI_CleanUp()
{
	if(m_pfBannerTitleFont != NULL)
	{
		m_pfBannerTitleFont->DeleteObject();
		SAFE_DELETE(m_pfBannerTitleFont);
	}
}

void NewGUI_SetWin32Banner()
{
	m_crBannerStart = RGB(235, 235, 255);
	m_crBannerEnd = RGB(192, 192, 255);
	m_crBannerText = RGB(0, 0, 0);
	m_bBannerFlip = false;
}

COLORREF NewGUI_GetBgColor()
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

COLORREF NewGUI_GetBtnColor()
{
	return GetSysColor(COLOR_BTNFACE);
}

void NewGUI_SetImgButtons(BOOL bImageButtons)
{
	ASSERT((bImageButtons == TRUE) || (bImageButtons == FALSE));
	g_bImgButtons = bImageButtons;
}

/* void NewGUI_Button(void *pButton, int nBitmapIn, int nBitmapOut, BOOL bForceImage)
{
	CButtonST *p = (CButtonST *)pButton;
	ASSERT(p != NULL); if(p == NULL) return;

	p->SetFlat(FALSE);

	p->SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0, 0, 0), TRUE);
	p->SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 0, 0), TRUE);

	if((g_bImgButtons == FALSE) && (bForceImage == FALSE)) return;

	if((nBitmapIn != -1) && (nBitmapOut != -1))
		p->SetBitmaps(nBitmapIn, RGB(255, 0, 255), nBitmapOut, RGB(255, 0, 255));
} */

void NewGUI_SetThemeHelper(void *pThemeHelper)
{
	g_pThemeHelper = (CThemeHelperST *)pThemeHelper;

	if(g_pThemeHelper != NULL)
	{
		if(g_pThemeHelper->IsAppThemed() == FALSE)
			g_pThemeHelper = NULL;
	}
}

void NewGUI_XPButton(CXPStyleButtonST& rButton, int nBitmapIn,
	int nBitmapOut, BOOL bForceImage)
{
	if(g_pThemeHelper != NULL) rButton.SetFlat(TRUE);
	else rButton.SetFlat(FALSE);

	if(!((g_bImgButtons == FALSE) && (bForceImage == FALSE)))
	{
		if((nBitmapIn != -1) && (nBitmapOut != -1))
			rButton.SetBitmaps(nBitmapIn, RGB(255, 0, 255),
				nBitmapOut, RGB(255, 0, 255));
	}

	if(g_pThemeHelper != NULL) rButton.SetThemeHelper(g_pThemeHelper);
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
COLORREF NewGUI_LightenColor(COLORREF crColor, double dblFactor)
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

void NewGUI_ToolBarButton(void *pButton, int nBitmapIn, int nBitmapOut)
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

void NewGUI_TranslateCWnd(CWnd *pWnd)
{
	CString str;
	ASSERT(pWnd != NULL); if(pWnd == NULL) return;
	pWnd->GetWindowText(str);
	pWnd->SetWindowText(TRL_VAR(str));
}

BOOL CALLBACK NewGUI_TranslateWindowCb(HWND hwnd, LPARAM lParam)
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

void NewGUI_ConfigQualityMeter(void *pWnd)
{
	CGradientProgressCtrl *p = (CGradientProgressCtrl *)pWnd;

	ASSERT(p != NULL); if(p == NULL) return;
	p->SetStartColor(RGB(255, 128, 0));
	p->SetEndColor(RGB(0, 255, 0));
	p->SetStep(1);
	p->SetRange(0, 128);
	p->SetPos(0);
}

void NewGUI_ShowQualityMeter(void *pProgressBar, void *pStaticDesc, LPCTSTR pszPassword)
{
	CGradientProgressCtrl *pProgress = (CGradientProgressCtrl *)pProgressBar;
	CStatic *pStatic = (CStatic *)pStaticDesc;

	ASSERT(pProgress != NULL); if(pProgress == NULL) return;
	ASSERT(pStatic != NULL); if(pStatic == NULL) return;

	DWORD dwBits = CPwUtil::EstimatePasswordBits(pszPassword);
	if(dwBits > 9999) dwBits = 9999; // 4 characters display limit

	CString strQuality;
	strQuality.Format(_T("%u"), dwBits);
	strQuality += _T(" ");
	strQuality += TRL("bits");
	pStatic->SetWindowText((LPCTSTR)strQuality);
	if(dwBits > 128) dwBits = 128;
	pProgress->SetPos((int)dwBits);
}

void NewGUI_ConfigSideBanner(void *pBanner, void *pParentWnd)
{
	CKCSideBannerWnd *p = (CKCSideBannerWnd *)pBanner;
	CWnd *pParent = (CWnd *)pParentWnd;
	ASSERT(pBanner != NULL); if(pBanner == NULL) return;
	ASSERT(pParentWnd != NULL); if(pParentWnd == NULL) return;

	p->Attach(pParent, KCSB_ATTACH_TOP);

	const int nSize = p->GetSize();
	const int nNewSize = NewGUI_Scale(nSize, pParent);
	if(nNewSize != nSize) p->SetSize(nNewSize);

	const CSize sEdgeOffset = p->GetEdgeOffset();
	CSize sNewEdgeOffset = NewGUI_Scale(sEdgeOffset, pParent);
	if(sNewEdgeOffset != sEdgeOffset) p->SetEdgeOffset(sNewEdgeOffset);

	const CSize sCaptionOffset = p->GetCaptionOffset();
	CSize sNewCaptionOffset = NewGUI_Scale(sCaptionOffset, pParent);
	if(sNewCaptionOffset != sCaptionOffset) p->SetCaptionOffset(sNewCaptionOffset);

	p->SetColBkg(m_crBannerStart);
	p->SetColBkg2(m_crBannerEnd);

	p->SetColTxtTitle(m_crBannerText);
	p->SetColTxtCaption(m_crBannerText);

	p->SetColEdge(RGB(0, 0, 0));

	if(m_bBannerFlip) p->SetSwapGradientDirection(true);

	if(m_pfBannerTitleFont != NULL) p->SetTitleFont(m_pfBannerTitleFont);
	else
	{
		LOGFONT lf;
		ZeroMemory(&lf, sizeof(LOGFONT));
		p->GetTitleFont(&lf);
		lf.lfWeight = FW_BOLD;

		m_pfBannerTitleFont = new CFont();
		VERIFY(m_pfBannerTitleFont->CreateFontIndirect(&lf));

		p->SetTitleFont(m_pfBannerTitleFont);
	}
}

BOOL NewGUI_GetHeaderOrder(HWND hwListCtrl, INT *pOrder, INT nColumnCount)
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

BOOL NewGUI_SetHeaderOrder(HWND hwListCtrl, INT *pOrder, INT nColumnCount)
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

void NewGUI_MakeHyperLink(void *pXHyperLink)
{
	CXHyperLink *p = reinterpret_cast<CXHyperLink *>(pXHyperLink);
	ASSERT(p != NULL); if(p == NULL) return;

	p->SetVisited(FALSE);
	p->SetAutoSize(TRUE);
	p->SetUnderline(CXHyperLink::ulAlways);
	p->SetColours(RGB(0,0,255), RGB(0,0,255), RGB(100,100,255));
}

void NewGUI_DisableHideWnd(CWnd *pWnd)
{
	ASSERT(pWnd != NULL); if(pWnd == NULL) return;

	pWnd->EnableWindow(FALSE);
	pWnd->ShowWindow(SW_HIDE);
}

void NewGUI_MoveWnd(CWnd *pWnd, long lMoveRightPixels, long lMoveDownPixels,
	CWnd *pParent)
{
	ASSERT(pWnd != NULL); if(pWnd == NULL) return;
	ASSERT(pParent != NULL); if(pParent == NULL) return;

	RECT rect;
	pWnd->GetWindowRect(&rect);
	pParent->ScreenToClient(&rect);

	rect.left += lMoveRightPixels;
	rect.right += lMoveRightPixels;
	rect.top += lMoveDownPixels;
	rect.bottom += lMoveDownPixels;

	pWnd->MoveWindow(&rect);
}

void NewGUI_Resize(CWnd *pWnd, long lAddX, long lAddY, CWnd *pParent)
{
	ASSERT(pWnd != NULL); if(pWnd == NULL) return;

	if(pParent == pWnd) { ASSERT(FALSE); pParent = NULL; }

	RECT rect;
	pWnd->GetWindowRect(&rect);

	if(pParent != NULL) pParent->ScreenToClient(&rect);

	rect.right += lAddX;
	rect.bottom += lAddY;

	pWnd->MoveWindow(&rect);
}

void NewGUI_SetBannerColors(COLORREF crStart, COLORREF crEnd, COLORREF crText)
{
	ASSERT(sizeof(COLORREF) == sizeof(DWORD));

	if(crStart != DWORD_MAX)
		m_crBannerStart = crStart;
	if(crEnd != DWORD_MAX)
		m_crBannerEnd = crEnd;
	if(crText != DWORD_MAX)
		m_crBannerText = crText;
}

void NewGUI_SetBannerParams(bool bBannerFlip)
{
	m_bBannerFlip = bBannerFlip;
}

BOOL NewGUI_RemoveMenuCommand(BCMenu *pMenu, UINT uCommandID)
{
	ASSERT(pMenu != NULL); if(pMenu == NULL) return FALSE;

	return pMenu->DeleteMenu(uCommandID, MF_BYCOMMAND);
}

void NewGUI_RemoveInvalidSeparators(BCMenu *pMenu, BOOL bIsTopLevel)
{
	ASSERT(pMenu != NULL); if(pMenu == NULL) return;

	int nSub = 0;
	while(true)
	{
		BCMenu *pSub = NewGUI_GetBCMenu(pMenu->GetSubMenu(nSub));
		if(pSub == NULL) break;

		NewGUI_RemoveInvalidSeparators(pSub, FALSE);
		++nSub;
	}

	BOOL bSepAllowed = FALSE;
	UINT uCount = pMenu->GetMenuItemCount();
	for(UINT uItem = 0; uItem < uCount; ++uItem)
	{
		if(uItem == (uCount - 1)) bSepAllowed = FALSE;

		const UINT uState = pMenu->GetMenuState(uItem, MF_BYPOSITION);
		if((bIsTopLevel == FALSE) && ((uState & MF_SEPARATOR) != 0) &&
			((uState & MF_POPUP) == 0))
		{
			if(bSepAllowed == FALSE)
			{
				pMenu->DeleteMenu(uItem, MF_BYPOSITION);
				--uCount;
				--uItem;

				if((uItem == (uCount - 1)) && (uItem >= 1)) --uItem;
			}

			bSepAllowed = FALSE;
		}
		else bSepAllowed = TRUE;
	}
}

BCMenu *NewGUI_GetBCMenu(CMenu *pMenu)
{
	if(pMenu == NULL) return NULL; // No assert

	BCMenu *p = NULL;
	try { p = dynamic_cast<BCMenu *>(pMenu); }
	catch(...) { ASSERT(FALSE); p = NULL; }

	return p;
}

CSize NewGUI_GetWndBasePosDiff(CWnd *pWnd1, CWnd *pWnd2)
{
	ASSERT(pWnd1 != NULL); if(pWnd1 == NULL) return CSize(0, 0);
	ASSERT(pWnd2 != NULL); if(pWnd2 == NULL) return CSize(0, 0);

	RECT rect1, rect2;
	pWnd1->GetWindowRect(&rect1);
	pWnd2->GetWindowRect(&rect2);

	return CSize(abs(rect1.left - rect2.left), abs(rect1.top - rect2.top));
}

void NewGUI_SetCueBanner_TB(HWND hTextBox, LPCTSTR lpText)
{
	ASSERT(lpText != NULL); if(lpText == NULL) return;

	// On Windows XP there's a drawing bug at the left border (text is
	// not displayed correctly), therefore prepend a space on Windows XP
	CString strSearchTr = ((AU_IsAtLeastWinVistaSystem() == FALSE) ? _T(" ") : _T(""));
	strSearchTr += lpText;

#ifndef _UNICODE
	LPCWSTR pSearchUni = _StringToUnicode(strSearchTr);
#else // Unicode
	LPCWSTR pSearchUni = strSearchTr;
#endif

	::SendMessage(hTextBox, EM_SETCUEBANNER, 0, (LPARAM)pSearchUni);

#ifndef _UNICODE
	SAFE_DELETE_ARRAY(pSearchUni);
#endif
}

void NewGUI_SetCueBanner_CB(HWND hComboBox, LPCTSTR lpText)
{
	HWND hTextBox = NULL;
	NewGUI_ComboBox_GetInfo(hComboBox, NULL, &hTextBox, NULL);
	NewGUI_SetCueBanner_TB(hTextBox, lpText);
}

void NewGUI_ComboBox_GetInfo(HWND hComboBox, HWND* phComboBox,
	HWND* phEditBox, HWND* phListBox)
{
	COMBOBOXINFO cbi;
	ZeroMemory(&cbi, sizeof(COMBOBOXINFO));
	cbi.cbSize = sizeof(COMBOBOXINFO);
	VERIFY(GetComboBoxInfo(hComboBox, &cbi));

	if(phComboBox != NULL) *phComboBox = cbi.hwndCombo;
	if(phEditBox != NULL) *phEditBox = cbi.hwndItem;
	if(phListBox != NULL) *phListBox = cbi.hwndList;
}

bool NewGUI_ComboBox_HasFocus(HWND hComboBox, HWND hCurrentFocus)
{
	HWND hCombo = NULL;
	HWND hTextBox = NULL;
	HWND hListBox = NULL;
	NewGUI_ComboBox_GetInfo(hComboBox, &hCombo, &hTextBox, &hListBox);

	return ((hCombo == hCurrentFocus) || (hTextBox == hCurrentFocus) ||
		(hListBox == hCurrentFocus));
}

void NewGUI_ComboBox_UpdateHistory(CComboBox& comboBox,
	const std::basic_string<TCHAR>& strNew,
	std::vector<std::basic_string<TCHAR> >* pvHistoryItems,
	size_t dwMaxHistoryItems)
{
	ASSERT(pvHistoryItems != NULL); if(pvHistoryItems == NULL) return;

	if(strNew.size() > 0)
	{
		std::vector<std::basic_string<TCHAR> >::iterator itExists =
			std::find(pvHistoryItems->begin(), pvHistoryItems->end(), strNew);

		if(itExists != pvHistoryItems->end())
			pvHistoryItems->erase(itExists);

		size_t dwCurSize = pvHistoryItems->size();
		ASSERT(dwCurSize <= dwMaxHistoryItems);
		while(dwCurSize >= dwMaxHistoryItems)
		{
			pvHistoryItems->erase(pvHistoryItems->begin());

			if(dwCurSize == pvHistoryItems->size()) { ASSERT(FALSE); break; }
			dwCurSize = pvHistoryItems->size();
		}

		pvHistoryItems->push_back(strNew);
	}

	ASSERT(pvHistoryItems->size() <= dwMaxHistoryItems);

	const int nOrgCount = comboBox.GetCount();
	for(int n = 0; n < nOrgCount; ++n)
	{
		const UINT uIndex = static_cast<UINT>(nOrgCount - n - 1);
		VERIFY(comboBox.DeleteString(uIndex) != CB_ERR);
	}

	for(size_t i = 0; i < pvHistoryItems->size(); ++i)
	{
		const size_t iIndex = pvHistoryItems->size() - i - 1;
		comboBox.AddString(pvHistoryItems->at(iIndex).c_str());
	}

	if(comboBox.GetCount() > 0)
	{
		comboBox.AddString(HCMBX_SEPARATOR);
		comboBox.AddString(HCMBX_CLEARLIST);
	}
}

BOOL NewGUI_SetIcon(BCMenu& rMenu, UINT uCommand, int nResourceID)
{
	CString strMenuItemText;
	if(rMenu.GetMenuText(uCommand, strMenuItemText, MF_BYCOMMAND) == FALSE)
		return FALSE;
	if(strMenuItemText.GetLength() == 0) return FALSE;

	if(rMenu.ModifyODMenu(NULL, uCommand, nResourceID) == FALSE)
		return FALSE;

	return TRUE;
}

int CALLBACK NgslCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	std::pair<CListCtrl*, int>* pParam = (std::pair<CListCtrl*, int>*)lParamSort;
	if(pParam == NULL) { ASSERT(FALSE); return 0; }

	CListCtrl* pCtrl = pParam->first;

	ASSERT(pCtrl->GetHeaderCtrl()->GetItemCount() == pParam->second);
	for(int iColumn = 0; iColumn < pParam->second; ++iColumn)
	{
		CString x = pCtrl->GetItemText((int)lParam1, iColumn);
		CString y = pCtrl->GetItemText((int)lParam2, iColumn);

		const int nCmp = x.Compare(y);
		if(nCmp != 0) return nCmp;
	}

	return 0;
}

void NewGUI_SortList(CListCtrl* pListCtrl)
{
	if(pListCtrl == NULL) { ASSERT(FALSE); return; }

	std::pair<CListCtrl*, int> spParam(pListCtrl,
		pListCtrl->GetHeaderCtrl()->GetItemCount());

	VERIFY(ListView_SortItemsEx(pListCtrl->m_hWnd, NgslCompareFunc,
		(LPARAM)&spParam) != FALSE);
}

void NewGUI_AppendToRichEditCtrl(CAutoRichEditCtrlFx* pCtrl, LPCTSTR lpAppend,
	bool bScrollToBottom)
{
	if(pCtrl == NULL) { ASSERT(FALSE); return; }
	if(lpAppend == NULL) { ASSERT(FALSE); return; }
	if(lpAppend[0] == 0) return;

	CString strText;
	pCtrl->GetWindowText(strText);
	if(strText.GetLength() > 0) strText += _T("\r\n\r\n");

	strText += lpAppend;

	pCtrl->SetRTF(strText, SF_TEXT);

	if(bScrollToBottom) pCtrl->SendMessage(WM_VSCROLL, SB_BOTTOM, NULL);
}

void NewGUI_DeselectAllItems(CListCtrl* pCtrl)
{
	if(pCtrl == NULL) { ASSERT(FALSE); return; }

	for(int i = 0; i < pCtrl->GetItemCount(); ++i)
	{
		const UINT uState = pCtrl->GetItemState(i, LVIS_SELECTED);
		if((uState & LVIS_SELECTED) != 0)
		{
			VERIFY(pCtrl->SetItemState(i, 0, LVIS_SELECTED));
		}
	}
}

void NewGUI_EnableSortHeader(CListCtrl* pCtrl, bool bEnable)
{
	if(pCtrl == NULL) { ASSERT(FALSE); return; }

	CHeaderCtrl* pH = pCtrl->GetHeaderCtrl();
	if(pH == NULL) { ASSERT(FALSE); return; }
	HWND h = pH->m_hWnd;
	if(h == NULL) { ASSERT(FALSE); return; }

	const LONG_PTR lStyle = GetWindowLongPtr(h, GWL_STYLE);
	const bool bCur = ((lStyle & HDS_BUTTONS) != 0);

	if(bCur != bEnable)
		SetWindowLongPtr(h, GWL_STYLE, lStyle ^ HDS_BUTTONS);
}

void NewGUI_SetShield(CButton& rButton, BOOL bSetShield)
{
	rButton.SendMessage(BCM_SETSHIELD, 0, bSetShield);
}

int NewGUI_Scale(int nPixels, CWnd* pParentWindow)
{
	if(pParentWindow == NULL) { ASSERT(FALSE); return nPixels; }

	CDC* pDC = pParentWindow->GetDC();
	CFont* pFont = pParentWindow->GetFont();
	if((pDC == NULL) || (pFont == NULL)) { ASSERT(FALSE); return nPixels; }

	const double dDpi = static_cast<double>(GetDeviceCaps(pDC->m_hDC, LOGPIXELSY));
	const double dScaleDpi = (dDpi / 96.0);

	LOGFONT lf;
	ZeroMemory(&lf, sizeof(LOGFONT));
	if(pFont->GetLogFont(&lf) == 0) { ASSERT(FALSE); return nPixels; }

	const double dy = fabs((72.0 * static_cast<double>(lf.lfHeight)) / dDpi);
	const double dScaleFont = (dy / 8.25);

	return static_cast<int>(floor((static_cast<double>(nPixels) *
		dScaleDpi * dScaleFont) + 0.5));
}

CSize NewGUI_Scale(const CSize& rSize, CWnd* pParentWindow)
{
	CSize sNew;
	sNew.cx = NewGUI_Scale(rSize.cx, pParentWindow);
	sNew.cy = NewGUI_Scale(rSize.cy, pParentWindow);
	return sNew;
}

COLORREF NewGUI_ColorToGrayscale(COLORREF clr)
{
	int l = static_cast<int>((0.3f * static_cast<float>(GetRValue(clr))) +
		(0.59f * static_cast<float>(GetGValue(clr))) +
		(0.11f * static_cast<float>(GetBValue(clr))));
	
	if(l < 0) l = 0;
	else if(l >= 256) l = 255;

	return RGB(l, l, l);
}

void NewGUI_EnableWindowPeekPreview(HWND hWnd, bool bEnable)
{
	// if(!bEnable) InvalidateRect(hWnd, NULL, TRUE);

	HMODULE hDwm = LoadLibrary(DWMAPI_LIB_NAME);
	if(hDwm == NULL) return;

	LPDWMSETWINDOWATTRIBUTE lpDwmSetWindowAttribute = (LPDWMSETWINDOWATTRIBUTE)
		GetProcAddress(hDwm, DWMAPI_SETWINDOWATTRIBUTE);
	if(lpDwmSetWindowAttribute != NULL)
	{
		BOOL bDisallow = (bEnable ? FALSE : TRUE);
		BOOST_STATIC_ASSERT(sizeof(BOOL) == 4);
		lpDwmSetWindowAttribute(hWnd, DWMWA_DISALLOW_PEEK, &bDisallow, sizeof(BOOL));
	}

	VERIFY(FreeLibrary(hDwm));
}

void NewGUI_InitGDIPlus()
{
	Gdiplus::GdiplusStartupInput si;
	if(Gdiplus::GdiplusStartup(&g_gdiplusToken, &si, NULL) == Gdiplus::Ok)
		g_gdiplusInitialized = true;
}

void NewGUI_TerminateGDIPlus()
{
	if(g_gdiplusInitialized)
	{
		Gdiplus::GdiplusShutdown(g_gdiplusToken);
		g_gdiplusInitialized = false;
	}
}
