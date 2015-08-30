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

#include "stdafx.h"
#include "../resource.h"
#include "CustomListCtrlEx.h"
#include "../PwSafeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CCustomListCtrlEx::CCustomListCtrlEx()
{
	HDC hDC = NULL;
	int nBitsPerPixel = 0;

	hDC = ::GetDC(NULL);
	nBitsPerPixel = GetDeviceCaps(hDC, BITSPIXEL);
	::ReleaseDC(NULL, hDC);

	if(nBitsPerPixel > 8) m_bColorize = TRUE;
	else m_bColorize = FALSE;

	m_rgbColor = RGB(238,238,255);

	m_pParentI = NULL;
}

CCustomListCtrlEx::~CCustomListCtrlEx()
{
}

BEGIN_MESSAGE_MAP(CCustomListCtrlEx, CListCtrl)
	//{{AFX_MSG_MAP(CCustomListCtrlEx)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_SYSKEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CCustomListCtrlEx::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAWX* pLVCD = reinterpret_cast<NMLVCUSTOMDRAWX*>(pNMHDR);
	COLORREF crText, crBkgnd;

	// Take the default processing unless we set this to something else below.
	*pResult = CDRF_DODEFAULT;

	// First thing - check the draw stage. If it's the control's prepaint
	// stage, then tell Windows we want messages for every item.
	if(CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if(pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		// This is the prepaint stage for a subitem. Here's where we set the
		// item's text and background colors. Our return value will tell 
		// Windows to draw the subitem itself, but it will use the new colors
		// we set here.

		if(m_bColorize == TRUE) // Colorize the list only if enough colors are avail
		{
			if(pLVCD->nmcd.dwItemSpec & 1)
			{
				crText = RGB(0,0,0);
				crBkgnd = m_rgbColor;
			}
			else
			{
				crText = RGB(0,0,0);
				crBkgnd = RGB(255,255,255);
			}
		}
		else
		{
			crText = RGB(0,0,0);
			crBkgnd = RGB(255,255,255);
		}

		// Store the colors back in the NMLVCUSTOMDRAW struct.
		pLVCD->clrText = crText;
		pLVCD->clrTextBk = crBkgnd;
	}
}

BOOL CCustomListCtrlEx::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &= ~LVS_TYPEMASK;
	cs.style &= ~LVS_SHOWSELALWAYS;
	cs.style |= LVS_REPORT | LVS_OWNERDRAWFIXED;

	return CListCtrl::PreCreateWindow(cs);
}

COLORREF CCustomListCtrlEx::GetColorEx()
{
	return m_rgbColor;
}

void CCustomListCtrlEx::SetColorEx(COLORREF rgbColor)
{
	m_rgbColor = rgbColor;
	RedrawItems(0, GetItemCount() - 1);
	UpdateWindow();
}

void CCustomListCtrlEx::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// Parent must be initialized first
	ASSERT(m_pParentI != NULL); if(m_pParentI == NULL) return;
	((CPwSafeDlg *)m_pParentI)->NotifyUserActivity();

	if(nFlags & 0x2000)
	{
		if(nChar == VK_UP) ((CPwSafeDlg *)m_pParentI)->_ProcessListKey(nChar);
		else if(nChar == VK_DOWN) ((CPwSafeDlg *)m_pParentI)->_ProcessListKey(nChar);
		else if(nChar == VK_HOME) ((CPwSafeDlg *)m_pParentI)->_ProcessListKey(nChar);
		else if(nChar == VK_END) ((CPwSafeDlg *)m_pParentI)->_ProcessListKey(nChar);
		else CListCtrl::OnSysKeyDown(nChar, nRepCnt, nFlags);
	}
	else
	{
		CListCtrl::OnSysKeyDown(nChar, nRepCnt, nFlags);
	}
}

BOOL CCustomListCtrlEx::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *)lParam;

	// Parent must be initialized first
	ASSERT(m_pParentI != NULL);
	if(m_pParentI == NULL) return CListCtrl::OnNotify(wParam, lParam, pResult);

	if((phdn->hdr.code == HDN_ITEMCHANGEDW) || (phdn->hdr.code == HDN_ITEMCHANGEDA))
	{
		ASSERT(phdn->pitem != NULL);

		// Track only width changes
		if (phdn->pitem->mask & HDI_WIDTH)
			((CPwSafeDlg *)m_pParentI)->_OnPwlistColumnWidthChange(phdn->iItem, phdn->pitem->cxy);
	}

	return CListCtrl::OnNotify(wParam, lParam, pResult);
}

void CCustomListCtrlEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	ASSERT(m_pParentI != NULL); if(m_pParentI == NULL) return;
	((CPwSafeDlg *)m_pParentI)->NotifyUserActivity();

	CListCtrl::OnMouseMove(nFlags, point);
}

void CCustomListCtrlEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	ASSERT(m_pParentI != NULL); if(m_pParentI == NULL) return;
	((CPwSafeDlg *)m_pParentI)->NotifyUserActivity();

	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}
