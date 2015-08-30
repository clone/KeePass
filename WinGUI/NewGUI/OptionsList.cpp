/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2008 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "OptionsList.h"
#include "NewGUICommon.h"
#include "../../KeePassLibCpp/Util/TranslateEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

COptionsList::COptionsList()
{
	m_bTwoColumns = FALSE;

	m_ptrs.clear();
	m_ptrsLinked.clear();
	m_aLinkType.clear();
	m_pImages = NULL;
}

COptionsList::~COptionsList()
{
	m_ptrs.clear();
	m_ptrsLinked.clear();
	m_aLinkType.clear();
	m_pImages = NULL;
}

BEGIN_MESSAGE_MAP(COptionsList, CListCtrl)
	//{{AFX_MSG_MAP(COptionsList)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void COptionsList::InitOptionListEx(CImageList *pImages, BOOL bTwoColumns)
{
	ASSERT(pImages != NULL); if(pImages == NULL) return;
	m_pImages = pImages;

	m_bTwoColumns = bTwoColumns;

	PostMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_SI_MENU |
		LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

	SetImageList(m_pImages, LVSIL_SMALL);

	DeleteAllItems();
	RECT rect;
	GetClientRect(&rect);
	int nWidth = rect.right - rect.left - GetSystemMetrics(SM_CXVSCROLL) - 8;
	if(m_bTwoColumns == TRUE) nWidth /= 2;
	InsertColumn(0, TRL("Options"), LVCFMT_LEFT, nWidth, 0);

	if(m_bTwoColumns == TRUE)
		InsertColumn(1, TRL("Value"), LVCFMT_LEFT, nWidth, 1);
}

void COptionsList::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UNREFERENCED_PARAMETER(pNMHDR);

	CPoint pointM;
	GetCursorPos(&pointM);
	ScreenToClient(&pointM);

	UINT nFlags = 0;
	int nHitItem = HitTest(pointM, &nFlags);

	if(((nFlags & LVHT_ONITEM) != 0) && (nHitItem >= 0))
		ToggleItem(nHitItem);

	*pResult = 0;
}

int COptionsList::FindItemPointer(void *p)
{
	int i;

	for(i = 0; i < (int)m_ptrs.size(); i++)
		if(m_ptrs[i] == p) return i;

	return -1;
}

void COptionsList::SetListItemCheck(int nItem, BOOL bCheck)
{
	LV_ITEM lvi;

	ZeroMemory(&lvi, sizeof(LV_ITEM));

	lvi.mask = LVIF_IMAGE;
	lvi.iItem = nItem;
	lvi.iImage = (bCheck == TRUE) ? OL_CHECK_TRUE : OL_CHECK_FALSE;

	SetItem(&lvi);
}

void COptionsList::AddGroupText(LPCTSTR lpItemText, int nIcon)
{
	LV_ITEM lvi;

	ASSERT(lpItemText != NULL); if(lpItemText == NULL) return;

	m_ptrs.push_back(NULL);
	m_ptrsLinked.push_back(NULL);
	m_aLinkType.push_back(OL_LINK_NULL);

	ZeroMemory(&lvi, sizeof(LV_ITEM));

	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_INDENT;
	lvi.iItem = GetItemCount();
	lvi.pszText = (LPTSTR)lpItemText;
	lvi.cchTextMax = static_cast<int>(_tcslen(lpItemText));
	lvi.iImage = nIcon;

	InsertItem(&lvi);
}

void COptionsList::AddCheckItem(LPCTSTR lpItemText, BOOL *pValueStorage, BOOL *pLinkedValue, int nLinkType)
{
	LV_ITEM lvi;

	ASSERT(lpItemText != NULL); if(lpItemText == NULL) return;

	m_ptrs.push_back((void *)pValueStorage);
	m_ptrsLinked.push_back((void *)pLinkedValue);
	m_aLinkType.push_back(nLinkType);

	ZeroMemory(&lvi, sizeof(LV_ITEM));

	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_INDENT;
	lvi.iItem = GetItemCount();
	lvi.iSubItem = 0;
	lvi.pszText = (LPTSTR)lpItemText;
	lvi.cchTextMax = static_cast<int>(_tcslen(lpItemText));
	lvi.iImage = (*pValueStorage == TRUE) ? OL_CHECK_TRUE : OL_CHECK_FALSE;
	lvi.iIndent = 1;

	InsertItem(&lvi);
}

void COptionsList::AddCheckItemEx(LPCTSTR lpItemText, LPCTSTR lpSubItemText, BOOL *pValueStorage, BOOL *pLinkedValue, int nLinkType)
{
	AddCheckItem(lpItemText, pValueStorage, pLinkedValue, nLinkType);
	if(m_bTwoColumns == FALSE) return;

	ASSERT(lpSubItemText != NULL); if(lpSubItemText == NULL) return;

	LV_ITEM lvi;

	ZeroMemory(&lvi, sizeof(LV_ITEM));

	lvi.mask = LVIF_TEXT;
	lvi.iItem = GetItemCount() - 1;
	lvi.iSubItem = 1;
	lvi.pszText = (LPTSTR)lpSubItemText;
	lvi.cchTextMax = static_cast<int>(_tcslen(lpSubItemText));

	SetItem(&lvi);
}

BOOL COptionsList::PreTranslateMessage(MSG* pMsg)
{
	ASSERT(pMsg != NULL);

	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_SPACE)
		{
			ToggleSelectedItems();
			return TRUE;
		}
	}
	else if(pMsg->message == WM_KEYUP)
	{
		if(pMsg->wParam == VK_SPACE) return TRUE; // Ignore
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}

void COptionsList::ToggleItem(int nItem)
{
	ASSERT((nItem >= 0) && (nItem < (int)m_ptrs.size()));
	if((nItem < 0) || (nItem >= (int)m_ptrs.size())) return;

	BOOL *pb = (BOOL *)m_ptrs[nItem];

	if(pb != NULL)
	{
		*pb = (*pb == TRUE) ? FALSE : TRUE;

		SetListItemCheck(nItem, *pb);

		BOOL *pbl = (BOOL *)m_ptrsLinked[nItem];
		if(pbl != NULL)
		{
			int n = FindItemPointer((void *)pbl);

			if(n != -1)
			{
				switch(m_aLinkType[nItem])
				{
				case OL_LINK_SAME_TRIGGER_TRUE:
					if(*pb == TRUE) { *pbl = TRUE; SetListItemCheck(n, TRUE); }
					break;
				case OL_LINK_SAME_TRIGGER_FALSE:
					if(*pb == FALSE) { *pbl = FALSE; SetListItemCheck(n, FALSE); }
					break;
				case OL_LINK_SAME_TRIGGER_ALWAYS:
					*pbl = *pb;
					SetListItemCheck(n, *pb);
					break;
				case OL_LINK_INV_TRIGGER_TRUE:
					if(*pb == TRUE) { *pbl = FALSE; SetListItemCheck(n, FALSE); }
					break;
				case OL_LINK_INV_TRIGGER_FALSE:
					if(*pb == FALSE) { *pbl = TRUE; SetListItemCheck(n, TRUE); }
					break;
				case OL_LINK_INV_TRIGGER_ALWAYS:
					*pbl = (*pb == TRUE) ? FALSE : TRUE;
					SetListItemCheck(n, *pbl);
					break;
				default:
					break;
				}
			}
		}
	}
}

void COptionsList::ToggleSelectedItems()
{
	for(DWORD i = 0; i < static_cast<DWORD>(m_ptrs.size()); ++i)
	{
		UINT uState = this->GetItemState(static_cast<int>(i), LVIS_SELECTED);
		if((uState & LVIS_SELECTED) != 0)
			ToggleItem(static_cast<int>(i));
	}
}
