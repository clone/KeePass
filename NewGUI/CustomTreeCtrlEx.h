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

#ifndef AFX_CUSTOMTREECTRLEX_H__EE7BE580_7102_11D8_BF16_0050BF14F5CC__INCLUDED_
#define AFX_CUSTOMTREECTRLEX_H__EE7BE580_7102_11D8_BF16_0050BF14F5CC__INCLUDED_

#include "../Util/SysDefEx.h"

/////////////////////////////////////////////////////////////////////////////

class CCustomOleDropHandler : public COleDropTarget
{
public:
	DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	void OnDragLeave(CWnd* pWnd);
	BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);

	void SetDragAccept(BOOL bAccept);
	void _RemoveDropSelection();

	CWnd *m_pTopParentI;
	CTreeCtrl *m_pParentTree;

private:
	BOOL m_bAcceptDrop;
};

/////////////////////////////////////////////////////////////////////////////

class CPP_CLASS_SHARE CCustomTreeCtrlEx : public CTreeCtrl
{
public:
	CCustomTreeCtrlEx();
	virtual ~CCustomTreeCtrlEx();

	BOOL InitDropHandler();

	BOOL EnsureVisible(HTREEITEM hItem);
	UINT GetCount();

	CWnd *m_pParentI;
	CCustomOleDropHandler m_drop;

	//{{AFX_VIRTUAL(CCustomTreeCtrlEx)
	//}}AFX_VIRTUAL

private:
	BOOL m_bValidDropTarget;

protected:
	//{{AFX_MSG(CCustomTreeCtrlEx)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // AFX_CUSTOMTREECTRLEX_H__EE7BE580_7102_11D8_BF16_0050BF14F5CC__INCLUDED_
