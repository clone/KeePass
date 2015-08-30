/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2015 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "../Resource.h"
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
	HDC hDC = ::GetDC(NULL);
	const int nBitsPerPixel = GetDeviceCaps(hDC, BITSPIXEL);
	::ReleaseDC(NULL, hDC);
	m_bColorize = ((nBitsPerPixel > 8) ? TRUE : FALSE);

	m_rgbRowColor = RGB(238, 238, 255);

	m_pParentI = NULL;
	m_pbShowColumns = NULL;

	m_bRedrawingEnabled = true;
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
	if(pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if(pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		// This is the prepaint stage for a subitem. Here's where we set the
		// item's text and background colors. Our return value will tell 
		// Windows to draw the subitem itself, but it will use the new colors
		// we set here.

		crText = RGB(0, 0, 0);
		if(m_bColorize == TRUE) // Colorize the list only if enough colors are available
		{
			if((pLVCD->nmcd.dwItemSpec & 1) != 0) // Uneven item number
				crBkgnd = m_rgbRowColor;
			else // Even item number
				crBkgnd = RGB(255, 255, 255);
		}
		else crBkgnd = RGB(255, 255, 255);

		if((pLVCD->nmcd.lItemlParam & CLCIF_HIGHLIGHT_GREEN) != 0)
			crText = RGB(0, 128, 0);

		// Store the colors into the NMLVCUSTOMDRAW struct
		pLVCD->clrText = crText;
		pLVCD->clrTextBk = crBkgnd;
	}
}

BOOL CCustomListCtrlEx::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &= ~(LVS_TYPEMASK | LVS_SHOWSELALWAYS);
	cs.style |= (LVS_REPORT | LVS_OWNERDRAWFIXED);

	return CListCtrl::PreCreateWindow(cs);
}

COLORREF CCustomListCtrlEx::GetRowColorEx()
{
	return m_rgbRowColor;
}

void CCustomListCtrlEx::SetRowColorEx(COLORREF rgbColor)
{
	m_rgbRowColor = rgbColor;
	RedrawItems(0, GetItemCount() - 1);
	UpdateWindow();
}

void CCustomListCtrlEx::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// Parent must be initialized first
	ASSERT(m_pParentI != NULL); if(m_pParentI == NULL) return;
	((CPwSafeDlg *)m_pParentI)->NotifyUserActivity();

	if((nFlags & 0x2000) != 0)
	{
		if((nChar == VK_UP) || (nChar == VK_DOWN) || (nChar == VK_HOME) || (nChar == VK_END))
			((CPwSafeDlg *)m_pParentI)->_ProcessListKey(nChar, TRUE);
		else CListCtrl::OnSysKeyDown(nChar, nRepCnt, nFlags);
	}
	else CListCtrl::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CCustomListCtrlEx::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *)lParam;

	// Parent must be initialized first
	ASSERT((m_pParentI != NULL) && (m_pbShowColumns != NULL));
	if((m_pParentI == NULL) || (m_pbShowColumns == NULL))
		return CListCtrl::OnNotify(wParam, lParam, pResult);

	if((phdn->hdr.code == HDN_ITEMCHANGEDW) || (phdn->hdr.code == HDN_ITEMCHANGEDA))
	{
		ASSERT(phdn->pitem != NULL);

		// Track only width changes
		if (phdn->pitem->mask & HDI_WIDTH)
			((CPwSafeDlg *)m_pParentI)->CB_OnPwlistColumnWidthChange(phdn->iItem, phdn->pitem->cxy);
	}
	else if((phdn->hdr.code == HDN_BEGINTRACKW) || (phdn->hdr.code == HDN_BEGINTRACKA))
	{
		int nItem = phdn->iItem;

		if(nItem >= 0)
		{
			if(m_pbShowColumns[nItem] == FALSE)
			{
				*pResult = TRUE;
				return TRUE;
			}
		}
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

	if((nChar == VK_DELETE) && ((nFlags & 0x2000) == 0))
		((CPwSafeDlg *)m_pParentI)->_ProcessListKey(VK_DELETE, FALSE);
	else
		((CPwSafeDlg *)m_pParentI)->NotifyUserActivity();

	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CCustomListCtrlEx::FocusItem(int iIndex, BOOL bAlsoSelect)
{
	ASSERT(iIndex >= 0); if(iIndex < 0) return;

	const int nItemCount = this->GetItemCount();
	if(iIndex >= nItemCount) return; // No assert

	for(int i = nItemCount - 1; i >= 0; --i)
	{
		ASSERT((i >= 0) && (i < nItemCount));
		if(i == iIndex) continue;

		const UINT uState = this->GetItemState(i, LVIS_FOCUSED);
		if(uState != 0) this->SetItemState(i, 0, LVIS_FOCUSED);
	}

	const UINT uItemState = this->GetItemState(iIndex, LVIS_FOCUSED | LVIS_SELECTED);

	if((bAlsoSelect == TRUE) && ((uItemState & LVIS_SELECTED) == 0))
		this->SetItemState(iIndex, LVIS_SELECTED, LVIS_SELECTED);

	if((uItemState & LVIS_FOCUSED) == 0)
		this->SetItemState(iIndex, LVIS_FOCUSED, LVIS_FOCUSED);
}

bool CCustomListCtrlEx::LockRedrawEx(bool bBlockRedrawing)
{
	return this->LockRedrawEx(bBlockRedrawing, false);
}

bool CCustomListCtrlEx::LockRedrawEx(bool bBlockRedrawing, bool bFullInvalidate)
{
	if(bBlockRedrawing && m_bRedrawingEnabled)
	{
		m_bRedrawingEnabled = false;
		this->SetRedraw(FALSE);
		return true;
	}
	else if(!bBlockRedrawing && !m_bRedrawingEnabled)
	{
		m_bRedrawingEnabled = true;
		this->SetRedraw(TRUE);
		this->Invalidate(bFullInvalidate ? TRUE : FALSE);
		return true;
	}

	return false;
}

void CCustomListCtrlEx::DeleteAllItemsEx()
{
	if(this->GetItemCount() == 0) return;

	const bool bLocked = this->LockRedrawEx(true);
	VERIFY(this->DeleteAllItems());
	if(bLocked) { VERIFY(this->LockRedrawEx(false)); }
}
