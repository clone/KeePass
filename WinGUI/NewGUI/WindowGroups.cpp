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

#include "StdAfx.h"
#include "WindowGroups.h"
#include <math.h>

CWindowGroups::CWindowGroups()
{
	m_aWindows.RemoveAll();
	m_aGroupIDs.RemoveAll();
	m_aFlags.RemoveAll();
}

CWindowGroups::~CWindowGroups()
{
	m_aWindows.RemoveAll();
	m_aGroupIDs.RemoveAll();
	m_aFlags.RemoveAll();
}

BOOL CWindowGroups::AddWindow(CObject *pWindow, DWORD dwGroupID, BOOL bReposition)
{
	m_aWindows.Add(pWindow);
	m_aGroupIDs.Add(dwGroupID);

	BYTE bt = 0;
	if(bReposition == TRUE) bt |= WGF_REPOSITION;
	m_aFlags.Add(bt);

	return TRUE;
}

BOOL CWindowGroups::HideAllExcept(DWORD dwGroupID)
{
	ASSERT(m_aWindows.GetSize() == m_aGroupIDs.GetSize());

	for(int i = 0; i < m_aWindows.GetSize(); i++)
	{
		CWnd *p = (CWnd *)m_aWindows.GetAt(i);
		if(p == NULL) continue;

		if(m_aGroupIDs.GetAt(i) != dwGroupID)
		{
			p->ShowWindow(SW_HIDE);
			p->EnableWindow(FALSE);
		}
		else
		{
			p->EnableWindow(TRUE);
			p->ShowWindow(TRUE);
		}
	}

	return TRUE;
}

BOOL CWindowGroups::ArrangeWindows(CWnd *pParentWindow)
{
	int i;
	RECT rect = { 0, 0, 0, 0 };

	const DWORD dwYStep = static_cast<DWORD>(NewGUI_Scale(WG_Y_STEP, pParentWindow));

	CDWordArray aPos;
	aPos.RemoveAll();

	const DWORD dwXOffset = static_cast<DWORD>(NewGUI_Scale(WG_OFFSET_LEFT, pParentWindow));
	const DWORD dwYOffset = static_cast<DWORD>(NewGUI_Scale(WG_OFFSET_TOP, pParentWindow));
	for(i = 0; i < m_aWindows.GetSize(); i++) aPos.Add(dwYOffset);

	for(i = 0; i < m_aWindows.GetSize(); i++)
	{
		if((m_aFlags.GetAt(i) & WGF_REPOSITION) == 0) continue;

		CWnd *p = (CWnd *)m_aWindows.GetAt(i);

		if(p != NULL)
		{
			p->GetWindowRect(&rect);
			rect.right = rect.right - rect.left; // Coords to sizes
			rect.bottom = rect.bottom - rect.top;
		}
		rect.top = (LONG)aPos.GetAt((int)m_aGroupIDs.GetAt(i));
		rect.left = dwXOffset;

		if(p != NULL)
			p->MoveWindow(rect.left, rect.top, rect.right, rect.bottom, TRUE);

		aPos.SetAt((int)m_aGroupIDs.GetAt(i), (DWORD)rect.top + dwYStep);
	}

	aPos.RemoveAll();
	return TRUE;
}
