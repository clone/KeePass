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

#if !defined(AFX_PWSAFEDLG_H__206CC2C3_063D_11D8_BF16_0050BF14F5CC__INCLUDED_)
#define AFX_PWSAFEDLG_H__206CC2C3_063D_11D8_BF16_0050BF14F5CC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "PwSafe/PwManager.h"
#include "PwSafe/PwExport.h"

#include "NewGUI/NewGUICommon.h"
#include "NewGUI/KCWndUtil.h"
#include "NewGUI/KCSideBannerWnd.h"
#include "NewGUI/BCMenu.h"
#include "NewGUI/XPStyleButtonST.h"
#include "NewGUI/ThemeHelperST.h"
#include "NewGUI/CustomListCtrlEx.h"
#include "NewGUI/CustomTreeCtrlEx.h"
#include "NewGUI/SystemTray.h"
#include "NewGUI/SystemTrayEx.h"
#include "NewGUI/AutoRichEditCtrl.h"
#include "Util/PluginMgr.h"
#include "Util/SInstance.h"
#include "Util/SysDefEx.h"

#define _CALLPLUGINS(__c,__l,__w) m_piMgr.CallPlugins((__c),(LPARAM)(__l),(LPARAM)(__w))

#define GUI_GROUPLIST_EXT 170
// Standard Windows Dialog GUI_SPACER = 11
#define GUI_SPACER 4

#define APPWND_TIMER_ID         1
#define APPWND_TIMER_ID_UPDATER 2

#define ICOIDX_REMOVABLE 16
#define ICOIDX_FIXED 17
#define ICOIDX_REMOTE 18
#define ICOIDX_CDROM 19
#define ICOIDX_RAMDISK 20
#define ICOIDX_NODRIVE 21

// #define PWS_DEFAULT_SPLITTER_Y 270
#define PWS_DEFAULT_SPLITTER_Y 10

#define PWS_TAN_ENTRY      TRL("<TAN>")
#define PWS_NEW_ATTACHMENT _T(":: ")

#define WM_PLUGINS_FIRST (0x9FFF)
#define WM_PLUGINS_LAST  (0xAFFF)

#define HOTKEYID_AUTOTYPE 33
#define HOTKEYID_RESTORE  34

#define CM_TIMED 0
#define CM_ENHSECURE 1

// Show expired / soon-to-expire entries at database open, flags
#define AUTOEXPIRE_NOSHOW  0
#define AUTOEXPIRE_EXPIRED 1
#define AUTOEXPIRE_SOONTO  2

/////////////////////////////////////////////////////////////////////////////

class CPP_CLASS_SHARE CPwSafeDlg : public CDialog
{
public:
	CPwSafeDlg(CWnd* pParent = NULL);

	void OnUpdateFlush(CMenu *pMenu); // BCMenu function

	static void _TranslateMenu(BCMenu *pBCMenu, BOOL bAppendSuffix = TRUE);
	static const TCHAR *_GetCmdAccelExt(const TCHAR *psz);
	void RestartApplication();

	void ProcessResize();
	void CleanUp();
	void SaveOptions();
	void SetStatusTextEx(LPCTSTR lpStatusText, int nPane = -1);
	inline void NotifyUserActivity();
	void UpdateAutoSortMenuItems();
	void BuildPluginMenu();
	BOOL RegisterGlobalHotKey(int nHotKeyID, DWORD dwHotKey, BOOL bReleasePrevious, BOOL bMessageBoxOnFail);
	void _AutoType(PW_ENTRY *pEntry, BOOL bLoseFocus);

	void UpdateGroupList();
	void UpdatePasswordList();
	void RefreshPasswordList(); // Refresh entries by UUID

	DWORD GetSelectedEntry();
	DWORD GetSelectedEntriesCount();
	DWORD GetSelectedGroupId();

	BOOL GetExportOptions(PWEXPORT_OPTIONS *pOptions, CPwExport *pPwExport);
	CString GetExportFile(int nFormat, LPCTSTR lpFileName = NULL);
	void ExportSelectedGroup(int nFormat);

	CString _MakeRtfString(LPCTSTR lptString);
	void ShowEntryDetails(PW_ENTRY *p);

	HTREEITEM _GetLastGroupItem(CTreeCtrl *pTree);
	void GroupSyncStates(BOOL bGuiToMgr = TRUE);

	HTREEITEM _GroupIdToHTreeItem(DWORD dwGroupId);

	void _ProcessGroupKey(UINT nChar, UINT nFlags);
	void _ProcessListKey(UINT nChar, BOOL bAlt);

	void _OnPwlistColumnWidthChange(int icolumn = -1, int isize = -1);
	void _SortListIfAutoSort();

	void _HandleEntryDrop(DWORD dwDropType, HTREEITEM hTreeItem);

	void _List_SaveView() { m_nSaveView = m_cList.GetTopIndex(); }
	void _List_RestoreView() // Restore the previous password list view
	{
		int nItemCount = m_cList.GetItemCount();

		if(nItemCount <= 0) return;

		m_cList.EnsureVisible(nItemCount - 1, FALSE);

		if(m_nSaveView < nItemCount)
			m_cList.EnsureVisible(m_nSaveView, FALSE);
	}
	void _Groups_SaveView(BOOL bSaveSelection = TRUE)
	{
		m_dwGroupsSaveFirstVisible = m_cGroups.GetItemData(m_cGroups.GetFirstVisibleItem());

		m_dwGroupsSaveSelected = DWORD_MAX;
		if(bSaveSelection == TRUE)
		{
			HTREEITEM h = m_cGroups.GetSelectedItem();
			if(h != NULL) m_dwGroupsSaveSelected = m_cGroups.GetItemData(h);
		}
	}
	void _Groups_RestoreView()
	{
		HTREEITEM h = _GroupIdToHTreeItem(m_dwGroupsSaveFirstVisible);
		if(h != NULL) m_cGroups.SelectSetFirstVisible(h);

		if(m_dwGroupsSaveSelected != DWORD_MAX)
		{
			h = _GroupIdToHTreeItem(m_dwGroupsSaveSelected);
			if(h != NULL) m_cGroups.SelectItem(h);
		}
	}
	void _CalcColumnSizes()
	{
		RECT rect;
		int nColumnWidth, nColumns = 0, i;
		for(i = 0; i < 11; i++)
		{
			if(m_bShowColumn[i] == TRUE) nColumns++;
			m_nColumnWidths[i] = 0;
		}
		if(nColumns == 0) return;
		m_cList.GetClientRect(&rect);
		nColumnWidth = ((rect.right - 9) / nColumns) - (GetSystemMetrics(SM_CXVSCROLL) / nColumns);
		for(i = 0; i < 11; i++)
			if(m_bShowColumn[i] == TRUE)
				m_nColumnWidths[i] = nColumnWidth;
	}
	void _SetColumnWidths()
	{
		for(int i = 0; i < 11; i++) m_cList.SetColumnWidth(i, m_nColumnWidths[i]);
	}
	void _SetListParameters()
	{
		LPARAM dw;
		dw = LVS_EX_SI_REPORT | LVS_EX_FULLROWSELECT | LVS_EX_ONECLICKACTIVATE | LVS_EX_UNDERLINEHOT;
		dw |= LVS_EX_HEADERDRAGDROP | LVS_EX_INFOTIP;
		if(m_bEntryGrid == TRUE) dw |= LVS_EX_GRIDLINES;
		if(m_dwOldListParameters != dw)
			m_cList.PostMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dw);
		m_dwOldListParameters = dw;
	}

	BOOL _RemoveSearchGroup();

	void _SelChangeView(UINT uID);
	void _List_SetEntry(DWORD dwInsertPos, PW_ENTRY *pwe, BOOL bIsNewEntry, PW_TIME *ptNow);
	DWORD _ListSelToEntryIndex(DWORD dwSelected = DWORD_MAX);
	DWORD _EntryUuidToListPos(BYTE *pUuid);

	void _DeleteTemporaryFiles();

	void _UpdateToolBar(BOOL bForceUpdate = FALSE);

	void _UpdateGuiToManager();

	void _TouchGroup(DWORD dwGroupId, BOOL bEdit);
	void _TouchEntry(DWORD dwListIndex, BOOL bEdit);

	void _SyncSubTree(CTreeCtrl *pTree, HTREEITEM hItem, BOOL bGuiToMgr);
	void _SyncItem(CTreeCtrl *pTree, HTREEITEM hItem, BOOL bGuiToMgr);

	void _SortList(DWORD dwByField, BOOL bAutoSortCall = FALSE);
	BOOL _CheckIfCanSort();

	HTREEITEM _FindSelectInTree(CTreeCtrl *pTree, HTREEITEM hRoot, DWORD dwGroupId);

	BOOL _IsUnsafeAllowed();

	void _OpenDatabase(const TCHAR *pszFile, const TCHAR *pszPassword, const TCHAR *pszKeyFile, BOOL bOpenLocked, LPCTSTR lpPreSelectPath);
	BOOL _ChangeMasterKey(BOOL bCreateNew);
	void _PrintGroup(DWORD dwGroupId);
	void _Find(DWORD dwFindGroupId);
	void _DoQuickFind();
	void _HandleSelectAll();
	void _ShowExpiredEntries(BOOL bShowIfNone, BOOL bShowExpired, BOOL bShowSoonToExpire);

	BOOL _ParseCommandLine();
	void _ParseSpecAndSetFont(const TCHAR *pszSpec);

	void _ShowToolBar(BOOL bShow = TRUE);
	void _EnableViewMenuItems(BCMenu *pMenu);

	void _FinishDragging(BOOL bDraggingImageList);

	int m_nClipboardMethod;
	BOOL m_bTimer;
	int m_nClipboardCountdown;
	int m_nClipboardState;
	HWND m_hwndNextViewer;
	BOOL m_bOpenLastDb;
	CString m_strLastDb;
	BOOL m_bImgButtons;
	BOOL m_bEntryGrid;
	BOOL m_bAutoSaveDb;
	long m_nLockTimeDef;
	long m_nLockCountdown;
	BOOL m_bAutoShowExpired;
	BOOL m_bAutoShowExpiredSoon;
	BOOL m_bBackupEntries;
	static BOOL m_bSecureEdits;
	BOOL m_bSingleClickTrayIcon;
	DWORD m_dwDefaultExpire;

	BOOL m_bExiting;
	BOOL m_bLocked;
	long m_nLockedViewParams[3];
	BOOL m_bShowWindow;
	BOOL m_bWasMaximized;
	BOOL m_bRestartApplication;

	BOOL m_bCheckForInstance;
	CInstanceChecker m_instanceChecker;

	CPwManager m_mgr;
	CPluginManager m_piMgr;

	BOOL m_bWindowsNewLine;
	BOOL m_bPasswordStars;
	BOOL m_bUserStars;
	BOOL m_bDisableUnsafe;
	BOOL m_bDisableUnsafeAtStart;
	BOOL m_bRememberLast;
	BOOL m_bUsePuttyForURLs;
	BOOL m_bSaveOnLATMod;
	BOOL m_bStartMinimized;
	BOOL m_bQuickFindIncBackup;

	CStatusBarCtrl m_sbStatus;
	BOOL m_bShowToolBar;

	BOOL m_bAlwaysOnTop;
	BOOL m_bShowTitle;
	BOOL m_bShowUserName;
	BOOL m_bShowURL;
	BOOL m_bShowPassword;
	BOOL m_bShowNotes;
	BOOL m_bShowCreation;
	BOOL m_bShowLastMod;
	BOOL m_bShowLastAccess;
	BOOL m_bShowExpire;
	BOOL m_bShowUUID;
	BOOL m_bShowAttach;
	BOOL m_bLockOnMinimize;
	BOOL m_bMinimizeToTray;
	BOOL m_bCloseMinimizes;
	BOOL m_bEntryView;
	BOOL m_bColAutoSize;
	int m_nAutoSort;
	BOOL m_bAutoPwGen;

	HICON m_hTrayIconNormal;
	HICON m_hTrayIconLocked;
	BOOL m_bDisplayDialog;

	BCMenu m_menu; // Our XP-style menu
	BOOL m_bMenu; // Menu created?

	BCMenu m_popmenu;
	BCMenu m_menuColView;
	CToolTipCtrl m_tip;

	CImageList m_ilIcons;
	CFont m_fListFont;
	CSystemTrayEx m_systray;
	HACCEL m_hAccel;

	CString m_strFile;
	BOOL m_bFileOpen;
	BOOL m_bModified;
	BOOL m_bMinimized;
	BOOL m_bMaximized;

	CString m_strTempFile;

	BOOL m_bShowColumn[11];
	int m_nColumnWidths[11];

	DWORD m_dwClipboardSecs;
	CString m_strFontSpec;
	CString m_strListFontFace;
	int m_nListFontSize;
	DWORD m_dwATHotKey;

	DWORD m_dwLastNumSelectedItems;
	DWORD m_dwLastFirstSelectedItem;
	HTREEITEM m_hLastSelectedGroup;

	BOOL m_bHashValid;
	BYTE m_aHashOfFile[32];

	UINT m_uACP;

	HICON m_hIcon;
	CThemeHelperST *m_pThemeHelper;

	int m_nSaveView;
	LPARAM m_dwOldListParameters;
	BYTE m_pPreLockItemUuid[16];
	ULONGLONG m_ullLastListParams;
	UINT m_uOriginalExtrasMenuItemCount;

	DWORD m_dwGroupsSaveFirstVisible;
	DWORD m_dwGroupsSaveSelected;

	BOOL m_bCachedToolBarUpdate;
	BOOL m_bCachedPwlistUpdate;
	BOOL m_bBlockPwListUpdate;

	BOOL m_bDragging;
	BOOL m_bDraggingHoriz;

	HTREEITEM m_hDraggingGroup;
	BOOL m_bCanDragGroup;

	HCURSOR m_hArrowCursor;
	HCURSOR m_hDragLeftRight;
	HCURSOR m_hDragUpDown;

	LONG m_lSplitterPosHoriz;
	LONG m_lSplitterPosVert;

	INT m_aHeaderOrder[11];

	BOOL m_bMenuExit;

	//{{AFX_DATA(CPwSafeDlg)
	enum { IDD = IDD_PWSAFE_DIALOG };
	CStatic	m_stcMenuLine;
	CEdit	m_cQuickFind;
	CCustomTreeCtrlEx	m_cGroups;
	CXPStyleButtonST	m_btnTbNew;
	CXPStyleButtonST	m_btnTbLock;
	CXPStyleButtonST	m_btnTbFind;
	CXPStyleButtonST	m_btnTbEditEntry;
	CXPStyleButtonST	m_btnTbDeleteEntry;
	CXPStyleButtonST	m_btnTbCopyUser;
	CXPStyleButtonST	m_btnTbCopyPw;
	CXPStyleButtonST	m_btnTbAddEntry;
	CXPStyleButtonST	m_btnTbAbout;
	CXPStyleButtonST	m_btnTbSave;
	CXPStyleButtonST	m_btnTbOpen;
	CCustomListCtrlEx	m_cList;
	CAutoRichEditCtrl	m_reEntryView;
	CString	m_strQuickFind;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CPwSafeDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

protected:
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
	afx_msg void OnGroupMoveTop();
	afx_msg void OnUpdateGroupMoveTop(CCmdUI* pCmdUI);
	afx_msg void OnGroupMoveBottom();
	afx_msg void OnUpdateGroupMoveBottom(CCmdUI* pCmdUI);
	afx_msg void OnGroupMoveUp();
	afx_msg void OnUpdateGroupMoveUp(CCmdUI* pCmdUI);
	afx_msg void OnGroupMoveDown();
	afx_msg void OnUpdateGroupMoveDown(CCmdUI* pCmdUI);
	afx_msg LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateClipboard(WPARAM wParam, LPARAM lParam);
	afx_msg void OnViewHide();
	afx_msg void OnImportCsv();
	afx_msg void OnUpdateImportCsv(CCmdUI* pCmdUI);
	afx_msg void OnClickPwlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnImportCWallet();
	afx_msg void OnUpdateImportCWallet(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileNew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
	afx_msg void OnImportPwSafe();
	afx_msg void OnUpdateImportPwSafe(CCmdUI* pCmdUI);
	afx_msg void OnViewCreation();
	afx_msg void OnViewLastMod();
	afx_msg void OnViewLastAccess();
	afx_msg void OnViewExpire();
	afx_msg void OnViewUuid();
	afx_msg void OnTbOpen();
	afx_msg void OnTbSave();
	afx_msg void OnTbNew();
	afx_msg void OnTbCopyUser();
	afx_msg void OnTbCopyPw();
	afx_msg void OnTbAddEntry();
	afx_msg void OnTbEditEntry();
	afx_msg void OnTbDeleteEntry();
	afx_msg void OnTbFind();
	afx_msg void OnTbLock();
	afx_msg void OnTbAbout();
	afx_msg void OnViewShowToolBar();
	afx_msg void OnPwlistMassModify();
	afx_msg void OnUpdatePwlistMassModify(CCmdUI* pCmdUI);
	afx_msg void OnKeyDownPwlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnViewEntryView();
	afx_msg void OnReCopySel();
	afx_msg void OnReCopyAll();
	afx_msg void OnReSelectAll();
	afx_msg void OnExtrasTanWizard();
	afx_msg void OnUpdateExtrasTanWizard(CCmdUI* pCmdUI);
	afx_msg void OnFilePrintPreview();
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnInfoTranslation();
	afx_msg void OnSafeAddSubgroup();
	afx_msg void OnUpdateSafeAddSubgroup(CCmdUI* pCmdUI);
	afx_msg void OnBeginDragGrouplist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCancelMode();
	afx_msg void OnGroupSort();
	afx_msg void OnUpdateGroupSort(CCmdUI* pCmdUI);
	afx_msg void OnPwlistSortTitle();
	afx_msg void OnUpdatePwlistSortTitle(CCmdUI* pCmdUI);
	afx_msg void OnPwlistSortUser();
	afx_msg void OnUpdatePwlistSortUser(CCmdUI* pCmdUI);
	afx_msg void OnPwlistSortUrl();
	afx_msg void OnUpdatePwlistSortUrl(CCmdUI* pCmdUI);
	afx_msg void OnPwlistSortPassword();
	afx_msg void OnUpdatePwlistSortPassword(CCmdUI* pCmdUI);
	afx_msg void OnPwlistSortNotes();
	afx_msg void OnUpdatePwlistSortNotes(CCmdUI* pCmdUI);
	afx_msg void OnPwlistSortCreation();
	afx_msg void OnUpdatePwlistSortCreation(CCmdUI* pCmdUI);
	afx_msg void OnPwlistSortLastmodify();
	afx_msg void OnUpdatePwlistSortLastmodify(CCmdUI* pCmdUI);
	afx_msg void OnPwlistSortLastaccess();
	afx_msg void OnUpdatePwlistSortLastaccess(CCmdUI* pCmdUI);
	afx_msg void OnPwlistSortExpire();
	afx_msg void OnUpdatePwlistSortExpire(CCmdUI* pCmdUI);
	afx_msg void OnGroupMoveLeft();
	afx_msg void OnUpdateGroupMoveLeft(CCmdUI* pCmdUI);
	afx_msg void OnGroupMoveRight();
	afx_msg void OnUpdateGroupMoveRight(CCmdUI* pCmdUI);
	afx_msg void OnViewHideUsers();
	afx_msg void OnViewAttach();
	afx_msg void OnPwlistSaveAttach();
	afx_msg void OnUpdatePwlistSaveAttach(CCmdUI* pCmdUI);
	afx_msg void OnFileShowDbInfo();
	afx_msg void OnUpdateFileShowDbInfo(CCmdUI* pCmdUI);
	afx_msg void OnExtrasShowExpired();
	afx_msg void OnUpdateExtrasShowExpired(CCmdUI* pCmdUI);
	afx_msg void OnImportPvault();
	afx_msg void OnUpdateImportPvault(CCmdUI* pCmdUI);
	afx_msg void OnSafeExportGroupTxt();
	afx_msg void OnUpdateSafeExportGroupTxt(CCmdUI* pCmdUI);
	afx_msg void OnPwlistSelectAll();
	afx_msg void OnUpdatePwlistSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnViewAutosortCreation();
	afx_msg void OnViewAutosortExpire();
	afx_msg void OnViewAutosortLastaccess();
	afx_msg void OnViewAutosortLastmodify();
	afx_msg void OnViewAutosortNosort();
	afx_msg void OnViewAutosortNotes();
	afx_msg void OnViewAutosortPassword();
	afx_msg void OnViewAutosortTitle();
	afx_msg void OnViewAutosortUrl();
	afx_msg void OnViewAutosortUser();
	afx_msg void OnUpdateSafeOptions(CCmdUI* pCmdUI);
	afx_msg void OnPwlistAutoType();
	afx_msg void OnUpdatePwlistAutoType(CCmdUI* pCmdUI);
	afx_msg void OnColumnClickPwlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnExtrasPluginMgr();
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
	afx_msg void OnImportGetMore();
	afx_msg void OnInfoDonate();
	//}}AFX_MSG

	afx_msg void OnPluginMessage(UINT nID);
	afx_msg void OnUpdatePluginMessage(CCmdUI* pCmdUI);
	afx_msg void OnRenderFormat(UINT nFormat);
	afx_msg void OnRenderAllFormats();
	afx_msg void OnChangeCbChain(HWND hWndRemove, HWND hWndAfter);
	afx_msg void OnDrawClipboard();

	afx_msg void OnEndSession(BOOL bEnding);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PWSAFEDLG_H__206CC2C3_063D_11D8_BF16_0050BF14F5CC__INCLUDED_)
