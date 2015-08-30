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
#include "../PwSafe.h"
#include "../resource.h"
#include "../PwSafeDlg.h"
#include "CustomTreeCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(push)
#pragma warning(disable: 4100)

/////////////////////////////////////////////////////////////////////////////
// CCustomTreeCtrlEx

CCustomTreeCtrlEx::CCustomTreeCtrlEx()
{
	m_pParentI = NULL;
	m_bValidDropTarget = FALSE;
}

CCustomTreeCtrlEx::~CCustomTreeCtrlEx()
{
	m_bValidDropTarget = FALSE;
}

BEGIN_MESSAGE_MAP(CCustomTreeCtrlEx, CTreeCtrl)
	//{{AFX_MSG_MAP(CCustomTreeCtrlEx)
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CCustomTreeCtrlEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// Parent must be initialized first
	ASSERT(m_pParentI != NULL); if(m_pParentI == NULL) return;
	((CPwSafeDlg *)m_pParentI)->NotifyUserActivity();

	if((nChar == VK_UP) | (nChar == VK_DOWN) | (nChar == VK_HOME) | (nChar == VK_END) |
		(nChar == VK_PRIOR) | (nChar == VK_NEXT) | (nChar == VK_LEFT) | (nChar == VK_RIGHT))
		((CPwSafeDlg *)m_pParentI)->_ProcessGroupKey(nChar, nFlags);

	// In this case let the framework handle the key because
	// it only sets flags and the framework must move the caret
	CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);

	if(((nChar >= 'A') && (nChar <= 'Z')) || ((nChar >= '0') && (nChar <= '9')))
		((CPwSafeDlg *)m_pParentI)->m_bCachedPwlistUpdate = TRUE;
}

void CCustomTreeCtrlEx::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// Parent must be initialized first
	ASSERT(m_pParentI != NULL); if(m_pParentI == NULL) return;
	((CPwSafeDlg *)m_pParentI)->NotifyUserActivity();

	if(nFlags & 0x2000)
	{
		if(nChar == VK_UP) ((CPwSafeDlg *)m_pParentI)->_ProcessGroupKey(nChar, nFlags);
		else if(nChar == VK_DOWN) ((CPwSafeDlg *)m_pParentI)->_ProcessGroupKey(nChar, nFlags);
		else if(nChar == VK_HOME) ((CPwSafeDlg *)m_pParentI)->_ProcessGroupKey(nChar, nFlags);
		else if(nChar == VK_END) ((CPwSafeDlg *)m_pParentI)->_ProcessGroupKey(nChar, nFlags);
		else if(nChar == VK_LEFT) ((CPwSafeDlg *)m_pParentI)->_ProcessGroupKey(nChar, nFlags);
		else if(nChar == VK_RIGHT) ((CPwSafeDlg *)m_pParentI)->_ProcessGroupKey(nChar, nFlags);
		else CTreeCtrl::OnSysKeyDown(nChar, nRepCnt, nFlags);
	}
	else
	{
		CTreeCtrl::OnSysKeyDown(nChar, nRepCnt, nFlags);
	}
}

BOOL CCustomTreeCtrlEx::InitDropHandler()
{
	if(m_bValidDropTarget == TRUE) { ASSERT(FALSE); return FALSE; } // Initialized already

	ASSERT(m_pParentI != NULL);
	m_drop.m_pTopParentI = m_pParentI;
	m_drop.m_pParentTree = this;

	m_bValidDropTarget = m_drop.Register(this);
	ASSERT(m_bValidDropTarget == TRUE);
	return m_bValidDropTarget;
}

void CCustomTreeCtrlEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	ASSERT(m_pParentI != NULL); if(m_pParentI == NULL) return;
	((CPwSafeDlg *)m_pParentI)->NotifyUserActivity();

	CTreeCtrl::OnMouseMove(nFlags, point);
}

BOOL CCustomTreeCtrlEx::EnsureVisible(HTREEITEM hItem)
{
	return CTreeCtrl::EnsureVisible(hItem);
}

UINT CCustomTreeCtrlEx::GetCount()
{
	return CTreeCtrl::GetCount();
}

/////////////////////////////////////////////////////////////////////////////

void CCustomOleDropHandler::SetDragAccept(BOOL bAccept)
{
	ASSERT((bAccept == TRUE) || (bAccept == FALSE));
	m_bAcceptDrop = bAccept;
}

DROPEFFECT CCustomOleDropHandler::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	return OnDragOver(pWnd, pDataObject, dwKeyState, point);
}

DROPEFFECT CCustomOleDropHandler::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	UINT dwState = 0;
	const UINT uGood = TVHT_ONITEMSTATEICON | TVHT_ONITEMBUTTON | TVHT_ONITEMICON | TVHT_ONITEMINDENT | TVHT_ONITEMLABEL | TVHT_ONITEMRIGHT;
	HTREEITEM h;

	ASSERT(m_pTopParentI != NULL); ASSERT(m_pParentTree != NULL);

	if(m_bAcceptDrop == FALSE) { _RemoveDropSelection(); return DROPEFFECT_NONE; } // Don't accept drop from other applications

	h = m_pParentTree->HitTest(point, &dwState);
	if(((dwState & uGood) > 0) && (h != NULL))
	{
		m_pParentTree->SelectDropTarget(h);
	}
	else
	{
		_RemoveDropSelection();
		return DROPEFFECT_NONE;
	}

	if((dwKeyState & MK_CONTROL) == MK_CONTROL) return DROPEFFECT_COPY;
	return DROPEFFECT_MOVE;
}

void CCustomOleDropHandler::OnDragLeave(CWnd* pWnd)
{
	_RemoveDropSelection();
}

BOOL CCustomOleDropHandler::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	UINT dwState = 0;
	const UINT uGood = TVHT_ONITEMSTATEICON | TVHT_ONITEMBUTTON | TVHT_ONITEMICON | TVHT_ONITEMINDENT | TVHT_ONITEMLABEL | TVHT_ONITEMRIGHT;
	HTREEITEM h;

	ASSERT(m_pTopParentI != NULL); ASSERT(m_pParentTree != NULL);

	if(m_bAcceptDrop == FALSE) return TRUE; // Don't accept drop from other applications

	h = m_pParentTree->HitTest(point, &dwState);
	if(((dwState & uGood) > 0) && (h != NULL))
	{
		((CPwSafeDlg *)m_pTopParentI)->_HandleEntryDrop(dropEffect, h);
	}

	m_bAcceptDrop = FALSE;
	_RemoveDropSelection();
	return TRUE;
}

void CCustomOleDropHandler::_RemoveDropSelection()
{
	ASSERT(m_pTopParentI != NULL); ASSERT(m_pParentTree != NULL);
	m_pParentTree->SelectDropTarget(NULL);
}

#pragma warning(pop)
