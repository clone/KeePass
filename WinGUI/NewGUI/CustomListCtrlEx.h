/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2010 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef AFX_CUSTOMLISTCTRLEX_H__14392920_86D7_11D7_BF16_0050BF14F5CC__INCLUDED_
#define AFX_CUSTOMLISTCTRLEX_H__14392920_86D7_11D7_BF16_0050BF14F5CC__INCLUDED_

#include "../../KeePassLibCpp/SysDefEx.h"
#include "NewGUICommon.h"

#define CLCIF_HIGHLIGHT_GREEN 1

/////////////////////////////////////////////////////////////////////////////

class CCustomListCtrlEx : public CListCtrl
{
public:
	CCustomListCtrlEx();
	virtual ~CCustomListCtrlEx();

	COLORREF GetRowColorEx();
	void SetRowColorEx(COLORREF rgbColor);

	void FocusItem(int iIndex, BOOL bAlsoSelect);

	bool LockRedrawEx(bool bBlockRedrawing);
	bool LockRedrawEx(bool bBlockRedrawing, bool bFullInvalidate);
	void DeleteAllItemsEx();

	CWnd *m_pParentI;
	BOOL *m_pbShowColumns;

	//{{AFX_VIRTUAL(CCustomListCtrlEx)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

private:
	COLORREF m_rgbRowColor;
	BOOL m_bColorize;

	bool m_bRedrawingEnabled;

protected:
	//{{AFX_MSG(CCustomListCtrlEx)
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // AFX_CUSTOMLISTCTRLEX_H__14392920_86D7_11D7_BF16_0050BF14F5CC__INCLUDED_
