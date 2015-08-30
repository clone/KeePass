/*
  Copyright (c) 2003, Dominik Reichl <dominik.reichl@t-online.de>
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

#if !defined(AFX_PWSAFEDLG_H__206CC2C3_063D_11D8_BF16_0050BF14F5CC__INCLUDED_)
#define AFX_PWSAFEDLG_H__206CC2C3_063D_11D8_BF16_0050BF14F5CC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "PwSafe/PwManager.h"

#include "NewGUI/NewGUICommon.h"
#include "NewGUI/KCWndUtil.h"
#include "NewGUI/KCSideBannerWnd.h"
#include "NewGUI/BCMenu.h"
#include "NewGUI/CustomListCtrlEx.h"

#define GUI_GROUPLIST_EXT 170
// Standard Windows Dialog GUI_SPACER = 11
#define GUI_SPACER 4

#define APPWND_TIMER_ID 1

#define ICOIDX_REMOVABLE 16
#define ICOIDX_FIXED 17
#define ICOIDX_REMOTE 18
#define ICOIDX_CDROM 19
#define ICOIDX_RAMDISK 20
#define ICOIDX_NODRIVE 21

#define PWS_SEARCHGROUP "Search results"

/////////////////////////////////////////////////////////////////////////////

class CPwSafeDlg : public CDialog
{
public:
	CPwSafeDlg(CWnd* pParent = NULL);

	void OnUpdateFlush(CMenu *pMenu); // BCMenu function

	void ProcessResize();
	void CleanUp();
	void _DeleteTemporaryFiles();
	void _ParseCommandLine();

	void UpdateGroupList();
	void UpdatePasswordList();

	int GetSelectedEntry();
	int GetSelectedGroup();
	int GetSafeSelectedGroup();

	CString GetExportFile(int nFormat);
	void ExportSelectedGroup(int nFormat);

	void _OpenDatabase(const TCHAR *pszFile);
	void _PrintGroup(int nGroup);
	void _Find(int nGroupIdX);

	BOOL m_bTimer;
	int m_nClipboardCountdown;
	BOOL m_bOpenLastDb;
	CString m_strLastDb;
	BOOL m_bImgButtons;

	BOOL m_bLocked;
	long m_nLockedViewParams[3];

	CPwManager m_mgr;

	BOOL m_bWindowsNewLine;
	BOOL m_bPasswordStars;

	CStatusBarCtrl m_sbStatus;

	BOOL m_bAlwaysOnTop;
	BOOL m_bShowTitle;
	BOOL m_bShowUserName;
	BOOL m_bShowURL;
	BOOL m_bShowPassword;
	BOOL m_bShowNotes;

	BCMenu m_menu; // Our XP-style menu
	BOOL m_bMenu; // Menu created?

	BCMenu m_popmenu;

	CImageList m_ilIcons;

	CString m_strFile;
	BOOL m_bFileOpen;
	BOOL m_bModified;

	CString m_strTempFile;

	BOOL m_bShowColumn[5];
	int m_nColumnWidths[5];

	DWORD m_dwClipboardSecs;

	//{{AFX_DATA(CPwSafeDlg)
	enum { IDD = IDD_PWSAFE_DIALOG };
	CStatic	m_stcMenuLine;
	CListCtrl	m_cGroups;
	CCustomListCtrlEx	m_cList;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CPwSafeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	void _List_SaveView() { m_nSaveView = m_cList.GetTopIndex(); }
	void _List_RestoreView() // Restore the previous password list view
	{
		m_cList.EnsureVisible(m_cList.GetItemCount() - 1, FALSE);
		m_cList.EnsureVisible(m_nSaveView, FALSE);
	}
	void _TranslateMenu(BCMenu *pBCMenu); // Translate this menu
	void _CalcColumnSizes()
	{
		RECT rect;
		int nColumnWidth, nColumns = 0, i;
		for(i = 0; i < 5; i++)
		{
			if(m_bShowColumn[i] == TRUE) nColumns++;
			m_nColumnWidths[i] = 0;
		}
		if(nColumns == 0) return;
		m_cList.GetClientRect(&rect);
		nColumnWidth = ((rect.right - 9) / nColumns) - (GetSystemMetrics(SM_CXVSCROLL) / nColumns);
		for(i = 0; i < 5; i++)
			if(m_bShowColumn[i] == TRUE)
				m_nColumnWidths[i] = nColumnWidth;
	}
	void _SetColumnWidths()
	{
		m_cList.SetColumnWidth(0, m_nColumnWidths[0]);
		m_cList.SetColumnWidth(1, m_nColumnWidths[1]);
		m_cList.SetColumnWidth(2, m_nColumnWidths[2]);
		m_cList.SetColumnWidth(3, m_nColumnWidths[3]);
		m_cList.SetColumnWidth(4, m_nColumnWidths[4]);
	}

	HICON m_hIcon;

	int m_nSaveView;

	//{{AFX_MSG(CPwSafeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnFileExit();
	afx_msg void OnInfoAbout();
	afx_msg void OnSafeAddGroup();
	afx_msg void OnViewHideStars();
	afx_msg void OnPwlistAdd();
	afx_msg void OnPwlistEdit();
	afx_msg void OnPwlistDelete();
	afx_msg void OnRclickPwlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickGroupList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPwlistCopyPw();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDblclkPwlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickGroupList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPwlistCopyUser();
	afx_msg void OnPwlistVisitUrl();
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileClose();
	afx_msg void OnSafeOptions();
	afx_msg void OnSafeRemoveGroup();
	afx_msg void OnFileChangeMasterPw();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileChangeMasterPw(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileClose(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePwlistCopyPw(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePwlistCopyUser(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePwlistDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePwlistEdit(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePwlistVisitUrl(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSafeRemoveGroup(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSafeAddGroup(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePwlistAdd(CCmdUI* pCmdUI);
	afx_msg void OnExportTxt();
	afx_msg void OnExportHtml();
	afx_msg void OnExportXml();
	afx_msg void OnExportCsv();
	afx_msg void OnUpdateExportTxt(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExportHtml(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExportXml(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExportCsv(CCmdUI* pCmdUI);
	afx_msg void OnFilePrint();
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnExtrasGenPw();
	afx_msg void OnSafeModifyGroup();
	afx_msg void OnUpdateSafeModifyGroup(CCmdUI* pCmdUI);
	afx_msg void OnPwlistFind();
	afx_msg void OnUpdatePwlistFind(CCmdUI* pCmdUI);
	afx_msg void OnPwlistFindInGroup();
	afx_msg void OnUpdatePwlistFindInGroup(CCmdUI* pCmdUI);
	afx_msg void OnPwlistDuplicate();
	afx_msg void OnUpdatePwlistDuplicate(CCmdUI* pCmdUI);
	afx_msg void OnInfoHomepage();
	afx_msg void OnViewAlwaysOnTop();
	afx_msg void OnSafeExportGroupHtml();
	afx_msg void OnSafeExportGroupXml();
	afx_msg void OnSafeExportGroupCsv();
	afx_msg void OnUpdateSafeExportGroupHtml(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSafeExportGroupXml(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSafeExportGroupCsv(CCmdUI* pCmdUI);
	afx_msg void OnSafePrintGroup();
	afx_msg void OnUpdateSafePrintGroup(CCmdUI* pCmdUI);
	afx_msg void OnPwlistMoveUp();
	afx_msg void OnUpdatePwlistMoveUp(CCmdUI* pCmdUI);
	afx_msg void OnPwlistMoveTop();
	afx_msg void OnUpdatePwlistMoveTop(CCmdUI* pCmdUI);
	afx_msg void OnPwlistMoveDown();
	afx_msg void OnUpdatePwlistMoveDown(CCmdUI* pCmdUI);
	afx_msg void OnPwlistMoveBottom();
	afx_msg void OnUpdatePwlistMoveBottom(CCmdUI* pCmdUI);
	afx_msg void OnBeginDragPwlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFileChangeLanguage();
	afx_msg void OnInfoReadme();
	afx_msg void OnInfoLicense();
	afx_msg void OnInfoPrintLicense();
	afx_msg void OnViewTitle();
	afx_msg void OnViewUsername();
	afx_msg void OnViewUrl();
	afx_msg void OnViewPassword();
	afx_msg void OnViewNotes();
	afx_msg void OnFileLock();
	afx_msg void OnUpdateFileLock(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PWSAFEDLG_H__206CC2C3_063D_11D8_BF16_0050BF14F5CC__INCLUDED_)
