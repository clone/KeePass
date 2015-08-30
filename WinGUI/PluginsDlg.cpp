/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2011 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "../KeePassLibCpp/PwManager.h"
#include "../KeePassLibCpp/Util/TranslateEx.h"
#include "../KeePassLibCpp/Util/StrUtil.h"
#include "Util/WinUtil.h"
#include "Util/CmdLine/Executable.h"

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
	DDX_Control(pDX, IDC_STATIC_HL_GETPLUGINS, m_hlGetPlugins);
	DDX_Control(pDX, IDOK, m_btClose);
	DDX_Control(pDX, IDC_PLUGINS_LIST, m_cList);
	DDX_Control(pDX, IDC_BTN_PLGHELP, m_btHelp);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPluginsDlg, CDialog)
	//{{AFX_MSG_MAP(CPluginsDlg)
	ON_NOTIFY(NM_RCLICK, IDC_PLUGINS_LIST, OnRClickPluginsList)
	ON_COMMAND(ID_PLUGIN_CONFIG, OnPluginConfig)
	ON_COMMAND(ID_PLUGIN_ABOUT, OnPluginAbout)
	ON_BN_CLICKED(IDC_BTN_PLGHELP, &CPluginsDlg::OnBtnClickedPlgHelp)
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

	NewGUI_XPButton(m_btClose, IDB_OK, IDB_OK);
	NewGUI_XPButton(m_btHelp, IDB_HELP_SMALL, IDB_HELP_SMALL);

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_PLUGINS),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(TRL("Plugins"));
	m_banner.SetCaption(TRL("Here you can configure all installed KeePass plugins."));

	NewGUI_MakeHyperLink(&m_hlGetPlugins);
	m_hlGetPlugins.EnableTooltip(FALSE);
	m_hlGetPlugins.SetNotifyParent(TRUE);
	m_hlGetPlugins.EnableURL(FALSE);

	m_cList.SetImageList(m_pImgList, LVSIL_SMALL);

	// m_cList.PostMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_SI_REPORT |
	//	LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE |
	//	LVS_EX_UNDERLINEHOT | LVS_EX_INFOTIP);
	m_cList.PostMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_SI_REPORT |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP);

	RECT rect;
	m_cList.GetWindowRect(&rect);
	const int nWidth = (rect.right - rect.left - GetSystemMetrics(SM_CXVSCROLL) - 8) / 7;
	m_cList.InsertColumn(0, TRL("Plugin"), LVCFMT_LEFT, nWidth * 3, 0);
	m_cList.InsertColumn(1, TRL("Version"), LVCFMT_LEFT, nWidth, 1);
	m_cList.InsertColumn(2, TRL("Author"), LVCFMT_LEFT, nWidth * 3, 2);
	m_cList.InsertColumn(3, TRL("ID"), LVCFMT_LEFT, 0, 3);

	// TCHAR tszBase[MAX_PATH]; tszBase[0] = 0; tszBase[1] = 0;
	// GetModuleFileName(NULL, tszBase, MAX_PATH - 2);
	// int jj;
	// for(jj = (int)_tcslen(tszBase) - 1; jj > 0; jj--)
	//	if(tszBase[jj] == _T('\\'))
	//	{
	//		tszBase[jj + 1] = 0;
	//		break;
	//	}
	// std_string strBase = Executable::instance().getPathOnly();
	// VERIFY(m_pPiMgr->AddAllPlugins(strBase.c_str(), _T("*.dll"), FALSE)); // Updates existing list

	UpdateGUI();

	return TRUE;
}

void CPluginsDlg::OnOK()
{
	m_cList.SetImageList(NULL, LVSIL_SMALL);
	CDialog::OnOK();
}

void CPluginsDlg::OnCancel()
{
	m_cList.SetImageList(NULL, LVSIL_SMALL);
	CDialog::OnCancel();
}

void CPluginsDlg::UpdateGUI()
{
	m_cList.DeleteAllItems();

	for(size_t i = 0; i < m_pPiMgr->m_plugins.size(); ++i)
	{
		KP_PLUGIN_INSTANCE* p = &m_pPiMgr->m_plugins[i];
		ASSERT(p != NULL); if(p == NULL) continue;

		LV_ITEM lvi;
		ZeroMemory(&lvi, sizeof(LV_ITEM));

		lvi.iItem = static_cast<int>(i);
		lvi.mask = (LVIF_TEXT | LVIF_IMAGE);

		// lvi.iImage = ((p->bEnabled == TRUE) ? 20 : 45);
		lvi.iImage = ((p->hinstDLL != NULL) ? 20 : 45);

		CString strPlugin = p->strName.c_str();
		lvi.pszText = const_cast<LPTSTR>((LPCTSTR)strPlugin);
		m_cList.InsertItem(&lvi);

		lvi.mask = LVIF_TEXT;

		// if((p->hinstDLL != NULL) && (p->bEnabled == FALSE))
		//	lvi.pszText = (LPTSTR)TRL("This plugin will be disabled after you restart KeePass.");
		// else if((p->hinstDLL != NULL) && (p->bEnabled == TRUE))
		//	lvi.pszText = (LPTSTR)TRL("Enabled, loaded");
		// else if((p->hinstDLL == NULL) && (p->bEnabled == FALSE))
		//	lvi.pszText = (LPTSTR)TRL("Disabled, not loaded");
		// else if((p->hinstDLL == NULL) && (p->bEnabled == TRUE))
		//	lvi.pszText = (LPTSTR)TRL("This plugin will be enabled after you restart KeePass.");
		// else { ASSERT(FALSE); lvi.pszText = _T(""); }

		CString strVersion = p->strVersion.c_str();
		lvi.pszText = const_cast<LPTSTR>((LPCTSTR)strVersion);
		lvi.iSubItem = 1;
		m_cList.SetItem(&lvi);

		CString strAuthor = p->strAuthor.c_str();
		lvi.pszText = const_cast<LPTSTR>((LPCTSTR)strAuthor);
		lvi.iSubItem = 2;
		m_cList.SetItem(&lvi);

		CString strT;
		strT.Format(_T("%u"), p->dwPluginID);
		lvi.pszText = const_cast<LPTSTR>((LPCTSTR)strT);
		lvi.iSubItem = 3;
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

	BCMenu* psub = NewGUI_GetBCMenu(m_popmenu.GetSubMenu(0));
	if(psub == NULL) { ASSERT(FALSE); psub = &m_popmenu; }

	// psub->ModifyODMenu(NULL, (UINT)ID_PLUGIN_ENABLE, m_pImgList, 2);
	// psub->ModifyODMenu(NULL, (UINT)ID_PLUGIN_DISABLE, m_pImgList, 45);
	psub->ModifyODMenu(NULL, (UINT)ID_PLUGIN_CONFIG, m_pImgList, 21);
	psub->ModifyODMenu(NULL, (UINT)ID_PLUGIN_ABOUT, m_pImgList, 22);

	CPwSafeDlg::_TranslateMenu(psub, FALSE, NULL);

	if(p != NULL)
	{
		// if(p->bEnabled == FALSE)
		// {
		//	psub->EnableMenuItem(ID_PLUGIN_DISABLE, MF_BYCOMMAND | MF_GRAYED);
		//	psub->EnableMenuItem(ID_PLUGIN_CONFIG, MF_BYCOMMAND | MF_GRAYED);
		//	psub->EnableMenuItem(ID_PLUGIN_ABOUT, MF_BYCOMMAND | MF_GRAYED);
		// }
		// else psub->EnableMenuItem(ID_PLUGIN_ENABLE, MF_BYCOMMAND | MF_GRAYED);

		if(p->hinstDLL == NULL)
		{
			psub->EnableMenuItem(ID_PLUGIN_CONFIG, MF_BYCOMMAND | MF_GRAYED);
			psub->EnableMenuItem(ID_PLUGIN_ABOUT, MF_BYCOMMAND | MF_GRAYED);
		}
	}
	else
	{
		// psub->EnableMenuItem(ID_PLUGIN_ENABLE, MF_BYCOMMAND | MF_GRAYED);
		// psub->EnableMenuItem(ID_PLUGIN_DISABLE, MF_BYCOMMAND | MF_GRAYED);
		psub->EnableMenuItem(ID_PLUGIN_CONFIG, MF_BYCOMMAND | MF_GRAYED);
		psub->EnableMenuItem(ID_PLUGIN_ABOUT, MF_BYCOMMAND | MF_GRAYED);
	}

	psub->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);

	m_popmenu.DestroyMenu();
}

DWORD CPluginsDlg::GetSelectedPluginID()
{
	for(int i = 0; i < m_cList.GetItemCount(); ++i)
	{
		const UINT uState = m_cList.GetItemState(i, LVIS_SELECTED);
		if((uState & LVIS_SELECTED) != 0)
		{
			TCHAR tszBuf[13];
			ZeroMemory(&tszBuf[0], 13 * sizeof(TCHAR));

			LV_ITEM lvi;
			ZeroMemory(&lvi, sizeof(LV_ITEM));

			lvi.iItem = i;
			lvi.iSubItem = 3;
			lvi.mask = LVIF_TEXT;
			lvi.pszText = tszBuf;
			lvi.cchTextMax = 12;
			m_cList.GetItem(&lvi);

			return static_cast<DWORD>(_ttol(lvi.pszText));
		}
	}

	return DWORD_MAX;
}

void CPluginsDlg::OnPluginConfig()
{
	const DWORD dwID = GetSelectedPluginID();
	ASSERT(dwID != DWORD_MAX); if(dwID == DWORD_MAX) return;
	m_pPiMgr->CallSinglePlugin(dwID, KPM_DIRECT_CONFIG, (LPARAM)m_hWnd, 0);
}

void CPluginsDlg::OnPluginAbout()
{
	const DWORD dwID = GetSelectedPluginID();
	ASSERT(dwID != DWORD_MAX); if(dwID == DWORD_MAX) return;
	m_pPiMgr->CallSinglePlugin(dwID, KPM_PLUGIN_INFO, (LPARAM)m_hWnd, 0);
}

LRESULT CPluginsDlg::OnXHyperLinkClicked(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	if(wParam == IDC_STATIC_HL_GETPLUGINS)
		ShellExecute(NULL, NULL, PWM_URL_PLUGINS, NULL, NULL, SW_SHOW);

	OnCancel();
	return 0;
}

void CPluginsDlg::OnBtnClickedPlgHelp()
{
	WU_OpenAppHelp(PWM_HELP_PLUGINS, m_hWnd);
}
