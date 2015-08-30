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

#if !defined(AFX_PLUGINSDLG_H__ADE71601_3FDF_11D9_BF17_0050BF14F5CC__INCLUDED_)
#define AFX_PLUGINSDLG_H__ADE71601_3FDF_11D9_BF17_0050BF14F5CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../KeePassLibCpp/SysDefEx.h"
#include "PwSafeDlg.h"
#include "Plugins/PluginMgr.h"
#include "NewGUI/XPStyleButtonST.h"
#include "NewGUI/BCMenu.h"
#include "NewGUI/KCSideBannerWnd.h"
#include "NewGUI/XHyperLink.h"

/////////////////////////////////////////////////////////////////////////////

class CPluginsDlg : public CDialog
{
public:
	CPluginsDlg(CWnd* pParent = NULL);

	void UpdateGUI();

	DWORD GetSelectedPluginID();

	CPluginManager *m_pPiMgr;
	CImageList *m_pImgList;

	BCMenu m_popmenu;
	CKCSideBannerWnd m_banner;

	//{{AFX_DATA(CPluginsDlg)
	enum { IDD = IDD_PLUGINS_DLG };
	CXHyperLink	m_hlHelp;
	CXHyperLink	m_hlGetPlugins;
	CXPStyleButtonST	m_btClose;
	CListCtrl	m_cList;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CPluginsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CPluginsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnRClickPluginsList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPluginEnable();
	afx_msg void OnPluginDisable();
	afx_msg void OnPluginConfig();
	afx_msg void OnPluginAbout();
	//}}AFX_MSG

	afx_msg LRESULT OnXHyperLinkClicked(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PLUGINSDLG_H__ADE71601_3FDF_11D9_BF17_0050BF14F5CC__INCLUDED_)
