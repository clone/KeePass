/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2015 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "GlobalWindowManager.h"

std::vector<CDialog*> CGlobalWindowManager::m_vDialogs;

CGlobalWindowManager::CGlobalWindowManager()
{
}

HRESULT CGlobalWindowManager::AddDialog(CDialog* pDlg)
{
	if(pDlg == NULL) { ASSERT(FALSE); return E_POINTER; }

	m_vDialogs.push_back(pDlg);
	return S_OK;
}

HRESULT CGlobalWindowManager::RemoveDialog(CDialog* pDlg)
{
	if(pDlg == NULL) { ASSERT(FALSE); return E_POINTER; }

	for(std::vector<CDialog*>::iterator it = m_vDialogs.begin();
		it != m_vDialogs.end(); ++it)
	{
		if(pDlg == *it)
		{
			m_vDialogs.erase(it);
			return S_OK;
		}
	}

	return S_FALSE;
}

DWORD CGlobalWindowManager::GetCount()
{
	return static_cast<DWORD>(m_vDialogs.size());
}

HWND CGlobalWindowManager::GetTopWindow()
{
	if(m_vDialogs.size() == 0) return NULL;

	CDialog* pDlg = m_vDialogs[m_vDialogs.size() - 1];
	return pDlg->m_hWnd;
}
