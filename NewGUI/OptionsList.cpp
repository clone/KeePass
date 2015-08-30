#include "StdAfx.h"
#include "..\PwSafe.h"
#include "OptionsList.h"
#include "NewGUICommon.h"
#include "TranslateEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

COptionsList::COptionsList()
{
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

void COptionsList::InitOptionListEx(CImageList *pImages)
{
	ASSERT(pImages != NULL); if(pImages == NULL) return;
	m_pImages = pImages;

	PostMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_SI_MENU | LVS_EX_FULLROWSELECT);

	SetImageList(m_pImages, LVSIL_SMALL);

	DeleteAllItems();
	RECT rect;
	GetClientRect(&rect);
	int nWidth = rect.right - rect.left - GetSystemMetrics(SM_CXVSCROLL) - 8;
	InsertColumn(0, TRL("Options"), LVCFMT_LEFT, nWidth, 0);
}

void COptionsList::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint pointM;

	GetCursorPos(&pointM);

	UNREFERENCED_PARAMETER(pNMHDR);

	ScreenToClient(&pointM);

	UINT nFlags = 0;
	int nHitItem = HitTest(pointM, &nFlags);

	if((nFlags & LVHT_ONITEM) && (nHitItem >= 0))
	{
		BOOL *pb = (BOOL *)m_ptrs[nHitItem];

		if(pb != NULL)
		{
			*pb = (*pb == TRUE) ? FALSE : TRUE;

			SetListItemCheck(nHitItem, *pb);

			BOOL *pbl = (BOOL *)m_ptrsLinked[nHitItem];
			if(pbl != NULL)
			{
				int n = FindItemPointer((void *)pbl);

				if(n != -1)
				{
					switch(m_aLinkType[nHitItem])
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
	lvi.cchTextMax = _tcslen(lpItemText);
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
	lvi.cchTextMax = _tcslen(lpItemText);
	lvi.iImage = (*pValueStorage == TRUE) ? OL_CHECK_TRUE : OL_CHECK_FALSE;
	lvi.iIndent = 1;

	InsertItem(&lvi);
}
