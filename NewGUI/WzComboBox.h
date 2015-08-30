//	WzComboBox.h : header file
/*********************************************************************
*	Author:		Simon Wang
*	Date:		2002-06-21
*	Contact us:	Inte2000@263.net
*
*	Modified:	Dominik Reichl
*	Date:		2003-07-02
*	Contact:	dominik.reichl@t-online.de
*	Info:		Customized CWzComboBox (changed CImageList impl, ...)
**********************************************************************/

#if !defined(AFX_CWZCOMBOBOX_H__86AA2FAB_5927_430B_AF57_06B24164E192__INCLUDED_)
#define AFX_CWZCOMBOBOX_H__86AA2FAB_5927_430B_AF57_06B24164E192__INCLUDED_

#include "NewGUICommon.h"
#include "../Util/SysDefEx.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WZ_ROOT_INDEX ((WORD)(-1))

#define FC_DRAWNORMAL	0x00000001
#define FC_DRAWRAISED	0x00000002
#define FC_DRAWPRESSD	0x00000004

typedef struct tagITEMDATA
{
	WORD wParentOriginIdx;
	WORD wOriginIdx;
	BYTE cType;
	BYTE cLevel;
}ITEMDATA,*LPITEMDATA;

/////////////////////////////////////////////////////////////////////////////
// CWzComboBox window

class CWzComboBox : public CComboBox
{
// Construction
public:
	CWzComboBox();

// Attributes
public:

// Operations
public:
	WORD AddCTString(WORD wParentIdx,BYTE cType,LPCTSTR lpszString);
	int DeleteCTString(int index);

	COLORREF GetBkGndColor() { return m_crBkGnd;}
	COLORREF GetHiLightBkGndColor() { return m_crHiLightBkGnd;}
	COLORREF GetTextColor() {return m_crText;}
	COLORREF GetHiLightTextColor() {return m_crHiLightText;}
	COLORREF GetHiLightFrameColor() {return m_crHiLightFrame;};

	void SetBkGndColor(COLORREF crBkGnd) { m_crBkGnd = crBkGnd;}
	void SetHiLightBkGndColor(COLORREF crHiLightBkGnd) { m_crHiLightBkGnd = crHiLightBkGnd;}
	void SetTextColor(COLORREF crText) { m_crText = crText;}
	void SetHiLightTextColor(COLORREF crHiLightText) { m_crHiLightText = crHiLightText;}
	void SetHiLightFrameColor(COLORREF crHiLightFrame) { m_crHiLightFrame = crHiLightFrame;}

	void SetXImageList(CImageList *pImgList);

	CImageList *m_pImgList;

protected:
	//{{AFX_VIRTUAL(CWzComboBox)
	public:
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
	//}}AFX_VIRTUAL
	int GetChildCount(WORD wParentIdx);//Get All children node,include children's children node
	int CurrentIdxFromOriginIdx(int wOriginIdx);
	void RecalcDropWidth();
	void DrawIconString(LPDRAWITEMSTRUCT lpDIS, BOOL bselected);
	void DrawCombo(DWORD dwStyle, COLORREF clrTopLeft, COLORREF clrBottomRight);
  
	//Below function should not be called
	virtual int AddString(LPCTSTR lpszString)
	{
		UNREFERENCED_PARAMETER(lpszString);
		return -1;
	}
	
	virtual int InsertString(int nIndex, LPCTSTR lpszString)
	{
		UNREFERENCED_PARAMETER(nIndex);
		UNREFERENCED_PARAMETER(lpszString);
		return -1;
	}

	virtual int DeleteString(int nIndex)
	{
		UNREFERENCED_PARAMETER(nIndex);
		return -1;
	}

// Implementation
public:
	virtual ~CWzComboBox();

	// Generated message map functions
protected:
	COLORREF m_crBkGnd,m_crHiLightBkGnd;
	COLORREF m_crText,m_crHiLightText;
	COLORREF m_crHiLightFrame;
	//{{AFX_MSG(CWzComboBox)
	afx_msg void OnDropdown();
	afx_msg void OnPaint();
	afx_msg void OnSysColorChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_CTCOMBOBOX_H__86AA2FAB_5927_430B_AF57_06B24164E192__INCLUDED_)
