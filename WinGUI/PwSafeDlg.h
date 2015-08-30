/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2013 Dominik Reichl <dominik.reichl@t-online.de>

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

#include "../KeePassLibCpp/PwManager.h"
#include "../KeePassLibCpp/DataExchange/PwExport.h"
#include "../KeePassLibCpp/PasswordGenerator/PasswordGenerator.h"

#include <map>

#include "NewGUI/NewGUICommon.h"
#include "NewGUI/KCWndUtil.h"
#include "NewGUI/KCSideBannerWnd.h"
#include "NewGUI/BCMenu.h"
#include "NewGUI/XPStyleButtonST.h"
#include "NewGUI/ThemeHelperST.h"
#include "NewGUI/CustomListCtrlEx.h"
#include "NewGUI/CustomTreeCtrlEx.h"
#include "NewGUI/SystemTrayEx.h"
#include "NewGUI/AutoRichEditCtrlFx.h"
#include "Plugins/PluginMgr.h"
#include "Util/PrivateConfigEx.h"
#include "Util/SInstance.h"
#include "Util/SessionNotify.h"
#include "Util/RemoteControl.h"
#include "../KeePassLibCpp/SysDefEx.h"
#include "afxwin.h"

#define GUI_GROUPLIST_EXT 170
// Standard Windows Dialog GUI_SPACER = 11
#define GUI_SPACER 4

#define APPWND_TIMER_ID         1
#define APPWND_TIMER_ID_UPDATER 2

#define ICOIDX_GENERIC_KEYPROVIDER 10
#define ICOIDX_REMOVABLE 16
#define ICOIDX_FIXED 17
#define ICOIDX_REMOTE 18
#define ICOIDX_CDROM 19
#define ICOIDX_RAMDISK 20
#define ICOIDX_NODRIVE 21

// #define PWS_DEFAULT_SPLITTER_Y 270
#define PWS_DEFAULT_SPLITTER_Y 10
#define MENU_EXTRAS_ITEMCOUNT  8

#define PWS_NEW_ATTACHMENT _T(":: ")

#define WM_PLUGINS_FIRST   (0x9FFF)
#define WM_PLUGINS_LAST    (0xAFFF)

#define HOTKEYID_AUTOTYPE  33
#define HOTKEYID_RESTORE   34

#define CM_TIMED 0
#define CM_ENHSECURE 1

// Show expired / soon-to-expire entries at database open, flags
#define AUTOEXPIRE_NOSHOW  0
#define AUTOEXPIRE_EXPIRED 1
#define AUTOEXPIRE_SOONTO  2

// Auto-type methods
#define ATM_DROPBACK       0
#define ATM_MINIMIZE       1

#define LVSX_CHANGING      DWORD_MAX

#define KPCM_NULL          0
#define KPCM_EXIT          1
#define KPCM_AUTOTYPE      2
#define KPCM_LOCK          3
#define KPCM_UNLOCK        4

#define MWS_MIN_CX 314
#define MWS_MIN_CY 207

#define HTMLTAG_BOLD_S   _T("<b>")
#define HTMLTAG_BOLD_E   _T("</b>")
#define HTMLTAG_ITALIC_S _T("<i>")
#define HTMLTAG_ITALIC_E _T("</i>")
#define HTMLTAG_ULINE_S  _T("<u>")
#define HTMLTAG_ULINE_E  _T("</u>")
#define HTMLTAG_STRIKE_S _T("<s>")
#define HTMLTAG_STRIKE_E _T("</s>")

/////////////////////////////////////////////////////////////////////////////

class CPwSafeDlg : public CDialog
{
public:
	CPwSafeDlg(CWnd* pParent = NULL);

	void OnUpdateFlush(CMenu *pMenu); // BCMenu function

	static void _TranslateMenu(BCMenu *pBCMenu, BOOL bAppendSuffix, BOOL *pFlags);
	static const TCHAR *_GetCmdAccelExt(const TCHAR *psz);
	void RestartApplication();
	static CString _GetSecureEditTipText(const TCHAR *tszBase);
	void RebuildContextMenus();
	void DeleteContextMenus();
	void _SetDefaultFocus();

	void ProcessResize();
	void CleanUp();

	void SaveOptions();
	void _SaveWindowPositionAndSize(CPrivateConfigEx* pcfg);

	void SetStatusTextEx(LPCTSTR lpStatusText, int nPane = -1);
	void NotifyUserActivity();
	void UpdateAutoSortMenuItems();
	void BuildPluginMenu();
	BOOL RegisterGlobalHotKey(int nHotKeyID, DWORD dwHotKey, BOOL bReleasePrevious, BOOL bMessageBoxOnFail);
	void AdjustPwListMode();
	void AdjustColumnWidths();

	bool _IsDisplayingDialog();
	void _SetDisplayDialog(bool bDisplay);
	bool _IsDisplayingMenu();
	void _SetDisplayMenu(bool bDisplay);
	void _AssertDisplayCounts(int cDialogs, int cMenus);

	void ParseAndOpenURLWithEntryInfo(LPCTSTR lpURL, PW_ENTRY *pEntry);
	void _AutoType(PW_ENTRY *pEntry, BOOL bLoseFocus, DWORD dwAutoTypeSeq,
		LPCTSTR lpTargetWindowName);

	void UpdateGroupList();
	void UpdatePasswordList();
	void RefreshPasswordList(); // Refresh entries by UUID

	void _ChangeLockState(BOOL bLocked);

	DWORD GetSelectedEntry();
	std::vector<DWORD> GetSelectedEntriesUIIndices();
	std::vector<DWORD> GetSelectedEntriesMgrIndices();
	DWORD GetSelectedEntriesCount();
	DWORD GetSelectedGroupId();

	BOOL GetExportOptions(PWEXPORT_OPTIONS *pOptions, CPwExport *pPwExport,
		BOOL bPrinting);
	CString GetExportFile(int nFormat, LPCTSTR lpBaseFileName, BOOL bFixFileName);
	void ExportSelectedGroup(int nFormat);
	void ExportGroupToKeePass(DWORD dwGroupId);

	CString _MakeRtfString(LPCTSTR lptString);
	void ShowEntryDetails(PW_ENTRY *p);

	HTREEITEM _GetLastGroupItem(CTreeCtrl *pTree);
	void GroupSyncStates(BOOL bGuiToMgr = TRUE);

	HTREEITEM _GroupIdToHTreeItem(DWORD_PTR dwGroupId);

	void _ProcessGroupKey(UINT nChar, UINT nFlags);
	void _ProcessListKey(UINT nChar, BOOL bAlt);

	void CB_OnPwlistColumnWidthChange(int iColumn, int iSize);
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
		HTREEITEM hTop = m_cGroups.GetFirstVisibleItem();
		if(hTop != NULL) m_dwGroupsSaveFirstVisible = m_cGroups.GetItemData(hTop);
		else m_dwGroupsSaveFirstVisible = DWORD_MAX;

		m_dwGroupsSaveSelected = DWORD_MAX;
		if(bSaveSelection == TRUE)
		{
			HTREEITEM h = m_cGroups.GetSelectedItem();
			if(h != NULL) m_dwGroupsSaveSelected = m_cGroups.GetItemData(h);
		}
	}
	void _Groups_RestoreView()
	{
		HTREEITEM h;
		
		if(m_dwGroupsSaveFirstVisible != DWORD_MAX)
		{
			h = _GroupIdToHTreeItem(m_dwGroupsSaveFirstVisible);
			if(h != NULL) m_cGroups.SelectSetFirstVisible(h);
		}

		if(m_dwGroupsSaveSelected != DWORD_MAX)
		{
			h = _GroupIdToHTreeItem(m_dwGroupsSaveSelected);
			if(h != NULL) m_cGroups.SelectItem(h);
		}
	}

	void _CalcColumnSizes();
	void _SetColumnWidths();
	void _SetListParameters();

	bool _IsSearchGroup();
	BOOL _RemoveSearchGroup();
	void _DeleteBackupEntries();

	void _SelChangeView(UINT uID);
	void _List_SetEntry(DWORD dwInsertPos, PW_ENTRY *pwe, BOOL bIsNewEntry, PW_TIME *ptNow);
	DWORD _ListSelToEntryIndex(DWORD dwSelected = DWORD_MAX);
	DWORD _EntryUuidToListPos(BYTE *pUuid);

	void _DeleteTemporaryFiles();

	void _UpdateToolBar(BOOL bForceUpdate = FALSE);
	void _UpdateTitleBar();
	void _UpdateTrayIcon(bool bUpdateOnlyVisibility = false);

	void _UpdateGuiToManager();

	void _UpdateCachedGroupIDs();
	static void _SetLVItemParam(LV_ITEM* pItem, const PW_ENTRY* pe);

	void _UpdateSortMenuItemState(CCmdUI* pCmdUI);

	void _TouchGroup(DWORD dwGroupId, BOOL bEdit);
	void _TouchEntry(DWORD dwListIndex, BOOL bEdit);

	void _SyncSubTree(CTreeCtrl *pTree, HTREEITEM hItem, BOOL bGuiToMgr);
	void _SyncItem(CTreeCtrl *pTree, HTREEITEM hItem, BOOL bGuiToMgr);

	void _SortList(DWORD dwByField, BOOL bAutoSortCall);
	BOOL _CheckIfCanSort();

	HTREEITEM _FindSelectInTree(CTreeCtrl *pTree, HTREEITEM hRoot, DWORD_PTR dwGroupId);

	static BOOL IsUnsafeAllowed(HWND hWndParent);

	void _OpenDatabase(CPwManager *pDbMgr, const TCHAR *pszFile, const TCHAR *pszPassword,
		const TCHAR *pszKeyFile, BOOL bOpenLocked, LPCTSTR lpPreSelectPath,
		BOOL bIgnoreCorrupted);
	BOOL _ChangeMasterKey(CPwManager *pDbMgr, BOOL bCreateNew);
	void _PrintGroup(DWORD dwGroupId);
	void _Find(DWORD dwFindGroupId);
	void _DoQuickFind(LPCTSTR lpText);
	void _HandleSelectAll();
	void _ShowExpiredEntries(BOOL bShowIfNone, BOOL bShowExpired, BOOL bShowSoonToExpire);

	BOOL _ParseCommandLine();
	void _ParseSpecAndSetFont(const TCHAR *pszSpec, bool bNotes);

	void _ShowToolBar(BOOL bShow);
	void _EnableViewMenuItems(BCMenu *pMenu);

	void _FinishDragging(BOOL bDraggingImageList);

	static UINT GetKeePassControlMessageID();

	static void SetLastMasterKeyDlg(HWND h);

	BCMenu m_menu; // Our XP-style menu
	BOOL m_bMenu; // Menu created?

	CImageList m_ilIcons;

	BOOL m_bCheckForInstance;
	CInstanceChecker m_instanceChecker;

	HICON m_hTrayIconNormal;
	HICON m_hTrayIconLocked;
	HICON m_hLockOverlayIcon;

	BOOL m_bCachedToolBarUpdate;
	BOOL m_bCachedPwlistUpdate;

	CString m_strFile;
	CString m_strFileAbsolute;
	BOOL m_bFileOpen;
	BOOL m_bModified;
	BOOL m_bLocked;
	BOOL m_bFileReadOnly;
	BOOL m_bInitialCmdLineFile;

	CPwManager m_mgr;

	static BOOL m_bMiniMode;
	static BOOL m_bSecureEdits;
	static BOOL m_bDisableUnsafeAtStart;
	static BOOL m_bUseLocalTimeFormat;
	static PW_GEN_SETTINGS_EX m_pgsAutoProfile;
	static CString m_strDefaultAutoTypeSequence;
	static BOOL m_bSortAutoTypeSelItems;

	static BOOL m_bForceAllowChangeMasterKey;
	static BOOL m_bForceAllowPrinting;
	static BOOL m_bForceAllowImport;
	static BOOL m_bForceAllowExport;
	static BOOL m_bDisallowPrintingPasswords;

private:
	int m_nClipboardMethod;
	BOOL m_bTimer;
	int m_nClipboardCountdown;
	int m_nClipboardState;
	int m_nFileRelockCountdown;
	HWND m_hwndNextViewer;
	BOOL m_bOpenLastDb;
	CString m_strLastDb;
	BOOL m_bImgButtons;
	BOOL m_bEntryGrid;
	BOOL m_bAutoSaveDb;
	BOOL m_bAutoShowExpired;
	BOOL m_bAutoShowExpiredSoon;
	BOOL m_bBackupEntries;
	BOOL m_bSingleClickTrayIcon;
	BOOL m_bShowTrayOnlyIfTrayed;
	DWORD m_dwDefaultExpire;
	BOOL m_bDeleteBackupsOnSave;
	BOOL m_bDisableAutoType;
	BOOL m_bLockOnWinLock;
	BOOL m_bClearClipOnDbClose;
	BOOL m_bUseTransactedFileWrites;

	long m_nLockTimeDef;
	// long m_nLockCountdown;
	UINT64 m_uLockAt;

	BOOL m_bExiting;
	BOOL m_bIsLocking;
	long m_nLockedViewParams[3];
	BOOL m_bWasMaximized;
	BOOL m_bMinimized;
	BOOL m_bTrayed;
	BOOL m_bRestartApplication;

	volatile int m_iDisplayDialog;
	volatile int m_iDisplayMenu;

	CSessionNotify m_sessionNotify;

	BOOL m_bWindowsNewLine;
	BOOL m_bPasswordStars;
	BOOL m_bUserStars;
	BOOL m_bDisableUnsafe;
	BOOL m_bRememberLast;
	BOOL m_bUsePuttyForURLs;
	BOOL m_bSaveOnLATMod;
	BOOL m_bStartMinimized;
	BOOL m_bQuickFindIncBackup;
	BOOL m_bQuickFindIncExpired;
	BOOL m_bCheckForUpdate;
	BOOL m_bCheckForUpdateCfg;
	BOOL m_bFocusResAfterQuickFind;

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
	BOOL m_bMinimizeOnLock;
	BOOL m_bMinimizeToTray;
	BOOL m_bCloseMinimizes;
	BOOL m_bEntryView;
	BOOL m_bColAutoSize;
	int m_nAutoSort;
	int m_nAutoTypeMethod;
	BOOL m_bShowFullPath;
	BOOL m_bCopyURLs;
	BOOL m_bExitInsteadOfLockAT;
	CString m_strURLOverride;
	BOOL m_bAllowSaveIfModifiedOnly;
	BOOL m_bRegisterRestoreHotKey;
	BOOL m_bDropToBackOnCopy;
	BOOL m_bDeleteTANsAfterUse;

	BCMenu *m_pPwListMenu;
	BCMenu *m_pGroupListMenu;
	BCMenu *m_pEntryViewMenu;
	BCMenu *m_pPwListTrackableMenu;
	BCMenu *m_pGroupListTrackableMenu;
	BCMenu *m_pEntryViewTrackableMenu;
	BOOL m_bRestoreHotKeyRegistered;

	BCMenu m_popmenu;
	BCMenu m_menuColView;
	CToolTipCtrl m_tip;

	CFont m_fListFont;
	CSystemTrayEx m_systray;
	HACCEL m_hAccel;

	CFont m_fNotesFont;
	CString m_strNotesFontSpec;
	CString m_strNotesFontRtfSpec;
	int m_nNotesFontSize;

	CStringList m_lNotesRegex;
	CStringList m_lNotesFormat;

	std::basic_string<TCHAR> m_strInitialEnvDir;
	std::vector<std::basic_string<TCHAR> > m_vTempFiles;

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
	DWORD m_dwLastEntryIndex;

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
	LONG m_lGroupUrlStart;

	DWORD_PTR m_dwGroupsSaveFirstVisible;
	DWORD_PTR m_dwGroupsSaveSelected;

	BOOL m_bBlockPwListUpdate;
	BOOL m_bIgnoreSizeEvent;
	BOOL m_bGlobalAutoTypePending;
	BOOL m_bBlockQuickFindSelChange;

	BOOL m_bDragging;
	BOOL m_bDraggingHoriz;

	HTREEITEM m_hDraggingGroup;
	// BOOL m_bCanDragGroup;

	HCURSOR m_hArrowCursor;
	HCURSOR m_hDragLeftRight;
	HCURSOR m_hDragUpDown;

	LONG m_lSplitterPosHoriz;
	LONG m_lSplitterPosVert;

	INT m_aHeaderOrder[11];

	BOOL m_bMenuExit;

	BOOL m_bSimpleTANView;
	BOOL m_bShowTANIndices;
	BOOL m_bTANsOnly;
	DWORD m_dwPwListMode;

	CRemoteControl m_remoteControl;

	BOOL m_bAutoTypeIEFix;
	BOOL m_bAutoTypeSameKL;

	CString m_strWindowTitleSuffix;

	COLORREF m_clrIcoStoreMain;
	HICON m_hIcoStoreMain;
	COLORREF m_clrIcoStoreTrayNormal;
	HICON m_hIcoStoreTrayNormal;
	// COLORREF m_clrIcoStoreTrayLocked;
	// HICON m_hIcoStoreTrayLocked;

	static BOOL m_bUnintrusiveMiniMode;

	static DWORD m_dwCachedBackupGroupID;
	static DWORD m_dwCachedBackupSrcGroupID;

	static std::map<std_string, std_string> m_mHtmlToRtf;

private:
	void SetViewHideState(BOOL bReqVisible, BOOL bPreferTray);
	void ToggleViewHideState(BOOL bPreferTray);
	void SetTrayState(BOOL bMinimizeToTray);

	void SetToMiniModeIfEnabled(BCMenu *pMenu, BOOL bRemoveAccels,
		BOOL bProcessToolBar);
	void _MiniModeShowWindow();
	void _GetNewDbFileInUserDir(const CString& strLoadedIniLastDb,
		const CPrivateConfigEx& cConfig);

	void RegisterRestoreHotKey(BOOL bRegister);
	void DropToBackgroundIfOptionEnabled(bool bForceDrop);

	void _PostUseTANEntry(DWORD dwListIndex, DWORD dwEntryIndex);
	void _PreDatabaseWrite();

	inline void _SetAutoLockTimeout(long lSeconds);
	inline UINT64 _GetCurrentTimeUtc();

	LONG m_lNormalWndPosX;
	LONG m_lNormalWndPosY;
	LONG m_lNormalWndSizeW;
	LONG m_lNormalWndSizeH;

public:
	//{{AFX_DATA(CPwSafeDlg)
	enum { IDD = IDD_PWSAFE_DIALOG };
	CStatic	m_stcMenuLine;
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
	CComboBox m_cQuickFind;
	CString m_strQuickFind;
	CCustomListCtrlEx	m_cList;
	CAutoRichEditCtrlFx	m_reEntryView;
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
	afx_msg void OnUpdateFileExit(CCmdUI *pCmdUI);
	afx_msg void OnInfoAbout();
	afx_msg void OnSafeAddGroup();
	afx_msg void OnViewHideStars();
	afx_msg void OnPwlistAdd();
	afx_msg void OnPwlistEdit();
	afx_msg void OnPwlistDelete();
	afx_msg void OnRclickPwlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickGroupList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPwlistCopyPw();
	afx_msg void OnTimer(WPARAM nIDEvent);
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
	afx_msg void OnUpdateReCopySel(CCmdUI* pCmdUI);
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
	afx_msg void OnExtrasRepairDb();
	afx_msg void OnUpdateExtrasRepairDb(CCmdUI* pCmdUI);
	afx_msg void OnImportKeePass();
	afx_msg void OnUpdateImportKeePass(CCmdUI* pCmdUI);
	afx_msg void OnSafeExportGroupKeePass();
	afx_msg void OnUpdateSafeExportGroupKeePass(CCmdUI* pCmdUI);
	afx_msg void OnExportKeePass();
	afx_msg void OnUpdateExportKeePass(CCmdUI* pCmdUI);
	afx_msg void OnViewSimpleTANView();
	afx_msg void OnViewShowTANIndices();
	afx_msg void OnUpdateExtrasGenPw(CCmdUI *pCmdUI);
	afx_msg void OnUpdateInfoAbout(CCmdUI *pCmdUI);
	afx_msg void OnInfoChkForUpd();
	afx_msg void OnUpdateViewHide(CCmdUI *pCmdUI);
	afx_msg void OnQuickFindSelChange();
	afx_msg void OnInfoHelpSelectHelpSource();
	//}}AFX_MSG

	afx_msg void OnPluginMessage(UINT nID);
	afx_msg void OnUpdatePluginMessage(CCmdUI* pCmdUI);
	afx_msg void OnRenderFormat(UINT nFormat);
	afx_msg void OnRenderAllFormats();
	afx_msg void OnChangeCbChain(HWND hWndRemove, HWND hWndAfter);
	afx_msg void OnDrawClipboard();

	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnEndSession(BOOL bEnding);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);

	afx_msg LRESULT OnTaskbarCreated(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTaskbarButtonCreated(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnProcessMailslot(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnKeePassControlMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWTSSessionChange(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PWSAFEDLG_H__206CC2C3_063D_11D8_BF16_0050BF14F5CC__INCLUDED_)
