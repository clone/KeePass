/*
  Copyright (c) 2003-2005, Dominik Reichl <dominik.reichl@t-online.de>
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

#include "StdAfx.h"
#include "WindowGroups.h"

CWindowGroups::CWindowGroups()
{
	m_aWindows.RemoveAll();
	m_aGroupIDs.RemoveAll();
}

CWindowGroups::~CWindowGroups()
{
	m_aWindows.RemoveAll();
	m_aGroupIDs.RemoveAll();
}

BOOL CWindowGroups::AddWindow(CObject *pWindow, DWORD dwGroupID)
{
	m_aWindows.Add(pWindow);
	m_aGroupIDs.Add(dwGroupID);
	return TRUE;
}

BOOL CWindowGroups::HideAllExcept(DWORD dwGroupID)
{
	int i;
	CWnd *p;

	ASSERT(m_aWindows.GetSize() == m_aGroupIDs.GetSize());

	for(i = 0; i < m_aWindows.GetSize(); i++)
	{
		p = (CWnd *)m_aWindows.GetAt(i);

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
	CWnd *p;
	CDWordArray aPos;
	int i;
	RECT rect = { 0, 0, 0, 0 };

	UNREFERENCED_PARAMETER(pParentWindow);

	aPos.RemoveAll();

	for(i = 0; i < m_aWindows.GetSize(); i++) aPos.Add(WG_OFFSET_TOP);

	for(i = 0; i < m_aWindows.GetSize(); i++)
	{
		p = (CWnd *)m_aWindows.GetAt(i);

		if(p != NULL)
		{
			p->GetWindowRect(&rect);
			rect.right = rect.right - rect.left; // Coords to sizes
			rect.bottom = rect.bottom - rect.top;
		}
		rect.top = (LONG)aPos.GetAt((int)m_aGroupIDs.GetAt(i));
		rect.left = WG_OFFSET_LEFT;

		if(p != NULL)
			p->MoveWindow(rect.left, rect.top, rect.right, rect.bottom, TRUE);

		aPos.SetAt((int)m_aGroupIDs.GetAt(i), (DWORD)rect.top + WG_Y_STEP);
	}

	aPos.RemoveAll();
	return TRUE;
}
