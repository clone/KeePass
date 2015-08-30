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

#if !defined(AFX_PLUGINSDLG_H__ADE71601_3FDF_11D9_BF17_0050BF14F5CC__INCLUDED_)
#define AFX_PLUGINSDLG_H__ADE71601_3FDF_11D9_BF17_0050BF14F5CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Util/SysDefEx.h"
#include "PwSafeDlg.h"
#include "Util/PluginMgr.h"
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
