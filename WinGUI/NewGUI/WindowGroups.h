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

#ifndef ___WINDOW_GROUPS_H___
#define ___WINDOW_GROUPS_H___

#include "NewGUICommon.h"

#define WG_OFFSET_TOP  105
#define WG_OFFSET_LEFT  35
#define WG_Y_STEP       20

#define WGF_REPOSITION 1

class CWindowGroups
{
public:
	CWindowGroups();
	virtual ~CWindowGroups();

	BOOL AddWindow(CObject *pWindow, DWORD dwGroupID, BOOL bReposition);
	BOOL ArrangeWindows(CWnd *pParentWindow);
	BOOL HideAllExcept(DWORD dwGroupID);

private:
	CObArray m_aWindows;
	CDWordArray m_aGroupIDs;
	CByteArray m_aFlags;
};

#endif
