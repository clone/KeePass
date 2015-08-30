#if !defined(AFX_OPTIONSLIST_H__BF7FEFE1_9E08_11D9_BF17_0050BF14F5CC__INCLUDED_)
#define AFX_OPTIONSLIST_H__BF7FEFE1_9E08_11D9_BF17_0050BF14F5CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#define OL_CHECK_FALSE 1
#define OL_CHECK_TRUE  2
#define OL_RADIO_FALSE 3
#define OL_RADIO_TRUE  4
#define OL_CHECK_LOCKED_FALSE 5
#define OL_CHECK_LOCKED_TRUE  6

#define OLF_LOCKED     1

#define OL_LINK_NULL                0
#define OL_LINK_SAME_TRIGGER_FALSE  1
#define OL_LINK_INV_TRIGGER_FALSE   2
#define OL_LINK_SAME_TRIGGER_TRUE   3
#define OL_LINK_INV_TRIGGER_TRUE    4
#define OL_LINK_SAME_TRIGGER_ALWAYS 5
#define OL_LINK_INV_TRIGGER_ALWAYS  6

/////////////////////////////////////////////////////////////////////////////
// COptionsList window

class COptionsList : public CListCtrl
{
// Construction
public:
	COptionsList();
	virtual ~COptionsList();

	void InitOptionListEx(CImageList *pImages);

	void AddGroupText(LPCTSTR lpItemText, int nIcon);
	void AddCheckItem(LPCTSTR lpItemText, BOOL *pValueStorage, BOOL *pLinkedValue, int nLinkType);

	//{{AFX_VIRTUAL(COptionsList)
	//}}AFX_VIRTUAL

private:
	void SetListItemCheck(int nItem, BOOL bCheck);
	int FindItemPointer(void *p);

	CImageList *m_pImages;
	std::vector<void *> m_ptrs;
	std::vector<void *> m_ptrsLinked;
	std::vector<BOOL> m_aLinkType;

protected:
	//{{AFX_MSG(COptionsList)
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_OPTIONSLIST_H__BF7FEFE1_9E08_11D9_BF17_0050BF14F5CC__INCLUDED_)
