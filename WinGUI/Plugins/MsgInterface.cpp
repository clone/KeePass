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

#include "StdAfx.h"
#include "../Resource.h"
#include "MsgInterface.h"
#include "../../KeePassLibCpp/Util/TranslateEx.h"
#include "../PwSafeDlg.h"
#include "../Util/WinUtil.h"

static CPwSafeDlg *g_pMainDlg = NULL;

void KPMI_SetMainDialog(void *pMainPwSafeDlg)
{
	g_pMainDlg = (CPwSafeDlg *)pMainPwSafeDlg;
}

C_FN_SHARE DWORD KP_API KP_Call(DWORD dwCode, LPARAM lParamW, LPARAM lParamL, LPARAM lParamM)
{
	ASSERT(g_pMainDlg != NULL);

	if(dwCode == KPC_INSERT_IMPORTFROM_ITEM)
	{
		BCMenu *p = (BCMenu *)g_pMainDlg->m_menu.GetSubMenu((TCHAR *)TRL("&File"));
		if(p != NULL)
		{
			p = p->GetSubBCMenu((TCHAR *)TRL("&Import From"));
			if(p != NULL)
			{
				if(lParamW == 0) p->AppendMenu(MF_SEPARATOR);
				else p->AppendODMenu((LPTSTR)lParamW,
					MF_STRING | MF_OWNERDRAW,
					(DWORD)lParamL,
					&g_pMainDlg->m_ilIcons, lParamM);
			}
			else return KPR_FAILED;
		}
		else return KPR_FAILED;
	}
	else if(dwCode == KPC_MODIFIED)
		g_pMainDlg->m_bModified = (BOOL)lParamW;
	else if(dwCode == KPC_DISPLAYDIALOG)
		g_pMainDlg->m_bDisplayDialog = (BOOL)lParamW;
	else if(dwCode == KPC_PWLIST_ENSUREVISIBLE)
		g_pMainDlg->m_cList.EnsureVisible((int)lParamW, (BOOL)lParamL);
	else if(dwCode == KPC_UPDATE_TOOLBAR)
		g_pMainDlg->_UpdateToolBar();
	else if(dwCode == KPC_UPDATE_GROUPLIST)
		g_pMainDlg->UpdateGroupList();
	else if(dwCode == KPC_UPDATE_PASSWORDLIST)
		g_pMainDlg->UpdatePasswordList();
	else if(dwCode == KPC_STATUSBARTEXT)
		g_pMainDlg->SetStatusTextEx((LPCTSTR)lParamW);
	else if(dwCode == KPC_UPDATE_UI)
	{
		g_pMainDlg->_Groups_SaveView(TRUE);
		g_pMainDlg->_List_SaveView();
		g_pMainDlg->UpdateGroupList();
		g_pMainDlg->_Groups_RestoreView();
		g_pMainDlg->UpdatePasswordList();
		g_pMainDlg->_List_RestoreView();
	}
	else if(dwCode == KPC_OPENFILE_DIALOG)
		return WU_GetFileNameSz(TRUE, (LPCTSTR)lParamW, (LPTSTR)lParamL, (DWORD)lParamM);
	else if(dwCode == KPC_ADD_ENTRY)
		return g_pMainDlg->m_mgr.AddEntry((const PW_ENTRY *)lParamW);

	return KPR_SUCCESS;
}

C_FN_SHARE DWORD KP_API KP_Query(DWORD dwCode, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	if(dwCode == KPQ_VERSION) return PWM_VERSION_DW;
	else if(dwCode == KPQ_FILEOPEN)
		return g_pMainDlg->m_bFileOpen;
	else if(dwCode == KPQ_PWLIST_ITEMCOUNT)
		return g_pMainDlg->m_cList.GetItemCount();
	else if(dwCode == KPQ_SELECTEDGROUPID)
		return g_pMainDlg->GetSelectedGroupId();
	else if(dwCode == KPQ_NEVER_EXPIRE_TIME)
		g_pMainDlg->m_mgr.GetNeverExpireTime((PW_TIME *)lParam);
	else if(dwCode == KPQ_GET_GROUP)
	{
		ASSERT(sizeof(void *) == sizeof(DWORD));
		return (DWORD)g_pMainDlg->m_mgr.GetGroup((DWORD)lParam);
	}
	else if(dwCode == KPQ_ABSOLUTE_DB_PATH)
	{
		ASSERT(sizeof(LPCTSTR) == sizeof(DWORD));
		return (DWORD)(LPCTSTR)g_pMainDlg->m_strFileAbsolute;
	}
	else if(dwCode == KPQ_TRANSLATION_NAME)
	{
		ASSERT(sizeof(LPCTSTR) == sizeof(DWORD));
		return (DWORD)GetCurrentTranslationTable();
	}

	return 0;
}
