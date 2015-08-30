/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2005 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "PwSafe.h"
#include "PluginsDlg.h"
#include "NewGUI/NewGUICommon.h"
#include "NewGUI/TranslateEx.h"
#include "Util/StrUtil.h"
#include "Util/WinUtil.h"
#include "PwSafe/PwManager.h"
#include "Util/PrivateConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CPluginsDlg::CPluginsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPluginsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPluginsDlg)
	//}}AFX_DATA_INIT

	m_pPiMgr = NULL;
	m_pImgList = NULL;
}

void CPluginsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPluginsDlg)
	DDX_Control(pDX, IDC_STATIC_HL_HELP, m_hlHelp);
	DDX_Control(pDX, IDC_STATIC_HL_GETPLUGINS, m_hlGetPlugins);
	DDX_Control(pDX, IDOK, m_btClose);
	DDX_Control(pDX, IDC_PLUGINS_LIST, m_cList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPluginsDlg, CDialog)
	//{{AFX_MSG_MAP(CPluginsDlg)
	ON_NOTIFY(NM_RCLICK, IDC_PLUGINS_LIST, OnRClickPluginsList)
	ON_COMMAND(ID_PLUGIN_ENABLE, OnPluginEnable)
	ON_COMMAND(ID_PLUGIN_DISABLE, OnPluginDisable)
	ON_COMMAND(ID_PLUGIN_CONFIG, OnPluginConfig)
	ON_COMMAND(ID_PLUGIN_ABOUT, OnPluginAbout)
	//}}AFX_MSG_MAP

	ON_REGISTERED_MESSAGE(WM_XHYPERLINK_CLICKED, OnXHyperLinkClicked)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CPluginsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	ASSERT(m_pPiMgr != NULL);
	ASSERT(m_pImgList != NULL);

	NewGUI_XPButton(&m_btClose, IDB_OK, IDB_OK);

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_PLUGINS),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(TRL("Plugin Manager"));
	m_banner.SetCaption(TRL("Here you can configure all installed KeePass plugins."));

	NewGUI_MakeHyperLink(&m_hlGetPlugins);
	m_hlGetPlugins.EnableTooltip(FALSE);
	m_hlGetPlugins.SetNotifyParent(TRUE);
	m_hlGetPlugins.EnableURL(FALSE);

	NewGUI_MakeHyperLink(&m_hlHelp);
	m_hlHelp.EnableTooltip(FALSE);
	m_hlHelp.SetNotifyParent(TRUE);
	m_hlHelp.EnableURL(FALSE);

	m_cList.SetImageList(m_pImgList, LVSIL_SMALL);

	m_cList.PostMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0,
		LVS_EX_SI_REPORT | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE | LVS_EX_UNDERLINEHOT);

	RECT rect;
	m_cList.GetWindowRect(&rect);
	int nWidth = (rect.right - rect.left - GetSystemMetrics(SM_CXVSCROLL) - 8) / 6;
	m_cList.InsertColumn(0, TRL("Files"), LVCFMT_LEFT, nWidth * 2, 0);
	m_cList.InsertColumn(1, TRL("Status"), LVCFMT_LEFT, nWidth * 4, 1);
	m_cList.InsertColumn(2, TRL("ID"), LVCFMT_LEFT, 0, 2);

	TCHAR tszBase[MAX_PATH]; tszBase[0] = 0; tszBase[1] = 0;
	GetModuleFileName(NULL, tszBase, MAX_PATH - 2);
	int jj;
	for(jj = (int)_tcslen(tszBase) - 1; jj > 0; jj--)
		if(tszBase[jj] == _T('\\'))
		{
			tszBase[jj + 1] = 0;
			break;
		}
	VERIFY(m_pPiMgr->AddAllPlugins(tszBase, _T("*.dll"), TRUE));

	UpdateGUI();

	return TRUE;
}

void CPluginsDlg::OnOK() 
{
	CDialog::OnOK();
}

void CPluginsDlg::OnCancel() 
{
	CDialog::OnCancel();
}

void CPluginsDlg::UpdateGUI()
{
	LV_ITEM lvi;

	m_cList.DeleteAllItems();

	ZeroMemory(&lvi, sizeof(LV_ITEM));

	unsigned int i;
	KP_PLUGIN_INSTANCE *p;
	CString str, strT;

	for(i = 0; i < m_pPiMgr->m_plugins.size(); i++)
	{
		p = &m_pPiMgr->m_plugins[i];
		ASSERT(p != NULL); if(p == NULL) continue;

		lvi.iItem = (int)i;
		lvi.iSubItem = 0;
		lvi.mask = LVIF_TEXT | LVIF_IMAGE;

		lvi.iImage = (p->bEnabled == TRUE) ? 20 : 45;

		strT = p->tszFile;
		str = CsFileOnly(&strT);
		lvi.pszText = (TCHAR *)(LPCTSTR)str;
		m_cList.InsertItem(&lvi);

		lvi.mask = LVIF_TEXT;

		if((p->hinstDLL != NULL) && (p->bEnabled == FALSE))
			lvi.pszText = (TCHAR *)TRL("This plugin will be disabled after you restart KeePass.");
		else if((p->hinstDLL != NULL) && (p->bEnabled == TRUE))
			lvi.pszText = (TCHAR *)TRL("Enabled, loaded");
		else if((p->hinstDLL == NULL) && (p->bEnabled == FALSE))
			lvi.pszText = (TCHAR *)TRL("Disabled, not loaded");
		else if((p->hinstDLL == NULL) && (p->bEnabled == TRUE))
			lvi.pszText = (TCHAR *)TRL("This plugin will be enabled after you restart KeePass.");
		else { ASSERT(FALSE); lvi.pszText = _T(""); }

		lvi.iSubItem = 1;
		m_cList.SetItem(&lvi);

		lvi.iSubItem = 2;
		strT.Format(_T("%u"), p->dwPluginID);
		lvi.pszText = (TCHAR *)(LPCTSTR)strT;
		m_cList.SetItem(&lvi);
	}
}

void CPluginsDlg::OnRClickPluginsList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	DWORD dwID = GetSelectedPluginID();
	KP_PLUGIN_INSTANCE *p = m_pPiMgr->GetPluginByID(dwID);

	UNREFERENCED_PARAMETER(pNMHDR);

	*pResult = 0;

	POINT pt;
	GetCursorPos(&pt);

	m_popmenu.LoadMenu(IDR_PLUGINS_MENU);

	m_popmenu.SetMenuDrawMode(BCMENU_DRAWMODE_XP); // <<<!=>>> BCMENU_DRAWMODE_ORIGINAL
	m_popmenu.SetSelectDisableMode(FALSE);
	m_popmenu.SetXPBitmap3D(TRUE);
	m_popmenu.SetBitmapBackground(RGB(255, 0, 255));
	m_popmenu.SetIconSize(16, 16);

	m_popmenu.LoadToolbar(IDR_INFOICONS, IDB_INFOICONS_EX);

	BCMenu *psub = (BCMenu *)m_popmenu.GetSubMenu(0);

	psub->ModifyODMenu(NULL, (UINT)ID_PLUGIN_ENABLE, m_pImgList, 2);
	psub->ModifyODMenu(NULL, (UINT)ID_PLUGIN_DISABLE, m_pImgList, 45);
	psub->ModifyODMenu(NULL, (UINT)ID_PLUGIN_CONFIG, m_pImgList, 21);
	psub->ModifyODMenu(NULL, (UINT)ID_PLUGIN_ABOUT, m_pImgList, 22);

	CPwSafeDlg::_TranslateMenu(psub, FALSE);

	if(p != NULL)
	{
		if(p->bEnabled == FALSE)
		{
			psub->EnableMenuItem(ID_PLUGIN_DISABLE, MF_BYCOMMAND | MF_GRAYED);
			psub->EnableMenuItem(ID_PLUGIN_CONFIG, MF_BYCOMMAND | MF_GRAYED);
			psub->EnableMenuItem(ID_PLUGIN_ABOUT, MF_BYCOMMAND | MF_GRAYED);
		}
		else
		{
			psub->EnableMenuItem(ID_PLUGIN_ENABLE, MF_BYCOMMAND | MF_GRAYED);

			if(p->hinstDLL != NULL)
			{
			}
			else
			{
			}
		}

		if(p->hinstDLL == NULL)
		{
			psub->EnableMenuItem(ID_PLUGIN_CONFIG, MF_BYCOMMAND | MF_GRAYED);
			psub->EnableMenuItem(ID_PLUGIN_ABOUT, MF_BYCOMMAND | MF_GRAYED);
		}
	}
	else
	{
		psub->EnableMenuItem(ID_PLUGIN_ENABLE, MF_BYCOMMAND | MF_GRAYED);
		psub->EnableMenuItem(ID_PLUGIN_DISABLE, MF_BYCOMMAND | MF_GRAYED);
		psub->EnableMenuItem(ID_PLUGIN_CONFIG, MF_BYCOMMAND | MF_GRAYED);
		psub->EnableMenuItem(ID_PLUGIN_ABOUT, MF_BYCOMMAND | MF_GRAYED);
	}

	psub->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);

	m_popmenu.DestroyMenu();
}

DWORD CPluginsDlg::GetSelectedPluginID()
{
	DWORD i;
	UINT uState;
	LV_ITEM lvi;
	TCHAR tszBuf[13];

	ZeroMemory(&lvi, sizeof(LV_ITEM));

	for(i = 0; i < (DWORD)m_cList.GetItemCount(); i++)
	{
		uState = m_cList.GetItemState((int)i, LVIS_SELECTED);
		if(uState & LVIS_SELECTED)
		{
			lvi.iItem = (int)i;
			lvi.iSubItem = 2;
			lvi.mask = LVIF_TEXT;
			lvi.pszText = tszBuf;
			lvi.cchTextMax = 12;
			m_cList.GetItem(&lvi);

			return (DWORD)_ttoi(lvi.pszText);
		}
	}

	return DWORD_MAX;
}

void CPluginsDlg::OnPluginEnable() 
{
	DWORD dwID = GetSelectedPluginID();
	if(dwID == DWORD_MAX) return;

	m_pPiMgr->EnablePluginByID(dwID, TRUE);

	UpdateGUI();
}

void CPluginsDlg::OnPluginDisable() 
{
	DWORD dwID = GetSelectedPluginID();
	if(dwID == DWORD_MAX) return;

	m_pPiMgr->EnablePluginByID(dwID, FALSE);

	UpdateGUI();
}

void CPluginsDlg::OnPluginConfig() 
{
	DWORD dwID = GetSelectedPluginID();
	ASSERT(dwID != DWORD_MAX); if(dwID == DWORD_MAX) return;
	m_pPiMgr->CallSinglePlugin(dwID, KPM_DIRECT_CONFIG, 0, 0);
}

void CPluginsDlg::OnPluginAbout() 
{
	DWORD dwID = GetSelectedPluginID();
	ASSERT(dwID != DWORD_MAX); if(dwID == DWORD_MAX) return;
	m_pPiMgr->CallSinglePlugin(dwID, KPM_PLUGIN_INFO, 0, 0);
}

LRESULT CPluginsDlg::OnXHyperLinkClicked(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	if(wParam == IDC_STATIC_HL_GETPLUGINS)
		ShellExecute(NULL, _T("open"), PWM_URL_PLUGINS, NULL, NULL, SW_SHOW);
	else if(wParam == IDC_STATIC_HL_HELP)
		WU_OpenAppHelp(_T("plugins.html"));

	return 0;
}
