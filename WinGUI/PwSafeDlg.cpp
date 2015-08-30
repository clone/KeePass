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
#include <afxadv.h>
#include <winuser.h>
#include <mmsystem.h>
#include <set>

#include "PwSafe.h"
#include "PwSafeDlg.h"

#include "../KeePassLibCpp/PwManager.h"
#include "../KeePassLibCpp/DataExchange/PwImport.h"
#include "../KeePassLibCpp/Util/PwUtil.h"
#include "../KeePassLibCpp/Crypto/TestImpl.h"
#include "../KeePassLibCpp/Crypto/MemoryProtectionEx.h"
#include "../KeePassLibCpp/Crypto/KeyTransform_BCrypt.h"
#include "../KeePassLibCpp/Util/MemUtil.h"
#include "../KeePassLibCpp/Util/StrUtil.h"
#include "../KeePassLibCpp/Util/AppUtil.h"
#include "Util/WinUtil.h"
#include "Util/SendKeysEx.h"
#include "Util/FileLock.h"
#include "Util/CmdLine/CmdArgs.h"
#include "Util/CmdLine/Executable.h"
#include "Util/UpdateCheckEx.h"
#include "Util/RestartManagerEx.h"
#include "Util/KeySourcesPool.h"
#include "Util/SprEngine/SprEngine.h"
#include "../KeePassLibCpp/Util/TranslateEx.h"
#include "NewGUI/XHyperLink.h"
#include "NewGUI/XPStyleButtonST.h"
#include "NewGUI/SecureEditEx.h"
#include "NewGUI/MsgRelayWnd.h"
#include "NewGUI/TaskDialog/VistaTaskDialog.h"
#include "NewGUI/TaskbarListEx/TaskbarListEx.h"
// #include "NewGUI/VistaMenu/VistaMenu.h"
#include "NewGUI/NewDialogsEx.h"
#include "NewGUI/NewColorizerEx.h"
#include "Plugins/KpApiImpl.h"

#include "PasswordDlg.h"
#include "AddEntryDlg.h"
#include "AddGroupDlg.h"
#include "PwGeneratorExDlg.h"
#include "FindInDbDlg.h"
#include "LanguagesDlg.h"
#include "OptionsDlg.h"
#include "GetRandomDlg.h"
#include "EntryPropertiesDlg.h"
#include "TanWizardDlg.h"
#include "EntryListDlg.h"
#include "DbSettingsDlg.h"
#include "PluginsDlg.h"
#include "CheckOptionsDlg.h"
#include "ImportOptionsDlg.h"
#include "HelpSourceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _UNICODE
#pragma message("Unicode builds are not supported. It is recommended to switch to ANSI configuration.")
#endif

#define WM_MY_SYSTRAY_NOTIFY (WM_APP + 10)
#define WM_MY_UPDATECLIPBOARD (WM_APP + 11)

#ifdef _DEBUG
// #define or #undef sample group and entries
#define ___PWSAFE_SAMPLE_DATA
#endif

static TCHAR g_pNullString[4] = { 0, 0, 0, 0 };
static const BYTE g_uuidZero[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static PW_TIME g_tNeverExpire;
static const TCHAR *g_psztNeverExpires = _T("");

static int g_nIndicatorWidths[3] = { 200, 340, -1 }; // Not const (DPI scaling)

BOOL CPwSafeDlg::m_bMiniMode = FALSE;
BOOL CPwSafeDlg::m_bUnintrusiveMiniMode = FALSE;
BOOL CPwSafeDlg::m_bSecureEdits = TRUE;
BOOL CPwSafeDlg::m_bUseLocalTimeFormat = TRUE;
BOOL CPwSafeDlg::m_bDisableUnsafeAtStart = FALSE;
BOOL CPwSafeDlg::m_bSortAutoTypeSelItems = TRUE;

BOOL CPwSafeDlg::m_bForceAllowChangeMasterKey = FALSE;
BOOL CPwSafeDlg::m_bForceAllowPrinting = FALSE;
BOOL CPwSafeDlg::m_bForceAllowImport = FALSE;
BOOL CPwSafeDlg::m_bForceAllowExport = FALSE;
BOOL CPwSafeDlg::m_bDisallowPrintingPasswords = FALSE;

PW_GEN_SETTINGS_EX CPwSafeDlg::m_pgsAutoProfile;

CString CPwSafeDlg::m_strDefaultAutoTypeSequence;

DWORD CPwSafeDlg::m_dwCachedBackupGroupID = 0;
DWORD CPwSafeDlg::m_dwCachedBackupSrcGroupID = 0;

const UINT WM_REG_TASKBARCREATED = ::RegisterWindowMessage(_T("TaskbarCreated"));
const UINT WM_REG_TASKBARBUTTONCREATED = ::RegisterWindowMessage(_T("TaskbarButtonCreated"));
const UINT WM_REG_PROCESSMAILSLOT = ::RegisterWindowMessage(_T("KeePassProcessMailslot"));
const UINT WM_REG_KEEPASSCONTROL = ::RegisterWindowMessage(_T("KeePassControl"));

static volatile UINT g_uMasterKeyDlgs = 0;
static volatile HWND g_hLastMasterKeyDlg = NULL;
static std::set<std_string> g_vAutoTypeSelectionDialogs;

/////////////////////////////////////////////////////////////////////////////

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	CKCSideBannerWnd m_banner;
	HICON m_hWindowIcon;

	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CXHyperLink	m_hlDonate;
	CXHyperLink	m_hlCredits;
	CXHyperLink	m_hlLicense;
	CXHyperLink	m_hlHelp;
	CXHyperLink	m_hlHomepage;
	CXPStyleButtonST	m_btOK;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	afx_msg LRESULT OnXHyperLinkClicked(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_HLINK_DONATE, m_hlDonate);
	DDX_Control(pDX, IDC_HLINK_CREDITS, m_hlCredits);
	DDX_Control(pDX, IDC_HLINK_LICENSEFILE, m_hlLicense);
	DDX_Control(pDX, IDC_HLINK_HELPFILE, m_hlHelp);
	DDX_Control(pDX, IDC_HLINK_HOMEPAGE, m_hlHomepage);
	DDX_Control(pDX, IDOK, m_btOK);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP

	ON_REGISTERED_MESSAGE(WM_XHYPERLINK_CLICKED, OnXHyperLinkClicked)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

class CPwSafeAppRI : public CNewRandomInterface
{
public:
	BOOL GenerateRandomSequence(unsigned long uRandomSeqSize, unsigned char *pBuffer) const;
};

BOOL CPwSafeAppRI::GenerateRandomSequence(unsigned long uRandomSeqSize,
	unsigned char *pBuffer) const
{
	ASSERT(uRandomSeqSize <= 32); // Only up to 32-byte long random sequence is supported for now!
	if(uRandomSeqSize > 32) uRandomSeqSize = 32;

	CGetRandomDlg dlg;
	if(dlg.DoModal() == IDCANCEL) return FALSE;

	memcpy(pBuffer, dlg.m_pFinalRandom, uRandomSeqSize);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

CPwSafeDlg::CPwSafeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPwSafeDlg::IDD, pParent), m_instanceChecker(_T("KeePass Instance"))
	, m_strQuickFind(_T("")), m_lGroupUrlStart(0)
{
	//{{AFX_DATA_INIT(CPwSafeDlg)
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pThemeHelper = NULL;

	m_bExiting = FALSE;
	m_bIsLocking = FALSE;
	m_bTrayed = FALSE;
	m_bMinimized = FALSE;
	m_bIgnoreSizeEvent = TRUE;
	m_bCheckForInstance = FALSE;
	m_bRestoreHotKeyRegistered = FALSE;
	m_bInitialCmdLineFile = FALSE;

	m_uLockAt = UINT64_MAX;

	m_hLastSelectedGroup = NULL;
	m_dwLastNumSelectedItems = 0;
	m_dwLastFirstSelectedItem = 0;
	m_dwLastEntryIndex = DWORD_MAX;

	m_nClipboardState = 0;
	m_hwndNextViewer = NULL;
	m_bGlobalAutoTypePending = FALSE;
	m_bBlockQuickFindSelChange = FALSE;

	m_pPwListMenu = m_pGroupListMenu = m_pEntryViewMenu = NULL;
	m_pPwListTrackableMenu = m_pGroupListTrackableMenu = m_pEntryViewTrackableMenu = NULL;

	m_lNormalWndPosX = m_lNormalWndPosY = 0;
	m_lNormalWndSizeW = m_lNormalWndSizeH = -1;

	m_clrIcoStoreMain = DWORD_MAX;
	m_hIcoStoreMain = NULL;
	m_clrIcoStoreTrayNormal = DWORD_MAX;
	m_hIcoStoreTrayNormal = NULL;
	// m_clrIcoStoreTrayLocked = DWORD_MAX;
	// m_hIcoStoreTrayLocked = NULL;

	m_mgr.InitPrimaryInstance();
}

void CPwSafeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPwSafeDlg)
	DDX_Control(pDX, IDC_MENULINE, m_stcMenuLine);
	DDX_Control(pDX, IDC_GROUPLIST, m_cGroups);
	DDX_Control(pDX, IDC_TB_NEW, m_btnTbNew);
	DDX_Control(pDX, IDC_TB_LOCK, m_btnTbLock);
	DDX_Control(pDX, IDC_TB_FIND, m_btnTbFind);
	DDX_Control(pDX, IDC_TB_EDITENTRY, m_btnTbEditEntry);
	DDX_Control(pDX, IDC_TB_DELETEENTRY, m_btnTbDeleteEntry);
	DDX_Control(pDX, IDC_TB_COPYUSER, m_btnTbCopyUser);
	DDX_Control(pDX, IDC_TB_COPYPW, m_btnTbCopyPw);
	DDX_Control(pDX, IDC_TB_ADDENTRY, m_btnTbAddEntry);
	DDX_Control(pDX, IDC_TB_ABOUT, m_btnTbAbout);
	DDX_Control(pDX, IDC_TB_SAVE, m_btnTbSave);
	DDX_Control(pDX, IDC_TB_OPEN, m_btnTbOpen);
	DDX_Control(pDX, IDC_PWLIST, m_cList);
	DDX_Control(pDX, IDC_RE_ENTRYVIEW, m_reEntryView);
	DDX_Control(pDX, IDC_COMBO_QUICKFIND, m_cQuickFind);
	DDX_CBString(pDX, IDC_COMBO_QUICKFIND, m_strQuickFind);
//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPwSafeDlg, CDialog)
	//{{AFX_MSG_MAP(CPwSafeDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_MEASUREITEM()
	ON_WM_MENUCHAR()
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXIT, OnUpdateFileExit)
	ON_COMMAND(ID_INFO_ABOUT, OnInfoAbout)
	ON_COMMAND(ID_SAFE_ADDGROUP, OnSafeAddGroup)
	ON_COMMAND(ID_VIEW_HIDESTARS, OnViewHideStars)
	ON_COMMAND(ID_PWLIST_ADD, OnPwlistAdd)
	ON_COMMAND(ID_PWLIST_EDIT, OnPwlistEdit)
	ON_COMMAND(ID_PWLIST_DELETE, OnPwlistDelete)
	ON_NOTIFY(NM_RCLICK, IDC_PWLIST, OnRclickPwlist)
	ON_NOTIFY(NM_CLICK, IDC_GROUPLIST, OnClickGroupList)
	ON_COMMAND(ID_PWLIST_COPYPW, OnPwlistCopyPw)
	ON_WM_TIMER()
	ON_NOTIFY(NM_DBLCLK, IDC_PWLIST, OnDblclkPwlist)
	ON_NOTIFY(NM_RCLICK, IDC_GROUPLIST, OnRclickGroupList)
	ON_COMMAND(ID_PWLIST_COPYUSER, OnPwlistCopyUser)
	ON_COMMAND(ID_PWLIST_VISITURL, OnPwlistVisitUrl)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_COMMAND(ID_SAFE_OPTIONS, OnSafeOptions)
	ON_COMMAND(ID_SAFE_REMOVEGROUP, OnSafeRemoveGroup)
	ON_COMMAND(ID_FILE_CHANGEMASTERPW, OnFileChangeMasterPw)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEAS, OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_CHANGEMASTERPW, OnUpdateFileChangeMasterPw)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateFileClose)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_COPYPW, OnUpdatePwlistCopyPw)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_COPYUSER, OnUpdatePwlistCopyUser)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_DELETE, OnUpdatePwlistDelete)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_EDIT, OnUpdatePwlistEdit)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_VISITURL, OnUpdatePwlistVisitUrl)
	ON_UPDATE_COMMAND_UI(ID_SAFE_REMOVEGROUP, OnUpdateSafeRemoveGroup)
	ON_UPDATE_COMMAND_UI(ID_SAFE_ADDGROUP, OnUpdateSafeAddGroup)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_ADD, OnUpdatePwlistAdd)
	ON_COMMAND(ID_EXPORT_TXT, OnExportTxt)
	ON_COMMAND(ID_EXPORT_HTML, OnExportHtml)
	ON_COMMAND(ID_EXPORT_XML, OnExportXml)
	ON_COMMAND(ID_EXPORT_CSV, OnExportCsv)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_TXT, OnUpdateExportTxt)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_HTML, OnUpdateExportHtml)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_XML, OnUpdateExportXml)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_CSV, OnUpdateExportCsv)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_COMMAND(ID_EXTRAS_GENPW, OnExtrasGenPw)
	ON_COMMAND(ID_SAFE_MODIFYGROUP, OnSafeModifyGroup)
	ON_UPDATE_COMMAND_UI(ID_SAFE_MODIFYGROUP, OnUpdateSafeModifyGroup)
	ON_COMMAND(ID_PWLIST_FIND, OnPwlistFind)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_FIND, OnUpdatePwlistFind)
	ON_COMMAND(ID_PWLIST_FINDINGROUP, OnPwlistFindInGroup)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_FINDINGROUP, OnUpdatePwlistFindInGroup)
	ON_COMMAND(ID_PWLIST_DUPLICATE, OnPwlistDuplicate)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_DUPLICATE, OnUpdatePwlistDuplicate)
	ON_COMMAND(ID_INFO_HOMEPAGE, OnInfoHomepage)
	ON_COMMAND(ID_VIEW_ALWAYSONTOP, OnViewAlwaysOnTop)
	ON_COMMAND(ID_SAFE_EXPORTGROUP_HTML, OnSafeExportGroupHtml)
	ON_COMMAND(ID_SAFE_EXPORTGROUP_XML, OnSafeExportGroupXml)
	ON_COMMAND(ID_SAFE_EXPORTGROUP_CSV, OnSafeExportGroupCsv)
	ON_UPDATE_COMMAND_UI(ID_SAFE_EXPORTGROUP_HTML, OnUpdateSafeExportGroupHtml)
	ON_UPDATE_COMMAND_UI(ID_SAFE_EXPORTGROUP_XML, OnUpdateSafeExportGroupXml)
	ON_UPDATE_COMMAND_UI(ID_SAFE_EXPORTGROUP_CSV, OnUpdateSafeExportGroupCsv)
	ON_COMMAND(ID_SAFE_PRINTGROUP, OnSafePrintGroup)
	ON_UPDATE_COMMAND_UI(ID_SAFE_PRINTGROUP, OnUpdateSafePrintGroup)
	ON_COMMAND(ID_PWLIST_MOVEUP, OnPwlistMoveUp)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_MOVEUP, OnUpdatePwlistMoveUp)
	ON_COMMAND(ID_PWLIST_MOVETOP, OnPwlistMoveTop)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_MOVETOP, OnUpdatePwlistMoveTop)
	ON_COMMAND(ID_PWLIST_MOVEDOWN, OnPwlistMoveDown)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_MOVEDOWN, OnUpdatePwlistMoveDown)
	ON_COMMAND(ID_PWLIST_MOVEBOTTOM, OnPwlistMoveBottom)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_MOVEBOTTOM, OnUpdatePwlistMoveBottom)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_PWLIST, OnBeginDragPwlist)
	ON_COMMAND(ID_FILE_CHANGELANGUAGE, OnFileChangeLanguage)
	ON_COMMAND(ID_INFO_README, OnInfoReadme)
	ON_COMMAND(ID_VIEW_TITLE, OnViewTitle)
	ON_COMMAND(ID_VIEW_USERNAME, OnViewUsername)
	ON_COMMAND(ID_VIEW_URL, OnViewUrl)
	ON_COMMAND(ID_VIEW_PASSWORD, OnViewPassword)
	ON_COMMAND(ID_VIEW_NOTES, OnViewNotes)
	ON_COMMAND(ID_FILE_LOCK, OnFileLock)
	ON_UPDATE_COMMAND_UI(ID_FILE_LOCK, OnUpdateFileLock)
	ON_COMMAND(ID_GROUP_MOVETOP, OnGroupMoveTop)
	ON_UPDATE_COMMAND_UI(ID_GROUP_MOVETOP, OnUpdateGroupMoveTop)
	ON_COMMAND(ID_GROUP_MOVEBOTTOM, OnGroupMoveBottom)
	ON_UPDATE_COMMAND_UI(ID_GROUP_MOVEBOTTOM, OnUpdateGroupMoveBottom)
	ON_COMMAND(ID_GROUP_MOVEUP, OnGroupMoveUp)
	ON_UPDATE_COMMAND_UI(ID_GROUP_MOVEUP, OnUpdateGroupMoveUp)
	ON_COMMAND(ID_GROUP_MOVEDOWN, OnGroupMoveDown)
	ON_UPDATE_COMMAND_UI(ID_GROUP_MOVEDOWN, OnUpdateGroupMoveDown)
	ON_MESSAGE(WM_MY_SYSTRAY_NOTIFY, OnTrayNotification)
	ON_MESSAGE(WM_MY_UPDATECLIPBOARD, OnUpdateClipboard)
	ON_COMMAND(ID_VIEW_HIDE, OnViewHide)
	ON_COMMAND(ID_IMPORT_CSV, OnImportCsv)
	ON_UPDATE_COMMAND_UI(ID_IMPORT_CSV, OnUpdateImportCsv)
	ON_NOTIFY(NM_CLICK, IDC_PWLIST, OnClickPwlist)
	ON_COMMAND(ID_IMPORT_CWALLET, OnImportCWallet)
	ON_UPDATE_COMMAND_UI(ID_IMPORT_CWALLET, OnUpdateImportCWallet)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
	ON_COMMAND(ID_IMPORT_PWSAFE, OnImportPwSafe)
	ON_UPDATE_COMMAND_UI(ID_IMPORT_PWSAFE, OnUpdateImportPwSafe)
	ON_COMMAND(ID_VIEW_CREATION, OnViewCreation)
	ON_COMMAND(ID_VIEW_LASTMOD, OnViewLastMod)
	ON_COMMAND(ID_VIEW_LASTACCESS, OnViewLastAccess)
	ON_COMMAND(ID_VIEW_EXPIRE, OnViewExpire)
	ON_COMMAND(ID_VIEW_UUID, OnViewUuid)
	ON_BN_CLICKED(IDC_TB_OPEN, OnTbOpen)
	ON_BN_CLICKED(IDC_TB_SAVE, OnTbSave)
	ON_BN_CLICKED(IDC_TB_NEW, OnTbNew)
	ON_BN_CLICKED(IDC_TB_COPYUSER, OnTbCopyUser)
	ON_BN_CLICKED(IDC_TB_COPYPW, OnTbCopyPw)
	ON_BN_CLICKED(IDC_TB_ADDENTRY, OnTbAddEntry)
	ON_BN_CLICKED(IDC_TB_EDITENTRY, OnTbEditEntry)
	ON_BN_CLICKED(IDC_TB_DELETEENTRY, OnTbDeleteEntry)
	ON_BN_CLICKED(IDC_TB_FIND, OnTbFind)
	ON_BN_CLICKED(IDC_TB_LOCK, OnTbLock)
	ON_BN_CLICKED(IDC_TB_ABOUT, OnTbAbout)
	ON_COMMAND(ID_VIEW_SHOWTOOLBAR, OnViewShowToolBar)
	ON_COMMAND(ID_PWLIST_MASSMODIFY, OnPwlistMassModify)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_MASSMODIFY, OnUpdatePwlistMassModify)
	ON_NOTIFY(LVN_KEYDOWN, IDC_PWLIST, OnKeyDownPwlist)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_VIEW_ENTRYVIEW, OnViewEntryView)
	ON_COMMAND(ID_RE_COPYSEL, OnReCopySel)
	ON_UPDATE_COMMAND_UI(ID_RE_COPYSEL, OnUpdateReCopySel)
	ON_COMMAND(ID_RE_COPYALL, OnReCopyAll)
	ON_COMMAND(ID_RE_SELECTALL, OnReSelectAll)
	ON_COMMAND(ID_EXTRAS_TANWIZARD, OnExtrasTanWizard)
	ON_UPDATE_COMMAND_UI(ID_EXTRAS_TANWIZARD, OnUpdateExtrasTanWizard)
	ON_COMMAND(ID_FILE_PRINTPREVIEW, OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINTPREVIEW, OnUpdateFilePrintPreview)
	ON_COMMAND(ID_INFO_TRANSLATION, OnInfoTranslation)
	ON_COMMAND(ID_SAFE_ADDSUBGROUP, OnSafeAddSubgroup)
	ON_UPDATE_COMMAND_UI(ID_SAFE_ADDSUBGROUP, OnUpdateSafeAddSubgroup)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_GROUPLIST, OnBeginDragGrouplist)
	ON_WM_CANCELMODE()
	ON_COMMAND(ID_GROUP_SORT, OnGroupSort)
	ON_UPDATE_COMMAND_UI(ID_GROUP_SORT, OnUpdateGroupSort)
	ON_COMMAND(ID_PWLIST_SORT_TITLE, OnPwlistSortTitle)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_SORT_TITLE, OnUpdatePwlistSortTitle)
	ON_COMMAND(ID_PWLIST_SORT_USER, OnPwlistSortUser)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_SORT_USER, OnUpdatePwlistSortUser)
	ON_COMMAND(ID_PWLIST_SORT_URL, OnPwlistSortUrl)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_SORT_URL, OnUpdatePwlistSortUrl)
	ON_COMMAND(ID_PWLIST_SORT_PASSWORD, OnPwlistSortPassword)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_SORT_PASSWORD, OnUpdatePwlistSortPassword)
	ON_COMMAND(ID_PWLIST_SORT_NOTES, OnPwlistSortNotes)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_SORT_NOTES, OnUpdatePwlistSortNotes)
	ON_COMMAND(ID_PWLIST_SORT_CREATION, OnPwlistSortCreation)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_SORT_CREATION, OnUpdatePwlistSortCreation)
	ON_COMMAND(ID_PWLIST_SORT_LASTMODIFY, OnPwlistSortLastmodify)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_SORT_LASTMODIFY, OnUpdatePwlistSortLastmodify)
	ON_COMMAND(ID_PWLIST_SORT_LASTACCESS, OnPwlistSortLastaccess)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_SORT_LASTACCESS, OnUpdatePwlistSortLastaccess)
	ON_COMMAND(ID_PWLIST_SORT_EXPIRE, OnPwlistSortExpire)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_SORT_EXPIRE, OnUpdatePwlistSortExpire)
	ON_COMMAND(ID_GROUP_MOVELEFT, OnGroupMoveLeft)
	ON_UPDATE_COMMAND_UI(ID_GROUP_MOVELEFT, OnUpdateGroupMoveLeft)
	ON_COMMAND(ID_GROUP_MOVERIGHT, OnGroupMoveRight)
	ON_UPDATE_COMMAND_UI(ID_GROUP_MOVERIGHT, OnUpdateGroupMoveRight)
	ON_COMMAND(ID_VIEW_HIDEUSERS, OnViewHideUsers)
	ON_COMMAND(ID_VIEW_ATTACH, OnViewAttach)
	ON_COMMAND(ID_PWLIST_SAVEATTACH, OnPwlistSaveAttach)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_SAVEATTACH, OnUpdatePwlistSaveAttach)
	ON_COMMAND(ID_FILE_SHOWDBINFO, OnFileShowDbInfo)
	ON_UPDATE_COMMAND_UI(ID_FILE_SHOWDBINFO, OnUpdateFileShowDbInfo)
	ON_COMMAND(ID_EXTRAS_SHOWEXPIRED, OnExtrasShowExpired)
	ON_UPDATE_COMMAND_UI(ID_EXTRAS_SHOWEXPIRED, OnUpdateExtrasShowExpired)
	ON_COMMAND(ID_IMPORT_PVAULT, OnImportPvault)
	ON_UPDATE_COMMAND_UI(ID_IMPORT_PVAULT, OnUpdateImportPvault)
	ON_COMMAND(ID_SAFE_EXPORTGROUP_TXT, OnSafeExportGroupTxt)
	ON_UPDATE_COMMAND_UI(ID_SAFE_EXPORTGROUP_TXT, OnUpdateSafeExportGroupTxt)
	ON_COMMAND(ID_PWLIST_SELECTALL, OnPwlistSelectAll)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_SELECTALL, OnUpdatePwlistSelectAll)
	ON_COMMAND(ID_VIEW_AUTOSORT_CREATION, OnViewAutosortCreation)
	ON_COMMAND(ID_VIEW_AUTOSORT_EXPIRE, OnViewAutosortExpire)
	ON_COMMAND(ID_VIEW_AUTOSORT_LASTACCESS, OnViewAutosortLastaccess)
	ON_COMMAND(ID_VIEW_AUTOSORT_LASTMODIFY, OnViewAutosortLastmodify)
	ON_COMMAND(ID_VIEW_AUTOSORT_NOSORT, OnViewAutosortNosort)
	ON_COMMAND(ID_VIEW_AUTOSORT_NOTES, OnViewAutosortNotes)
	ON_COMMAND(ID_VIEW_AUTOSORT_PASSWORD, OnViewAutosortPassword)
	ON_COMMAND(ID_VIEW_AUTOSORT_TITLE, OnViewAutosortTitle)
	ON_COMMAND(ID_VIEW_AUTOSORT_URL, OnViewAutosortUrl)
	ON_COMMAND(ID_VIEW_AUTOSORT_USER, OnViewAutosortUser)
	ON_UPDATE_COMMAND_UI(ID_SAFE_OPTIONS, OnUpdateSafeOptions)
	ON_COMMAND(ID_PWLIST_AUTOTYPE, OnPwlistAutoType)
	ON_UPDATE_COMMAND_UI(ID_PWLIST_AUTOTYPE, OnUpdatePwlistAutoType)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_PWLIST, OnColumnClickPwlist)
	ON_COMMAND(ID_EXTRAS_PLUGINMGR, OnExtrasPluginMgr)
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	ON_COMMAND(ID_IMPORT_GETMORE, OnImportGetMore)
	ON_COMMAND(ID_INFO_DONATE, OnInfoDonate)
	ON_COMMAND(ID_EXTRAS_REPAIRDB, OnExtrasRepairDb)
	ON_UPDATE_COMMAND_UI(ID_EXTRAS_REPAIRDB, OnUpdateExtrasRepairDb)
	ON_COMMAND(ID_IMPORT_KEEPASS, OnImportKeePass)
	ON_UPDATE_COMMAND_UI(ID_IMPORT_KEEPASS, OnUpdateImportKeePass)
	ON_COMMAND(ID_SAFE_EXPORTGROUP_KEEPASS, OnSafeExportGroupKeePass)
	ON_UPDATE_COMMAND_UI(ID_SAFE_EXPORTGROUP_KEEPASS, OnUpdateSafeExportGroupKeePass)
	ON_COMMAND(ID_EXPORT_KEEPASS, OnExportKeePass)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_KEEPASS, OnUpdateExportKeePass)
	ON_COMMAND(ID_VIEW_SIMPLETANVIEW, OnViewSimpleTANView)
	ON_COMMAND(ID_VIEW_SHOWTANINDICES, OnViewShowTANIndices)
	ON_UPDATE_COMMAND_UI(ID_EXTRAS_GENPW, OnUpdateExtrasGenPw)
	ON_UPDATE_COMMAND_UI(ID_INFO_ABOUT, OnUpdateInfoAbout)
	ON_COMMAND(ID_INFO_CHKFORUPD, OnInfoChkForUpd)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HIDE, OnUpdateViewHide)
	ON_CBN_SELCHANGE(IDC_COMBO_QUICKFIND, &CPwSafeDlg::OnQuickFindSelChange)
	ON_COMMAND(ID_INFO_HELP_SELECTHELPSOURCE, &CPwSafeDlg::OnInfoHelpSelectHelpSource)
	//}}AFX_MSG_MAP

	ON_COMMAND_RANGE(WM_PLUGINS_FIRST, WM_PLUGINS_LAST, OnPluginMessage)
	ON_UPDATE_COMMAND_UI_RANGE(WM_PLUGINS_FIRST, WM_PLUGINS_LAST, OnUpdatePluginMessage)
	ON_WM_RENDERFORMAT()
	ON_WM_RENDERALLFORMATS()
	ON_WM_CHANGECBCHAIN()
	ON_WM_DRAWCLIPBOARD()

	ON_WM_QUERYENDSESSION()
	ON_WM_ENDSESSION()
	ON_WM_COPYDATA()

	ON_MESSAGE(WM_WTSSESSION_CHANGE, OnWTSSessionChange)

	ON_REGISTERED_MESSAGE(WM_REG_TASKBARCREATED, OnTaskbarCreated)
	ON_REGISTERED_MESSAGE(WM_REG_TASKBARBUTTONCREATED, OnTaskbarButtonCreated)
	ON_REGISTERED_MESSAGE(WM_REG_PROCESSMAILSLOT, OnProcessMailslot)
	ON_REGISTERED_MESSAGE(WM_REG_KEEPASSCONTROL, OnKeePassControlMessage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	NewGUI_XPButton(m_btOK, IDB_OK, IDB_OK);

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);

	m_banner.SetTitle(PWM_PRODUCT_NAME);

	CString str = TRL("Version");
	str += _T(" ");
	str += PWM_VERSION_STR;
	if(CPwSafeDlg::m_bMiniMode == TRUE)
	{
		str += _T(" (");
		str += TRL("in \"Minimal functionality\" mode");
		str += _T(")");
	}
	m_banner.SetCaption(str);

	m_hWindowIcon = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(m_hWindowIcon, TRUE);
	SetIcon(m_hWindowIcon, FALSE);

	// NewGUI_MakeHyperLink(&m_hlHomepage);
	// m_hlHomepage.SetURL(CString(PWM_HOMEPAGE));
	str = TRL("KeePass Website");
	m_hlHomepage.SetWindowText(str);
	NewGUI_MakeHyperLink(&m_hlHomepage);
	m_hlHomepage.EnableTooltip(FALSE);
	m_hlHomepage.SetNotifyParent(TRUE);
	m_hlHomepage.EnableURL(FALSE);

	str = TRL("Open &Help File"); RemoveAcceleratorTip(&str);
	m_hlHelp.SetWindowText(str);
	NewGUI_MakeHyperLink(&m_hlHelp);
	m_hlHelp.EnableTooltip(FALSE);
	m_hlHelp.SetNotifyParent(TRUE);
	m_hlHelp.EnableURL(FALSE);

	str = TRL("Open &License File"); RemoveAcceleratorTip(&str);
	m_hlLicense.SetWindowText(str);
	NewGUI_MakeHyperLink(&m_hlLicense);
	m_hlLicense.EnableTooltip(FALSE);
	m_hlLicense.SetNotifyParent(TRUE);
	m_hlLicense.EnableURL(FALSE);

	str = TRL("Acknowledgements");
	m_hlCredits.SetWindowText(str);
	NewGUI_MakeHyperLink(&m_hlCredits);
	m_hlCredits.EnableTooltip(FALSE);
	m_hlCredits.SetNotifyParent(TRUE);
	m_hlCredits.EnableURL(FALSE);

	str = TRL("&Donate..."); RemoveAcceleratorTip(&str); str.Remove(_T('.'));
	m_hlDonate.SetWindowText(str);
	NewGUI_MakeHyperLink(&m_hlDonate);
	m_hlDonate.EnableTooltip(FALSE);
	m_hlDonate.SetNotifyParent(TRUE);
	m_hlDonate.EnableURL(FALSE);

	UpdateData(FALSE);

	return TRUE;
}

LRESULT CAboutDlg::OnXHyperLinkClicked(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	if(wParam == IDC_HLINK_HOMEPAGE)
		ShellExecute(NULL, NULL, PWM_HOMEPAGE, NULL, NULL, SW_SHOW);
	else if(wParam == IDC_HLINK_HELPFILE)
		WU_OpenAppHelp(NULL, m_hWnd);
	else if(wParam == IDC_HLINK_LICENSEFILE)
		_OpenLocalFile(PWM_LICENSE_FILE, OLF_OPEN);
	else if(wParam == IDC_HLINK_CREDITS)
		WU_OpenAppHelp(PWM_HELP_CREDITS, m_hWnd);
	else if(wParam == IDC_HLINK_DONATE)
		ShellExecute(NULL, NULL, PWM_URL_DONATE, NULL, NULL, SW_SHOW);
	else return 0;

	OnOK();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CPwSafeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	srand((unsigned int)time(NULL));
	KPMI_SetMainDialog(this);

	if(m_bCheckForInstance == TRUE) m_instanceChecker.TrackFirstInstanceRunning();

	m_bWindowsNewLine = TRUE;
	m_bFileOpen = FALSE;
	m_bFileReadOnly = FALSE;
	m_bLocked = FALSE;
	m_bModified = FALSE;
	m_dwOldListParameters = 0;
	m_bWasMaximized = FALSE;
	m_bCachedToolBarUpdate = FALSE;
	m_bCachedPwlistUpdate = FALSE;
	m_bDragging = FALSE;
	m_bDisplayDialog = FALSE;
	m_hDraggingGroup = NULL;
	// m_bDraggingEntry = FALSE;
	m_bMenuExit = FALSE;
	m_bBlockPwListUpdate = FALSE;
	m_bRestartApplication = FALSE;
	m_mgr.GetNeverExpireTime(&g_tNeverExpire);
	m_nFileRelockCountdown = -1;
	m_bTANsOnly = FALSE;
	m_dwPwListMode = LVS_REPORT;

	m_bHashValid = FALSE;
	ZeroMemory(m_aHashOfFile, 32);

	m_cList.m_pParentI = this;
	m_cList.m_pbShowColumns = &m_bShowColumn[0];
	m_cGroups.m_pParentI = this;

	m_hArrowCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	m_hDragLeftRight = AfxGetApp()->LoadStandardCursor(IDC_SIZEWE);
	m_hDragUpDown = AfxGetApp()->LoadStandardCursor(IDC_SIZENS);

	m_menu.LoadMenu(IDR_MAINMENU); // Load the main menu

	// Setup menu style
	m_menu.SetMenuDrawMode(BCMENU_DRAWMODE_XP); // <<<!=>>> BCMENU_DRAWMODE_ORIGINAL
	m_menu.SetSelectDisableMode(FALSE);
	m_menu.SetXPBitmap3D(TRUE);
	m_menu.SetBitmapBackground(RGB(255,0,255));
	m_menu.SetIconSize(16, 16);

	// Make up the main menu, insert the group list and password list menus to the edit menu

	m_popmenu.LoadMenu(IDR_GROUPLIST_MENU);
	BCMenu *pSrc = NewGUI_GetBCMenu(m_popmenu.GetSubMenu(0));
	ASSERT(pSrc != NULL); if(pSrc == NULL) pSrc = &m_popmenu;

	BCMenu *pDest = NewGUI_GetBCMenu(m_menu.GetSubMenu(1));
	ASSERT(pDest != NULL); if(pDest == NULL) pDest = &m_menu;

	UINT i, uState, uID, uLastID = static_cast<UINT>(-1);
	CString str;

	// pDest->AppendMenu(MF_SEPARATOR);
	for(i = 0; i < pSrc->GetMenuItemCount(); ++i)
	{
		uID = pSrc->GetMenuItemID(i);
		uState = pSrc->GetMenuState(i, MF_BYPOSITION);
		pSrc->GetMenuText(i, str, MF_BYPOSITION);
		if(str == _T("&Rearrange")) continue;
		if(str == _T("&Export Group To")) continue;
		if(str == _T("Find in t&his Group...")) continue;
		// if((uID == ID_GROUP_MOVETOP) || (uID == ID_GROUP_MOVEBOTTOM)) continue;
		// if((uID == ID_GROUP_MOVEUP) || (uID == ID_GROUP_MOVEDOWN)) continue;
		if(str == _T("Pr&int Group...")) continue;

		if(uLastID != uID) pDest->AppendMenu(uState, uID, const_cast<LPTSTR>((LPCTSTR)str));
		uLastID = uID;
	}
	m_popmenu.DestroyMenu();

	// pDest->GetMenuText(0, str, MF_BYPOSITION);
	// if(str.GetLength() == 0) pDest->DeleteMenu(0, MF_BYPOSITION);
	if(pDest->GetMenuItemID(0) == 0) pDest->DeleteMenu(0, MF_BYPOSITION);

	m_popmenu.LoadMenu(IDR_PWLIST_MENU);
	pDest = NewGUI_GetBCMenu(m_menu.GetSubMenu(1));
	ASSERT(pDest != NULL); if(pDest == NULL) pDest = &m_menu;
	pSrc = NewGUI_GetBCMenu(m_popmenu.GetSubMenu(0));
	ASSERT(pSrc != NULL); if(pSrc == NULL) pSrc = &m_popmenu;

	// pDest->AppendMenu(MF_SEPARATOR);
	for(i = 0; i < pSrc->GetMenuItemCount() - 2; ++i)
	{
		uID = pSrc->GetMenuItemID(i);
		uState = pSrc->GetMenuState(i, MF_BYPOSITION);
		pSrc->GetMenuText(i, str, MF_BYPOSITION);
		if(str == _T("&Rearrange")) continue;
		// if((uID == ID_PWLIST_MOVETOP) || (uID == ID_PWLIST_MOVEBOTTOM)) continue;
		// if((uID == ID_PWLIST_MOVEUP) || (uID == ID_PWLIST_MOVEDOWN)) continue;

		if(uLastID != uID) pDest->AppendMenu(uState, uID, const_cast<LPTSTR>((LPCTSTR)str));
		uLastID = uID;
	}
	m_popmenu.DestroyMenu();

	CPrivateConfigEx cConfig(FALSE);
	TCHAR szTemp[SI_REGSIZE];

	// Load the translation file and cache strings
	CPwSafeApp::LoadTranslationEx(&cConfig);
	g_psztNeverExpires = TRL("Never expires");

	m_bMiniMode = cConfig.GetBool(PWMKEY_MINIMODE, FALSE);
	m_bUnintrusiveMiniMode = cConfig.GetBool(PWMKEY_UNINTRUSIVEMINIMODE, FALSE);

	m_bForceAllowChangeMasterKey = cConfig.GetEnforcedBool(PWMKEY_FORCEALLOWCHANGEMKEY, FALSE, TRUE);
	m_bForceAllowPrinting = cConfig.GetEnforcedBool(PWMKEY_FORCEALLOWPRINTING, FALSE, TRUE);
	m_bForceAllowImport = cConfig.GetEnforcedBool(PWMKEY_FORCEALLOWIMPORT, FALSE, TRUE);
	m_bForceAllowExport = cConfig.GetEnforcedBool(PWMKEY_FORCEALLOWEXPORT, FALSE, TRUE);

	m_bDisallowPrintingPasswords = cConfig.GetBool(PWMKEY_DISALLOWPRINTINGPWS, FALSE);

	cConfig.Get(PWMKEY_DEFAULTATSEQ, szTemp);
	if(szTemp[0] != 0) m_strDefaultAutoTypeSequence = szTemp;
	else m_strDefaultAutoTypeSequence = _T("{USERNAME}{TAB}{PASSWORD}{ENTER}");

	/* cConfig.Get(PWMKEY_PWGEN_OPTIONS, szTemp);
	if(szTemp[0] != 0)
	{
		CString strOptions = szTemp;

		szTemp[0] = 0; szTemp[1] = 0;
		cConfig.Get(PWMKEY_PWGEN_CHARS, szTemp);
		CString strCharSet = szTemp;

		szTemp[0] = 0; szTemp[1] = 0;
		cConfig.Get(PWMKEY_PWGEN_NUMCHARS, szTemp);
		if(szTemp[0] != 0)
			CPwGeneratorDlg::SetOptions(strOptions, strCharSet, (UINT)(_ttoi(szTemp)));
	}
	else CPwGeneratorDlg::SetOptions(CString(_T("11100000001")), CString(_T("")), 16); */

	m_strInitialEnvDir = WU_GetCurrentDirectory();

	cConfig.Get(PWMKEY_LASTDIR, szTemp);
	if(szTemp[0] != 0) WU_SetCurrentDirectory(szTemp);

	CKeySourcesPool::SetEnabled(cConfig.GetBool(PWMKEY_REMEMBERKEYSOURCES, TRUE));
	CKeySourcesPool::Load(&cConfig);

	cConfig.Get(PWMKEY_CLIPBOARDMETHOD, szTemp);
	if(szTemp[0] != 0) m_nClipboardMethod = _ttoi(szTemp);
	else m_nClipboardMethod = 0;

	cConfig.Get(PWMKEY_CLIPSECS, szTemp);
	if(szTemp[0] != 0)
	{
		m_dwClipboardSecs = static_cast<DWORD>(_ttol(szTemp));
		if((m_dwClipboardSecs == 0) || (m_dwClipboardSecs == DWORD_MAX))
			m_dwClipboardSecs = 10 + 1;
	}
	else m_dwClipboardSecs = 10 + 1;

	WORD wATHotKeyVK = static_cast<WORD>(_T('A'));
	WORD wATHotKeyMod = (HOTKEYF_CONTROL | HOTKEYF_ALT);
	DWORD dwDefaultATHotKey = ((DWORD)wATHotKeyMod << 16) | (DWORD)wATHotKeyVK;

	cConfig.Get(PWMKEY_AUTOTYPEHOTKEY, szTemp);
	if(szTemp[0] != 0) m_dwATHotKey = static_cast<DWORD>(_ttol(szTemp));
	else m_dwATHotKey = dwDefaultATHotKey;

	cConfig.Get(PWMKEY_DEFAULTEXPIRE, szTemp);
	if(szTemp[0] != 0) m_dwDefaultExpire = static_cast<DWORD>(_ttol(szTemp));
	else m_dwDefaultExpire = 0;

	m_bWindowsNewLine = TRUE; // Assume Windows
	cConfig.Get(PWMKEY_NEWLINE, szTemp);
	if(_tcsicmp(szTemp, _T("Unix")) == 0) m_bWindowsNewLine = FALSE;

	m_bImgButtons = cConfig.GetBool(PWMKEY_IMGBTNS, TRUE);
	NewGUI_SetImgButtons(m_bImgButtons);

	m_bDisableUnsafe = cConfig.GetBool(PWMKEY_DISABLEUNSAFE, FALSE);
	m_bDisableUnsafeAtStart = m_bDisableUnsafe;

	cConfig.Get(PWMKEY_AUTOSORT, szTemp);
	if(szTemp[0] != 0) m_nAutoSort = _ttoi(szTemp);
	else m_nAutoSort = 0;

	m_bUsePuttyForURLs = cConfig.GetBool(PWMKEY_USEPUTTYFORURLS, FALSE);
	m_bSaveOnLATMod = cConfig.GetBool(PWMKEY_SAVEONLATMOD, FALSE);
	m_bEntryGrid = cConfig.GetBool(PWMKEY_ENTRYGRID, FALSE);
	m_bAlwaysOnTop = cConfig.GetBool(PWMKEY_ALWAYSTOP, FALSE);

	m_bShowTitle = cConfig.GetBool(PWMKEY_SHOWTITLE, TRUE);
	m_bShowUserName = cConfig.GetBool(PWMKEY_SHOWUSER, TRUE);
	m_bShowURL = cConfig.GetBool(PWMKEY_SHOWURL, TRUE);
	m_bShowPassword = cConfig.GetBool(PWMKEY_SHOWPASS, TRUE);
	m_bShowNotes = cConfig.GetBool(PWMKEY_SHOWNOTES, TRUE);
	m_bShowCreation = cConfig.GetBool(PWMKEY_SHOWCREATION, FALSE);
	m_bShowLastMod = cConfig.GetBool(PWMKEY_SHOWLASTMOD, FALSE);
	m_bShowLastAccess = cConfig.GetBool(PWMKEY_SHOWLASTACCESS, FALSE);
	m_bShowExpire = cConfig.GetBool(PWMKEY_SHOWEXPIRE, FALSE);
	m_bShowUUID = cConfig.GetBool(PWMKEY_SHOWUUID, FALSE);
	m_bShowAttach = cConfig.GetBool(PWMKEY_SHOWATTACH, FALSE);

	CPwSafeDlg::m_bUseLocalTimeFormat = cConfig.GetBool(PWMKEY_USELOCALTIMEFMT, TRUE);

	m_bPasswordStars = cConfig.GetBool(PWMKEY_HIDESTARS, TRUE);
	m_bUserStars = cConfig.GetBool(PWMKEY_HIDEUSERS, FALSE);

	m_bLockOnMinimize = cConfig.GetBool(PWMKEY_LOCKMIN, FALSE);
	m_bMinimizeOnLock = cConfig.GetBool(PWMKEY_MINLOCK, TRUE);
	m_bMinimizeToTray = cConfig.GetBool(PWMKEY_MINTRAY, FALSE);
	m_bCloseMinimizes = cConfig.GetBool(PWMKEY_CLOSEMIN, FALSE);

	m_bStartMinimized = cConfig.GetBool(PWMKEY_STARTMINIMIZED, FALSE);

	m_bBackupEntries = cConfig.GetBool(PWMKEY_BACKUPENTRIES, TRUE);
	m_bSecureEdits = cConfig.GetBool(PWMKEY_SECUREEDITS, TRUE);
	m_bSingleClickTrayIcon = cConfig.GetBool(PWMKEY_SINGLECLICKTRAY, FALSE);
	m_bQuickFindIncBackup = cConfig.GetBool(PWMKEY_QUICKFINDINCBK, TRUE);
	m_bQuickFindIncExpired = cConfig.GetBool(PWMKEY_QUICKFINDINCEXP, TRUE);
	m_bDeleteBackupsOnSave = cConfig.GetBool(PWMKEY_DELETEBKONSAVE, FALSE);
	m_bDisableAutoType = cConfig.GetBool(PWMKEY_DISABLEAUTOTYPE, FALSE);
	m_bCopyURLs = cConfig.GetBool(PWMKEY_COPYURLS, FALSE);
	m_bExitInsteadOfLockAT = cConfig.GetBool(PWMKEY_EXITINSTEADLOCK, FALSE);
	m_bAllowSaveIfModifiedOnly = cConfig.GetBool(PWMKEY_ALLOWSAVEIFMODIFIEDONLY, FALSE);
	m_bFocusResAfterQuickFind = cConfig.GetBool(PWMKEY_FOCUSRESAFTERQUICKFIND, FALSE);
	m_bAutoTypeIEFix = cConfig.GetBool(PWMKEY_AUTOTYPEIEFIX, TRUE);
	m_bDropToBackOnCopy = cConfig.GetBool(PWMKEY_DROPTOBACKONCOPY, FALSE);
	m_bSortAutoTypeSelItems = cConfig.GetBool(PWMKEY_SORTAUTOTYPESELITEMS, TRUE);
	m_bUseTransactedFileWrites = cConfig.GetBool(PWMKEY_TRANSACTEDFILEWRITE, TRUE);

	m_bShowToolBar = cConfig.GetBool(PWMKEY_SHOWTOOLBAR, TRUE);
	m_bShowFullPath = cConfig.GetBool(PWMKEY_SHOWFULLPATH, FALSE);

	m_bSimpleTANView = cConfig.GetBool(PWMKEY_SIMPLETANVIEW, TRUE);
	m_bShowTANIndices = cConfig.GetBool(PWMKEY_SHOWTANINDICES, TRUE);
	m_bDeleteTANsAfterUse = cConfig.GetBool(PWMKEY_DELETETANSAFTERUSE, FALSE);

	m_bCheckForUpdate = cConfig.GetBool(PWMKEY_CHECKFORUPDATE, FALSE);
	m_bLockOnWinLock = cConfig.GetBool(PWMKEY_LOCKONWINLOCK, FALSE);
	m_bClearClipOnDbClose = cConfig.GetBool(PWMKEY_CLEARCLIPONDBCLOSE, TRUE);

	m_remoteControl.InitStatic(&m_mgr, this->m_hWnd);
	m_remoteControl.SetAlwaysAllowFullAccess(cConfig.GetBool(PWMKEY_ALWAYSALLOWIPC, FALSE));
	m_remoteControl.EnableRemoteControl(cConfig.GetBool(PWMKEY_ENABLEREMOTECTRL, FALSE));

	WU_SetAppHelpSource((cConfig.GetBool(PWMKEY_USEHELPCENTER, TRUE) == TRUE) ?
		APPHS_ONLINE : APPHS_LOCAL);

	cConfig.Get(PWMKEY_ROWCOLOR, szTemp);
	if(szTemp[0] != 0)
	{
		COLORREF cref = (COLORREF)_ttol(szTemp);
		m_cList.SetRowColorEx(cref);
	}

	cConfig.Get(PWMKEY_AUTOTYPEMETHOD, szTemp);
	if(szTemp[0] != 0) m_nAutoTypeMethod = _ttoi(szTemp);
	else m_nAutoTypeMethod = ATM_DROPBACK;

	cConfig.Get(PWMKEY_URLOVERRIDE, szTemp);
	if(szTemp[0] != 0) m_strURLOverride = szTemp;
	else m_strURLOverride = _T("");

	cConfig.Get(PWMKEY_WNDTITLESUFFIX, szTemp);
	if(szTemp[0] != 0) m_strWindowTitleSuffix = szTemp;
	else m_strWindowTitleSuffix = _T("");

	m_nLockTimeDef = -1;
	cConfig.Get(PWMKEY_LOCKTIMER, szTemp);
	if(szTemp[0] != 0) m_nLockTimeDef = _ttol(szTemp);
	// m_nLockCountdown = m_nLockTimeDef; // NotifyUserActivity at the end of OnInitDialog

	m_bAutoShowExpired = cConfig.GetBool(PWMKEY_AUTOSHOWEXPIRED, FALSE);
	m_bAutoShowExpiredSoon = cConfig.GetBool(PWMKEY_AUTOSHOWEXPIREDS, FALSE);

	m_bEntryView = cConfig.GetBool(PWMKEY_ENTRYVIEW, TRUE);
	m_bColAutoSize = cConfig.GetBool(PWMKEY_COLAUTOSIZE, TRUE);

	cConfig.Get(PWMKEY_BANNERCOLORSTART, szTemp);
	if(szTemp[0] != 0)
		NewGUI_SetBannerColors(static_cast<DWORD>(_ttol(szTemp)), DWORD_MAX, DWORD_MAX);
	cConfig.Get(PWMKEY_BANNERCOLOREND, szTemp);
	if(szTemp[0] != 0)
		NewGUI_SetBannerColors(DWORD_MAX, static_cast<DWORD>(_ttol(szTemp)), DWORD_MAX);
	cConfig.Get(PWMKEY_BANNERCOLORTEXT, szTemp);
	if(szTemp[0] != 0)
		NewGUI_SetBannerColors(DWORD_MAX, DWORD_MAX, static_cast<DWORD>(_ttol(szTemp)));
	cConfig.Get(PWMKEY_BANNERFLIP, szTemp);
	if(szTemp[0] != 0)
		NewGUI_SetBannerParams(cConfig.GetBool(PWMKEY_BANNERFLIP, TRUE) != FALSE);

	// Translate the menu
	BCMenu *pSubMenu = &m_menu;
	const TCHAR *pSuffix = _T("");
	CString strItem;
	int nItem = 0, nSub = 0;
	UINT nID;
	while(true)
	{
		if(pSubMenu->GetMenuString((UINT)nItem, strItem, MF_BYPOSITION) == FALSE) break;
		pSuffix = _GetCmdAccelExt(strItem);
		CString strNew = TRL_VAR(strItem);
		if(_tcslen(pSuffix) != 0) { strNew += _T("\t"); strNew += pSuffix; }
		nID = pSubMenu->GetMenuItemID(nItem);
		if(pSubMenu->ModifyMenu(nItem, MF_BYPOSITION | MF_STRING, nID, strNew) == FALSE) { ASSERT(FALSE); }
		++nItem;
	}
	pSubMenu = NewGUI_GetBCMenu(m_menu.GetSubMenu(nSub));
	while(true)
	{
		_TranslateMenu(pSubMenu, TRUE, &m_bCopyURLs);

		pSubMenu = NewGUI_GetBCMenu(m_menu.GetSubMenu(nSub));
		++nSub;
		if(pSubMenu == NULL) break;
	}

	pSubMenu = NewGUI_GetBCMenu(m_menu.GetSubMenu(TRL("&Tools")));
	ASSERT(pSubMenu != NULL);
	if(pSubMenu != NULL) m_uOriginalExtrasMenuItemCount = pSubMenu->GetMenuItemCount();
	else m_uOriginalExtrasMenuItemCount = MENU_EXTRAS_ITEMCOUNT;

	m_menu.LoadToolbar(IDR_INFOICONS, IDB_INFOICONS_EX);

	m_bMenu = SetMenu(&m_menu); ASSERT(m_bMenu == TRUE);

	RebuildContextMenus();

	g_nIndicatorWidths[0] = NewGUI_Scale(g_nIndicatorWidths[0], this);
	g_nIndicatorWidths[1] = NewGUI_Scale(g_nIndicatorWidths[1], this);

	RECT rectClient;
	RECT rectSB;
	GetClientRect(&rectClient);
	rectSB.top = rectClient.bottom - rectClient.top - 8;
	rectSB.bottom = rectClient.bottom - rectClient.top;
	rectSB.left = 0;
	rectSB.right = rectClient.right - rectClient.left;
	m_sbStatus.Create(WS_CHILD | WS_VISIBLE | CCS_BOTTOM | SBARS_SIZEGRIP | SBT_TOOLTIPS,
		rectSB, this, AFX_IDW_STATUS_BAR);
	// m_sbStatus.SetSimple(TRUE);
	m_sbStatus.SetParts(3, g_nIndicatorWidths);
	m_sbStatus.SetSimple(FALSE);
	::SendMessage(m_sbStatus.m_hWnd, SB_SETTIPTEXT, 0, (LPARAM)TRL("Total number of groups / entries"));
	::SendMessage(m_sbStatus.m_hWnd, SB_SETTIPTEXT, 1, (LPARAM)TRL("Number of selected entries (number of entries in the list)"));
	::SendMessage(m_sbStatus.m_hWnd, SB_SETTIPTEXT, 2, (LPARAM)TRL("Status"));
	m_sbStatus.SetText(_T(""), 0, 0); m_sbStatus.SetText(_T(""), 1, 0); m_sbStatus.SetText(_T(""), 2, 0);
	SetStatusTextEx(TRL("Ready."));

	// m_ilIcons.Create(CPwSafeApp::GetClientIconsResourceID(), 16, 1, RGB(255,0,255)); // purple is transparent
	CPwSafeApp::CreateHiColorImageList(&m_ilIcons, IDB_CLIENTICONS_EX, 16);
	m_cList.SetImageList(&m_ilIcons, LVSIL_SMALL);
	m_cGroups.SetImageList(&m_ilIcons, TVSIL_NORMAL);

	m_bShowColumn[0] = m_bShowTitle; m_bShowColumn[1] = m_bShowUserName;
	m_bShowColumn[2] = m_bShowURL; m_bShowColumn[3] = m_bShowPassword;
	m_bShowColumn[4] = m_bShowNotes; m_bShowColumn[5] = m_bShowCreation;
	m_bShowColumn[6] = m_bShowLastMod; m_bShowColumn[7] = m_bShowLastAccess;
	m_bShowColumn[8] = m_bShowExpire; m_bShowColumn[9] = m_bShowUUID;
	m_bShowColumn[10] = m_bShowAttach;

	if(m_bShowTitle == TRUE) uState = MF_CHECKED; else uState = MF_UNCHECKED;
	m_menu.CheckMenuItem(ID_VIEW_TITLE, MF_BYCOMMAND | uState);
	if(m_bShowUserName == TRUE) uState = MF_CHECKED; else uState = MF_UNCHECKED;
	m_menu.CheckMenuItem(ID_VIEW_USERNAME, MF_BYCOMMAND | uState);
	if(m_bShowURL == TRUE) uState = MF_CHECKED; else uState = MF_UNCHECKED;
	m_menu.CheckMenuItem(ID_VIEW_URL, MF_BYCOMMAND | uState);
	if(m_bShowPassword == TRUE) uState = MF_CHECKED; else uState = MF_UNCHECKED;
	m_menu.CheckMenuItem(ID_VIEW_PASSWORD, MF_BYCOMMAND | uState);
	if(m_bShowNotes == TRUE) uState = MF_CHECKED; else uState = MF_UNCHECKED;
	m_menu.CheckMenuItem(ID_VIEW_NOTES, MF_BYCOMMAND | uState);
	if(m_bShowCreation == TRUE) uState = MF_CHECKED; else uState = MF_UNCHECKED;
	m_menu.CheckMenuItem(ID_VIEW_CREATION, MF_BYCOMMAND | uState);
	if(m_bShowLastMod == TRUE) uState = MF_CHECKED; else uState = MF_UNCHECKED;
	m_menu.CheckMenuItem(ID_VIEW_LASTMOD, MF_BYCOMMAND | uState);
	if(m_bShowLastAccess == TRUE) uState = MF_CHECKED; else uState = MF_UNCHECKED;
	m_menu.CheckMenuItem(ID_VIEW_LASTACCESS, MF_BYCOMMAND | uState);
	if(m_bShowExpire == TRUE) uState = MF_CHECKED; else uState = MF_UNCHECKED;
	m_menu.CheckMenuItem(ID_VIEW_EXPIRE, MF_BYCOMMAND | uState);
	if(m_bShowUUID == TRUE) uState = MF_CHECKED; else uState = MF_UNCHECKED;
	m_menu.CheckMenuItem(ID_VIEW_UUID, MF_BYCOMMAND | uState);
	if(m_bShowAttach == TRUE) uState = MF_CHECKED; else uState = MF_UNCHECKED;
	m_menu.CheckMenuItem(ID_VIEW_ATTACH, MF_BYCOMMAND | uState);
	if(m_bPasswordStars == TRUE) uState = MF_CHECKED; else uState = MF_UNCHECKED;
	m_menu.CheckMenuItem(ID_VIEW_HIDESTARS, MF_BYCOMMAND | uState);
	if(m_bUserStars == TRUE) uState = MF_CHECKED; else uState = MF_UNCHECKED;
	m_menu.CheckMenuItem(ID_VIEW_HIDEUSERS, MF_BYCOMMAND | uState);
	if(m_bAlwaysOnTop == TRUE) uState = MF_CHECKED; else uState = MF_UNCHECKED;
	m_menu.CheckMenuItem(ID_VIEW_ALWAYSONTOP, MF_BYCOMMAND | uState);

	if(m_bShowToolBar == TRUE) uState = MF_CHECKED; else uState = MF_UNCHECKED;
	m_menu.CheckMenuItem(ID_VIEW_SHOWTOOLBAR, MF_BYCOMMAND | uState);
	if(m_bEntryView == TRUE) uState = MF_CHECKED; else uState = MF_UNCHECKED;
	m_menu.CheckMenuItem(ID_VIEW_ENTRYVIEW, MF_BYCOMMAND | uState);

	if(m_bSimpleTANView == TRUE) uState = MF_CHECKED; else uState = MF_UNCHECKED;
	m_menu.CheckMenuItem(ID_VIEW_SIMPLETANVIEW, MF_BYCOMMAND | uState);
	if(m_bShowTANIndices == TRUE) uState = MF_CHECKED; else uState = MF_UNCHECKED;
	m_menu.CheckMenuItem(ID_VIEW_SHOWTANINDICES, MF_BYCOMMAND | uState);

	_CalcColumnSizes();
	// Windows computes the sizes wrong the first time because there aren't
	// any 16x16 icons in the list yet
	m_cList.InsertColumn(0, TRL("Title"), LVCFMT_LEFT, 10 - 4, 0);
	m_cList.InsertColumn(1, TRL("User Name"), LVCFMT_LEFT, 10 - 4, 1);
	m_cList.InsertColumn(2, TRL("URL"), LVCFMT_LEFT, 10 - 3, 2);
	m_cList.InsertColumn(3, TRL("Password"), LVCFMT_LEFT, 10 - 3, 3);
	m_cList.InsertColumn(4, TRL("Notes"), LVCFMT_LEFT, 10 - 3, 4);
	m_cList.InsertColumn(5, TRL("Creation Time"), LVCFMT_LEFT, 10 - 3, 4);
	m_cList.InsertColumn(6, TRL("Last Modification"), LVCFMT_LEFT, 10 - 3, 4);
	m_cList.InsertColumn(7, TRL("Last Access"), LVCFMT_LEFT, 10 - 3, 4);
	m_cList.InsertColumn(8, TRL("Expires"), LVCFMT_LEFT, 10 - 3, 4);
	m_cList.InsertColumn(9, TRL("UUID"), LVCFMT_LEFT, 10 - 3, 4);
	m_cList.InsertColumn(10, TRL("Attachment"), LVCFMT_LEFT, 10 - 3, 4);

	ASSERT(LVM_SETEXTENDEDLISTVIEWSTYLE == (0x1000 + 54));
	_SetListParameters();

	cConfig.Get(PWMKEY_LISTFONT, szTemp);
	if(szTemp[0] == 0) _tcscpy_s(szTemp, _countof(szTemp), _T("MS Shell Dlg;8,0000"));
	_ParseSpecAndSetFont(szTemp);

	// int nColumnWidth;
	// RECT rect;
	// m_cGroups.GetClientRect(&rect);
	// nColumnWidth = rect.right - rect.left - GetSystemMetrics(SM_CXVSCROLL);
	// nColumnWidth -= 8;
	// m_cGroups.InsertColumn(0, TRL("Password Groups"), LVCFMT_LEFT, nColumnWidth, 0);

	// m_cGroups.PostMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_SI_MENU | LVS_EX_INFOTIP);
	m_cGroups.ModifyStyle(0, TVS_TRACKSELECT, 0);

	const UINT32 ulTest = TestCryptoImpl();
	if(ulTest != 0)
	{
		CString strTCI = TRL("The following self-tests failed:");
		strTCI += _T("\r\n");

		if((ulTest & TI_ERR_SHAVAR32) != 0)
			{ strTCI += TRL("- SHA 32-bit variables"); strTCI += _T("\r\n"); }
		if((ulTest & TI_ERR_SHAVAR64) != 0)
			{ strTCI += TRL("- SHA 64-bit variables"); strTCI += _T("\r\n"); }
		if((ulTest & TI_ERR_SHACMP256) != 0)
			{ strTCI += TRL("- SHA-256 test vector(s)"); strTCI += _T("\r\n"); }
		if((ulTest & TI_ERR_SHACMP512) != 0)
			{ strTCI += TRL("- SHA-512 test vector(s)"); strTCI += _T("\r\n"); }
		if((ulTest & TI_ERR_RIJNDAEL_ENCRYPT) != 0)
			{ strTCI += TRL("- Rijndael encryption"); strTCI += _T("\r\n"); }
		if((ulTest & TI_ERR_RIJNDAEL_DECRYPT) != 0)
			{ strTCI += TRL("- Rijndael decryption"); strTCI += _T("\r\n"); }
		if((ulTest & TI_ERR_ARCFOUR_CRYPT) != 0)
			{ strTCI += TRL("- Arcfour crypto routine"); strTCI += _T("\r\n"); }
		if((ulTest & TI_ERR_TWOFISH) != 0)
			{ strTCI += TRL("- Twofish algorithm"); strTCI += _T("\r\n"); }

		strTCI += _T("\r\n");
		strTCI += TRL("The program will exit now.");
		MessageBox(strTCI, TRL("Self-Test(s) Failed"), MB_OK | MB_ICONWARNING);
		OnCancel();
	}

	m_strFile.Empty(); m_strFileAbsolute.Empty();
	m_cList.EnableWindow(FALSE);
	m_cGroups.EnableWindow(FALSE);

	// Initialize the xorshift pseudo-random number generator
	const DWORD dwSkip = ((timeGetTime() >> 3) & 0x03FF); // Shift out 8 ms precision
	for(DWORD dwXrPos = 0; dwXrPos < dwSkip; ++dwXrPos) randXorShift();

	// CRestartManagerEx::Register();

	// _UpdateTitleBar(); // Updated by _UpdateToolBar()

	ProcessResize();
	UpdateGroupList();
	UpdatePasswordList();

	m_bTimer = TRUE;
	m_nClipboardCountdown = -1;
	SetTimer(APPWND_TIMER_ID, 1000, NULL);
	SetTimer(APPWND_TIMER_ID_UPDATER, 500, NULL);

	m_bRememberLast = cConfig.GetBool(PWMKEY_REMEMBERLAST, TRUE);

	m_strLastDb.Empty();

	m_bOpenLastDb = cConfig.GetBool(PWMKEY_OPENLASTB, TRUE);
	m_bAutoSaveDb = cConfig.GetBool(PWMKEY_AUTOSAVEB, FALSE);

	m_reEntryView.LimitText(0);
	m_reEntryView.SetEventMask(ENM_MOUSEEVENTS | ENM_LINK);
	m_reEntryView.SendMessage(EM_AUTOURLDETECT, TRUE, 0);
	m_reEntryView.SetBackgroundColor(FALSE, GetSysColor(COLOR_3DFACE));

	/* LVBKIMAGE lvbk;
	TCHAR szMe[MAX_PATH * 2];
	GetModuleFileName(NULL, szMe, MAX_PATH * 2);
	CString strImagePath;
	strImagePath.Format(_T("res://%s/#2/#199"), szMe);

	LPCTSTR lpImage = strImagePath.LockBuffer();
	ZeroMemory(&lvbk, sizeof(LVBKIMAGE));
	lvbk.cchImageMax = _tcslen(lpImage);
	lvbk.pszImage = (LPTSTR)lpImage;
	lvbk.ulFlags = LVBKIF_SOURCE_URL | LVBKIF_STYLE_NORMAL;
	lvbk.xOffsetPercent = 90;
	lvbk.yOffsetPercent = 90;
	ListView_SetBkImage(m_cList.m_hWnd, &lvbk);
	strImagePath.UnlockBuffer(); */

	m_hTrayIconNormal = AfxGetApp()->LoadIcon(IDI_UNLOCKED);
	m_hTrayIconLocked = AfxGetApp()->LoadIcon(IDI_LOCKED);
	m_hLockOverlayIcon = AfxGetApp()->LoadIcon(IDI_LOCK_OVERLAY);

	CMsgRelayWnd::SetRelayTarget(this->m_hWnd);
	CMsgRelayWnd::AddRelayedMessage(WM_QUERYENDSESSION);
	CMsgRelayWnd::AddRelayedMessage(WM_ENDSESSION);
	CMsgRelayWnd::AddRelayedMessage(WM_COPYDATA);
	CMsgRelayWnd::AddRelayedMessage(WM_WTSSESSION_CHANGE);
	CMsgRelayWnd::AddRelayedMessage(WM_REG_TASKBARCREATED);
	CMsgRelayWnd::AddRelayedMessage(WM_REG_TASKBARBUTTONCREATED);
	CMsgRelayWnd::AddRelayedMessage(WM_REG_PROCESSMAILSLOT);
	CMsgRelayWnd::AddRelayedMessage(WM_REG_KEEPASSCONTROL);

	VERIFY(m_systray.Create(this, WM_MY_SYSTRAY_NOTIFY, PWM_PRODUCT_NAME_SHORT,
		m_hTrayIconNormal, IDR_SYSTRAY_MENU, FALSE,
		NULL, NULL, NIIF_NONE, 0));
	m_systray.SetMenuDefaultItem(0, TRUE);
	m_systray.MoveToRight();

	for(DWORD ttc = 0; ttc < 11; ++ttc) m_aHeaderOrder[ttc] = ttc;
	m_aHeaderOrder[2] = 3; m_aHeaderOrder[3] = 2;
	cConfig.Get(PWMKEY_HEADERORDER, szTemp);
	if(szTemp[0] != 0) str2ar(szTemp, m_aHeaderOrder, 11);
	NewGUI_SetHeaderOrder(m_cList.m_hWnd, m_aHeaderOrder, 11);

	m_pThemeHelper = new CThemeHelperST();
	NewGUI_SetThemeHelper((void *)m_pThemeHelper);

	NewGUI_ToolBarButton(&m_btnTbNew, IDB_TB_NEW, IDB_TB_NEW);
	NewGUI_ToolBarButton(&m_btnTbOpen, IDB_TB_OPEN, IDB_TB_OPEN);
	NewGUI_ToolBarButton(&m_btnTbSave, IDB_TB_SAVE, IDB_TB_SAVE);
	NewGUI_ToolBarButton(&m_btnTbAddEntry, IDB_TB_ADDENTRY, IDB_TB_ADDENTRY);
	NewGUI_ToolBarButton(&m_btnTbEditEntry, IDB_TB_EDITENTRY, IDB_TB_EDITENTRY);
	NewGUI_ToolBarButton(&m_btnTbDeleteEntry, IDB_TB_DELETEENTRY, IDB_TB_DELETEENTRY);
	NewGUI_ToolBarButton(&m_btnTbCopyPw, IDB_TB_COPYPW, IDB_TB_COPYPW);
	NewGUI_ToolBarButton(&m_btnTbCopyUser, IDB_TB_COPYUSER, IDB_TB_COPYUSER);
	NewGUI_ToolBarButton(&m_btnTbFind, IDB_TB_FIND, IDB_TB_FIND);
	NewGUI_ToolBarButton(&m_btnTbLock, IDB_TB_LOCK, IDB_TB_LOCK);
	NewGUI_ToolBarButton(&m_btnTbAbout, IDB_TB_ABOUT, IDB_TB_ABOUT);

	_ShowToolBar(m_bShowToolBar);

	m_hAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCEL_MAIN));
	ASSERT(m_hAccel != NULL);

	VERIFY(m_tip.Create(this, 0x40));
	CString strQFHint = TRL("Type to search the database");
	strQFHint += _T(" (");
	strQFHint += TRL("Ctrl");
	strQFHint += _T("+E)");
	m_tip.AddTool(&m_cQuickFind, strQFHint);
	m_tip.Activate(TRUE);

	NewGUI_SetCueBanner_CB(m_cQuickFind.m_hWnd, TRL("Search..."));

	m_lSplitterPosHoriz = NewGUI_Scale(GUI_GROUPLIST_EXT + 1, this);
	m_lSplitterPosVert = NewGUI_Scale((rectClient.bottom - rectClient.top -
		PWS_DEFAULT_SPLITTER_Y) / 4, this);
	ASSERT(m_lSplitterPosVert > 0);

	int px, py, dx, dy; // Restore window position
	cConfig.Get(PWMKEY_WINDOWPX, szTemp);
	if(szTemp[0] != 0) px = _ttoi(szTemp); else px = -1;
	cConfig.Get(PWMKEY_WINDOWPY, szTemp);
	if(szTemp[0] != 0) py = _ttoi(szTemp); else py = -1;
	cConfig.Get(PWMKEY_WINDOWDX, szTemp);
	if(szTemp[0] != 0) dx = _ttoi(szTemp); else dx = -1;
	cConfig.Get(PWMKEY_WINDOWDY, szTemp);
	if(szTemp[0] != 0) dy = _ttoi(szTemp); else dy = -1;

	const int pxVirtScreen = GetSystemMetrics(SM_XVIRTUALSCREEN);
	const int cxVirtScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	const int pyVirtScreen = GetSystemMetrics(SM_YVIRTUALSCREEN);
	const int cyVirtScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	if((px == 3000) && (py == 3000))
	{
		px = pxVirtScreen;
		py = pyVirtScreen;
	}

	if(px >= (pxVirtScreen + cxVirtScreen)) px = pxVirtScreen;
	if(py >= (pyVirtScreen + cyVirtScreen)) py = pyVirtScreen;

	if((px != -1) && (py != -1) && (dx != -1) && (dy != -1))
	{
		if((px + dx) <= pxVirtScreen) px = pxVirtScreen;
		if((py + dy) <= pyVirtScreen) py = pyVirtScreen;

		SetWindowPos(&wndNoTopMost, px, py, dx, dy, SWP_NOOWNERZORDER | SWP_NOZORDER);
	}

	for(DWORD ulCol = 0; ulCol < 11; ++ulCol) m_nColumnWidths[ulCol] = 0;

	// Restore column sizes
	cConfig.Get(PWMKEY_COLWIDTH0, szTemp);
	if(szTemp[0] != 0) m_nColumnWidths[0] = _ttoi(szTemp);
	cConfig.Get(PWMKEY_COLWIDTH1, szTemp);
	if(szTemp[0] != 0) m_nColumnWidths[1] = _ttoi(szTemp);
	cConfig.Get(PWMKEY_COLWIDTH2, szTemp);
	if(szTemp[0] != 0) m_nColumnWidths[2] = _ttoi(szTemp);
	cConfig.Get(PWMKEY_COLWIDTH3, szTemp);
	if(szTemp[0] != 0) m_nColumnWidths[3] = _ttoi(szTemp);
	cConfig.Get(PWMKEY_COLWIDTH4, szTemp);
	if(szTemp[0] != 0) m_nColumnWidths[4] = _ttoi(szTemp);
	cConfig.Get(PWMKEY_COLWIDTH5, szTemp);
	if(szTemp[0] != 0) m_nColumnWidths[5] = _ttoi(szTemp);
	cConfig.Get(PWMKEY_COLWIDTH6, szTemp);
	if(szTemp[0] != 0) m_nColumnWidths[6] = _ttoi(szTemp);
	cConfig.Get(PWMKEY_COLWIDTH7, szTemp);
	if(szTemp[0] != 0) m_nColumnWidths[7] = _ttoi(szTemp);
	cConfig.Get(PWMKEY_COLWIDTH8, szTemp);
	if(szTemp[0] != 0) m_nColumnWidths[8] = _ttoi(szTemp);
	cConfig.Get(PWMKEY_COLWIDTH9, szTemp);
	if(szTemp[0] != 0) m_nColumnWidths[9] = _ttoi(szTemp);
	cConfig.Get(PWMKEY_COLWIDTH10, szTemp);
	if(szTemp[0] != 0) m_nColumnWidths[10] = _ttoi(szTemp);

	cConfig.Get(PWMKEY_SPLITTERX, szTemp);
	if(szTemp[0] != 0) m_lSplitterPosHoriz = _ttol(szTemp);
	else m_lSplitterPosHoriz = NewGUI_Scale(GUI_GROUPLIST_EXT + 1, this);

	cConfig.Get(PWMKEY_SPLITTERY, szTemp);
	if(szTemp[0] != 0) m_lSplitterPosVert = _ttol(szTemp);
	// else m_lSplitterPosVert = PWS_DEFAULT_SPLITTER_Y; // Set above already

	// if(m_bAlwaysOnTop == TRUE)
	//	SetWindowPos(&wndTopMost, 0, 0, 0, 0,
	//		SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

	VERIFY(m_cGroups.InitDropHandler());

	cConfig.Get(PWMKEY_GENPROFILEAUTO, szTemp);
	if(szTemp[0] != 0)
	{
		std::basic_string<TCHAR> strAutoProfile = szTemp;
		PwgStringToProfile(strAutoProfile, &CPwSafeDlg::m_pgsAutoProfile);
	}
	else PwgGetDefaultProfile(&CPwSafeDlg::m_pgsAutoProfile);

	// Support legacy auto-generation flag
	if((cConfig.GetBool(PWMKEY_AUTOPWGEN, TRUE) == FALSE) &&
		(CPwSafeDlg::m_bMiniMode == FALSE))
	{
		CPwSafeDlg::m_pgsAutoProfile.dwLength = 0;
	}

	// KP_APP_INFO kpai;
	// ZeroMemory(&kpai, sizeof(KP_APP_INFO));
	// kpai.dwAppVersion = PWM_VERSION_DW;
	// kpai.hwndMain = this->m_hWnd;
	// // kpai.pApp = (void *)AfxGetApp();
	// kpai.pMainDlg = (void *)this;
	// kpai.pPwMgr = (void *)&m_mgr;
	// kpai.lpKPCall = KP_Call;
	// kpai.lpKPQuery = KP_Query;

	// CPluginManager::Instance().SetAppInfo(&kpai);
	VERIFY(CPluginManager::Instance().SetDirectCommandRange(WM_PLUGINS_FIRST,
		WM_PLUGINS_LAST));

	// CString strPluginKey;
	// int jj = 0;
	// while(1)
	// {
	//	strPluginKey.Format(_T("KeePlugin_%d"), jj);
	//	cConfig.Get(strPluginKey, szTemp);
	//	if(szTemp[0] == 0) break;
	//	if(_tcscmp(szTemp, _T("0")) == 0) break;
	//	if(szTemp[0] != _T('-'))
	//	{
	//		std_string strPluginName = szTemp;
	//		CPluginManager::Instance().m_vKnownNames.push_back(strPluginName);
	//	}
	//	++jj;
	// }

	// std_string strBase = Executable::instance().getPathOnly();
	// VERIFY(CPluginManager::Instance().AddAllPlugins(strBase.c_str(), _T("*.dll"), TRUE));
	// CPluginManager::Instance().m_vKnownNames.clear();

	// CString strPluginKey;
	// jj = 0;
	// while(1)
	// {
	//	strPluginKey.Format(_T("KeePlugin_%d"), jj);
	//	cConfig.Get(strPluginKey, szTemp);
	//	if(szTemp[0] == 0) break;
	//	if(_tcscmp(szTemp, _T("0")) == 0) break;
	//	if(szTemp[0] == _T('-'))
	//		CPluginManager::Instance().EnablePluginByStr(&szTemp[1], FALSE);
	//	else
	//		CPluginManager::Instance().EnablePluginByStr(szTemp, TRUE);
	//	jj++;
	// }

	// Plugins might access the command line parameters, so restore the
	// initial working directory now
	const std::basic_string<TCHAR> strCurDir = WU_GetCurrentDirectory();
	WU_SetCurrentDirectory(m_strInitialEnvDir.c_str());

	VERIFY(CPluginManager::Instance().LoadAllPlugins());
	BuildPluginMenu();
	_CallPlugins(KPM_DELAYED_INIT, 0, 0);
	_CallPlugins(KPM_READ_COMMAND_ARGS, 0, 0);

	WU_SetCurrentDirectory(strCurDir.c_str());

	CString strLoadedIniLastDb;
	BOOL bTriedOpenDb = FALSE;
	if(_ParseCommandLine() == FALSE)
	{
		if((m_bOpenLastDb == TRUE) && (m_bRememberLast == TRUE))
		{
			cConfig.Get(PWMKEY_LASTDB, szTemp);
			if(szTemp[0] != 0)
			{
				std_string strTemp = WU_ExpandEnvironmentVars(szTemp);
				strLoadedIniLastDb = strTemp.c_str();

				if(WU_IsAbsolutePath(strTemp.c_str()) == FALSE)
					strTemp = Executable::instance().getPathOnly() + strTemp;

				if(_FileAccessible(strTemp.c_str()) == TRUE)
				{
					_OpenDatabase(NULL, strTemp.c_str(), NULL, NULL, m_bStartMinimized, NULL, FALSE);
					bTriedOpenDb = TRUE;
				}
			}
		}
	}

	m_bIgnoreSizeEvent = FALSE;

	UpdateAutoSortMenuItems();
	_UpdateToolBar(TRUE);
	ProcessResize();

	if(m_dwATHotKey != 0) RegisterGlobalHotKey(HOTKEYID_AUTOTYPE, m_dwATHotKey, FALSE, FALSE);

	m_bRegisterRestoreHotKey = cConfig.GetBool(PWMKEY_RESTOREHOTKEY, TRUE);
	RegisterRestoreHotKey(m_bRegisterRestoreHotKey);

	// PostMessage(WM_NULL, 0, 0);

	m_uACP = GetACP();

	m_sessionNotify.Register(this->m_hWnd);
	if(m_bCheckForUpdate == TRUE) CUpdateCheckEx::Check(TRUE, this->m_hWnd, &m_ilIcons, TRUE);

	UpdateWindow();
	RedrawWindow();

	m_bWasMaximized = cConfig.GetBool(PWMKEY_WINSTATE_MAX, FALSE);
	if(m_bWasMaximized == TRUE) ShowWindow(SW_SHOWMAXIMIZED);

	if(m_bAlwaysOnTop == TRUE)
		SetWindowPos(&wndTopMost, 0, 0, 0, 0,
			SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

	if((m_bStartMinimized == TRUE) || CmdArgs::instance().minimizeInEffect())
		SetViewHideState(FALSE, FALSE);

	_CallPlugins(KPM_WND_INIT_POST, 0, 0);
	m_bDisplayDialog = FALSE;

	SetToMiniModeIfEnabled(&m_menu, TRUE, TRUE);
	if((m_bMiniMode == TRUE) && (m_bFileOpen == FALSE) && (m_bLocked == FALSE))
	{
		if((bTriedOpenDb == FALSE) && (m_bUnintrusiveMiniMode == FALSE))
		{
			OnFileNew();

			if(m_bFileOpen == FALSE)
			{
				OnFileExit();
				return FALSE;
			}
			else if(m_strFile.GetLength() == 0)
				_GetNewDbFileInUserDir(strLoadedIniLastDb, cConfig);
			else { ASSERT(FALSE); }

			SetViewHideState(TRUE, FALSE); // Always show window for newly created DB
		}

		_UpdateToolBar();
	}

	// m_btnTbAbout.SetFocus();
	// if((m_bFileOpen == FALSE) && (m_cQuickFind.IsWindowEnabled() == TRUE)) m_cQuickFind.SetFocus();
	// else m_btnTbAbout.SetFocus();
	_SetDefaultFocus();

	if((bTriedOpenDb == FALSE) && (m_bStartMinimized == TRUE))
	{
		if((m_bMiniMode == TRUE) && (m_bUnintrusiveMiniMode == FALSE))
		{
			SetViewHideState(TRUE, FALSE); // Show window with newly created DB
		}
	}

	NotifyUserActivity(); // Initialize the locking timeout
	return FALSE; // We set the focus ourselves
}

void CPwSafeDlg::_SetDefaultFocus()
{
	if(m_cQuickFind.IsWindowEnabled() == TRUE) m_cQuickFind.SetFocus();
	else m_btnTbAbout.SetFocus();
}

void CPwSafeDlg::_TranslateMenu(BCMenu *pBCMenu, BOOL bAppendSuffix, BOOL *pFlags)
{
	CString strItem, strNew;
	UINT nItem = 0;
	BCMenu *pNext;
	const TCHAR *pSuffix = _T("");
	BOOL bDefaultFlag = FALSE;

	ASSERT(pBCMenu != NULL); if(pBCMenu == NULL) return;

	if(pFlags == NULL) pFlags = &bDefaultFlag;

	while(true)
	{
		if(pBCMenu->GetMenuText((UINT)nItem, strItem, MF_BYPOSITION) == FALSE) break;
		if(bAppendSuffix == TRUE) pSuffix = _GetCmdAccelExt(strItem);

		CString strSource = strItem;

		if((strItem == _T("&Import From")) || (strItem == _T("&Export To")) ||
			(strItem == _T("Show &Columns")) || (strItem == _T("&Rearrange")) ||
			(strItem == _T("Auto-&Sort Password List")) || (strItem == _T("TAN View &Options")) ||
			(strItem == _T("&Export Group To")) || (strItem == _T("Insert Field Reference")))
		{
			pNext = pBCMenu->GetSubBCMenu(const_cast<LPTSTR>((LPCTSTR)strItem));
			if(pNext != NULL) _TranslateMenu(pNext, TRUE, pFlags);
		}

		// Replace URL menu item if required, pFlags[0] must be a pointer to m_bCopyURLs
		if(*pFlags == TRUE)
			if(strItem == _T("Open &URL(s)"))
				strItem = _T("Copy &URL to Clipboard");

		if(strItem.GetLength() != 0) strNew = TRL_VAR(strItem);
		else strNew = _T("");

		if(_tcslen(pSuffix) != 0) { strNew += _T("\t"); strNew += pSuffix; }
		if(pBCMenu->SetMenuText(nItem, strNew, MF_BYPOSITION) == FALSE) { ASSERT(FALSE); }

		++nItem;

		if(m_bMiniMode == TRUE)
		{
			if(strSource == _T("&Import From"))
			{
				--nItem;
				if(m_bForceAllowImport == FALSE)
					pBCMenu->DeleteMenu(nItem, MF_BYPOSITION);
				else ++nItem;
			}
			else if(strSource == _T("&Export To"))
			{
				--nItem;
				if(m_bForceAllowExport == FALSE)
				{
					pBCMenu->DeleteMenu(nItem, MF_BYPOSITION); // Export

					if(m_bForceAllowImport == FALSE)
						pBCMenu->DeleteMenu(nItem, MF_BYPOSITION); // Separator
				}
				else ++nItem;
			}
			else if(strSource == _T("TAN View &Options"))
			{
				--nItem;
				pBCMenu->DeleteMenu(nItem, MF_BYPOSITION);
			}
		}
	}
}

BOOL CPwSafeDlg::_ParseCommandLine()
{
	/* CString str;
	LPCTSTR lpPassword = NULL;
	LPCTSTR lpKeyFile = NULL;
	LPCTSTR lpPreSelectPath = NULL;

	if(CPwSafeApp::ParseCurrentCommandLine(&str, &lpPassword, &lpKeyFile, &lpPreSelectPath) == FALSE) return FALSE;

	if(_FileAccessible(str) == TRUE)
		_OpenDatabase(NULL, str, lpPassword, lpKeyFile, FALSE, lpPreSelectPath, FALSE); */

	// Ensure that relative paths on the command line are evaluated relatively to
	// the initial working directory instead of the last used directory that
	// KeePass remembers; FR 3059831
	const std::basic_string<TCHAR> strCurDir = WU_GetCurrentDirectory();
	WU_SetCurrentDirectory(m_strInitialEnvDir.c_str());
	CmdArgs::instance().getDatabase();
	WU_SetCurrentDirectory(strCurDir.c_str());

	m_bFileReadOnly = (CmdArgs::instance().readonlyIsInEffect() ? TRUE : FALSE);

	const FullPathName& fpnDatabase = CmdArgs::instance().getDatabase();
	const bool bFileNameExists = (fpnDatabase.getState() == FullPathName::PATH_AND_FILENAME);
	if(!bFileNameExists) return FALSE;

	const TCHAR* const lpFileName = fpnDatabase.getFullPathName().c_str();
	if(_FileAccessible(lpFileName) == FALSE)
	{
		std_string strMsg = TRL("File access error: failed to open file in read mode");
		strMsg += _T("!\r\n\r\n");
		strMsg += lpFileName;

		MessageBox(strMsg.c_str(), PWM_PRODUCT_NAME_SHORT, MB_OK | MB_ICONWARNING);

		return TRUE;
	}

	m_bInitialCmdLineFile = TRUE;

	const std_string& strPassword = CmdArgs::instance().getPassword();
	const FullPathName& fpnKeyFile = CmdArgs::instance().getKeyfile();
	enum { PATH_EXISTS = FullPathName::PATH_ONLY | FullPathName::PATH_AND_FILENAME };
	const TCHAR* const lpKeyFile = ((fpnKeyFile.getState() & PATH_EXISTS) ?
		fpnKeyFile.getFullPathName().c_str() : NULL);
	const bool bPreSelectIsInEffect = CmdArgs::instance().preselectIsInEffect();
	const BOOL bLocked = (CmdArgs::instance().lockIsInEffect() ? TRUE : FALSE);

	_OpenDatabase(NULL, lpFileName, strPassword.empty() ? NULL : strPassword.c_str(),
		(!bPreSelectIsInEffect) ? lpKeyFile : NULL, bLocked,
		bPreSelectIsInEffect ? lpKeyFile : NULL, FALSE);

	return TRUE;
}

void CPwSafeDlg::_ParseSpecAndSetFont(const TCHAR *pszSpec)
{
	HFONT hPre = (HFONT)m_fListFont;
	CDC *pDC = GetDC();
	HDC hDC = pDC->m_hDC;
	CString strFontSpec, strTemp;
	LOGFONT lf;
	int nSize;

	ASSERT(pszSpec != NULL);
	if(pszSpec == NULL) { ReleaseDC(pDC); return; }

	ZeroMemory(&lf, sizeof(LOGFONT));

	if(hPre != NULL) m_fListFont.DeleteObject();

	if(_tcslen(pszSpec) != 0) // Font spec format: <FACE>;<SIZE>,<FLAGS>
	{
		strFontSpec = pszSpec;
		CString strFace, strSize, strFlags;
		int nChars = strFontSpec.ReverseFind(_T(';'));
		int nSizeEnd = strFontSpec.ReverseFind(_T(','));
		strFace = strFontSpec.Left(nChars);
		strSize = strFontSpec.Mid(nChars + 1, nSizeEnd - nChars - 1);
		strFlags = strFontSpec.Right(4);
		nSize = _ttoi(strSize);
		int nWeight = FW_NORMAL;
		if(strFlags.GetAt(0) == _T('1')) nWeight = FW_BOLD;
		BYTE bItalic = (BYTE)((strFlags.GetAt(1) == _T('1')) ? TRUE : FALSE);
		BYTE bUnderlined = (BYTE)((strFlags.GetAt(2) == _T('1')) ? TRUE : FALSE);
		BYTE bStrikeOut = (BYTE)((strFlags.GetAt(3) == _T('1')) ? TRUE : FALSE);

		if(nSize < 0) nSize = -nSize;
		if(strFace.GetLength() >= 32) strFace = strFace.Left(31);

		lf.lfCharSet = DEFAULT_CHARSET; lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lf.lfEscapement = 0; lf.lfItalic = bItalic; lf.lfOrientation = 0;
		lf.lfOutPrecision = OUT_DEFAULT_PRECIS; lf.lfQuality = DEFAULT_QUALITY;
		lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
		lf.lfStrikeOut = bStrikeOut; lf.lfUnderline = bUnderlined;
		lf.lfWeight = nWeight; lf.lfWidth = 0;
		_tcscpy_s(lf.lfFaceName, _countof(lf.lfFaceName), strFace);
		lf.lfHeight = -MulDiv(nSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);

		m_fListFont.CreateFontIndirect(&lf);
	}
	else
	{
		nSize = 8;
		m_fListFont.CreateFont(nSize, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, _T("MS Shell Dlg"));
	}

	ZeroMemory(&lf, sizeof(LOGFONT));
	m_fListFont.GetLogFont(&lf);
	strFontSpec = lf.lfFaceName;
	strFontSpec += _T(';');
	strTemp.Format(_T("%d"), nSize);
	strFontSpec += strTemp;
	strFontSpec += _T(",");
	if(lf.lfWeight == FW_BOLD) strFontSpec += _T('1'); else strFontSpec += _T('0');
	if(lf.lfItalic != 0) strFontSpec += _T('1'); else strFontSpec += _T('0');
	if(lf.lfUnderline != 0) strFontSpec += _T('1'); else strFontSpec += _T('0');
	if(lf.lfStrikeOut != 0) strFontSpec += _T('1'); else strFontSpec += _T('0');

	m_strFontSpec = strFontSpec;
	m_strListFontFace = lf.lfFaceName;
	m_nListFontSize = nSize;

	ASSERT(m_fListFont.m_hObject != NULL);
	m_cGroups.SetFont(&m_fListFont, TRUE);
	m_cList.SetFont(&m_fListFont, TRUE);

	ReleaseDC(pDC);
}

// This should be replaced by a function that scans the accelerator table
const TCHAR *CPwSafeDlg::_GetCmdAccelExt(const TCHAR *psz)
{
	static LPCTSTR pEmpty = _T("");

	ASSERT(psz != NULL); if(psz == NULL) return pEmpty;

	if(_tcsicmp(psz, _T("&New...")) == 0) return _T("Ctrl+N");
	if(_tcsicmp(psz, _T("&Open...")) == 0) return _T("Ctrl+O");
	if(_tcsicmp(psz, _T("&Save")) == 0) return _T("Ctrl+S");
	if(_tcsicmp(psz, _T("&Close")) == 0) return _T("Ctrl+W");
	if(_tcsicmp(psz, _T("&Print...")) == 0) return _T("Ctrl+P");
	if(_tcsicmp(psz, _T("&Database Settings...")) == 0) return _T("Ctrl+I");
	if(_tcsicmp(psz, _T("&Lock Workspace")) == 0) return _T("Ctrl+L");
	if(_tcsicmp(psz, _T("E&xit")) == 0) return _T("Ctrl+X");

	if(_tcsicmp(psz, _T("&Add Entry...")) == 0) return _T("Ctrl+Y");
	if(_tcsicmp(psz, _T("&Edit/View Entry...")) == 0) return _T("Return");
	if(_tcsicmp(psz, _T("&Delete Entry")) == 0) return _T("Ctrl+D");
	if(_tcsicmp(psz, _T("Se&lect All")) == 0) return _T("Ctrl+A");
	if(_tcsicmp(psz, _T("&Find in Database...")) == 0) return _T("Ctrl+F");
	if(_tcsicmp(psz, _T("Find in t&his Group...")) == 0) return _T("Ctrl+Shift+F");
	if(_tcsicmp(psz, _T("Add &Group...")) == 0) return _T("Ctrl+G");
	if(_tcsicmp(psz, _T("Open &URL(s)")) == 0) return _T("Ctrl+U");
	if(_tcsicmp(psz, _T("Copy &Password")) == 0) return _T("Ctrl+C");
	if(_tcsicmp(psz, _T("Copy User &Name")) == 0) return _T("Ctrl+B");
	if(_tcsicmp(psz, _T("Dupli&cate Entry")) == 0) return _T("Ctrl+K");
	if(_tcsicmp(psz, _T("&Options...")) == 0) return _T("Ctrl+M");
	if(_tcsicmp(psz, _T("Perform Auto-&Type")) == 0) return _T("Ctrl+V");

	if(_tcsicmp(psz, _T("Password &Generator...")) == 0) return _T("Ctrl+Z");

	if(_tcsicmp(psz, _T("Hide &User Names Behind Asterisks (***)")) == 0) return _T("Ctrl+J");
	if(_tcsicmp(psz, _T("Hide &Passwords Behind Asterisks (***)")) == 0) return _T("Ctrl+H");

	if(_tcsicmp(psz, _T("Move Entry to &Top")) == 0) return _T("Alt+Home");
	if(_tcsicmp(psz, _T("Move Entry &One Up")) == 0) return _T("Alt+Up");
	if(_tcsicmp(psz, _T("Mo&ve Entry One Down")) == 0) return _T("Alt+Down");
	if(_tcsicmp(psz, _T("Move Entry to &Bottom")) == 0) return _T("Alt+End");

	if(_tcsicmp(psz, _T("Move Group to &Top")) == 0) return _T("Alt+Home");
	if(_tcsicmp(psz, _T("Move Group One &Up")) == 0) return _T("Alt+Up");
	if(_tcsicmp(psz, _T("Move Group &One Down")) == 0) return _T("Alt+Down");
	if(_tcsicmp(psz, _T("Move Group to &Bottom")) == 0) return _T("Alt+End");
	if(_tcsicmp(psz, _T("Move Group One &Left")) == 0) return _T("Alt+Left");
	if(_tcsicmp(psz, _T("Move Group One &Right")) == 0) return _T("Alt+Right");

	if(_tcsicmp(psz, _T("&Help Contents")) == 0) return _T("F1");

	return pEmpty;
}

void CPwSafeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// Map close button to minimize button if the user wants this
	if((nID == SC_CLOSE) && (m_bCloseMinimizes == TRUE))
	{
		::SendMessage(this->m_hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
		return;
	}

	if((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
		return;
	}
	else if(nID == SC_MINIMIZE)
	{
		_SaveWindowPositionAndSize(NULL);

		if(m_bMinimizeToTray == TRUE)
		{
			SetTrayState(TRUE);
			return;
		}
	}

	CDialog::OnSysCommand(nID, lParam);
}

void CPwSafeDlg::OnPaint()
{
	if(IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

		const int cxIcon = GetSystemMetrics(SM_CXICON);
		const int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		const int x = ((rect.Width() - cxIcon + 1) / 2);
		const int y = ((rect.Height() - cyIcon + 1) / 2);

		dc.DrawIcon(x, y, m_hIcon);
	}
	else CDialog::OnPaint();
}

HCURSOR CPwSafeDlg::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}

void CPwSafeDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	bool bSetFlag = false;

	if(lpMeasureItemStruct->CtlType == ODT_MENU)
	{
		if(IsMenu((HMENU)(UINT_PTR)lpMeasureItemStruct->itemID) &&
			BCMenu::IsMenu((HMENU)(UINT_PTR)lpMeasureItemStruct->itemID))
		{
			m_menu.MeasureItem(lpMeasureItemStruct);
			bSetFlag = true;
		}
	}

	if(!bSetFlag) CDialog::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

LRESULT CPwSafeDlg::OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu)
{
	LRESULT lResult = 0;

	if(BCMenu::IsMenu(pMenu))
		lResult = BCMenu::FindKeyboardShortcut(nChar, nFlags, pMenu);
	else
		lResult = CDialog::OnMenuChar(nChar, nFlags, pMenu);

	return lResult;
}

void CPwSafeDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	NotifyUserActivity();

	m_dwLastFirstSelectedItem = GetSelectedEntry();
	m_dwLastNumSelectedItems = GetSelectedEntriesCount();
	m_hLastSelectedGroup = m_cGroups.GetSelectedItem();

	if(m_dwLastFirstSelectedItem != DWORD_MAX)
		m_dwLastEntryIndex = _ListSelToEntryIndex(m_dwLastFirstSelectedItem);
	else m_dwLastEntryIndex = DWORD_MAX;

	// CMenu *p = m_menu.GetSubMenu(TRL("&Tools"));
	// if(p == pPopupMenu) RebuildPluginMenu();

	_CallPlugins(KPM_INIT_MENU_POPUP, 0, 0);

	CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	OnUpdateFlush(pPopupMenu);

	if(!bSysMenu)
		if(BCMenu::IsMenu(pPopupMenu)) BCMenu::UpdateMenu(pPopupMenu);
}

// BCMenu function
void CPwSafeDlg::OnUpdateFlush(CMenu *pMenu)
{
	ASSERT(pMenu != NULL);
	// check the enabled state of various menu items

	CCmdUI state;
	state.m_pMenu = pMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// determine if menu is popup in top-level menu and set m_pOther to
	//  it if so (m_pParentMenu == NULL indicates that it is secondary popup)
	HMENU hParentMenu;
	if(AfxGetThreadState()->m_hTrackingMenu == pMenu->m_hMenu)
		state.m_pParentMenu = pMenu;    // parent == child for tracking popup
	else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
	{
		CWnd* pParent = GetTopLevelParent();
		// child windows don't have menus -- need to go to the top!
		if(pParent != NULL && (hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if (::GetSubMenu(hParentMenu, nIndex) == pMenu->m_hMenu)
				{
					// when popup is found, m_pParentMenu is containing menu
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	state.m_nIndexMax = pMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++)
	{
		state.m_nID = pMenu->GetMenuItemID(state.m_nIndex);
		if(state.m_nID == 0) continue; // menu separator or invalid cmd - ignore it

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// possibly a popup menu, route to first item of that popup
			state.m_pSubMenu = pMenu->GetSubMenu(state.m_nIndex);
			if((state.m_pSubMenu == NULL) ||
				((state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0) ||
				(state.m_nID == (UINT)(-1)))
			{
				continue;       // first item of popup can't be routed to
			}
			state.DoUpdate(this, FALSE);    // popups are never auto disabled
		}
		else
		{
			// normal menu item
			// Auto enable/disable if frame window has 'm_bAutoMenuEnable'
			//    set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, state.m_nID < 0xF000);
		}

		// adjust for menu deletions and additions
		UINT nCount = pMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}
}

void CPwSafeDlg::OnSize(UINT nType, int cx, int cy)
{
	if(cx < MWS_MIN_CX) cx = MWS_MIN_CX; // Minimum sizes
	if(cy < MWS_MIN_CY) cy = MWS_MIN_CY;

	CDialog::OnSize(nType, cx, cy);

	BOOL bWasInvisible = FALSE;
	if(m_bMinimized == TRUE) bWasInvisible = TRUE;

	if(nType == SIZE_MAXIMIZED) m_bWasMaximized = TRUE;
	else if(m_bTrayed == FALSE)
	{
		if((nType == SIZE_RESTORED) && (m_bWasMaximized == TRUE)) m_bWasMaximized = FALSE;
		else if((nType == SIZE_RESTORED) && (m_bMinimized == FALSE)) m_bWasMaximized = FALSE;
	}

	if(nType == SIZE_MINIMIZED) m_bMinimized = TRUE;
	else if((nType == SIZE_RESTORED) && (m_bMinimized == TRUE)) m_bMinimized = FALSE;
	else if((nType == SIZE_MAXIMIZED) && (m_bMinimized == TRUE)) m_bMinimized = FALSE;

	if(m_bIgnoreSizeEvent == TRUE) return;

	if(nType == SIZE_MINIMIZED)
	{
		if(m_bMinimizeToTray == TRUE) SetTrayState(TRUE);
	}

	if(nType != SIZE_MINIMIZED) ProcessResize();

	BOOL m_bRestore = FALSE;

	if(((nType == SIZE_RESTORED) || (nType == SIZE_MAXIMIZED)) &&
		(bWasInvisible == TRUE))
	{
		_ChangeLockState(FALSE);
	}
	else if(nType == SIZE_MINIMIZED)
	{
		if(m_bLockOnMinimize == TRUE)
		{
			_ChangeLockState(TRUE); // Lock

			// Was the locking successful? If not: restore window
			if((m_bFileOpen == TRUE) && (m_bLocked == FALSE)) m_bRestore = TRUE;
		}
	}

	if(m_bRestore == TRUE)
	{
		if(m_bWasMaximized == TRUE) ShowWindow(SW_MAXIMIZE);
		else ShowWindow(SW_RESTORE);
		_UpdateTrayIcon();
	}

	_UpdateToolBar();
	_SaveWindowPositionAndSize(NULL);
}

void CPwSafeDlg::OnSizing(UINT nSide, LPRECT lpRect)
{
	if((lpRect->right - lpRect->left) < MWS_MIN_CX) // Minimum width
	{
		if((nSide == WMSZ_BOTTOMLEFT) || (nSide == WMSZ_LEFT) || (nSide == WMSZ_TOPLEFT))
			lpRect->left = lpRect->right - MWS_MIN_CX;
		else lpRect->right = lpRect->left + MWS_MIN_CX;
	}

	if((lpRect->bottom - lpRect->top) < MWS_MIN_CY) // Minimum height
	{
		if((nSide == WMSZ_TOPLEFT) || (nSide == WMSZ_TOP) || (nSide == WMSZ_TOPRIGHT))
			lpRect->top = lpRect->bottom - MWS_MIN_CY;
		else lpRect->bottom = lpRect->top + MWS_MIN_CY;
	}

	CDialog::OnSizing(nSide, lpRect);

	ProcessResize();
}

void CPwSafeDlg::ProcessResize()
{
	NotifyUserActivity();

	RECT rectClient;
	RECT rectList;
	int cyMenu = GetSystemMetrics(SM_CYMENU);
	LONG nAddTop;
	RECT rectTb;
	int tbHeight = NewGUI_Scale(23, this);
	const long lGuiSpacer = NewGUI_Scale(GUI_SPACER, this);

	if(IsWindow(m_btnTbAbout.m_hWnd))
	{
		if(m_bShowToolBar == TRUE)
		{
			m_btnTbAbout.GetWindowRect(&rectTb);
			tbHeight = rectTb.bottom - rectTb.top;
		}
	}

	if(m_bShowToolBar == TRUE) nAddTop = tbHeight;
	else nAddTop = 0;

	GetClientRect(&rectClient);

	RECT rectWindow;
	GetWindowRect(&rectWindow);
	BOOL bWindowValid = TRUE;
	if((rectWindow.right - rectWindow.left) < (MWS_MIN_CX - 32)) bWindowValid = FALSE;
	if((rectWindow.bottom - rectWindow.top) < (MWS_MIN_CY - 32)) bWindowValid = FALSE;

	if((m_bMinimized == FALSE) && (m_bTrayed == FALSE) && (bWindowValid == TRUE))
	{
		// Range check and correction for splitter windows

		if(m_lSplitterPosHoriz <= 1) m_lSplitterPosHoriz = 1;
		if(m_lSplitterPosHoriz >= rectClient.right - 10) m_lSplitterPosHoriz = rectClient.right - 10;

		if(m_lSplitterPosVert <= 26) m_lSplitterPosVert = 26;

		if((rectClient.bottom - m_lSplitterPosVert) <= (76 + nAddTop))
			m_lSplitterPosVert = rectClient.bottom - (76 + nAddTop);
	}

	LONG nEntryViewHeight = /* rectClient.bottom - rectClient.top - */ m_lSplitterPosVert;
	if(m_bEntryView == FALSE) nEntryViewHeight = -1;

	if(IsWindow(m_cGroups.m_hWnd)) // Resize group box
	{
		rectList.top = lGuiSpacer + nAddTop;
		rectList.bottom = rectClient.bottom - (lGuiSpacer >> 1) - cyMenu - nEntryViewHeight - 1;
		rectList.left = 0;
		rectList.right = lGuiSpacer + m_lSplitterPosHoriz - 1;
		m_cGroups.MoveWindow(&rectList, TRUE);

		// int nColumnWidth = (rectList.right - rectList.left) -
		//	GetSystemMetrics(SM_CXVSCROLL) - 8;
		// m_cGroups.SetColumnWidth(0, nColumnWidth);
	}

	if(IsWindow(m_cList.m_hWnd)) // Resize password list view
	{
		rectList.top = lGuiSpacer + nAddTop;
		rectList.bottom = rectClient.bottom - (lGuiSpacer >> 1) - cyMenu - nEntryViewHeight - 1;
		rectList.left = lGuiSpacer + m_lSplitterPosHoriz + 2;
		rectList.right = rectClient.right;
		m_cList.MoveWindow(&rectList, TRUE);

		if(m_bColAutoSize == TRUE) _CalcColumnSizes();
		_SetColumnWidths();
	}

	if(IsWindow(m_reEntryView.m_hWnd))
	{
		rectList.top = rectClient.bottom - (lGuiSpacer >> 1) - cyMenu - nEntryViewHeight + (lGuiSpacer >> 1);
		rectList.bottom = rectClient.bottom - (lGuiSpacer >> 1) - cyMenu;
		rectList.left = 0;
		rectList.right = rectClient.right - rectClient.left;
		m_reEntryView.MoveWindow(&rectList, TRUE);
	}

	if(IsWindow(m_stcMenuLine.m_hWnd)) // Resize menu line
	{
		rectList.top = 0;
		rectList.bottom = 2;
		rectList.left = 0;
		rectList.right = rectClient.right - rectClient.left;
		m_stcMenuLine.MoveWindow(&rectList, TRUE);
	}

	if(IsWindow(m_sbStatus.m_hWnd)) // Resize status bar
	{
		RECT rectSB;
		m_sbStatus.GetClientRect(&rectSB);
		rectSB.top = rectClient.bottom - rectClient.top - rectSB.bottom - rectSB.top;
		rectSB.bottom = rectClient.bottom - rectClient.top;
		rectSB.left = 0;
		rectSB.right = rectClient.right - rectClient.left;
		m_sbStatus.MoveWindow(&rectSB, TRUE);
		m_sbStatus.RedrawWindow(NULL);
	}
}

void CPwSafeDlg::CleanUp()
{
	SaveOptions();
	_CallPlugins(KPM_CLEANUP, 0, 0);

	m_sessionNotify.Unregister();
	m_remoteControl.FreeStatic();

	if(m_bTimer == TRUE)
	{
		KillTimer(APPWND_TIMER_ID);
		m_bTimer = FALSE;
	}

	if(m_dwATHotKey != 0) UnregisterHotKey(this->m_hWnd, HOTKEYID_AUTOTYPE);

	RegisterRestoreHotKey(FALSE);

	CPluginManager::Instance().UnloadAllPlugins(TRUE);

	if(m_bMenu == TRUE)
	{
		// Auto-destroyed in BCMenu destructor
		// m_menu.DestroyMenu();

		m_bMenu = FALSE;
	}
	DeleteContextMenus();

	if(m_nClipboardCountdown >= 0)
	{
		m_nClipboardCountdown = -1; // Disable clipboard clear countdown

		if(m_bClearClipOnDbClose == TRUE) ClearClipboardIfOwner();
	}

	_DeleteTemporaryFiles();
	FreeCurrentTranslationTable();

	HICON hReleaser; // Load/get all icons and release them safely
	hReleaser = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ICONPIC));
	DestroyIcon(hReleaser);
	hReleaser = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_KEY));
	DestroyIcon(hReleaser);
	hReleaser = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_OPTIONS));
	DestroyIcon(hReleaser);
	hReleaser = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_SEARCH));
	DestroyIcon(hReleaser);
	hReleaser = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_WORLD));
	DestroyIcon(hReleaser);
	hReleaser = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	DestroyIcon(hReleaser);

	VERIFY(DestroyAcceleratorTable(m_hAccel));

	m_mgr.NewDatabase();
	m_cList.DeleteAllItemsEx();
	m_cGroups.DeleteAllItemsEx();

	m_ilIcons.DeleteImageList();

	NewGUI_SetThemeHelper(NULL);
	SAFE_DELETE(m_pThemeHelper);

	if(m_bRestartApplication == TRUE) RestartApplication();
}

void CPwSafeDlg::SaveOptions()
{
	CPrivateConfigEx pcfg(TRUE);
	TCHAR szTemp[SI_REGSIZE];
	CString strTemp;

	// Save clipboard auto-clear time
	_ultot_s(m_dwClipboardSecs, szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_CLIPSECS, szTemp);

	_itot_s(m_nClipboardMethod, szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_CLIPBOARDMETHOD, szTemp);

	_ultot_s(m_dwATHotKey, szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_AUTOTYPEHOTKEY, szTemp);

	pcfg.SetBool(PWMKEY_RESTOREHOTKEY, m_bRegisterRestoreHotKey);

	// Save newline sequence
	if(m_bWindowsNewLine == TRUE) _tcscpy_s(szTemp, _countof(szTemp), _T("Windows"));
	else _tcscpy_s(szTemp, _countof(szTemp), _T("Unix"));
	pcfg.Set(PWMKEY_NEWLINE, szTemp);

	pcfg.SetBool(PWMKEY_USEPUTTYFORURLS, m_bUsePuttyForURLs);

	_itot_s(m_nAutoSort, szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_AUTOSORT, szTemp);

	pcfg.SetBool(PWMKEY_SAVEONLATMOD, m_bSaveOnLATMod);
	pcfg.SetBool(PWMKEY_OPENLASTB, m_bOpenLastDb);
	pcfg.SetBool(PWMKEY_AUTOSAVEB, m_bAutoSaveDb);
	pcfg.SetBool(PWMKEY_REMEMBERLAST, m_bRememberLast);
	pcfg.SetBool(PWMKEY_SINGLEINSTANCE, m_bCheckForInstance);

	if(m_bRememberLast == TRUE)
	{
		// GetCurrentDirectory(1024, szTemp);
		// if(szTemp[0] != 0) pcfg.Set(PWMKEY_LASTDIR, szTemp);
		const std_string strCurDir = WU_GetCurrentDirectory();
		if(strCurDir.size() > 0) pcfg.Set(PWMKEY_LASTDIR, strCurDir.c_str());

		// TCHAR tszTemp[SI_REGSIZE];
		// GetModuleFileName(NULL, tszTemp, SI_REGSIZE - 2);
		const std_string strTemp = Executable::instance().getFullPathName();
		pcfg.Set(PWMKEY_LASTDB, MakeRelativePathEx(strTemp.c_str(), m_strLastDb));
	}
	else
	{
		pcfg.Set(PWMKEY_LASTDIR, _T(""));
		pcfg.Set(PWMKEY_LASTDB, _T(""));
	}

	pcfg.SetBool(PWMKEY_REMEMBERKEYSOURCES, CKeySourcesPool::GetEnabled());
	CKeySourcesPool::Save(&pcfg);

	pcfg.SetBool(PWMKEY_STARTMINIMIZED, m_bStartMinimized);
	pcfg.SetBool(PWMKEY_DISABLEUNSAFE, m_bDisableUnsafe);
	pcfg.SetBool(PWMKEY_IMGBTNS, m_bImgButtons);
	pcfg.SetBool(PWMKEY_ENTRYGRID, m_bEntryGrid);
	pcfg.SetBool(PWMKEY_AUTOSHOWEXPIRED, m_bAutoShowExpired);
	pcfg.SetBool(PWMKEY_AUTOSHOWEXPIREDS, m_bAutoShowExpiredSoon);
	pcfg.SetBool(PWMKEY_BACKUPENTRIES, m_bBackupEntries);
	pcfg.SetBool(PWMKEY_SECUREEDITS, m_bSecureEdits);
	pcfg.SetBool(PWMKEY_SINGLECLICKTRAY, m_bSingleClickTrayIcon);
	pcfg.SetBool(PWMKEY_QUICKFINDINCBK, m_bQuickFindIncBackup);
	pcfg.SetBool(PWMKEY_QUICKFINDINCEXP, m_bQuickFindIncExpired);
	pcfg.SetBool(PWMKEY_DELETEBKONSAVE, m_bDeleteBackupsOnSave);
	pcfg.SetBool(PWMKEY_DISABLEAUTOTYPE, m_bDisableAutoType);
	pcfg.SetBool(PWMKEY_COPYURLS, m_bCopyURLs);
	pcfg.SetBool(PWMKEY_EXITINSTEADLOCK, m_bExitInsteadOfLockAT);
	pcfg.SetBool(PWMKEY_FOCUSRESAFTERQUICKFIND, m_bFocusResAfterQuickFind);
	pcfg.SetBool(PWMKEY_ALWAYSALLOWIPC, m_remoteControl.GetAlwaysAllowFullAccess());
	pcfg.SetBool(PWMKEY_DROPTOBACKONCOPY, m_bDropToBackOnCopy);
	pcfg.SetBool(PWMKEY_SORTAUTOTYPESELITEMS, m_bSortAutoTypeSelItems);
	pcfg.SetBool(PWMKEY_TRANSACTEDFILEWRITE, m_bUseTransactedFileWrites);

	pcfg.SetBool(PWMKEY_SHOWTITLE, m_bShowTitle);
	pcfg.SetBool(PWMKEY_SHOWUSER, m_bShowUserName);
	pcfg.SetBool(PWMKEY_SHOWURL, m_bShowURL);
	pcfg.SetBool(PWMKEY_SHOWPASS, m_bShowPassword);
	pcfg.SetBool(PWMKEY_SHOWNOTES, m_bShowNotes);

	pcfg.SetBool(PWMKEY_SHOWCREATION, m_bShowCreation);
	pcfg.SetBool(PWMKEY_SHOWLASTMOD, m_bShowLastMod);
	pcfg.SetBool(PWMKEY_SHOWLASTACCESS, m_bShowLastAccess);
	pcfg.SetBool(PWMKEY_SHOWEXPIRE, m_bShowExpire);
	pcfg.SetBool(PWMKEY_SHOWUUID, m_bShowUUID);
	pcfg.SetBool(PWMKEY_SHOWATTACH, m_bShowAttach);

	pcfg.SetBool(PWMKEY_USELOCALTIMEFMT, CPwSafeDlg::m_bUseLocalTimeFormat);

	pcfg.SetBool(PWMKEY_ENTRYVIEW, m_bEntryView);

	pcfg.SetBool(PWMKEY_HIDESTARS, m_bPasswordStars);
	pcfg.SetBool(PWMKEY_HIDEUSERS, m_bUserStars);
	pcfg.SetBool(PWMKEY_ALWAYSTOP, m_bAlwaysOnTop);
	pcfg.SetBool(PWMKEY_LOCKMIN, m_bLockOnMinimize);
	pcfg.SetBool(PWMKEY_MINLOCK, m_bMinimizeOnLock);
	pcfg.SetBool(PWMKEY_MINTRAY, m_bMinimizeToTray);
	pcfg.SetBool(PWMKEY_CLOSEMIN, m_bCloseMinimizes);
	pcfg.SetBool(PWMKEY_SHOWTOOLBAR, m_bShowToolBar);
	pcfg.SetBool(PWMKEY_COLAUTOSIZE, m_bColAutoSize);
	pcfg.SetBool(PWMKEY_SHOWFULLPATH, m_bShowFullPath);
	pcfg.SetBool(PWMKEY_DELETETANSAFTERUSE, m_bDeleteTANsAfterUse);

	pcfg.SetBool(PWMKEY_SIMPLETANVIEW, m_bSimpleTANView);
	pcfg.SetBool(PWMKEY_SHOWTANINDICES, m_bShowTANIndices);
	pcfg.SetBool(PWMKEY_ALLOWSAVEIFMODIFIEDONLY, m_bAllowSaveIfModifiedOnly);
	pcfg.SetBool(PWMKEY_CHECKFORUPDATE, m_bCheckForUpdate);
	pcfg.SetBool(PWMKEY_LOCKONWINLOCK, m_bLockOnWinLock);
	pcfg.SetBool(PWMKEY_CLEARCLIPONDBCLOSE, m_bClearClipOnDbClose);
	pcfg.SetBool(PWMKEY_ENABLEREMOTECTRL, m_remoteControl.IsEnabled());

	pcfg.SetBool(PWMKEY_USEHELPCENTER, (WU_GetAppHelpSource() == APPHS_ONLINE) ?
		TRUE : FALSE);

	NewGUI_GetHeaderOrder(m_cList.m_hWnd, m_aHeaderOrder, 11);
	ar2str(szTemp, m_aHeaderOrder, 11);
	pcfg.Set(PWMKEY_HEADERORDER, szTemp);

	/* CString strOptions, strCharSet; UINT nChars;
	CPwGeneratorDlg::GetOptions(&strOptions, &strCharSet, &nChars);
	pcfg.Set(PWMKEY_PWGEN_OPTIONS, strOptions);
	pcfg.Set(PWMKEY_PWGEN_CHARS, strCharSet);
	_itot_s((int)nChars, szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_PWGEN_NUMCHARS, szTemp); */

	_itot_s(m_nAutoTypeMethod, szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_AUTOTYPEMETHOD, szTemp);

	pcfg.Set(PWMKEY_LISTFONT, m_strFontSpec);

	_SaveWindowPositionAndSize(&pcfg);

	// Save all column widths
	_itot_s(m_nColumnWidths[0], szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_COLWIDTH0, szTemp);
	_itot_s(m_nColumnWidths[1], szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_COLWIDTH1, szTemp);
	_itot_s(m_nColumnWidths[2], szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_COLWIDTH2, szTemp);
	_itot_s(m_nColumnWidths[3], szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_COLWIDTH3, szTemp);
	_itot_s(m_nColumnWidths[4], szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_COLWIDTH4, szTemp);
	_itot_s(m_nColumnWidths[5], szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_COLWIDTH5, szTemp);
	_itot_s(m_nColumnWidths[6], szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_COLWIDTH6, szTemp);
	_itot_s(m_nColumnWidths[7], szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_COLWIDTH7, szTemp);
	_itot_s(m_nColumnWidths[8], szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_COLWIDTH8, szTemp);
	_itot_s(m_nColumnWidths[9], szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_COLWIDTH9, szTemp);
	_itot_s(m_nColumnWidths[10], szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_COLWIDTH10, szTemp);

	_ltot_s(m_lSplitterPosHoriz, szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_SPLITTERX, szTemp);
	_ltot_s(m_lSplitterPosVert, szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_SPLITTERY, szTemp);

	pcfg.SetBool(PWMKEY_WINSTATE_MAX, m_bWasMaximized);

	_ltot_s((long)m_cList.GetRowColorEx(), szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_ROWCOLOR, szTemp);

	_ltot_s(m_nLockTimeDef, szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_LOCKTIMER, szTemp);

	_ltot_s((long)m_dwDefaultExpire, szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_DEFAULTEXPIRE, szTemp);

	// int j = 0;
	// CString strT;
	// for(int i = 0; i < (int)CPluginManager::Instance().m_plugins.size(); ++i)
	// {
	//	if(CPluginManager::Instance().m_plugins[i].bEnabled == FALSE) continue;
	//	strTemp.Format(_T("KeePlugin_%d"), j);
	//	strT = CPluginManager::Instance().m_plugins[i].tszFile;
	//	pcfg.Set(strTemp, CsFileOnly(&strT));
	//	++j;
	// }
	// strTemp.Format(_T("KeePlugin_%d"), j);
	// VERIFY(pcfg.Set(strTemp, _T("0")));

	std::basic_string<TCHAR> strAutoProfile =
		PwgProfileToString(&CPwSafeDlg::m_pgsAutoProfile);
	pcfg.Set(PWMKEY_GENPROFILEAUTO, strAutoProfile.c_str());

	// Support legacy auto-generation flag
	pcfg.SetBool(PWMKEY_AUTOPWGEN, (CPwSafeDlg::m_pgsAutoProfile.dwLength > 0) ?
		TRUE : FALSE);

	if(m_strDefaultAutoTypeSequence.GetLength() > 0)
		pcfg.Set(PWMKEY_DEFAULTATSEQ, m_strDefaultAutoTypeSequence);
	pcfg.SetBool(PWMKEY_AUTOTYPEIEFIX, m_bAutoTypeIEFix);

	pcfg.SetBool(PWMKEY_USEDPAPIFORMEMPROT, *CMemoryProtectionEx::GetEnabledPtr());
	pcfg.SetBool(PWMKEY_USECNGBCRYPTFORKEYT, *CKeyTransformBCrypt::GetEnabledPtr());
}

void CPwSafeDlg::_SaveWindowPositionAndSize(CPrivateConfigEx* pConfig)
{
	RECT rect;
	GetWindowRect(&rect);

	if((m_bMinimized == FALSE) && (m_bTrayed == FALSE) && (m_bWasMaximized == FALSE))
	{
		m_lNormalWndPosX = rect.left;
		m_lNormalWndPosY = rect.top;
		m_lNormalWndSizeW = rect.right - rect.left;
		m_lNormalWndSizeH = rect.bottom - rect.top;
	}

	if((pConfig != NULL) && (m_lNormalWndSizeW >= 0) && (m_lNormalWndSizeH >= 0))
	{
		TCHAR szTemp[SI_REGSIZE];
		_ltot_s(m_lNormalWndPosX, szTemp, _countof(szTemp) - 1, 10);
		pConfig->Set(PWMKEY_WINDOWPX, szTemp);
		_ltot_s(m_lNormalWndPosY, szTemp, _countof(szTemp) - 1, 10);
		pConfig->Set(PWMKEY_WINDOWPY, szTemp);
		_ltot_s(m_lNormalWndSizeW, szTemp, _countof(szTemp) - 1, 10);
		pConfig->Set(PWMKEY_WINDOWDX, szTemp);
		_ltot_s(m_lNormalWndSizeH, szTemp, _countof(szTemp) - 1, 10);
		pConfig->Set(PWMKEY_WINDOWDY, szTemp);
	}
}

void CPwSafeDlg::_DeleteTemporaryFiles()
{
	if(_CallPlugins(KPM_DELETE_TEMP_FILES_PRE, 0, 0) == FALSE) return;

	std::vector<std::basic_string<TCHAR> > vRemaining;
	for(DWORD i = 0; i < m_vTempFiles.size(); ++i)
	{
		if(AU_SecureDeleteFile(m_vTempFiles[i].c_str()) == FALSE)
			vRemaining.push_back(m_vTempFiles[i]);
	}

	m_vTempFiles = vRemaining;
}

void CPwSafeDlg::OnOK()
{
	NotifyUserActivity();

	// Ignore enter in dialog

	// CleanUp();
	// CDialog::OnOK();
}

void CPwSafeDlg::OnCancel()
{
	NotifyUserActivity();

	if(m_hDraggingGroup != NULL)
	{
		SendMessage(WM_CANCELMODE);
		return;
	}

	// Are we called because of an Esc key-press?
	if((GetKeyState(VK_ESCAPE) & 0x8000) != 0)
	{
		if(m_bLocked == FALSE) OnFileLock();
		return;
	}

	if((m_bMenuExit == FALSE) && (m_bCloseMinimizes == TRUE)) return;

	if(_CallPlugins(KPM_WM_CANCEL, 0, 0) == FALSE) return;

	m_bExiting = TRUE;
	OnFileClose();

	if(m_bFileOpen == TRUE)
	{
		m_bExiting = FALSE;
		return;
	}

	CleanUp();
	CDialog::OnCancel();
}

void CPwSafeDlg::OnFileExit()
{
	NotifyUserActivity();

	if(_CallPlugins(KPM_FILE_EXIT_PRE, 0, 0) == FALSE) return;

	m_bMenuExit = TRUE;
	OnCancel();
	m_bMenuExit = FALSE;
}

void CPwSafeDlg::OnInfoAbout()
{
	NotifyUserActivity();
	m_bDisplayDialog = TRUE;

	if(_CallPlugins(KPM_INFO_ABOUT_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	CAboutDlg dlg;
	dlg.DoModal();

	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnSafeAddGroup()
{
	NotifyUserActivity();

	CString strGroupName;
	CAddGroupDlg dlg;
	PW_GROUP pwTemplate;
	PW_TIME pwTime;

	if(m_bFileOpen == FALSE) return;

	m_bDisplayDialog = TRUE;

	dlg.m_nIconId = PWM_STD_ICON_GROUP;
	dlg.m_strGroupName.Empty();
	dlg.m_pParentImageList = &m_ilIcons;
	dlg.m_bEditMode = FALSE;

	if(_CallPlugins(KPM_GROUP_ADD_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	if(dlg.DoModal() == IDOK)
	{
		_GetCurrentPwTime(&pwTime);
		pwTemplate.pszGroupName = const_cast<LPTSTR>((LPCTSTR)dlg.m_strGroupName);
		pwTemplate.tCreation = pwTime;
		m_mgr.GetNeverExpireTime(&pwTemplate.tExpire);
		pwTemplate.tLastAccess = pwTime;
		pwTemplate.tLastMod = pwTime;
		pwTemplate.uGroupId = 0; // 0 = create new group
		pwTemplate.uImageId = (DWORD)dlg.m_nIconId;
		pwTemplate.usLevel = 0; pwTemplate.dwFlags = 0;

		if(_CallPlugins(KPM_GROUP_ADD, (LPARAM)&pwTemplate, 0) == FALSE)
			{ m_bDisplayDialog = FALSE; return; }

		VERIFY(m_mgr.AddGroup(&pwTemplate));
		UpdateGroupList();

		HTREEITEM hLast = _GetLastGroupItem(&m_cGroups);
		ASSERT(hLast != NULL); // We have added a group, there must be at least one
		m_cGroups.EnsureVisible(hLast);
		m_cGroups.SelectItem(hLast);

		UpdatePasswordList();
		m_cList.SetFocus();

		dlg.m_strGroupName.Empty();

		m_bModified = TRUE;
	}

	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnViewHideStars()
{
	NotifyUserActivity();

	if(_CallPlugins(KPM_VIEW_HIDE_STARS_PRE, 0, 0) == FALSE) return;

	if((m_bPasswordStars == TRUE) && (IsUnsafeAllowed(this->m_hWnd) == FALSE)) return;

	UINT uState = m_menu.GetMenuState(ID_VIEW_HIDESTARS, MF_BYCOMMAND);
	ASSERT(uState != 0xFFFFFFFF);

	if((uState & MF_CHECKED) != 0)
	{
		uState = MF_UNCHECKED; // Toggle
		m_bPasswordStars = FALSE;
	}
	else
	{
		uState = MF_CHECKED; // Toggle
		m_bPasswordStars = TRUE;
	}

	m_menu.CheckMenuItem(ID_VIEW_HIDESTARS, MF_BYCOMMAND | uState);

	if(m_bPasswordStars == TRUE)
	{
		LV_ITEM lvi;
		ZeroMemory(&lvi, sizeof(LV_ITEM));
		lvi.mask = LVIF_TEXT;
		lvi.iSubItem = 3;
		lvi.pszText = PWM_PASSWORD_STRING;

		const int nItemCount = m_cList.GetItemCount();
		for(int nItem = 0; nItem < nItemCount; ++nItem)
		{
			lvi.iItem = nItem;
			m_cList.SetItem(&lvi);
		}
	}
	else RefreshPasswordList(); // Refresh list based on UUIDs

	// m_bCachedToolBarUpdate = TRUE;
	_UpdateToolBar(TRUE);
}

DWORD CPwSafeDlg::GetSelectedEntry()
{
	NotifyUserActivity();

	if(m_bFileOpen == FALSE) return DWORD_MAX;

	// LVIS_FOCUSED is not enough here, it must be LVIS_SELECTED
	POSITION posFirstItem = m_cList.GetFirstSelectedItemPosition();
	if(posFirstItem == NULL) return DWORD_MAX;

	return static_cast<DWORD>(m_cList.GetNextSelectedItem(posFirstItem));
}

DWORD CPwSafeDlg::GetSelectedEntriesCount()
{
	NotifyUserActivity();

	// DWORD i, uSelectedItems = 0;
	// UINT uState;

	if(m_bFileOpen == FALSE) return DWORD_MAX;

	// for(i = 0; i < (DWORD)m_cList.GetItemCount(); i++)
	// {
	//	uState = m_cList.GetItemState((int)i, LVIS_SELECTED);
	//	if((uState & LVIS_SELECTED) != 0) uSelectedItems++;
	// }
	// return uSelectedItems;

	return static_cast<DWORD>(m_cList.GetSelectedCount());
}

DWORD CPwSafeDlg::GetSelectedGroupId()
{
	HTREEITEM h = m_cGroups.GetSelectedItem();
	if(h == NULL) return DWORD_MAX;

	return static_cast<DWORD>(m_cGroups.GetItemData(h));
}

void CPwSafeDlg::UpdateGroupList()
{
	NotifyUserActivity();

	HTREEITEM hParent = TVI_ROOT;
	HTREEITEM hLastItem = TVI_ROOT;
	DWORD usLevel = 0;

	const bool bLockedRedraw = m_cGroups.LockRedrawEx(true);
	GroupSyncStates(TRUE); // Synchronize 'expanded'-flag from GUI to list manager
	m_cGroups.SelectItem(NULL);
	m_cGroups.DeleteAllItemsEx();

	if(m_bFileOpen == FALSE) { if(bLockedRedraw) m_cGroups.LockRedrawEx(false); return; }

	TVINSERTSTRUCT tvis;
	ZeroMemory(&tvis, sizeof(TVINSERTSTRUCT));
	tvis.hInsertAfter = TVI_LAST;
	tvis.item.mask = (TVIF_IMAGE | TVIF_TEXT | TVIF_PARAM | TVIF_STATE | TVIF_SELECTEDIMAGE);

	DWORD i;
	for(i = 0; i < m_mgr.GetNumberOfGroups(); ++i)
	{
		PW_GROUP *pgrp = m_mgr.GetGroup(i);
		ASSERT(pgrp != NULL); if(pgrp == NULL) continue;
		ASSERT(pgrp->pszGroupName != NULL);

		while(true)
		{
			if(usLevel == pgrp->usLevel) break;
			else if(usLevel == static_cast<USHORT>(pgrp->usLevel - 1))
			{
				hParent = hLastItem;
				++usLevel;
			}
			else if(usLevel < pgrp->usLevel) { ASSERT(FALSE); hParent = TVI_ROOT; break; }
			else if(usLevel > pgrp->usLevel)
			{
				if(hParent == TVI_ROOT) break;

				hParent = m_cGroups.GetParentItem(hParent);
				--usLevel;
			}
		}

		tvis.hParent = hParent;

		tvis.item.pszText = pgrp->pszGroupName;
		tvis.item.iSelectedImage = tvis.item.iImage = static_cast<int>(pgrp->uImageId);
		tvis.item.lParam = pgrp->uGroupId;
		tvis.item.stateMask = TVIS_EXPANDED;
		tvis.item.state = (((pgrp->dwFlags & PWGF_EXPANDED) != 0) ? TVIS_EXPANDED : 0);

		hLastItem = m_cGroups.InsertItem(&tvis);
	}

	if(bLockedRedraw) m_cGroups.LockRedrawEx(false, true);

	GroupSyncStates(FALSE); // Expand all tree items that were open before
	if(i > 0) m_cGroups.SelectItem(m_cGroups.GetRootItem());
}

void CPwSafeDlg::UpdatePasswordList()
{
	NotifyUserActivity();

	if(m_bFileOpen == FALSE) { m_cList.DeleteAllItemsEx(); return; }

	const DWORD dwGroupId = GetSelectedGroupId();
	if(dwGroupId == DWORD_MAX) { m_cList.DeleteAllItemsEx(); return; }

	if(m_bBlockPwListUpdate == TRUE) return;
	m_bBlockPwListUpdate = TRUE;
	_SortListIfAutoSort();
	m_bBlockPwListUpdate = FALSE;

	const bool bLockedRedraw = m_cList.LockRedrawEx(true);

	m_cList.DeleteAllItemsEx();
	m_bTANsOnly = TRUE;

	_UpdateCachedGroupIDs();

	PW_TIME tNow;
	_GetCurrentPwTime(&tNow);

	DWORD j = 0;
	for(DWORD i = 0; i < m_mgr.GetNumberOfEntries(); ++i)
	{
		PW_ENTRY *pwe = m_mgr.GetEntry(i);
		ASSERT_ENTRY(pwe);

		if(pwe != NULL)
		{
			if(pwe->uGroupId == dwGroupId)
			{
				_List_SetEntry(j, pwe, TRUE, &tNow);
				++j;
			}
		}
	}

	if(j == 0) m_bTANsOnly = FALSE; // Use report list view
	AdjustPwListMode();
	if(j == 0) m_bTANsOnly = TRUE; // Now set it to the correct value

	AdjustColumnWidths();

	if(bLockedRedraw) m_cList.LockRedrawEx(false, true);

	ShowEntryDetails(NULL);
}

void CPwSafeDlg::AdjustPwListMode()
{
	const DWORD dwStyle = m_cList.GetStyle();

	// Don't use SetRedraw and Invalidate in this function!

	if((m_bTANsOnly == TRUE) && (m_bSimpleTANView == TRUE))
	{
		if((dwStyle & LVS_REPORT) != 0) // Is in report display mode
		{
			m_dwPwListMode = LVSX_CHANGING;

			m_cList.ModifyStyle(LVS_REPORT, LVS_SMALLICON);

			m_dwPwListMode = LVS_SMALLICON;
			_SetColumnWidths();
			return;
		}
	}
	else // m_bTANsOnly == FALSE
	{
		if((dwStyle & LVS_LIST) != 0)
		{
			m_dwPwListMode = LVSX_CHANGING;

			m_cList.ModifyStyle(LVS_SMALLICON, LVS_REPORT);

			m_dwPwListMode = LVS_REPORT;
			_SetColumnWidths();
			return;
		}
	}
}

void CPwSafeDlg::AdjustColumnWidths()
{
	ASSERT(m_dwPwListMode != LVSX_CHANGING);
	if(m_dwPwListMode != LVS_SMALLICON) return;

	const int nItemCount = m_cList.GetItemCount();
	int nMaxWidth = 0;
	for(int i = 0; i < nItemCount; ++i)
	{
		const int n = m_cList.GetStringWidth(m_cList.GetItemText(i, 0));
		if(n > nMaxWidth) nMaxWidth = n;
	}

	nMaxWidth += 26;
	if(m_cList.GetColumnWidth(-1) != nMaxWidth)
		m_cList.SetColumnWidth(-1, nMaxWidth);
}

void CPwSafeDlg::_List_SetEntry(DWORD dwInsertPos, PW_ENTRY *pwe, BOOL bIsNewEntry, PW_TIME *ptNow)
{
	if((dwInsertPos == DWORD_MAX) && (bIsNewEntry == TRUE))
		dwInsertPos = static_cast<DWORD>(m_cList.GetItemCount());

	const BOOL bIsTAN = CPwUtil::IsTANEntry(pwe);
	BOOL bValidTANIndex = TRUE;

	if(bIsTAN == FALSE) m_bTANsOnly = FALSE;

	LV_ITEM lvi;
	ZeroMemory(&lvi, sizeof(LV_ITEM));
	lvi.iItem = static_cast<int>(dwInsertPos);
	lvi.iSubItem = 0;

	// Set 'expired' image if necessary
	DWORD uImageId;
	if(_pwtimecmp(&pwe->tExpire, ptNow) <= 0) uImageId = 45;
	else uImageId = pwe->uImageId;

	CString strTemp;

	lvi.mask = (LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM);
	if((m_bShowTitle == TRUE) || (bIsTAN == TRUE))
	{
		if((bIsTAN == FALSE) || (m_bShowTANIndices == FALSE))
			lvi.pszText = pwe->pszTitle;
		else
		{
			strTemp = pwe->pszTitle;

			if(pwe->pszUserName[0] != 0)
			{
				for(DWORD i = 0; ; ++i)
				{
					const TCHAR tch = pwe->pszUserName[i];

					if(tch == 0) break;
					else if((tch < _T('0')) || (tch > _T('9')))
					{
						bValidTANIndex = FALSE;
						break;
					}
				}

				if(bValidTANIndex == TRUE)
				{
					strTemp += _T(" (#");
					strTemp += pwe->pszUserName;
					strTemp += _T(")");
				}
			}

			lvi.pszText = const_cast<LPTSTR>((LPCTSTR)strTemp);
		}
	}
	else lvi.pszText = g_pNullString;

	lvi.iImage = uImageId;
	_SetLVItemParam(&lvi, pwe);

	if(bIsNewEntry == TRUE) m_cList.InsertItem(&lvi); // Add
	else m_cList.SetItem(&lvi); // Modify existing

	lvi.mask = LVIF_TEXT;
	lvi.iSubItem = 1;
	if(m_bShowUserName == TRUE)
	{
		// Hide usernames behind "********", if the user has selected this option
		if(m_bUserStars == TRUE)
		{
			lvi.pszText = const_cast<LPTSTR>(PWM_PASSWORD_STRING);
			m_cList.SetItem(&lvi);
		}
		else // Don't hide, display them
		{
			lvi.pszText = pwe->pszUserName;
			m_cList.SetItem(&lvi);
		}
	}
	else
	{
		lvi.pszText = g_pNullString;
		m_cList.SetItem(&lvi);
	}

	lvi.iSubItem = 2;
	if(m_bShowURL == TRUE) lvi.pszText = pwe->pszURL;
	else lvi.pszText = g_pNullString;
	m_cList.SetItem(&lvi);

	lvi.iSubItem = 3;
	if(m_bShowPassword == TRUE)
	{
		// Hide passwords behind "********", if the user has selected this option
		if(m_bPasswordStars == TRUE)
		{
			lvi.pszText = const_cast<LPTSTR>(PWM_PASSWORD_STRING);
			m_cList.SetItem(&lvi);
		}
		else // Don't hide, display them
		{
			m_mgr.UnlockEntryPassword(pwe);
			lvi.pszText = pwe->pszPassword;
			m_cList.SetItem(&lvi);
			m_mgr.LockEntryPassword(pwe);
		}
	}
	else // Hide passwords completely
	{
		lvi.pszText = g_pNullString;
		m_cList.SetItem(&lvi);
	}

	lvi.iSubItem = 4;
	if(m_bShowNotes == TRUE)
	{
		// Remove newline and break character for better display
		strTemp = pwe->pszAdditional;
		const int nNotesLength = strTemp.GetLength();
		for(int t = 0; t < nNotesLength; ++t)
		{
			if((strTemp.GetAt(t) == _T('\r')) || (strTemp.GetAt(t) == _T('\n')))
				strTemp.SetAt(t, _T(' '));
		}
		lvi.pszText = const_cast<LPTSTR>((LPCTSTR)strTemp);
	}
	else lvi.pszText = g_pNullString;
	m_cList.SetItem(&lvi);

	lvi.iSubItem = 5;
	if(m_bShowCreation == TRUE)
	{
		_PwTimeToStringEx(pwe->tCreation, strTemp, CPwSafeDlg::m_bUseLocalTimeFormat);
		lvi.pszText = const_cast<LPTSTR>((LPCTSTR)strTemp);
	}
	else lvi.pszText = g_pNullString;
	m_cList.SetItem(&lvi);

	lvi.iSubItem = 6;
	if(m_bShowLastMod == TRUE)
	{
		_PwTimeToStringEx(pwe->tLastMod, strTemp, CPwSafeDlg::m_bUseLocalTimeFormat);
		lvi.pszText = const_cast<LPTSTR>((LPCTSTR)strTemp);
	}
	else lvi.pszText = g_pNullString;
	m_cList.SetItem(&lvi);

	lvi.iSubItem = 7;
	if(m_bShowLastAccess == TRUE)
	{
		_PwTimeToStringEx(pwe->tLastAccess, strTemp, CPwSafeDlg::m_bUseLocalTimeFormat);
		lvi.pszText = const_cast<LPTSTR>((LPCTSTR)strTemp);
	}
	else lvi.pszText = g_pNullString;
	m_cList.SetItem(&lvi);

	lvi.iSubItem = 8;
	if(m_bShowExpire == TRUE)
	{
		if(memcmp(&pwe->tExpire, &g_tNeverExpire, sizeof(PW_TIME)) == 0)
			lvi.pszText = const_cast<LPTSTR>(g_psztNeverExpires);
		else
		{
			_PwTimeToStringEx(pwe->tExpire, strTemp, CPwSafeDlg::m_bUseLocalTimeFormat);
			lvi.pszText = const_cast<LPTSTR>((LPCTSTR)strTemp);
		}
	}
	else lvi.pszText = g_pNullString;
	m_cList.SetItem(&lvi);

	// Ignore m_bShowUUID, the UUID field is needed in all cases
	lvi.iSubItem = 9;
	_UuidToString(pwe->uuid, &strTemp);
	lvi.pszText = const_cast<LPTSTR>((LPCTSTR)strTemp);
	m_cList.SetItem(&lvi);

	lvi.iSubItem = 10;
	if(m_bShowAttach == TRUE) lvi.pszText = pwe->pszBinaryDesc;
	else lvi.pszText = g_pNullString;
	m_cList.SetItem(&lvi);
}

void CPwSafeDlg::RefreshPasswordList()
{
	NotifyUserActivity();

	TCHAR szTemp[1024];
	BYTE aUuid[16];

	if(m_bFileOpen == FALSE) return;

	PW_TIME tNow;
	_GetCurrentPwTime(&tNow);

	_UpdateCachedGroupIDs();

	const bool bLockedRedraw = m_cList.LockRedrawEx(true);

	const int nItemCount = m_cList.GetItemCount();
	DWORD j = 0;
	for(int i = 0; i < nItemCount; ++i)
	{
		LV_ITEM lvi;
		ZeroMemory(&lvi, sizeof(LV_ITEM));
		lvi.iItem = i;
		lvi.iSubItem = 9;
		lvi.mask = LVIF_TEXT;
		lvi.cchTextMax = 1024;
		lvi.pszText = szTemp;
		m_cList.GetItem(&lvi);

		_StringToUuid(lvi.pszText, aUuid);
		PW_ENTRY *pwe = m_mgr.GetEntryByUuid(aUuid);
		ASSERT_ENTRY(pwe);

		if(pwe != NULL)
		{
			_List_SetEntry(j, pwe, FALSE, &tNow);
			++j;
		}
	}

	_SortListIfAutoSort();
	AdjustPwListMode();
	AdjustColumnWidths();

	if(bLockedRedraw) m_cList.LockRedrawEx(false, true);
}

void CPwSafeDlg::OnPwlistAdd()
{
	NotifyUserActivity();

	DWORD uGroupId = GetSelectedGroupId();
	PW_ENTRY pwTemplate;
	PW_TIME tNow;
	BYTE aUuid[16];

	if(m_bFileOpen == FALSE) return;
	if(uGroupId == DWORD_MAX) return; // No group selected or other error

	m_bDisplayDialog = TRUE;

	CAddEntryDlg dlg;
	dlg.m_pMgr = &m_mgr;
	dlg.m_dwEntryIndex = DWORD_MAX;
	dlg.m_pParentIcons = &m_ilIcons;
	dlg.m_bStars = m_bPasswordStars;
	dlg.m_nGroupId = static_cast<int>(m_mgr.GetGroupByIdN(uGroupId)); // m_nGroupId of the dialog is an index, not an ID
	dlg.m_bEditMode = FALSE;
	dlg.m_dwDefaultExpire = m_dwDefaultExpire;
	dlg.m_strUserName = m_mgr.GetPropertyString(PWP_DEFAULT_USER_NAME).c_str();

	dlg.m_nIconId = 0;
	PW_GROUP *pgContainer = m_mgr.GetGroupById(uGroupId);
	if(pgContainer != NULL)
	{
		if((pgContainer->uImageId != PWM_STD_ICON_GROUP) &&
			(pgContainer->uImageId != PWM_STD_ICON_GROUP_OPEN) &&
			(pgContainer->uImageId != PWM_STD_ICON_GROUP_EMAIL) &&
			(pgContainer->uImageId != PWM_STD_ICON_GROUP_PKG) &&
			(pgContainer->uImageId != DWORD_MAX))
		{
			dlg.m_nIconId = (int)pgContainer->uImageId;
		}
	}

	DWORD dwInitialGroup = uGroupId; // ID

	if(m_dwDefaultExpire == 0) m_mgr.GetNeverExpireTime(&dlg.m_tExpire);
	else
	{
		CTime t = CTime::GetCurrentTime();
		t += CTimeSpan((LONG)m_dwDefaultExpire, 0, 0, 0);

		dlg.m_tExpire.btDay = static_cast<BYTE>(t.GetDay());
		dlg.m_tExpire.btHour = static_cast<BYTE>(t.GetHour());
		dlg.m_tExpire.btMinute = static_cast<BYTE>(t.GetMinute());
		dlg.m_tExpire.btMonth = static_cast<BYTE>(t.GetMonth());
		dlg.m_tExpire.btSecond = static_cast<BYTE>(t.GetSecond());
		dlg.m_tExpire.shYear = static_cast<USHORT>(t.GetYear());
	}

	if(_CallPlugins(KPM_ADD_ENTRY_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	if(dlg.DoModal() == IDOK)
	{
		ASSERT(dlg.m_lpPassword != NULL); if(dlg.m_lpPassword == NULL) { dlg.m_lpPassword = new TCHAR[2]; dlg.m_lpPassword[0] = 0; }
		ASSERT(dlg.m_lpRepeatPw != NULL); if(dlg.m_lpRepeatPw == NULL) { dlg.m_lpRepeatPw = new TCHAR[2]; dlg.m_lpRepeatPw[0] = 0; }

		_GetCurrentPwTime(&tNow);
		memset(&pwTemplate, 0, sizeof(PW_ENTRY));
		pwTemplate.pszAdditional = const_cast<LPTSTR>((LPCTSTR)dlg.m_strNotes);
		pwTemplate.pszPassword = dlg.m_lpPassword;
		pwTemplate.pszTitle = const_cast<LPTSTR>((LPCTSTR)dlg.m_strTitle);
		pwTemplate.pszURL = const_cast<LPTSTR>((LPCTSTR)dlg.m_strURL);
		pwTemplate.pszUserName = const_cast<LPTSTR>((LPCTSTR)dlg.m_strUserName);
		pwTemplate.tCreation = tNow;
		pwTemplate.tExpire = dlg.m_tExpire;
		pwTemplate.tLastAccess = tNow;
		pwTemplate.tLastMod = tNow;
		pwTemplate.uGroupId = m_mgr.GetGroupIdByIndex((DWORD)dlg.m_nGroupId);
		pwTemplate.uImageId = static_cast<DWORD>(dlg.m_nIconId);
		pwTemplate.uPasswordLen = static_cast<DWORD>(_tcslen(pwTemplate.pszPassword));
		pwTemplate.pszBinaryDesc = _T("");

		if(_CallPlugins(KPM_ADD_ENTRY, (LPARAM)&pwTemplate, 0) == FALSE)
			{ m_bDisplayDialog = FALSE; return; }

		// Add the entry to the password manager
		VERIFY(m_mgr.AddEntry(&pwTemplate));

		CSecureEditEx::DeletePassword(dlg.m_lpPassword); dlg.m_lpPassword = NULL;
		CSecureEditEx::DeletePassword(dlg.m_lpRepeatPw); dlg.m_lpRepeatPw = NULL;

		PW_ENTRY *pNew = m_mgr.GetLastEditedEntry();
		memcpy(aUuid, pNew->uuid, 16);

		const int nAttachLen = dlg.m_strAttachment.GetLength();
		const int nEscapeLen = static_cast<int>(_tcslen(PWS_NEW_ATTACHMENT));

		if(nAttachLen > nEscapeLen)
		{
			if(dlg.m_strAttachment.Left(nEscapeLen) == CString(PWS_NEW_ATTACHMENT))
				CPwUtil::AttachFileAsBinaryData(pNew,
					dlg.m_strAttachment.Right(dlg.m_strAttachment.GetLength() -
					nEscapeLen));
		}

		// Add the password to the GUI, but only if it's visible
		if(pNew->uGroupId == dwInitialGroup) // dwInitialGroup is an ID
		{
			_UpdateCachedGroupIDs();
			_List_SetEntry(DWORD_MAX, pNew, TRUE, &tNow); // No unlock needed

			_SortListIfAutoSort();

			AdjustPwListMode();
			AdjustColumnWidths();

			if(m_nAutoSort == 0)
			{
				m_cList.EnsureVisible(m_cList.GetItemCount() - 1, FALSE);
				m_cList.FocusItem(m_cList.GetItemCount() - 1, TRUE);
			}
			else
			{
				const DWORD dw = _EntryUuidToListPos(aUuid); ASSERT(dw != DWORD_MAX);
				if(dw != DWORD_MAX)
				{
					m_cList.EnsureVisible(static_cast<int>(dw), FALSE);
					m_cList.FocusItem(static_cast<int>(dw), TRUE);
				}
			}
		}

		m_bModified = TRUE; // Didn't we? :)
	}

	// Cleanup dialog data
	EraseCString(&dlg.m_strTitle);
	EraseCString(&dlg.m_strUserName);
	EraseCString(&dlg.m_strURL);
	EraseCString(&dlg.m_strNotes);
	EraseCString(&dlg.m_strAttachment);

	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

DWORD CPwSafeDlg::_ListSelToEntryIndex(DWORD dwSelected)
{
	DWORD dwSel;
	if(dwSelected == DWORD_MAX) dwSel = GetSelectedEntry();
	else dwSel = dwSelected;

	if(dwSel == DWORD_MAX) return DWORD_MAX;

	TCHAR tszTemp[40];

	LV_ITEM lvi;
	ZeroMemory(&lvi, sizeof(LV_ITEM));
	lvi.iItem = static_cast<int>(dwSel);
	lvi.iSubItem = 9;
	lvi.mask = LVIF_TEXT;
	lvi.pszText = tszTemp;
	lvi.cchTextMax = 40; // Including terminating NULL
	if(m_cList.GetItem(&lvi) == FALSE) return DWORD_MAX;

	BYTE aUuid[16];
	_StringToUuid(lvi.pszText, aUuid);

	dwSel = m_mgr.GetEntryByUuidN(aUuid);
	ASSERT(dwSel != DWORD_MAX);
	return dwSel;
}

void CPwSafeDlg::OnPwlistEdit()
{
	NotifyUserActivity();

	DWORD dwEntryIndex;
	PW_ENTRY *pEntry;
	DWORD dwNewGroupId;
	BOOL bNeedFullUpdate = FALSE, bNeedGroupUpdate = FALSE;
	int nModifyLevel = 0;
	DWORD dwSelectedEntry = GetSelectedEntry();
	BYTE aUuid[16];
	PW_ENTRY pweBackup;
	BOOL bDoBackup = FALSE;

	if(m_bFileOpen == FALSE) return;
	if(dwSelectedEntry == DWORD_MAX) return;

	dwEntryIndex = _ListSelToEntryIndex(dwSelectedEntry);
	ASSERT(dwEntryIndex != DWORD_MAX); if(dwEntryIndex == DWORD_MAX) return;

	pEntry = m_mgr.GetEntry(dwEntryIndex);
	ASSERT_ENTRY(pEntry); if(pEntry == NULL) return;

	m_bDisplayDialog = TRUE;

	CAddEntryDlg dlg;
	dlg.m_pMgr = &m_mgr;
	dlg.m_pOriginalEntry = pEntry;
	dlg.m_dwEntryIndex = dwEntryIndex;
	dlg.m_pParentIcons = &m_ilIcons;
	dlg.m_bEditMode = TRUE;
	dlg.m_bStars = m_bPasswordStars;
	dlg.m_dwDefaultExpire = m_dwDefaultExpire;

	dlg.m_nGroupId = (int)m_mgr.GetGroupByIdN(pEntry->uGroupId); // ID to index
	dlg.m_strTitle = pEntry->pszTitle;
	dlg.m_strUserName = pEntry->pszUserName;
	dlg.m_strURL = pEntry->pszURL;
	dlg.m_strNotes = pEntry->pszAdditional;
	dlg.m_strAttachment = pEntry->pszBinaryDesc; // Copy binary description
	dlg.m_nIconId = (int)pEntry->uImageId;
	dlg.m_tExpire = pEntry->tExpire; // Copy expiration time
	m_mgr.UnlockEntryPassword(pEntry); // We must unlock the entry, otherwise we cannot access the password
	dlg.m_lpPassword = _TcsCryptDupAlloc(pEntry->pszPassword);
	dlg.m_lpRepeatPw = _TcsCryptDupAlloc(pEntry->pszPassword);
	m_mgr.LockEntryPassword(pEntry);

	if(_CallPlugins(KPM_EDIT_ENTRY_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	if(dlg.DoModal() == IDOK)
	{
		m_mgr.UnlockEntryPassword(pEntry); ASSERT_ENTRY(pEntry);

		if(dlg.m_strTitle != pEntry->pszTitle) nModifyLevel = 2;
		else if(dlg.m_strUserName != pEntry->pszUserName) nModifyLevel = 2;
		else if(dlg.m_strURL != pEntry->pszURL) nModifyLevel = 2;
		else if(dlg.m_strNotes != pEntry->pszAdditional) nModifyLevel = 2;
		else if(dlg.m_strAttachment != pEntry->pszBinaryDesc) nModifyLevel = 2;
		else if(_pwtimecmp(&dlg.m_tExpire, &pEntry->tExpire) != 0) nModifyLevel = 2;
		else if(_tcscmp(dlg.m_lpPassword, pEntry->pszPassword) != 0) nModifyLevel = 2;
		else if(dlg.m_nGroupId != (int)m_mgr.GetGroupByIdN(pEntry->uGroupId)) nModifyLevel = 1;
		else if(dlg.m_nIconId != (int)pEntry->uImageId) nModifyLevel = 1;

		if((nModifyLevel == 2) && (m_bBackupEntries == TRUE))
			bDoBackup = CPwUtil::MemAllocCopyEntry(pEntry, &pweBackup);

		m_mgr.LockEntryPassword(pEntry);

		PW_ENTRY pwTemplate = *pEntry;

		PW_TIME tNow;
		_GetCurrentPwTime(&tNow);

		pwTemplate.pszAdditional = const_cast<LPTSTR>((LPCTSTR)dlg.m_strNotes);
		pwTemplate.pszPassword = dlg.m_lpPassword;
		pwTemplate.pszTitle = const_cast<LPTSTR>((LPCTSTR)dlg.m_strTitle);
		pwTemplate.pszURL = const_cast<LPTSTR>((LPCTSTR)dlg.m_strURL);
		pwTemplate.pszUserName = const_cast<LPTSTR>((LPCTSTR)dlg.m_strUserName);
		// pwTemplate.tCreation = pEntry->tCreation;
		pwTemplate.tExpire = dlg.m_tExpire;
		pwTemplate.tLastAccess = tNow;
		pwTemplate.tLastMod = tNow;
		pwTemplate.uImageId = static_cast<DWORD>(dlg.m_nIconId);
		pwTemplate.uPasswordLen = static_cast<DWORD>(_tcslen(pwTemplate.pszPassword));

		// If the entry has been moved to a different group, a full
		// update of the list is required
		dwNewGroupId = m_mgr.GetGroupIdByIndex(static_cast<DWORD>(dlg.m_nGroupId));
		if(dwNewGroupId != pwTemplate.uGroupId) bNeedFullUpdate = TRUE;
		pwTemplate.uGroupId = dwNewGroupId;

		VERIFY(m_mgr.SetEntry(dwEntryIndex, &pwTemplate));

		CSecureEditEx::DeletePassword(dlg.m_lpPassword); dlg.m_lpPassword = NULL;
		CSecureEditEx::DeletePassword(dlg.m_lpRepeatPw); dlg.m_lpRepeatPw = NULL;

		int nAttachLen = dlg.m_strAttachment.GetLength();
		int nEscapeLen = (int)_tcslen(PWS_NEW_ATTACHMENT);

		if((nAttachLen == nEscapeLen) && (dlg.m_strAttachment == CString(PWS_NEW_ATTACHMENT)))
			CPwUtil::RemoveBinaryData(m_mgr.GetEntry(dwEntryIndex));
		else if(nAttachLen > nEscapeLen)
		{
			if(dlg.m_strAttachment.Left((int)_tcslen(PWS_NEW_ATTACHMENT)) == CString(PWS_NEW_ATTACHMENT))
				CPwUtil::AttachFileAsBinaryData(m_mgr.GetEntry(dwEntryIndex),
					dlg.m_strAttachment.Right(dlg.m_strAttachment.GetLength() - (int)_tcslen(PWS_NEW_ATTACHMENT)));
		}

		PW_ENTRY *pBase = m_mgr.GetEntry(dwEntryIndex); ASSERT_ENTRY(pBase);
		memcpy(aUuid, pBase->uuid, 16);

		if(bDoBackup == TRUE)
		{
			if(m_mgr.BackupEntry(&pweBackup, &bNeedGroupUpdate) == FALSE)
				MessageBox(TRL("Failed to create backup of this entry!"), TRL("An error occurred"), MB_ICONWARNING | MB_OK);

			CPwUtil::MemFreeEntry(&pweBackup);
		}

		if(bNeedGroupUpdate == TRUE)
		{
			_Groups_SaveView(TRUE);
			UpdateGroupList();
			_Groups_RestoreView();
		}

		if(bNeedFullUpdate == TRUE) // Full list update(!) needed
		{
			_List_SaveView();
			UpdatePasswordList(); // Refresh is not enough!
			_List_RestoreView();
		}
		else // Just update the selected item, not the whole list
		{
			_UpdateCachedGroupIDs();

			PW_ENTRY *pUpdated = m_mgr.GetEntry(dwEntryIndex); ASSERT(pUpdated != NULL);
			_List_SetEntry(dwSelectedEntry, pUpdated, FALSE, &tNow);

			AdjustPwListMode();
			AdjustColumnWidths();
		}

		_SortListIfAutoSort();
		const DWORD dwListEntry = _EntryUuidToListPos(aUuid);
		if(dwListEntry != DWORD_MAX)
		{
			m_cList.EnsureVisible(static_cast<int>(dwListEntry), FALSE);
			m_cList.FocusItem(static_cast<int>(dwListEntry), TRUE);
		}

		if(nModifyLevel != 0) m_bModified = TRUE;
	}
	else _TouchEntry(GetSelectedEntry(), FALSE); // User had viewed it only

	// Cleanup dialog data
	EraseCString(&dlg.m_strTitle);
	EraseCString(&dlg.m_strUserName);
	EraseCString(&dlg.m_strURL);
	EraseCString(&dlg.m_strNotes);

	m_ullLastListParams = 0; // Invalidate
	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnPwlistDelete()
{
	NotifyUserActivity();

	if(m_bFileOpen == FALSE) return;
	// if(m_dwLastNumSelectedItems == 0) return;

	m_bDisplayDialog = TRUE;

	if(_CallPlugins(KPM_DELETE_ENTRY_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	CVistaTaskDialog dlgTask(this->m_hWnd, AfxGetInstanceHandle(), false);
	dlgTask.SetContent(TRL("This will remove all selected entries unrecoverably!"));
	dlgTask.SetMainInstruction(TRL("Are you sure you want to delete all selected entries?"));
	dlgTask.SetWindowTitle(PWM_PRODUCT_NAME_SHORT);
	dlgTask.SetIcon(V_MTDI_QUESTION);
	dlgTask.AddButton(TRL("&Delete"), NULL, IDOK);
	dlgTask.AddButton(TRL("&Cancel"), NULL, IDCANCEL);
	int nMsg = dlgTask.ShowDialog();

	if(nMsg < 0) // No task dialog support?
	{
		CString str;
		str = TRL("This will remove all selected entries unrecoverably!");
		str += _T("\r\n\r\n");
		str += TRL("Are you sure you want to delete all selected entries?");
		nMsg = MessageBox(str, TRL("Delete Entries Confirmation"), MB_ICONQUESTION | MB_YESNO);
	}
	if((nMsg == IDCANCEL) || (nMsg == IDNO)) { m_bDisplayDialog = FALSE; return; }

	const DWORD dwInvGroup1 = m_mgr.GetGroupId(PWS_BACKUPGROUP);
	const DWORD dwInvGroup2 = m_mgr.GetGroupId(PWS_BACKUPGROUP_SRC);

	BOOL bNeedGroupUpdate = FALSE;
	while(true)
	{
		const DWORD dwSel = GetSelectedEntry();
		if(dwSel == DWORD_MAX) break;

		const DWORD dwIndex = _ListSelToEntryIndex(dwSel);
		ASSERT(dwIndex != DWORD_MAX); if(dwIndex == DWORD_MAX) break;

		if(m_bBackupEntries != FALSE)
		{
			PW_ENTRY *p = m_mgr.GetEntry(dwIndex);
			ASSERT(p != NULL); if(p == NULL) break;

			if((p->uGroupId != dwInvGroup1) && (p->uGroupId != dwInvGroup2))
			{
				m_mgr.UnlockEntryPassword(p);
				BOOL b = FALSE;
				m_mgr.BackupEntry(p, &b);
				bNeedGroupUpdate |= b;
				m_mgr.LockEntryPassword(p);
			}
		}

		VERIFY(m_mgr.DeleteEntry(dwIndex)); // Delete from password manager
		VERIFY(m_cList.DeleteItem(static_cast<int>(dwSel))); // Delete from GUI
	}

	if(bNeedGroupUpdate == TRUE)
	{
		_Groups_SaveView(TRUE);
		UpdateGroupList();
		_Groups_RestoreView();
	}

	m_cList.RedrawWindow();
	m_bModified = TRUE;

	ShowEntryDetails(NULL);

	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::RebuildContextMenus()
{
	DeleteContextMenus();

	m_pPwListMenu = new BCMenu();
	m_pPwListMenu->LoadMenu(IDR_PWLIST_MENU);

	m_pPwListMenu->SetMenuDrawMode(BCMENU_DRAWMODE_XP); // <<<!=>>> BCMENU_DRAWMODE_ORIGINAL
	m_pPwListMenu->SetSelectDisableMode(FALSE);
	m_pPwListMenu->SetXPBitmap3D(TRUE);
	m_pPwListMenu->SetBitmapBackground(RGB(255, 0, 255));
	m_pPwListMenu->SetIconSize(16, 16);

	m_pPwListMenu->LoadToolbar(IDR_INFOICONS, IDB_INFOICONS_EX);

	SetToMiniModeIfEnabled(m_pPwListMenu, FALSE, FALSE);

	m_pPwListTrackableMenu = NewGUI_GetBCMenu(m_pPwListMenu->GetSubMenu(0));
	_TranslateMenu(m_pPwListTrackableMenu, TRUE, &m_bCopyURLs);

	/////////////////////////////////////////////////////////////////////////

	m_pGroupListMenu = new BCMenu();
	m_pGroupListMenu->LoadMenu(IDR_GROUPLIST_MENU);

	m_pGroupListMenu->SetMenuDrawMode(BCMENU_DRAWMODE_XP); // <<<!=>>> BCMENU_DRAWMODE_ORIGINAL
	m_pGroupListMenu->SetSelectDisableMode(FALSE);
	m_pGroupListMenu->SetXPBitmap3D(TRUE);
	m_pGroupListMenu->SetBitmapBackground(RGB(255, 0, 255));
	m_pGroupListMenu->SetIconSize(16, 16);

	m_pGroupListMenu->LoadToolbar(IDR_INFOICONS, IDB_INFOICONS_EX);

	m_pGroupListTrackableMenu = NewGUI_GetBCMenu(m_pGroupListMenu->GetSubMenu(0));

	if(m_bMiniMode == TRUE)
		m_pGroupListTrackableMenu->DeleteMenu(_T("&Export Group To"));

	_TranslateMenu(m_pGroupListTrackableMenu, TRUE, NULL);

	/////////////////////////////////////////////////////////////////////////

	m_pEntryViewMenu = new BCMenu();
	m_pEntryViewMenu->LoadMenu(IDR_RECTX_MENU);

	m_pEntryViewMenu->SetMenuDrawMode(BCMENU_DRAWMODE_XP); // <<<!=>>> BCMENU_DRAWMODE_ORIGINAL
	m_pEntryViewMenu->SetSelectDisableMode(FALSE);
	m_pEntryViewMenu->SetXPBitmap3D(TRUE);
	m_pEntryViewMenu->SetBitmapBackground(RGB(255, 0, 255));
	m_pEntryViewMenu->SetIconSize(16, 16);

	m_pEntryViewMenu->LoadToolbar(IDR_INFOICONS, IDB_INFOICONS_EX);

	m_pEntryViewTrackableMenu = NewGUI_GetBCMenu(m_pEntryViewMenu->GetSubMenu(0));
	_TranslateMenu(m_pEntryViewTrackableMenu, TRUE, NULL);
}

void CPwSafeDlg::DeleteContextMenus()
{
	if(m_pPwListMenu != NULL)
	{
		m_pPwListMenu->DestroyMenu();
		delete m_pPwListMenu;
		m_pPwListMenu = NULL;
	}
	m_pPwListTrackableMenu = NULL;

	if(m_pGroupListMenu != NULL)
	{
		m_pGroupListMenu->DestroyMenu();
		delete m_pGroupListMenu;
		m_pGroupListMenu = NULL;
	}
	m_pGroupListTrackableMenu = NULL;

	if(m_pEntryViewMenu != NULL)
	{
		m_pEntryViewMenu->DestroyMenu();
		delete m_pEntryViewMenu;
		m_pEntryViewMenu = NULL;
	}
	m_pEntryViewTrackableMenu = NULL;
}

void CPwSafeDlg::OnRclickPwlist(NMHDR* pNMHDR, LRESULT* pResult)
{
	NotifyUserActivity();

	POINT pt;

	UNREFERENCED_PARAMETER(pNMHDR);
	*pResult = 0;

	if(m_bFileOpen == FALSE) return;

	m_bDisplayDialog = TRUE;

	ASSERT(m_pPwListTrackableMenu != NULL);
	if(m_pPwListTrackableMenu == NULL) { m_bDisplayDialog = FALSE; return; }

	GetCursorPos(&pt);

	if(_CallPlugins(KPM_PWLIST_RCLICK, (LPARAM)m_pPwListTrackableMenu, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	m_pPwListTrackableMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		pt.x, pt.y, AfxGetMainWnd());

	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnClickGroupList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NotifyUserActivity();

	UNREFERENCED_PARAMETER(pNMHDR);

	*pResult = 0;

	POINT pt;
	GetCursorPos(&pt);
	m_cGroups.ScreenToClient(&pt);
	HTREEITEM h = m_cGroups.HitTest(CPoint(pt));
	m_cGroups.SelectItem(h); // Select the item the user pointed to

	// Remove the search group because we cannot handle it like a normal group
	_Groups_SaveView(TRUE);
	DWORD_PTR dwCachedFirst = m_dwGroupsSaveFirstVisible;
	DWORD_PTR dwCachedSelected = ((h != NULL) ? m_cGroups.GetItemData(h) : DWORD_MAX);
	if(_RemoveSearchGroup() == TRUE)
	{
		// UpdateGroupList();
		m_dwGroupsSaveFirstVisible = dwCachedFirst;
		m_dwGroupsSaveSelected = dwCachedSelected;
		_Groups_RestoreView();
	}

	UpdatePasswordList();
	_UpdateToolBar(TRUE);
}

void CPwSafeDlg::OnPwlistCopyPw()
{
	NotifyUserActivity();
	m_bDisplayDialog = TRUE;
	if(_CallPlugins(KPM_PW_COPY, 0, 0) == FALSE) { m_bDisplayDialog = FALSE; return; }
	m_bDisplayDialog = FALSE;

	const DWORD dwIndex = _ListSelToEntryIndex();
	ASSERT(dwIndex != DWORD_MAX); if(dwIndex == DWORD_MAX) return;

	PW_ENTRY *p = m_mgr.GetEntry(dwIndex);
	ASSERT_ENTRY(p); if(p == NULL) return;

	if(m_nClipboardMethod == CM_TIMED)
	{
		m_mgr.UnlockEntryPassword(p);
		CString strPwCopy = p->pszPassword;
		m_mgr.LockEntryPassword(p);

		CopyStringToClipboard(strPwCopy, p, &m_mgr);
		EraseCString(&strPwCopy);
		SetStatusTextEx(TRL("Field copied to clipboard."));

		m_nClipboardCountdown = static_cast<int>(m_dwClipboardSecs);
	}
	else if(m_nClipboardMethod == CM_ENHSECURE)
	{
		m_nClipboardState = CLIPBOARD_DELAYED_PASSWORD;
		MakeClipboardDelayRender(m_hWnd, &m_hwndNextViewer);

		CString str = TRL("Field copied to clipboard.");
		str += _T(" ");
		str += TRL("Waiting for paste command.");
		SetStatusTextEx(str);
	}

	DropToBackgroundIfOptionEnabled(false);

	if(CPwUtil::IsTANEntry(p) == TRUE) // If it is a TAN entry, expire it
	{
		_PostUseTANEntry(GetSelectedEntry(), dwIndex);
		p = NULL; // p may now be invalid!
	}

	_TouchEntry(GetSelectedEntry(), FALSE);
	_UpdateToolBar();
}

void CPwSafeDlg::OnTimer(WPARAM nIDEvent)
{
	if((nIDEvent == APPWND_TIMER_ID_UPDATER))
	{
		if(m_bCachedToolBarUpdate)
		{
			m_bCachedToolBarUpdate = FALSE;
			_UpdateToolBar();
		}

		if(m_bCachedPwlistUpdate)
		{
			m_bCachedPwlistUpdate = FALSE;
			UpdatePasswordList();
			_UpdateToolBar(TRUE);
		}
	}
	else if(nIDEvent == APPWND_TIMER_ID)
	{
		if(m_nClipboardCountdown != -1)
		{
			--m_nClipboardCountdown;

			if(m_nClipboardCountdown == -1)
			{
				SetStatusTextEx(TRL("Ready."));
				ClearClipboardIfOwner();
			}
			else if(m_nClipboardCountdown == 0)
				SetStatusTextEx(TRL("Clipboard cleared."));
			else
			{
				CString str;
				str.Format(TRL("Field copied to clipboard. Clipboard will be cleared in %d seconds."), m_nClipboardCountdown);
				SetStatusTextEx(str);
			}
		}

		if((m_bLocked == FALSE) && (m_bFileOpen == TRUE) && (m_bDisplayDialog == FALSE))
		{
			if(m_nLockTimeDef != -1)
			{
				// if(m_nLockCountdown != 0)
				// {

				// TryEnterCriticalSection apparently is available on Windows 98,
				// but it doesn't do anything
				const bool b9x = (AU_IsWin9xSystem() != FALSE);
				if(b9x || (TryEnterCriticalSection(CPwSafeApp::GetLockTimerCS()) != FALSE))
				{
					// --m_nLockCountdown;
					// if(m_nLockCountdown == 0)

					const UINT64 uCurTimeUtc = _GetCurrentTimeUtc();
					if(uCurTimeUtc >= m_uLockAt)
					{
						if(m_bExitInsteadOfLockAT == FALSE)
						{
							OnFileLock();

							if((m_bLocked == TRUE) && (m_bMinimized == FALSE) &&
								(m_bTrayed == FALSE))
							{
								SetViewHideState(FALSE, FALSE); // Minimize/tray
							}
						}
						else OnFileExit();
					}

					if(!b9x) LeaveCriticalSection(CPwSafeApp::GetLockTimerCS());
				}
				
				// }
			}
		}

		if(m_nFileRelockCountdown != -1)
		{
			--m_nFileRelockCountdown;

			if(m_nFileRelockCountdown == -1)
			{
				if(((m_bFileOpen == TRUE) || (m_bLocked == TRUE)) && (m_strLastDb.IsEmpty() == FALSE))
				{
					if(FileLock_Lock(m_strLastDb, TRUE) == TRUE) m_nFileRelockCountdown = FL_TIME_RELOCK_AFTER * 60;
					else m_nFileRelockCountdown = 30;
				}
				else m_nFileRelockCountdown = FL_TIME_RELOCK_AFTER * 60;
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CPwSafeDlg::OnDblclkPwlist(NMHDR* pNMHDR, LRESULT* pResult)
{
	NotifyUserActivity();

	NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
	CString strData;
	const DWORD dwEntryIndex = _ListSelToEntryIndex();

	*pResult = 0;

	if(dwEntryIndex == DWORD_MAX) return;

	PW_ENTRY *p = m_mgr.GetEntry(dwEntryIndex);
	ASSERT_ENTRY(p); if(p == NULL) return;

	const BOOL bIsTAN = CPwUtil::IsTANEntry(p);

	switch(pNMListView->iSubItem)
	{
	case 0:
		if((bIsTAN == TRUE) && (m_dwPwListMode != LVS_REPORT)) OnPwlistCopyPw();
		else OnPwlistEdit();
		p = NULL; // p may now be invalid!
		break;
	case 1:
		OnPwlistCopyUser();
		p = NULL; // p may now be invalid!
		break;
	case 2:
		OnPwlistVisitUrl();
		break;
	case 3:
		OnPwlistCopyPw();
		p = NULL; // p may now be invalid!
		break;
	case 4:
		strData = p->pszAdditional;
		strData = CsRemoveMeta(&strData);
		CopyStringToClipboard(strData, p, &m_mgr);
		EraseCString(&strData);
		m_nClipboardCountdown = static_cast<int>(m_dwClipboardSecs);
		SetStatusTextEx(TRL("Field copied to clipboard."));
		DropToBackgroundIfOptionEnabled(false);
		break;
	case 5:
		_PwTimeToStringEx(p->tCreation, strData, CPwSafeDlg::m_bUseLocalTimeFormat);
		CopyStringToClipboard(strData, NULL, NULL);
		m_nClipboardCountdown = static_cast<int>(m_dwClipboardSecs);
		SetStatusTextEx(TRL("Field copied to clipboard."));
		DropToBackgroundIfOptionEnabled(false);
		break;
	case 6:
		_PwTimeToStringEx(p->tLastMod, strData, CPwSafeDlg::m_bUseLocalTimeFormat);
		CopyStringToClipboard(strData, NULL, NULL);
		m_nClipboardCountdown = static_cast<int>(m_dwClipboardSecs);
		SetStatusTextEx(TRL("Field copied to clipboard."));
		DropToBackgroundIfOptionEnabled(false);
		break;
	case 7:
		_PwTimeToStringEx(p->tLastAccess, strData, CPwSafeDlg::m_bUseLocalTimeFormat);
		CopyStringToClipboard(strData, NULL, NULL);
		m_nClipboardCountdown = static_cast<int>(m_dwClipboardSecs);
		SetStatusTextEx(TRL("Field copied to clipboard."));
		DropToBackgroundIfOptionEnabled(false);
		break;
	case 8:
		_PwTimeToStringEx(p->tExpire, strData, CPwSafeDlg::m_bUseLocalTimeFormat);
		CopyStringToClipboard(strData, NULL, NULL);
		m_nClipboardCountdown = static_cast<int>(m_dwClipboardSecs);
		SetStatusTextEx(TRL("Field copied to clipboard."));
		DropToBackgroundIfOptionEnabled(false);
		break;
	case 9:
		_UuidToString(p->uuid, &strData);
		CopyStringToClipboard(strData, NULL, NULL);
		m_nClipboardCountdown = static_cast<int>(m_dwClipboardSecs);
		SetStatusTextEx(TRL("Field copied to clipboard."));
		DropToBackgroundIfOptionEnabled(false);
		break;
	case 10:
		CopyStringToClipboard(p->pszBinaryDesc, p, &m_mgr);
		m_nClipboardCountdown = static_cast<int>(m_dwClipboardSecs);
		SetStatusTextEx(TRL("Field copied to clipboard."));
		DropToBackgroundIfOptionEnabled(false);
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	_TouchEntry(GetSelectedEntry(), FALSE);
	_UpdateToolBar();
}

void CPwSafeDlg::OnRclickGroupList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NotifyUserActivity();

	UNREFERENCED_PARAMETER(pNMHDR);
	*pResult = 0;

	if(m_bFileOpen == FALSE) return;

	m_bDisplayDialog = TRUE;
	if(_CallPlugins(KPM_GROUPLIST_RCLICK, (LPARAM)pNMHDR, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	POINT pt;
	GetCursorPos(&pt);

	UINT uFlags = 0;
	POINT ptClient = pt;
	m_cGroups.ScreenToClient(&ptClient);
	// HTREEITEM hItem = m_cGroups.HitTest(CPoint(ptClient), &uFlags);
	// if(uFlags & (TVHT_ONITEM | TVHT_ONITEMINDENT)) m_cGroups.SelectItem(hItem);
	// else m_cGroups.SelectItem(NULL);
	m_cGroups.SelectItem(m_cGroups.HitTest(CPoint(ptClient), &uFlags));

	ASSERT(m_pGroupListTrackableMenu != NULL);
	if(m_pGroupListTrackableMenu == NULL) { m_bDisplayDialog = FALSE; return; }

	m_pGroupListTrackableMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		pt.x, pt.y, AfxGetMainWnd());

	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnPwlistCopyUser()
{
	NotifyUserActivity();
	m_bDisplayDialog = TRUE;
	if(_CallPlugins(KPM_USER_COPY, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }
	m_bDisplayDialog = FALSE;

	if(m_nClipboardMethod == CM_TIMED)
	{
		DWORD dwEntryIndex = _ListSelToEntryIndex();

		ASSERT(dwEntryIndex != DWORD_MAX); if(dwEntryIndex == DWORD_MAX) return;

		PW_ENTRY *p = m_mgr.GetEntry(dwEntryIndex);
		ASSERT_ENTRY(p); if(p == NULL) return;

		if(CPwUtil::IsTANEntry(p) == TRUE)
		{
			// It is a TAN entry, so copy the password instead of the user name
			OnPwlistCopyPw();
			return;
		}

		CopyStringToClipboard(p->pszUserName, p, &m_mgr);
		m_nClipboardCountdown = static_cast<int>(m_dwClipboardSecs);
	}
	else if(m_nClipboardMethod == CM_ENHSECURE)
	{
		m_nClipboardState = CLIPBOARD_DELAYED_USERNAME;
		MakeClipboardDelayRender(m_hWnd, &m_hwndNextViewer);
	}

	SetStatusTextEx(TRL("Field copied to clipboard."));
	DropToBackgroundIfOptionEnabled(false);

	_TouchEntry(GetSelectedEntry(), FALSE);
	_UpdateToolBar();
}

void CPwSafeDlg::ParseAndOpenURLWithEntryInfo(LPCTSTR lpURL, PW_ENTRY *pEntry)
{
	if((pEntry != NULL) && (lpURL != NULL))
	{
		CString strURL = lpURL;

		if(strURL.GetLength() != 0)
		{
			CString strOverride;
			strOverride = ExtractParameterFromString(pEntry->pszAdditional, _T("url-override:"), 0);
			if(strOverride.GetLength() != 0)
				strURL = strOverride;
			else if(m_strURLOverride.GetLength() != 0)
			{
				if(IsUnsafeAllowed(this->m_hWnd) == TRUE)
				{
					if(WU_IsCommandLineURL(strURL) == false)
						strURL = m_strURLOverride; // Apply override
				}
			}

			const bool bCmdQuotes = WU_IsCommandLineURL(strURL);

			strURL = SprCompile(strURL, false, pEntry, &m_mgr, false, bCmdQuotes);

			strURL = strURL.TrimLeft(_T(" \t\r\n"));

			if(WU_IsCommandLineURL(strURL) == false) FixURL(&strURL);

			if(strURL.GetLength() != 0)
			{
				if(m_bUsePuttyForURLs == TRUE)
				{
					CString strUser = SprCompile(pEntry->pszUserName, false, pEntry,
						&m_mgr, false, true);
					if(OpenUrlUsingPutty(strURL, strUser) == FALSE)
						OpenUrlEx(strURL, this->m_hWnd);
				}
				else OpenUrlEx(strURL, this->m_hWnd);
			}
		}
	}
}

void CPwSafeDlg::OnPwlistVisitUrl()
{
	NotifyUserActivity();

	PW_ENTRY *p;
	DWORD dwGroupId = GetSelectedGroupId();
	BOOL bLaunched = FALSE;

	ASSERT(dwGroupId != DWORD_MAX); if(dwGroupId == DWORD_MAX) return;

	m_bDisplayDialog = TRUE;
	if(_CallPlugins(KPM_URL_VISIT, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }
	m_bDisplayDialog = FALSE;

	if(m_bCopyURLs == FALSE) // Open URLs
	{
		for(int i = 0; i < m_cList.GetItemCount(); ++i)
		{
			const UINT uState = m_cList.GetItemState(i, LVIS_SELECTED);
			if((uState & LVIS_SELECTED) != 0)
			{
				// p = m_mgr.GetEntryByGroup(dwGroupId, (DWORD)i);

				DWORD dwIndex = _ListSelToEntryIndex(static_cast<DWORD>(i));
				ASSERT(dwIndex != DWORD_MAX); if(dwIndex == DWORD_MAX) continue;

				p = m_mgr.GetEntry(dwIndex);
				ASSERT_ENTRY(p);

				ParseAndOpenURLWithEntryInfo(p->pszURL, p);

				_TouchEntry((DWORD)i, FALSE);
			}
		}
	}
	else // m_bCopyURLs == TRUE
	{
		const DWORD dwSelectedEntry = GetSelectedEntry();
		ASSERT(dwSelectedEntry != DWORD_MAX); if(dwSelectedEntry == DWORD_MAX) return;

		const DWORD dwEntryIndex = _ListSelToEntryIndex(dwSelectedEntry);
		ASSERT(dwEntryIndex != DWORD_MAX); if(dwEntryIndex == DWORD_MAX) return;

		p = m_mgr.GetEntry(dwEntryIndex);
		ASSERT_ENTRY(p); if(p == NULL) return;

		CString strURL = p->pszURL;

		const bool bCmdQuotes = WU_IsCommandLineURL(strURL);

		strURL = SprCompile(strURL, false, p, &m_mgr, false, bCmdQuotes);

		if(WU_IsCommandLineURL(strURL))
		{
			if(m_bUsePuttyForURLs == TRUE)
			{
				CString strUser = SprCompile(p->pszUserName, false, p, &m_mgr, false, true);
				if(OpenUrlUsingPutty(strURL, strUser) == FALSE)
					OpenUrlEx(strURL, this->m_hWnd);
			}
			else OpenUrlEx(strURL, this->m_hWnd);

			bLaunched = TRUE;
		}
		else CopyStringToClipboard(strURL, NULL, NULL); // Dereferenced already

		EraseCString(&strURL);

		_TouchEntry(dwSelectedEntry, FALSE);

		if(bLaunched == FALSE)
		{
			m_nClipboardCountdown = (int)m_dwClipboardSecs;
			SetStatusTextEx(TRL("Field copied to clipboard."));
			DropToBackgroundIfOptionEnabled(false);
		}
	}

	_UpdateToolBar();
}

void CPwSafeDlg::OnFileNew()
{
	NotifyUserActivity();

	// if(m_bLocked == TRUE) return;

	if(m_bFileOpen == TRUE) OnFileClose();
	if(m_bFileOpen == TRUE) return;

	if(_CallPlugins(KPM_FILE_NEW_PRE, 0, 0) == FALSE) return;

	m_mgr.NewDatabase();
	if(_ChangeMasterKey(NULL, TRUE) == FALSE) return;

	m_bInitialCmdLineFile = FALSE;

	m_bFileOpen = TRUE;
	m_bLocked = FALSE;
	m_cList.EnableWindow(TRUE);
	m_cGroups.EnableWindow(TRUE);
	m_bModified = TRUE;

	m_strFile.Empty(); m_strFileAbsolute.Empty();

	PW_GROUP pwTemplate;
	PW_TIME tNow;
	ZeroMemory(&pwTemplate, sizeof(PW_GROUP));
	_GetCurrentPwTime(&tNow);
	pwTemplate.tCreation = tNow;
	m_mgr.GetNeverExpireTime(&pwTemplate.tExpire);
	pwTemplate.tLastAccess = tNow;
	pwTemplate.tLastMod = tNow;

	CPrivateConfigEx cfg(FALSE);
	std::vector<std::basic_string<TCHAR> > vNewGroups =
		cfg.GetArray(PWMKEY_GROUPONNEW_PRE);
	bool bCustomGroups = (vNewGroups.size() > 0);
	if(bCustomGroups && (vNewGroups[0] == _T("0")))
		bCustomGroups = false;

	if(bCustomGroups)
	{
		for(DWORD dwNewGroup = DWORD_MAX; dwNewGroup != vNewGroups.size(); ++dwNewGroup)
		{
			std::basic_string<TCHAR> strSpec;

			if(dwNewGroup != DWORD_MAX) strSpec = vNewGroups[dwNewGroup];
			else
			{
				TCHAR lpRootGroup[SI_REGSIZE];
				if(cfg.Get(PWMKEY_ROOTONNEW, lpRootGroup) == TRUE)
					strSpec = lpRootGroup;
				else strSpec = _T("@");
			}

			if(strSpec.size() == 0) continue;

			std::basic_string<TCHAR> strSplit = strSpec.substr(0, 1);
			std::basic_string<TCHAR> strDef = strSpec.substr(1);
			std::vector<std::basic_string<TCHAR> > vSpec;
			SU_Split(vSpec, strDef, strSplit.c_str());

			pwTemplate.pszGroupName = const_cast<TCHAR *>(TRL("General"));
			if((vSpec.size() > 0) && (vSpec[0].size() > 0))
				pwTemplate.pszGroupName = const_cast<TCHAR *>(vSpec[0].c_str());

			pwTemplate.uImageId = 48;
			if((vSpec.size() > 1) && (vSpec[1].size() > 0))
				pwTemplate.uImageId = static_cast<DWORD>(_ttol(vSpec[1].c_str()));

			pwTemplate.usLevel = ((dwNewGroup == DWORD_MAX) ? 0 : 1);
			pwTemplate.uGroupId = 0;
			pwTemplate.dwFlags = ((dwNewGroup == DWORD_MAX) ? PWGF_EXPANDED : 0);
			VERIFY(m_mgr.AddGroup(&pwTemplate));
		}
	}
	else
	{
		pwTemplate.uImageId = 48;
		pwTemplate.pszGroupName = const_cast<TCHAR *>(TRL("General"));
		pwTemplate.usLevel = 0; pwTemplate.uGroupId = 0; // 0 = create new group ID
		pwTemplate.dwFlags |= PWGF_EXPANDED;
		VERIFY(m_mgr.AddGroup(&pwTemplate));

		// Add standard groups
		pwTemplate.uImageId = 38; pwTemplate.pszGroupName = const_cast<LPTSTR>(TRL("Windows"));
		pwTemplate.usLevel = 1; pwTemplate.uGroupId = 0; // 0 = create new group ID
		pwTemplate.dwFlags = 0;
		VERIFY(m_mgr.AddGroup(&pwTemplate));
		pwTemplate.uImageId = 3; pwTemplate.pszGroupName = const_cast<LPTSTR>(TRL("Network"));
		pwTemplate.usLevel = 1; pwTemplate.uGroupId = 0; // 0 = create new group ID
		VERIFY(m_mgr.AddGroup(&pwTemplate));
		pwTemplate.uImageId = 1; pwTemplate.pszGroupName = const_cast<LPTSTR>(TRL("Internet"));
		pwTemplate.usLevel = 1; pwTemplate.uGroupId = 0; // 0 = create new group ID
		VERIFY(m_mgr.AddGroup(&pwTemplate));
		pwTemplate.uImageId = 19; pwTemplate.pszGroupName = const_cast<LPTSTR>(TRL("eMail"));
		pwTemplate.usLevel = 1; pwTemplate.uGroupId = 0; // 0 = create new group ID
		VERIFY(m_mgr.AddGroup(&pwTemplate));
		pwTemplate.uImageId = 37; pwTemplate.pszGroupName = const_cast<LPTSTR>(TRL("Homebanking"));
		pwTemplate.usLevel = 1; pwTemplate.uGroupId = 0; // 0 = create new group ID
		VERIFY(m_mgr.AddGroup(&pwTemplate));
		/* pwTemplate.uImageId = 37; pwTemplate.pszGroupName = const_cast<LPTSTR>(TRL("Mobile devices"));
		pwTemplate.usLevel = 0; pwTemplate.uGroupId = 0; // 0 = create new group ID
		VERIFY(m_mgr.AddGroup(&pwTemplate));
		pwTemplate.uImageId = 37; pwTemplate.pszGroupName = const_cast<LPTSTR>(TRL("Credit cards"));
		pwTemplate.usLevel = 0; pwTemplate.uGroupId = 0; // 0 = create new group ID
		VERIFY(m_mgr.AddGroup(&pwTemplate)); */

		// TESTING CODE, creates entries in the debug version
#ifdef ___PWSAFE_SAMPLE_DATA
		PW_ENTRY pwT;
		pwT.pBinaryData = NULL; pwT.pszBinaryDesc = NULL; pwT.uBinaryDataLen = 0;
		pwT.pszAdditional = _T("Some Notes");
		pwT.pszPassword = _T("The Password");
		pwT.pszURL = _T("google.com");
		pwT.pszUserName = _T("Anonymous");
		pwT.tCreation = tNow; m_mgr.GetNeverExpireTime(&pwT.tExpire); pwT.tLastAccess = tNow;
		pwT.tLastMod = tNow;
		for(int ix = 0; ix < (32 + 32 + 32 + 3); ++ix)
		{
			CString str;
			str.Format(_T("%d Group"), ix);
			pwTemplate.uImageId = (static_cast<DWORD>(rand()) % 30);
			pwTemplate.pszGroupName = const_cast<LPTSTR>((LPCTSTR)str);
			pwTemplate.uGroupId = 0; // 0 = create new group
			pwTemplate.usLevel = static_cast<USHORT>(rand() % 5);
			VERIFY(m_mgr.AddGroup(&pwTemplate));
		}

		PW_TIME tExpired;
		tExpired.btDay = 1; tExpired.btHour = 1; tExpired.btMinute = 1;
		tExpired.btMonth = 1; tExpired.btSecond = 0; tExpired.shYear = 2000;
		for(int ir = 0; ir < 10; ++ir)
		{
			pwT.pszTitle = _T("I am expired");
			pwT.uGroupId = m_mgr.GetGroupIdByIndex(static_cast<DWORD>(rand()) % 6);
			pwT.uImageId = (static_cast<DWORD>(rand()) % 30);
			pwT.tExpire = tExpired;
			ZeroMemory(pwT.uuid, 16);
			VERIFY(m_mgr.AddEntry(&pwT));
		}

		for(int iy = 0; iy < (1024 + 256 + 3); ++iy)
		{
			CString str;
			str.Format(_T("Sample #%d"), iy);
			pwT.pszTitle = const_cast<LPTSTR>((LPCTSTR)str);
			pwT.uGroupId = m_mgr.GetGroupIdByIndex((DWORD)rand() % 8);
			pwT.uImageId = (DWORD)rand() % 30;
			m_mgr.GetNeverExpireTime(&pwT.tExpire);
			ZeroMemory(pwT.uuid, 16);
			VERIFY(m_mgr.AddEntry(&pwT));
		}
#endif
	}

	m_mgr.FixGroupTree();
	UpdateGroupList();
	UpdatePasswordList();
	_UpdateToolBar();
	_SetDefaultFocus();

	_CallPlugins(KPM_FILE_NEW_POST, 0, 0);
}

BOOL CPwSafeDlg::_ChangeMasterKey(CPwManager *pDbMgr, BOOL bCreateNew)
{
	CPasswordDlg dlg;
	int nConfirmAttempts;
	BOOL bSuccess;
	CPwManager *pMgr;
	DWORD_PTR dwOpFlags = 0;
	DWORD_PTR aOpParams[3];

	if(bCreateNew == TRUE) dwOpFlags |= 1;

	aOpParams[0] = 0; // Buffer for return value -- plugins should fill it
	aOpParams[1] = 0; // Deprecated, (DWORD_PTR)pDbMgr;
	aOpParams[2] = dwOpFlags;
	if(_CallPlugins(KPM_CHANGE_MASTER_KEY_PRE, (LPARAM)&aOpParams[0], 0) == FALSE)
		return ((aOpParams[0] == 0) ? FALSE : TRUE);

	if(pDbMgr == NULL) pMgr = &m_mgr;
	else pMgr = pDbMgr;

	m_bDisplayDialog = TRUE;

	dlg.m_bKeyMethod = PWM_KEYMETHOD_OR;
	dlg.m_bLoadMode = FALSE;
	dlg.m_bConfirm = FALSE;
	dlg.m_hWindowIcon = m_hIcon;
	dlg.m_bChanging = ((bCreateNew == FALSE) ? TRUE : FALSE);

	if(dlg.DoModal() == IDCANCEL) { m_bDisplayDialog = FALSE; return FALSE; }

	if((dlg.m_bKeyFile == FALSE) || (dlg.m_bKeyMethod == PWM_KEYMETHOD_AND))
	{
		bSuccess = FALSE;

		for(nConfirmAttempts = 0; nConfirmAttempts < 3; ++nConfirmAttempts)
		{
			CPasswordDlg *pDlg2 = new CPasswordDlg();
			ASSERT(pDlg2 != NULL); if(pDlg2 == NULL) continue;

			pDlg2->m_bKeyMethod = PWM_KEYMETHOD_OR;
			pDlg2->m_bLoadMode = FALSE;
			pDlg2->m_bConfirm = TRUE;
			pDlg2->m_hWindowIcon = m_hIcon;
			pDlg2->m_bChanging = ((bCreateNew == TRUE) ? FALSE : TRUE);

			if(pDlg2->DoModal() == IDCANCEL)
			{
				ASSERT((pDlg2->m_lpKey == NULL) && (pDlg2->m_lpKey2 == NULL));
				SAFE_DELETE(pDlg2);
				dlg.FreePasswords();
				m_bDisplayDialog = FALSE;
				return FALSE;
			}

			if(dlg.m_bKeyMethod == PWM_KEYMETHOD_OR)
			{
				if((_tcscmp(pDlg2->m_lpKey, dlg.m_lpKey) != 0) || (pDlg2->m_bKeyFile == TRUE))
				{
					pDlg2->FreePasswords();
					SAFE_DELETE(pDlg2);
					MessageBox(TRL("Password and repeated password aren't identical!"),
						PWM_PRODUCT_NAME_SHORT, MB_ICONWARNING | MB_OK);
					continue;
				}
			}
			else // dlg.m_nKeyMethod == PWM_KEYMETHOD_AND
			{
				if((_tcscmp(pDlg2->m_lpKey, dlg.m_lpKey2) != 0) || (pDlg2->m_bKeyFile == TRUE))
				{
					pDlg2->FreePasswords();
					SAFE_DELETE(pDlg2);
					MessageBox(TRL("Password and repeated password aren't identical!"),
						PWM_PRODUCT_NAME_SHORT, MB_ICONWARNING | MB_OK);
					continue;
				}
			}

			pDlg2->FreePasswords();
			SAFE_DELETE(pDlg2);
			bSuccess = TRUE;
			break;
		}

		if(bSuccess == FALSE) { dlg.FreePasswords(); m_bDisplayDialog = FALSE; return FALSE; }
	}

	CPwSafeAppRI ri;
	int nErrCode, nErrCode2;

	if(dlg.m_bKeyMethod == PWM_KEYMETHOD_OR)
		nErrCode = pMgr->SetMasterKey(dlg.m_lpKey, dlg.m_bKeyFile, NULL, &ri, FALSE,
			dlg.m_strSelectedKeyProv);
	else
	{
		ASSERT(dlg.m_bKeyFile == TRUE);
		nErrCode = pMgr->SetMasterKey(dlg.m_lpKey, dlg.m_bKeyFile, dlg.m_lpKey2, &ri,
			FALSE, dlg.m_strSelectedKeyProv);
	}

	if(nErrCode != PWE_SUCCESS)
	{
		if(dlg.m_bKeyFile == TRUE)
		{
			CVistaTaskDialog dlgTask(this->m_hWnd, AfxGetInstanceHandle(), true);
			CString strTX = TRL("The selected key file already exists");
			strTX += _T(":\r\n");
			strTX += dlg.m_lpKey;
			dlgTask.SetContent(strTX);
			dlgTask.SetMainInstruction(TRL("Overwrite existing key file?"));
			dlgTask.SetWindowTitle(PWM_PRODUCT_NAME_SHORT);
			dlgTask.SetIcon(MAKEINTRESOURCEW(IDI_KEY));
			dlgTask.AddButton(TRL("Overwrite"), TRL("Generate a new key and overwrite the existing key file. The key currently stored in the existing key file will be lost."), IDYES);
			dlgTask.AddButton(TRL("Reuse"), TRL("Load the existing key file and use it as key for the current database. The key file will not be modified."), IDNO);
			dlgTask.AddButton(TRL("Cancel"), TRL("Abort specifying a new key for the current database."), IDCANCEL);
			int nMsg = dlgTask.ShowDialog();

			if(nMsg < 0) // No task dialog support?
			{
				CString strMsg = TRL("A key file already exists on this drive. Do you want to overwrite or reuse it?");
				strMsg += _T("\r\n\r\n");
				strMsg += TRL("Click [Yes] to overwrite the key file."); strMsg += _T("\r\n");
				strMsg += TRL("Click [No] to reuse the selected key file for this database."); strMsg += _T("\r\n");
				strMsg += TRL("Click [Cancel] to abort changing the key.");

				nMsg = MessageBox(strMsg, TRL("Overwrite?"), MB_ICONQUESTION | MB_YESNOCANCEL);
			}

			if(nMsg == IDYES)
			{
				if(dlg.m_bKeyMethod == PWM_KEYMETHOD_OR)
					nErrCode2 = pMgr->SetMasterKey(dlg.m_lpKey, dlg.m_bKeyFile, NULL,
						&ri, TRUE, dlg.m_strSelectedKeyProv);
				else
					nErrCode2 = pMgr->SetMasterKey(dlg.m_lpKey, dlg.m_bKeyFile,
						dlg.m_lpKey2, &ri, TRUE, dlg.m_strSelectedKeyProv);

				if(nErrCode2 != PWE_SUCCESS)
				{
					dlg.FreePasswords();
					CNewDialogsEx::ShowError(this->m_hWnd, nErrCode2, 0);
					m_bDisplayDialog = FALSE; return FALSE;
				}
			}
			else if(nMsg == IDNO)
			{
				if(dlg.m_bKeyMethod == PWM_KEYMETHOD_OR)
					nErrCode2 = pMgr->SetMasterKey(dlg.m_lpKey, dlg.m_bKeyFile,
						NULL, NULL, FALSE, dlg.m_strSelectedKeyProv);
				else
					nErrCode2 = pMgr->SetMasterKey(dlg.m_lpKey, dlg.m_bKeyFile,
						dlg.m_lpKey2, NULL, FALSE, dlg.m_strSelectedKeyProv);

				if(nErrCode2 != PWE_SUCCESS)
				{
					dlg.FreePasswords();
					CNewDialogsEx::ShowError(this->m_hWnd, nErrCode2, 0);
					m_bDisplayDialog = FALSE; return FALSE;
				}
			}
			else { dlg.FreePasswords(); m_bDisplayDialog = FALSE; return FALSE; }
		}
		else
		{
			dlg.FreePasswords();
			CNewDialogsEx::ShowError(this->m_hWnd, nErrCode, 0);
			m_bDisplayDialog = FALSE; return FALSE;
		}
	}

	if(pDbMgr == NULL) m_bModified = TRUE;

	dlg.FreePasswords();

	if(bCreateNew == FALSE)
	{
		CString str = TRL("The composite master key has been changed!");
		str += _T("\r\n\r\n");
		str += TRL("Save the database now in order to get the new key applied.");
		MessageBox(str, TRL("Success"), MB_ICONINFORMATION | MB_OK);
	}

	m_bDisplayDialog = FALSE;
	return TRUE;
}

// When pszFile == NULL a file selection dialog is displayed
void CPwSafeDlg::_OpenDatabase(CPwManager *pDbMgr, const TCHAR *pszFile,
	const TCHAR *pszPassword, const TCHAR *pszKeyFile, BOOL bOpenLocked,
	LPCTSTR lpPreSelectPath, BOOL bIgnoreCorrupted)
{
	NotifyUserActivity();

	CString strFile, strText;
	DWORD dwFlags;
	INT_PTR nRet = IDCANCEL;
	int nErr;
	const TCHAR *pSuffix = _T("");
	CPasswordDlg *pDlgPass = NULL;
	CPwManager *pMgr;
	DWORD_PTR dwOpFlags = 0;
	DWORD_PTR aOpParams[7];

	if(pDbMgr == NULL) pMgr = &m_mgr;
	else pMgr = pDbMgr;

	PWDB_REPAIR_INFO repairInfo;
	ZeroMemory(&repairInfo, sizeof(PWDB_REPAIR_INFO));

	if(bOpenLocked == TRUE) dwOpFlags |= 1;
	if(bIgnoreCorrupted == TRUE) dwOpFlags |= 2;
	aOpParams[0] = 0; // Currently unused
	aOpParams[1] = 0; // Deprecated, (DWORD_PTR)pDbMgr;
	aOpParams[2] = (DWORD_PTR)pszFile;
	aOpParams[3] = (DWORD_PTR)pszPassword;
	aOpParams[4] = (DWORD_PTR)pszKeyFile;
	aOpParams[5] = (DWORD_PTR)lpPreSelectPath;
	aOpParams[6] = dwOpFlags;
	if(_CallPlugins(KPM_OPENDB_PRE, (LPARAM)&aOpParams[0], 0) == FALSE) return;

	CString strFilter = TRL("KeePass Password Safe Files");
	strFilter += _T(" (*.kdb/*.pwd)|*.kdb;*.pwd|");
	strFilter += TRL("All Files");
	strFilter += _T(" (*.*)|*.*||");

	dwFlags = (OFN_LONGNAMES | OFN_EXTENSIONDIFFERENT | OFN_EXPLORER | OFN_ENABLESIZING);
	dwFlags |= (OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST); // | OFN_HIDEREADONLY
	if(m_bFileReadOnly == TRUE) dwFlags |= OFN_READONLY;
	if(pDbMgr != NULL) dwFlags |= OFN_NOCHANGEDIR;

	CFileDialog dlg(TRUE, NULL, NULL, dwFlags, strFilter, this);

	m_bDisplayDialog = TRUE;
	if(pszFile == NULL) nRet = dlg.DoModal();
	else strFile = pszFile;
	m_bDisplayDialog = FALSE;

	if((pszFile != NULL) || (nRet == IDOK))
	{
		if(pDbMgr == NULL)
		{
			if(m_bFileOpen == TRUE) OnFileClose();
			if(m_bFileOpen == TRUE) return;
		}

		if(pszFile == NULL)
		{
			strFile = dlg.GetPathName();
			m_bInitialCmdLineFile = FALSE;
		}

		if(pDbMgr == NULL) m_strLastDb = strFile;

		if((bOpenLocked == TRUE) && (pDbMgr == NULL))
		{
			m_strFile = strFile;
			m_strFileAbsolute = GetShortestAbsolutePath(m_strFile);
			// const FullPathName database((LPCTSTR)m_strFile);
			// m_strFileAbsolute = database.getFullPathName().c_str();            

			m_bLocked = TRUE;
			CString strExtended = TRL("&Unlock Workspace");
			strExtended += _T("\t");
			strExtended += _GetCmdAccelExt(_T("&Lock Workspace"));
			m_menu.SetMenuText(ID_FILE_LOCK, strExtended, MF_BYCOMMAND);
			SetStatusTextEx(CString(TRL("Workspace locked")) + _T("."));
			m_btnTbLock.SetTooltipText(RemoveAcceleratorTipEx(TRL("&Unlock Workspace")));
		}
		else
		{
			int nAllowedAttempts;
			if((pszPassword != NULL) || (pszKeyFile != NULL)) nAllowedAttempts = 1;
			else nAllowedAttempts = 3;

			for(int nOpenAttempts = 0; nOpenAttempts < nAllowedAttempts; ++nOpenAttempts)
			{
				ASSERT(pDlgPass == NULL); if(pDlgPass != NULL) { pDlgPass->FreePasswords(); delete pDlgPass; }
				pDlgPass = new CPasswordDlg();
				ASSERT(pDlgPass != NULL); if(pDlgPass == NULL) continue;

				pDlgPass->m_bLoadMode = TRUE;
				pDlgPass->m_bConfirm = FALSE;
				pDlgPass->m_hWindowIcon = m_hIcon;
				pDlgPass->m_lpPreSelectPath = lpPreSelectPath;

				if(pszFile == NULL)
				{
					pDlgPass->m_strDatabasePath = dlg.GetPathName();
					pDlgPass->m_strDescriptiveName = dlg.GetFileName();
				}
				else
				{
					pDlgPass->m_strDatabasePath = strFile;
					pDlgPass->m_strDescriptiveName = CsFileOnly(&strFile);
				}

				if((pszPassword == NULL) && (pszKeyFile == NULL))
				{
					m_bDisplayDialog = TRUE;
					++g_uMasterKeyDlgs;
					if(pDlgPass->DoModal() == IDCANCEL)
					{
						delete pDlgPass; pDlgPass = NULL;
						m_bDisplayDialog = FALSE;
						--g_uMasterKeyDlgs;
						CPwSafeDlg::SetLastMasterKeyDlg(NULL);
						return;
					}
					m_bDisplayDialog = FALSE;
					--g_uMasterKeyDlgs;
					CPwSafeDlg::SetLastMasterKeyDlg(NULL);
				}
				else
				{
					if(pszKeyFile != NULL)
					{
						pDlgPass->m_bKeyFile = TRUE;
						pDlgPass->m_lpKey = _TcsSafeDupAlloc(pszKeyFile);

						if(pszPassword != NULL)
						{
							pDlgPass->m_lpKey2 = _TcsSafeDupAlloc(pszPassword);
							pDlgPass->m_bKeyMethod = PWM_KEYMETHOD_AND;
						}
						else pDlgPass->m_bKeyMethod = PWM_KEYMETHOD_OR;
					}
					else // pszKeyFile == NULL
					{
						pDlgPass->m_bKeyFile = FALSE;
						pDlgPass->m_lpKey = _TcsSafeDupAlloc(pszPassword);
						pDlgPass->m_bKeyMethod = PWM_KEYMETHOD_OR;
					}
				}

				CString strKeyFilePath;
				if((pDlgPass->m_bKeyFile != FALSE) && (pDlgPass->m_lpKey != NULL))
					strKeyFilePath = pDlgPass->m_lpKey;

				if(pDlgPass->m_bKeyMethod == PWM_KEYMETHOD_OR)
					nErr = pMgr->SetMasterKey(pDlgPass->m_lpKey, pDlgPass->m_bKeyFile,
						NULL, NULL, FALSE, pDlgPass->m_strSelectedKeyProv);
				else
					nErr = pMgr->SetMasterKey(pDlgPass->m_lpKey, pDlgPass->m_bKeyFile,
						pDlgPass->m_lpKey2, NULL, FALSE, pDlgPass->m_strSelectedKeyProv);

				pDlgPass->FreePasswords(); delete pDlgPass; pDlgPass = NULL;

				if(nErr != PWE_SUCCESS)
				{
					CNewDialogsEx::ShowError(this->m_hWnd, nErr, 0);
					continue;
				}

				CTaskbarListEx::SetProgressState(this->m_hWnd, TBPF_INDETERMINATE);

				if(bIgnoreCorrupted == TRUE)
					nErr = pMgr->OpenDatabase(strFile, &repairInfo);
				else
					nErr = pMgr->OpenDatabase(strFile, NULL);

				CTaskbarListEx::SetProgressState(this->m_hWnd, TBPF_NOPROGRESS);

				if(nErr == PWE_SUCCESS) // Set or clear key source
					CKeySourcesPool::Set(GetShortestAbsolutePath(strFile), strKeyFilePath);

				if(bIgnoreCorrupted == TRUE)
				{
					CString strRepaired = CPwUtil::FormatError(nErr, PWFF_NO_INTRO |
						PWFF_INVKEY_WITH_CODE);
					CString strRTemp = TRL("&Repair KeePass Database File...");
					strRTemp.Remove(_T('&'));
					CString strRTitle = strRTemp;

					strRepaired += _T("\r\n\r\n");
					strRepaired += TRL("Group:");
					strRTemp.Format(_T(" %u / %u.\r\n"), pMgr->GetNumberOfGroups(), repairInfo.dwOriginalGroupCount);
					strRepaired += strRTemp;
					strRepaired += TRL("Entry");
					strRTemp.Format(_T(": %u / %u.\r\nMeta: %u."), pMgr->GetNumberOfEntries(),
						repairInfo.dwOriginalEntryCount - repairInfo.dwRecognizedMetaStreamCount,
						repairInfo.dwRecognizedMetaStreamCount);
					strRepaired += strRTemp;

					MessageBox(strRepaired, strRTitle, MB_ICONINFORMATION | MB_OK);

					nErr = PWE_SUCCESS;
				}

				if(nErr != PWE_SUCCESS)
				{
					CNewDialogsEx::ShowError(this->m_hWnd, nErr, 0);

					if(nErr == PWE_UNSUPPORTED_KDBX) nOpenAttempts = nAllowedAttempts;
				}
				else if(pDbMgr == NULL)
				{
					m_bHashValid = SHA256_HashFile(strFile, &m_aHashOfFile[0]);

					if(pszFile == NULL) m_bFileReadOnly = dlg.GetReadOnlyPref();
					// else if(CmdArgs::instance().readonlyIsInEffect() == false)
					//	m_bFileReadOnly = FALSE;

					if(m_bFileReadOnly == FALSE)
					{
						std::basic_string<TCHAR> strLockingUser;
						if(FileLock_IsLocked(strFile, strLockingUser) == TRUE)
						{
							CString strMessage = TRL("The file you are trying to open is locked, i.e. someone else has currently opened the file and is editing it.");
							strMessage += _T("\r\n\r\n");

							if(strLockingUser.size() > 0)
							{
								strMessage += TRL("User name:");
								strMessage += _T(" ");
								strMessage += strLockingUser.c_str();
								strMessage += _T(".");
							}

							CVistaTaskDialog vtd(this->m_hWnd, AfxGetInstanceHandle(), true);
							vtd.SetContent(strMessage);
							vtd.SetMainInstruction(TRL("Open the file in read-only mode?"));
							vtd.SetWindowTitle(PWM_PRODUCT_NAME_SHORT);
							vtd.SetIcon(V_MTDI_QUESTION);

							vtd.AddButton(TRL("Read-only"), TRL("Open the file in read-only mode. Changes to the database can be saved to a different file."), IDYES);
							vtd.AddButton(TRL("Normal"), TRL("Take ownership of the file and open it in normal writing mode."), IDNO);

							int nOpenRet = vtd.ShowDialog(NULL);
							if(nOpenRet < 0) // No task dialog support?
							{
								strMessage += _T("\r\n\r\n");
								strMessage += TRL("Open the file in read-only mode?");
								strMessage += _T("\r\n\r\n");
								strMessage += TRL("Click [Yes] to open the file in read-only mode.");
								strMessage += _T("\r\n");
								strMessage += TRL("Click [No] to open the file in normal writing mode.");

								nOpenRet = MessageBox(strMessage, TRL("Open the file in read-only mode?"),
									MB_YESNO | MB_ICONQUESTION);
							}

							if((nOpenRet == IDYES) || (nOpenRet == IDOK))
								m_bFileReadOnly = TRUE;
							else
							{
								m_bFileReadOnly = FALSE;

								FileLock_Lock(strFile, TRUE);
								m_nFileRelockCountdown = FL_TIME_RELOCK_AFTER * 60;
							}
						}
						else // if(m_bFileReadOnly == FALSE)
						{
							FileLock_Lock(strFile, TRUE);
							m_nFileRelockCountdown = FL_TIME_RELOCK_AFTER * 60;
						}
					}

					m_strFile = strFile;
					m_strFileAbsolute = GetShortestAbsolutePath(m_strFile);
					// const FullPathName database((LPCTSTR)m_strFile);
					// m_strFileAbsolute = database.getFullPathName().c_str();

					m_bFileOpen = TRUE;

					m_bModified = FALSE;
					m_cList.EnableWindow(TRUE);
					m_cGroups.EnableWindow(TRUE);

					m_bLocked = FALSE;

					pSuffix = _GetCmdAccelExt(_T("&Lock Workspace"));
					strText = TRL("&Lock Workspace");
					if(_tcslen(pSuffix) != 0) { strText += _T("\t"); strText += pSuffix; }
					m_menu.SetMenuText(ID_FILE_LOCK, strText, MF_BYCOMMAND);
					m_btnTbLock.SetTooltipText(RemoveAcceleratorTipEx(TRL("&Lock Workspace")));

					UpdateGroupList();

					HTREEITEM h;
					h = _GroupIdToHTreeItem(pMgr->m_dwLastSelectedGroupId);
					if(h != NULL) m_cGroups.SelectItem(h);
					UpdatePasswordList();

					h = _GroupIdToHTreeItem(pMgr->m_dwLastTopVisibleGroupId);
					if(h != NULL)
					{
						m_cGroups.Select(h, TVGN_FIRSTVISIBLE);
						// m_cGroups.SetItemState(h, 0, TVIS_SELECTED);
					}

					DWORD dwEntryPos = _EntryUuidToListPos(pMgr->m_aLastTopVisibleEntryUuid);
					if(dwEntryPos != DWORD_MAX)
					{
						m_cList.EnsureVisible(m_cList.GetItemCount() - 1, FALSE);
						m_cList.EnsureVisible(static_cast<int>(dwEntryPos), FALSE);
					}

					dwEntryPos = _EntryUuidToListPos(pMgr->m_aLastSelectedEntryUuid);
					if(dwEntryPos != DWORD_MAX)
						m_cList.FocusItem(static_cast<int>(dwEntryPos), TRUE);

					NewGUI_ComboBox_UpdateHistory(m_cQuickFind, std::basic_string<TCHAR>(),
						pMgr->AccessPropertyStrArray(PWPA_SEARCH_HISTORY),
						PWM_STD_MAX_HISTORYITEMS);

					SetStatusTextEx(TRL("Ready."));

					_CallPlugins(KPM_OPENDB_COMMITTED, (LPARAM)pMgr->GetLastDatabaseHeader(), 0);
					break;
				}
				else if(pDbMgr != NULL) break;
			}
		}
	}
	ASSERT(pDlgPass == NULL);

	if(pDbMgr == NULL)
	{
		if(m_bLocked == FALSE)
		{
			if((m_bAutoShowExpired == TRUE) || (m_bAutoShowExpiredSoon == TRUE))
				_ShowExpiredEntries(FALSE, m_bAutoShowExpired, m_bAutoShowExpiredSoon);

			_CallPlugins(KPM_OPENDB_POST, 0, 0);
		}

		_UpdateTrayIcon();
		_UpdateToolBar(TRUE); // Updates titlebar, too
	}
	else { _CallPlugins(KPM_OPENDB_POST, 1, 0); }

	NotifyUserActivity();
	_SetDefaultFocus();
}

void CPwSafeDlg::OnFileOpen()
{
	NotifyUserActivity();

	// if(m_bLocked == TRUE) return;

	m_bDisplayDialog = TRUE;

	if(_CallPlugins(KPM_FILE_OPEN_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	_OpenDatabase(NULL, NULL, NULL, NULL, FALSE, NULL, FALSE);
	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::_UpdateGuiToManager()
{
	m_mgr.m_dwLastSelectedGroupId = GetSelectedGroupId();

	HTREEITEM hGroup = m_cGroups.GetFirstVisibleItem();
	if(hGroup != NULL)
		m_mgr.m_dwLastTopVisibleGroupId = (DWORD)m_cGroups.GetItemData(hGroup);
	else m_mgr.m_dwLastTopVisibleGroupId = DWORD_MAX;

	DWORD dwIndex = _ListSelToEntryIndex((DWORD)m_cList.GetTopIndex());
	if(dwIndex != DWORD_MAX)
	{
		PW_ENTRY *pEn = m_mgr.GetEntry(dwIndex);
		ASSERT_ENTRY(pEn);
		if(pEn != NULL)
		{
			memcpy(m_mgr.m_aLastTopVisibleEntryUuid, pEn->uuid, 16);
		}
	}
	dwIndex = _ListSelToEntryIndex();
	if(dwIndex != DWORD_MAX)
	{
		PW_ENTRY *pEn2 = m_mgr.GetEntry(dwIndex);
		ASSERT_ENTRY(pEn2);
		if(pEn2 != NULL)
		{
			memcpy(m_mgr.m_aLastSelectedEntryUuid, pEn2->uuid, 16);
		}
	}
}

void CPwSafeDlg::_DeleteBackupEntries()
{
	DWORD idGroup;
	BOOL bUpd = FALSE;

	idGroup = m_mgr.GetGroupId(PWS_BACKUPGROUP);
	if((idGroup != 0) && (idGroup != DWORD_MAX)) { VERIFY(m_mgr.DeleteGroupById(idGroup, FALSE)); bUpd = TRUE; }

	idGroup = m_mgr.GetGroupId(PWS_BACKUPGROUP_SRC);
	if((idGroup != 0) && (idGroup != DWORD_MAX)) { VERIFY(m_mgr.DeleteGroupById(idGroup, FALSE)); bUpd = TRUE; }

	if(bUpd == TRUE)
	{
		_Groups_SaveView(TRUE);
		UpdateGroupList();
		_Groups_RestoreView();
		ShowEntryDetails(NULL);
	}
}

void CPwSafeDlg::OnFileSave()
{
	NotifyUserActivity();

	if(m_bFileOpen == FALSE) return;
	if(m_bFileReadOnly == TRUE) return;

	m_bDisplayDialog = TRUE;

	if(_CallPlugins(KPM_FILE_SAVE_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	GroupSyncStates(TRUE);

	if(m_strFile.IsEmpty())
	{
		OnFileSaveAs();
		m_bDisplayDialog = FALSE;
		return;
	}

	if(m_bHashValid == TRUE)
	{
		BYTE aNewHash[32];

		if(SHA256_HashFile(m_strFile, (BYTE *)aNewHash) == TRUE)
		{
			if(memcmp(aNewHash, m_aHashOfFile, 32) != 0)
			{
				CString str;
				str = TRL("The database file has been modified!");
				str += _T("\r\n\r\n");
				str += TRL("Most probably someone has changed the file while you were editing it.");
				str += _T("\r\n\r\n");
				str += TRL("Do you want to overwrite it?");
				if(MessageBox(str, TRL("Overwrite?"), MB_YESNO | MB_ICONQUESTION) == IDNO)
				{
					m_bDisplayDialog = FALSE;
					return;
				}
			}
		}
	}

	if(m_bDeleteBackupsOnSave == TRUE) _DeleteBackupEntries();
	_RemoveSearchGroup();
	_UpdateGuiToManager();

	_PreDatabaseWrite();

	CTaskbarListEx::SetProgressState(this->m_hWnd, TBPF_INDETERMINATE);

	BYTE vWrittenHash[32];
	const int nErr = m_mgr.SaveDatabase(m_strFile, &vWrittenHash[0]);

	CTaskbarListEx::SetProgressState(this->m_hWnd, TBPF_NOPROGRESS);

	if(nErr != PWE_SUCCESS)
	{
		CNewDialogsEx::ShowError(this->m_hWnd, nErr, PWFF_DATALOSS_WITHOUT_SAVE);
		m_bDisplayDialog = FALSE;
		return;
	}

	WU_FlushStorageBuffersEx(m_strFile, TRUE);

	// Update file contents hash
	m_bHashValid = SHA256_HashFile(m_strFile, (BYTE *)m_aHashOfFile);

	if((m_bHashValid == FALSE) || (memcmp(vWrittenHash, m_aHashOfFile, 32) != 0))
	{
		CNewDialogsEx::ShowError(this->m_hWnd, PWE_FILEERROR_VERIFY,
			PWFF_DATALOSS_WITHOUT_SAVE);
		m_bDisplayDialog = FALSE;
		return;
	}

	// if(m_bCreateBackupFileAfterSaving != FALSE)
	// {
	//	const std::basic_string<TCHAR> strBkRes = WU_CreateBackupFile(m_strFile);
	//	if(strBkRes.size() > 0)
	//		MessageBox(strBkRes.c_str(), PWM_PRODUCT_NAME_SHORT, MB_ICONWARNING | MB_OK);
	// }

	_CallPlugins(KPM_SAVEDB_POST, (LPARAM)(LPCTSTR)m_strFile, 0);

	m_strLastDb = m_strFile;
	m_bModified = FALSE;
	CKeySourcesPool::Set(GetShortestAbsolutePath(m_strFile), m_mgr.GetKeySource()); // Set or clear

	_UpdateToolBar(); // Updates titlebar, too
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnFileSaveAs()
{
	NotifyUserActivity();

	if(m_bFileOpen == FALSE) return;

	m_bDisplayDialog = TRUE;

	if(_CallPlugins(KPM_FILE_SAVEAS_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	GroupSyncStates(TRUE);

	CString strFilter = TRL("KeePass Password Safe Files");
	strFilter += _T(" (*.kdb)|*.kdb|");
	strFilter += TRL("All Files");
	strFilter += _T(" (*.*)|*.*||");

	DWORD dwFlags = (OFN_LONGNAMES | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
	dwFlags |= OFN_EXTENSIONDIFFERENT;
	// OFN_EXPLORER = 0x00080000, OFN_ENABLESIZING = 0x00800000
	dwFlags |= (0x00080000 | 0x00800000 | OFN_NOREADONLYRETURN);
	
	CFileDialog dlg(FALSE, _T("kdb"), _T("Database.kdb"), dwFlags, strFilter, this);
	if(dlg.DoModal() == IDOK)
	{
		// strFile = dlg.GetPathName();
		const FullPathName database((LPCTSTR)dlg.GetPathName());
		CString strFile = database.getFullPathName().c_str();

		if(m_bDeleteBackupsOnSave == TRUE) _DeleteBackupEntries();
		_RemoveSearchGroup();
		_UpdateGuiToManager();

		_PreDatabaseWrite();

		CTaskbarListEx::SetProgressState(this->m_hWnd, TBPF_INDETERMINATE);

		BYTE vWrittenHash[32];
		const int nErr = m_mgr.SaveDatabase(strFile, &vWrittenHash[0]);

		CTaskbarListEx::SetProgressState(this->m_hWnd, TBPF_NOPROGRESS);

		if(nErr != PWE_SUCCESS)
			CNewDialogsEx::ShowError(this->m_hWnd, nErr, PWFF_DATALOSS_WITHOUT_SAVE);
		else
		{
			WU_FlushStorageBuffersEx(strFile, TRUE);

			// Update file contents hash
			m_bHashValid = SHA256_HashFile(strFile, (BYTE *)m_aHashOfFile);

			if((m_bHashValid == FALSE) || (memcmp(vWrittenHash, m_aHashOfFile, 32) != 0))
			{
				CNewDialogsEx::ShowError(this->m_hWnd, PWE_FILEERROR_VERIFY,
					PWFF_DATALOSS_WITHOUT_SAVE);
				m_bDisplayDialog = FALSE;
				return;
			}

			// if(m_bCreateBackupFileAfterSaving != FALSE)
			// {
			//	const std::basic_string<TCHAR> strBkRes = WU_CreateBackupFile(strFile);
			//	if(strBkRes.size() > 0)
			//		MessageBox(strBkRes.c_str(), PWM_PRODUCT_NAME_SHORT, MB_ICONWARNING | MB_OK);
			// }

			_CallPlugins(KPM_SAVEDB_AS_POST, (LPARAM)(LPCTSTR)strFile, 0);

			if(m_bFileReadOnly == FALSE)
			{
				m_nFileRelockCountdown = -1;
				FileLock_Lock(m_strFile, FALSE); // Unlock the previous database file
			}

			m_strFile = strFile;
			m_strFileAbsolute = GetShortestAbsolutePath(m_strFile);

			FileLock_Lock(m_strFile, TRUE);
			m_nFileRelockCountdown = FL_TIME_RELOCK_AFTER * 60;

			m_bModified = FALSE;
			m_strLastDb = strFile;
			CKeySourcesPool::Set(m_strFileAbsolute, m_mgr.GetKeySource()); // Set or clear

			m_bFileReadOnly = FALSE;

			// _UpdateTitleBar(); // Updated by _UpdateToolBar()
		}
	}

	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnFileClose()
{
	NotifyUserActivity();

	m_bDisplayDialog = TRUE;

	if(_CallPlugins(KPM_FILE_CLOSE_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	GroupSyncStates(TRUE);

	if((m_bFileOpen == TRUE) && (m_bModified == TRUE))
	{
		int nRes;

		if(m_bAutoSaveDb == TRUE) nRes = IDYES;
		else
		{
			SetForegroundWindow();

			const bool bLockOrExit = ((m_bExiting == TRUE) || (m_bIsLocking == TRUE));
			const bool bDoVerification = ((m_bAutoSaveDb == FALSE) && bLockOrExit);

			CString strContent = TRL("The current database file has been modified");

			if(m_strFileAbsolute.GetLength() > 0)
			{
				strContent += _T(":\r\n");
				strContent += m_strFileAbsolute;
				// strContent += _T("\r\n\r\n");
				// strContent += TRL("Do you want to save the changes?");
			}
			else strContent += _T(".");

			CString strSaveText = TRL("Save all changes made to the database and close the file.");
			if(m_bExiting == TRUE) strSaveText = TRL("Save all changes made to the database and exit KeePass.");
			if(m_bIsLocking == TRUE) strSaveText = TRL("Save all changes made to the database and lock the KeePass workspace.");

			CString strDiscardText = TRL("Discard all changes made to the database and close the file.");
			if(m_bExiting == TRUE) strDiscardText = TRL("Discard all changes made to the database and exit KeePass.");
			if(m_bIsLocking == TRUE) strDiscardText = TRL("Discard all changes made to the database and lock the KeePass workspace.");

			CString strCancelText = TRL("Abort the current operation.");
			strCancelText += _T(" ");
			if(m_bIsLocking == TRUE) strCancelText += TRL("The KeePass workspace will not be locked.");
			else if(m_bExiting == TRUE) strCancelText += TRL("KeePass will not be closed.");
			else strCancelText += TRL("The file will not be closed.");

			CString strMain = TRL("Save database changes before closing the file?");
			if(m_bExiting == TRUE) strMain = TRL("Save database changes before exiting KeePass?");
			if(m_bIsLocking == TRUE) strMain = TRL("Save database changes before locking the workspace?");

			CVistaTaskDialog dlgTask(this->m_hWnd, AfxGetInstanceHandle(), true);
			dlgTask.SetWindowTitle(PWM_PRODUCT_NAME_SHORT);
			dlgTask.SetContent(strContent);
			dlgTask.SetMainInstruction(strMain);
			dlgTask.SetIcon(V_MTDI_QUESTION);

			if(bDoVerification)
				dlgTask.SetVerification(TRL("Automatically save when closing/locking the database"));

			dlgTask.AddButton(TRL("&Save"), strSaveText, IDYES);
			dlgTask.AddButton(TRL("&Discard changes"), strDiscardText, IDNO);
			dlgTask.AddButton(TRL("&Cancel"), strCancelText, IDCANCEL);

			BOOL bAutoSaveCB = FALSE;
			nRes = dlgTask.ShowDialog(bDoVerification ? &bAutoSaveCB : NULL);

			if(nRes < 0)
			{
				CString str = TRL("The current database file has been modified");

				if(m_strFileAbsolute.GetLength() > 0)
				{
					str += _T(":\r\n");
					str += m_strFileAbsolute;
				}
				else str += _T(".");

				str += _T("\r\n\r\n");
				str += TRL("Do you want to save the changes before closing?");

				nRes = MessageBox(str, TRL("KeePass - Save Before Close/Lock?"),
					MB_YESNOCANCEL | MB_ICONQUESTION);
			}
			else if(bDoVerification && (bAutoSaveCB != FALSE) && (nRes == IDYES))
				m_bAutoSaveDb = TRUE;
		}

		if(nRes == IDCANCEL) { m_bDisplayDialog = FALSE; return; }
		else if(nRes == IDYES)
		{
			OnFileSave();

			if((m_bModified == TRUE) && (m_bExiting == TRUE))
			{
				CString strMsg = TRL("The file couldn't be saved.");
				strMsg += _T("\r\n\r\n");
				strMsg += TRL("Maybe it's read-only or the storage media has been removed.");
				strMsg += _T("\r\n");
				strMsg += TRL("If you exit now, all changes to the current database will be lost.");
				strMsg += _T("\r\n\r\n");
				strMsg += TRL("Would you like to exit anyway?");

				int nRet = MessageBox(strMsg, TRL("Exit?"), MB_YESNO | MB_ICONQUESTION);
				if(nRet == IDYES) m_bModified = FALSE;
			}
		}
		else m_bModified = FALSE; // nRes == IDNO
	}

	if(m_bModified == TRUE) { m_bDisplayDialog = FALSE; return; }

	m_cQuickFind.ResetContent();
	ASSERT(m_cQuickFind.GetWindowTextLength() == 0);

	if(((m_bMenuExit == TRUE) || (m_bMiniMode == FALSE)) && (m_bClearClipOnDbClose == TRUE))
		ClearClipboardIfOwner();

	if(m_bFileReadOnly == FALSE)
	{
		m_nFileRelockCountdown = -1;
		FileLock_Lock(m_strFile, FALSE); // Unlock the database file
	}

	m_cList.DeleteAllItemsEx();
	m_cGroups.DeleteAllItemsEx();
	ShowEntryDetails(NULL);
	m_mgr.NewDatabase();
	m_mgr.ClearMasterKey(TRUE, TRUE);

	m_strFile.Empty(); m_strFileAbsolute.Empty();
	m_bFileOpen = FALSE;
	// m_bFileReadOnly = FALSE;
	m_bModified = FALSE;

	m_cList.EnableWindow(FALSE);
	m_cGroups.EnableWindow(FALSE);

	_DeleteTemporaryFiles();

	_UpdateTrayIcon();
	// _UpdateTitleBar(); // Updated by _UpdateToolBar()
	_UpdateToolBar();

	_CallPlugins(KPM_FILE_CLOSE_POST, 0, 0);
	m_bDisplayDialog = FALSE;

	m_btnTbAbout.SetFocus();
}

void CPwSafeDlg::OnSafeOptions()
{
	NotifyUserActivity();

	COptionsDlg dlg;
	BOOL bRebuildContextMenus = FALSE, bRestartNeeded = FALSE;
	BOOL bStartWithWindows = CPwSafeApp::GetStartWithWindows();

	m_bDisplayDialog = TRUE;

	dlg.m_pParentDlg = this;

	if(m_bWindowsNewLine == TRUE) dlg.m_nNewlineSequence = 0;
	else dlg.m_nNewlineSequence = 1;

	dlg.m_uClipboardSeconds = m_dwClipboardSecs - 1;
	dlg.m_nClipboardMethod = m_nClipboardMethod;
	dlg.m_bOpenLastDb = m_bOpenLastDb;
	dlg.m_bStartMinimized = m_bStartMinimized;
	dlg.m_bImgButtons = m_bImgButtons;
	dlg.m_bEntryGrid = m_bEntryGrid;
	dlg.m_bAutoSave = m_bAutoSaveDb;
	dlg.m_strFontSpec = m_strFontSpec;
	dlg.m_bLockOnMinimize = m_bLockOnMinimize;
	dlg.m_bMinimizeOnLock = m_bMinimizeOnLock;
	dlg.m_bMinimizeToTray = m_bMinimizeToTray;
	dlg.m_bCloseMinimizes = m_bCloseMinimizes;
	dlg.m_bLockAfterTime = ((m_nLockTimeDef != -1) ? TRUE : FALSE);

	if(m_nLockTimeDef != -1) dlg.m_nLockAfter = static_cast<UINT>(m_nLockTimeDef);
	else dlg.m_nLockAfter = 0;

	dlg.m_rgbRowHighlight = m_cList.GetRowColorEx();
	dlg.m_bColAutoSize = m_bColAutoSize;
	dlg.m_bDisableUnsafe = m_bDisableUnsafe;
	dlg.m_bRememberLast = m_bRememberLast;
	dlg.m_bUsePuttyForURLs = m_bUsePuttyForURLs;
	dlg.m_bSaveOnLATMod = m_bSaveOnLATMod;
	dlg.m_dwATHotKey = m_dwATHotKey;
	dlg.m_bAutoShowExpired = m_bAutoShowExpired;
	dlg.m_bAutoShowExpiredSoon = m_bAutoShowExpiredSoon;
	dlg.m_bStartWithWindows = bStartWithWindows;
	dlg.m_bBackupEntries = m_bBackupEntries;
	dlg.m_bSingleInstance = m_bCheckForInstance;
	dlg.m_bSecureEdits = m_bSecureEdits;
	dlg.m_bSingleClickTrayIcon = m_bSingleClickTrayIcon;
	dlg.m_dwDefaultExpire = m_dwDefaultExpire;
	dlg.m_bQuickFindIncBackup = m_bQuickFindIncBackup;
	dlg.m_bQuickFindIncExpired = m_bQuickFindIncExpired;
	dlg.m_bMinimizeBeforeAT = ((m_nAutoTypeMethod == ATM_MINIMIZE) ? TRUE : FALSE);
	dlg.m_bDeleteBackupsOnSave = m_bDeleteBackupsOnSave;
	dlg.m_bShowFullPath = m_bShowFullPath;
	dlg.m_bDisableAutoType = m_bDisableAutoType;
	dlg.m_bCopyURLs = m_bCopyURLs;
	dlg.m_bExitInsteadOfLockAT = m_bExitInsteadOfLockAT;
	dlg.m_bAllowSaveIfModifiedOnly = m_bAllowSaveIfModifiedOnly;
	dlg.m_bCheckForUpdate = m_bCheckForUpdate;
	dlg.m_bLockOnWinLock = m_bLockOnWinLock;
	dlg.m_bClearClipOnDbClose = m_bClearClipOnDbClose;
	dlg.m_bEnableRemoteCtrl = m_remoteControl.IsEnabled();
	dlg.m_bUseLocalTimeFormat = CPwSafeDlg::m_bUseLocalTimeFormat;
	dlg.m_bRegisterRestoreHotKey = m_bRegisterRestoreHotKey;
	dlg.m_bFocusResAfterQuickFind = m_bFocusResAfterQuickFind;
	dlg.m_bAlwaysAllowIpc = m_remoteControl.GetAlwaysAllowFullAccess();
	dlg.m_strDefaultAutoTypeSequence = m_strDefaultAutoTypeSequence;
	dlg.m_bAutoTypeIEFix = m_bAutoTypeIEFix;
	dlg.m_bDropToBackOnCopy = m_bDropToBackOnCopy;
	dlg.m_bSortAutoTypeSelItems = m_bSortAutoTypeSelItems;
	dlg.m_bDeleteTANsAfterUse = m_bDeleteTANsAfterUse;
	dlg.m_bUseTransactedFileWrites = m_bUseTransactedFileWrites;
	dlg.m_bRememberKeySources = CKeySourcesPool::GetEnabled();

	if(_CallPlugins(KPM_OPTIONS_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	if(dlg.DoModal() == IDOK)
	{
		m_bWindowsNewLine = ((dlg.m_nNewlineSequence == 0) ? TRUE : FALSE);
		m_dwClipboardSecs = dlg.m_uClipboardSeconds + 1;
		m_bOpenLastDb = dlg.m_bOpenLastDb;
		m_bStartMinimized = dlg.m_bStartMinimized;
		m_bImgButtons = dlg.m_bImgButtons;
		m_bEntryGrid = dlg.m_bEntryGrid;
		m_bAutoSaveDb = dlg.m_bAutoSave;
		m_bLockOnMinimize = dlg.m_bLockOnMinimize;
		m_bMinimizeOnLock = dlg.m_bMinimizeOnLock;
		m_bMinimizeToTray = dlg.m_bMinimizeToTray;
		m_bCloseMinimizes = dlg.m_bCloseMinimizes;
		if(dlg.m_rgbRowHighlight == 0xFF000000) dlg.m_rgbRowHighlight = RGB(238, 238, 255);
		m_cList.SetRowColorEx(dlg.m_rgbRowHighlight);
		m_bColAutoSize = dlg.m_bColAutoSize;
		m_bRememberLast = dlg.m_bRememberLast;
		m_bUsePuttyForURLs = dlg.m_bUsePuttyForURLs;
		m_bSaveOnLATMod = dlg.m_bSaveOnLATMod;
		m_dwATHotKey = dlg.m_dwATHotKey; // HotKey change is made by COptionsDlg
		m_bAutoShowExpired = dlg.m_bAutoShowExpired;
		m_bAutoShowExpiredSoon = dlg.m_bAutoShowExpiredSoon;
		m_bBackupEntries = dlg.m_bBackupEntries;
		m_bCheckForInstance = dlg.m_bSingleInstance;
		m_bSecureEdits = dlg.m_bSecureEdits;
		m_bSingleClickTrayIcon = dlg.m_bSingleClickTrayIcon;
		m_bQuickFindIncBackup = dlg.m_bQuickFindIncBackup;
		m_bQuickFindIncExpired = dlg.m_bQuickFindIncExpired;
		m_bDeleteBackupsOnSave = dlg.m_bDeleteBackupsOnSave;
		m_bShowFullPath = dlg.m_bShowFullPath;
		m_bDisableAutoType = dlg.m_bDisableAutoType;
		m_bExitInsteadOfLockAT = dlg.m_bExitInsteadOfLockAT;
		m_bAllowSaveIfModifiedOnly = dlg.m_bAllowSaveIfModifiedOnly;
		m_bCheckForUpdate = dlg.m_bCheckForUpdate;
		m_bLockOnWinLock = dlg.m_bLockOnWinLock;
		m_bClearClipOnDbClose = dlg.m_bClearClipOnDbClose;
		m_bFocusResAfterQuickFind = dlg.m_bFocusResAfterQuickFind;
		m_strDefaultAutoTypeSequence = dlg.m_strDefaultAutoTypeSequence;
		m_bAutoTypeIEFix = dlg.m_bAutoTypeIEFix;
		m_bDropToBackOnCopy = dlg.m_bDropToBackOnCopy;
		m_bSortAutoTypeSelItems = dlg.m_bSortAutoTypeSelItems;
		m_bDeleteTANsAfterUse = dlg.m_bDeleteTANsAfterUse;
		m_bUseTransactedFileWrites = dlg.m_bUseTransactedFileWrites;
		CKeySourcesPool::SetEnabled(dlg.m_bRememberKeySources);

		m_remoteControl.SetAlwaysAllowFullAccess(dlg.m_bAlwaysAllowIpc);
		m_remoteControl.EnableRemoteControl(dlg.m_bEnableRemoteCtrl);

		m_bRegisterRestoreHotKey = dlg.m_bRegisterRestoreHotKey;
		RegisterRestoreHotKey(m_bRegisterRestoreHotKey);

		m_nAutoTypeMethod = ((dlg.m_bMinimizeBeforeAT == TRUE) ? ATM_MINIMIZE : ATM_DROPBACK);

		if(dlg.m_bUseLocalTimeFormat != CPwSafeDlg::m_bUseLocalTimeFormat)
		{
			CPwSafeDlg::m_bUseLocalTimeFormat = dlg.m_bUseLocalTimeFormat;
			RefreshPasswordList();
		}

		if(dlg.m_bDefaultExpire == FALSE) m_dwDefaultExpire = 0;
		else
		{
			m_dwDefaultExpire = dlg.m_dwDefaultExpire;
			if(m_dwDefaultExpire > 2920) m_dwDefaultExpire = 2920;
		}

		if(dlg.m_bStartWithWindows != bStartWithWindows)
		{
			if(CPwSafeApp::SetStartWithWindows(dlg.m_bStartWithWindows) == FALSE)
				MessageBox(TRL("Unable to create/remove the auto-start key of KeePass in the Windows registry!"),
					PWM_PRODUCT_NAME_SHORT, MB_ICONWARNING | MB_OK);
		}

		if(dlg.m_nClipboardMethod != m_nClipboardMethod)
		{
			ClearClipboardIfOwner();
			m_nClipboardMethod = dlg.m_nClipboardMethod;
		}

		if(dlg.m_bDisableUnsafe != m_bDisableUnsafe)
		{
			bRestartNeeded = TRUE;
			m_bDisableUnsafe = dlg.m_bDisableUnsafe;
		}

		if(dlg.m_bLockAfterTime == FALSE) m_nLockTimeDef = -1;
		else m_nLockTimeDef = static_cast<long>(dlg.m_nLockAfter);

		if(dlg.m_strFontSpec != m_strFontSpec)
		{
			_ParseSpecAndSetFont(dlg.m_strFontSpec);
		}

		if(dlg.m_bCopyURLs != m_bCopyURLs)
		{
			m_bCopyURLs = dlg.m_bCopyURLs;
			bRebuildContextMenus = TRUE;
		}

		NewGUI_SetImgButtons(m_bImgButtons);
		_SetListParameters();

		_CallPlugins(KPM_OPTIONS_POST, 0, 0);

		ProcessResize();

		if(bRebuildContextMenus == TRUE) RebuildContextMenus();

		if(bRestartNeeded == TRUE)
		{
			CString str;

			str = TRL("You have changed an option that requires restarting KeePass to get active.");
			str += _T("\r\n\r\n");
			str += TRL("Do you wish to restart KeePass now?");
			int nRet = MessageBox(str, TRL("Restart KeePass?"), MB_YESNO | MB_ICONQUESTION);
			if(nRet == IDYES)
			{
				m_bRestartApplication = TRUE;
				m_bDisplayDialog = FALSE;
				OnFileExit(); // Also calls SaveOptions!
				return;
			}
		}

		SaveOptions();
	}

	NotifyUserActivity();
	m_ullLastListParams = 0; // Invalidate
	_UpdateToolBar(TRUE);

	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::RestartApplication()
{
	// TCHAR szFile[1024];
	// GetModuleFileName(NULL, szFile, 1024);
	std_string strFile = Executable::instance().getFullPathName();

	if(TWinExec(strFile.c_str(), 0) <= 31)
	{
		CString str;
		str = TRL("Application cannot be restarted automatically!");
		str += _T("\r\n\r\n");
		str += TRL("Please restart KeePass manually.");
		MessageBox(str, TRL("Loading error"), MB_OK | MB_ICONWARNING);
	}
}

void CPwSafeDlg::OnSafeRemoveGroup()
{
	CDWordArray aGroupIds;

	NotifyUserActivity();

	DWORD dwGroupId = GetSelectedGroupId();
	ASSERT(dwGroupId != DWORD_MAX); if(dwGroupId == DWORD_MAX) return;

	m_bDisplayDialog = TRUE;

	if(_CallPlugins(KPM_GROUP_REMOVE_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	DWORD dwGroupIndex = m_mgr.GetGroupByIdN(dwGroupId);
	PW_GROUP *p = m_mgr.GetGroup(dwGroupIndex);
	ASSERT(p != NULL); if(p == NULL) { m_bDisplayDialog = FALSE; return; }
	const USHORT usTopLevel = p->usLevel;

	while(true)
	{
		aGroupIds.Add(dwGroupId);

		++dwGroupIndex;
		if(dwGroupIndex == m_mgr.GetNumberOfGroups()) break;

		p = m_mgr.GetGroup(dwGroupIndex);
		ASSERT(p != NULL); if(p == NULL) break;

		if(p->usLevel <= usTopLevel) break;

		dwGroupId = p->uGroupId;
	}

	CVistaTaskDialog dlgTask(this->m_hWnd, AfxGetInstanceHandle(), false);
	dlgTask.SetContent(TRL("Deleting a group will delete all items and subgroups in that group, too."));
	dlgTask.SetMainInstruction(TRL("Are you sure you want to delete this group?"));
	dlgTask.SetWindowTitle(PWM_PRODUCT_NAME_SHORT);
	dlgTask.SetIcon(V_MTDI_QUESTION);
	dlgTask.AddButton(TRL("&Delete"), NULL, IDOK);
	dlgTask.AddButton(TRL("&Cancel"), NULL, IDCANCEL);
	int nMsg = dlgTask.ShowDialog();

	if(nMsg < 0) // No task dialog support?
	{
		CString str;
		str = TRL("Deleting a group will delete all items and subgroups in that group, too.");
		str += _T("\r\n\r\n");
		str += TRL("Are you sure you want to delete this group?");
		nMsg = MessageBox(str, TRL("Delete Group Confirmation"), MB_ICONQUESTION | MB_YESNO);
	}

	if((nMsg == IDOK) || (nMsg == IDYES))
	{
		const INT_PTR nCount = aGroupIds.GetSize();
		for(INT_PTR i = 0; i < nCount; ++i)
		{
			VERIFY(m_mgr.DeleteGroupById(aGroupIds.GetAt(nCount - i - 1), m_bBackupEntries));
		}
		aGroupIds.RemoveAll();

		_Groups_SaveView(FALSE);
		UpdateGroupList();
		_Groups_RestoreView();
		m_cGroups.SelectItem(m_cGroups.GetRootItem());
		UpdatePasswordList();

		m_bModified = TRUE;
	}

	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnFileChangeMasterPw()
{
	NotifyUserActivity();

	if(m_bFileOpen == FALSE) return;

	if(m_bForceAllowChangeMasterKey == FALSE)
	{
		if(IsUnsafeAllowed(this->m_hWnd) == FALSE) return;
	}

	if(_CallPlugins(KPM_FILE_CHANGE_MASTER_KEY_PRE, 0, 0) == FALSE) return;

	_ChangeMasterKey(NULL, FALSE);

	_UpdateToolBar();
}

void CPwSafeDlg::OnUpdateFileSave(CCmdUI* pCmdUI)
{
	if((m_bFileOpen == TRUE) && (m_bFileReadOnly == FALSE))
	{
		if(m_bAllowSaveIfModifiedOnly == FALSE) pCmdUI->Enable(TRUE);
		else pCmdUI->Enable(m_bModified);
	}
	else pCmdUI->Enable(FALSE);
}

void CPwSafeDlg::OnUpdateFileSaveAs(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnUpdateFileChangeMasterPw(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnUpdateFileClose(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnUpdatePwlistCopyPw(CCmdUI* pCmdUI)
{
	bool b = ((m_dwLastFirstSelectedItem != DWORD_MAX) && (m_dwLastNumSelectedItems == 1));

	if((m_dwLastEntryIndex != DWORD_MAX) && (m_dwLastEntryIndex < m_mgr.GetNumberOfEntries()))
	{
		const PW_ENTRY *p = m_mgr.GetEntry(m_dwLastEntryIndex);
		if(p != NULL) b &= (p->uPasswordLen > 0);
	}

	pCmdUI->Enable(b ? TRUE : FALSE);
}

void CPwSafeDlg::OnUpdatePwlistCopyUser(CCmdUI* pCmdUI)
{
	bool b = ((m_dwLastFirstSelectedItem != DWORD_MAX) && (m_dwLastNumSelectedItems == 1));

	if((m_dwLastEntryIndex != DWORD_MAX) && (m_dwLastEntryIndex < m_mgr.GetNumberOfEntries()))
	{
		const PW_ENTRY *p = m_mgr.GetEntry(m_dwLastEntryIndex);
		if(p != NULL) b &= (p->pszUserName[0] != 0);
	}

	pCmdUI->Enable(b ? TRUE : FALSE);
}

void CPwSafeDlg::OnUpdatePwlistDelete(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(((m_dwLastFirstSelectedItem != DWORD_MAX) ? TRUE : FALSE));
}

void CPwSafeDlg::OnUpdatePwlistEdit(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((((m_dwLastFirstSelectedItem != DWORD_MAX) && (m_dwLastNumSelectedItems == 1)) ? TRUE : FALSE));
}

void CPwSafeDlg::OnUpdatePwlistVisitUrl(CCmdUI* pCmdUI)
{
	bool b = true;

	if(m_bCopyURLs == FALSE)
		b &= ((m_dwLastFirstSelectedItem != DWORD_MAX) && (m_dwLastNumSelectedItems >= 1));
	else
		b &= ((m_dwLastFirstSelectedItem != DWORD_MAX) && (m_dwLastNumSelectedItems == 1));

	if((m_dwLastNumSelectedItems == 1) && (m_dwLastEntryIndex != DWORD_MAX) &&
		(m_dwLastEntryIndex < m_mgr.GetNumberOfEntries()))
	{
		const PW_ENTRY *p = m_mgr.GetEntry(m_dwLastEntryIndex);
		if(p != NULL) b &= (p->pszURL[0] != 0);
	}

	pCmdUI->Enable(b ? TRUE : FALSE);
}

void CPwSafeDlg::OnUpdateSafeRemoveGroup(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((m_hLastSelectedGroup != NULL) ? TRUE : FALSE);
}

void CPwSafeDlg::OnUpdateSafeAddGroup(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnUpdatePwlistAdd(CCmdUI* pCmdUI)
{
	BOOL b = m_bFileOpen;
	b &= ((m_mgr.GetNumberOfGroups() > 0) ? TRUE : FALSE);
	pCmdUI->Enable(b);
}

#define LCL_CHKOPT_PARAM_COUNT 19

BOOL CPwSafeDlg::GetExportOptions(PWEXPORT_OPTIONS *pOptions,
	CPwExport *pPwExport, BOOL bPrinting)
{
	NotifyUserActivity();

	ASSERT(pOptions != NULL); if(pOptions == NULL) return FALSE;
	ASSERT(pPwExport != NULL); if(pPwExport == NULL) return FALSE;

	m_bDisplayDialog = TRUE;

	*pOptions = pPwExport->m_aDefaults[pPwExport->m_nFormat];

	CCheckOptionsDlg dlg;
	CHKOPT_PARAM pa[LCL_CHKOPT_PARAM_COUNT];

	ZeroMemory(pa, sizeof(CHKOPT_PARAM) * LCL_CHKOPT_PARAM_COUNT);

	DWORD n = DWORD_MAX;

	pa[++n].lpString = TRL("Options");
	pa[n].nIcon = 9;

	if(bPrinting == FALSE)
	{
		pa[++n].lpString = TRL("Encode/replace newline characters by '\\n'");
		pa[n].pbValue = &pOptions->bEncodeNewlines;
	}

	if(bPrinting == TRUE)
		pa[++n].lpString = TRL("Print backup entries (entries in the 'Backup' group)");
	else
		pa[++n].lpString = TRL("Export backup entries (entries in the 'Backup' group)");
	pa[n].pbValue = &pOptions->bExportBackups;

	pa[++n].lpString = _T("");
	pa[n].nIcon = 0;

	if(bPrinting == TRUE) pa[++n].lpString = TRL("Fields to print");
	else pa[++n].lpString = TRL("Fields to export");
	pa[n].nIcon = 8;

	pa[++n].lpString = TRL("Password Groups");
	pa[n].pbValue = &pOptions->bGroup;
	pa[++n].lpString = TRL("Group Tree");
	pa[n].pbValue = &pOptions->bGroupTree;
	pa[++n].lpString = TRL("Title");
	pa[n].pbValue = &pOptions->bTitle;
	pa[++n].lpString = TRL("User Name");
	pa[n].pbValue = &pOptions->bUserName;

	if(m_bDisallowPrintingPasswords == FALSE)
	{
		pa[++n].lpString = TRL("Password");
		pa[n].pbValue = &pOptions->bPassword;
	}
	else pOptions->bPassword = FALSE;
	
	pa[++n].lpString = TRL("URL");
	pa[n].pbValue = &pOptions->bURL;
	pa[++n].lpString = TRL("Notes");
	pa[n].pbValue = &pOptions->bNotes;
	pa[++n].lpString = TRL("Creation Time");
	pa[n].pbValue = &pOptions->bCreationTime;
	pa[++n].lpString = TRL("Last Access");
	pa[n].pbValue = &pOptions->bLastAccTime;
	pa[++n].lpString = TRL("Last Modification");
	pa[n].pbValue = &pOptions->bLastModTime;
	pa[++n].lpString = TRL("Expires");
	pa[n].pbValue = &pOptions->bExpireTime;
	pa[++n].lpString = TRL("Icon");
	pa[n].pbValue = &pOptions->bImage;
	pa[++n].lpString = TRL("UUID");
	pa[n].pbValue = &pOptions->bUUID;
	pa[++n].lpString = TRL("Attachment");
	pa[n].pbValue = &pOptions->bAttachment;

	if(bPrinting == TRUE)
	{
		dlg.m_strTitle = TRL("Print Options");
		dlg.m_strDescription = TRL("Here you can customize the printed sheet.");
	}
	else
	{
		dlg.m_strTitle = TRL("Export Options");
		dlg.m_strDescription = TRL("Here you can customize the exported files.");
	}

	if(bPrinting == TRUE)
	{
		ASSERT((n == (LCL_CHKOPT_PARAM_COUNT - 2)) || (n == (LCL_CHKOPT_PARAM_COUNT - 3)));
	}
	else { ASSERT(n == (LCL_CHKOPT_PARAM_COUNT - 1)); }
	dlg.m_dwNumParams = n + 1;
	dlg.m_pParams = (CHKOPT_PARAM *)pa;

	if(dlg.DoModal() == IDOK) { m_bDisplayDialog = FALSE; return TRUE; }
	m_bDisplayDialog = FALSE;
	return FALSE;
}

CString CPwSafeDlg::GetExportFile(int nFormat, LPCTSTR lpBaseFileName, BOOL bFixFileName)
{
	if(m_bFileOpen == FALSE) return CString("");

	m_bDisplayDialog = TRUE;

	LPCTSTR lp = _T("exp");
	if(nFormat == PWEXP_TXT) lp = _T("txt");
	else if(nFormat == PWEXP_HTML) lp = _T("html");
	else if(nFormat == PWEXP_XML) lp = _T("xml");
	else if(nFormat == PWEXP_CSV) lp = _T("csv");
	else if(nFormat == PWEXP_KEEPASS) lp = _T("kdb");
	else { ASSERT(FALSE); }

	CString strSample;
	if(lpBaseFileName == NULL) strSample = _T("Export");
	else
	{
		strSample = lpBaseFileName;

		if(bFixFileName == TRUE)
		{
			strSample.Replace(_T('*'), _T('-')); strSample.Replace(_T('?'), _T('-'));
			strSample.Replace(_T(','), _T('-')); strSample.Replace(_T(';'), _T('-'));
			strSample.Replace(_T(':'), _T('-')); strSample.Replace(_T('|'), _T('-'));
			strSample.Replace(_T('<'), _T('-')); strSample.Replace(_T('>'), _T('-'));
			strSample.Replace(_T('/'), _T('-')); strSample.Replace(_T('\"'), _T('-'));
			strSample.Replace(_T('\\'), _T('-'));
		}
	}

	strSample += _T(".");
	strSample += lp;

	CString strFilter = TRL("All Files");
	strFilter += _T(" (*.*)|*.*||");

	const DWORD dwFlags = (OFN_LONGNAMES | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT |
		OFN_EXTENSIONDIFFERENT | OFN_EXPLORER | OFN_ENABLESIZING | OFN_NOREADONLYRETURN);

	CFileDialog dlg(FALSE, lp, strSample, dwFlags, strFilter, this);
	if(dlg.DoModal() == IDOK) { m_bDisplayDialog = FALSE; return dlg.GetPathName(); }

	strSample.Empty();
	m_bDisplayDialog = FALSE;
	return strSample;
}

void CPwSafeDlg::OnExportTxt()
{
	NotifyUserActivity();
	CPwExport cExp; PWEXPORT_OPTIONS pwo;
	CString strFile;
	if(m_bFileOpen == FALSE) return;
	if(m_bForceAllowExport == FALSE)
	{
		if(IsUnsafeAllowed(this->m_hWnd) == FALSE) return;
	}
	cExp.SetManager(&m_mgr);
	cExp.SetNewLineSeq(m_bWindowsNewLine);
	cExp.SetFormat(PWEXP_TXT);
	strFile = GetExportFile(PWEXP_TXT, CsFileOnly(&m_strFile), FALSE);
	if(strFile.GetLength() != 0)
		if(GetExportOptions(&pwo, &cExp, FALSE) == TRUE)
			cExp.ExportAll(strFile, &pwo, NULL);
}

void CPwSafeDlg::OnExportHtml()
{
	NotifyUserActivity();
	CPwExport cExp; PWEXPORT_OPTIONS pwo;
	CString strFile;
	if(m_bFileOpen == FALSE) return;
	if(m_bForceAllowExport == FALSE)
	{
		if(IsUnsafeAllowed(this->m_hWnd) == FALSE) return;
	}
	cExp.SetManager(&m_mgr);
	cExp.SetNewLineSeq(m_bWindowsNewLine);
	cExp.SetFormat(PWEXP_HTML);
	strFile = GetExportFile(PWEXP_HTML, CsFileOnly(&m_strFile), FALSE);
	if(strFile.GetLength() != 0)
		if(GetExportOptions(&pwo, &cExp, FALSE) == TRUE)
			cExp.ExportAll(strFile, &pwo, NULL);
}

void CPwSafeDlg::OnExportXml()
{
	NotifyUserActivity();
	CPwExport cExp; PWEXPORT_OPTIONS pwo;
	CString strFile;
	if(m_bFileOpen == FALSE) return;
	if(m_bForceAllowExport == FALSE)
	{
		if(IsUnsafeAllowed(this->m_hWnd) == FALSE) return;
	}
	cExp.SetManager(&m_mgr);
	cExp.SetNewLineSeq(m_bWindowsNewLine);
	cExp.SetFormat(PWEXP_XML);
	strFile = GetExportFile(PWEXP_XML, CsFileOnly(&m_strFile), FALSE);
	if(strFile.GetLength() != 0)
		if(GetExportOptions(&pwo, &cExp, FALSE) == TRUE)
			cExp.ExportAll(strFile, &pwo, NULL);
}

void CPwSafeDlg::OnExportCsv()
{
	NotifyUserActivity();
	CPwExport cExp; PWEXPORT_OPTIONS pwo;
	CString strFile;
	if(m_bFileOpen == FALSE) return;
	if(m_bForceAllowExport == FALSE)
	{
		if(IsUnsafeAllowed(this->m_hWnd) == FALSE) return;
	}
	cExp.SetManager(&m_mgr);
	cExp.SetNewLineSeq(m_bWindowsNewLine);
	cExp.SetFormat(PWEXP_CSV);
	strFile = GetExportFile(PWEXP_CSV, CsFileOnly(&m_strFile), FALSE);
	if(strFile.GetLength() != 0)
		if(GetExportOptions(&pwo, &cExp, FALSE) == TRUE)
			cExp.ExportAll(strFile, &pwo, NULL);
}

void CPwSafeDlg::OnUpdateExportTxt(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnUpdateExportHtml(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnUpdateExportXml(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnUpdateExportCsv(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::_PrintGroup(DWORD dwGroupId)
{
	NotifyUserActivity();

	if(m_bFileOpen == FALSE) return;
	if(m_bForceAllowPrinting == FALSE)
	{
		if(IsUnsafeAllowed(this->m_hWnd) == FALSE) return;
	}

	CPwExport cExp;
	cExp.SetManager(&m_mgr);
	cExp.SetNewLineSeq(m_bWindowsNewLine);
	cExp.SetFormat(PWEXP_HTML);

	PWEXPORT_OPTIONS pwo;
	if(GetExportOptions(&pwo, &cExp, TRUE) == FALSE) return;

	std::basic_string<TCHAR> tszFile = WU_GetTempFile(_T(".html"));

	const BOOL bRet = cExp.ExportGroup(tszFile.c_str(), dwGroupId, &pwo, NULL);
	if(bRet == FALSE)
	{
		MessageBox(TRL("Cannot open temporary file for printing!"),
			PWM_PRODUCT_NAME_SHORT, MB_OK | MB_ICONWARNING);
		return;
	}

	ShellExecute(m_hWnd, _T("print"), tszFile.c_str(), NULL, NULL, SW_SHOW);

	m_vTempFiles.push_back(tszFile);
}

void CPwSafeDlg::OnFilePrint()
{
	NotifyUserActivity();
	if(_CallPlugins(KPM_FILE_PRINT_PRE, 0, 0) == FALSE) return;
	_PrintGroup(DWORD_MAX); // _PrintGroup handles everything, like unsafe check, etc.
}

void CPwSafeDlg::OnUpdateFilePrint(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnExtrasGenPw()
{
	NotifyUserActivity();

	CPwGeneratorExDlg dlg;
	DWORD dwGroupId = GetSelectedGroupId();
	BYTE aUuid[16];

	m_bDisplayDialog = TRUE;

	if(_CallPlugins(KPM_GEN_PASSWORD_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	if((m_bFileOpen == FALSE) || (dwGroupId == DWORD_MAX))
	{
		dlg.InitEx(0, m_bPasswordStars, m_bTrayed);
		dlg.DoModal();
	}
	else // m_bFileOpen == TRUE
	{
		dlg.InitEx(1, m_bPasswordStars, m_bTrayed);

		if(dlg.DoModal() == IDOK)
		{
			LPTSTR lpPassword = dlg.GetGeneratedPassword();
			PW_TIME tNow;
			PW_ENTRY pwTemplate;

			_GetCurrentPwTime(&tNow);
			memset(&pwTemplate, 0, sizeof(PW_ENTRY));
			pwTemplate.pszAdditional = _T("");
			pwTemplate.pszPassword = ((lpPassword != NULL) ? lpPassword : _T(""));
			pwTemplate.pszTitle = _T("");
			pwTemplate.pszURL = _T("");
			pwTemplate.pszUserName = _T("");
			pwTemplate.tCreation = tNow;
			m_mgr.GetNeverExpireTime(&pwTemplate.tExpire);
			pwTemplate.tLastAccess = tNow;
			pwTemplate.tLastMod = tNow;
			pwTemplate.uGroupId = dwGroupId;
			pwTemplate.uImageId = 0;
			pwTemplate.uPasswordLen = 0;
			pwTemplate.pszBinaryDesc = _T("");

			if(m_mgr.AddEntry(&pwTemplate) == TRUE)
			{
				PW_ENTRY *pNew = m_mgr.GetLastEditedEntry(); ASSERT_ENTRY(pNew);
				DWORD dwListIndex;
				memcpy(aUuid, pNew->uuid, 16);

				_SortListIfAutoSort();
				if(m_nAutoSort == 0) UpdatePasswordList();

				dwListIndex = _EntryUuidToListPos(aUuid);

				m_cList.EnsureVisible(static_cast<int>(dwListIndex), FALSE);
				m_cList.FocusItem(static_cast<int>(dwListIndex), TRUE);
			}

			CSecureEditEx::DeletePassword(lpPassword); lpPassword = NULL;

			m_bModified = TRUE;
		}
	}

	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnSafeModifyGroup()
{
	NotifyUserActivity();

	CString strGroupName;
	CAddGroupDlg dlg;
	DWORD dwGroupId = GetSelectedGroupId();
	DWORD dwGroup;
	PW_GROUP *p;
	PW_GROUP pwTemplate;
	PW_TIME tNow;

	if(dwGroupId == DWORD_MAX) return;

	p = m_mgr.GetGroupById(dwGroupId);
	ASSERT(p != NULL);
	if(p == NULL) return;
	dwGroup = m_mgr.GetGroupByIdN(dwGroupId);
	ASSERT(dwGroup != DWORD_MAX);
	if(dwGroup == DWORD_MAX) return;

	m_bDisplayDialog = TRUE;

	dlg.m_nIconId = (int)p->uImageId;
	dlg.m_strGroupName = p->pszGroupName;
	dlg.m_pParentImageList = &m_ilIcons;
	dlg.m_bEditMode = TRUE;

	if(_CallPlugins(KPM_GROUP_MODIFY_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	if(dlg.DoModal() == IDOK)
	{
		_GetCurrentPwTime(&tNow);
		pwTemplate = *p; // Copy previous standard values like group ID, etc.
		pwTemplate.tLastAccess = tNow; // Update times
		pwTemplate.tLastMod = tNow;
		pwTemplate.pszGroupName = const_cast<LPTSTR>((LPCTSTR)dlg.m_strGroupName);
		pwTemplate.uImageId = (DWORD)dlg.m_nIconId;

		m_mgr.SetGroup(dwGroup, &pwTemplate);

		_Groups_SaveView();
		UpdateGroupList();
		_Groups_RestoreView();

		UpdatePasswordList();
		m_cList.SetFocus();

		dlg.m_strGroupName.Empty();

		m_bModified = TRUE;
	}
	else
		_TouchGroup(dwGroupId, FALSE);

	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdateSafeModifyGroup(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((m_hLastSelectedGroup != NULL) ? TRUE : FALSE);
}

void CPwSafeDlg::OnPwlistFind()
{
	NotifyUserActivity();
	if(_CallPlugins(KPM_PWLIST_FIND_PRE, 0, 0) == FALSE) return;
	_Find(DWORD_MAX);
}

#define CPSDFM_CONTINUE { cnt = dw + 1; if(cnt >= dwMaxItems) break; continue; }

void CPwSafeDlg::_Find(DWORD dwFindGroupId)
{
	CFindInDbDlg dlg;

	if(m_bFileOpen == FALSE) return;

	const DWORD dwMaxItems = m_mgr.GetNumberOfEntries();
	if(dwMaxItems == 0) return; // Nothing to search for

	m_bDisplayDialog = TRUE;

	DWORD dwFindGroupIndex;
	if(dwFindGroupId != DWORD_MAX) dwFindGroupIndex = m_mgr.GetGroupByIdN(dwFindGroupId);
	else dwFindGroupIndex = DWORD_MAX;

	if(dwFindGroupIndex != DWORD_MAX)
	{
		const PW_GROUP *pg = m_mgr.GetGroup(dwFindGroupIndex);
		if(pg != NULL) dlg.m_lpGroupName = pg->pszGroupName;
	}

	if(dlg.DoModal() == IDOK)
	{
		m_cList.DeleteAllItemsEx();
		m_bTANsOnly = TRUE;

		PW_TIME tNow;
		_GetCurrentPwTime(&tNow);

		DWORD dwFlags = 0;
		if(dlg.m_bTitle != FALSE)      dwFlags |= PWMF_TITLE;
		if(dlg.m_bUserName != FALSE)   dwFlags |= PWMF_USER;
		if(dlg.m_bURL != FALSE)        dwFlags |= PWMF_URL;
		if(dlg.m_bPassword != FALSE)   dwFlags |= PWMF_PASSWORD;
		if(dlg.m_bAdditional != FALSE) dwFlags |= PWMF_ADDITIONAL;
		if(dlg.m_bGroupName != FALSE)  dwFlags |= PWMF_GROUPNAME;
		if(dlg.m_bRegex != FALSE)      dwFlags |= PWMS_REGEX;

		// Create the search group if it doesn't exist already
		DWORD dwGroupId = m_mgr.GetGroupId(PWS_SEARCHGROUP);
		if(dwGroupId == DWORD_MAX)
		{
			PW_GROUP pwTemplate;
			pwTemplate.pszGroupName = const_cast<LPTSTR>(PWS_SEARCHGROUP);
			pwTemplate.tCreation = tNow; m_mgr.GetNeverExpireTime(&pwTemplate.tExpire);
			pwTemplate.tLastAccess = tNow; pwTemplate.tLastMod = tNow;
			pwTemplate.uGroupId = 0; // 0 = create new group ID
			pwTemplate.uImageId = 40; // 40 = 'search' icon
			pwTemplate.usLevel = 0; pwTemplate.dwFlags = 0;

			VERIFY(m_mgr.AddGroup(&pwTemplate));
			dwGroupId = m_mgr.GetGroupId(PWS_SEARCHGROUP);
		}
		ASSERT((dwGroupId != DWORD_MAX) && (dwGroupId != 0));
		if((dwGroupId == DWORD_MAX) || (dwGroupId == 0)) { m_bDisplayDialog = FALSE; return; }

		_UpdateCachedGroupIDs();

		DWORD cnt = 0;
		PW_ENTRY *pwFirst = NULL;

		while(true)
		{
			const DWORD dw = m_mgr.FindEx(dlg.m_strFind, dlg.m_bCaseSensitive, dwFlags, cnt);

			if(dw == DWORD_MAX) break;
			else
			{
				PW_ENTRY *p = m_mgr.GetEntry(dw);
				ASSERT_ENTRY(p);
				if(p == NULL) break;

				if(p->uGroupId != dwGroupId) // Not in 'Search Results' group
				{
					const DWORD dwGroupInx = m_mgr.GetGroupByIdN(p->uGroupId);
					ASSERT(dwGroupInx != DWORD_MAX);

					// Only accept entries that are stored in the specified group
					if((dwFindGroupIndex == DWORD_MAX) || (dwFindGroupIndex == dwGroupInx))
					{
						if(dlg.m_bExcludeBackups != FALSE)
						{
							if(_tcscmp(m_mgr.GetGroup(dwGroupInx)->pszGroupName, PWS_BACKUPGROUP) == 0)
								CPSDFM_CONTINUE;

							if(_tcscmp(m_mgr.GetGroup(dwGroupInx)->pszGroupName, PWS_BACKUPGROUP_SRC) == 0)
								CPSDFM_CONTINUE;
						}

						if(dlg.m_bExcludeExpired != FALSE)
						{
							if(_pwtimecmp(&tNow, &p->tExpire) > 0)
								CPSDFM_CONTINUE;
						}

						// The entry could get reallocated by AddEntry, therefore
						// save it to a local CString object
						// m_mgr.UnlockEntryPassword(p);
						// CString strTemp = p->pszPassword;
						// m_mgr.LockEntryPassword(p);

						_List_SetEntry(m_cList.GetItemCount(), p, TRUE, &tNow);

						if(pwFirst == NULL) pwFirst = p;

						// EraseCString(&strTemp); // Destroy the plaintext password
					}
				}
			}

			cnt = dw + 1;
			if(cnt >= dwMaxItems) break;
		}

		_Groups_SaveView();
		UpdateGroupList();
		_Groups_RestoreView();
		HTREEITEM hSelect = _GroupIdToHTreeItem(dwGroupId);
		if(hSelect != NULL)
		{
			m_cGroups.EnsureVisible(hSelect);
			m_cGroups.SelectItem(hSelect);
		}

		if(m_cList.GetItemCount() == 0) m_bTANsOnly = FALSE; // Use report list view
		AdjustPwListMode();

		// m_bModified = TRUE;
		if((m_cList.GetItemCount() > 0) && (pwFirst != NULL))
		{
			m_cList.FocusItem(0, TRUE);
			ShowEntryDetails(pwFirst);
		}
		else ShowEntryDetails(NULL);

		m_cList.SetFocus();
	}

	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdatePwlistFind(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen && (m_mgr.GetNumberOfEntries() != 0));
}

void CPwSafeDlg::OnPwlistFindInGroup()
{
	NotifyUserActivity();
	if(_CallPlugins(KPM_PWLIST_FIND_IN_GROUP_PRE, 0, 0) == FALSE) return;
	const DWORD dwCurGroup = GetSelectedGroupId();
	if(dwCurGroup == DWORD_MAX) return;
	_Find(dwCurGroup);
}

void CPwSafeDlg::OnUpdatePwlistFindInGroup(CCmdUI* pCmdUI)
{
	if((m_bFileOpen == FALSE) || (m_hLastSelectedGroup == NULL))
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	DWORD dwGroupId = (DWORD)m_cGroups.GetItemData(m_hLastSelectedGroup);
	DWORD dwRefId = m_mgr.GetGroupId(PWS_SEARCHGROUP);
	BOOL bEnable = ((dwGroupId != dwRefId) ? TRUE : FALSE);
	pCmdUI->Enable(bEnable && (m_mgr.GetNumberOfEntries() != 0));
}

void CPwSafeDlg::OnPwlistDuplicate()
{
	NotifyUserActivity();

	if(_CallPlugins(KPM_DUPLICATE_ENTRY_PRE, 0, 0) == FALSE) return;

	PW_TIME tNow;
	_GetCurrentPwTime(&tNow);

	std::vector<std::vector<BYTE> > vUuids;
	const int nItemCount = m_cList.GetItemCount();

	for(int i = 0; i < nItemCount; ++i)
	{
		const UINT uState = m_cList.GetItemState(i, LVIS_SELECTED);

		if((uState & LVIS_SELECTED) != 0)
		{
			const DWORD dwEntryIndex = _ListSelToEntryIndex(static_cast<DWORD>(i));
			ASSERT(dwEntryIndex != DWORD_MAX); if(dwEntryIndex == DWORD_MAX) continue;

			PW_ENTRY *p = m_mgr.GetEntry(dwEntryIndex);
			ASSERT_ENTRY(p); if(p == NULL) continue;

			PW_ENTRY pwTemplate = *p; // Duplicate

			memset(pwTemplate.uuid, 0, 16); // We need a new UUID
			pwTemplate.tCreation = tNow; // Set new times
			pwTemplate.tLastMod = tNow;
			pwTemplate.tLastAccess = tNow;

			std::basic_string<TCHAR> strTitle = pwTemplate.pszTitle;
			strTitle += _T(" - ");
			strTitle += FilterTrlComment(_T("Copy [of an item]"));
			pwTemplate.pszTitle = const_cast<TCHAR *>(strTitle.c_str());

			m_mgr.UnlockEntryPassword(&pwTemplate);
			VERIFY(m_mgr.AddEntry(&pwTemplate));
			m_mgr.LockEntryPassword(&pwTemplate);

			PW_ENTRY *pNew = m_mgr.GetLastEditedEntry();
			ASSERT_ENTRY(pNew);
			if(pNew != NULL)
			{
				std::vector<BYTE> vUuid(16);
				memcpy(&vUuid[0], pNew->uuid, 16);
				vUuids.push_back(vUuid);
			}
		}
	}

	m_bModified = TRUE;
	_UpdateCachedGroupIDs();

	// if(m_nAutoSort == 0) UpdatePasswordList();
	// else _SortListIfAutoSort();
	for(size_t iNew = 0; iNew < vUuids.size(); ++iNew)
	{
		PW_ENTRY *peToAdd = m_mgr.GetEntryByUuid(&(vUuids[iNew][0]));
		if(peToAdd == NULL) { ASSERT(FALSE); continue; }

		_List_SetEntry(static_cast<DWORD>(m_cList.GetItemCount()), peToAdd, TRUE, &tNow);
	}

	NewGUI_DeselectAllItems(&m_cList);
	for(size_t iFocus = 0; iFocus < vUuids.size(); ++iFocus) // Select all copies
	{
		const DWORD dwListItem = _EntryUuidToListPos(&(vUuids[iFocus][0]));
		if(dwListItem != DWORD_MAX)
		{
			m_cList.SetItemState(static_cast<int>(dwListItem), LVIS_SELECTED, LVIS_SELECTED);
			
			if(iFocus == (vUuids.size() - 1))
				m_cList.EnsureVisible(static_cast<int>(dwListItem), FALSE);
		}
	}

	_UpdateToolBar();
}

void CPwSafeDlg::OnUpdatePwlistDuplicate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((m_dwLastFirstSelectedItem != DWORD_MAX) ? TRUE : FALSE);
}

void CPwSafeDlg::OnInfoHomepage()
{
	NotifyUserActivity();
	ShellExecute(NULL, NULL, PWM_HOMEPAGE, NULL, NULL, SW_SHOW);
}

void CPwSafeDlg::OnViewAlwaysOnTop()
{
	NotifyUserActivity();

	UINT uState;
	BOOL bChecked;

	uState = m_menu.GetMenuState(ID_VIEW_ALWAYSONTOP, MF_BYCOMMAND);
	ASSERT(uState != 0xFFFFFFFF);

	if(uState & MF_CHECKED) bChecked = TRUE;
	else bChecked = FALSE;

	if(bChecked == TRUE)
	{
		uState = MF_UNCHECKED; // Toggle
		SetWindowPos(&wndNoTopMost, 0, 0, 0, 0,
			SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		m_bAlwaysOnTop = FALSE;
	}
	else
	{
		uState = MF_CHECKED; // Toggle
		SetWindowPos(&wndTopMost, 0, 0, 0, 0,
			SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		m_bAlwaysOnTop = TRUE;
	}

	m_menu.CheckMenuItem(ID_VIEW_ALWAYSONTOP, MF_BYCOMMAND | uState);
}

void CPwSafeDlg::ExportSelectedGroup(int nFormat)
{
	CPwExport cExp; PWEXPORT_OPTIONS pwo;
	CString strFile;
	DWORD dwSelectedGroup = GetSelectedGroupId();

	ASSERT(dwSelectedGroup != DWORD_MAX);
	if(dwSelectedGroup == DWORD_MAX) return;
	if(m_bForceAllowExport == FALSE)
	{
		if(IsUnsafeAllowed(this->m_hWnd) == FALSE) return;
	}

	cExp.SetManager(&m_mgr);
	cExp.SetNewLineSeq(m_bWindowsNewLine);
	cExp.SetFormat(nFormat);
	strFile = GetExportFile(nFormat, m_mgr.GetGroupById(dwSelectedGroup)->pszGroupName, TRUE);
	if(strFile.GetLength() != 0)
		if(GetExportOptions(&pwo, &cExp, FALSE) == TRUE)
			cExp.ExportGroup(strFile, dwSelectedGroup, &pwo, NULL);
}

void CPwSafeDlg::OnSafeExportGroupHtml()
{
	NotifyUserActivity();
	ExportSelectedGroup(PWEXP_HTML);
}

void CPwSafeDlg::OnSafeExportGroupXml()
{
	NotifyUserActivity();
	ExportSelectedGroup(PWEXP_XML);
}

void CPwSafeDlg::OnSafeExportGroupCsv()
{
	NotifyUserActivity();
	ExportSelectedGroup(PWEXP_CSV);
}

void CPwSafeDlg::OnUpdateSafeExportGroupHtml(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((m_hLastSelectedGroup != NULL) ? TRUE : FALSE);
}

void CPwSafeDlg::OnUpdateSafeExportGroupXml(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((m_hLastSelectedGroup != NULL) ? TRUE : FALSE);
}

void CPwSafeDlg::OnUpdateSafeExportGroupCsv(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((m_hLastSelectedGroup != NULL) ? TRUE : FALSE);
}

void CPwSafeDlg::OnSafePrintGroup()
{
	NotifyUserActivity();

	const DWORD dwGroup = GetSelectedGroupId();
	if(dwGroup == DWORD_MAX) return;

	if(_CallPlugins(KPM_GROUP_PRINT_PRE, dwGroup, 0) == FALSE) return;

	_PrintGroup(dwGroup);
}

void CPwSafeDlg::OnUpdateSafePrintGroup(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((m_hLastSelectedGroup != NULL) ? TRUE : FALSE);
}

void CPwSafeDlg::OnPwlistMoveUp()
{
	NotifyUserActivity();

	DWORD dwRelativeEntry = GetSelectedEntry();
	if(dwRelativeEntry == 0) return;

	DWORD dwEntryIndex = _ListSelToEntryIndex(dwRelativeEntry);
	ASSERT(dwEntryIndex != DWORD_MAX); if(dwEntryIndex == DWORD_MAX) return;

	_TouchEntry(dwRelativeEntry, FALSE);

	if(m_nAutoSort != 0)
	{
		MessageBox(TRL("Auto-sorting of the password list is enabled, you cannot move entries manually."),
			PWM_PRODUCT_NAME_SHORT, MB_OK | MB_ICONWARNING);
		return;
	}

	PW_ENTRY *p = m_mgr.GetEntry(dwEntryIndex);
	ASSERT(p != NULL); if(p == NULL) return;
	m_mgr.MoveEntry(p->uGroupId, dwRelativeEntry, dwRelativeEntry - 1);

	_List_SaveView();
	UpdatePasswordList();
	_List_RestoreView();
	m_cList.FocusItem(dwRelativeEntry - 1, TRUE);
	m_cList.EnsureVisible(dwRelativeEntry - 1, FALSE);
	m_bModified = TRUE;
	_UpdateToolBar();
}

void CPwSafeDlg::OnUpdatePwlistMoveUp(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(((m_dwLastFirstSelectedItem != DWORD_MAX) && (m_dwLastNumSelectedItems == 1)) ? TRUE : FALSE);
}

void CPwSafeDlg::OnPwlistMoveTop()
{
	NotifyUserActivity();

	DWORD dwRelativeEntry = GetSelectedEntry();
	if(dwRelativeEntry == 0) return;

	DWORD dwEntryIndex = _ListSelToEntryIndex(dwRelativeEntry);
	ASSERT(dwEntryIndex != DWORD_MAX); if(dwEntryIndex == DWORD_MAX) return;

	PW_ENTRY *p;

	_TouchEntry(dwRelativeEntry, FALSE);

	if(m_nAutoSort != 0)
	{
		MessageBox(TRL("Auto-sorting of the password list is enabled, you cannot move entries manually."),
			PWM_PRODUCT_NAME_SHORT, MB_OK | MB_ICONWARNING);
		return;
	}

	p = m_mgr.GetEntry(dwEntryIndex);
	ASSERT(p != NULL); if(p == NULL) return;
	m_mgr.MoveEntry(p->uGroupId, dwRelativeEntry, 0);

	_List_SaveView();
	UpdatePasswordList();
	_List_RestoreView();
	m_cList.FocusItem(0, TRUE);
	m_cList.EnsureVisible(0, FALSE);
	m_bModified = TRUE;
	_UpdateToolBar();
}

void CPwSafeDlg::OnUpdatePwlistMoveTop(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(((m_dwLastFirstSelectedItem != DWORD_MAX) && (m_dwLastNumSelectedItems == 1)) ? TRUE : FALSE);
}

void CPwSafeDlg::OnPwlistMoveDown()
{
	NotifyUserActivity();

	DWORD dwItemCount = (DWORD)m_cList.GetItemCount();
	PW_ENTRY *p;

	DWORD dwRelativeEntry = GetSelectedEntry();
	if(dwRelativeEntry == (dwItemCount - 1)) return;

	DWORD dwEntryIndex = _ListSelToEntryIndex();
	ASSERT(dwEntryIndex != DWORD_MAX); if(dwEntryIndex == DWORD_MAX) return;

	_TouchEntry(dwRelativeEntry, FALSE);

	if(m_nAutoSort != 0)
	{
		MessageBox(TRL("Auto-sorting of the password list is enabled, you cannot move entries manually."),
			PWM_PRODUCT_NAME_SHORT, MB_OK | MB_ICONWARNING);
		return;
	}

	p = m_mgr.GetEntry(dwEntryIndex);
	ASSERT(p != NULL); if(p == NULL) return;
	m_mgr.MoveEntry(p->uGroupId, dwRelativeEntry, dwRelativeEntry + 1);

	_List_SaveView();
	UpdatePasswordList();
	_List_RestoreView();
	const int nSel = min(dwRelativeEntry + 1, dwItemCount - 1);
	m_cList.FocusItem(nSel, TRUE);
	m_cList.EnsureVisible(nSel, FALSE);
	m_bModified = TRUE;
	_UpdateToolBar();
}

void CPwSafeDlg::OnUpdatePwlistMoveDown(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(((m_dwLastFirstSelectedItem != DWORD_MAX) && (m_dwLastNumSelectedItems == 1)) ? TRUE : FALSE);
}

void CPwSafeDlg::OnPwlistMoveBottom()
{
	NotifyUserActivity();

	DWORD dwItemCount = (DWORD)m_cList.GetItemCount();
	PW_ENTRY *p;

	DWORD dwRelativeEntry = GetSelectedEntry();
	if(dwRelativeEntry == (dwItemCount - 1)) return;

	DWORD dwEntryIndex = _ListSelToEntryIndex(dwRelativeEntry);
	if(dwEntryIndex == DWORD_MAX) return;

	_TouchEntry(dwRelativeEntry, FALSE);

	if(m_nAutoSort != 0)
	{
		MessageBox(TRL("Auto-sorting of the password list is enabled, you cannot move entries manually."),
			PWM_PRODUCT_NAME_SHORT, MB_OK | MB_ICONWARNING);
		return;
	}

	p = m_mgr.GetEntry(dwEntryIndex);
	ASSERT(p != NULL); if(p == NULL) return;
	m_mgr.MoveEntry(p->uGroupId, dwRelativeEntry, dwItemCount - 1);

	_List_SaveView();
	UpdatePasswordList();
	_List_RestoreView();
	const int nSel = static_cast<int>(dwItemCount - 1);
	m_cList.FocusItem(nSel, TRUE);
	m_cList.EnsureVisible(nSel, FALSE);
	m_bModified = TRUE;
	_UpdateToolBar();
}

void CPwSafeDlg::OnUpdatePwlistMoveBottom(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(((m_dwLastFirstSelectedItem != DWORD_MAX) && (m_dwLastNumSelectedItems == 1)) ? TRUE : FALSE);
}

void CPwSafeDlg::OnBeginDragPwlist(NMHDR* pNMHDR, LRESULT* pResult)
{
	NotifyUserActivity();

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;

	if(m_bFileOpen == FALSE) return;

	const DWORD dwEntryIndex = _ListSelToEntryIndex();
	if(dwEntryIndex == DWORD_MAX) return;

	PW_ENTRY *p = m_mgr.GetEntry(dwEntryIndex);
	ASSERT_ENTRY(p); if(p == NULL) return;

	m_bDisplayDialog = TRUE;

	// Auto-type drag&drop doesn't work with Firefox (always DROPEFFECT_NONE)
	// UINT uFlags = 0;
	// m_cList.HitTest(CPoint(pNMListView->ptAction), &uFlags);
	// const bool bAutoType = ((uFlags & LVHT_ONITEMICON) != 0);

	// if(!bAutoType)
	m_cGroups.m_drop.SetDragAccept(TRUE);

	COleDropSource *pDropSource = new COleDropSource;
	COleDataSource *pDataSource = new COleDataSource;
	// DROPEFFECT de = DROPEFFECT_NONE;

	TRY
	{
		CSharedFile fileShared;
		TRY
		{
			CArchive ar(&fileShared, CArchive::store);
			TRY
			{
				CString strData;
				bool bDereferenceData = false;

				switch(pNMListView->iSubItem)
				{
				case 0:
					strData = p->pszTitle;
					bDereferenceData = true;
					break;
				case 1:
					strData = p->pszUserName;
					bDereferenceData = true;
					break;
				case 2:
					strData = p->pszURL;
					bDereferenceData = true;
					break;
				case 3:
					m_mgr.UnlockEntryPassword(p);
					strData = p->pszPassword;
					m_mgr.LockEntryPassword(p);
					bDereferenceData = true;
					break;
				case 4:
					strData = p->pszAdditional;
					bDereferenceData = true;
					break;
				case 5:
					_PwTimeToStringEx(p->tCreation, strData, CPwSafeDlg::m_bUseLocalTimeFormat);
					break;
				case 6:
					_PwTimeToStringEx(p->tLastMod, strData, CPwSafeDlg::m_bUseLocalTimeFormat);
					break;
				case 7:
					_PwTimeToStringEx(p->tLastAccess, strData, CPwSafeDlg::m_bUseLocalTimeFormat);
					break;
				case 8:
					_PwTimeToStringEx(p->tExpire, strData, CPwSafeDlg::m_bUseLocalTimeFormat);
					break;
				case 9:
					_UuidToString(p->uuid, &strData);
					break;
				case 10:
					strData = p->pszBinaryDesc;
					bDereferenceData = true;
					break;
				default:
					ASSERT(FALSE);
					break;
				}

				CString strToTransfer;
				if(bDereferenceData == false) strToTransfer = strData;
				else strToTransfer = SprCompile(strData, false, p, &m_mgr, false, false);

				// if(bAutoType)
				// {
				//	strToTransfer.Empty();
				//	DropToBackgroundIfOptionEnabled(true);
				// }

				ar.Write((LPCTSTR)strToTransfer, strToTransfer.GetLength() + sizeof(TCHAR));
				ar.Close();
			}
			CATCH_ALL(eInner) { ASSERT(FALSE); }
			END_CATCH_ALL;
		}
		CATCH_ALL(eMiddle) { ASSERT(FALSE); }
		END_CATCH_ALL;

		pDataSource->CacheGlobalData(CF_TEXT, fileShared.Detach());
		pDataSource->DoDragDrop(DROPEFFECT_MOVE | DROPEFFECT_COPY, NULL, pDropSource);
	}
	CATCH_ALL(eOuter) { ASSERT(FALSE); }
	END_CATCH_ALL;

	SAFE_DELETE(pDataSource);
	SAFE_DELETE(pDropSource);

	m_cGroups.m_drop._RemoveDropSelection();
	m_cGroups.m_drop.SetDragAccept(FALSE);

	// if(bAutoType && (de != DROPEFFECT_NONE))
	// {
	//	WU_MouseClick(false);
	//	_AutoType(p, false, 0, NULL);
	// }

	_TouchEntry(GetSelectedEntry(), FALSE);
	_UpdateToolBar();

	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnFileChangeLanguage()
{
	NotifyUserActivity();

	m_bDisplayDialog = TRUE;

	CLanguagesDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		m_bRestartApplication = TRUE;
		m_bDisplayDialog = FALSE;
		OnFileExit();
	}

	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnInfoReadme()
{
	NotifyUserActivity();
	WU_OpenAppHelp(NULL, m_hWnd);
}

// void CPwSafeDlg::OnInfoLicense()
// {
//	NotifyUserActivity();
//	_OpenLocalFile(PWM_LICENSE_FILE, OLF_OPEN);
// }

// void CPwSafeDlg::OnInfoPrintLicense()
// {
//	NotifyUserActivity();
//	_OpenLocalFile(PWM_LICENSE_FILE, OLF_PRINT);
// }

void CPwSafeDlg::_SelChangeView(UINT uID)
{
	UINT uState;
	BOOL bChecked;
	BOOL *pFlag = &bChecked; // Just to avoid NULL pointer
	UINT uIndex = 0;
	RECT rect;

	uState = m_menu.GetMenuState(uID, MF_BYCOMMAND);
	ASSERT(uState != 0xFFFFFFFF);

	if(uID == ID_VIEW_TITLE)           { pFlag = &m_bShowTitle;      uIndex = 0; }
	else if(uID == ID_VIEW_USERNAME)   { pFlag = &m_bShowUserName;   uIndex = 1; }
	else if(uID == ID_VIEW_URL)        { pFlag = &m_bShowURL;        uIndex = 2; }
	else if(uID == ID_VIEW_PASSWORD)   { pFlag = &m_bShowPassword;   uIndex = 3; }
	else if(uID == ID_VIEW_NOTES)      { pFlag = &m_bShowNotes;      uIndex = 4; }
	else if(uID == ID_VIEW_CREATION)   { pFlag = &m_bShowCreation;   uIndex = 5; }
	else if(uID == ID_VIEW_LASTMOD)    { pFlag = &m_bShowLastMod;    uIndex = 6; }
	else if(uID == ID_VIEW_LASTACCESS) { pFlag = &m_bShowLastAccess; uIndex = 7; }
	else if(uID == ID_VIEW_EXPIRE)     { pFlag = &m_bShowExpire;     uIndex = 8; }
	else if(uID == ID_VIEW_UUID)       { pFlag = &m_bShowUUID;       uIndex = 9; }
	else if(uID == ID_VIEW_ATTACH)     { pFlag = &m_bShowAttach;     uIndex = 10; }
	else { ASSERT(FALSE); return; }

	if(uState & MF_CHECKED) bChecked = TRUE;
	else bChecked = FALSE;

	if(bChecked == TRUE)
	{
		uState = MF_UNCHECKED; // Toggle
		*pFlag = FALSE;
	}
	else
	{
		uState = MF_CHECKED; // Toggle
		*pFlag = TRUE;
	}
	m_menu.CheckMenuItem(uID, MF_BYCOMMAND | uState);

	GetClientRect(&rect);
	int nColumnWidth = rect.right - rect.left - GetSystemMetrics(SM_CXVSCROLL);
	nColumnWidth -= 8; nColumnWidth >>= 3;

	m_bShowColumn[uIndex] = *pFlag;
	if(*pFlag == TRUE) m_nColumnWidths[uIndex] = nColumnWidth;
	else m_nColumnWidths[uIndex] = 0;

	RefreshPasswordList();
	ProcessResize();
}

void CPwSafeDlg::_EnableViewMenuItems(BCMenu *pMenu)
{
	for(int i = 0; i < 11; ++i)
	{
		UINT uID = ID_VIEW_TITLE;
		BOOL *pFlag;

		if(i == 0) { uID = ID_VIEW_TITLE;           pFlag = &m_bShowTitle; }
		else if(i == 1) { uID = ID_VIEW_USERNAME;   pFlag = &m_bShowUserName; }
		else if(i == 2) { uID = ID_VIEW_URL;        pFlag = &m_bShowURL; }
		else if(i == 3) { uID = ID_VIEW_PASSWORD;   pFlag = &m_bShowPassword; }
		else if(i == 4) { uID = ID_VIEW_NOTES;      pFlag = &m_bShowNotes; }
		else if(i == 5) { uID = ID_VIEW_CREATION;   pFlag = &m_bShowCreation; }
		else if(i == 6) { uID = ID_VIEW_LASTMOD;    pFlag = &m_bShowLastMod; }
		else if(i == 7) { uID = ID_VIEW_LASTACCESS; pFlag = &m_bShowLastAccess; }
		else if(i == 8) { uID = ID_VIEW_EXPIRE;     pFlag = &m_bShowExpire; }
		else if(i == 9) { uID = ID_VIEW_UUID;       pFlag = &m_bShowUUID; }
		else if(i == 10) { uID = ID_VIEW_ATTACH;    pFlag = &m_bShowAttach; }
		else { ASSERT(FALSE); break; }

		const UINT uState = ((*pFlag == TRUE) ? MF_CHECKED : MF_UNCHECKED);
		pMenu->CheckMenuItem(uID, MF_BYCOMMAND | uState);
	}
}

void CPwSafeDlg::OnViewTitle()
{
	NotifyUserActivity();
	_SelChangeView(ID_VIEW_TITLE);
}

void CPwSafeDlg::OnViewUsername()
{
	NotifyUserActivity();
	_SelChangeView(ID_VIEW_USERNAME);
}

void CPwSafeDlg::OnViewUrl()
{
	NotifyUserActivity();
	_SelChangeView(ID_VIEW_URL);
}

void CPwSafeDlg::OnViewPassword()
{
	NotifyUserActivity();
	_SelChangeView(ID_VIEW_PASSWORD);
}

void CPwSafeDlg::OnViewNotes()
{
	NotifyUserActivity();
	_SelChangeView(ID_VIEW_NOTES);
}

void CPwSafeDlg::OnFileLock()
{
	NotifyUserActivity();

	// if(m_bDisplayDialog == TRUE) return; // Cannot do anything while displaying a dialog

	// This is a thread-critical function, therefore we fake to show a dialog, this
	// prevents the timer function from doing anything that could interfer with us
	m_bDisplayDialog = TRUE;

	if(_CallPlugins(KPM_FILE_LOCK_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	if((m_bFileOpen == FALSE) && (m_bLocked == FALSE)) { m_bDisplayDialog = FALSE; return; }

	CString strMenuItem;
	m_menu.GetMenuText(ID_FILE_LOCK, strMenuItem, MF_BYCOMMAND);

	LPCTSTR lpSuffix = _GetCmdAccelExt(_T("&Lock Workspace"));
	CString strExtended = TRL("&Lock Workspace");
	strExtended += _T("\t");
	strExtended += lpSuffix;

	const bool bDoLock = ((strMenuItem == TRL("&Lock Workspace")) ||
		(strMenuItem == strExtended));
	if(bDoLock) // Lock
	{
		_DeleteTemporaryFiles();

		// m_nLockedViewParams[0] = (long)m_cGroups.GetFirstVisibleItem();
		m_nLockedViewParams[1] = GetSelectedEntry();
		_Groups_SaveView();
		m_nLockedViewParams[2] = m_cList.GetTopIndex();

		const DWORD dwLastSelected = _ListSelToEntryIndex(DWORD_MAX);
		if(dwLastSelected != DWORD_MAX) memcpy(m_pPreLockItemUuid, m_mgr.GetEntry(dwLastSelected)->uuid, 16);
		else memset(m_pPreLockItemUuid, 0, 16);

		m_bExiting = TRUE;
		m_bIsLocking = TRUE;
        _CallPlugins(KPM_FILE_LOCK_CLOSE_PRE, 0, 0);
		OnFileClose();
        _CallPlugins(KPM_FILE_LOCK_CLOSE_POST, 0, 0);
		m_bIsLocking = FALSE;
		m_bExiting = FALSE;
		if(m_bFileOpen == TRUE)
		{
			// MessageBox(TRL("First close the open file before opening another one!"),
			//	PWM_PRODUCT_NAME_SHORT, MB_OK | MB_ICONWARNING);
			m_bDisplayDialog = FALSE; return;
		}

		if(m_strLastDb.IsEmpty() == TRUE) { m_bDisplayDialog = FALSE; return; }

		m_bLocked = TRUE;
		strExtended = TRL("&Unlock Workspace");
		strExtended += _T("\t");
		strExtended += _GetCmdAccelExt(_T("&Lock Workspace"));
		m_menu.SetMenuText(ID_FILE_LOCK, strExtended, MF_BYCOMMAND);
		SetStatusTextEx(CString(TRL("Workspace locked")) + _T("."));
		m_btnTbLock.SetTooltipText(RemoveAcceleratorTipEx(TRL("&Unlock Workspace")));

		// _UpdateTitleBar(); // Updated by _UpdateToolBar()

		ShowEntryDetails(NULL);
		NewGUI_EnableWindowPeekPreview(this->m_hWnd, false);

		if(m_bEntryView == TRUE) m_reEntryView.SetFocus();
		else if(m_bShowToolBar == TRUE) m_cQuickFind.SetFocus();
		else m_cList.SetFocus();
	}
	else // Unlock
	{
		_OpenDatabase(NULL, m_strLastDb, NULL, NULL, FALSE, NULL, FALSE);

		if(m_bFileOpen == FALSE)
		{
			// strExtended = PWM_PRODUCT_NAME; strExtended += _T(" [");
			// strExtended += TRL("Workspace locked"); strExtended += _T("]");
			// SetWindowText(strExtended);
			// m_systray.SetTooltipText(strExtended); // Done in _OpenDatabase already
			// m_systray.SetIcon(m_hTrayIconLocked);

			// MessageBox(TRL("Workspace is locked."), PWM_PRODUCT_NAME_SHORT, MB_ICONINFORMATION | MB_OK);
			SetStatusTextEx(TRL("Workspace is locked."));
            _CallPlugins(KPM_FILE_UNLOCK_FAILED, 0, 0);
			m_bDisplayDialog = FALSE; return;
		}

		NotifyUserActivity();
		m_bLocked = FALSE;
		strExtended = TRL("&Lock Workspace");
		strExtended += _T("\t");
		strExtended += _GetCmdAccelExt(_T("&Lock Workspace"));
		m_menu.SetMenuText(ID_FILE_LOCK, strExtended, MF_BYCOMMAND);
		m_btnTbLock.SetTooltipText(RemoveAcceleratorTipEx(TRL("&Lock Workspace")));

		// m_cGroups.SelectSetFirstVisible((HTREEITEM)m_nLockedViewParams[0]);
		// m_cGroups.SelectItem((HTREEITEM)m_nLockedViewParams[1]);
		_Groups_RestoreView();
		UpdatePasswordList();

		m_cList.EnsureVisible(m_cList.GetItemCount() - 1, FALSE);
		m_cList.EnsureVisible(m_nLockedViewParams[2], FALSE);

		if(m_nLockedViewParams[1] != DWORD_MAX)
			m_cList.FocusItem(static_cast<int>(m_nLockedViewParams[1]), TRUE);

		SetStatusTextEx(TRL("Ready."));

		// strExtended = PWM_PRODUCT_NAME;
		// strExtended += _T(" - ");
		// strExtended += CsFileOnly(&m_strFile);
		// SetWindowText(strExtended);
		// m_systray.SetIcon(m_hTrayIconNormal);
		// m_systray.SetTooltipText(strExtended);

		BYTE aNoUuid[16];
		memset(aNoUuid, 0, 16);
		if(memcmp(m_pPreLockItemUuid, aNoUuid, 16) != 0)
		{
			PW_ENTRY *p = m_mgr.GetEntryByUuid(m_pPreLockItemUuid);
			ShowEntryDetails(p); // NULL is allowed
		}

		NewGUI_EnableWindowPeekPreview(this->m_hWnd, true);

		m_cList.SetFocus();
	}

	_UpdateTrayIcon();
	_UpdateToolBar(TRUE);
	m_bDisplayDialog = FALSE;

	if((m_bMinimizeOnLock != FALSE) && bDoLock && (m_bMinimized == FALSE) &&
		(m_bTrayed == FALSE))
		SetViewHideState(FALSE, FALSE);
}

void CPwSafeDlg::OnUpdateFileLock(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((m_bFileOpen || m_bLocked) && (!m_bDisplayDialog));
}

void CPwSafeDlg::OnGroupMoveTop()
{
	NotifyUserActivity();

	const DWORD dwGroupId = GetSelectedGroupId();
	if(dwGroupId == DWORD_MAX) return;

	_TouchGroup(dwGroupId, FALSE);

	// DWORD dwGroup = m_mgr.GetGroupByIdN(dwGroupId);
	// m_mgr.MoveGroup(dwGroup, 0);
	m_mgr.MoveGroupExDir(dwGroupId, -2);

	_Groups_SaveView(FALSE); UpdateGroupList(); _Groups_RestoreView();
	HTREEITEM hItem = _GroupIdToHTreeItem(dwGroupId);
	m_cGroups.EnsureVisible(hItem);
	m_cGroups.SelectItem(hItem);
	_List_SaveView(); UpdatePasswordList(); _List_RestoreView();
	m_bModified = TRUE;
	_UpdateToolBar();
}

void CPwSafeDlg::OnUpdateGroupMoveTop(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((m_hLastSelectedGroup != NULL) ? TRUE : FALSE);
}

void CPwSafeDlg::OnGroupMoveBottom()
{
	NotifyUserActivity();

	DWORD dwGroupId = GetSelectedGroupId();
	if(dwGroupId == DWORD_MAX) return;
	_TouchGroup(dwGroupId, FALSE);

	// DWORD dwGroup = m_mgr.GetGroupByIdN(dwGroupId);
	// m_mgr.MoveGroup(dwGroup, (DWORD)m_cGroups.GetCount() - 1);
	m_mgr.MoveGroupExDir(dwGroupId, 2);

	_Groups_SaveView(FALSE); UpdateGroupList(); _Groups_RestoreView();
	HTREEITEM hItem = _GroupIdToHTreeItem(dwGroupId);
	m_cGroups.EnsureVisible(hItem);
	m_cGroups.SelectItem(hItem);
	_List_SaveView(); UpdatePasswordList(); _List_RestoreView();
	m_bModified = TRUE;
	_UpdateToolBar();
}

void CPwSafeDlg::OnUpdateGroupMoveBottom(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((m_hLastSelectedGroup != NULL) ? TRUE : FALSE);
}

void CPwSafeDlg::OnGroupMoveUp()
{
	NotifyUserActivity();

	DWORD dwGroupId = GetSelectedGroupId();
	if(dwGroupId == DWORD_MAX) return;
	_TouchGroup(dwGroupId, FALSE);

	// DWORD dwGroup = m_mgr.GetGroupByIdN(dwGroupId);
	// if(dwGroup == 0) return; // Already is the top item
	// VERIFY(m_mgr.MoveGroup(dwGroup, dwGroup - 1));
	m_mgr.MoveGroupExDir(dwGroupId, -1);

	_Groups_SaveView(FALSE); UpdateGroupList(); _Groups_RestoreView();
	HTREEITEM hItem = _GroupIdToHTreeItem(dwGroupId);
	m_cGroups.EnsureVisible(hItem);
	m_cGroups.SelectItem(hItem);
	_List_SaveView(); UpdatePasswordList(); _List_RestoreView();
	m_bModified = TRUE;
	_UpdateToolBar();
}

void CPwSafeDlg::OnUpdateGroupMoveUp(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((m_hLastSelectedGroup != NULL) ? TRUE : FALSE);
}

void CPwSafeDlg::OnGroupMoveDown()
{
	NotifyUserActivity();

	DWORD dwGroupId = GetSelectedGroupId();
	if(dwGroupId == DWORD_MAX) return;
	_TouchGroup(dwGroupId, FALSE);

	// DWORD dwGroup = m_mgr.GetGroupByIdN(dwGroupId);
	// if(dwGroup == (m_mgr.GetNumberOfGroups() - 1)) return; // Already is last group
	// VERIFY(m_mgr.MoveGroup(dwGroup, dwGroup + 1));
	m_mgr.MoveGroupExDir(dwGroupId, 1);

	_Groups_SaveView(FALSE); UpdateGroupList(); _Groups_RestoreView();
	HTREEITEM hItem = _GroupIdToHTreeItem(dwGroupId);
	m_cGroups.EnsureVisible(hItem);
	m_cGroups.SelectItem(hItem);
	_List_SaveView(); UpdatePasswordList(); _List_RestoreView();
	m_bModified = TRUE;
	_UpdateToolBar();
}

void CPwSafeDlg::OnUpdateGroupMoveDown(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((m_hLastSelectedGroup != NULL) ? TRUE : FALSE);
}

LRESULT CPwSafeDlg::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	if(m_bSingleClickTrayIcon == TRUE)
	{
		if(lParam == WM_LBUTTONDBLCLK) return 0;
		if(lParam == WM_LBUTTONUP) lParam = WM_LBUTTONDBLCLK;
	}

	return m_systray.OnTrayNotification(wParam, lParam);
}

LRESULT CPwSafeDlg::OnUpdateClipboard(WPARAM wParam, LPARAM lParam)
{
	static int iSpinner = 0;

	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	if(m_nClipboardMethod != CM_ENHSECURE) return 0;

	if((!MakeClipboardDelayRender(m_hWnd, &m_hwndNextViewer)) && (iSpinner < 100))
	{
		// If we could not get onwership of the clipboard try again
		// sometimes we post WM_MY_UPDATECLIPBOARD message to soon
		// after putting delayed data on the clipboard whoever
		// requested the clipboard data still hasn't freed their lock
		// on the clipboard.
		++iSpinner;
		PostMessage(WM_MY_UPDATECLIPBOARD); // Failed to own clipboard so try again
	}
	else iSpinner = 0; // Stop posting WM_MY_UPDATECLIPBOARD messages

	return 0;
}

void CPwSafeDlg::OnViewHide()
{
	NotifyUserActivity();
	if(m_bDisplayDialog == TRUE) return;
	ToggleViewHideState(TRUE);
	_UpdateToolBar(TRUE);
}

void CPwSafeDlg::OnImportCsv()
{
	NotifyUserActivity();
	m_bDisplayDialog = TRUE;

	CString strFilter = TRL("CSV Files");
	strFilter += _T(" (*.csv)|*.csv|");
	strFilter += TRL("All Files");
	strFilter += _T(" (*.*)|*.*||");

	std::vector<std::basic_string<TCHAR> > vFiles = WU_GetFileNames(TRUE, _T("csv"),
		strFilter, TRUE, this, FALSE, TRUE);

	const DWORD dwGroupId = GetSelectedGroupId();
	ASSERT(dwGroupId != DWORD_MAX);

	for(size_t iFile = 0; iFile < vFiles.size(); ++iFile)
	{
		CPwImport pvi;
		const DWORD dwResult = pvi.ImportCsvToDb(vFiles[iFile].c_str(), &m_mgr, dwGroupId);

		if(dwResult == DWORD_MAX)
		{
			_SortListIfAutoSort();
			if(m_nAutoSort == 0) UpdatePasswordList();
			m_bModified = TRUE;
		}
		else
		{
			CString str = vFiles[iFile].c_str();
			str += _T("\r\n\r\n");
			str += TRL("An error occurred while importing the file. File cannot be imported.");
			str += _T("\r\n\r\n");
			str += TRL("Entry"); str += _T(": #");
			CString strTemp; strTemp.Format(_T("%u"), dwResult);
			str += strTemp;

			str += _T("\r\n\r\n");
			str += TRL("The help file contains detailed information about the expected input format. Do you want to open the help file?");

			if(MessageBox(str, PWM_PRODUCT_NAME_SHORT, MB_ICONWARNING | MB_YESNO) == IDYES)
				WU_OpenAppHelp(PWM_HELP_CSV, m_hWnd);
		}
	}

	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdateImportCsv(CCmdUI* pCmdUI)
{
	BOOL b = m_bFileOpen;
	b &= ((m_mgr.GetNumberOfGroups() != 0) ? TRUE : FALSE);
	pCmdUI->Enable(b);
}

void CPwSafeDlg::OnClickPwlist(NMHDR* pNMHDR, LRESULT* pResult)
{
	NotifyUserActivity();

	*pResult = 0;
	UNREFERENCED_PARAMETER(pNMHDR);

	// This would break the multiselect ability of the list, therefor we removed it
	// if((GetKeyState(VK_CONTROL) & 0x8000) > 0) OnPwlistEdit();

	_UpdateToolBar();
}

void CPwSafeDlg::_SortList(DWORD dwByField, BOOL bAutoSortCall)
{
	const DWORD dwGroupId = GetSelectedGroupId();

	if(dwGroupId == DWORD_MAX) return;
	if(dwGroupId == m_mgr.GetGroupId(PWS_SEARCHGROUP))
	{
		if(bAutoSortCall == FALSE)
		{
			_RemoveSearchGroup();
			m_cList.DeleteAllItemsEx();
		}

		return;
	}

	const int nTop = m_cList.GetTopIndex();

	m_mgr.SortGroup(dwGroupId, dwByField);
	if(bAutoSortCall == FALSE) m_bModified = TRUE;
	UpdatePasswordList();
	_UpdateToolBar();

	m_cList.EnsureVisible(m_cList.GetItemCount() - 1, FALSE);
	m_cList.EnsureVisible(nTop, FALSE);
}

void CPwSafeDlg::OnColumnClickPwlist(NMHDR* pNMHDR, LRESULT* pResult)
{
	NotifyUserActivity();

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;
	if(_CheckIfCanSort() == FALSE) return;
	_SortList((DWORD)pNMListView->iSubItem, FALSE);
}

void CPwSafeDlg::OnImportCWallet()
{
	NotifyUserActivity();
	m_bDisplayDialog = TRUE;

	CString strFilter = TRL("Text Files");
	strFilter += _T(" (*.txt)|*.txt|");
	strFilter += TRL("All Files");
	strFilter += _T(" (*.*)|*.*||");

	std::vector<std::basic_string<TCHAR> > vFiles = WU_GetFileNames(TRUE, _T("txt"),
		strFilter, TRUE, this, FALSE, TRUE);

	for(size_t iFile = 0; iFile < vFiles.size(); ++iFile)
	{
		CPwImport pvi;
		if(pvi.ImportCWalletToDb(vFiles[iFile].c_str(), &m_mgr) == TRUE)
		{
			UpdateGroupList();
			m_cGroups.EnsureVisible(_GetLastGroupItem(&m_cGroups));
			_SortListIfAutoSort();
			if(m_nAutoSort == 0) UpdatePasswordList();
			m_bModified = TRUE;
		}
		else
		{
			CString strMsg = vFiles[iFile].c_str();
			strMsg += _T("\r\n\r\n");
			strMsg += TRL("An error occurred while importing the file. File cannot be imported.");
			MessageBox(strMsg, PWM_PRODUCT_NAME_SHORT, MB_ICONWARNING | MB_OK);
		}
	}

	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdateImportCWallet(CCmdUI* pCmdUI)
{
	BOOL b = m_bFileOpen;
	b &= ((m_mgr.GetNumberOfGroups() != 0) ? TRUE : FALSE);
	pCmdUI->Enable(b);
}

BOOL CPwSafeDlg::PreTranslateMessage(MSG* pMsg)
{
	ASSERT(pMsg != NULL);

	// if(m_hAccel != NULL)
	// {
	//	CWnd *pWnd = GetFocus();
	//	if(pWnd != NULL)
	//	{
	//		if((pWnd != &m_reEntryView) || (m_bFileOpen == FALSE) || (m_bLocked == TRUE))
	//		{
	//			if(TranslateAccelerator(this->m_hWnd, m_hAccel, pMsg))
	//				return(TRUE);
	//		}
	//	}
	// }

	if(((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_SYSKEYDOWN)) && (m_hAccel != NULL))
	{
		CWnd *pFocusWnd = GetFocus();

		if(pFocusWnd == &m_reEntryView)
		{
			if((pMsg->wParam != 'C') && (pMsg->wParam != 'A'))
				if(TranslateAccelerator(this->m_hWnd, m_hAccel, pMsg) != FALSE)
					return TRUE;
		}
		// else if(pFocusWnd == &m_cQuickFind)
		else if(NewGUI_ComboBox_HasFocus(m_cQuickFind.m_hWnd,
			(pFocusWnd != NULL) ? pFocusWnd->m_hWnd : NULL))
		{
			if((pMsg->wParam != 'C') && (pMsg->wParam != 'A') && (pMsg->wParam != 'V') && (pMsg->wParam != 'X'))
				if(TranslateAccelerator(this->m_hWnd, m_hAccel, pMsg) != FALSE)
					return TRUE;
		}
		else
		{
			if(TranslateAccelerator(this->m_hWnd, m_hAccel, pMsg) != FALSE)
				return TRUE;
		}
	}

	if(((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_KEYUP)) &&
		(pMsg->wParam == VK_RETURN))
	{
		CWnd *pFocusWnd = GetFocus();

		// if(pFocusWnd == &m_cQuickFind)
		if(NewGUI_ComboBox_HasFocus(m_cQuickFind.m_hWnd,
			(pFocusWnd != NULL) ? pFocusWnd->m_hWnd : NULL))
		{
			if(pMsg->message == WM_KEYDOWN) _DoQuickFind(NULL);
			return TRUE; // Also ignores WM_KEYUP sent to the edit control
		}
		else if(pFocusWnd == &m_cList)
		{
			if(pMsg->message == WM_KEYDOWN) OnPwlistEdit();
			return TRUE; // Also ignores WM_KEYUP sent to the edit control
		}
	}

	// When changing Ctrl+E, also change the tooltip text of the quick search box
	if(((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_KEYUP)) &&
		(pMsg->wParam == 'E') && ((GetKeyState(VK_CONTROL) & 0x8000) != 0))
	{
		if(pMsg->message == WM_KEYDOWN) m_cQuickFind.SetFocus();
		return TRUE;
	}

	if((pMsg->message == WM_POWERBROADCAST) &&
		((pMsg->wParam == PBT_APMQUERYSUSPEND) ||
		(pMsg->wParam == PBT_APMSUSPEND)))
	{
		if(m_bLockOnWinLock != FALSE) _ChangeLockState(TRUE);
	}

	m_tip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}

void CPwSafeDlg::OnUpdateFileNew(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE); // m_bLocked == FALSE
}

void CPwSafeDlg::OnUpdateFileOpen(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE); // m_bLocked == FALSE
}

void CPwSafeDlg::OnImportPwSafe()
{
	NotifyUserActivity();

	m_bDisplayDialog = TRUE;

	CString strFilter = TRL("Text Files");
	strFilter += _T(" (*.txt)|*.txt|");
	strFilter += TRL("All Files");
	strFilter += _T(" (*.*)|*.*||");

	std::vector<std::basic_string<TCHAR> > vFiles = WU_GetFileNames(TRUE, _T("txt"),
		strFilter, TRUE, this, FALSE, TRUE);

	for(size_t iFile = 0; iFile < vFiles.size(); ++iFile)
	{
		CPwImport pvi;
		if(pvi.ImportPwSafeToDb(vFiles[iFile].c_str(), &m_mgr) == TRUE)
		{
			_Groups_SaveView(); UpdateGroupList(); _Groups_RestoreView();
			_SortListIfAutoSort();
			if(m_nAutoSort == 0) UpdatePasswordList();
			m_bModified = TRUE;
		}
		else
		{
			CString strMsg = vFiles[iFile].c_str();
			strMsg += _T("\r\n\r\n");
			strMsg += TRL("An error occurred while importing the file. File cannot be imported.");
			MessageBox(strMsg, PWM_PRODUCT_NAME_SHORT, MB_ICONWARNING | MB_OK);
		}
	}

	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdateImportPwSafe(CCmdUI* pCmdUI)
{
	BOOL b = m_bFileOpen;
	b &= ((m_mgr.GetNumberOfGroups() != 0) ? TRUE : FALSE);
	pCmdUI->Enable(b);
}

void CPwSafeDlg::OnViewCreation()
{
	NotifyUserActivity();
	_SelChangeView(ID_VIEW_CREATION);
}

void CPwSafeDlg::OnViewLastMod()
{
	NotifyUserActivity();
	_SelChangeView(ID_VIEW_LASTMOD);
}

void CPwSafeDlg::OnViewLastAccess()
{
	NotifyUserActivity();
	_SelChangeView(ID_VIEW_LASTACCESS);
}

void CPwSafeDlg::OnViewExpire()
{
	NotifyUserActivity();
	_SelChangeView(ID_VIEW_EXPIRE);
}

void CPwSafeDlg::OnViewUuid()
{
	NotifyUserActivity();
	_SelChangeView(ID_VIEW_UUID);
}

void CPwSafeDlg::_TouchGroup(DWORD dwGroupId, BOOL bEdit)
{
	PW_GROUP *pGroup = m_mgr.GetGroupById(dwGroupId);
	ASSERT(pGroup != NULL);

	PW_TIME tNow;
	_GetCurrentPwTime(&tNow);

	pGroup->tLastAccess = tNow;
	if(bEdit == TRUE) pGroup->tLastMod = tNow;

	if(m_bSaveOnLATMod == TRUE) m_bModified = TRUE;
}

void CPwSafeDlg::_TouchEntry(DWORD dwListIndex, BOOL bEdit)
{
	LV_ITEM lvi;
	TCHAR szTemp[1024];
	CString strTemp;
	BYTE aUuid[16];
	PW_TIME tNow;

	if(dwListIndex >= (DWORD)m_cList.GetItemCount()) return;

	// ASSERT(dwListIndex != DWORD_MAX);
	// if(dwListIndex == DWORD_MAX) return;

	ZeroMemory(&lvi, sizeof(LV_ITEM));
	lvi.iItem = (int)dwListIndex;
	lvi.iSubItem = 9;
	lvi.mask = LVIF_TEXT;
	lvi.pszText = szTemp;
	lvi.cchTextMax = 1024;
	m_cList.GetItem(&lvi);

	strTemp = lvi.pszText;
	_StringToUuid(strTemp, aUuid);

	EraseCString(&strTemp);

	_GetCurrentPwTime(&tNow);

	PW_ENTRY *pEntry = m_mgr.GetEntryByUuid(aUuid);
	ASSERT_ENTRY(pEntry);

	if(pEntry != NULL)
	{
		pEntry->tLastAccess = tNow;
		if(bEdit == TRUE) pEntry->tLastMod = tNow;

		_UpdateCachedGroupIDs();
		_List_SetEntry(dwListIndex, pEntry, FALSE, &tNow);
	}

	if(m_bSaveOnLATMod == TRUE) m_bModified = TRUE;
}

void CPwSafeDlg::OnTbOpen()
{
	OnFileOpen();
}

void CPwSafeDlg::OnTbSave()
{
	OnFileSave();
}

#define UTB_ENSURE_ENABLED_STATE(_pWnd, _bEnabled) { if((_pWnd)->IsWindowEnabled() != (_bEnabled)) { \
	(_pWnd)->EnableWindow(_bEnabled); bNeedSetFocus = TRUE; } }

void CPwSafeDlg::_UpdateToolBar(BOOL bForceUpdate)
{
	_UpdateTitleBar();

	CWnd *pFocusWnd = this->GetFocus();
	BOOL bNeedSetFocus = FALSE;

	const DWORD dwSelectedEntry = ((m_bLocked == FALSE) ? GetSelectedEntry() : DWORD_MAX);
	const DWORD dwNumSelectedEntries = ((m_bLocked == FALSE) ? GetSelectedEntriesCount() : 0);
	const DWORD dwNumberOfGroups = ((m_bLocked == FALSE) ? m_mgr.GetNumberOfGroups() : 0);
	const DWORD dwNumberOfEntries = ((m_bLocked == FALSE) ? m_mgr.GetNumberOfEntries() : 0);

	CString strStatus;
	strStatus.Format(TRL("Total: %u groups / %u entries"), dwNumberOfGroups, dwNumberOfEntries);
	SetStatusTextEx(strStatus, 0);
	strStatus.Format(TRL("%u of %u selected"), (dwNumSelectedEntries == DWORD_MAX) ? 0 : dwNumSelectedEntries,
		(m_bFileOpen == TRUE) ? m_cList.GetItemCount() : 0);
	SetStatusTextEx(strStatus, 1);

	if((m_bMinimized == TRUE) || (m_bTrayed == TRUE)) return;

	const ULONGLONG ullListParams = (((ULONGLONG)(dwSelectedEntry & 0x0000FFFF) << 48) |
		((ULONGLONG)(dwNumSelectedEntries & 0x000000FF) << 40) |
		((ULONGLONG)(dwNumberOfGroups & 0x000000FF) << 32) |
		((ULONGLONG)(dwNumberOfEntries & 0x0000FFFF) << 16) |
		((ULONGLONG)(m_bLocked & 0x00000001) << 15) |
		((ULONGLONG)(m_bModified & 0x00000001) << 14));

	// ullListParams = (((ULONGLONG)dwSelectedEntry) << 34) ^
	//	(((ULONGLONG)dwNumSelectedEntries) << 2) ^ (m_bLocked << 1) ^
	//	m_bModified ^ (((ULONGLONG)dwNumberOfEntries) << 14);

	// Update the rest (toolbar, entry view, ...) only if needed
	if((ullListParams == m_ullLastListParams) && (bForceUpdate == FALSE)) return;
	m_ullLastListParams = ullListParams;

	const DWORD dwFirstEntryIndex = _ListSelToEntryIndex(dwSelectedEntry);
	PW_ENTRY *p = NULL;

	if(dwFirstEntryIndex != DWORD_MAX) p = m_mgr.GetEntry(dwFirstEntryIndex);
	ShowEntryDetails(p);

	if(m_bModified)
		m_btnTbSave.SetBitmaps(IDB_TB_SAVE, RGB(255,0,255));
	else
		m_btnTbSave.SetBitmaps(IDB_TB_SAVE_DISABLED, RGB(255,0,255));

	if(m_bFileOpen == TRUE)
	{
		UTB_ENSURE_ENABLED_STATE(&m_btnTbAddEntry, (m_mgr.GetNumberOfGroups() != 0) ? TRUE : FALSE);
		UTB_ENSURE_ENABLED_STATE(&m_cQuickFind, TRUE);

		if(m_bFileReadOnly == FALSE)
		{
			if(m_bAllowSaveIfModifiedOnly == FALSE) UTB_ENSURE_ENABLED_STATE(&m_btnTbSave, TRUE)
			else
			{
				if(m_bModified == FALSE) UTB_ENSURE_ENABLED_STATE(&m_btnTbSave, FALSE)
				else UTB_ENSURE_ENABLED_STATE(&m_btnTbSave, TRUE)
			}
		}
		else UTB_ENSURE_ENABLED_STATE(&m_btnTbSave, FALSE)
	}
	else
	{
		UTB_ENSURE_ENABLED_STATE(&m_btnTbAddEntry, FALSE);
		UTB_ENSURE_ENABLED_STATE(&m_cQuickFind, FALSE);
		UTB_ENSURE_ENABLED_STATE(&m_btnTbSave, FALSE);
	}

	if((dwSelectedEntry != DWORD_MAX) && (dwNumSelectedEntries == 1))
	{
		UTB_ENSURE_ENABLED_STATE(&m_btnTbCopyUser, TRUE);
		UTB_ENSURE_ENABLED_STATE(&m_btnTbCopyPw, TRUE);
		UTB_ENSURE_ENABLED_STATE(&m_btnTbEditEntry, TRUE);
	}
	else
	{
		UTB_ENSURE_ENABLED_STATE(&m_btnTbCopyUser, FALSE);
		UTB_ENSURE_ENABLED_STATE(&m_btnTbCopyPw, FALSE);
		UTB_ENSURE_ENABLED_STATE(&m_btnTbEditEntry, FALSE);
	}

	if((dwSelectedEntry != DWORD_MAX) && (dwNumSelectedEntries >= 1))
		UTB_ENSURE_ENABLED_STATE(&m_btnTbDeleteEntry, TRUE)
	else
		UTB_ENSURE_ENABLED_STATE(&m_btnTbDeleteEntry, FALSE)

	if(m_bFileOpen && (dwNumberOfEntries != 0))
		UTB_ENSURE_ENABLED_STATE(&m_btnTbFind, TRUE)
	else
		UTB_ENSURE_ENABLED_STATE(&m_btnTbFind, FALSE)

	if(m_bFileOpen || m_bLocked)
		UTB_ENSURE_ENABLED_STATE(&m_btnTbLock, TRUE)
	else
		UTB_ENSURE_ENABLED_STATE(&m_btnTbLock, FALSE)

	if((pFocusWnd != NULL) && (bNeedSetFocus == TRUE))
		pFocusWnd->SetFocus(); // Restore the focus!
}

void CPwSafeDlg::OnTbNew()
{
	OnFileNew();
}

void CPwSafeDlg::OnTbCopyUser()
{
	OnPwlistCopyUser();
}

void CPwSafeDlg::OnTbCopyPw()
{
	OnPwlistCopyPw();
}

void CPwSafeDlg::OnTbAddEntry()
{
	OnPwlistAdd();
}

void CPwSafeDlg::OnTbEditEntry()
{
	OnPwlistEdit();
}

void CPwSafeDlg::OnTbDeleteEntry()
{
	OnPwlistDelete();
}

void CPwSafeDlg::OnTbFind()
{
	OnPwlistFind();
}

void CPwSafeDlg::OnTbLock()
{
	OnFileLock();
}

void CPwSafeDlg::OnTbAbout()
{
	OnInfoAbout();
}

void CPwSafeDlg::OnViewShowToolBar()
{
	NotifyUserActivity();

	UINT uState = m_menu.GetMenuState(ID_VIEW_SHOWTOOLBAR, MF_BYCOMMAND);
	ASSERT(uState != 0xFFFFFFFF);

	BOOL bChecked;
	if((uState & MF_CHECKED) != 0) bChecked = TRUE;
	else bChecked = FALSE;

	if(bChecked == TRUE)
	{
		uState = MF_UNCHECKED; // Toggle
		bChecked = FALSE;
	}
	else
	{
		uState = MF_CHECKED; // Toggle
		bChecked = TRUE;
	}
	m_menu.CheckMenuItem(ID_VIEW_SHOWTOOLBAR, MF_BYCOMMAND | uState);

	m_bShowToolBar = bChecked;

	_ShowToolBar(m_bShowToolBar);
	ProcessResize();
}

void CPwSafeDlg::_ShowToolBar(BOOL bShow)
{
	const int nCommand = ((bShow == FALSE) ? SW_HIDE : SW_SHOW);

	m_btnTbNew.ShowWindow(nCommand); m_btnTbOpen.ShowWindow(nCommand);
	m_btnTbSave.ShowWindow(nCommand); m_btnTbAddEntry.ShowWindow(nCommand);
	m_btnTbEditEntry.ShowWindow(nCommand); m_btnTbDeleteEntry.ShowWindow(nCommand);
	m_btnTbCopyPw.ShowWindow(nCommand); m_btnTbCopyUser.ShowWindow(nCommand);
	m_btnTbFind.ShowWindow(nCommand); m_btnTbLock.ShowWindow(nCommand);
	m_btnTbAbout.ShowWindow(nCommand);

	m_cQuickFind.ShowWindow(nCommand);

	GetDlgItem(IDC_STATIC_TBSEP0)->ShowWindow(nCommand); GetDlgItem(IDC_STATIC_TBSEP1)->ShowWindow(nCommand);
	GetDlgItem(IDC_STATIC_TBSEP2)->ShowWindow(nCommand); GetDlgItem(IDC_STATIC_TBSEP3)->ShowWindow(nCommand);
	GetDlgItem(IDC_STATIC_TBSEP4)->ShowWindow(nCommand); GetDlgItem(IDC_STATIC_TBSEP5)->ShowWindow(nCommand);
}

BOOL CPwSafeDlg::_RemoveSearchGroup()
{
	const DWORD dwSearchGroupId = m_mgr.GetGroupId(PWS_SEARCHGROUP);
	if(dwSearchGroupId != DWORD_MAX)
	{
		const DWORD dwSearchItemsCount = m_mgr.GetNumberOfItemsInGroupN(dwSearchGroupId);
		if(dwSearchItemsCount == 0) // Delete only if the group is empty
		{
			m_mgr.DeleteGroupById(dwSearchGroupId, FALSE); // Remove from password manager

			HTREEITEM hTreeNode = _GroupIdToHTreeItem(dwSearchGroupId);
			if(hTreeNode != NULL) m_cGroups.DeleteItem(hTreeNode);
			else { ASSERT(FALSE); UpdateGroupList(); }

			ShowEntryDetails(NULL);
			return TRUE;
		}
	}

	return FALSE;
}

void CPwSafeDlg::OnPwlistMassModify()
{
	NotifyUserActivity();

	if(m_bFileOpen == FALSE) return;

	m_bDisplayDialog = TRUE;

	CEntryPropertiesDlg dlg;
	dlg.m_pMgr = &m_mgr;
	dlg.m_pParentIcons = &m_ilIcons;
	dlg.m_dwDefaultExpire = m_dwDefaultExpire;

	if(_CallPlugins(KPM_MASSMODIFY_ENTRIES_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	if(dlg.DoModal() == TRUE)
	{
		const DWORD dwGroupId = m_mgr.GetGroupIdByIndex((DWORD)dlg.m_nGroupInx);
		ASSERT(dwGroupId != DWORD_MAX);

		for(DWORD i = 0; i < static_cast<DWORD>(m_cList.GetItemCount()); ++i)
		{
			const UINT uState = m_cList.GetItemState(static_cast<int>(i), LVIS_SELECTED);
			if((uState & LVIS_SELECTED) != 0)
			{
				const DWORD dwIndex = _ListSelToEntryIndex(i); // Uses UUID to get the entry
				ASSERT(dwIndex != DWORD_MAX); if(dwIndex == DWORD_MAX) continue;

				PW_ENTRY *p = m_mgr.GetEntry(dwIndex);
				ASSERT(p != NULL); if(p == NULL) continue;

				if(dlg.m_bModGroup == TRUE) p->uGroupId = dwGroupId;
				if(dlg.m_bModIcon == TRUE) p->uImageId = (DWORD)dlg.m_nIconId;
				if(dlg.m_bModExpire == TRUE) p->tExpire = dlg.m_tExpire;

				_TouchEntry(i, TRUE); // Doesn't change the entry except the time fields
			}
		}

		if(dlg.m_bModGroup == TRUE) // We need a full update
		{
			_List_SaveView();
			_SortListIfAutoSort();
			if(m_nAutoSort == 0) UpdatePasswordList();
			_List_RestoreView();
		}
		else // Refresh is enough, no entries have been moved
		{
			_SortListIfAutoSort();
			if(m_nAutoSort == 0) RefreshPasswordList();
		}

		m_bModified = TRUE;
	}

	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdatePwlistMassModify(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((m_bFileOpen && (m_dwLastNumSelectedItems >= 1)) ? TRUE : FALSE);
}

void CPwSafeDlg::OnKeyDownPwlist(NMHDR* pNMHDR, LRESULT* pResult)
{
	NotifyUserActivity();

	UNREFERENCED_PARAMETER(pNMHDR);
	*pResult = 0;

	m_bCachedToolBarUpdate = TRUE;
}

CString CPwSafeDlg::_MakeRtfString(LPCTSTR lptString)
{
	if(lptString == NULL) { ASSERT(FALSE); return CString(); }

#ifdef _UNICODE
	const WCHAR* lpw = lptString;
#else
	const WCHAR* lpw = _StringToUnicode(lptString);
#endif
	if(lpw == NULL) { ASSERT(FALSE); return CString(lptString); }

	CStringBuilderEx sb;

	const size_t uMax = wcslen(lpw);
	for(size_t u = 0; u < uMax; ++u)
	{
		const WCHAR wch = lpw[u];

		if(wch == L'\\') sb.Append(_T("\\\\"));
		else if(wch == L'\r') { }
		else if(wch == L'{') sb.Append(_T("\\{"));
		else if(wch == L'}') sb.Append(_T("\\}"));
		else if(wch == L'\n') sb.Append(_T("\\par "));
		else if(wch <= 255) sb.Append(static_cast<TCHAR>(wch));
		else
		{
			CString strT;
			strT.Format(_T("\\u%d?"), static_cast<int>(wch));
			sb.Append((LPCTSTR)strT);
		}

		// else if((tch < 128) && (tch >= 32)) str += tch;
		// else
		// {
		//	str2.Format(_T("%02x"), (BYTE)(tch & 0xFF));
		//	str += _T("\\'"); str += str2;
		// }
	}

#ifndef _UNICODE
	SAFE_DELETE_ARRAY(lpw);
#endif

	return CString(sb.ToString().c_str());
}

void CPwSafeDlg::ShowEntryDetails(PW_ENTRY *p)
{
	CString str, str2;

	if(p != NULL) { ASSERT_ENTRY(p); }

	if(m_bEntryView == FALSE) return;

	if(p == NULL) // if p == NULL, just clear the view
	{
#ifndef _UNICODE
		m_reEntryView.SetRTF(CString(_T("")), SF_TEXT);
#else
		m_reEntryView.SetRTF(CString(_T("")), SF_TEXT | SF_UNICODE);
#endif
		return;
	}

	CHARRANGE crURL;
	ZeroMemory(&crURL, sizeof(CHARRANGE));

	// === Begin entry view RTF assembly ===

	CString strTemp;
	str = _T("{\\rtf1\\ansi\\ansicpg");
	strTemp.Format(_T("%u"), m_uACP);
	str += strTemp;
	str += _T("\\deff0{\\fonttbl{\\f0\\fswiss MS Sans Serif;}{\\f1\\froman\\fcharset2 Symbol;}{\\f2\\fswiss ");
	str += m_strListFontFace;
	str += _T(";}{\\f3\\fswiss Arial;}}");
	str += _T("{\\colortbl\\red0\\green0\\blue0;}");
	str += _T("\\deflang1031\\pard\\plain\\f2\\cf0 ");
	strTemp.Format(_T("%d"), m_nListFontSize * 2);
	str += _T("\\fs"); str += strTemp;
	str += _T("\\b ");
	str += TRL("Group:"); str += _T("\\b0  ");
	const PW_GROUP *pg = m_mgr.GetGroupById(p->uGroupId); ASSERT(pg != NULL);
	if(pg != NULL) str += _MakeRtfString(pg->pszGroupName);

	m_lGroupUrlStart = static_cast<LONG>(_tcslen(TRL("Group:")) + 1);
	crURL.cpMin += (m_lGroupUrlStart + static_cast<LONG>(_tcslen(pg->pszGroupName)));

	if((p->pszTitle != NULL) && (p->pszTitle[0] != 0))
	{
		str += _T(", \\b ");
		str += TRL("Title:"); str += _T("\\b0  ");
		str += _MakeRtfString(p->pszTitle);

		crURL.cpMin += static_cast<LONG>(_tcslen(TRL("Title:")) + 3 +
			_tcslen(p->pszTitle));
	}

	if((p->pszUserName != NULL) && (p->pszUserName[0] != 0))
	{
		str += _T(", \\b ");
		str += TRL("User Name"); str += _T(":\\b0  ");

		crURL.cpMin += static_cast<LONG>(_tcslen(TRL("User Name")) + 4);

		if(m_bUserStars == FALSE)
		{
			str += _MakeRtfString(p->pszUserName);
			crURL.cpMin += static_cast<LONG>(_tcslen(p->pszUserName));
		}
		else
		{
			str += _T("********");
			crURL.cpMin += 8;
		}
	}

	m_mgr.UnlockEntryPassword(p);
	if((p->pszPassword != NULL) && (p->pszPassword[0] != 0))
	{
		str += _T(", \\b ");
		str += TRL("Password:"); str += _T("\\b0  ");

		crURL.cpMin += static_cast<LONG>(_tcslen(TRL("Password:")) + 3);

		// Do not dereference the password here; if dereferencing should
		// be implemented here in the future, make sure to make local
		// copy of the password in order to not conflict with in-memory
		// protection and the Spr engine
		CString strTempPassword = _MakeRtfString(p->pszPassword);
		if(m_bPasswordStars == FALSE)
		{
			str += strTempPassword;
			crURL.cpMin += static_cast<LONG>(_tcslen(p->pszPassword));
		}
		else
		{
			str += _T("********");
			crURL.cpMin += 8;
		}
		EraseCString(&strTempPassword);
	}
	m_mgr.LockEntryPassword(p);

	if((p->pszURL != NULL) && (p->pszURL[0] != 0))
	{
		str += _T(", \\b ");
		str += TRL("URL:"); str += _T("\\b0  ");
		str += _MakeRtfString(p->pszURL);

		crURL.cpMin += static_cast<LONG>(_tcslen(TRL("URL:")) + 3);
	}

	str += _T(", \\b ");
	str += TRL("Creation Time"); str += _T(":\\b0  ");
	_PwTimeToStringEx(p->tCreation, str2, CPwSafeDlg::m_bUseLocalTimeFormat);
	str += _MakeRtfString(str2);

	str += _T(", \\b ");
	str += TRL("Last Modification"); str += _T(":\\b0  ");
	_PwTimeToStringEx(p->tLastMod, str2, CPwSafeDlg::m_bUseLocalTimeFormat);
	str += _MakeRtfString(str2);

	str += _T(", \\b ");
	str += TRL("Last Access"); str += _T(":\\b0  ");
	_PwTimeToStringEx(p->tLastAccess, str2, CPwSafeDlg::m_bUseLocalTimeFormat);
	str += _MakeRtfString(str2);

	if(memcmp(&p->tExpire, &g_tNeverExpire, sizeof(PW_TIME)) != 0)
	{
		str += _T(", \\b ");
		str += TRL("Expires"); str += _T(":\\b0  ");
		_PwTimeToStringEx(p->tExpire, str2, CPwSafeDlg::m_bUseLocalTimeFormat);
		str += _MakeRtfString(str2);
	}

	if((p->pszBinaryDesc != NULL) && (p->pszBinaryDesc[0] != 0))
	{
		str += _T(", \\b ");
		str += TRL("Attachment"); str += _T(":\\b0  ");
		str += _MakeRtfString((LPCTSTR)(p->pszBinaryDesc));
	}

	if((p->pszAdditional != NULL) && (p->pszAdditional[0] != 0))
	{
		str += _T("\\par \\par ");

		str2 = _MakeRtfString(p->pszAdditional);

		str2.Replace(_T("<b>"), _T("\\b "));
		str2.Replace(_T("</b>"), _T("\\b0 "));
		str2.Replace(_T("<i>"), _T("\\i "));
		str2.Replace(_T("</i>"), _T("\\i0 "));
		str2.Replace(_T("<u>"), _T("\\ul "));
		str2.Replace(_T("</u>"), _T("\\ul0 "));
		// str2.Replace(_T("<center>"), _T("\\qc "));
		// str2.Replace(_T("</center>"), _T("\\ql "));

		str += str2;
	}

	str += _T("\\pard }");

	// === End entry view RTF assembly ===

#ifndef _UNICODE
	m_reEntryView.SetRTF(str, SF_RTF);
#else
	m_reEntryView.SetRTF(str, SF_RTF | SF_UNICODE);
#endif

	CHARFORMAT cfURL;
	ZeroMemory(&cfURL, sizeof(CHARFORMAT));
	cfURL.cbSize = sizeof(CHARFORMAT);
	cfURL.dwMask = (CFM_LINK | CFM_COLOR | CFM_UNDERLINE);
	cfURL.dwEffects = (CFE_LINK | CFE_UNDERLINE);
	cfURL.crTextColor = RGB(0, 0, 255);

	if((pg->pszGroupName != NULL) && (pg->pszGroupName[0] != 0))
	{
		CHARRANGE crGroup;
		ZeroMemory(&crGroup, sizeof(CHARRANGE));
		crGroup.cpMin = m_lGroupUrlStart;
		crGroup.cpMax = crGroup.cpMin + static_cast<LONG>(_tcslen(pg->pszGroupName));

		m_reEntryView.SetSel(crGroup);
		m_reEntryView.SetSelectionCharFormat(cfURL);
	}

	if((p->pszURL != NULL) && (p->pszURL[0] != 0)) // Make the URL clickable
	{
		crURL.cpMax = crURL.cpMin + static_cast<LONG>(_tcslen(p->pszURL));

		m_reEntryView.SetSel(crURL);
		m_reEntryView.SetSelectionCharFormat(cfURL);
	}

	m_reEntryView.SetSel(0, 0);
}

void CPwSafeDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	NotifyUserActivity();

	if(m_hDraggingGroup != NULL)
	{
		CPoint pt = point; // Local copy of 'point', because we will modify it here
		ClientToScreen(&pt);

		CImageList::DragMove(pt);
		CImageList::DragShowNolock(FALSE);

		if(CWnd::WindowFromPoint(pt) != &m_cGroups) // || (m_bCanDragGroup == FALSE)
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
		}
		else
		{
			if(GetKeyState(VK_CONTROL) & 0x1000)
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_UPARROW));
			else
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

			TVHITTESTINFO tvhti;
			tvhti.pt = pt;
			m_cGroups.ScreenToClient(&tvhti.pt);
			HTREEITEM hItemSel = m_cGroups.HitTest(&tvhti);

			if(tvhti.flags & (TVHT_ONITEM | TVHT_ONITEMINDENT))
				m_cGroups.SelectDropTarget(tvhti.hItem);
			else
				m_cGroups.SelectDropTarget(NULL);

			if(hItemSel == m_hDraggingGroup) // Cannot drag on itself
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
		}

		CImageList::DragShowNolock(TRUE);
		return;
	}

	RECT rectWindow;
	GetWindowRect(&rectWindow);

	const LONG nAddTop = ((m_bShowToolBar == TRUE) ? 26 : 0);
	const int cyMenu = GetSystemMetrics(SM_CYMENU);

	if(m_bDragging == TRUE)
	{
		if(m_bDraggingHoriz == TRUE)
		{
			m_lSplitterPosHoriz = point.x - 3;
		}
		else
		{
			m_lSplitterPosVert = rectWindow.bottom - rectWindow.top - point.y - nAddTop - 3 - (cyMenu * 2);
		}

		ProcessResize();
	}

	RECT rectGroupList; RECT rectPwList; RECT rectEntryView;
	m_cGroups.GetWindowRect(&rectGroupList); ScreenToClient(&rectGroupList);
	m_cList.GetWindowRect(&rectPwList); ScreenToClient(&rectPwList);
	m_reEntryView.GetWindowRect(&rectEntryView); ScreenToClient(&rectEntryView);

	if((point.x >= (rectGroupList.right - 1)) && (point.x <= (rectPwList.left + 1)))
	{
		SetCursor(m_hDragLeftRight);
	}
	else if((point.y >= (rectGroupList.bottom - 1)) && (point.y <= (rectEntryView.top + 1)))
	{
		if(m_bEntryView == TRUE) SetCursor(m_hDragUpDown);
	}
	else SetCursor(m_hArrowCursor);

	CDialog::OnMouseMove(nFlags, point);
}

void CPwSafeDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	NotifyUserActivity();
	CDialog::OnLButtonDown(nFlags, point);

	if(m_bDragging == FALSE)
	{
		RECT rectGroupList; RECT rectPwList; RECT rectEntryView;
		m_cGroups.GetWindowRect(&rectGroupList); ScreenToClient(&rectGroupList);
		m_cList.GetWindowRect(&rectPwList); ScreenToClient(&rectPwList);
		m_reEntryView.GetWindowRect(&rectEntryView); ScreenToClient(&rectEntryView);

		if((point.x >= (rectGroupList.right - 1)) && (point.x <= (rectPwList.left + 1)))
		{
			SetCursor(m_hDragLeftRight);
			m_bDraggingHoriz = TRUE;
			m_bDragging = TRUE;
		}
		else if((point.y >= (rectGroupList.bottom - 1)) && (point.y <= (rectEntryView.top + 1)))
		{
			if(m_bEntryView == TRUE)
			{
				SetCursor(m_hDragUpDown);
				m_bDraggingHoriz = FALSE;
				m_bDragging = TRUE;
			}
		}

		if(m_bDragging == TRUE) SetCapture();
	}
}

void CPwSafeDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	NotifyUserActivity();

	if(m_hDraggingGroup != NULL)
	{
		// if(m_bCanDragGroup == TRUE) {

		CPoint pt = point;
		ClientToScreen(&pt);

		const BOOL bCopy = (((GetKeyState(VK_CONTROL) & 0x1000) > 0) ? TRUE : FALSE);

		HTREEITEM hItemDropTo = m_cGroups.GetDropHilightItem();

		if(hItemDropTo != m_hDraggingGroup)
		{
			const DWORD dwDragGroupId = (DWORD)m_cGroups.GetItemData(m_hDraggingGroup);
			const DWORD dwDragGroupPos = m_mgr.GetGroupByIdN(dwDragGroupId);
			DWORD dwNewGroupId = dwDragGroupId;

			ASSERT(dwDragGroupPos != DWORD_MAX);
			if((dwDragGroupPos != DWORD_MAX) && (bCopy == TRUE))
			{
				PW_GROUP grpNew = *m_mgr.GetGroup(dwDragGroupPos);
				grpNew.usLevel = 0;
				grpNew.uGroupId = 0; // Create new group
				m_mgr.AddGroup(&grpNew);
				PW_GROUP* pNew = m_mgr.GetGroup(m_mgr.GetNumberOfGroups() - 1);
				dwNewGroupId = pNew->uGroupId;
			}

			if(hItemDropTo != NULL) // Dropped on item
			{
				const DWORD dwDragToGroupId = (DWORD)m_cGroups.GetItemData(hItemDropTo);
				ASSERT(dwDragToGroupId != DWORD_MAX);
				const DWORD dwDragToGroupPos = m_mgr.GetGroupByIdN(dwDragToGroupId);
				ASSERT(dwDragToGroupPos != DWORD_MAX);

				// DWORD dwVParent = m_mgr.GetLastChildGroup(dwDragToGroupPos);

				// if(pNew != NULL)
				//	pNew->usLevel = (USHORT)(m_mgr.GetGroup(dwDragToGroupPos)->usLevel + 1);

				m_mgr.GetGroup(dwDragToGroupPos)->dwFlags |= PWGF_EXPANDED;
				
				// m_mgr.MoveGroup(m_mgr.GetNumberOfGroups() - 1, dwVParent + 1);
				m_mgr.MoveGroupEx(dwNewGroupId, dwDragToGroupId);
			}
			else // Dropped on empty space
			{
			}

			// Fix group ID, unassociate all entries from the group that we will delete
			// if(bCopy == FALSE)
			// {
			//	m_mgr.SubstEntryGroupIds(dwDragGroupId, dwNewGroupId);
			//	// If moving, delete source group
			//	m_mgr.DeleteGroupById(dwDragGroupId, FALSE);
			// }

			if(hItemDropTo != NULL) _SyncItem(&m_cGroups, hItemDropTo, FALSE);
			m_cGroups.SelectDropTarget(NULL);
			_Groups_SaveView(FALSE); UpdateGroupList(); _Groups_RestoreView();
			m_bModified = TRUE;
		}

		m_cGroups.SelectDropTarget(NULL);
		_FinishDragging(TRUE);
		RedrawWindow();
		_UpdateToolBar();
	}
	else if(m_bDragging == TRUE)
	{
		ReleaseCapture();
		m_bDragging = FALSE;
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void CPwSafeDlg::OnViewEntryView()
{
	NotifyUserActivity();

	UINT uState;
	BOOL bChecked;

	uState = m_menu.GetMenuState(ID_VIEW_ENTRYVIEW, MF_BYCOMMAND);
	ASSERT(uState != 0xFFFFFFFF);

	if(uState & MF_CHECKED) bChecked = TRUE;
	else bChecked = FALSE;

	if(bChecked == TRUE)
	{
		uState = MF_UNCHECKED; // Toggle
		m_bEntryView = FALSE;
	}
	else
	{
		uState = MF_CHECKED; // Toggle
		m_bEntryView = TRUE;
	}

	m_menu.CheckMenuItem(ID_VIEW_ENTRYVIEW, MF_BYCOMMAND | uState);

	m_reEntryView.ShowWindow((m_bEntryView == TRUE) ? SW_SHOW : SW_HIDE);
	m_bCachedToolBarUpdate = TRUE;
	ProcessResize();
}

BOOL CPwSafeDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	MSGFILTER *lpMsgFilter = (MSGFILTER *)lParam;
	ENLINK *pEL = (ENLINK *)lParam;

	if(lParam != 0)
	{
		if((LOWORD(wParam) == IDC_RE_ENTRYVIEW) && (lpMsgFilter->nmhdr.code == EN_MSGFILTER)
			&& (lpMsgFilter->msg == WM_RBUTTONDOWN))
		{
			POINT pt;
			GetCursorPos(&pt);

			ASSERT(m_pEntryViewTrackableMenu != NULL);

			if(m_pEntryViewTrackableMenu != NULL)
				m_pEntryViewTrackableMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
					pt.x, pt.y, AfxGetMainWnd());
		}
		else if((LOWORD(wParam) == IDC_RE_ENTRYVIEW) && (pEL->nmhdr.code == EN_LINK) && (pEL->msg == WM_LBUTTONDOWN))
		{
			CHARRANGE cr;
			m_reEntryView.GetSel(cr); // Push current user selection
			m_reEntryView.SetSel(pEL->chrg);
			CString strSelectedURL = m_reEntryView.GetSelText();
			m_reEntryView.SetSel(cr); // Pop current user selection

			DWORD dwCmpEntryIndex = _ListSelToEntryIndex(DWORD_MAX);
			PW_ENTRY *pCmpEntry = m_mgr.GetEntry(dwCmpEntryIndex);

			if(pCmpEntry != NULL)
			{
				PW_GROUP *pGroup = m_mgr.GetGroupById(pCmpEntry->uGroupId);
				if((pEL->chrg.cpMin == m_lGroupUrlStart) && (pGroup != NULL) &&
					(_tcscmp(pGroup->pszGroupName, strSelectedURL) == 0))
				{
					HTREEITEM hAutoSelect = _GroupIdToHTreeItem(pGroup->uGroupId);
					if(hAutoSelect != NULL)
					{
						m_cGroups.EnsureVisible(hAutoSelect);
						m_cGroups.SelectItem(hAutoSelect);
						_List_SaveView();
						UpdatePasswordList();
						_List_RestoreView();
						const DWORD dwAutoPos = _EntryUuidToListPos(pCmpEntry->uuid);
						if(dwAutoPos != DWORD_MAX)
						{
							m_cList.EnsureVisible(static_cast<int>(dwAutoPos), FALSE);
							m_cList.FocusItem(static_cast<int>(dwAutoPos), TRUE);
						}
						ShowEntryDetails(pCmpEntry);
						m_cList.SetFocus();
					}
				}
				else if(strSelectedURL == pCmpEntry->pszURL) OnPwlistVisitUrl();
				else ParseAndOpenURLWithEntryInfo(strSelectedURL, pCmpEntry);
			}
			else OpenUrlEx(strSelectedURL, this->m_hWnd);

			// OnPwlistVisitUrl();
		}
		else if((LOWORD(wParam) == IDC_RE_ENTRYVIEW) && (pEL->msg == WM_MOUSEMOVE))
		{
			NotifyUserActivity();
		}
		else if((wParam == 0) && (((NMHDR *)lParam)->code == NM_RCLICK))
		{
			POINT pt, ptList;
			GetCursorPos(&pt);
			ptList = pt;
			m_cList.ScreenToClient(&ptList);

			HDHITTESTINFO hitTest;

			hitTest.pt.x = ptList.x + m_cList.GetScrollPos(SB_HORZ);
			hitTest.pt.y = ptList.y;

			m_cList.GetHeaderCtrl()->SendMessage(HDM_HITTEST, 0, (LPARAM)&hitTest);

			if((hitTest.flags & (HHT_ONHEADER | HHT_ONDIVIDER | HHT_ONDIVOPEN | HHT_NOWHERE)) > 0)
			{
				m_bDisplayDialog = TRUE;
				m_menuColView.LoadMenu(IDR_COLVIEW_MENU);

				m_menuColView.SetMenuDrawMode(BCMENU_DRAWMODE_XP); // <<<!=>>> BCMENU_DRAWMODE_ORIGINAL
				m_menuColView.SetSelectDisableMode(FALSE);
				m_menuColView.SetXPBitmap3D(TRUE);
				m_menuColView.SetBitmapBackground(RGB(255, 0, 255));
				m_menuColView.SetIconSize(16, 16);

				m_menuColView.LoadToolbar(IDR_INFOICONS, IDB_INFOICONS_EX);

				BCMenu *psub = NewGUI_GetBCMenu(m_menuColView.GetSubMenu(0));
				if(psub != NULL)
				{
					_TranslateMenu(psub, TRUE, NULL);
					_EnableViewMenuItems(psub);
					psub->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, AfxGetMainWnd());
				}
				else { ASSERT(FALSE); }
				m_menuColView.DestroyMenu();
				m_bDisplayDialog = FALSE;
			}
		}
	}

	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CPwSafeDlg::OnReCopySel()
{
	NotifyUserActivity();
	m_reEntryView.Copy();
}

void CPwSafeDlg::OnUpdateReCopySel(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_reEntryView.GetSelText().IsEmpty() ? FALSE : TRUE);
}

void CPwSafeDlg::OnReCopyAll()
{
	NotifyUserActivity();

	long lStart, lEnd;
	m_reEntryView.GetSel(lStart, lEnd);
	m_reEntryView.SetSel(0, -1);
	m_reEntryView.Copy();
	m_reEntryView.SetSel(lStart, lEnd);
}

void CPwSafeDlg::OnReSelectAll()
{
	NotifyUserActivity();
	m_reEntryView.SetSel(0, -1);
}

void CPwSafeDlg::OnExtrasTanWizard()
{
	NotifyUserActivity();

	const DWORD dwCurGroupId = GetSelectedGroupId();

	ASSERT(dwCurGroupId != DWORD_MAX); if(dwCurGroupId == DWORD_MAX) return;

	m_bDisplayDialog = TRUE;

	if(_CallPlugins(KPM_TANWIZARD_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	CTanWizardDlg dlg;

	const PW_GROUP *pgTo = m_mgr.GetGroupById(dwCurGroupId);
	if(pgTo != NULL) dlg.m_strGroupName = pgTo->pszGroupName;

	if(dlg.DoModal() == IDOK)
	{
		PW_ENTRY pwTemplate;
		memset(&pwTemplate, 0, sizeof(PW_ENTRY));

		PW_TIME tNow;
		_GetCurrentPwTime(&tNow);

		pwTemplate.tCreation = tNow;
		pwTemplate.tLastMod = tNow;
		pwTemplate.tLastAccess = tNow;
		pwTemplate.pszTitle = const_cast<LPTSTR>(PWS_TAN_ENTRY);
		pwTemplate.pszURL = _T("");
		pwTemplate.pszAdditional = _T("");
		m_mgr.GetNeverExpireTime(&pwTemplate.tExpire);
		pwTemplate.uImageId = 29;
		pwTemplate.uGroupId = dwCurGroupId;
		pwTemplate.pBinaryData = NULL;
		pwTemplate.pszBinaryDesc = _T("");
		pwTemplate.uBinaryDataLen = 0;

		const bool bNatural = NSCAPI_Supported();
		DWORD dwNumber = dlg.m_dwStartNumber;

		TCHAR tchTestSep = _T('!');
		while(dlg.m_strTans.Find(tchTestSep) >= 0) { ++tchTestSep; }
		dlg.m_strTans += tchTestSep; // Append terminating char

		CString strSubString;

		BOOL bValidSubString = FALSE;
		for(int i = 0; i < dlg.m_strTans.GetLength(); ++i)
		{
			const TCHAR tch = dlg.m_strTans.GetAt(i);

			BOOL bAcceptable = FALSE;
			if(dlg.m_strTANChars.Find(tch) >= 0) bAcceptable = TRUE;

			if((bAcceptable == TRUE) && (bValidSubString == FALSE))
			{
				strSubString = tch;
				bValidSubString = TRUE;
			}
			else if((bAcceptable == TRUE) && (bValidSubString == TRUE))
			{
				strSubString += tch;
			}
			else if((bAcceptable == FALSE) && (bValidSubString == TRUE))
			{
				ZeroMemory(&pwTemplate.uuid[0], 16);
				pwTemplate.pszPassword = const_cast<LPTSTR>((LPCTSTR)strSubString);
				pwTemplate.uPasswordLen = strSubString.GetLength();

				CString strNumberTemp;
				if(dlg.m_bAssignNumbers == FALSE)
					pwTemplate.pszUserName = _T("");
				else
				{
					strNumberTemp.Format(bNatural ? _T("%u") : _T("%03u"), dwNumber);
					pwTemplate.pszUserName = const_cast<LPTSTR>((LPCTSTR)strNumberTemp);
					++dwNumber;
				}

				VERIFY(m_mgr.AddEntry(&pwTemplate));

				bValidSubString = FALSE;
				EraseCString(&strSubString);
			}
		}

		EraseCString(&dlg.m_strTans);

		_SortListIfAutoSort();
		if(m_nAutoSort == 0) UpdatePasswordList();
		m_bModified = TRUE;
	}

	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdateExtrasTanWizard(CCmdUI* pCmdUI)
{
	BOOL b = m_bFileOpen;
	b &= ((m_mgr.GetNumberOfGroups() > 0) ? TRUE : FALSE);
	pCmdUI->Enable(b);
}

void CPwSafeDlg::OnFilePrintPreview()
{
	NotifyUserActivity();

	CPwExport cExp;
	PWEXPORT_OPTIONS pwo;

	if(m_bFileOpen == FALSE) return;
	if(m_bForceAllowPrinting == FALSE)
	{
		if(IsUnsafeAllowed(this->m_hWnd) == FALSE) return;
	}

	m_bDisplayDialog = TRUE;

	cExp.SetManager(&m_mgr);
	cExp.SetNewLineSeq(m_bWindowsNewLine);
	cExp.SetFormat(PWEXP_HTML);

	if(_CallPlugins(KPM_FILE_PRINTPREVIEW_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	if(GetExportOptions(&pwo, &cExp, TRUE) == FALSE) { m_bDisplayDialog = FALSE; return; }

	std::basic_string<TCHAR> tszFile = WU_GetTempFile(_T(".html"));

	const BOOL bRet = cExp.ExportGroup(tszFile.c_str(), DWORD_MAX, &pwo, NULL); // Export all: set group ID to DWORD_MAX
	if(bRet == FALSE)
	{
		MessageBox(TRL("Cannot open temporary file for printing!"), PWM_PRODUCT_NAME_SHORT,
			MB_OK | MB_ICONWARNING);
		m_bDisplayDialog = FALSE;
		return;
	}

	ShellExecute(m_hWnd, NULL, tszFile.c_str(), NULL, NULL, SW_SHOW);

	m_vTempFiles.push_back(tszFile);
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnInfoTranslation()
{
	NotifyUserActivity();

	CString str = TRL("Currently Used Language"); str += _T(": ");
	if(_tcscmp(TRL("~LANGUAGENAME"), _T("~LANGUAGENAME")) != 0)
	{
		str += TRL("~LANGUAGENAME");
		str += _T(" (");
		LPCTSTR lpName = GetCurrentTranslationTable();
		str += ((lpName != NULL) ? lpName : _T("<?>"));
		str += _T(".lng)");
	}
	else str += _T("Unknown or English version");
	str += _T("\r\n\r\n");

	str += TRL("Language File Version"); str += _T(": ");
	if(_tcscmp(TRL("~LANGUAGEVERSION"), _T("~LANGUAGEVERSION")) != 0)
		str += TRL("~LANGUAGEVERSION");
	else str += _T("Unknown or English version");
	str += _T("\r\n");

	str += TRL("Author"); str += _T(": ");
	if(_tcscmp(TRL("~LANGUAGEAUTHOR"), _T("~LANGUAGEAUTHOR")) != 0)
		str += TRL("~LANGUAGEAUTHOR");
	else str += _T("Unknown or English version");
	str += _T("\r\n");

	str += TRL("Translation Author Contact"); str += _T(": ");
	if(_tcscmp(TRL("~LANGUAGEAUTHOREMAIL"), _T("~LANGUAGEAUTHOREMAIL")) != 0)
		str += TRL("~LANGUAGEAUTHOREMAIL");
	else str += _T("Unknown or English version");

	MessageBox(str, TRL("Translation information"), MB_OK | MB_ICONINFORMATION);
}

HTREEITEM CPwSafeDlg::_GetLastGroupItem(CTreeCtrl *pTree)
{
	HTREEITEM hPrev = NULL, h;

	h = pTree->GetFirstVisibleItem();

	while(h != NULL)
	{
		hPrev = h;
		h = pTree->GetNextVisibleItem(h);
	}

	return hPrev;
}

void CPwSafeDlg::GroupSyncStates(BOOL bGuiToMgr)
{
	if(m_bFileOpen == FALSE) return;
	if(m_cGroups.GetCount() == 0) return;

	_SyncSubTree(&m_cGroups, m_cGroups.GetRootItem(), bGuiToMgr);
}

void CPwSafeDlg::_SyncSubTree(CTreeCtrl *pTree, HTREEITEM hItem, BOOL bGuiToMgr)
{
	HTREEITEM h;

	ASSERT(pTree != NULL);
	if(hItem == NULL) return;

	h = hItem;
	while(1)
	{
		if(pTree->ItemHasChildren(h) == TRUE)
		{
			_SyncItem(pTree, h, bGuiToMgr);
			_SyncSubTree(pTree, pTree->GetChildItem(h), bGuiToMgr);
		}

		h = pTree->GetNextSiblingItem(h);
		if(h == NULL) break;
	}
}

void CPwSafeDlg::_SyncItem(CTreeCtrl *pTree, HTREEITEM hItem, BOOL bGuiToMgr)
{
	ASSERT((pTree != NULL) && (hItem != NULL));
	if((pTree == NULL) || (hItem == NULL)) return;

	DWORD dwGroupId = (DWORD)pTree->GetItemData(hItem);
	PW_GROUP *pGroup = m_mgr.GetGroupById(dwGroupId);

	if(pGroup != NULL)
	{
		if(bGuiToMgr == TRUE)
		{
			if(pTree->GetItemState(hItem, UINT_MAX) & TVIS_EXPANDED)
				pGroup->dwFlags |= PWGF_EXPANDED; // Set bit
			else
				pGroup->dwFlags &= ~PWGF_EXPANDED; // Remove bit
		}
		else
		{
			if((pGroup->dwFlags & PWGF_EXPANDED) != 0)
				pTree->Expand(hItem, TVE_EXPAND);
			else
				pTree->Expand(hItem, TVE_COLLAPSE);
		}
	}
}

HTREEITEM CPwSafeDlg::_GroupIdToHTreeItem(DWORD_PTR dwGroupId)
{
	if(dwGroupId == DWORD_MAX) return NULL;

	return _FindSelectInTree(&m_cGroups, m_cGroups.GetRootItem(), dwGroupId);
}

HTREEITEM CPwSafeDlg::_FindSelectInTree(CTreeCtrl *pTree, HTREEITEM hRoot, DWORD_PTR dwGroupId)
{
	HTREEITEM h;

	ASSERT(pTree != NULL);
	if(hRoot == NULL) return NULL;

	h = hRoot;
	while(1)
	{
		if(pTree->GetItemData(h) == dwGroupId) return h;

		if(pTree->ItemHasChildren(h) == TRUE)
		{
			HTREEITEM hSub = _FindSelectInTree(pTree, pTree->GetChildItem(h), dwGroupId);
			if(hSub != NULL) return hSub;
		}

		h = pTree->GetNextSiblingItem(h);
		if(h == NULL) break;
	}

	return NULL;
}

void CPwSafeDlg::OnSafeAddSubgroup()
{
	NotifyUserActivity();

	m_bDisplayDialog = TRUE;
	if(_CallPlugins(KPM_GROUP_ADD_SUBGROUP_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }
	m_bDisplayDialog = FALSE;

	DWORD dwGroupCount = m_mgr.GetNumberOfGroups();
	ASSERT(dwGroupCount >= 1); if(dwGroupCount == 0) return;

	HTREEITEM hItem = m_cGroups.GetSelectedItem();
	ASSERT(hItem != NULL); if(hItem == NULL) return;

	DWORD dwRealParentId = (DWORD)m_cGroups.GetItemData(hItem);
	ASSERT(dwRealParentId != DWORD_MAX); if(dwRealParentId == DWORD_MAX) return;

	_Groups_SaveView();
	OnSafeAddGroup();

	// Have we added a group?
	if(m_mgr.GetNumberOfGroups() == (dwGroupCount + 1))
	{
		PW_GROUP *pRealParent = m_mgr.GetGroupById(dwRealParentId);
		ASSERT(pRealParent != NULL); if(pRealParent == NULL) return;

		DWORD dwVParentPos = m_mgr.GetLastChildGroup(m_mgr.GetGroupByIdN(dwRealParentId));
		ASSERT(dwVParentPos != DWORD_MAX);
		if(dwVParentPos == DWORD_MAX) dwVParentPos = m_mgr.GetGroupByIdN(dwRealParentId);

		PW_GROUP *pVParent = m_mgr.GetGroup(dwVParentPos);
		ASSERT(pVParent != NULL); if(pVParent == NULL) return;

		DWORD dwVParentGroupId = pVParent->uGroupId;
		ASSERT(dwVParentGroupId != DWORD_MAX); if(dwVParentGroupId == DWORD_MAX) return;

		ASSERT(dwVParentPos <= m_mgr.GetNumberOfGroups());
		ASSERT(m_mgr.GetGroupById(dwVParentGroupId) != NULL);

		// Get the position of the added group
		DWORD dwNewGroupPos = m_mgr.GetNumberOfGroups() - 1;

		DWORD dwNewGroupId = m_mgr.GetGroup(dwNewGroupPos)->uGroupId;

		// Set new level of the added group and expand its parent
		m_mgr.GetGroup(dwNewGroupPos)->usLevel = (USHORT)(pRealParent->usLevel + 1);
		pRealParent->dwFlags |= PWGF_EXPANDED;
		GroupSyncStates(FALSE); // 'Send' expanded flag to GUI

		// Move it to the correct position
		m_mgr.MoveGroup(dwNewGroupPos, dwVParentPos + 1);

		UpdateGroupList();
		_Groups_RestoreView();

		HTREEITEM h = _GroupIdToHTreeItem(dwNewGroupId);
		if(h != NULL) m_cGroups.SelectItem(h);
		UpdatePasswordList();
	}
}

void CPwSafeDlg::OnUpdateSafeAddSubgroup(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((m_hLastSelectedGroup != NULL) ? TRUE : FALSE);
}

void CPwSafeDlg::OnBeginDragGrouplist(NMHDR* pNMHDR, LRESULT* pResult)
{
	NotifyUserActivity();

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW *)pNMHDR;
	*pResult = 0;

	m_bDisplayDialog = TRUE;

	CImageList* pDragImageList = NULL;
	POINT ptOffset;
	RECT rcItem;

	pDragImageList = m_cGroups.CreateDragImage(pNMTreeView->itemNew.hItem);
	ASSERT(pDragImageList != NULL); if(pDragImageList == NULL) { m_bDisplayDialog = FALSE; return; }

	if(m_cGroups.GetItemRect(pNMTreeView->itemNew.hItem, &rcItem, TRUE) != FALSE)
	{
		CPoint ptDragBegin;
		int nX = 0, nY = 0;

		ptDragBegin = pNMTreeView->ptDrag;
		ImageList_GetIconSize(pDragImageList->GetSafeHandle(), &nX, &nY);
		ptOffset.x = (ptDragBegin.x - rcItem.left) + (nX - (rcItem.right - rcItem.left));
		ptOffset.y = (ptDragBegin.y - rcItem.top) + (nY - (rcItem.bottom - rcItem.top));

		MapWindowPoints(NULL, &rcItem);
	}
	else
	{
		GetWindowRect(&rcItem);
		ptOffset.x = ptOffset.y = 8;
	}

	BOOL bDragBegun = pDragImageList->BeginDrag(0, ptOffset);
	if(bDragBegun == FALSE)
	{
		SAFE_DELETE(pDragImageList); ASSERT(FALSE); m_bDisplayDialog = FALSE; return;
	}

	CPoint ptDragEnter = pNMTreeView->ptDrag;
	ClientToScreen(&ptDragEnter);
	if(pDragImageList->DragEnter(NULL, ptDragEnter) == FALSE)
	{
		SAFE_DELETE(pDragImageList); ASSERT(FALSE); m_bDisplayDialog = FALSE; return;
	}

	SAFE_DELETE(pDragImageList);

	this->SetFocus();
	InvalidateRect(&rcItem, TRUE);
	UpdateWindow();
	SetCapture();

	m_hDraggingGroup = pNMTreeView->itemNew.hItem;

	// if(m_cGroups.ItemHasChildren(m_hDraggingGroup) == TRUE) m_bCanDragGroup = FALSE;
	// else m_bCanDragGroup = TRUE;

	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnCancelMode()
{
	NotifyUserActivity();

	_FinishDragging(TRUE);
	RedrawWindow();
	CDialog::OnCancelMode();
}

void CPwSafeDlg::_FinishDragging(BOOL bDraggingImageList)
{
	if(m_hDraggingGroup != NULL)
	{
		if(bDraggingImageList == TRUE)
		{
			CImageList::DragLeave(NULL);
			CImageList::EndDrag();
		}

		m_hDraggingGroup = NULL;
		ReleaseCapture();
		ShowCursor(TRUE);
		m_cGroups.SelectDropTarget(NULL);
	}
}

void CPwSafeDlg::OnGroupSort()
{
	NotifyUserActivity();

	if((m_bFileOpen == FALSE) || (m_bLocked == TRUE)) return;

	if(_CallPlugins(KPM_GROUP_SORT_PRE, 0, 0) == FALSE) return;

	m_mgr.SortGroupList();
	UpdateGroupList();
	UpdatePasswordList();
	m_bModified = TRUE;
}

void CPwSafeDlg::OnUpdateGroupSort(CCmdUI* pCmdUI)
{
	BOOL b = m_bFileOpen;
	b &= ((m_mgr.GetNumberOfGroups() != 0) ? TRUE : FALSE);
	pCmdUI->Enable(b);
}

BOOL CPwSafeDlg::_CheckIfCanSort()
{
	if(m_nAutoSort == 0) return TRUE;

	MessageBox(TRL("Auto-sorting of the password list is enabled, you cannot sort the list manually now."),
		PWM_PRODUCT_NAME_SHORT, MB_ICONWARNING | MB_OK);
	return FALSE;
}

void CPwSafeDlg::OnPwlistSortTitle()
{
	NotifyUserActivity();
	if(_CheckIfCanSort() == FALSE) return;
	_SortList(0, FALSE);
}

void CPwSafeDlg::OnUpdatePwlistSortTitle(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnPwlistSortUser()
{
	NotifyUserActivity();
	if(_CheckIfCanSort() == FALSE) return;
	_SortList(1, FALSE);
}

void CPwSafeDlg::OnUpdatePwlistSortUser(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnPwlistSortUrl()
{
	NotifyUserActivity();
	if(_CheckIfCanSort() == FALSE) return;
	_SortList(2, FALSE);
}

void CPwSafeDlg::OnUpdatePwlistSortUrl(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnPwlistSortPassword()
{
	NotifyUserActivity();
	if(_CheckIfCanSort() == FALSE) return;
	_SortList(3, FALSE);
}

void CPwSafeDlg::OnUpdatePwlistSortPassword(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnPwlistSortNotes()
{
	NotifyUserActivity();
	if(_CheckIfCanSort() == FALSE) return;
	_SortList(4, FALSE);
}

void CPwSafeDlg::OnUpdatePwlistSortNotes(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnPwlistSortCreation()
{
	NotifyUserActivity();
	if(_CheckIfCanSort() == FALSE) return;
	_SortList(5, FALSE);
}

void CPwSafeDlg::OnUpdatePwlistSortCreation(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnPwlistSortLastmodify()
{
	NotifyUserActivity();
	if(_CheckIfCanSort() == FALSE) return;
	_SortList(6, FALSE);
}

void CPwSafeDlg::OnUpdatePwlistSortLastmodify(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnPwlistSortLastaccess()
{
	NotifyUserActivity();
	if(_CheckIfCanSort() == FALSE) return;
	_SortList(7, FALSE);
}

void CPwSafeDlg::OnUpdatePwlistSortLastaccess(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnPwlistSortExpire()
{
	NotifyUserActivity();
	if(_CheckIfCanSort() == FALSE) return;
	_SortList(8, FALSE);
}

void CPwSafeDlg::OnUpdatePwlistSortExpire(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnGroupMoveLeft()
{
	NotifyUserActivity();

	DWORD dwGroupId = GetSelectedGroupId();
	PW_GROUP *pGroup = m_mgr.GetGroupById(dwGroupId);
	ASSERT(pGroup != NULL); if(pGroup == NULL) return;

	if(pGroup->usLevel != 0) pGroup->usLevel--;
	m_mgr.FixGroupTree();
	_Groups_SaveView(); UpdateGroupList(); _Groups_RestoreView();
	m_bModified = TRUE;
}

void CPwSafeDlg::OnUpdateGroupMoveLeft(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((m_hLastSelectedGroup != NULL) ? TRUE : FALSE);
}

void CPwSafeDlg::OnGroupMoveRight()
{
	NotifyUserActivity();

	DWORD dwGroupId = GetSelectedGroupId();
	DWORD dwGroupPos = m_mgr.GetGroupByIdN(dwGroupId);
	PW_GROUP *pGroup = m_mgr.GetGroup(dwGroupPos);
	PW_GROUP *pParent;
	if(dwGroupPos != 0) pParent = m_mgr.GetGroup(dwGroupPos - 1); else pParent = NULL;
	ASSERT(pGroup != NULL); if(pGroup == NULL) return;

	if(pGroup->usLevel != 0xFFFF) pGroup->usLevel++;
	if(pParent != NULL) pParent->dwFlags |= PWGF_EXPANDED;
	m_mgr.FixGroupTree();
	_Groups_SaveView(); UpdateGroupList(); _Groups_RestoreView();
	m_bModified = TRUE;
}

void CPwSafeDlg::OnUpdateGroupMoveRight(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((m_hLastSelectedGroup != NULL) ? TRUE : FALSE);
}

void CPwSafeDlg::OnViewHideUsers()
{
	NotifyUserActivity();

	UINT uState;
	BOOL bChecked;
	int nItem;
	LV_ITEM lvi;

	if(m_bUserStars == TRUE)
	{
		if(IsUnsafeAllowed(this->m_hWnd) == FALSE) return;
	}

	uState = m_menu.GetMenuState(ID_VIEW_HIDEUSERS, MF_BYCOMMAND);
	ASSERT(uState != 0xFFFFFFFF);

	if(uState & MF_CHECKED) bChecked = TRUE;
	else bChecked = FALSE;

	if(bChecked == TRUE)
	{
		uState = MF_UNCHECKED; // Toggle
		m_bUserStars = FALSE;
	}
	else
	{
		uState = MF_CHECKED; // Toggle
		m_bUserStars = TRUE;
	}

	m_menu.CheckMenuItem(ID_VIEW_HIDEUSERS, MF_BYCOMMAND | uState);

	if(m_bUserStars == TRUE)
	{
		ZeroMemory(&lvi, sizeof(LV_ITEM));
		lvi.mask = LVIF_TEXT;
		lvi.iSubItem = 1;
		lvi.pszText = PWM_PASSWORD_STRING;
		for(nItem = 0; nItem < m_cList.GetItemCount(); ++nItem)
		{
			lvi.iItem = nItem;
			m_cList.SetItem(&lvi);
		}
	}
	else
		RefreshPasswordList(); // Refresh list based on UUIDs

	// m_bCachedToolBarUpdate = TRUE;
	_UpdateToolBar(TRUE);
}

void CPwSafeDlg::OnViewAttach()
{
	NotifyUserActivity();
	_SelChangeView(ID_VIEW_ATTACH);
}

void CPwSafeDlg::OnPwlistSaveAttach()
{
	NotifyUserActivity();

	DWORD dwFlags;
	PW_ENTRY *pEntry;
	DWORD dwSelectedInx = _ListSelToEntryIndex();

	if(m_bFileOpen == FALSE) return;
	if(dwSelectedInx == DWORD_MAX) return;

	pEntry = m_mgr.GetEntry(dwSelectedInx);
	ASSERT(pEntry != NULL); if(pEntry == NULL) return;

	m_bDisplayDialog = TRUE;

	if(_tcslen(pEntry->pszBinaryDesc) == 0)
	{
		MessageBox(TRL("There is no file attached with this entry."),
			PWM_PRODUCT_NAME_SHORT, MB_ICONINFORMATION | MB_OK);
		m_bDisplayDialog = FALSE;
		return;
	}

	CString strSample = pEntry->pszBinaryDesc;

	CString strFilter = TRL("All Files");
	strFilter += _T(" (*.*)|*.*||");

	dwFlags = (OFN_LONGNAMES | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
	dwFlags |= OFN_EXTENSIONDIFFERENT;
	dwFlags |= (OFN_EXPLORER | OFN_ENABLESIZING | OFN_NOREADONLYRETURN);
	
	CFileDialog dlg(FALSE, NULL, strSample, dwFlags, strFilter, this);
	if(dlg.DoModal() == IDOK)
		CPwUtil::SaveBinaryData(pEntry, dlg.GetPathName());

	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdatePwlistSaveAttach(CCmdUI* pCmdUI)
{
	BOOL bEnable = TRUE;

	if(m_dwLastFirstSelectedItem == DWORD_MAX) bEnable = FALSE;
	if(m_dwLastNumSelectedItems != 1) bEnable = FALSE;

	if((m_dwLastEntryIndex != DWORD_MAX) && (m_dwLastEntryIndex < m_mgr.GetNumberOfEntries()))
	{
		const PW_ENTRY *p = m_mgr.GetEntry(m_dwLastEntryIndex);
		if(p != NULL) bEnable &= ((p->pBinaryData != NULL) ? TRUE : FALSE);
	}

	pCmdUI->Enable(bEnable);
}

void CPwSafeDlg::_ProcessGroupKey(UINT nChar, UINT nFlags)
{
	NotifyUserActivity();

	if((nFlags & 0x2000) > 0)
	{
		if(nChar == VK_UP) OnGroupMoveUp();
		else if(nChar == VK_DOWN) OnGroupMoveDown();
		else if(nChar == VK_HOME) OnGroupMoveTop();
		else if(nChar == VK_END) OnGroupMoveBottom();
		else if(nChar == VK_LEFT) OnGroupMoveLeft();
		else if(nChar == VK_RIGHT) OnGroupMoveRight();
	}
	else
	{
		if((nChar == VK_UP) || (nChar == VK_DOWN) || (nChar == VK_HOME) ||
			(nChar == VK_END) || (nChar == VK_PRIOR) || (nChar == VK_NEXT) ||
			(nChar == VK_LEFT) || (nChar == VK_RIGHT))
			m_bCachedPwlistUpdate = TRUE;
		else
			m_bCachedPwlistUpdate = FALSE;
	}
}

void CPwSafeDlg::_ProcessListKey(UINT nChar, BOOL bAlt)
{
	NotifyUserActivity();

	if(bAlt == TRUE)
	{
		if(nChar == VK_UP) OnPwlistMoveUp();
		else if(nChar == VK_DOWN) OnPwlistMoveDown();
		else if(nChar == VK_HOME) OnPwlistMoveTop();
		else if(nChar == VK_END) OnPwlistMoveBottom();
	}
	else
	{
		if(nChar == VK_DELETE) OnPwlistDelete();
	}
}

void CPwSafeDlg::CB_OnPwlistColumnWidthChange(int iColumn, int iSize)
{
	ASSERT((iColumn >= 0) && (iColumn < 11));

	if(m_dwPwListMode != LVS_REPORT) return;

	if((iColumn >= 0) && (iColumn < 11))
	{
		if(m_bShowColumn[iColumn] == TRUE)
			m_nColumnWidths[iColumn]= iSize;
	}
}

BOOL CPwSafeDlg::IsUnsafeAllowed(HWND hWndParent)
{
	if(m_bDisableUnsafeAtStart == FALSE) return TRUE;

	CString str = TRL("Unsafe operations are disabled.");
	str += _T("\r\n\r\n");
	str += TRL("To execute this operation you must enable unsafe operations in the options dialog.");
	::MessageBox(hWndParent, (LPCTSTR)str, PWM_PRODUCT_NAME_SHORT, MB_ICONWARNING | MB_OK);

	return FALSE;
}

void CPwSafeDlg::OnFileShowDbInfo()
{
	NotifyUserActivity();

	CDbSettingsDlg dlg;

	m_bDisplayDialog = TRUE;

	dlg.m_nAlgorithm = m_mgr.GetAlgorithm();
	const int nAlgorithmOld = dlg.m_nAlgorithm;

	dlg.m_dwNumKeyEnc = m_mgr.GetKeyEncRounds();
	const DWORD dwOldKeyEncRounds = dlg.m_dwNumKeyEnc;

	CString strName = m_mgr.GetPropertyString(PWP_DEFAULT_USER_NAME).c_str();
	dlg.m_strDefaultUserName = strName; // Copy

	const COLORREF clrOld = m_mgr.GetColor();
	dlg.m_clr = clrOld;

	if(_CallPlugins(KPM_FILE_DBSETTINGS_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	if(dlg.DoModal() == IDOK)
	{
		m_mgr.SetAlgorithm(dlg.m_nAlgorithm);
		if(nAlgorithmOld != dlg.m_nAlgorithm) m_bModified = TRUE;

		m_mgr.SetKeyEncRounds(dlg.m_dwNumKeyEnc);
		if(dwOldKeyEncRounds != dlg.m_dwNumKeyEnc) m_bModified = TRUE;

		if(_tcscmp((LPCTSTR)dlg.m_strDefaultUserName, (LPCTSTR)strName) != 0)
		{
			VERIFY(m_mgr.SetPropertyString(PWP_DEFAULT_USER_NAME,
				(LPCTSTR)dlg.m_strDefaultUserName));
			m_bModified = TRUE;
		}

		m_mgr.SetColor(dlg.m_clr);
		if(clrOld != dlg.m_clr)
		{
			m_bModified = TRUE;
			_UpdateTrayIcon();
		}
	}

	NotifyUserActivity();
	_UpdateToolBar();

	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdateFileShowDbInfo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::_ShowExpiredEntries(BOOL bShowIfNone, BOOL bShowExpired, BOOL bShowSoonToExpire)
{
	NotifyUserActivity();

	if(m_bFileOpen == FALSE) return;
	if(m_bGlobalAutoTypePending == TRUE) return;

	m_bDisplayDialog = TRUE;

	PW_TIME tNow;
	_GetCurrentPwTime(&tNow);
	const DWORD dwDateNow = ((DWORD)tNow.shYear * 13 * 32) + ((DWORD)tNow.btMonth * 32) +
		((DWORD)tNow.btDay & 0xFF);
	const DWORD dwSoonToExpireDays = _GetSoonToExpireDays();

	CEntryListDlg dlg;
	dlg.m_pMgr = &m_mgr;
	dlg.m_bPasswordStars = m_bPasswordStars;
	dlg.m_bUserStars = m_bUserStars;
	dlg.m_pImgList = &m_ilIcons;
	dlg.m_strBannerTitle = TRL("Expired Entries");
	CString strExpText = TRL("This is a list of all expired entries");
	dlg.m_strBannerCaption = strExpText + _T(".");
	ZeroMemory(dlg.m_aUuid, 16);

	if((bShowExpired == TRUE) && (bShowSoonToExpire == TRUE))
		dlg.m_nDisplayMode = ELDMODE_EXPSOONEXP;
	else if(bShowExpired == TRUE)
		dlg.m_nDisplayMode = ELDMODE_EXPIRED;
	else if(bShowSoonToExpire == TRUE)
		dlg.m_nDisplayMode = ELDMODE_SOONTOEXP;
	else { ASSERT(FALSE); m_bDisplayDialog = FALSE; return; }

	if(bShowIfNone == FALSE)
	{
		BOOL bAtLeastOneExpired = FALSE;
		const DWORD dwInvalidGroup1 = m_mgr.GetGroupId(PWS_BACKUPGROUP_SRC);
		const DWORD dwInvalidGroup2 = m_mgr.GetGroupId(PWS_BACKUPGROUP);

		for(DWORD i = 0; i < m_mgr.GetNumberOfEntries(); ++i)
		{
			PW_ENTRY *pe = m_mgr.GetEntry(i);
			ASSERT(pe != NULL); if(pe == NULL) continue;

			if((pe->uGroupId == dwInvalidGroup1) || (pe->uGroupId == dwInvalidGroup2)) continue;
			if(CPwUtil::IsTANEntry(pe) == TRUE) continue;

			if(bShowExpired == TRUE)
			{
				if(_pwtimecmp(&tNow, &pe->tExpire) > 0)
				{
					bAtLeastOneExpired = TRUE;
					break;
				}
			}

			if(bShowSoonToExpire == TRUE)
			{
				const DWORD dwDate = ((DWORD)pe->tExpire.shYear * 13 * 32) +
					((DWORD)pe->tExpire.btMonth * 32) + ((DWORD)pe->tExpire.btDay & 0xFF);

				if((dwDate >= dwDateNow) && ((dwDate - dwDateNow) <= dwSoonToExpireDays))
				{
					bAtLeastOneExpired = TRUE;
					break;
				}
			}
		}

		if(bAtLeastOneExpired == FALSE) { m_bDisplayDialog = FALSE; return; }
	}

	if(dlg.DoModal() == IDOK)
	{
		if(memcmp(dlg.m_aUuid, g_uuidZero, 16) != 0)
		{
			PW_ENTRY *p = m_mgr.GetEntryByUuid(dlg.m_aUuid);

			ASSERT(p != NULL);
			if(p != NULL)
			{
				UpdateGroupList();
				HTREEITEM h = _GroupIdToHTreeItem(p->uGroupId);

				m_cGroups.EnsureVisible(h);
				m_cGroups.SelectItem(h);

				UpdatePasswordList();

				DWORD dwPos = m_mgr.GetEntryPosInGroup(p);
				ASSERT(dwPos != DWORD_MAX);

				m_cList.EnsureVisible(static_cast<int>(dwPos), FALSE);
				m_cList.FocusItem(static_cast<int>(dwPos), TRUE);

				m_cList.SetFocus();

				// Overwrite locked view parameters, so the unlocking
				// method doesn't destroy the current view
				m_nLockedViewParams[1] = GetSelectedEntry();
				m_nLockedViewParams[2] = m_cList.GetTopIndex();
			}
		}
	}

	NotifyUserActivity();
	_UpdateToolBar();

	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnExtrasShowExpired()
{
	_ShowExpiredEntries(TRUE, TRUE, FALSE);
}

void CPwSafeDlg::OnUpdateExtrasShowExpired(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnImportPvault()
{
	NotifyUserActivity();
	m_bDisplayDialog = TRUE;

	CString strFilter = TRL("Text Files");
	strFilter += _T(" (*.txt)|*.txt|");
	strFilter += TRL("All Files");
	strFilter += _T(" (*.*)|*.*||");

	std::vector<std::basic_string<TCHAR> > vFiles = WU_GetFileNames(TRUE, _T("txt"),
		strFilter, TRUE, this, FALSE, TRUE);

	for(size_t iFile = 0; iFile < vFiles.size(); ++iFile)
	{
		CPwImport pvi;
		if(pvi.ImportPVaultToDb(vFiles[iFile].c_str(), &m_mgr) == TRUE)
		{
			_Groups_SaveView(); UpdateGroupList(); _Groups_RestoreView();
			_SortListIfAutoSort();
			if(m_nAutoSort == 0) UpdatePasswordList();
			m_bModified = TRUE;
		}
		else
		{
			CString strMsg = vFiles[iFile].c_str();
			strMsg += _T("\r\n\r\n");
			strMsg += TRL("An error occurred while importing the file. File cannot be imported.");
			MessageBox(strMsg, PWM_PRODUCT_NAME_SHORT, MB_ICONWARNING | MB_OK);
		}
	}

	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdateImportPvault(CCmdUI* pCmdUI)
{
	BOOL b = m_bFileOpen;
	b &= ((m_mgr.GetNumberOfGroups() != 0) ? TRUE : FALSE);
	pCmdUI->Enable(b);
}

void CPwSafeDlg::_HandleEntryDrop(DWORD dwDropType, HTREEITEM hTreeItem)
{
	const DWORD dwToGroupId = (DWORD)m_cGroups.GetItemData(hTreeItem);
	ASSERT(m_mgr.GetGroupById(dwToGroupId) != NULL); if(m_mgr.GetGroupById(dwToGroupId) == NULL) return;

	if((dwDropType != DROPEFFECT_MOVE) && (dwDropType != DROPEFFECT_COPY)) { ASSERT(FALSE); return; }

	CString strGroupTest;
	const PW_GROUP *pGroupTest = m_mgr.GetGroupById(dwToGroupId);
	if(CPwUtil::IsAllowedStoreGroup(pGroupTest->pszGroupName, PWS_SEARCHGROUP) == FALSE)
	{
		MessageBox(TRL("The group you selected cannot store entries. Please select a different group."),
			PWM_PRODUCT_NAME_SHORT, MB_ICONWARNING | MB_OK);
		return;
	}

	PW_TIME tNow;
	_GetCurrentPwTime(&tNow);

	for(DWORD i = 0; i < static_cast<DWORD>(m_cList.GetItemCount()); ++i)
	{
		const UINT uState = m_cList.GetItemState(static_cast<int>(i), LVIS_SELECTED);

		if((uState & LVIS_SELECTED) != 0)
		{
			const DWORD dwIndex = _ListSelToEntryIndex(i); // Uses UUID to get the entry
			ASSERT(dwIndex != DWORD_MAX); if(dwIndex == DWORD_MAX) continue;

			PW_ENTRY *p = m_mgr.GetEntry(dwIndex);
			ASSERT(p != NULL); if(p == NULL) continue;

			if(dwDropType == DROPEFFECT_MOVE)
			{
				p->tLastAccess = tNow;
				p->tLastMod = tNow;
				p->uGroupId = dwToGroupId;
			}
			else if(dwDropType == DROPEFFECT_COPY)
			{
				m_mgr.UnlockEntryPassword(p);
				CString strPasswordCopy = p->pszPassword;
				m_mgr.LockEntryPassword(p);

				PW_ENTRY pwT;
				memcpy(&pwT, p, sizeof(PW_ENTRY)); // Copy entry
				ZeroMemory(pwT.uuid, 16 * sizeof(BYTE)); // Create new UUID
				pwT.uGroupId = dwToGroupId; // Set group ID
				pwT.pszPassword = const_cast<LPTSTR>((LPCTSTR)strPasswordCopy);
				pwT.tLastAccess = tNow;
				pwT.tLastMod = tNow;
				m_mgr.AddEntry(&pwT); // Add as new entry

				EraseCString(&strPasswordCopy);
			}
			else { ASSERT(FALSE); }
		}
	}

	m_cGroups.m_drop.SetDragAccept(FALSE);

	// Full GUI update
	_Groups_SaveView(TRUE);
	_List_SaveView();
	UpdateGroupList();
	_Groups_RestoreView();
	UpdatePasswordList();
	_List_RestoreView();

	m_bModified = TRUE;
}

void CPwSafeDlg::SetStatusTextEx(LPCTSTR lpStatusText, int nPane)
{
	if(nPane == -1) nPane = 2;
	ASSERT(nPane < 3); if(nPane >= 3) nPane = 2;

	if(::IsWindow(m_sbStatus.m_hWnd) == TRUE)
		m_sbStatus.SetText(lpStatusText, nPane, 0);
}

void CPwSafeDlg::_DoQuickFind(LPCTSTR lpText)
{
	const DWORD dwFlags = PWMF_TITLE | PWMF_USER | PWMF_URL | PWMF_PASSWORD |
		PWMF_ADDITIONAL | PWMF_GROUPNAME;

	LPCTSTR lpSearch = lpText;
	if(lpSearch == NULL)
	{
		UpdateData(TRUE);
		lpSearch = m_strQuickFind;
	}

	if(_tcscmp(lpSearch, HCMBX_CLEARLIST) == 0)
	{
		m_mgr.AccessPropertyStrArray(PWPA_SEARCH_HISTORY)->clear();
		m_cQuickFind.ResetContent();
		_UpdateToolBar(TRUE);
		return;
	}
	else if(_tcscmp(lpSearch, HCMBX_SEPARATOR) == 0)
	{
		_UpdateToolBar(TRUE);
		return;
	}

	m_cList.DeleteAllItemsEx();

	PW_TIME tNow;
	_GetCurrentPwTime(&tNow);

	PW_ENTRY *pwFirst = NULL;
	const DWORD dwMaxItems = m_mgr.GetNumberOfEntries();

	// Create the search group if it doesn't exist already
	DWORD dwGroupId = m_mgr.GetGroupId(PWS_SEARCHGROUP);
	if(dwGroupId == DWORD_MAX)
	{
		PW_GROUP pwTemplate;
		pwTemplate.pszGroupName = const_cast<LPTSTR>(PWS_SEARCHGROUP);
		pwTemplate.tCreation = tNow; CPwManager::GetNeverExpireTime(&pwTemplate.tExpire);
		pwTemplate.tLastAccess = tNow; pwTemplate.tLastMod = tNow;
		pwTemplate.uGroupId = 0; // 0 = create new group ID
		pwTemplate.uImageId = 40; // 40 = 'search' icon
		pwTemplate.usLevel = 0; pwTemplate.dwFlags = 0;

		VERIFY(m_mgr.AddGroup(&pwTemplate));
		dwGroupId = m_mgr.GetGroupId(PWS_SEARCHGROUP);
	}
	ASSERT((dwGroupId != DWORD_MAX) && (dwGroupId != 0));
	if((dwGroupId == DWORD_MAX) || (dwGroupId == 0)) return;

	const DWORD dwBackup1 = m_mgr.GetGroupId(PWS_BACKUPGROUP);
	const DWORD dwBackup2 = m_mgr.GetGroupId(PWS_BACKUPGROUP_SRC);
	_UpdateCachedGroupIDs();

	m_bTANsOnly = TRUE;

	DWORD cnt = 0;
	while(true)
	{
		const DWORD dw = m_mgr.FindEx(lpSearch, FALSE, dwFlags, cnt);

		if(dw == DWORD_MAX) break;
		else
		{
			PW_ENTRY *p = m_mgr.GetEntry(dw);
			ASSERT_ENTRY(p);
			if(p == NULL) break;

			const bool bTimeValid = ((m_bQuickFindIncExpired != FALSE) ||
				(_pwtimecmp(&tNow, &p->tExpire) <= 0));

			if(p->uGroupId != dwGroupId) // Not in 'Search Results' group
			{
				if(((m_bQuickFindIncBackup != FALSE) || ((p->uGroupId != dwBackup1) &&
					(p->uGroupId != dwBackup2))) && bTimeValid)
				{
					ASSERT(m_mgr.GetGroupByIdN(p->uGroupId) != DWORD_MAX);

					// The entry could get reallocated by AddEntry, therefore
					// save it to a local CString object
					// m_mgr.UnlockEntryPassword(p);
					// CString strTemp = p->pszPassword;
					// m_mgr.LockEntryPassword(p);

					_List_SetEntry(m_cList.GetItemCount(), p, TRUE, &tNow);

					if(pwFirst == NULL) pwFirst = p;

					// EraseCString(&strTemp); // Destroy the plaintext password
				}
			}
		}

		cnt = dw + 1;
		if(cnt >= dwMaxItems) break;
	}

	_Groups_SaveView();
	UpdateGroupList();
	_Groups_RestoreView();
	HTREEITEM hSelect = _GroupIdToHTreeItem(dwGroupId);
	if(hSelect != NULL)
	{
		m_cGroups.EnsureVisible(hSelect);
		m_cGroups.SelectItem(hSelect);
	}

	if(m_cList.GetItemCount() == 0) m_bTANsOnly = FALSE; // Use report list view
	AdjustPwListMode();

	// m_bModified = TRUE;
	if((m_cList.GetItemCount() > 0) && (pwFirst != NULL))
	{
		m_cList.FocusItem(0, TRUE);
		ShowEntryDetails(pwFirst);
	}
	else ShowEntryDetails(NULL);

	std::vector<std::basic_string<TCHAR> >* pvHistory =
		m_mgr.AccessPropertyStrArray(PWPA_SEARCH_HISTORY);
	NewGUI_ComboBox_UpdateHistory(m_cQuickFind, std::basic_string<TCHAR>(
		lpSearch), pvHistory, PWM_STD_MAX_HISTORYITEMS);

	_UpdateToolBar(TRUE);

	if((m_bFocusResAfterQuickFind == TRUE) && (m_cList.GetItemCount() > 0))
		m_cList.SetFocus();
}

void CPwSafeDlg::NotifyUserActivity()
{
	// m_nLockCountdown = m_nLockTimeDef;

	if(m_nLockTimeDef < 0) m_uLockAt = UINT64_MAX;
	else _SetAutoLockTimeout(m_nLockTimeDef);
}

inline void CPwSafeDlg::_SetAutoLockTimeout(long lSeconds)
{
	m_uLockAt = (_GetCurrentTimeUtc() + (static_cast<UINT64>(lSeconds) *
		0x989680ui64)); // 10000000
}

inline UINT64 CPwSafeDlg::_GetCurrentTimeUtc()
{
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);

	return ((static_cast<UINT64>(ft.dwHighDateTime) << 32) |
		static_cast<UINT64>(ft.dwLowDateTime));
}

void CPwSafeDlg::OnSafeExportGroupTxt()
{
	NotifyUserActivity();
	ExportSelectedGroup(PWEXP_TXT);
}

void CPwSafeDlg::OnUpdateSafeExportGroupTxt(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((m_hLastSelectedGroup != NULL) ? TRUE : FALSE);
}

void CPwSafeDlg::OnPwlistSelectAll()
{
	NotifyUserActivity();

	ASSERT(m_bFileOpen == TRUE); if(m_bFileOpen == FALSE) return;

	m_cList.SetFocus();
	for(int i = 0; i < m_cList.GetItemCount(); ++i)
		m_cList.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);

	_UpdateToolBar(TRUE);
}

void CPwSafeDlg::OnUpdatePwlistSelectAll(CCmdUI* pCmdUI)
{
	BOOL b = m_bFileOpen;
	b &= ((m_cList.GetItemCount() > 0) ? TRUE : FALSE);
	pCmdUI->Enable(b);
}

void CPwSafeDlg::UpdateAutoSortMenuItems()
{
	UINT uCmdID = ID_VIEW_AUTOSORT_NOSORT;

	for(int i = 0; i < 10; ++i)
	{
		switch(i)
		{
			case 0: uCmdID = ID_VIEW_AUTOSORT_NOSORT; break;
			case 1: uCmdID = ID_VIEW_AUTOSORT_TITLE; break;
			case 2: uCmdID = ID_VIEW_AUTOSORT_USER; break;
			case 3: uCmdID = ID_VIEW_AUTOSORT_URL; break;
			case 4: uCmdID = ID_VIEW_AUTOSORT_PASSWORD; break;
			case 5: uCmdID = ID_VIEW_AUTOSORT_NOTES; break;
			case 6: uCmdID = ID_VIEW_AUTOSORT_CREATION; break;
			case 7: uCmdID = ID_VIEW_AUTOSORT_LASTMODIFY; break;
			case 8: uCmdID = ID_VIEW_AUTOSORT_LASTACCESS; break;
			case 9: uCmdID = ID_VIEW_AUTOSORT_EXPIRE; break;
			default: ASSERT(FALSE); break;
		}

		if(i == m_nAutoSort)
			m_menu.CheckMenuItem(uCmdID, MF_BYCOMMAND | MF_CHECKED);
		else
			m_menu.CheckMenuItem(uCmdID, MF_BYCOMMAND | MF_UNCHECKED);
	}
}

void CPwSafeDlg::_SortListIfAutoSort()
{
	if(m_nAutoSort == 0) return;
	_SortList(static_cast<DWORD>(m_nAutoSort - 1), TRUE);
}

void CPwSafeDlg::OnViewAutosortCreation()
{
	NotifyUserActivity();
	m_nAutoSort = 6;
	UpdateAutoSortMenuItems();
	_SortListIfAutoSort();
}

void CPwSafeDlg::OnViewAutosortExpire()
{
	NotifyUserActivity();
	m_nAutoSort = 9;
	UpdateAutoSortMenuItems();
	_SortListIfAutoSort();
}

void CPwSafeDlg::OnViewAutosortLastaccess()
{
	NotifyUserActivity();
	m_nAutoSort = 8;
	UpdateAutoSortMenuItems();
	_SortListIfAutoSort();
}

void CPwSafeDlg::OnViewAutosortLastmodify()
{
	NotifyUserActivity();
	m_nAutoSort = 7;
	UpdateAutoSortMenuItems();
	_SortListIfAutoSort();
}

void CPwSafeDlg::OnViewAutosortNosort()
{
	NotifyUserActivity();
	m_nAutoSort = 0;
	UpdateAutoSortMenuItems();
	_SortListIfAutoSort();
}

void CPwSafeDlg::OnViewAutosortNotes()
{
	NotifyUserActivity();
	m_nAutoSort = 5;
	UpdateAutoSortMenuItems();
	_SortListIfAutoSort();
}

void CPwSafeDlg::OnViewAutosortPassword()
{
	NotifyUserActivity();
	m_nAutoSort = 4;
	UpdateAutoSortMenuItems();
	_SortListIfAutoSort();
}

void CPwSafeDlg::OnViewAutosortTitle()
{
	NotifyUserActivity();
	m_nAutoSort = 1;
	UpdateAutoSortMenuItems();
	_SortListIfAutoSort();
}

void CPwSafeDlg::OnViewAutosortUrl()
{
	NotifyUserActivity();
	m_nAutoSort = 3;
	UpdateAutoSortMenuItems();
	_SortListIfAutoSort();
}

void CPwSafeDlg::OnViewAutosortUser()
{
	NotifyUserActivity();
	m_nAutoSort = 2;
	UpdateAutoSortMenuItems();
	_SortListIfAutoSort();
}

DWORD CPwSafeDlg::_EntryUuidToListPos(BYTE *pUuid)
{
	TCHAR szTemp[66];
	BYTE aUuid[16];

	LV_ITEM lvi;
	ZeroMemory(&lvi, sizeof(LV_ITEM));
	lvi.iSubItem = 9;
	lvi.cchTextMax = 65;
	lvi.pszText = szTemp;
	lvi.mask = LVIF_TEXT;

	const int nItemCount = m_cList.GetItemCount();
	for(int i = 0; i < nItemCount; ++i)
	{
		lvi.iItem = i;
		lvi.pszText = szTemp;
		m_cList.GetItem(&lvi);

		_StringToUuid(lvi.pszText, aUuid);
		if(memcmp(aUuid, pUuid, 16) == 0) return static_cast<DWORD>(i);
	}

	return DWORD_MAX;
}

void CPwSafeDlg::OnUpdateSafeOptions(CCmdUI* pCmdUI)
{
	if(m_bMiniMode == TRUE) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable((((m_bLocked == FALSE) ? TRUE : FALSE) &&
		(!m_bDisplayDialog)) ? TRUE : FALSE);
}

void CPwSafeDlg::_AutoType(PW_ENTRY *pEntry, BOOL bLoseFocus, DWORD dwAutoTypeSeq,
	LPCTSTR lpTargetWindowName)
{
	BOOL bRefreshView = FALSE;

	ASSERT_ENTRY(pEntry); if(pEntry == NULL) return;

	if(m_bDisableAutoType == TRUE) return;

	const BOOL bNoBlockInput = AU_IsWin9xSystem();
	if(bNoBlockInput == FALSE) ::BlockInput(TRUE);

	CString strLookup;
	if(dwAutoTypeSeq == 0) strLookup = _T("auto-type:");
	else strLookup.Format(_T("%s%d%s"), _T("auto-type-"), dwAutoTypeSeq, _T(":"));

	CString str = ExtractParameterFromString(pEntry->pszAdditional, strLookup, 0);
	if(str.GetLength() == 0)
	{
		if(CPwUtil::IsTANEntry(pEntry) == TRUE) // Is it a TAN entry?
		{
			str = _T("{PASSWORD}");

			_GetCurrentPwTime(&pEntry->tExpire);
			m_bModified = TRUE;
			bRefreshView = TRUE;
		}
		else str = m_strDefaultAutoTypeSequence;
	}

	str = SprCompile(str, true, pEntry, &m_mgr, true, false);

	if((m_bAutoTypeIEFix == TRUE) && (lpTargetWindowName != NULL) &&
		((_tcsstr(lpTargetWindowName, _T("Windows Internet Explorer")) != NULL) ||
		(_tcsstr(lpTargetWindowName, _T("Maxthon")) != NULL)))
	{
		str = CString(_T("{DELAY 50}1{DELAY 50}{BACKSPACE}")) + str;
	}

	if(bLoseFocus == TRUE)
	{
		if(m_nAutoTypeMethod == ATM_DROPBACK)
		{
			SafeActivateNextWindow(this->m_hWnd);
		}
		else // m_nAutoTypeMethod == ATM_MINIMIZE
		{
			const BOOL bPrevIgnoreSizeEvent = m_bIgnoreSizeEvent;
			m_bIgnoreSizeEvent = TRUE;

			HWND hMe = ::GetForegroundWindow();

			if(m_bMinimized == FALSE)
			{
				if(m_bMinimizeToTray == TRUE)
					SafeActivateNextWindow(this->m_hWnd);

				ShowWindow(SW_MINIMIZE);
			}
			else SafeActivateNextWindow(hMe);

			const DWORD dwStartTime = timeGetTime();
			while(1)
			{
				if(::GetForegroundWindow() != hMe) break;

				if((timeGetTime() - dwStartTime) > 1000) break;
				Sleep(50);
			}

			m_bIgnoreSizeEvent = bPrevIgnoreSizeEvent;
		}
	}

	Sleep(50);

	CSendKeysEx sk;
	BOOL bToggleCapsLock = FALSE;
	sk.SetDelay(10);

	// sk.SendKeys(_T("(^@+%)"), false); // Release all modifier keys
	sk.SendKeyUp(VK_LSHIFT); sk.SendKeyUp(VK_RSHIFT); sk.SendKeyUp(VK_SHIFT);
	sk.SendKeyUp(VK_LCONTROL); sk.SendKeyUp(VK_RCONTROL); sk.SendKeyUp(VK_CONTROL);
	sk.SendKeyUp(VK_LMENU); sk.SendKeyUp(VK_RMENU); sk.SendKeyUp(VK_MENU);
	sk.SendKeyUp(VK_LWIN); sk.SendKeyUp(VK_RWIN);

	if((GetKeyState(VK_CAPITAL) & 1) > 0)
		{ bToggleCapsLock = TRUE; sk.SendKeys(_T("({CAPSLOCK})"), true); }

	MSG msgClearWait;
	while(::PeekMessage(&msgClearWait, 0, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE))
	{
		::TranslateMessage(&msgClearWait);
		::DispatchMessage(&msgClearWait);
	}

	sk.SendKeys(str, true);

	if(bToggleCapsLock == TRUE) sk.SendKeys(_T("({CAPSLOCK})"), true);

	sk.Release();

	if(bNoBlockInput == FALSE) ::BlockInput(FALSE);

	if((bLoseFocus == TRUE) && (m_bAlwaysOnTop == TRUE))
		SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);

	ASSERT(m_bLocked == FALSE);
	if((pEntry != NULL) && (m_bLocked == FALSE)) // Update last-access time
	{
		PW_TIME tNow;
		_GetCurrentPwTime(&tNow);
		pEntry->tLastAccess = tNow;

		if(m_bSaveOnLATMod == TRUE) m_bModified = TRUE;

		bRefreshView = TRUE;
	}

	const DWORD uEntryInx = _ListSelToEntryIndex();
	if((CPwUtil::IsTANEntry(pEntry) == TRUE) && (m_mgr.GetEntry(uEntryInx) == pEntry))
		_PostUseTANEntry(GetSelectedEntry(), uEntryInx);

	EraseCString(&str);
	if(bRefreshView == TRUE) RefreshPasswordList();
}

void CPwSafeDlg::OnPwlistAutoType()
{
	NotifyUserActivity();

	if(m_bDisableAutoType == TRUE) return;

	DWORD dwEntry = _ListSelToEntryIndex();
	PW_ENTRY *p;

	ASSERT(dwEntry != DWORD_MAX); if(dwEntry == DWORD_MAX) return;

	_TouchEntry(GetSelectedEntry(), FALSE);

	p = m_mgr.GetEntry(dwEntry);
	ASSERT(p != NULL); if(p == NULL) return;

	_AutoType(p, TRUE, 0, NULL);
}

void CPwSafeDlg::OnUpdatePwlistAutoType(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((((m_dwLastFirstSelectedItem != DWORD_MAX) && (m_dwLastNumSelectedItems == 1)) ? TRUE : FALSE)
		&& (!m_bDisableAutoType));
}

void CPwSafeDlg::OnExtrasPluginMgr()
{
	NotifyUserActivity();

	m_bDisplayDialog = TRUE;

	CPluginsDlg dlg;
	dlg.m_pPiMgr = &CPluginManager::Instance();
	dlg.m_pImgList = &m_ilIcons;

	dlg.DoModal();
	NotifyUserActivity();
	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnPluginMessage(UINT nID)
{
	NotifyUserActivity();
	_CallPlugins(KPM_DIRECT_EXEC, (LPARAM)nID, 0);
}

void CPwSafeDlg::OnUpdatePluginMessage(CCmdUI* pCmdUI)
{
	for(size_t i = 0; i < CPluginManager::Instance().m_plugins.size(); ++i)
	{
		KP_PLUGIN_INSTANCE* p = &CPluginManager::Instance().m_plugins[i];
		if((p->hinstDLL == NULL) || (p->pInterface == NULL)) { ASSERT(FALSE); continue; }

		const DWORD dwNumCommands = p->pInterface->GetMenuItemCount();
		const KP_MENU_ITEM* pMenuItems = p->pInterface->GetMenuItems();

		if((dwNumCommands == 0) || (pMenuItems == NULL)) continue;

		for(DWORD j = 0; j < dwNumCommands; ++j)
		{
			const KP_MENU_ITEM* pMenuItem = &pMenuItems[j];

			if(pMenuItem->dwCommandID == static_cast<DWORD>(pCmdUI->m_nID))
			{
				pCmdUI->Enable(((pMenuItem->dwFlags & KPMIF_DISABLED) != 0) ? FALSE : TRUE);

				if((pMenuItem->dwFlags & KPMIF_CHECKBOX) != 0)
					pCmdUI->SetCheck((pMenuItem->dwState & KPMIS_CHECKED) ? TRUE : FALSE);

				return;
			}
		}
	}
}

void CPwSafeDlg::BuildPluginMenu()
{
	std::vector<BCMenu *> ptrs;
	BCMenu *psub = NewGUI_GetBCMenu(m_menu.GetSubMenu(const_cast<LPTSTR>(TRL("&Tools"))));
	CString strItem;
	BOOL bMakeSeparator = TRUE;

	ASSERT(psub != NULL); if(psub == NULL) return;

	ptrs.push_back(psub);

	for(size_t i = 0; i < CPluginManager::Instance().m_plugins.size(); ++i)
	{
		KP_PLUGIN_INSTANCE* pPlugin = &CPluginManager::Instance().m_plugins[i];
		if((pPlugin->hinstDLL == NULL) || (pPlugin->pInterface == NULL)) continue;

		const DWORD dwNumCommands = pPlugin->pInterface->GetMenuItemCount();
		KP_MENU_ITEM* pMenuItems = pPlugin->pInterface->GetMenuItems();

		if((dwNumCommands == 0) || (pMenuItems == NULL)) continue;

		for(DWORD j = 0; j < dwNumCommands; ++j)
		{
			psub = ptrs[ptrs.size() - 1];
			KP_MENU_ITEM* p = &pMenuItems[j];

			UINT uFlags = (MF_STRING | MF_OWNERDRAW);

			if(_tcslen(p->lpCommandString) == 0)
			{
				uFlags |= MF_SEPARATOR;
				uFlags &= ~MF_STRING;
			}
			else if((p->dwFlags & KPMIF_CHECKBOX) > 0)
			{
				if((p->dwState & KPMIS_CHECKED) > 0)
					uFlags |= MF_CHECKED;
				else
					uFlags |= MF_UNCHECKED;
			}

			if((p->dwFlags & KPMIF_DISABLED) > 0) uFlags |= MF_GRAYED;
			else uFlags |= MF_ENABLED;

			if((p->dwFlags & KPMIF_POPUP_START) > 0)
				uFlags |= MF_POPUP;

			// psub->AppendMenu(uFlags, p->dwCommandID, p->lpCommandString);

			if((p->dwFlags & KPMIF_POPUP_START) > 0)
			{
				if(bMakeSeparator == TRUE)
					{ psub->AppendODMenu(_T(""), MF_OWNERDRAW | MF_SEPARATOR); bMakeSeparator = FALSE; }

				psub = psub->AppendODPopupMenu(p->lpCommandString);
				if(psub != NULL) ptrs.push_back(psub);
			}
			else if((p->dwFlags & KPMIF_POPUP_END) > 0)
			{
				if(ptrs.size() >= 2) ptrs.pop_back();
			}
			else
			{
				if(bMakeSeparator == TRUE)
					{ psub->AppendODMenu(_T(""), MF_OWNERDRAW | MF_SEPARATOR); bMakeSeparator = FALSE; }

				if(p->dwIcon != 0)
					psub->AppendODMenu(p->lpCommandString, uFlags, (int)p->dwCommandID, &m_ilIcons, (int)(p->dwIcon - 1));
				else
					psub->AppendODMenu(p->lpCommandString, uFlags, (int)p->dwCommandID);
			}
		}

		bMakeSeparator = TRUE;
	}

	ptrs.clear();
}

BOOL CPwSafeDlg::RegisterGlobalHotKey(int nHotKeyID, DWORD dwHotKey, BOOL bReleasePrevious, BOOL bMessageBoxOnFail)
{
	UINT uModifiers = 0;
	if(((dwHotKey >> 16) & HOTKEYF_ALT) > 0) uModifiers |= MOD_ALT;
	if(((dwHotKey >> 16) & HOTKEYF_SHIFT) > 0) uModifiers |= MOD_SHIFT;
	if(((dwHotKey >> 16) & HOTKEYF_CONTROL) > 0) uModifiers |= MOD_CONTROL;

	if(bReleasePrevious == TRUE) ::UnregisterHotKey(this->m_hWnd, nHotKeyID);

	if(dwHotKey == 0) return TRUE; // Nothing to register

	const BOOL b = ::RegisterHotKey(this->m_hWnd, nHotKeyID, uModifiers,
		static_cast<UINT>(dwHotKey & 0x0000FFFF));

	if(b == FALSE)
	{
		CString str;

		str = TRL("Cannot register the global hot key.");

		if(bMessageBoxOnFail == TRUE)
		{
			str += _T("\r\n\r\n");
			str += TRL("Most probably another application has reserved this hot key already."); str += _T("\r\n\r\n");
			str += TRL("You can choose a different hot key combination in 'Tools' - 'Options' - 'Advanced' - 'Auto-Type'.");

			// Show a (global!) warning window
			::MessageBox(::GetDesktopWindow(), str, _T("KeePass"), MB_OK | MB_ICONWARNING);
		}
		else SetStatusTextEx(str);
	}

	return b;
}

LRESULT CPwSafeDlg::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	ASSERT((wParam == HOTKEYID_AUTOTYPE) || (wParam == HOTKEYID_RESTORE));
	if(wParam == HOTKEYID_AUTOTYPE)
	{
		NotifyUserActivity();

		if(m_bDisableAutoType == TRUE) return 0;
		if(g_uMasterKeyDlgs > 0)
		{
			if(g_hLastMasterKeyDlg != NULL) ::SetForegroundWindow(g_hLastMasterKeyDlg);
			return 0;
		}

		m_bGlobalAutoTypePending = TRUE;

		if(m_bLocked == TRUE)
		{
			HWND hWndSave = ::GetForegroundWindow(); // Save currently active window

			SetForegroundWindow();
			OnFileLock();

			if((hWndSave == NULL) || (hWndSave == this->m_hWnd)) { m_bGlobalAutoTypePending = FALSE; return 0; }
			::SetForegroundWindow(hWndSave);
			Sleep(10);

			if(m_bLocked == TRUE) { m_bGlobalAutoTypePending = FALSE; return 0; }
		}
		if(m_bFileOpen == FALSE) { m_bGlobalAutoTypePending = FALSE; return 0; }

		HWND hWnd = ::GetForegroundWindow();
		const int nLen = ::GetWindowTextLength(hWnd);
		if(nLen <= 0) { m_bGlobalAutoTypePending = FALSE; return 0; }

		std::vector<TCHAR> vWindow(static_cast<size_t>(nLen + 3), _T('\0'));

		m_bDisplayDialog = TRUE;

		::GetWindowText(hWnd, &vWindow[0], nLen + 2);

		PW_ENTRY *pe = NULL;
		CString strCurWindow = &vWindow[0], strWindowExp, strWindowLookup;
		bool bLeft, bRight;
		int nSubLen;
		DWORD dwWindowField, dwWindowFieldSeq, dwWindowFieldSeqFound = 0;
		PW_UUID_STRUCT pwUuid;

		CEntryListDlg dlg;
		dlg.m_nDisplayMode = ELDMODE_LIST_ATITEMS;
		dlg.m_pMgr = &m_mgr;
		dlg.m_pImgList = &m_ilIcons;
		dlg.m_bPasswordStars = m_bPasswordStars;
		dlg.m_bUserStars = m_bUserStars;
		ZeroMemory(dlg.m_aUuid, 16);
		dlg.m_strBannerTitle = TRL("Auto-Type Entry Selection");
		dlg.m_strBannerCaption = TRL("Multiple entries exist for the current window. Please select the entry to auto-type.");

		std_string strCurWindowStl = &vWindow[0];
		if(g_vAutoTypeSelectionDialogs.find(strCurWindowStl) != g_vAutoTypeSelectionDialogs.end())
			{ SetForegroundWindow(); m_bGlobalAutoTypePending = FALSE; return 0; }

		PW_TIME tNow;
		_GetCurrentPwTime(&tNow);

		const DWORD dwInvalidId1 = m_mgr.GetGroupId(PWS_BACKUPGROUP_SRC);
		const DWORD dwInvalidId2 = m_mgr.GetGroupId(PWS_BACKUPGROUP);
		strCurWindow = strCurWindow.MakeLower();

		for(DWORD i = 0; i < m_mgr.GetNumberOfEntries(); ++i)
		{
			pe = m_mgr.GetEntry(i); ASSERT(pe != NULL); if(pe == NULL) continue;

			if((pe->uGroupId == dwInvalidId1) || (pe->uGroupId == dwInvalidId2)) continue;
			if(_pwtimecmp(&tNow, &pe->tExpire) > 0) continue; // Ignore expired entries

			dwWindowField = 0;
			dwWindowFieldSeq = 0;

			while(true) // Search all auto-type-window strings
			{
				if(dwWindowFieldSeq > 0)
					strWindowLookup.Format(_T("%s%d%s"), _T("auto-type-window-"), dwWindowFieldSeq, _T(":"));
				else
					strWindowLookup = _T("auto-type-window:");

				strWindowExp = ExtractParameterFromString(pe->pszAdditional, strWindowLookup, dwWindowField);
				strWindowExp = strWindowExp.MakeLower();

				if(strWindowExp.GetLength() != 0) // An auto-type-window definition has been found
				{
					bLeft = (strWindowExp.Left(1) == _T("*"));
					bRight = (strWindowExp.Right(1) == _T("*"));

					if(bLeft) strWindowExp.Delete(0, 1);
					if(bRight) strWindowExp.Delete(strWindowExp.GetLength() - 1, 1);

					nSubLen = strWindowExp.GetLength();

					bool bDoAdd = false;
					if(bLeft && bRight && (nSubLen <= nLen))
					{
						if(strCurWindow.Find(strWindowExp, 0) != -1)
							bDoAdd = true;
					}
					else if(bLeft && (nSubLen <= nLen))
					{
						if(strCurWindow.Right(nSubLen) == strWindowExp)
							bDoAdd = true;
					}
					else if(bRight && (nSubLen <= nLen))
					{
						if(strCurWindow.Left(nSubLen) == strWindowExp)
							bDoAdd = true;
					}
					else if(nSubLen == nLen)
					{
						if(strCurWindow == strWindowExp)
							bDoAdd = true;
					}

					if(bDoAdd)
					{
						memcpy(pwUuid.uuid, pe->uuid, 16);
						dlg.m_vEntryList.push_back(pwUuid); 
						dwWindowFieldSeqFound = dwWindowFieldSeq;
						break;
					}

					++dwWindowField;
				}
				else if(dwWindowField != 0) // Previous auto-type-window field was found but did not match, so increment sequence and search again
				{
					dwWindowField = 0;
					++dwWindowFieldSeq;
				}
				else if((dwWindowField == 0) && (dwWindowFieldSeq == 0)) // Entry doesn't have any auto-type-window definition
				{
					strWindowExp = pe->pszTitle;
					nSubLen = strWindowExp.GetLength();

					if((nSubLen != 0) && (nSubLen <= nLen))
					{
						strWindowExp = strWindowExp.MakeLower();
						if(strCurWindow.Find(strWindowExp, 0) != -1)
						{
							memcpy(pwUuid.uuid, pe->uuid, 16);
							dlg.m_vEntryList.push_back(pwUuid);
						}
					}

					break;
				}
				else break;
			}
		}

		const DWORD dwMatchingEntriesCount = static_cast<DWORD>(dlg.m_vEntryList.size());
		if(dwMatchingEntriesCount != 0)
		{
			if(dwMatchingEntriesCount == 1)
				pe = m_mgr.GetEntryByUuid(dlg.m_vEntryList[0].uuid);
			else
			{
				g_vAutoTypeSelectionDialogs.insert(strCurWindowStl);

				SetForegroundWindow();
				dlg.DoModal();

				g_vAutoTypeSelectionDialogs.erase(strCurWindowStl);

				// Check if the target window still exists / is valid
				if(::IsWindow(hWnd) == TRUE)
				{
					VERIFY(::SetForegroundWindow(hWnd) == TRUE); // Restore window focus

					if(CPwUtil::IsZeroUUID(dlg.m_aUuid) == FALSE)
						pe = m_mgr.GetEntryByUuid(dlg.m_aUuid);
					else pe = NULL;
				}
				else pe = NULL;

				dwWindowFieldSeqFound = 0; // Set this back to zero as cannot choose the sequence of auto-type in the dialog
			}

			if(pe != NULL)
			{
				if(dwMatchingEntriesCount != 1)
				{
					dwWindowField = 0;
					dwWindowFieldSeq = 0;

					while(1) // Search all auto-type-window strings
					{
						if(dwWindowFieldSeq > 0)
							strWindowLookup.Format(_T("%s%d%s"), _T("auto-type-window-"), dwWindowFieldSeq, _T(":"));
						else
							strWindowLookup = _T("auto-type-window:");

						strWindowExp = ExtractParameterFromString(pe->pszAdditional, strWindowLookup, dwWindowField);
						strWindowExp = strWindowExp.MakeLower();

						if(strWindowExp.GetLength() != 0) // An auto-type-window definition has been found
						{
							bLeft = (strWindowExp.Left(1) == _T("*"));
							bRight = (strWindowExp.Right(1) == _T("*"));

							if(bLeft) strWindowExp.Delete(0, 1);
							if(bRight) strWindowExp.Delete(strWindowExp.GetLength() - 1, 1);

							nSubLen = strWindowExp.GetLength();

							if(bLeft && bRight && (nSubLen <= nLen))
							{
								if(strCurWindow.Find(strWindowExp, 0) != -1)
								{
									dwWindowFieldSeqFound = dwWindowFieldSeq;
									break;
								}
							}
							else if(bLeft && (nSubLen <= nLen))
							{
								if(strCurWindow.Right(nSubLen) == strWindowExp)
								{
									dwWindowFieldSeqFound = dwWindowFieldSeq;
									break;
								}
							}
							else if(bRight && (nSubLen <= nLen))
							{
								if(strCurWindow.Left(nSubLen) == strWindowExp)
								{
									dwWindowFieldSeqFound = dwWindowFieldSeq;
									break;
								}
							}
							else if(nSubLen == nLen)
							{
								if(strCurWindow == strWindowExp)
								{
									dwWindowFieldSeqFound = dwWindowFieldSeq;
									break;
								}
							}

							++dwWindowField;
						}
						else if(dwWindowField != 0) // Previous auto-type-window field was found but did not match, so increment sequence and search again
						{
							dwWindowField = 0;
							++dwWindowFieldSeq;
						}
						else break; // Entry doesn't have any auto-type-window definition
					}
				}

				_AutoType(pe, FALSE, dwWindowFieldSeqFound, &vWindow[0]);
			}
		}

		dlg.m_vEntryList.clear();

		m_bGlobalAutoTypePending = FALSE;
		m_bDisplayDialog = FALSE;
	}
	else if(wParam == HOTKEYID_RESTORE)
	{
		NotifyUserActivity();
		SetForegroundWindow();
		if((m_bMinimized == TRUE) || (m_bTrayed == TRUE))
			SetViewHideState(TRUE, FALSE);
	}

	return 0;
}

// User-requested clipboard contents
void CPwSafeDlg::OnRenderFormat(UINT nFormat)
{
	if(m_nClipboardMethod != CM_ENHSECURE)
	{
		CDialog::OnRenderFormat(nFormat);
		return;
	}

	const DWORD dwIndex = _ListSelToEntryIndex();

	if(dwIndex == DWORD_MAX) return;
	PW_ENTRY *p = m_mgr.GetEntry(dwIndex);
	ASSERT_ENTRY(p); if(p == NULL) return;

	CString str = TRL("Field has been pasted."); str += _T(" ");
	str += TRL("Clipboard cleared.");

	CString str2;

	// CDialog::OnRenderFormat(nFormat);
	switch(m_nClipboardState)
	{
		case CLIPBOARD_DELAYED_USERNAME:
			m_nClipboardState = CLIPBOARD_DELAYED_NONE;
			CopyDelayRenderedClipboardData(p->pszUserName, &m_mgr);
			m_nClipboardState = CLIPBOARD_DELAYED_NONE;
			SetStatusTextEx(str);
			PostMessage(WM_MY_UPDATECLIPBOARD);
			break;

		case CLIPBOARD_DELAYED_PASSWORD:
			m_nClipboardState = CLIPBOARD_DELAYED_NONE;
			m_mgr.UnlockEntryPassword(p);
			str2 = p->pszPassword;
			m_mgr.LockEntryPassword(p);
			CopyDelayRenderedClipboardData(str2, &m_mgr);
			EraseCString(&str2);
			m_nClipboardState = CLIPBOARD_DELAYED_NONE;
			SetStatusTextEx(str);
			PostMessage(WM_MY_UPDATECLIPBOARD);
			break;

		case CLIPBOARD_DELAYED_NONE:
			CopyDelayRenderedClipboardData(_T(""), NULL);
			str2 = TRL("Paste operation completed."); str2 += _T(" ");
			str2 += TRL("Clipboard cleared.");
			SetStatusTextEx(str2);
			if(m_hwndNextViewer != NULL)
			{
				ChangeClipboardChain(m_hwndNextViewer);
				m_hwndNextViewer = NULL;
			}
			break;

		default:
			ASSERT(FALSE);
			break;
	};
}

// Called when we setup the clipboard for delayed rendering
// but nobody tried to copy from the clipboard before the next
// Copy to the clipboard occurred.
void CPwSafeDlg::OnRenderAllFormats()
{
	if(m_nClipboardMethod != CM_ENHSECURE)
	{
		CDialog::OnRenderAllFormats();
		return;
	}

	// CDialog::OnRenderAllFormats();
	// User copied different contents to the windows clipboard
	// set clipboard contents to blank string
	CopyDelayRenderedClipboardData(_T(""), NULL);
}

void CPwSafeDlg::OnChangeCbChain(HWND hWndRemove, HWND hWndAfter)
{
	CDialog::OnChangeCbChain(hWndRemove, hWndAfter);

	if(m_nClipboardMethod != CM_ENHSECURE) return;

    // If the next window is closing, repair the chain.
    if(hWndRemove == m_hwndNextViewer)
        m_hwndNextViewer = hWndAfter;
    // Otherwise, pass the message to the next link.
    else if(m_hwndNextViewer != NULL)
		::SendMessage(m_hwndNextViewer, WM_CHANGECBCHAIN, (WPARAM)hWndRemove, (LPARAM)hWndAfter);
}

void CPwSafeDlg::OnDrawClipboard()
{
	if(m_nClipboardMethod != CM_ENHSECURE)
	{
		CDialog::OnDrawClipboard();
		return;
	}

	// CDialog::OnDrawClipboard();
	// Clipboard contents changed
}

BOOL CPwSafeDlg::OnQueryEndSession()
{
	SaveOptions();
	OnFileClose();

	if(m_bDisplayDialog == TRUE) return FALSE;
	return TRUE;
}

void CPwSafeDlg::OnEndSession(BOOL bEnding)
{
	if(bEnding == TRUE)
		OnFileExit();

	CWnd::OnEndSession(bEnding);
}

void CPwSafeDlg::OnImportGetMore()
{
	ShellExecute(NULL, NULL, PWM_URL_PLUGINS, NULL, NULL, SW_SHOW);
}

BOOL CPwSafeDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	DWORD dwLen, dwPasswordLen, dwKeyFileLen;
	CString strData, strPassword, strKeyFile, strFile;

	UNREFERENCED_PARAMETER(pWnd);

	NotifyUserActivity();
	if(pCopyDataStruct == NULL) return TRUE;

	if(m_bDisplayDialog == TRUE) return TRUE; // Do nothing if a modal window is displayed

	if(pCopyDataStruct->lpData != NULL)
	{
		dwLen = (DWORD)_tcslen((LPCTSTR)pCopyDataStruct->lpData);
		if((dwLen == 0) && (pCopyDataStruct->dwData == 0xF0FFFFF0))
		{
			if((m_bMinimized == TRUE) || (m_bTrayed == TRUE))
				SetViewHideState(TRUE, FALSE); // Restore window
			return TRUE;
		}
		else if(dwLen == 0) return TRUE;

		strData = (LPCTSTR)pCopyDataStruct->lpData;
		dwPasswordLen = (DWORD)(pCopyDataStruct->dwData >> 16);
		dwKeyFileLen = (DWORD)(pCopyDataStruct->dwData & 0xFFFF);

		if((dwPasswordLen + dwKeyFileLen) >= dwLen) return TRUE;

		if(dwPasswordLen != 0) strPassword = strData.Mid(0, dwPasswordLen);
		if(dwKeyFileLen != 0) strKeyFile = strData.Mid(dwPasswordLen, dwKeyFileLen);

		if((dwPasswordLen + dwKeyFileLen) != 0)
			strFile = strData.Right(strData.GetLength() - (int)dwPasswordLen - (int)dwKeyFileLen);
		else strFile = strData;

		if(strFile.GetLength() == 0) return TRUE;

		if(m_strFile.GetLength() != 0)
		{
			CString strThis = m_strFileAbsolute;
			CString strNew = GetShortestAbsolutePath(strFile);

			strThis = strThis.MakeLower();
			strNew = strNew.MakeLower();

			if(strThis == strNew) return TRUE;
		}

		OnFileClose();
		if(m_bFileOpen == TRUE) return TRUE;

		if((dwPasswordLen != 0) && (dwKeyFileLen != 0))
			_OpenDatabase(NULL, strFile, strPassword, strKeyFile, FALSE, NULL, FALSE);
		else if(dwPasswordLen != 0)
			_OpenDatabase(NULL, strFile, strPassword, NULL, FALSE, NULL, FALSE);
		else if(dwKeyFileLen != 0)
			_OpenDatabase(NULL, strFile, NULL, strKeyFile, FALSE, NULL, FALSE);
		else
			_OpenDatabase(NULL, strFile, NULL, NULL, FALSE, NULL, FALSE);

		if((m_bMinimized == TRUE) || (m_bTrayed == TRUE))
			SetViewHideState(TRUE, FALSE);
	}

	return TRUE;
}

void CPwSafeDlg::OnInfoDonate()
{
	NotifyUserActivity();
	ShellExecute(NULL, NULL, PWM_URL_DONATE, NULL, NULL, SW_SHOW);
}

void CPwSafeDlg::OnExtrasRepairDb()
{
	NotifyUserActivity();

	if(m_bLocked == TRUE) return;

	m_bDisplayDialog = TRUE;

	if(_CallPlugins(KPM_FILE_OPEN_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	_OpenDatabase(NULL, NULL, NULL, NULL, FALSE, NULL, TRUE);
	_UpdateToolBar();
	_SetDefaultFocus();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdateExtrasRepairDb(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((m_bFileOpen == FALSE) && (m_bLocked == FALSE));
}

CString CPwSafeDlg::_GetSecureEditTipText(const TCHAR *tszBase)
{
	CString str;

	ASSERT(tszBase != NULL); if(tszBase == NULL) return CString(_T(""));

	str = TRL_VAR(tszBase);
	str = str.TrimRight(_T(':'));
	str += _T("\r\n(");
	str += TRL("Press Shift-Home or Shift-End to clear this edit control");
	str += _T(")");
	return str;
}

void CPwSafeDlg::_UpdateTitleBar()
{
	CString strFull = PWM_PRODUCT_NAME, strShort = PWM_PRODUCT_NAME;

	if(m_bLocked == TRUE)
	{
		if(m_strLastDb.GetLength() > 0)
		{
			if(m_bShowFullPath == FALSE) strFull = CsFileOnly(&m_strLastDb);
			else strFull = m_strLastDb;

			strFull += _T(" [");
			strFull += TRL("Locked");
			strFull += _T("] - ");
		}
		else
		{
			strFull = TRL("Workspace locked");
			strFull += _T(" - ");
		}

		if(m_strWindowTitleSuffix.GetLength() > 0)
			strFull += PWM_PRODUCT_NAME_SHORT;
		else strFull += PWM_PRODUCT_NAME;

		strShort = PWM_PRODUCT_NAME_SHORT;
		strShort += _T(" - ");
		strShort += TRL("Locked");
		
		if(m_strLastDb.GetLength() > 0)
		{
			if(WU_SupportsMultiLineTooltips() == TRUE) strShort += _T("\r\n");
			else strShort += _T(" - ");

			strShort += CsFileOnly(&m_strLastDb);
		}
	}
	else if(m_bFileOpen == TRUE)
	{
		if((m_strFile.GetLength() > 0) && (m_strFileAbsolute.GetLength() > 0))
		{
			if(m_bShowFullPath == FALSE) strFull = CsFileOnly(&m_strFile);
			else strFull = m_strFileAbsolute;

			if(m_bModified == TRUE) strFull += _T("*");

			strFull += _T(" - ");
		}
		else strFull = _T("");

		if(m_strWindowTitleSuffix.GetLength() > 0)
			strFull += PWM_PRODUCT_NAME_SHORT;
		else strFull += PWM_PRODUCT_NAME;

		strShort = PWM_PRODUCT_NAME_SHORT;
		if(m_strFile.GetLength() > 0)
		{
			if(WU_SupportsMultiLineTooltips() == TRUE) strShort += _T("\r\n");
			else strShort += _T(" - ");

			strShort += CsFileOnly(&m_strFile);

			if(m_bModified == TRUE) strShort += _T("*");
		}
	}

	if(m_strWindowTitleSuffix.GetLength() > 0)
	{
		strFull += _T(" - ");
		strFull += m_strWindowTitleSuffix;
	}

	CString strCurrent;
	this->GetWindowText(strCurrent);
	if(strCurrent != strFull)
	{
		this->SetWindowText(strFull);

		// NOTIFYICONDATA only supports tooltip texts up to 63 characters
		if(strShort.GetLength() > 63)
		{
			strShort = strShort.Left(60); // Replace rest by 3 dots
			strShort += _T("...");
		}

		m_systray.SetTooltipText(strShort);
	}
}

void CPwSafeDlg::OnImportKeePass()
{
	NotifyUserActivity();

	const std::basic_string<TCHAR> strDir = WU_GetCurrentDirectory();

	CPwManager mgrImport;
	_OpenDatabase(&mgrImport, NULL, NULL, NULL, FALSE, NULL, FALSE);

	WU_SetCurrentDirectory(strDir.c_str());

	m_bDisplayDialog = TRUE;

	if(mgrImport.GetNumberOfGroups() != 0)
	{
		_Groups_SaveView(TRUE);
		_List_SaveView();

		CImportOptionsDlg dlg;
		if(dlg.DoModal() == IDCANCEL) { m_bDisplayDialog = FALSE; return; }

		const BOOL bNewIDs = ((dlg.m_nImportMethod == 0) ? TRUE : FALSE);
		const BOOL bCompareTimes = ((dlg.m_nImportMethod == 2) ? TRUE : FALSE);
		m_mgr.MergeIn(&mgrImport, bNewIDs, bCompareTimes);

		m_bModified = TRUE;
		UpdateGroupList();
		_Groups_RestoreView();
		UpdatePasswordList();
		_List_RestoreView();
		_UpdateToolBar();
	}

	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdateImportKeePass(CCmdUI* pCmdUI)
{
	BOOL b = m_bFileOpen;
	b &= ((m_mgr.GetNumberOfGroups() != 0) ? TRUE : FALSE);
	pCmdUI->Enable(b);
}

void CPwSafeDlg::ExportGroupToKeePass(DWORD dwGroupId)
{
	CPwManager pwTempMgr;
	CPwExport cExp;

	NotifyUserActivity();

	if(m_bForceAllowExport == FALSE)
	{
		if(IsUnsafeAllowed(this->m_hWnd) == FALSE) return;
	}

	cExp.SetManager(&m_mgr);
	cExp.SetNewLineSeq(m_bWindowsNewLine);
	cExp.SetFormat(PWEXP_KEEPASS);

	CString strFile;

	if(dwGroupId != DWORD_MAX) strFile = GetExportFile(PWEXP_KEEPASS, m_mgr.GetGroupById(dwGroupId)->pszGroupName, TRUE);
	else strFile = GetExportFile(PWEXP_KEEPASS, CsFileOnly(&m_strFile), TRUE);

	if(strFile.GetLength() == 0) return;

	CString strText = TRL("Please choose if KeePass should use the key of the currently opened database or ask you to define a new key to encrypt the exported file.");
	strText += _T("\r\n\r\n");
	strText += TRL("Click [Yes] to use the same key as the currently opened database.");
	strText += _T("\r\n");
	strText += TRL("Click [No] to set a new key for the exported file.");
	strText += _T("\r\n\r\n");
	strText += TRL("Reuse the current key/password?");

	const int nUse = MessageBox(strText, PWM_PRODUCT_NAME_SHORT, MB_ICONQUESTION | MB_YESNO);
	if(nUse == IDNO)
	{
		if(_ChangeMasterKey(&pwTempMgr, TRUE) != TRUE) return;
	}
	else
	{
		BYTE aMasterKey[32]; // Copy the master key
		m_mgr.GetRawMasterKey(aMasterKey);
		pwTempMgr.SetRawMasterKey(aMasterKey);
		mem_erase(aMasterKey, 32);
	}

	if(dwGroupId == DWORD_MAX)
	{
		// Clone manager properties when exporting all groups and entries
		pwTempMgr.m_dwLastSelectedGroupId = m_mgr.m_dwLastSelectedGroupId;
		pwTempMgr.m_dwLastTopVisibleGroupId = m_mgr.m_dwLastTopVisibleGroupId;
		memcpy(pwTempMgr.m_aLastSelectedEntryUuid, m_mgr.m_aLastSelectedEntryUuid, 16);
		memcpy(pwTempMgr.m_aLastTopVisibleEntryUuid, m_mgr.m_aLastTopVisibleEntryUuid, 16);
	}

	cExp.ExportGroup(strFile, dwGroupId, NULL, &pwTempMgr);

	pwTempMgr.SetRawMasterKey(NULL); // Clear copy of key
}

void CPwSafeDlg::OnSafeExportGroupKeePass()
{
	DWORD dwSelectedGroup = GetSelectedGroupId();

	ASSERT(dwSelectedGroup != DWORD_MAX);
	if(dwSelectedGroup == DWORD_MAX) return;

	ExportGroupToKeePass(dwSelectedGroup);
}

void CPwSafeDlg::OnUpdateSafeExportGroupKeePass(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((m_hLastSelectedGroup != NULL) ? TRUE : FALSE);
}

void CPwSafeDlg::OnExportKeePass()
{
	ExportGroupToKeePass(DWORD_MAX);
}

void CPwSafeDlg::OnUpdateExportKeePass(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

// Many thanks to Kenneth Ballard for the contribution of this code
// to recreate the tray icon (e.g. after Windows Explorer crashed)
LRESULT CPwSafeDlg::OnTaskbarCreated(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	// When the taskbar gets recreated, possibly because of a crash
	// in Explorer, re-add the icon.
	m_systray.AddIcon();
	_UpdateTrayIcon();
	_UpdateTitleBar();
	return 0;
}

LRESULT CPwSafeDlg::OnTaskbarButtonCreated(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	_UpdateTrayIcon(); // Set overlay icon
	return 0;
}

void CPwSafeDlg::_CalcColumnSizes()
{
	RECT rect;
	int nColumnWidth, nColumns = 0, i;

	for(i = 0; i < 11; ++i) // Count active columns and reset all widths
	{
		if(m_bShowColumn[i] == TRUE) nColumns++;
		m_nColumnWidths[i] = 0;
	}
	if(nColumns == 0) return;

	m_cList.GetClientRect(&rect); // Calculate and set new widths
	nColumnWidth = ((rect.right - 9) / nColumns) - (GetSystemMetrics(SM_CXVSCROLL) / nColumns);
	for(i = 0; i < 11; ++i)
		if(m_bShowColumn[i] == TRUE)
			m_nColumnWidths[i] = nColumnWidth;
}

void CPwSafeDlg::_SetColumnWidths()
{
	if(m_dwPwListMode == LVS_REPORT)
	{
		for(int i = 0; i < 11; ++i)
		{
			if(m_cList.GetColumnWidth(i) != m_nColumnWidths[i])
				m_cList.SetColumnWidth(i, m_nColumnWidths[i]);
		}
	}
	else if(m_dwPwListMode == LVS_SMALLICON) AdjustColumnWidths();

	// Do nothing if m_dwPwListMode == LVSX_CHANGING
}

void CPwSafeDlg::_SetListParameters()
{
	LPARAM dw = (LVS_EX_SI_REPORT | LVS_EX_FULLROWSELECT | LVS_EX_ONECLICKACTIVATE |
		LVS_EX_UNDERLINEHOT | LVS_EX_HEADERDRAGDROP | LVS_EX_INFOTIP);

	// Windows XP and higher: enable double-buffering
	// Windows Vista and higher: enable alpha-blended marquee selection
	dw |= LVS_EX_DOUBLEBUFFER;

	if(m_bEntryGrid == TRUE) dw |= LVS_EX_GRIDLINES;

	if(m_dwOldListParameters != dw)
		m_cList.PostMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dw);

	m_dwOldListParameters = dw;
}

void CPwSafeDlg::OnViewSimpleTANView() 
{
	m_bSimpleTANView = ((m_bSimpleTANView == FALSE) ? TRUE : FALSE);
	m_menu.CheckMenuItem(ID_VIEW_SIMPLETANVIEW, MF_BYCOMMAND |
		((m_bSimpleTANView == TRUE) ? MF_CHECKED : MF_UNCHECKED));

	RefreshPasswordList();
}

void CPwSafeDlg::OnViewShowTANIndices() 
{
	m_bShowTANIndices = ((m_bShowTANIndices == FALSE) ? TRUE : FALSE);
	m_menu.CheckMenuItem(ID_VIEW_SHOWTANINDICES, MF_BYCOMMAND |
		((m_bShowTANIndices == TRUE) ? MF_CHECKED : MF_UNCHECKED));

	RefreshPasswordList();
}

void CPwSafeDlg::OnUpdateFileExit(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((m_bDisplayDialog == FALSE) ? TRUE : FALSE);
}

void CPwSafeDlg::OnUpdateExtrasGenPw(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((m_bDisplayDialog == FALSE) ? TRUE : FALSE);
}

void CPwSafeDlg::OnUpdateInfoAbout(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((m_bDisplayDialog == FALSE) ? TRUE : FALSE);
}

LRESULT CPwSafeDlg::OnWTSSessionChange(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	if(m_bDisplayDialog == TRUE) return 0;

	if(m_bLockOnWinLock == FALSE)
	{
		if(wParam == WTS_SESSION_LOGOFF)
		{
			SaveOptions();

			if((m_bFileOpen == TRUE) && (m_bLocked == FALSE) && (m_bModified == TRUE))
				OnFileSave();
		}
	}
	else // m_bLockOnWinLock == TRUE
	{
		if((wParam == WTS_SESSION_LOGOFF) || (wParam == WTS_SESSION_LOCK))
		{
			SaveOptions();

			if((m_bFileOpen == TRUE) && (m_bLocked == FALSE))
				OnFileLock();
		}
	}

	return 0;
}

void CPwSafeDlg::OnInfoChkForUpd()
{
	CUpdateCheckEx::Check(FALSE, this->m_hWnd, &m_ilIcons, FALSE);
}

LRESULT CPwSafeDlg::OnProcessMailslot(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	m_remoteControl.ProcessAllRequests();
	if(m_remoteControl.RequiresGUIUpdate() == TRUE)
	{
		m_bModified = TRUE;

		_Groups_SaveView(TRUE);
		UpdateGroupList();
		_Groups_RestoreView();

		_List_SaveView();
		UpdatePasswordList();
		_List_RestoreView();

		_UpdateToolBar();
	}

	return 0;
}

void CPwSafeDlg::_UpdateTrayIcon()
{
	USES_CONVERSION;

	HICON hAssign = NULL, hDestroy = NULL;
	NewGUI_UpdateColorizedIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), NULL, m_mgr.GetColor(),
		0, &m_hIcoStoreMain, &m_clrIcoStoreMain, &hAssign, &hDestroy);
	SetIcon(hAssign, FALSE);
	SetIcon(hAssign, TRUE);
	if(hDestroy != NULL) { VERIFY(DestroyIcon(hDestroy)); }

	if(m_bLocked == FALSE)
	{
		NewGUI_UpdateColorizedIcon(m_hTrayIconNormal, NULL, m_mgr.GetColor(), 16,
			&m_hIcoStoreTrayNormal, &m_clrIcoStoreTrayNormal, &hAssign, &hDestroy);

		m_systray.SetIcon(hAssign);
		if(hDestroy != NULL) { VERIFY(DestroyIcon(hDestroy)); }

		CTaskbarListEx::SetOverlayIcon(this->m_hWnd, NULL, L"");
	}
	else
	{
		// NewGUI_UpdateColorizedIcon(m_hTrayIconNormal, m_hLockOverlayIcon, m_mgr.GetColor(),
		//	16, &m_hIcoStoreTrayLocked, &m_clrIcoStoreTrayLocked, &hAssign, &hDestroy);

		// m_systray.SetIcon(hAssign);
		// if(hDestroy != NULL) { VERIFY(DestroyIcon(hDestroy)); }

		m_systray.SetIcon(m_hTrayIconLocked);

		CTaskbarListEx::SetOverlayIcon(this->m_hWnd, m_hLockOverlayIcon,
			T2CW(TRL("Locked")));
	}
}

UINT CPwSafeDlg::GetKeePassControlMessageID()
{
	return WM_REG_KEEPASSCONTROL;
}

LRESULT CPwSafeDlg::OnKeePassControlMessage(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	if(wParam == KPCM_EXIT) OnFileExit();
	else if(wParam == KPCM_AUTOTYPE) OnHotKey(HOTKEYID_AUTOTYPE, 0);
	else if(wParam == KPCM_LOCK) _ChangeLockState(TRUE);
	else if(wParam == KPCM_UNLOCK)
	{
		if(m_bLocked != FALSE) SetForegroundWindow();
		_ChangeLockState(FALSE);
	}

	return 0;
}

void CPwSafeDlg::_ChangeLockState(BOOL bLocked)
{
	if((bLocked != FALSE) && (m_bLocked == FALSE)) // Should lock
		OnFileLock();
	else if((bLocked == FALSE) && (m_bLocked != FALSE)) // Should unlock
		OnFileLock();
}

void CPwSafeDlg::ToggleViewHideState(BOOL bPreferTray)
{
	SetViewHideState(((m_bMinimized == TRUE) || (m_bTrayed == TRUE)) ? TRUE :
		FALSE, bPreferTray);
}

void CPwSafeDlg::SetViewHideState(BOOL bReqVisible, BOOL bPreferTray)
{
	NotifyUserActivity();

	if(bReqVisible == TRUE) // Show the window
	{
		_MiniModeShowWindow();

		if(m_bMinimized == TRUE)
		{
			if(m_bWasMaximized == TRUE) ShowWindow(SW_MAXIMIZE);
			else ShowWindow(SW_RESTORE);
			_UpdateTrayIcon();
		}
		else if(m_bTrayed == TRUE) SetTrayState(FALSE);

		if(m_bAlwaysOnTop == TRUE)
			SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	}
	else // Hide the window
	{
		_SaveWindowPositionAndSize(NULL);

		if((m_bMinimizeToTray == TRUE) || (bPreferTray == TRUE))
			SetTrayState(TRUE);
		else ShowWindow(SW_MINIMIZE);
	}
}

void CPwSafeDlg::_MiniModeShowWindow()
{
	if(m_bMiniMode == FALSE) return;

	if((m_bFileOpen == FALSE) && (m_strLastDb.GetLength() == 0))
	{
		OnFileNew();

		if(m_bFileOpen == FALSE) return;
		else if(m_strFile.GetLength() == 0)
		{
			CString strLoadedIniLastDb;

			if((m_bOpenLastDb == TRUE) && (m_bRememberLast == TRUE))
			{
				CPrivateConfigEx cConfig(FALSE);
				TCHAR szTemp[SI_REGSIZE];

				cConfig.Get(PWMKEY_LASTDB, szTemp);
				if(szTemp[0] != 0)
				{
					std_string strTemp = WU_ExpandEnvironmentVars(szTemp);
					strLoadedIniLastDb = strTemp.c_str();

					if(WU_IsAbsolutePath(strTemp.c_str()) == FALSE)
						strTemp = Executable::instance().getPathOnly() + strTemp;

					if(_FileAccessible(strTemp.c_str()) == TRUE)
						_OpenDatabase(NULL, strTemp.c_str(), NULL, NULL, m_bStartMinimized, NULL, FALSE);
				}

				_GetNewDbFileInUserDir(strLoadedIniLastDb, cConfig);
			}
			else { ASSERT(FALSE); }
		}
		else { ASSERT(FALSE); }
	}
}

void CPwSafeDlg::_GetNewDbFileInUserDir(const CString& strLoadedIniLastDb,
	const CPrivateConfigEx& cConfig)
{
	if((_FileAccessible(strLoadedIniLastDb) == FALSE) &&
		(_FileWritable(strLoadedIniLastDb) == TRUE))
	{
		VERIFY(DeleteFile(strLoadedIniLastDb));
		m_strFile = strLoadedIniLastDb;
		m_strFileAbsolute = strLoadedIniLastDb;
	}
	else
	{
		m_strFile = cConfig.GetUserPath().c_str();
		std_string strNewPathSpec = cConfig.GetSafe(PWMKEY_AUTONEWDBBASEPATH);
		if(strNewPathSpec.size() > 0) m_strFile = strNewPathSpec.c_str();

		VERIFY(WU_CreateDirectoryTree(m_strFile) == S_OK); // Ensure the directory exists
		
		m_strFile += _T("\\");
		
		std_string strNewNameSpec = _T("Database");
		std_string strNewNameSpecIni = cConfig.GetSafe(PWMKEY_AUTONEWDBBASENAME);
		if(strNewNameSpecIni.size() > 0) strNewNameSpec = strNewNameSpecIni;

		m_strFile += strNewNameSpec.c_str();

		bool bSuccess = false;
		for(DWORD dwTest = 0; dwTest < 1000; ++dwTest)
		{
			CString strCan;

			if(dwTest > 0)
				strCan.Format(_T("%s%u%s"), (LPCTSTR)m_strFile, dwTest, _T(".kdb"));
			else
				strCan.Format(_T("%s%s"), (LPCTSTR)m_strFile, _T(".kdb"));

			if((_FileAccessible(strCan) == FALSE) &&
				(_FileWritable(strCan) == TRUE))
			{
				VERIFY(DeleteFile(strCan));
				m_strFile = strCan;
				m_strFileAbsolute = strCan;
				bSuccess = true;
				break;
			}
		}

		if(bSuccess == false)
		{
			m_strFile.Empty();

			CString strMsg = TRL("File access error: failed to open file in write mode");
			strMsg += _T(": ");
			strMsg += TRL("KeePass Password Database");
			strMsg += _T(".");

			MessageBox(strMsg, PWM_PRODUCT_NAME_SHORT, MB_OK | MB_ICONWARNING);
			exit(1);
			return;
		}
	}
}

void CPwSafeDlg::SetTrayState(BOOL bMinimizeToTray)
{
	if(bMinimizeToTray == TRUE)
	{
		_SaveWindowPositionAndSize(NULL);

		m_bTrayed = TRUE;

		SetMenu(NULL);

		m_systray.MinimiseToTray(this);
		CMsgRelayWnd::EnableRelaying(TRUE);

		if(m_bLockOnMinimize == TRUE)
			_ChangeLockState(TRUE);
	}
	else // Restore window from tray
	{
		CMsgRelayWnd::EnableRelaying(FALSE);
		m_systray.MaximiseFromTray(this, FALSE, m_bWasMaximized);

		m_bMenu = SetMenu(&m_menu);

		m_bTrayed = FALSE;

		_UpdateTrayIcon();
		_ChangeLockState(FALSE);
	}
}

void CPwSafeDlg::OnUpdateViewHide(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((m_bDisplayDialog == FALSE) ? TRUE : FALSE);
}

void CPwSafeDlg::SetToMiniModeIfEnabled(BCMenu *pMenu, BOOL bRemoveAccels,
	BOOL bProcessToolBar)
{
	ASSERT(pMenu != NULL); if(pMenu == NULL) return;

	if(m_bMiniMode == FALSE) return; // Mini mode is disabled

	const DWORD dwIDCount = 13;
	const UINT vIDs[dwIDCount] = {
		ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVEAS, ID_FILE_CLOSE,
		ID_FILE_SHOWDBINFO,
		ID_FILE_CHANGELANGUAGE,
		ID_PWLIST_SAVEATTACH, ID_PWLIST_AUTOTYPE,
		ID_EXTRAS_TANWIZARD, ID_EXTRAS_REPAIRDB,
		ID_SAFE_OPTIONS, ID_EXTRAS_PLUGINMGR,
		ID_INFO_CHKFORUPD
	};

	for(DWORD i = 0; i < dwIDCount; ++i)
		NewGUI_RemoveMenuCommand(pMenu, vIDs[i]);

	NewGUI_RemoveInvalidSeparators(pMenu, TRUE);

	if(bRemoveAccels == TRUE)
	{
		const int nAccelCount = CopyAcceleratorTable(m_hAccel, NULL, 0);
		std::vector<ACCEL> vCurAccels(static_cast<size_t>(nAccelCount));
		VERIFY(CopyAcceleratorTable(m_hAccel, &vCurAccels[0], nAccelCount) == nAccelCount);

		std::vector<ACCEL> vNewAccels;
		for(int iAccel = 0; iAccel < nAccelCount; ++iAccel)
		{
			ACCEL *pCur = &vCurAccels[iAccel];
			bool bValid = true;
			for(DWORD dwInvalid = 0; dwInvalid < dwIDCount; ++dwInvalid)
			{
				if(pCur->cmd == vIDs[dwInvalid]) bValid = false;
			}

			if(bValid) vNewAccels.push_back(*pCur);
		}

		VERIFY(DestroyAcceleratorTable(m_hAccel));
		m_hAccel = CreateAcceleratorTable(&vNewAccels[0],
			static_cast<int>(vNewAccels.size()));
	}

	if(bProcessToolBar == TRUE)
	{
		long lToRight = 0;
		NewGUI_DisableHideWnd(&m_btnTbNew);
		NewGUI_DisableHideWnd(&m_btnTbOpen);
		lToRight -= NewGUI_GetWndBasePosDiff(&m_btnTbNew, &m_btnTbSave).cx;
		NewGUI_MoveWnd(&m_btnTbSave, lToRight, 0, this);
		NewGUI_MoveWnd(GetDlgItem(IDC_STATIC_TBSEP0), lToRight, 0, this);
		NewGUI_MoveWnd(&m_btnTbAddEntry, lToRight, 0, this);
		NewGUI_MoveWnd(&m_btnTbEditEntry, lToRight, 0, this);
		NewGUI_MoveWnd(&m_btnTbDeleteEntry, lToRight, 0, this);
		NewGUI_MoveWnd(GetDlgItem(IDC_STATIC_TBSEP1), lToRight, 0, this);
		NewGUI_MoveWnd(&m_btnTbCopyUser, lToRight, 0, this);
		NewGUI_MoveWnd(&m_btnTbCopyPw, lToRight, 0, this);
		NewGUI_MoveWnd(GetDlgItem(IDC_STATIC_TBSEP2), lToRight, 0, this);
		NewGUI_MoveWnd(&m_btnTbFind, lToRight, 0, this);
		NewGUI_MoveWnd(GetDlgItem(IDC_STATIC_TBSEP3), lToRight, 0, this);
		NewGUI_MoveWnd(&m_btnTbLock, lToRight, 0, this);
		NewGUI_MoveWnd(GetDlgItem(IDC_STATIC_TBSEP4), lToRight, 0, this);
		NewGUI_MoveWnd(&m_btnTbAbout, lToRight, 0, this);
		NewGUI_MoveWnd(GetDlgItem(IDC_STATIC_TBSEP5), lToRight, 0, this);
		NewGUI_MoveWnd(&m_cQuickFind, lToRight, 0, this);
	}
}

void CPwSafeDlg::RegisterRestoreHotKey(BOOL bRegister)
{
	if((bRegister == TRUE) && (m_bRestoreHotKeyRegistered == FALSE))
		m_bRestoreHotKeyRegistered = ::RegisterHotKey(this->m_hWnd,
			HOTKEYID_RESTORE, MOD_ALT | MOD_CONTROL, 'K');
	else if((bRegister == FALSE) && (m_bRestoreHotKeyRegistered != FALSE))
		::UnregisterHotKey(this->m_hWnd, HOTKEYID_RESTORE);
}

void CPwSafeDlg::OnQuickFindSelChange()
{
	if(m_bBlockQuickFindSelChange == TRUE) return;
	m_bBlockQuickFindSelChange = TRUE;

	const int iSel = m_cQuickFind.GetCurSel();
	if(iSel == CB_ERR)
	{
		m_bBlockQuickFindSelChange = FALSE;
		return;
	}

	m_cQuickFind.GetLBText(iSel, m_strQuickFind);

	_DoQuickFind(m_strQuickFind);

	if(m_strQuickFind != HCMBX_SEPARATOR)
		m_cQuickFind.SelectString(-1, m_strQuickFind);
	else
		m_cQuickFind.SetCurSel(-1);

	m_bBlockQuickFindSelChange = FALSE;
}

void CPwSafeDlg::DropToBackgroundIfOptionEnabled(bool bForceDrop)
{
	if((m_bDropToBackOnCopy == TRUE) || bForceDrop)
		SafeActivateNextWindow(this->m_hWnd);
}

void CPwSafeDlg::OnInfoHelpSelectHelpSource()
{
	CHelpSourceDlg dlg;
	dlg.DoModal();
}

void CPwSafeDlg::_PostUseTANEntry(DWORD dwListIndex, DWORD dwEntryIndex)
{
	if(dwEntryIndex == DWORD_MAX) { ASSERT(FALSE); return; }

	PW_ENTRY* p = m_mgr.GetEntry(dwEntryIndex);
	if(p == NULL) { ASSERT(FALSE); return; }

	if(CPwUtil::IsTANEntry(p) == TRUE) // If it is a TAN entry, expire it
	{
		_GetCurrentPwTime(&p->tExpire);
		m_bModified = TRUE;

		if(m_bDeleteTANsAfterUse == TRUE)
		{
			if(m_bBackupEntries != FALSE)
			{
				m_mgr.UnlockEntryPassword(p);
				m_mgr.BackupEntry(p, NULL);
				m_mgr.LockEntryPassword(p);
			}

			VERIFY(m_mgr.DeleteEntry(dwEntryIndex)); // Delete from password manager
			VERIFY(m_cList.DeleteItem(static_cast<int>(dwListIndex))); // Delete from GUI
		}
	}
	else { ASSERT(FALSE); }
}

void CPwSafeDlg::SetLastMasterKeyDlg(HWND h)
{
	g_hLastMasterKeyDlg = h;
}

void CPwSafeDlg::_PreDatabaseWrite()
{
	m_mgr.SetTransactedFileWrites(m_bUseTransactedFileWrites);
}

void CPwSafeDlg::_UpdateCachedGroupIDs()
{
	m_dwCachedBackupGroupID = m_mgr.GetGroupId(PWS_BACKUPGROUP);
	m_dwCachedBackupSrcGroupID = m_mgr.GetGroupId(PWS_BACKUPGROUP_SRC);
}

void CPwSafeDlg::_SetLVItemParam(LV_ITEM* pItem, const PW_ENTRY* pe)
{
	if((pItem == NULL) || (pe == NULL)) { ASSERT(FALSE); return; }

	pItem->lParam = 0;

	if((pe->uGroupId == m_dwCachedBackupGroupID) || (pe->uGroupId == m_dwCachedBackupSrcGroupID))
		pItem->lParam |= CLCIF_HIGHLIGHT_GREEN;
}
