/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2012 Dominik Reichl <dominik.reichl@t-online.de>

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

/*
#include "VistaMenu.h"

IMPLEMENT_DYNAMIC(CVistaMenu, CMenu)

CVistaMenu::CVistaMenu()
{
}

CVistaMenu::~CVistaMenu()
{
	this->DeleteBitmaps();
}

void CVistaMenu::DeleteBitmaps()
{
	for(size_t i = 0; i < m_vBitmaps.size(); ++i)
	{
		VERIFY(m_vBitmaps[i]->DeleteObject() != FALSE);
	}

	m_vBitmaps.clear();
}

void CVistaMenu::LoadToolbar(UINT uToolBar, UINT uBitmap)
{
	CWnd* pWnd = AfxGetMainWnd();
	if(pWnd == NULL) { ASSERT(FALSE); pWnd = CWnd::GetDesktopWindow(); }

	CToolBar bar;
	bar.Create(pWnd);

	if(bar.LoadToolBar(uToolBar))
	{
		CBitmap bmp;
		VERIFY(bmp.LoadBitmap(uBitmap));

		CDC* pDC = pWnd->GetDC();
		if(pDC == NULL) { ASSERT(FALSE); return; }
		CBitmap* pbmpPrev = pDC->SelectObject(&bmp);

		for(int i = 0; i < bar.GetCount(); ++i)
		{
			const UINT uID = bar.GetItemID(i);
			if((uID != 0) && (this->GetMenuState(uID, MF_BYCOMMAND) != 0xFFFFFFFF))
			{
				CDC cdcExtracted;
				VERIFY(cdcExtracted.CreateCompatibleDC(pDC));
				CBitmap* pbmpExtracted = new CBitmap();
				VERIFY(pbmpExtracted->CreateCompatibleBitmap(&cdcExtracted, 16, 16));
				CBitmap* pbmpMemPrev = cdcExtracted.SelectObject(pbmpExtracted);

				cdcExtracted.SelectObject(pbmpMemPrev);
				VERIFY(cdcExtracted.DeleteDC());

				this->SetMenuItemBitmaps(uID, MF_BYCOMMAND, pbmpExtracted, pbmpExtracted);

				m_vBitmaps.push_back(pbmpExtracted);
			}
		}

		pDC->SelectObject(pbmpPrev);
		pWnd->ReleaseDC(pDC);
		bmp.DeleteObject();
	}
	else { ASSERT(FALSE); }
}
*/
