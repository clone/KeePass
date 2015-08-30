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
#include "SystemTrayEx.h"
#include "../Plugins/PluginMgr.h"
#include "../../KeePassLibCpp/Util/TranslateEx.h"

void CSystemTrayEx::CustomizeMenu(CMenu *pMenu)
{
	if(pMenu == NULL) { ASSERT(FALSE); return; }

	_CallPlugins(KPM_TRAY_CUSTOMIZE_PRE, (LPARAM)(pMenu->m_hMenu), 0);

	// Translate the menu
	for(UINT uItem = 0; uItem < pMenu->GetMenuItemCount(); ++uItem)
	{
		const UINT uItemID = pMenu->GetMenuItemID(static_cast<int>(uItem));
		if(uItemID == 0) continue;

		CString strItem;
		pMenu->GetMenuString(uItem, strItem, MF_BYPOSITION);

		CString strNew = TRL_VAR(strItem);
		VERIFY(pMenu->ModifyMenu(uItem, MF_BYPOSITION | MF_STRING, uItemID, strNew));
	}

	_CallPlugins(KPM_TRAY_CUSTOMIZE_POST, (LPARAM)(pMenu->m_hMenu), 0);
}
