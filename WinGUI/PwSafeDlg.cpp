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
#include <afxadv.h>
#include <winable.h>
#include <mmsystem.h>

#include "PwSafe.h"
#include "PwSafeDlg.h"

#include "../KeePassLibCpp/PwManager.h"
#include "../KeePassLibCpp/DataExchange/PwImport.h"
#include "../KeePassLibCpp/Util/PwUtil.h"
#include "../KeePassLibCpp/Crypto/TestImpl.h"
#include "../KeePassLibCpp/Util/MemUtil.h"
#include "../KeePassLibCpp/Util/StrUtil.h"
#include "../KeePassLibCpp/Util/AppUtil.h"
#include "Util/WinUtil.h"
#include "Util/PrivateConfig.h"
#include "Util/SendKeys.h"
#include "Util/FileLock.h"
#include "Util/CmdLine/CmdArgs.h"
#include "Util/CmdLine/Executable.h"
#include "Util/UpdateCheck.h"
#include "../KeePassLibCpp/Util/TranslateEx.h"
#include "NewGUI/XHyperLink.h"
#include "NewGUI/XPStyleButtonST.h"
#include "NewGUI/SecureEditEx.h"
#include "Plugins/MsgInterface.h"

#include "PasswordDlg.h"
#include "AddEntryDlg.h"
#include "AddGroupDlg.h"
#include "PwGeneratorDlg.h"
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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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

static int g_nIndicatorWidths[3] = { 200, 340, -1 };

BOOL CPwSafeDlg::m_bSecureEdits = TRUE;

const UINT WM_REG_TASKBARCREATED = ::RegisterWindowMessage(_T("TaskbarCreated"));

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

class CPP_CLASS_SHARE CPwSafeAppRI : public CNewRandomInterface
{
public:
	BOOL GenerateRandomSequence(unsigned long uRandomSeqSize, unsigned char *pBuffer) const;
};

BOOL CPwSafeAppRI::GenerateRandomSequence(unsigned long uRandomSeqSize, unsigned char *pBuffer) const
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
{
	//{{AFX_DATA_INIT(CPwSafeDlg)
	m_strQuickFind = _T("");
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_bExiting = FALSE;
	m_bCheckForInstance = FALSE;
	m_bRestoreHotKeyRegistered = FALSE;

	m_hLastSelectedGroup = 0;
	m_dwLastNumSelectedItems = 0;
	m_dwLastFirstSelectedItem = 0;

	m_nClipboardState = 0;
	m_hwndNextViewer = NULL;

	m_pPwListMenu = m_pGroupListMenu = m_pEntryViewMenu = NULL;
	m_pPwListTrackableMenu = m_pGroupListTrackableMenu = m_pEntryViewTrackableMenu = NULL;

	m_mgr.InitPrimaryInstance();
}

void CPwSafeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPwSafeDlg)
	DDX_Control(pDX, IDC_MENULINE, m_stcMenuLine);
	DDX_Control(pDX, IDC_EDIT_QUICKFIND, m_cQuickFind);
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
	DDX_Text(pDX, IDC_EDIT_QUICKFIND, m_strQuickFind);
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
	ON_COMMAND(ID_INFO_CHKFORUPD, &CPwSafeDlg::OnInfoChkForUpd)
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
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString str;

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	NewGUI_XPButton(&m_btOK, IDB_OK, IDB_OK);

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);

	str = TRL("Version ");
	str += PWM_VERSION_STR;
	m_banner.SetTitle(PWM_PRODUCT_NAME);
	m_banner.SetCaption(str);

	m_hWindowIcon = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(m_hWindowIcon, TRUE);
	SetIcon(m_hWindowIcon, FALSE);

	NewGUI_MakeHyperLink(&m_hlHomepage);
	m_hlHomepage.SetURL(CString(PWM_HOMEPAGE));

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

	if(wParam == IDC_HLINK_HELPFILE)
		_OpenLocalFile(PWM_README_FILE, OLF_OPEN);
	else if(wParam == IDC_HLINK_LICENSEFILE)
		_OpenLocalFile(PWM_LICENSE_FILE, OLF_OPEN);
	else if(wParam == IDC_HLINK_CREDITS)
		WU_OpenAppHelp(PWM_HELP_CREDITS);
	else if(wParam == IDC_HLINK_DONATE)
		ShellExecute(NULL, _T("open"), PWM_URL_DONATE, NULL, NULL, SW_SHOW);

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

	if(m_bCheckForInstance == TRUE) m_instanceChecker.TrackFirstInstanceRunning();

	srand((unsigned int)time(NULL));
	KPMI_SetMainDialog(this);

	m_bWindowsNewLine = TRUE;
	m_bFileOpen = FALSE;
	m_bFileReadOnly = FALSE;
	m_bLocked = FALSE;
	m_bModified = FALSE;
	m_dwOldListParameters = 0;
	m_bMinimized = FALSE;
	m_bMaximized = FALSE;
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
	m_cList.m_pbShowColumns = (BOOL *)m_bShowColumn;
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

	BCMenu *pDest;
	BCMenu *pSrc;
	UINT i;
	UINT uState, uID, uLastID = (UINT)(-1);
	CString str;

	m_popmenu.LoadMenu(IDR_GROUPLIST_MENU);
	pSrc = (BCMenu *)m_popmenu.GetSubMenu(0);
	ASSERT(pSrc != NULL); if(pSrc == NULL) pSrc = &m_popmenu;

	pDest = (BCMenu *)m_menu.GetSubMenu(1);
	ASSERT(pDest != NULL); if(pDest == NULL) pDest = &m_menu;

	// pDest->AppendMenu(MF_SEPARATOR);
	for(i = 0; i < pSrc->GetMenuItemCount(); i++)
	{
		uID = pSrc->GetMenuItemID(i);
		uState = pSrc->GetMenuState(i, MF_BYPOSITION);
		pSrc->GetMenuText(i, str, MF_BYPOSITION);
		if(str == _T("&Rearrange")) continue;
		if(str == _T("&Export To")) continue;
		if(str == _T("Find In T&his Group...")) continue;
		// if((uID == ID_GROUP_MOVETOP) || (uID == ID_GROUP_MOVEBOTTOM)) continue;
		// if((uID == ID_GROUP_MOVEUP) || (uID == ID_GROUP_MOVEDOWN)) continue;
		if(str == _T("Pr&int Group...")) continue;

		if(uLastID != uID) pDest->AppendMenu(uState, uID, (LPTSTR)(LPCTSTR)str);
		uLastID = uID;
	}
	m_popmenu.DestroyMenu();

	// pDest->GetMenuText(0, str, MF_BYPOSITION);
	// if(str.GetLength() == 0) pDest->DeleteMenu(0, MF_BYPOSITION);
	if(pDest->GetMenuItemID(0) == 0) pDest->DeleteMenu(0, MF_BYPOSITION);

	m_popmenu.LoadMenu(IDR_PWLIST_MENU);
	pDest = (BCMenu *)m_menu.GetSubMenu(1);
	ASSERT(pDest != NULL); if(pDest == NULL) pDest = &m_menu;
	pSrc = (BCMenu *)m_popmenu.GetSubMenu(0);
	ASSERT(pSrc != NULL); if(pSrc == NULL) pSrc = &m_popmenu;

	// pDest->AppendMenu(MF_SEPARATOR);
	for(i = 0; i < pSrc->GetMenuItemCount() - 2; i++)
	{
		uID = pSrc->GetMenuItemID(i);
		uState = pSrc->GetMenuState(i, MF_BYPOSITION);
		pSrc->GetMenuText(i, str, MF_BYPOSITION);
		if(str == _T("&Rearrange")) continue;
		// if((uID == ID_PWLIST_MOVETOP) || (uID == ID_PWLIST_MOVEBOTTOM)) continue;
		// if((uID == ID_PWLIST_MOVEUP) || (uID == ID_PWLIST_MOVEDOWN)) continue;

		if(uLastID != uID) pDest->AppendMenu(uState, uID, (LPTSTR)(LPCTSTR)str);
		uLastID = uID;
	}
	m_popmenu.DestroyMenu();

	// Load the translation file
	CPrivateConfig cConfig(FALSE);
	TCHAR szTemp[SI_REGSIZE];

	szTemp[0] = 0; szTemp[1] = 0;
	cConfig.Get(PWMKEY_LANG, szTemp);
	LoadTranslationTable(szTemp);
	g_psztNeverExpires = TRL("Never expires");

	cConfig.Get(PWMKEY_PWGEN_OPTIONS, szTemp);
	if(_tcslen(szTemp) > 0)
	{
		CString strOptions = szTemp;

		szTemp[0] = 0; szTemp[1] = 0;
		cConfig.Get(PWMKEY_PWGEN_CHARS, szTemp);
		CString strCharSet = szTemp;

		szTemp[0] = 0; szTemp[1] = 0;
		cConfig.Get(PWMKEY_PWGEN_NUMCHARS, szTemp);
		if(_tcslen(szTemp) > 0)
			CPwGeneratorDlg::SetOptions(strOptions, strCharSet, (UINT)(_ttoi(szTemp)));
	}
	else CPwGeneratorDlg::SetOptions(CString(_T("11100000001")), CString(_T("")), 16);

	cConfig.Get(PWMKEY_LASTDIR, szTemp);
	if(_tcslen(szTemp) != 0) SetCurrentDirectory(szTemp);

	cConfig.Get(PWMKEY_CLIPBOARDMETHOD, szTemp);
	if(_tcslen(szTemp) != 0) m_nClipboardMethod = _ttoi(szTemp);
	else m_nClipboardMethod = 0;

	cConfig.Get(PWMKEY_CLIPSECS, szTemp);
	if(_tcslen(szTemp) > 0)
	{
		m_dwClipboardSecs = (DWORD)_ttol(szTemp);
		if(m_dwClipboardSecs == 0) m_dwClipboardSecs = 10 + 1;
		if(m_dwClipboardSecs == (DWORD)(-1)) m_dwClipboardSecs = 10 + 1;
	}
	else m_dwClipboardSecs = 10 + 1;

	WORD wATHotKeyVK = (WORD)(_T('A'));
	WORD wATHotKeyMod = HOTKEYF_CONTROL | HOTKEYF_ALT;
	DWORD dwDefaultATHotKey = ((DWORD)wATHotKeyMod << 16) | (DWORD)wATHotKeyVK;

	cConfig.Get(PWMKEY_AUTOTYPEHOTKEY, szTemp);
	if(_tcslen(szTemp) > 0) m_dwATHotKey = (DWORD)_ttol(szTemp);
	else m_dwATHotKey = dwDefaultATHotKey;

	cConfig.Get(PWMKEY_DEFAULTEXPIRE, szTemp);
	if(_tcslen(szTemp) > 0) m_dwDefaultExpire = (DWORD)_ttol(szTemp);
	else m_dwDefaultExpire = 0;

	m_bWindowsNewLine = TRUE; // Assume Windows
	cConfig.Get(PWMKEY_NEWLINE, szTemp);
	if(_tcsicmp(szTemp, _T("Unix")) == 0) m_bWindowsNewLine = FALSE;

	cConfig.Get(PWMKEY_IMGBTNS, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		if(_tcscmp(szTemp, _T("False")) == 0) m_bImgButtons = FALSE;
		else m_bImgButtons = TRUE;
	}
	else m_bImgButtons = TRUE;
	NewGUI_SetImgButtons(m_bImgButtons);

	m_bDisableUnsafe = cConfig.GetBool(PWMKEY_DISABLEUNSAFE, FALSE);
	m_bDisableUnsafeAtStart = m_bDisableUnsafe;

	cConfig.Get(PWMKEY_AUTOSORT, szTemp);
	if(_tcslen(szTemp) != 0) m_nAutoSort = _ttoi(szTemp);
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

	m_bPasswordStars = cConfig.GetBool(PWMKEY_HIDESTARS, TRUE);
	m_bUserStars = cConfig.GetBool(PWMKEY_HIDEUSERS, FALSE);

	m_bLockOnMinimize = cConfig.GetBool(PWMKEY_LOCKMIN, TRUE);
	m_bMinimizeToTray = cConfig.GetBool(PWMKEY_MINTRAY, FALSE);
	m_bStartMinimized = cConfig.GetBool(PWMKEY_STARTMINIMIZED, FALSE);
	m_bCloseMinimizes = cConfig.GetBool(PWMKEY_CLOSEMIN, FALSE);

	m_bBackupEntries = cConfig.GetBool(PWMKEY_BACKUPENTRIES, TRUE);
	m_bSecureEdits = cConfig.GetBool(PWMKEY_SECUREEDITS, TRUE);
	m_bSingleClickTrayIcon = cConfig.GetBool(PWMKEY_SINGLECLICKTRAY, FALSE);
	m_bAutoPwGen = cConfig.GetBool(PWMKEY_AUTOPWGEN, TRUE);
	m_bQuickFindIncBackup = cConfig.GetBool(PWMKEY_QUICKFINDINCBK, TRUE);
	m_bDeleteBackupsOnSave = cConfig.GetBool(PWMKEY_DELETEBKONSAVE, FALSE);
	m_bDisableAutoType = cConfig.GetBool(PWMKEY_DISABLEAUTOTYPE, FALSE);
	m_bCopyURLs = cConfig.GetBool(PWMKEY_COPYURLS, FALSE);
	m_bExitInsteadOfLockAT = cConfig.GetBool(PWMKEY_EXITINSTEADLOCK, FALSE);
	m_bAllowSaveIfModifiedOnly = cConfig.GetBool(PWMKEY_ALLOWSAVEIFMODIFIEDONLY, FALSE);

	m_bShowToolBar = cConfig.GetBool(PWMKEY_SHOWTOOLBAR, TRUE);
	m_bShowFullPath = cConfig.GetBool(PWMKEY_SHOWFULLPATH, FALSE);

	m_bSimpleTANView = cConfig.GetBool(PWMKEY_SIMPLETANVIEW, TRUE);
	m_bShowTANIndices = cConfig.GetBool(PWMKEY_SHOWTANINDICES, TRUE);

	m_bCheckForUpdate = cConfig.GetBool(PWMKEY_CHECKFORUPDATE, FALSE);

	cConfig.Get(PWMKEY_ROWCOLOR, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		COLORREF cref = (COLORREF)_ttol(szTemp);
		m_cList.SetRowColorEx(cref);
	}

	cConfig.Get(PWMKEY_AUTOTYPEMETHOD, szTemp);
	if(_tcslen(szTemp) != 0) m_nAutoTypeMethod = _ttoi(szTemp);
	else m_nAutoTypeMethod = ATM_DROPBACK;

	cConfig.Get(PWMKEY_URLOVERRIDE, szTemp);
	if(_tcslen(szTemp) != 0) m_strURLOverride = szTemp;
	else m_strURLOverride = _T("");

	m_nLockTimeDef = -1;
	cConfig.Get(PWMKEY_LOCKTIMER, szTemp);
	if(_tcslen(szTemp) != 0) m_nLockTimeDef = _ttol(szTemp);
	m_nLockCountdown = m_nLockTimeDef;

	m_bAutoShowExpired = cConfig.GetBool(PWMKEY_AUTOSHOWEXPIRED, FALSE);
	m_bAutoShowExpiredSoon = cConfig.GetBool(PWMKEY_AUTOSHOWEXPIREDS, FALSE);

	m_bEntryView = cConfig.GetBool(PWMKEY_ENTRYVIEW, TRUE);
	m_bColAutoSize = cConfig.GetBool(PWMKEY_COLAUTOSIZE, TRUE);

	// Translate the menu
	BCMenu *pSubMenu = &m_menu;
	const TCHAR *pSuffix = _T("");
	CString strItem, strNew;
	int nItem = 0, nSub = 0;
	UINT nID;
	while(1)
	{
		if(pSubMenu->GetMenuString((UINT)nItem, strItem, MF_BYPOSITION) == FALSE) break;
		pSuffix = _GetCmdAccelExt(strItem);
		strNew = TRL_VAR(strItem);
		if(_tcslen(pSuffix) != 0) { strNew += _T("\t"); strNew += pSuffix; }
		nID = pSubMenu->GetMenuItemID(nItem);
		if(pSubMenu->ModifyMenu(nItem, MF_BYPOSITION | MF_STRING, nID, strNew) == FALSE) { ASSERT(FALSE); }
		nItem++;
	}
	pSubMenu = (BCMenu *)m_menu.GetSubMenu(nSub);
	while(1)
	{
		_TranslateMenu(pSubMenu, TRUE, &m_bCopyURLs);

		pSubMenu = (BCMenu *)m_menu.GetSubMenu(nSub);
		nSub++;
		if(pSubMenu == NULL) break;
	}

	pSubMenu = (BCMenu *)m_menu.GetSubMenu(TRL("&Tools")); ASSERT(pSubMenu != NULL);
	if(pSubMenu != NULL) m_uOriginalExtrasMenuItemCount = pSubMenu->GetMenuItemCount();
	else m_uOriginalExtrasMenuItemCount = MENU_EXTRAS_ITEMCOUNT;

	m_menu.LoadToolbar(IDR_INFOICONS, IDB_INFOICONS_EX);

	m_bMenu = SetMenu(&m_menu); ASSERT(m_bMenu == TRUE);

	RebuildContextMenus();

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
	if(_tcslen(szTemp) == 0) _tcscpy_s(szTemp, _countof(szTemp), _T("MS Shell Dlg;8,0000"));
	_ParseSpecAndSetFont(szTemp);

	// int nColumnWidth;
	// RECT rect;
	// m_cGroups.GetClientRect(&rect);
	// nColumnWidth = rect.right - rect.left - GetSystemMetrics(SM_CXVSCROLL);
	// nColumnWidth -= 8;
	// m_cGroups.InsertColumn(0, TRL("Password Groups"), LVCFMT_LEFT, nColumnWidth, 0);

	// m_cGroups.PostMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_SI_MENU | LVS_EX_INFOTIP);
	m_cGroups.ModifyStyle(0, TVS_TRACKSELECT, 0);

	unsigned long ul;
	ul = TestCryptoImpl();
	if(ul != 0)
	{
		CString str;

		str = TRL("The following self-tests failed:");
		str += _T("\r\n");

		if(ul & TI_ERR_SHAVAR32)
			{ str += TRL("- SHA 32-bit variables"); str += _T("\r\n"); }
		if(ul & TI_ERR_SHAVAR64)
			{ str += TRL("- SHA 64-bit variables"); str += _T("\r\n"); }
		if(ul & TI_ERR_SHACMP256)
			{ str += TRL("- SHA-256 test vector(s)"); str += _T("\r\n"); }
		if(ul & TI_ERR_SHACMP512)
			{ str += TRL("- SHA-512 test vector(s)"); str += _T("\r\n"); }
		if(ul & TI_ERR_RIJNDAEL_ENCRYPT)
			{ str += TRL("- Rijndael encryption"); str += _T("\r\n"); }
		if(ul & TI_ERR_RIJNDAEL_DECRYPT)
			{ str += TRL("- Rijndael decryption"); str += _T("\r\n"); }
		if(ul & TI_ERR_ARCFOUR_CRYPT)
			{ str += TRL("- Arcfour crypto routine"); str += _T("\r\n"); }
		if(ul & TI_ERR_TWOFISH)
			{ str += TRL("- Twofish algorithm"); str += _T("\r\n"); }

		str += _T("\r\n");
		str += TRL("The program will exit now.");
		MessageBox(str, TRL("Self-Test(s) Failed"), MB_OK | MB_ICONWARNING);
		OnCancel();
	}

	m_strFile.Empty(); m_strFileAbsolute.Empty();
	m_cList.EnableWindow(FALSE);
	m_cGroups.EnableWindow(FALSE);

	// "Initialize" the xorshift pseudo-random number generator
	CNewRandom nr;
	nr.Initialize();
	DWORD dwSkip, dwPos;
	nr.GetRandomBuffer((BYTE *)&dwSkip, 4);
	dwSkip &= 0x07FF;
	for(dwPos = 0; dwPos < dwSkip; dwPos++) randXorShift();

	// _UpdateTitleBar(); // Updated by _UpdateToolBar()

	ProcessResize();
	UpdateGroupList();
	UpdatePasswordList();

	m_strTempFile.Empty();
	m_bTimer = TRUE;
	m_nClipboardCountdown = -1;
	SetTimer(APPWND_TIMER_ID, 1000, NULL);
	SetTimer(APPWND_TIMER_ID_UPDATER, 500, NULL);

	m_bRememberLast = cConfig.GetBool(PWMKEY_REMEMBERLAST, TRUE);

	m_strLastDb.Empty();

	m_bOpenLastDb = cConfig.GetBool(PWMKEY_OPENLASTB, FALSE);
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

	m_bShowWindow = TRUE;
	VERIFY(m_systray.Create(this, WM_MY_SYSTRAY_NOTIFY, PWM_PRODUCT_NAME_SHORT,
		m_hTrayIconNormal, IDR_SYSTRAY_MENU, FALSE,
		NULL, NULL, NIIF_NONE, 0));
	m_systray.SetMenuDefaultItem(0, TRUE);
	m_systray.MoveToRight();

	for(INT tt = 0; tt < 11; tt++) m_aHeaderOrder[tt] = tt;
	cConfig.Get(PWMKEY_HEADERORDER, szTemp);
	if(_tcslen(szTemp) > 0)
	{
		str2ar(szTemp, m_aHeaderOrder, 11);
		NewGUI_SetHeaderOrder(m_cList.m_hWnd, m_aHeaderOrder, 11);
	}

	m_pThemeHelper = new CThemeHelperST();
	ASSERT(m_pThemeHelper != NULL);
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
	m_tip.AddTool(GetDlgItem(IDC_EDIT_QUICKFIND), TRL("Find a string in the password list"));
	m_tip.Activate(TRUE);

	int px, py, dx, dy; // Restore window position
	cConfig.Get(PWMKEY_WINDOWPX, szTemp);
	if(_tcslen(szTemp) > 0) px = _ttoi(szTemp); else px = -1;
	cConfig.Get(PWMKEY_WINDOWPY, szTemp);
	if(_tcslen(szTemp) > 0) py = _ttoi(szTemp); else py = -1;
	cConfig.Get(PWMKEY_WINDOWDX, szTemp);
	if(_tcslen(szTemp) > 0) dx = _ttoi(szTemp); else dx = -1;
	cConfig.Get(PWMKEY_WINDOWDY, szTemp);
	if(_tcslen(szTemp) > 0) dy = _ttoi(szTemp); else dy = -1;

	if((px == 3000) && (py == 3000)) { px = 0; py = 0; }
	if(px > GetSystemMetrics(SM_CXSCREEN)) px = 0;
	if(py > GetSystemMetrics(SM_CYSCREEN)) py = 0;

	m_lSplitterPosHoriz = GUI_GROUPLIST_EXT + 1;
	m_lSplitterPosVert = (rectClient.bottom - rectClient.top - PWS_DEFAULT_SPLITTER_Y) / 4;
	ASSERT(m_lSplitterPosVert > 0);

	if((px != -1) && (py != -1) && (dx != -1) && (dy != -1))
	{
		if(px < -20) px = 0;
		if(py < -20) py = 0;

		SetWindowPos(&wndNoTopMost, px, py, dx, dy, SWP_NOOWNERZORDER | SWP_NOZORDER);
	}

	for(ul = 0; ul < 11; ul++) m_nColumnWidths[ul] = 0;

	// Restore column sizes
	cConfig.Get(PWMKEY_COLWIDTH0, szTemp);
	if(_tcslen(szTemp) != 0) m_nColumnWidths[0] = _ttoi(szTemp);
	cConfig.Get(PWMKEY_COLWIDTH1, szTemp);
	if(_tcslen(szTemp) != 0) m_nColumnWidths[1] = _ttoi(szTemp);
	cConfig.Get(PWMKEY_COLWIDTH2, szTemp);
	if(_tcslen(szTemp) != 0) m_nColumnWidths[2] = _ttoi(szTemp);
	cConfig.Get(PWMKEY_COLWIDTH3, szTemp);
	if(_tcslen(szTemp) != 0) m_nColumnWidths[3] = _ttoi(szTemp);
	cConfig.Get(PWMKEY_COLWIDTH4, szTemp);
	if(_tcslen(szTemp) != 0) m_nColumnWidths[4] = _ttoi(szTemp);
	cConfig.Get(PWMKEY_COLWIDTH5, szTemp);
	if(_tcslen(szTemp) != 0) m_nColumnWidths[5] = _ttoi(szTemp);
	cConfig.Get(PWMKEY_COLWIDTH6, szTemp);
	if(_tcslen(szTemp) != 0) m_nColumnWidths[6] = _ttoi(szTemp);
	cConfig.Get(PWMKEY_COLWIDTH7, szTemp);
	if(_tcslen(szTemp) != 0) m_nColumnWidths[7] = _ttoi(szTemp);
	cConfig.Get(PWMKEY_COLWIDTH8, szTemp);
	if(_tcslen(szTemp) != 0) m_nColumnWidths[8] = _ttoi(szTemp);
	cConfig.Get(PWMKEY_COLWIDTH9, szTemp);
	if(_tcslen(szTemp) != 0) m_nColumnWidths[9] = _ttoi(szTemp);
	cConfig.Get(PWMKEY_COLWIDTH10, szTemp);
	if(_tcslen(szTemp) != 0) m_nColumnWidths[10] = _ttoi(szTemp);

	cConfig.Get(PWMKEY_SPLITTERX, szTemp);
	if(_tcslen(szTemp) != 0) m_lSplitterPosHoriz = _ttol(szTemp);
	else m_lSplitterPosHoriz = GUI_GROUPLIST_EXT + 1;

	cConfig.Get(PWMKEY_SPLITTERY, szTemp);
	if(_tcslen(szTemp) != 0) m_lSplitterPosVert = _ttol(szTemp);
	// else m_lSplitterPosVert = PWS_DEFAULT_SPLITTER_Y; // Set above already

	// if(m_bAlwaysOnTop == TRUE)
	//	SetWindowPos(&wndTopMost, 0, 0, 0, 0,
	//		SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

	VERIFY(m_cGroups.InitDropHandler());

	KP_APP_INFO kpai;
	ZeroMemory(&kpai, sizeof(KP_APP_INFO));
	kpai.dwAppVersion = PWM_VERSION_DW;
	kpai.hwndMain = this->m_hWnd;
	// kpai.pApp = (void *)AfxGetApp();
	kpai.pMainDlg = (void *)this;
	kpai.pPwMgr = (void *)&m_mgr;
	kpai.lpKPCall = KP_Call;
	kpai.lpKPQuery = KP_Query;

	CPluginManager::Instance().SetAppInfo(&kpai);
	CPluginManager::Instance().SetDirectCommandRange(WM_PLUGINS_FIRST, WM_PLUGINS_LAST);

	CString strPluginKey;
	int jj = 0;
	while(1)
	{
		strPluginKey.Format(_T("KeePlugin_%d"), jj);
		cConfig.Get(strPluginKey, szTemp);
		if(_tcslen(szTemp) == 0) break;
		if(_tcscmp(szTemp, _T("0")) == 0) break;
		if(szTemp[0] != _T('-'))
		{
			std::string strPluginName = szTemp;
			CPluginManager::Instance().m_vKnownNames.push_back(strPluginName);
		}
		jj++;
	}

	std::string strBase = Executable::instance().getPathOnly();
	VERIFY(CPluginManager::Instance().AddAllPlugins(strBase.c_str(), _T("*.dll"), TRUE));
	CPluginManager::Instance().m_vKnownNames.clear();

	// CString strPluginKey;
	// jj = 0;
	// while(1)
	// {
	//	strPluginKey.Format(_T("KeePlugin_%d"), jj);
	//	cConfig.Get(strPluginKey, szTemp);
	//	if(_tcslen(szTemp) == 0) break;
	//	if(_tcscmp(szTemp, _T("0")) == 0) break;
	//	if(szTemp[0] == _T('-'))
	//		CPluginManager::Instance().EnablePluginByStr(&szTemp[1], FALSE);
	//	else
	//		CPluginManager::Instance().EnablePluginByStr(szTemp, TRUE);
	//	jj++;
	// }

	VERIFY(CPluginManager::Instance().LoadAllPlugins());
	BuildPluginMenu();

	_CALLPLUGINS(KPM_READ_COMMAND_ARGS, 0, 0);
	if(_ParseCommandLine() == FALSE)
	{
		if((m_bOpenLastDb == TRUE) && (m_bRememberLast == TRUE))
		{
			cConfig.Get(PWMKEY_LASTDB, szTemp);
			if(_tcslen(szTemp) != 0)
			{
				// TCHAR tszTemp[SI_REGSIZE];
				// int i;

				// GetModuleFileName(NULL, tszTemp, SI_REGSIZE - 2);
				std::string strTemp = "";

				if((szTemp[1] == _T(':')) && (szTemp[2] == _T('\\')))
					// _tcscpy_s(tszTemp, _countof(tszTemp), szTemp);
					strTemp = szTemp;
				else if((szTemp[0] == _T('\\')) && (szTemp[1] == _T('\\')))
					// _tcscpy_s(tszTemp, _countof(tszTemp), szTemp);
					strTemp = szTemp;
				else
				{
					// for(i = _tcslen(tszTemp) - 1; i > 1; i--)
					// {
					//	if((tszTemp[i] == _T('\\')) || (tszTemp[i] == _T('/')))
					//	{
					//		tszTemp[i + 1] = 0;
					//		break;
					//	}
					// }
					
					// _tcscat_s(tszTemp, _countof(tszTemp), szTemp);
					strTemp = Executable::instance().getPathOnly();
					strTemp += szTemp;
				}

				if(_FileAccessible(strTemp.c_str()) == TRUE)
					_OpenDatabase(NULL, strTemp.c_str(), NULL, NULL, m_bStartMinimized, NULL, FALSE);
			}
		}
	}

	UpdateAutoSortMenuItems();
	_UpdateToolBar(TRUE);
	ProcessResize();

	if(m_dwATHotKey != 0) RegisterGlobalHotKey(HOTKEYID_AUTOTYPE, m_dwATHotKey, FALSE, FALSE);

	m_bRestoreHotKeyRegistered = RegisterHotKey(this->m_hWnd, HOTKEYID_RESTORE, MOD_ALT | MOD_CONTROL, 'K');

	// PostMessage(WM_NULL, 0, 0);

	m_uACP = GetACP();

	m_sessionNotify.Register(this->m_hWnd);
	if(m_bCheckForUpdate == TRUE) CheckForUpdateAsync(this->m_hWnd, PWM_VERSION_DW, PWM_URL_VERSION, FALSE);

	UpdateWindow();
	RedrawWindow();

	m_bWasMaximized = cConfig.GetBool(PWMKEY_WINSTATE_MAX, FALSE);
	if(m_bWasMaximized == TRUE) ShowWindow(SW_SHOWMAXIMIZED);

	if(m_bAlwaysOnTop == TRUE)
		SetWindowPos(&wndTopMost, 0, 0, 0, 0,
			SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

	if(m_bStartMinimized == TRUE)
	{
		if(m_bMinimizeToTray == TRUE) ViewHideHandler();
		else ShowWindow(SW_MINIMIZE);
	}

	_CALLPLUGINS(KPM_WND_INIT_POST, 0, 0);
	m_bDisplayDialog = FALSE;

	// m_btnTbAbout.SetFocus();
	// if((m_bFileOpen == FALSE) && (m_cQuickFind.IsWindowEnabled() == TRUE)) m_cQuickFind.SetFocus();
	// else m_btnTbAbout.SetFocus();
	if(m_cQuickFind.IsWindowEnabled() == TRUE) m_cQuickFind.SetFocus();
	else m_btnTbAbout.SetFocus();
	return FALSE; // We set the focus ourselves
}

void CPwSafeDlg::_TranslateMenu(BCMenu *pBCMenu, BOOL bAppendSuffix, BOOL *pFlags)
{
	CString strItem, strNew;
	UINT nItem = 0;
	BCMenu *pNext;
	const TCHAR *pSuffix = _T("");
	BOOL bDefaultFlag = FALSE;

	ASSERT(pBCMenu != NULL);
	if(pBCMenu == NULL) return;

	if(pFlags == NULL) pFlags = &bDefaultFlag;

	while(1)
	{
		if(pBCMenu->GetMenuText((UINT)nItem, strItem, MF_BYPOSITION) == FALSE) break;
		if(bAppendSuffix == TRUE) pSuffix = _GetCmdAccelExt(strItem);

		if((strItem == _T("&Export To")) || (strItem == _T("&Import From")) ||
			(strItem == _T("Show &Columns")) || (strItem == _T("&Rearrange")) ||
			(strItem == _T("Auto-&Sort Password List")) || (strItem == _T("TAN View &Options")))
		{
			pNext = pBCMenu->GetSubBCMenu((TCHAR *)(LPCTSTR)strItem);
			if(pNext != NULL) _TranslateMenu(pNext, TRUE, pFlags);
		}

		// Replace URL menu item if required, pFlags[0] must be a pointer to m_bCopyURLs
		if(*pFlags == TRUE)
			if(strItem == _T("Open &URL(s)"))
				strItem = _T("Copy &URL To Clipboard");

		if(strItem.GetLength() != 0) strNew = TRL_VAR(strItem);
		else strNew = _T("");

		if(_tcslen(pSuffix) != 0) { strNew += _T("\t"); strNew += pSuffix; }
		if(pBCMenu->SetMenuText(nItem, strNew, MF_BYPOSITION) == FALSE) { ASSERT(FALSE); }

		nItem++;
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

	const FullPathName& database = CmdArgs::instance().getDatabase();
	const bool filenameExists = database.getState()==FullPathName::PATH_AND_FILENAME;
	if(!filenameExists) return FALSE;

	const TCHAR* const filename = database.getFullPathName().c_str();
	if(!_FileAccessible(filename)) return TRUE;

	const std_string& password = CmdArgs::instance().getPassword();
	const FullPathName& keyfile = CmdArgs::instance().getKeyfile();
	enum {PATH_EXISTS = FullPathName::PATH_ONLY | FullPathName::PATH_AND_FILENAME};
	const TCHAR* const pKeyfile = keyfile.getState() & PATH_EXISTS ? keyfile.getFullPathName().c_str() : 0;
	const bool preselectIsInEffect = CmdArgs::instance().preselectIsInEffect();

	_OpenDatabase(NULL, filename, password.empty() ? NULL : password.c_str(),
		(!preselectIsInEffect) ? pKeyfile : NULL, FALSE, 
		preselectIsInEffect ? pKeyfile : NULL, FALSE);

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
	static const TCHAR *pEmpty = _T("");

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
	if(_tcsicmp(psz, _T("&Edit/View Entry...")) == 0) return _T("Ctrl+E");
	if(_tcsicmp(psz, _T("&Delete Entry")) == 0) return _T("Ctrl+D");
	if(_tcsicmp(psz, _T("Se&lect All")) == 0) return _T("Ctrl+A");
	if(_tcsicmp(psz, _T("&Find In Database...")) == 0) return _T("Ctrl+F");
	if(_tcsicmp(psz, _T("Add Password &Group...")) == 0) return _T("Ctrl+G");
	if(_tcsicmp(psz, _T("Open &URL(s)")) == 0) return _T("Ctrl+U");
	if(_tcsicmp(psz, _T("Copy &Password To Clipboard")) == 0) return _T("Ctrl+C");
	if(_tcsicmp(psz, _T("Copy User &Name To Clipboard")) == 0) return _T("Ctrl+B");
	if(_tcsicmp(psz, _T("Dupli&cate Entry")) == 0) return _T("Ctrl+K");
	if(_tcsicmp(psz, _T("&Options...")) == 0) return _T("Ctrl+M");
	if(_tcsicmp(psz, _T("Perform Auto-&Type")) == 0) return _T("Ctrl+V");

	if(_tcsicmp(psz, _T("Password &Generator...")) == 0) return _T("Ctrl+Z");

	if(_tcsicmp(psz, _T("Hide &User Names Behind Asterisks (***)")) == 0) return _T("Ctrl+J");
	if(_tcsicmp(psz, _T("Hide &Passwords Behind Asterisks (***)")) == 0) return _T("Ctrl+H");

	if(_tcsicmp(psz, _T("Move Entry To &Top")) == 0) return _T("Alt+Home");
	if(_tcsicmp(psz, _T("Move Entry &One Up")) == 0) return _T("Alt+Up");
	if(_tcsicmp(psz, _T("Mo&ve Entry One Down")) == 0) return _T("Alt+Down");
	if(_tcsicmp(psz, _T("Move Entry To &Bottom")) == 0) return _T("Alt+End");

	if(_tcsicmp(psz, _T("Move Group To &Top")) == 0) return _T("Alt+Home");
	if(_tcsicmp(psz, _T("Move Group One &Up")) == 0) return _T("Alt+Up");
	if(_tcsicmp(psz, _T("Move Group &One Down")) == 0) return _T("Alt+Down");
	if(_tcsicmp(psz, _T("Move Group To &Bottom")) == 0) return _T("Alt+End");
	if(_tcsicmp(psz, _T("Move Group One &Left")) == 0) return _T("Alt+Left");
	if(_tcsicmp(psz, _T("Move Group One &Right")) == 0) return _T("Alt+Right");

	return pEmpty;
}

void CPwSafeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	BOOL m_bRestore = FALSE;

	// if(m_bDisplayDialog == TRUE) return; // No dialog must be displayed at this time

	// Map close button to minimize button if the user wants this
	if((nID == SC_CLOSE) && (m_bCloseMinimizes == TRUE))
	{
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
		return;
	}

	if(nID == SC_MAXIMIZE) m_bWasMaximized = TRUE;
	else if((nID == SC_RESTORE) && (m_bMaximized == TRUE)) m_bWasMaximized = FALSE;
	else if((nID == SC_RESTORE) && (m_bMinimized == FALSE)) m_bWasMaximized = FALSE;

	if((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		if(nID == SC_MINIMIZE)
		{
			if(m_bMinimizeToTray == TRUE)
			{
				m_bShowWindow = FALSE;
				m_systray.MinimiseToTray(this);
			}
			else CDialog::OnSysCommand(nID, lParam);
		}
		else if(nID == SC_RESTORE)
		{
			if((m_bMinimizeToTray == TRUE) && (m_bMinimized == TRUE))
			{
				m_systray.MaximiseFromTray(this);
				m_bShowWindow = TRUE;
			}
			else CDialog::OnSysCommand(nID, lParam);
		}
		else CDialog::OnSysCommand(nID, lParam);

		if(nID == SC_MAXIMIZE)
		{
			m_bMinimized = FALSE;
			// m_bMaximized = TRUE;
		}
		else if(nID == SC_RESTORE)
		{
			if(m_bLocked == TRUE) OnFileLock();

			m_bMinimized = FALSE;
		}
		else if(nID == SC_MINIMIZE)
		{
			if((m_bLocked == FALSE) && (m_bLockOnMinimize == TRUE))
			{
				OnFileLock(); // Lock or unlock, toggle lock state

				// Was the locking successful? If not: restore window
				if((m_bFileOpen == TRUE) && (m_bLocked == FALSE) && (nID == SC_MINIMIZE)) m_bRestore = TRUE;
			}

			m_bMinimized = TRUE;
		}

		WINDOWPLACEMENT wp;
		wp.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(&wp);
		m_bMaximized = (wp.showCmd == SW_SHOWMAXIMIZED) ? TRUE : FALSE;
	}

	_UpdateToolBar();

	// if(m_bRestore == TRUE) SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
	if(m_bRestore == TRUE) ShowWindow(SW_RESTORE);
}

void CPwSafeDlg::OnPaint()
{
	if(IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CPwSafeDlg::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}

void CPwSafeDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	BOOL setflag = FALSE;

	if(lpMeasureItemStruct->CtlType==ODT_MENU)
	{
		if(IsMenu((HMENU)lpMeasureItemStruct->itemID)&&BCMenu::IsMenu((HMENU)lpMeasureItemStruct->itemID))
		{
			m_menu.MeasureItem(lpMeasureItemStruct);
			setflag = TRUE;
		}
	}

	if(!setflag)CDialog::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

LRESULT CPwSafeDlg::OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu)
{
	LRESULT lresult = 0;

	if(BCMenu::IsMenu(pMenu))
		lresult = BCMenu::FindKeyboardShortcut(nChar, nFlags, pMenu);
	else
		lresult = CDialog::OnMenuChar(nChar, nFlags, pMenu);

	return(lresult);
}

void CPwSafeDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	NotifyUserActivity();

	m_dwLastFirstSelectedItem = GetSelectedEntry();
	m_dwLastNumSelectedItems = GetSelectedEntriesCount();
	m_hLastSelectedGroup = m_cGroups.GetSelectedItem();

	// CMenu *p = m_menu.GetSubMenu(TRL("&Tools"));
	// if(p == pPopupMenu) RebuildPluginMenu();

	_CALLPLUGINS(KPM_INIT_MENU_POPUP, 0, 0);

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
	if(cx < 314) cx = 314; // Minimum sizes
	if(cy < 207) cy = 207;

	CDialog::OnSize(nType, cx, cy);

	ProcessResize();
}

void CPwSafeDlg::OnSizing(UINT nSide, LPRECT lpRect)
{
	if((lpRect->right - lpRect->left) < 314) lpRect->right = 314 + lpRect->left;
	if((lpRect->bottom - lpRect->top) < 207) lpRect->bottom = 207 + lpRect->top;

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
	int tbHeight = 23;

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
	if((rectWindow.right - rectWindow.left) < (314 - 32)) bWindowValid = FALSE;
	if((rectWindow.bottom - rectWindow.top) < (207 - 32)) bWindowValid = FALSE;

	if((m_bMinimized == FALSE) && (m_bShowWindow == TRUE) && (bWindowValid == TRUE))
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
		rectList.top = GUI_SPACER + nAddTop;
		rectList.bottom = rectClient.bottom - (GUI_SPACER >> 1) - cyMenu - nEntryViewHeight - 1;
		rectList.left = 0;
		rectList.right = GUI_SPACER + m_lSplitterPosHoriz - 1;
		m_cGroups.MoveWindow(&rectList, TRUE);

		// int nColumnWidth = (rectList.right - rectList.left) -
		//	GetSystemMetrics(SM_CXVSCROLL) - 8;
		// m_cGroups.SetColumnWidth(0, nColumnWidth);
	}

	if(IsWindow(m_cList.m_hWnd)) // Resize password list view
	{
		rectList.top = GUI_SPACER + nAddTop;
		rectList.bottom = rectClient.bottom - (GUI_SPACER >> 1) - cyMenu - nEntryViewHeight - 1;
		rectList.left = GUI_SPACER + m_lSplitterPosHoriz + 2;
		rectList.right = rectClient.right;
		m_cList.MoveWindow(&rectList, TRUE);

		if(m_bColAutoSize == TRUE) _CalcColumnSizes();
		_SetColumnWidths();
	}

	if(IsWindow(m_reEntryView.m_hWnd))
	{
		rectList.top = rectClient.bottom - (GUI_SPACER >> 1) - cyMenu - nEntryViewHeight + (GUI_SPACER >> 1);
		rectList.bottom = rectClient.bottom - (GUI_SPACER >> 1) - cyMenu;
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
	m_sessionNotify.Unregister();

	if(m_bTimer == TRUE)
	{
		KillTimer(APPWND_TIMER_ID);
		m_bTimer = FALSE;
	}

	if(m_dwATHotKey != 0) UnregisterHotKey(this->m_hWnd, HOTKEYID_AUTOTYPE);

	if(m_bRestoreHotKeyRegistered != FALSE) UnregisterHotKey(this->m_hWnd, HOTKEYID_RESTORE);

	CPluginManager::Instance().UnloadAllPlugins();

	if(m_bMenu == TRUE)
	{
		// Auto-destroyed in BCMenu destructor
		// m_menu.DestroyMenu();

		m_bMenu = FALSE;
	}
	DeleteContextMenus();

	if(m_nClipboardCountdown >= 0)
	{
		ClearClipboardIfOwner(); // This clears the clipboard if we own it
		m_nClipboardCountdown = -1; // Disable clipboard clear countdown
	}

	_DeleteTemporaryFiles();
	FreeCurrentTranslationTable();

	SaveOptions();

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

	m_mgr.NewDatabase();
	m_cList.DeleteAllItems();
	m_cGroups.DeleteAllItems();

	m_ilIcons.DeleteImageList();

	SAFE_DELETE(m_pThemeHelper);

	if(m_bRestartApplication == TRUE) RestartApplication();
}

void CPwSafeDlg::SaveOptions()
{
	CPrivateConfig pcfg(TRUE);
	TCHAR szTemp[1024];
	CString strTemp;

	// Save clipboard auto-clear time
	_ultot_s(m_dwClipboardSecs, szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_CLIPSECS, szTemp);

	_itot_s(m_nClipboardMethod, szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_CLIPBOARDMETHOD, szTemp);

	GetCurrentDirectory(1024, szTemp);
	if(_tcslen(szTemp) != 0) pcfg.Set(PWMKEY_LASTDIR, szTemp);

	_ultot_s(m_dwATHotKey, szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_AUTOTYPEHOTKEY, szTemp);

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
		// TCHAR tszTemp[SI_REGSIZE];
		// GetModuleFileName(NULL, tszTemp, SI_REGSIZE - 2);
		std::string strTemp = Executable::instance().getFullPathName();
		pcfg.Set(PWMKEY_LASTDB, MakeRelativePathEx(strTemp.c_str(), m_strLastDb));
	}
	else pcfg.Set(PWMKEY_LASTDB, _T(""));

	pcfg.SetBool(PWMKEY_STARTMINIMIZED, m_bStartMinimized);
	pcfg.SetBool(PWMKEY_DISABLEUNSAFE, m_bDisableUnsafe);
	pcfg.SetBool(PWMKEY_IMGBTNS, m_bImgButtons);
	pcfg.SetBool(PWMKEY_ENTRYGRID, m_bEntryGrid);
	pcfg.SetBool(PWMKEY_AUTOSHOWEXPIRED, m_bAutoShowExpired);
	pcfg.SetBool(PWMKEY_AUTOSHOWEXPIREDS, m_bAutoShowExpiredSoon);
	pcfg.SetBool(PWMKEY_BACKUPENTRIES, m_bBackupEntries);
	pcfg.SetBool(PWMKEY_SECUREEDITS, m_bSecureEdits);
	pcfg.SetBool(PWMKEY_SINGLECLICKTRAY, m_bSingleClickTrayIcon);
	pcfg.SetBool(PWMKEY_AUTOPWGEN, m_bAutoPwGen);
	pcfg.SetBool(PWMKEY_QUICKFINDINCBK, m_bQuickFindIncBackup);
	pcfg.SetBool(PWMKEY_DELETEBKONSAVE, m_bDeleteBackupsOnSave);
	pcfg.SetBool(PWMKEY_DISABLEAUTOTYPE, m_bDisableAutoType);
	pcfg.SetBool(PWMKEY_COPYURLS, m_bCopyURLs);
	pcfg.SetBool(PWMKEY_EXITINSTEADLOCK, m_bExitInsteadOfLockAT);

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

	pcfg.SetBool(PWMKEY_ENTRYVIEW, m_bEntryView);

	pcfg.SetBool(PWMKEY_HIDESTARS, m_bPasswordStars);
	pcfg.SetBool(PWMKEY_HIDEUSERS, m_bUserStars);
	pcfg.SetBool(PWMKEY_ALWAYSTOP, m_bAlwaysOnTop);
	pcfg.SetBool(PWMKEY_LOCKMIN, m_bLockOnMinimize);
	pcfg.SetBool(PWMKEY_MINTRAY, m_bMinimizeToTray);
	pcfg.SetBool(PWMKEY_CLOSEMIN, m_bCloseMinimizes);
	pcfg.SetBool(PWMKEY_SHOWTOOLBAR, m_bShowToolBar);
	pcfg.SetBool(PWMKEY_COLAUTOSIZE, m_bColAutoSize);
	pcfg.SetBool(PWMKEY_SHOWFULLPATH, m_bShowFullPath);

	pcfg.SetBool(PWMKEY_SIMPLETANVIEW, m_bSimpleTANView);
	pcfg.SetBool(PWMKEY_SHOWTANINDICES, m_bShowTANIndices);
	pcfg.SetBool(PWMKEY_ALLOWSAVEIFMODIFIEDONLY, m_bAllowSaveIfModifiedOnly);
	pcfg.SetBool(PWMKEY_CHECKFORUPDATE, m_bCheckForUpdate);

	NewGUI_GetHeaderOrder(m_cList.m_hWnd, m_aHeaderOrder, 11);
	ar2str(szTemp, m_aHeaderOrder, 11);
	pcfg.Set(PWMKEY_HEADERORDER, szTemp);

	CString strOptions, strCharSet; UINT nChars;
	CPwGeneratorDlg::GetOptions(&strOptions, &strCharSet, &nChars);
	pcfg.Set(PWMKEY_PWGEN_OPTIONS, strOptions);
	pcfg.Set(PWMKEY_PWGEN_CHARS, strCharSet);
	_itot_s((int)nChars, szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_PWGEN_NUMCHARS, szTemp);

	_itot_s(m_nAutoTypeMethod, szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_AUTOTYPEMETHOD, szTemp);

	pcfg.Set(PWMKEY_LISTFONT, m_strFontSpec);

	if((m_bMinimized == FALSE) && (m_bMaximized == FALSE))
	{
		RECT rect;

		GetWindowRect(&rect);
		_itot_s(rect.left, szTemp, _countof(szTemp), 10);
		pcfg.Set(PWMKEY_WINDOWPX, szTemp);
		_itot_s(rect.top, szTemp, _countof(szTemp), 10);
		pcfg.Set(PWMKEY_WINDOWPY, szTemp);
		_itot_s(rect.right - rect.left, szTemp, _countof(szTemp), 10);
		pcfg.Set(PWMKEY_WINDOWDX, szTemp);
		_itot_s(rect.bottom - rect.top, szTemp, _countof(szTemp), 10);
		pcfg.Set(PWMKEY_WINDOWDY, szTemp);
	}

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

	pcfg.SetBool(PWMKEY_WINSTATE_MAX, m_bMaximized);

	_ltot_s((long)m_cList.GetRowColorEx(), szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_ROWCOLOR, szTemp);

	_ltot_s(m_nLockTimeDef, szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_LOCKTIMER, szTemp);

	_ltot_s((long)m_dwDefaultExpire, szTemp, _countof(szTemp), 10);
	pcfg.Set(PWMKEY_DEFAULTEXPIRE, szTemp);

	int j = 0;
	CString strT;
	for(int i = 0; i < (int)CPluginManager::Instance().m_plugins.size(); i++)
	{
		if(CPluginManager::Instance().m_plugins[i].bEnabled == FALSE) continue;

		strTemp.Format(_T("KeePlugin_%d"), j);
		strT = CPluginManager::Instance().m_plugins[i].tszFile;

		pcfg.Set(strTemp, CsFileOnly(&strT));
		j++;
	}
	strTemp.Format(_T("KeePlugin_%d"), j);
	VERIFY(pcfg.Set(strTemp, _T("0")));
}

void CPwSafeDlg::_DeleteTemporaryFiles()
{
	if(_CALLPLUGINS(KPM_DELETE_TEMP_FILES_PRE, 0, 0) == FALSE) return;

	if(m_strTempFile.IsEmpty() == FALSE)
	{
		if(SecureDeleteFile(m_strTempFile) == TRUE)
			m_strTempFile.Empty();
	}
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
	if((GetKeyState(27) & 0x8000) != 0)
	{
		if(m_bLocked == FALSE) OnFileLock();
		return;
	}

	if((m_bMenuExit == FALSE) && (m_bCloseMinimizes == TRUE)) return;

	if(_CALLPLUGINS(KPM_WM_CANCEL, 0, 0) == FALSE) return;

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

	if(_CALLPLUGINS(KPM_FILE_EXIT_PRE, 0, 0) == FALSE) return;

	m_bMenuExit = TRUE;
	OnCancel();
	m_bMenuExit = FALSE;
}

void CPwSafeDlg::OnInfoAbout()
{
	NotifyUserActivity();
	m_bDisplayDialog = TRUE;

	CAboutDlg dlg;

	if(_CALLPLUGINS(KPM_INFO_ABOUT_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

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

	dlg.m_nIconId = 48;
	dlg.m_strGroupName.Empty();
	dlg.m_pParentImageList = &m_ilIcons;
	dlg.m_bEditMode = FALSE;

	if(_CALLPLUGINS(KPM_GROUP_ADD_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	if(dlg.DoModal() == IDOK)
	{
		_GetCurrentPwTime(&pwTime);
		pwTemplate.pszGroupName = (LPTSTR)(LPCTSTR)dlg.m_strGroupName;
		pwTemplate.tCreation = pwTime;
		m_mgr.GetNeverExpireTime(&pwTemplate.tExpire);
		pwTemplate.tLastAccess = pwTime;
		pwTemplate.tLastMod = pwTime;
		pwTemplate.uGroupId = 0; // 0 = create new group
		pwTemplate.uImageId = (DWORD)dlg.m_nIconId;
		pwTemplate.usLevel = 0; pwTemplate.dwFlags = 0;

		if(_CALLPLUGINS(KPM_GROUP_ADD, &pwTemplate, 0) == FALSE)
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

	UINT uState;
	BOOL bChecked;
	int nItem;
	LV_ITEM lvi;

	if(_CALLPLUGINS(KPM_VIEW_HIDE_STARS_PRE, 0, 0) == FALSE) return;

	if((m_bPasswordStars == TRUE) && (_IsUnsafeAllowed() == FALSE)) return;

	uState = m_menu.GetMenuState(ID_VIEW_HIDESTARS, MF_BYCOMMAND);
	ASSERT(uState != 0xFFFFFFFF);

	if(uState & MF_CHECKED) bChecked = TRUE;
	else bChecked = FALSE;

	if(bChecked == TRUE)
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
		ZeroMemory(&lvi, sizeof(LV_ITEM));
		lvi.mask = LVIF_TEXT;
		lvi.iSubItem = 3;
		lvi.pszText = PWM_PASSWORD_STRING;
		for(nItem = 0; nItem < m_cList.GetItemCount(); nItem++)
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

DWORD CPwSafeDlg::GetSelectedEntry()
{
	NotifyUserActivity();

	// DWORD i;
	// UINT uState;

	if(m_bFileOpen == FALSE) return DWORD_MAX;

	// LVIS_FOCUSED is not enough here, it must be LVIS_SELECTED
	// for(i = 0; i < (DWORD)m_cList.GetItemCount(); i++)
	// {
	//	uState = m_cList.GetItemState((int)i, LVIS_SELECTED);
	//	if(uState & LVIS_SELECTED) return i;
	// }

	POSITION posFirstItem = m_cList.GetFirstSelectedItemPosition();
	if(posFirstItem == NULL) return DWORD_MAX;

	return (DWORD)m_cList.GetNextSelectedItem(posFirstItem);
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
	//	if(uState & LVIS_SELECTED) uSelectedItems++;
	// }
	// return uSelectedItems;

	return (DWORD)m_cList.GetSelectedCount();
}

DWORD CPwSafeDlg::GetSelectedGroupId()
{
	HTREEITEM h = m_cGroups.GetSelectedItem();
	if(h == NULL) return DWORD_MAX;

	return m_cGroups.GetItemData(h);
}

void CPwSafeDlg::UpdateGroupList()
{
	NotifyUserActivity();

	DWORD i;
	TVINSERTSTRUCT tvis;
	PW_GROUP *pgrp;
	HTREEITEM hParent = TVI_ROOT;
	HTREEITEM hLastItem = TVI_ROOT;
	DWORD usLevel = 0;

	m_cGroups.SetRedraw(FALSE);
	GroupSyncStates(TRUE); // Synchronize 'expanded'-flag from GUI to list manager
	m_cGroups.SelectItem(NULL);
	m_cGroups.DeleteAllItems();

	if(m_bFileOpen == FALSE) { m_cGroups.SetRedraw(TRUE); return; }
	// m_cGroups.SetRedraw(FALSE);

	ZeroMemory(&tvis, sizeof(TVINSERTSTRUCT));
	tvis.hInsertAfter = TVI_LAST;
	tvis.item.mask = TVIF_IMAGE | TVIF_TEXT | TVIF_PARAM | TVIF_STATE | TVIF_SELECTEDIMAGE;

	for(i = 0; i < m_mgr.GetNumberOfGroups(); i++)
	{
		pgrp = m_mgr.GetGroup(i);
		ASSERT(pgrp != NULL); if(pgrp == NULL) continue;
		ASSERT(pgrp->pszGroupName != NULL);

		while(1)
		{
			if(usLevel == pgrp->usLevel) break;
			else if(usLevel == (USHORT)(pgrp->usLevel - 1))
			{
				hParent = hLastItem;
				usLevel++;
			}
			else if(usLevel < pgrp->usLevel) { ASSERT(FALSE); hParent = TVI_ROOT; break; }
			else if(usLevel > pgrp->usLevel)
			{
				if(hParent == TVI_ROOT) break;

				hParent = m_cGroups.GetParentItem(hParent);
				usLevel--;
			}
		}

		tvis.hParent = hParent;

		tvis.item.pszText = pgrp->pszGroupName;
		tvis.item.iSelectedImage = tvis.item.iImage = (int)pgrp->uImageId;
		tvis.item.lParam = pgrp->uGroupId;
		tvis.item.stateMask = TVIS_EXPANDED;
		tvis.item.state = (pgrp->dwFlags & PWGF_EXPANDED) ? TVIS_EXPANDED : 0;

		hLastItem = m_cGroups.InsertItem(&tvis);
	}

	m_cGroups.SetRedraw(TRUE);
	m_cGroups.Invalidate();

	GroupSyncStates(FALSE); // Expand all tree items that were open before
	if(i > 0) m_cGroups.SelectItem(m_cGroups.GetRootItem());
}

void CPwSafeDlg::UpdatePasswordList()
{
	NotifyUserActivity();

	DWORD i, j = 0;
	DWORD dwGroupId;
	PW_ENTRY *pwe;
	PW_TIME tNow;

	if(m_bFileOpen == FALSE) return;

	dwGroupId = GetSelectedGroupId();
	if(dwGroupId == DWORD_MAX) return;

	if(m_bBlockPwListUpdate == TRUE) return;
	m_bBlockPwListUpdate = TRUE;
	_SortListIfAutoSort();
	m_bBlockPwListUpdate = FALSE;

	m_cList.SetRedraw(FALSE);

	m_cList.DeleteAllItems();
	m_bTANsOnly = TRUE;

	_GetCurrentPwTime(&tNow);

	for(i = 0; i < m_mgr.GetNumberOfEntries(); i++)
	{
		pwe = m_mgr.GetEntry(i);
		ASSERT_ENTRY(pwe);

		if(pwe != NULL)
		{
			if(pwe->uGroupId == dwGroupId)
			{
				_List_SetEntry(j, pwe, TRUE, &tNow);
				j++;
			}
		}
	}

	if(j == 0) m_bTANsOnly = FALSE; // Use report list view
	AdjustPwListMode();
	if(j == 0) m_bTANsOnly = TRUE; // Now set it to the correct value

	AdjustColumnWidths();

	m_cList.SetRedraw(TRUE);
	m_cList.Invalidate();

	ShowEntryDetails(NULL);
}

void CPwSafeDlg::AdjustPwListMode()
{
	DWORD dwStyle = m_cList.GetStyle();

	// Don't use SetRedraw and Invalidate in this function!

	if((m_bTANsOnly == TRUE) && (m_bSimpleTANView == TRUE))
	{
		if((dwStyle & LVS_REPORT) != 0) // Is in report display mode
		{
			m_dwPwListMode = LVSX_CHANGING;

			// for(int i = 1; i < 11; i++) m_cList.SetColumnWidth(i, 0);
			// m_cList.SetColumnWidth(0, 100); // LVSCW_AUTOSIZE

			m_cList.ModifyStyle(LVS_REPORT, LVS_SMALLICON);
			// m_cList.SetColumnWidth(-1, LVSCW_AUTOSIZE);

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

	int n, nMaxWidth = 0;

	for(int i = 0; i < m_cList.GetItemCount(); i++)
	{
		n = m_cList.GetStringWidth(m_cList.GetItemText(i, 0));

		if(n > nMaxWidth) nMaxWidth = n;
	}

	nMaxWidth += 26;
	if(m_cList.GetColumnWidth(-1) != nMaxWidth)
		m_cList.SetColumnWidth(-1, nMaxWidth);
}

void CPwSafeDlg::_List_SetEntry(DWORD dwInsertPos, PW_ENTRY *pwe, BOOL bIsNewEntry, PW_TIME *ptNow)
{
	LV_ITEM lvi;
	DWORD i, t;
	CString strTemp;
	DWORD uImageId;

	if((dwInsertPos == DWORD_MAX) && (bIsNewEntry == TRUE))
		dwInsertPos = (DWORD)m_cList.GetItemCount();

	BOOL bIsTAN = (_tcscmp(pwe->pszTitle, PWS_TAN_ENTRY) != 0) ? FALSE : TRUE;
	BOOL bValidTANIndex = TRUE;
	TCHAR tch;

	if(bIsTAN == FALSE) m_bTANsOnly = FALSE;

	ZeroMemory(&lvi, sizeof(LV_ITEM));
	lvi.iItem = (int)dwInsertPos;
	lvi.iSubItem = 0;

	// Set 'expired' image if necessary
	if(_pwtimecmp(&pwe->tExpire, ptNow) <= 0) uImageId = 45;
	else uImageId = pwe->uImageId;

	lvi.mask = LVIF_TEXT | LVIF_IMAGE;
	if((m_bShowTitle == TRUE) || (bIsTAN == TRUE))
	{
		if((bIsTAN == FALSE) || (m_bShowTANIndices == FALSE))
			lvi.pszText = pwe->pszTitle;
		else
		{
			strTemp = pwe->pszTitle;

			if(pwe->pszAdditional[0] != 0)
			{
				i = 0;
				while(1)
				{
					tch = pwe->pszAdditional[i];

					if(tch == 0) break;
					else if((tch < _T('0')) || (tch > _T('9')))
					{
						bValidTANIndex = FALSE;
						break;
					}

					i++;
				}

				if(bValidTANIndex == TRUE)
				{
					strTemp += _T(" (#");
					strTemp += pwe->pszAdditional;
					strTemp += _T(")");
				}
			}

			lvi.pszText = (LPTSTR)(LPCTSTR)strTemp;
		}
	}
	else
		lvi.pszText = g_pNullString;
	lvi.iImage = uImageId;

	if(bIsNewEntry == TRUE)
		m_cList.InsertItem(&lvi); // Add
	else
		m_cList.SetItem(&lvi); // Set

	lvi.mask = LVIF_TEXT;
	lvi.iSubItem = 1;
	if(m_bShowUserName == TRUE)
	{
		// Hide usernames behind ******** if the user has selected this option
		if(m_bUserStars == TRUE)
		{
			lvi.pszText = PWM_PASSWORD_STRING;
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
	if(m_bShowURL == TRUE)
		lvi.pszText = pwe->pszURL;
	else
		lvi.pszText = g_pNullString;
	m_cList.SetItem(&lvi);

	lvi.iSubItem = 3;
	if(m_bShowPassword == TRUE)
	{
		// Hide passwords behind ******** if the user has selected this option
		if(m_bPasswordStars == TRUE)
		{
			lvi.pszText = PWM_PASSWORD_STRING;
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
		for(t = 0; t < (DWORD)strTemp.GetLength(); t++)
			if((strTemp.GetAt(t) == _T('\r')) || (strTemp.GetAt(t) == _T('\n')))
				strTemp.SetAt(t, _T(' '));
		lvi.pszText = (LPTSTR)(LPCTSTR)strTemp;
	}
	else
		lvi.pszText = g_pNullString;
	m_cList.SetItem(&lvi);

	lvi.iSubItem = 5;
	if(m_bShowCreation == TRUE)
	{
		_PwTimeToString(pwe->tCreation, &strTemp);
		lvi.pszText = (LPTSTR)(LPCTSTR)strTemp;
	}
	else
		lvi.pszText = g_pNullString;
	m_cList.SetItem(&lvi);

	lvi.iSubItem = 6;
	if(m_bShowLastMod == TRUE)
	{
		_PwTimeToString(pwe->tLastMod, &strTemp);
		lvi.pszText = (LPTSTR)(LPCTSTR)strTemp;
	}
	else
		lvi.pszText = g_pNullString;
	m_cList.SetItem(&lvi);

	lvi.iSubItem = 7;
	if(m_bShowLastAccess == TRUE)
	{
		_PwTimeToString(pwe->tLastAccess, &strTemp);
		lvi.pszText = (LPTSTR)(LPCTSTR)strTemp;
	}
	else
		lvi.pszText = g_pNullString;
	m_cList.SetItem(&lvi);

	lvi.iSubItem = 8;
	if(m_bShowExpire == TRUE)
	{
		if(memcmp(&pwe->tExpire, &g_tNeverExpire, sizeof(PW_TIME)) == 0)
		{
			lvi.pszText = (TCHAR *)g_psztNeverExpires;
		}
		else
		{
			_PwTimeToString(pwe->tExpire, &strTemp);

			// The COleDateTime class seems to be really slow, any better ideas?
			// COleDateTime dt((int)pwe->tExpire.shYear, (int)pwe->tExpire.btMonth, (int)pwe->tExpire.btDay,
			//	(int)pwe->tExpire.btHour, (int)pwe->tExpire.btMinute, (int)pwe->tExpire.btSecond);
			// strTemp = dt.Format(LOCALE_NOUSEROVERRIDE, LANG_USER_DEFAULT);

			lvi.pszText = (LPTSTR)(LPCTSTR)strTemp;
		}
	}
	else
		lvi.pszText = g_pNullString;
	m_cList.SetItem(&lvi);

	// Ignore m_bShowUUID, the UUID field is needed in all cases
	lvi.iSubItem = 9;
	_UuidToString(pwe->uuid, &strTemp);
	lvi.pszText = (LPTSTR)(LPCTSTR)strTemp;
	m_cList.SetItem(&lvi);

	lvi.iSubItem = 10;
	if(m_bShowAttach == TRUE)
		lvi.pszText = pwe->pszBinaryDesc;
	else
		lvi.pszText = g_pNullString;
	m_cList.SetItem(&lvi);
}

void CPwSafeDlg::RefreshPasswordList()
{
	NotifyUserActivity();

	DWORD i, j = 0;
	PW_ENTRY *pwe;
	LV_ITEM lvi;
	TCHAR szTemp[1024];
	BYTE aUuid[16];
	CString strTemp;
	PW_TIME tNow;

	if(m_bFileOpen == FALSE) return;

	_GetCurrentPwTime(&tNow);

	m_cList.SetRedraw(FALSE);

	for(i = 0; i < (DWORD)m_cList.GetItemCount(); i++)
	{
		ZeroMemory(&lvi, sizeof(LV_ITEM));
		lvi.iItem = (int)i;
		lvi.iSubItem = 9;
		lvi.mask = LVIF_TEXT;
		lvi.cchTextMax = 1024;
		lvi.pszText = szTemp;
		m_cList.GetItem(&lvi);
		strTemp = lvi.pszText;

		_StringToUuid(strTemp, aUuid);
		pwe = m_mgr.GetEntryByUuid(aUuid);
		ASSERT_ENTRY(pwe);

		if(pwe != NULL)
		{
			_List_SetEntry(j, pwe, FALSE, &tNow);
			j++;
		}
	}

	_SortListIfAutoSort();
	AdjustPwListMode();
	AdjustColumnWidths();

	m_cList.SetRedraw(TRUE);
	m_cList.Invalidate();
}

void CPwSafeDlg::OnPwlistAdd()
{
	NotifyUserActivity();

	CAddEntryDlg dlg;
	DWORD uGroupId = GetSelectedGroupId();
	PW_ENTRY pwTemplate;
	PW_TIME tNow;
	DWORD dwInitialGroup; // ID
	BYTE aUuid[16];

	if(m_bFileOpen == FALSE) return;
	if(uGroupId == DWORD_MAX) return; // No group selected or other error

	m_bDisplayDialog = TRUE;

	dlg.m_pMgr = &m_mgr;
	dlg.m_dwEntryIndex = DWORD_MAX;
	dlg.m_pParentIcons = &m_ilIcons;
	dlg.m_bStars = m_bPasswordStars;
	dlg.m_nGroupId = (int)m_mgr.GetGroupByIdN(uGroupId); // m_nGroupId of the dialog is an index, not an ID
	dlg.m_nIconId = 0;
	dlg.m_bEditMode = FALSE;
	dlg.m_dwDefaultExpire = m_dwDefaultExpire;
	dlg.m_bAutoPwGen = m_bAutoPwGen;
	dwInitialGroup = uGroupId; // ID

	if(m_dwDefaultExpire == 0) m_mgr.GetNeverExpireTime(&dlg.m_tExpire);
	else
	{
		CTime t = CTime::GetCurrentTime();
		t += CTimeSpan((LONG)m_dwDefaultExpire, 0, 0, 0);

		dlg.m_tExpire.btDay = (BYTE)t.GetDay();
		dlg.m_tExpire.btHour = (BYTE)t.GetHour();
		dlg.m_tExpire.btMinute = (BYTE)t.GetMinute();
		dlg.m_tExpire.btMonth = (BYTE)t.GetMonth();
		dlg.m_tExpire.btSecond = (BYTE)t.GetSecond();
		dlg.m_tExpire.shYear = (USHORT)t.GetYear();
	}

	if(_CALLPLUGINS(KPM_ADD_ENTRY_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	if(dlg.DoModal() == IDOK)
	{
		ASSERT(dlg.m_lpPassword != NULL); if(dlg.m_lpPassword == NULL) { dlg.m_lpPassword = new TCHAR[2]; dlg.m_lpPassword[0] = 0; }
		ASSERT(dlg.m_lpRepeatPw != NULL); if(dlg.m_lpRepeatPw == NULL) { dlg.m_lpRepeatPw = new TCHAR[2]; dlg.m_lpRepeatPw[0] = 0; }

		_GetCurrentPwTime(&tNow);
		memset(&pwTemplate, 0, sizeof(PW_ENTRY));
		pwTemplate.pszAdditional = (TCHAR *)(LPCTSTR)dlg.m_strNotes;
		pwTemplate.pszPassword = dlg.m_lpPassword;
		pwTemplate.pszTitle = (TCHAR *)(LPCTSTR)dlg.m_strTitle;
		pwTemplate.pszURL = (TCHAR *)(LPCTSTR)dlg.m_strURL;
		pwTemplate.pszUserName = (TCHAR *)(LPCTSTR)dlg.m_strUserName;
		pwTemplate.tCreation = tNow;
		pwTemplate.tExpire = dlg.m_tExpire;
		pwTemplate.tLastAccess = tNow;
		pwTemplate.tLastMod = tNow;
		pwTemplate.uGroupId = m_mgr.GetGroupIdByIndex((DWORD)dlg.m_nGroupId);
		pwTemplate.uImageId = (DWORD)dlg.m_nIconId;
		pwTemplate.uPasswordLen = (DWORD)_tcslen(pwTemplate.pszPassword);
		pwTemplate.pszBinaryDesc = _T("");

		if(_CALLPLUGINS(KPM_ADD_ENTRY, &pwTemplate, 0) == FALSE)
			{ m_bDisplayDialog = FALSE; return; }

		// Add the entry to the password manager
		VERIFY(m_mgr.AddEntry(&pwTemplate));

		CSecureEditEx::DeletePassword(dlg.m_lpPassword); dlg.m_lpPassword = NULL;
		CSecureEditEx::DeletePassword(dlg.m_lpRepeatPw); dlg.m_lpRepeatPw = NULL;

		PW_ENTRY *pNew = m_mgr.GetLastEditedEntry();

		int nAttachLen = dlg.m_strAttachment.GetLength();
		int nEscapeLen = (int)_tcslen(PWS_NEW_ATTACHMENT);
		memcpy(aUuid, pNew->uuid, 16);

		if(nAttachLen > nEscapeLen)
		{
			if(dlg.m_strAttachment.Left(_tcslen(PWS_NEW_ATTACHMENT)) == CString(PWS_NEW_ATTACHMENT))
				m_mgr.AttachFileAsBinaryData(pNew,
					dlg.m_strAttachment.Right(dlg.m_strAttachment.GetLength() - _tcslen(PWS_NEW_ATTACHMENT)));
		}

		// Add the password to the GUI, but only if it's visible
		if(pNew->uGroupId == dwInitialGroup) // dwInitialGroup is an ID
		{
			_List_SetEntry(DWORD_MAX, pNew, TRUE, &tNow); // No unlock needed

			_SortListIfAutoSort();

			AdjustPwListMode();
			AdjustColumnWidths();

			if(m_nAutoSort == 0)
			{
				m_cList.EnsureVisible(m_cList.GetItemCount() - 1, FALSE);
				m_cList.SetItemState(m_cList.GetItemCount() - 1, LVIS_SELECTED, LVIS_SELECTED);
			}
			else
			{
				DWORD dw = _EntryUuidToListPos(aUuid); ASSERT(dw != DWORD_MAX);
				if(dw != DWORD_MAX)
				{
					m_cList.EnsureVisible((int)dw, FALSE);
					m_cList.SetItemState((int)dw, LVIS_SELECTED, LVIS_SELECTED);
				}
			}
		}

		m_bModified = TRUE; // Haven't we? :)
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
	LV_ITEM lvi;
	DWORD dwSel;
	TCHAR tszTemp[40];
	CString strTemp;
	BYTE aUuid[16];

	if(dwSelected == DWORD_MAX) dwSel = GetSelectedEntry();
	else dwSel = dwSelected;

	if(dwSel == DWORD_MAX) return DWORD_MAX;

	ZeroMemory(&lvi, sizeof(LV_ITEM));
	lvi.iItem = (int)dwSel;
	lvi.iSubItem = 9;
	lvi.mask = LVIF_TEXT;
	lvi.pszText = tszTemp;
	lvi.cchTextMax = 40;
	if(m_cList.GetItem(&lvi) == FALSE) return DWORD_MAX;

	strTemp = lvi.pszText;
	_StringToUuid(strTemp, aUuid);

	EraseCString(&strTemp);

	dwSel = m_mgr.GetEntryByUuidN(aUuid);
	ASSERT(dwSel != DWORD_MAX);
	return dwSel;
}

void CPwSafeDlg::OnPwlistEdit()
{
	NotifyUserActivity();

	DWORD dwEntryIndex;
	PW_ENTRY *pEntry;
	CAddEntryDlg dlg;
	PW_ENTRY pwTemplate;
	PW_TIME tNow;
	DWORD dwNewGroupId;
	BOOL bNeedFullUpdate = FALSE, bNeedGroupUpdate = FALSE;
	int nModifyLevel = 0;
	DWORD dwSelectedEntry = GetSelectedEntry();
	BYTE aUuid[16];
	PW_ENTRY pweBackup;
	BOOL bDoBackup = FALSE;

	if(m_bFileOpen == FALSE) return;

	ASSERT(dwSelectedEntry != DWORD_MAX); if(dwSelectedEntry == DWORD_MAX) return;

	dwEntryIndex = _ListSelToEntryIndex(dwSelectedEntry);
	ASSERT(dwEntryIndex != DWORD_MAX); if(dwEntryIndex == DWORD_MAX) return;

	pEntry = m_mgr.GetEntry(dwEntryIndex);
	ASSERT_ENTRY(pEntry); if(pEntry == NULL) return;

	m_bDisplayDialog = TRUE;

	dlg.m_pMgr = &m_mgr;
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
	dlg.m_lpPassword = _TcsSafeDupAlloc(pEntry->pszPassword);
	dlg.m_lpRepeatPw = _TcsSafeDupAlloc(pEntry->pszPassword);
	m_mgr.LockEntryPassword(pEntry);

	if(_CALLPLUGINS(KPM_EDIT_ENTRY_PRE, 0, 0) == FALSE)
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
			bDoBackup = m_mgr.MemAllocCopyEntry(pEntry, &pweBackup);

		m_mgr.LockEntryPassword(pEntry);

		pwTemplate = *pEntry;

		_GetCurrentPwTime(&tNow);

		pwTemplate.pszAdditional = (TCHAR *)(LPCTSTR)dlg.m_strNotes;
		pwTemplate.pszPassword = dlg.m_lpPassword;
		pwTemplate.pszTitle = (TCHAR *)(LPCTSTR)dlg.m_strTitle;
		pwTemplate.pszURL = (TCHAR *)(LPCTSTR)dlg.m_strURL;
		pwTemplate.pszUserName = (TCHAR *)(LPCTSTR)dlg.m_strUserName;
		// pwTemplate.tCreation = pEntry->tCreation;
		pwTemplate.tExpire = dlg.m_tExpire;
		pwTemplate.tLastAccess = tNow;
		pwTemplate.tLastMod = tNow;
		pwTemplate.uImageId = (DWORD)dlg.m_nIconId;
		pwTemplate.uPasswordLen = (DWORD)_tcslen(pwTemplate.pszPassword);

		// If the entry has been moved to a different group, a full
		// update of the list is required
		dwNewGroupId = m_mgr.GetGroupIdByIndex((DWORD)dlg.m_nGroupId);
		if(dwNewGroupId != pwTemplate.uGroupId) bNeedFullUpdate = TRUE;
		pwTemplate.uGroupId = dwNewGroupId;

		VERIFY(m_mgr.SetEntry(dwEntryIndex, &pwTemplate));

		CSecureEditEx::DeletePassword(dlg.m_lpPassword); dlg.m_lpPassword = NULL;
		CSecureEditEx::DeletePassword(dlg.m_lpRepeatPw); dlg.m_lpRepeatPw = NULL;

		int nAttachLen = dlg.m_strAttachment.GetLength();
		int nEscapeLen = (int)_tcslen(PWS_NEW_ATTACHMENT);

		if((nAttachLen == nEscapeLen) && (dlg.m_strAttachment == CString(PWS_NEW_ATTACHMENT)))
		{
			m_mgr.RemoveBinaryData(m_mgr.GetEntry(dwEntryIndex));
		}
		else if(nAttachLen > nEscapeLen)
		{
			if(dlg.m_strAttachment.Left((int)_tcslen(PWS_NEW_ATTACHMENT)) == CString(PWS_NEW_ATTACHMENT))
				m_mgr.AttachFileAsBinaryData(m_mgr.GetEntry(dwEntryIndex),
					dlg.m_strAttachment.Right(dlg.m_strAttachment.GetLength() - (int)_tcslen(PWS_NEW_ATTACHMENT)));
		}

		PW_ENTRY *pBase = m_mgr.GetEntry(dwEntryIndex); ASSERT_ENTRY(pBase);
		memcpy(aUuid, pBase->uuid, 16);

		if(bDoBackup == TRUE)
		{
			if(m_mgr.BackupEntry(&pweBackup, &bNeedGroupUpdate) == FALSE)
				MessageBox(TRL("Failed to create backup of this entry!"), TRL("An error occured"), MB_ICONWARNING | MB_OK);

			m_mgr.MemFreeEntry(&pweBackup);
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
			PW_ENTRY *pUpdated = m_mgr.GetEntry(dwEntryIndex); ASSERT(pUpdated != NULL);
			_List_SetEntry(dwSelectedEntry, pUpdated, FALSE, &tNow);

			AdjustPwListMode();
			AdjustColumnWidths();
		}

		_SortListIfAutoSort();
		DWORD dwListEntry = _EntryUuidToListPos(aUuid);
		if(dwListEntry != DWORD_MAX)
		{
			m_cList.EnsureVisible((int)dwListEntry, FALSE);
			m_cList.SetItemState((int)dwListEntry, LVIS_SELECTED, LVIS_SELECTED);
		}

		if(nModifyLevel != 0) m_bModified = TRUE;
	}
	else
		_TouchEntry(GetSelectedEntry(), FALSE); // User had viewed it only

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

	DWORD dwInvGroup1, dwInvGroup2;
	DWORD dwIndex;
	DWORD dwSel;
	PW_ENTRY *p;
	BOOL b, bNeedGroupUpdate = FALSE;

	if(m_bFileOpen == FALSE) return;
	// if(m_dwLastNumSelectedItems == 0) return;

	m_bDisplayDialog = TRUE;

	if(_CALLPLUGINS(KPM_DELETE_ENTRY_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	CString str;
	str = TRL("This will remove all selected entries unrecoverably!");
	str += _T("\r\n\r\n");
	str += TRL("Are you sure you want to delete all selected entries?");
	int nRes = MessageBox(str, TRL("Delete Entries Confirmation"), MB_ICONQUESTION | MB_YESNO);
	if(nRes == IDNO) { m_bDisplayDialog = FALSE; return; }

	dwInvGroup1 = m_mgr.GetGroupId(PWS_BACKUPGROUP);
	dwInvGroup2 = m_mgr.GetGroupId(PWS_BACKUPGROUP_SRC);

	while(1)
	{
		dwSel = GetSelectedEntry();
		if(dwSel == DWORD_MAX) break;

		dwIndex = _ListSelToEntryIndex(dwSel);
		ASSERT(dwIndex != DWORD_MAX); if(dwIndex == DWORD_MAX) break;

		if(m_bBackupEntries != FALSE)
		{
			p = m_mgr.GetEntry(dwIndex);
			ASSERT(p != NULL); if(p == NULL) break;

			if((p->uGroupId != dwInvGroup1) && (p->uGroupId != dwInvGroup2))
			{
				m_mgr.UnlockEntryPassword(p);
				b = FALSE;
				m_mgr.BackupEntry(p, &b);
				bNeedGroupUpdate |= b;
				m_mgr.LockEntryPassword(p);
			}
		}

		VERIFY(m_mgr.DeleteEntry(dwIndex)); // Delete from password manager
		VERIFY(m_cList.DeleteItem((int)dwSel)); // Delete from GUI
	}

	if(bNeedGroupUpdate == TRUE)
	{
		_Groups_SaveView(TRUE);
		UpdateGroupList();
		_Groups_RestoreView();
	}

	m_cList.RedrawWindow();
	m_bModified = TRUE;

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

	m_pPwListTrackableMenu = (BCMenu *)m_pPwListMenu->GetSubMenu(0);
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

	m_pGroupListTrackableMenu = (BCMenu *)m_pGroupListMenu->GetSubMenu(0);
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

	m_pEntryViewTrackableMenu = (BCMenu *)m_pEntryViewMenu->GetSubMenu(0);
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

	if(_CALLPLUGINS(KPM_PWLIST_RCLICK, m_pPwListTrackableMenu, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	m_pPwListTrackableMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		pt.x, pt.y, AfxGetMainWnd());

	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnClickGroupList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NotifyUserActivity();

	POINT pt;

	UNREFERENCED_PARAMETER(pNMHDR);

	*pResult = 0;

	GetCursorPos(&pt);
	m_cGroups.ScreenToClient(&pt);
	HTREEITEM h = m_cGroups.HitTest(CPoint(pt));
	m_cGroups.SelectItem(h); // Select the item the user pointed to

	// Remove the search group because we cannot handle it like a normal group
	_Groups_SaveView(TRUE);
	DWORD dwCachedFirst = m_dwGroupsSaveFirstVisible;
	DWORD dwCachedSelected = (h != NULL) ? m_cGroups.GetItemData(h) : DWORD_MAX;
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
	if(_CALLPLUGINS(KPM_PW_COPY, 0, 0) == FALSE) { m_bDisplayDialog = FALSE; return; }
	m_bDisplayDialog = FALSE;

	DWORD dwIndex = _ListSelToEntryIndex();
	PW_ENTRY *p;

	ASSERT(dwIndex != DWORD_MAX); if(dwIndex == DWORD_MAX) return;

	p = m_mgr.GetEntry(dwIndex);
	ASSERT_ENTRY(p); if(p == NULL) return;

	if(m_nClipboardMethod == CM_TIMED)
	{
		m_mgr.UnlockEntryPassword(p);
		CopyStringToClipboard(p->pszPassword);
		m_mgr.LockEntryPassword(p);
		SetStatusTextEx(TRL("Field copied to clipboard."));

		m_nClipboardCountdown = (int)m_dwClipboardSecs;
	}
	else if(m_nClipboardMethod == CM_ENHSECURE)
	{
		m_nClipboardState = CLIPBOARD_DELAYED_PASSWORD;
		MakeClipboardDelayRender(m_hWnd, &m_hwndNextViewer);

		CString str;
		str = TRL("Field copied to clipboard."); str += _T(" ");
		str += TRL("Waiting for paste command.");
		SetStatusTextEx(str);
	}

	if(_tcscmp(p->pszTitle, PWS_TAN_ENTRY) == 0) // If it is a TAN entry, expire it
	{
		_GetCurrentPwTime(&p->tExpire);
		m_bModified = TRUE;
	}

	_TouchEntry(GetSelectedEntry(), FALSE);
	_UpdateToolBar();
}

void CPwSafeDlg::OnTimer(UINT nIDEvent)
{
	if((nIDEvent == APPWND_TIMER_ID_UPDATER))
	{
		if(m_bCachedToolBarUpdate)
		{
			_UpdateToolBar();
			m_bCachedToolBarUpdate = FALSE;
		}

		if(m_bCachedPwlistUpdate)
		{
			UpdatePasswordList();
			m_bCachedPwlistUpdate = FALSE;
		}
	}
	else if(nIDEvent == APPWND_TIMER_ID)
	{
		if(m_nClipboardCountdown != -1)
		{
			m_nClipboardCountdown--;

			if(m_nClipboardCountdown == -1)
			{
				SetStatusTextEx(TRL("Ready."));
				ClearClipboardIfOwner();
			}
			else if(m_nClipboardCountdown == 0)
			{
				SetStatusTextEx(TRL("Clipboard cleared."));
			}
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
				if(m_nLockCountdown != 0)
				{
					m_nLockCountdown--;
					if(m_nLockCountdown == 0)
					{
						if(m_bExitInsteadOfLockAT == FALSE)
						{
							OnFileLock();

							if((m_bLocked == TRUE) && (m_bShowWindow == TRUE) && (m_bMinimized == FALSE))
								ViewHideHandler();
						}
						else OnFileExit();
					}
				}
			}
		}

		if(m_nFileRelockCountdown != -1)
		{
			m_nFileRelockCountdown--;

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
	DWORD dwEntryIndex = _ListSelToEntryIndex();
	PW_ENTRY *p;

	*pResult = 0;

	if(dwEntryIndex == DWORD_MAX) return;

	p = m_mgr.GetEntry(dwEntryIndex);
	ASSERT_ENTRY(p); if(p == NULL) return;

	BOOL bIsTAN = (_tcscmp(p->pszTitle, PWS_TAN_ENTRY) != 0) ? FALSE : TRUE;

	switch(pNMListView->iSubItem)
	{
	case 0:
		if((bIsTAN == TRUE) && (m_dwPwListMode != LVS_REPORT)) OnPwlistCopyPw();
		else OnPwlistEdit();
		break;
	case 1:
		OnPwlistCopyUser();
		break;
	case 2:
		OnPwlistVisitUrl();
		break;
	case 3:
		OnPwlistCopyPw();
		break;
	case 4:
		strData = p->pszAdditional;
		strData = CsRemoveMeta(&strData);
		CopyStringToClipboard(strData);
		EraseCString(&strData);
		m_nClipboardCountdown = (int)m_dwClipboardSecs;
		SetStatusTextEx(TRL("Field copied to clipboard."));
		break;
	case 5:
		_PwTimeToString(p->tCreation, &strData);
		CopyStringToClipboard(strData);
		m_nClipboardCountdown = (int)m_dwClipboardSecs;
		SetStatusTextEx(TRL("Field copied to clipboard."));
		break;
	case 6:
		_PwTimeToString(p->tLastMod, &strData);
		CopyStringToClipboard(strData);
		m_nClipboardCountdown = (int)m_dwClipboardSecs;
		SetStatusTextEx(TRL("Field copied to clipboard."));
		break;
	case 7:
		_PwTimeToString(p->tLastAccess, &strData);
		CopyStringToClipboard(strData);
		m_nClipboardCountdown = (int)m_dwClipboardSecs;
		SetStatusTextEx(TRL("Field copied to clipboard."));
		break;
	case 8:
		_PwTimeToString(p->tExpire, &strData);
		CopyStringToClipboard(strData);
		m_nClipboardCountdown = (int)m_dwClipboardSecs;
		SetStatusTextEx(TRL("Field copied to clipboard."));
		break;
	case 9:
		_UuidToString(p->uuid, &strData);
		CopyStringToClipboard(strData);
		m_nClipboardCountdown = (int)m_dwClipboardSecs;
		SetStatusTextEx(TRL("Field copied to clipboard."));
		break;
	case 10:
		CopyStringToClipboard(p->pszBinaryDesc);
		m_nClipboardCountdown = (int)m_dwClipboardSecs;
		SetStatusTextEx(TRL("Field copied to clipboard."));
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

	POINT pt;

	UNREFERENCED_PARAMETER(pNMHDR);
	*pResult = 0;

	if(m_bFileOpen == FALSE) return;

	m_bDisplayDialog = TRUE;
	if(_CALLPLUGINS(KPM_GROUPLIST_RCLICK, pNMHDR, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	GetCursorPos(&pt);

	UINT uFlags;
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
	if(_CALLPLUGINS(KPM_USER_COPY, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }
	m_bDisplayDialog = FALSE;

	if(m_nClipboardMethod == CM_TIMED)
	{
		DWORD dwEntryIndex = _ListSelToEntryIndex();
		PW_ENTRY *p;

		ASSERT(dwEntryIndex != DWORD_MAX); if(dwEntryIndex == DWORD_MAX) return;

		p = m_mgr.GetEntry(dwEntryIndex);
		ASSERT_ENTRY(p); if(p == NULL) return;

		if(_tcscmp(p->pszTitle, PWS_TAN_ENTRY) == 0) // Is it a TAN entry?
		{
			// It is a TAN entry, so copy the password instead of the user name
			OnPwlistCopyPw();
			return;
		}

		CopyStringToClipboard(p->pszUserName);
		m_nClipboardCountdown = (int)m_dwClipboardSecs;
	}
	else if(m_nClipboardMethod == CM_ENHSECURE)
	{
		m_nClipboardState = CLIPBOARD_DELAYED_USERNAME;
		MakeClipboardDelayRender(m_hWnd, &m_hwndNextViewer);
	}

	SetStatusTextEx(TRL("Field copied to clipboard."));

	_TouchEntry(GetSelectedEntry(), FALSE);
	_UpdateToolBar();
}

void CPwSafeDlg::ParseAndOpenURLWithEntryInfo(LPCTSTR lpURL, PW_ENTRY *pEntry)
{
	if((pEntry != NULL) && (lpURL != NULL))
	{
		CString strURL = lpURL;
		BOOL bParseTwoTimes = FALSE;

		if(strURL.GetLength() != 0)
		{
			CString strOverride;
			strOverride = ExtractParameterFromString(pEntry->pszAdditional, _T("url-override:"), 0);
			if(strOverride.GetLength() != 0)
			{
				strURL = strOverride;
				bParseTwoTimes = TRUE;
			}
			else if(m_strURLOverride.GetLength() != 0)
			{
				if(_IsUnsafeAllowed() == TRUE)
				{
					BOOL bIsSpecialURL = FALSE;

					if(strURL.GetLength() >= 6)
					{
						CString strTemp = strURL.Left(6);
						strTemp.MakeLower();

						if(strTemp == _T("cmd://")) bIsSpecialURL = TRUE;
					}

					if(bIsSpecialURL == FALSE)
					{
						strURL = m_strURLOverride;
						bParseTwoTimes = TRUE;
					}
				}
			}

			m_mgr.UnlockEntryPassword(pEntry);
			ParseURL(&strURL, pEntry, FALSE);
			if(bParseTwoTimes == TRUE) ParseURL(&strURL, pEntry, FALSE);
			m_mgr.LockEntryPassword(pEntry);

			FixURL(&strURL);

			if(strURL.GetLength() != 0)
			{
				if(m_bUsePuttyForURLs == TRUE)
				{
					if(OpenUrlUsingPutty(strURL, pEntry->pszUserName) == FALSE)
						OpenUrlEx(strURL);
				}
				else OpenUrlEx(strURL);
			}
		}
	}
}

void CPwSafeDlg::OnPwlistVisitUrl()
{
	NotifyUserActivity();

	int i;
	UINT uState;
	PW_ENTRY *p;
	DWORD dwGroupId = GetSelectedGroupId();
	CString strURL;
	BOOL bLaunched = FALSE;

	ASSERT(dwGroupId != DWORD_MAX); if(dwGroupId == DWORD_MAX) return;

	m_bDisplayDialog = TRUE;
	if(_CALLPLUGINS(KPM_URL_VISIT, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }
	m_bDisplayDialog = FALSE;

	if(m_bCopyURLs == FALSE)
	{
		for(i = 0; i < m_cList.GetItemCount(); i++)
		{
			uState = m_cList.GetItemState(i, LVIS_SELECTED);
			if((uState & LVIS_SELECTED) > 0)
			{
				// p = m_mgr.GetEntryByGroup(dwGroupId, (DWORD)i);

				DWORD dwIndex = _ListSelToEntryIndex((DWORD)i);
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
		DWORD dwSelectedEntry = GetSelectedEntry();
		ASSERT(dwSelectedEntry != DWORD_MAX); if(dwSelectedEntry == DWORD_MAX) return;

		DWORD dwEntryIndex = _ListSelToEntryIndex(dwSelectedEntry);
		ASSERT(dwEntryIndex != DWORD_MAX); if(dwEntryIndex == DWORD_MAX) return;

		p = m_mgr.GetEntry(dwEntryIndex);
		ASSERT_ENTRY(p); if(p == NULL) return;

		strURL = p->pszURL;
		// FixURL(&strURL);
		m_mgr.UnlockEntryPassword(p);
		ParseURL(&strURL, p, FALSE);
		m_mgr.LockEntryPassword(p);

		if(strURL.GetLength() >= 6)
		{
			if(strURL.Left(6) == _T("cmd://"))
			{
				if(m_bUsePuttyForURLs == TRUE)
				{
					if(OpenUrlUsingPutty(strURL, p->pszUserName) == FALSE)
						OpenUrlEx(strURL);
				}
				else OpenUrlEx(strURL);

				bLaunched = TRUE;
			}
			else CopyStringToClipboard(strURL);
		}
		else CopyStringToClipboard(strURL);

		EraseCString(&strURL);

		_TouchEntry(dwSelectedEntry, FALSE);

		if(bLaunched == FALSE)
		{
			m_nClipboardCountdown = (int)m_dwClipboardSecs;
			SetStatusTextEx(TRL("Field copied to clipboard."));
		}
	}

	_UpdateToolBar();
}

void CPwSafeDlg::OnFileNew()
{
	NotifyUserActivity();

	if(m_bLocked == TRUE) return;

	if(m_bFileOpen == TRUE) OnFileClose();
	if(m_bFileOpen == TRUE)
	{
		// MessageBox(TRL("First close the open file before opening another one!"), TRL("Password Safe"),
		//	MB_OK | MB_ICONWARNING);
		return;
	}

	if(_CALLPLUGINS(KPM_FILE_NEW_PRE, 0, 0) == FALSE) return;

	m_mgr.NewDatabase();
	if(_ChangeMasterKey(NULL, TRUE) == FALSE) return;

	m_bFileOpen = TRUE;
	m_cList.EnableWindow(TRUE);
	m_cGroups.EnableWindow(TRUE);
	m_bModified = TRUE;

	PW_GROUP pwTemplate;
	PW_TIME tNow;
	ZeroMemory(&pwTemplate, sizeof(PW_GROUP));
	_GetCurrentPwTime(&tNow);
	pwTemplate.tCreation = tNow;
	m_mgr.GetNeverExpireTime(&pwTemplate.tExpire);
	pwTemplate.tLastAccess = tNow;
	pwTemplate.tLastMod = tNow;

	// Add standard groups
	pwTemplate.uImageId = 48; pwTemplate.pszGroupName = (TCHAR *)TRL("General");
	pwTemplate.usLevel = 0; pwTemplate.uGroupId = 0; // 0 = create new group ID
	pwTemplate.dwFlags |= PWGF_EXPANDED;
	VERIFY(m_mgr.AddGroup(&pwTemplate));
	pwTemplate.uImageId = 38; pwTemplate.pszGroupName = (TCHAR *)TRL("Windows");
	pwTemplate.usLevel = 1; pwTemplate.uGroupId = 0; // 0 = create new group ID
	pwTemplate.dwFlags = 0;
	VERIFY(m_mgr.AddGroup(&pwTemplate));
	pwTemplate.uImageId = 3; pwTemplate.pszGroupName = (TCHAR *)TRL("Network");
	pwTemplate.usLevel = 1; pwTemplate.uGroupId = 0; // 0 = create new group ID
	VERIFY(m_mgr.AddGroup(&pwTemplate));
	pwTemplate.uImageId = 1; pwTemplate.pszGroupName = (TCHAR *)TRL("Internet");
	pwTemplate.usLevel = 1; pwTemplate.uGroupId = 0; // 0 = create new group ID
	VERIFY(m_mgr.AddGroup(&pwTemplate));
	pwTemplate.uImageId = 19; pwTemplate.pszGroupName = (TCHAR *)TRL("eMail");
	pwTemplate.usLevel = 1; pwTemplate.uGroupId = 0; // 0 = create new group ID
	VERIFY(m_mgr.AddGroup(&pwTemplate));
	pwTemplate.uImageId = 37; pwTemplate.pszGroupName = (TCHAR *)TRL("Homebanking");
	pwTemplate.usLevel = 1; pwTemplate.uGroupId = 0; // 0 = create new group ID
	VERIFY(m_mgr.AddGroup(&pwTemplate));
	/* pwTemplate.uImageId = 37; pwTemplate.pszGroupName = (TCHAR *)TRL("Mobile devices");
	pwTemplate.usLevel = 0; pwTemplate.uGroupId = 0; // 0 = create new group ID
	VERIFY(m_mgr.AddGroup(&pwTemplate));
	pwTemplate.uImageId = 37; pwTemplate.pszGroupName = (TCHAR *)TRL("Credit cards");
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
	for(int ix = 0; ix < (32+32+32+3); ix++)
	{
		CString str;
		str.Format(_T("%d Group"), ix);
		pwTemplate.uImageId = (DWORD)rand() % 30;
		pwTemplate.pszGroupName = (TCHAR *)(LPCTSTR)str;
		pwTemplate.uGroupId = 0; // 0 = create new group
		pwTemplate.usLevel = (USHORT)(rand() % 5);
		VERIFY(m_mgr.AddGroup(&pwTemplate));
	}

	PW_TIME tExpired;
	tExpired.btDay = 1; tExpired.btHour = 1; tExpired.btMinute = 1;
	tExpired.btMonth = 1; tExpired.btSecond = 0; tExpired.shYear = 2000;
	for(int ir = 0; ir < 10; ir++)
	{
		pwT.pszTitle = _T("I am expired");
		pwT.uGroupId = m_mgr.GetGroupIdByIndex((DWORD)rand() % 6);
		pwT.uImageId = rand() % 30;
		pwT.tExpire = tExpired;
		ZeroMemory(pwT.uuid, 16);
		VERIFY(m_mgr.AddEntry(&pwT));
	}

	for(int iy = 0; iy < (1024+256+3); iy++)
	{
		CString str;
		str.Format(_T("Sample #%d"), iy);
		pwT.pszTitle = (TCHAR *)(LPCTSTR)str;
		pwT.uGroupId = m_mgr.GetGroupIdByIndex((DWORD)rand() % 8);
		pwT.uImageId = (DWORD)rand() % 30;
		m_mgr.GetNeverExpireTime(&pwT.tExpire);
		ZeroMemory(pwT.uuid, 16);
		VERIFY(m_mgr.AddEntry(&pwT));
	}
#endif

	m_mgr.FixGroupTree();
	UpdateGroupList();
	UpdatePasswordList();
	_UpdateToolBar();
	m_cGroups.SetFocus();

	_CALLPLUGINS(KPM_FILE_NEW_POST, 0, 0);
}

BOOL CPwSafeDlg::_ChangeMasterKey(CPwManager *pDbMgr, BOOL bCreateNew)
{
	CPasswordDlg dlg;
	int nConfirmAttempts;
	BOOL bSuccess;
	CPwManager *pMgr;
	DWORD dwOpFlags = 0;
	DWORD aOpParams[3];

	if(bCreateNew == TRUE) dwOpFlags |= 1;

	aOpParams[0] = 0; // Buffer for return value -- plugins should fill it
	aOpParams[1] = (DWORD)pDbMgr;
	aOpParams[2] = dwOpFlags;
	if(_CALLPLUGINS(KPM_CHANGE_MASTER_KEY_PRE, aOpParams, 0) == FALSE)
		return (aOpParams[0] == 0) ? FALSE : TRUE;

	if(pDbMgr == NULL) pMgr = &m_mgr;
	else pMgr = pDbMgr;

	m_bDisplayDialog = TRUE;

	dlg.m_bKeyMethod = PWM_KEYMETHOD_OR;
	dlg.m_bLoadMode = FALSE;
	dlg.m_bConfirm = FALSE;
	dlg.m_hWindowIcon = m_hIcon;
	dlg.m_bChanging = (bCreateNew == FALSE) ? TRUE : FALSE;

	if(dlg.DoModal() == IDCANCEL) { m_bDisplayDialog = FALSE; return FALSE; }

	if((dlg.m_bKeyFile == FALSE) || (dlg.m_bKeyMethod == PWM_KEYMETHOD_AND))
	{
		bSuccess = FALSE;

		for(nConfirmAttempts = 0; nConfirmAttempts < 3; nConfirmAttempts++)
		{
			CPasswordDlg *pDlg2 = new CPasswordDlg();
			ASSERT(pDlg2 != NULL); if(pDlg2 == NULL) continue;

			pDlg2->m_bKeyMethod = PWM_KEYMETHOD_OR;
			pDlg2->m_bLoadMode = FALSE;
			pDlg2->m_bConfirm = TRUE;
			pDlg2->m_hWindowIcon = m_hIcon;
			pDlg2->m_bChanging = (bCreateNew == TRUE) ? FALSE : TRUE;

			if(pDlg2->DoModal() == IDCANCEL)
			{
				ASSERT((pDlg2->m_lpKey == NULL) && (pDlg2->m_lpKey2 == NULL));
				SAFE_DELETE(pDlg2);
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
						TRL("Password Safe"), MB_ICONWARNING);
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
						TRL("Password Safe"), MB_ICONWARNING);
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

	int nErrCode;
	int nErrCode2;

	if(dlg.m_bKeyMethod == PWM_KEYMETHOD_OR)
		nErrCode = pMgr->SetMasterKey(dlg.m_lpKey, dlg.m_bKeyFile, NULL, &ri, FALSE);
	else
	{
		ASSERT(dlg.m_bKeyFile == TRUE);
		nErrCode = pMgr->SetMasterKey(dlg.m_lpKey, dlg.m_bKeyFile, dlg.m_lpKey2, &ri, FALSE);
	}

	if(nErrCode != PWE_SUCCESS)
	{
		if(dlg.m_bKeyFile == TRUE)
		{
			CString strMsg = TRL("A key-file already exists on this drive. Do you want to overwrite or reuse it?");
			strMsg += _T("\r\n\r\n");
			strMsg += TRL("Click [Yes] to overwrite the key-file."); strMsg += _T("\r\n");
			strMsg += TRL("Click [No] to reuse the selected key-file for this database."); strMsg += _T("\r\n");
			strMsg += TRL("Click [Cancel] to abort changing the key.");

			int nMsg = MessageBox(strMsg, TRL("Overwrite?"), MB_ICONQUESTION | MB_YESNOCANCEL);

			if(nMsg == IDYES)
			{
				if(dlg.m_bKeyMethod == PWM_KEYMETHOD_OR)
					nErrCode2 = pMgr->SetMasterKey(dlg.m_lpKey, dlg.m_bKeyFile, NULL, &ri, TRUE);
				else
					nErrCode2 = pMgr->SetMasterKey(dlg.m_lpKey, dlg.m_bKeyFile, dlg.m_lpKey2, &ri, TRUE);

				if(nErrCode2 != PWE_SUCCESS)
				{
					dlg.FreePasswords();
					MessageBox(PWM_FormatStaticError(nErrCode2, 0), TRL("Password Safe"),
						MB_OK | MB_ICONWARNING);
					m_bDisplayDialog = FALSE; return FALSE;
				}
			}
			else if(nMsg == IDNO)
			{
				if(dlg.m_bKeyMethod == PWM_KEYMETHOD_OR)
					nErrCode2 = pMgr->SetMasterKey(dlg.m_lpKey, dlg.m_bKeyFile, NULL, NULL, FALSE);
				else
					nErrCode2 = pMgr->SetMasterKey(dlg.m_lpKey, dlg.m_bKeyFile, dlg.m_lpKey2, NULL, FALSE);

				if(nErrCode2 != PWE_SUCCESS)
				{
					dlg.FreePasswords();
					MessageBox(PWM_FormatStaticError(nErrCode2, 0), TRL("Password Safe"), MB_OK | MB_ICONWARNING);
					m_bDisplayDialog = FALSE; return FALSE;
				}
			}
			else { dlg.FreePasswords(); m_bDisplayDialog = FALSE; return FALSE; }
		}
		else
		{
			dlg.FreePasswords();
			MessageBox(PWM_FormatStaticError(nErrCode, 0), TRL("Password Safe"), MB_OK | MB_ICONWARNING);
			m_bDisplayDialog = FALSE; return FALSE;
		}
	}

	if(pDbMgr == NULL) m_bModified = TRUE;
	dlg.FreePasswords();

	if(bCreateNew == FALSE)
	{
		CString str;
		str = TRL("Master key has been changed!");
		str += _T("\r\n\r\n");
		str += TRL("Save the database now in order to get the new key applied.");
		MessageBox(str, TRL("Success"), MB_ICONINFORMATION | MB_OK);
	}

	m_bDisplayDialog = FALSE;
	return TRUE;
}

// When pszFile == NULL a file selection dialog is displayed
void CPwSafeDlg::_OpenDatabase(CPwManager *pDbMgr, const TCHAR *pszFile, const TCHAR *pszPassword, const TCHAR *pszKeyFile, BOOL bOpenLocked, LPCTSTR lpPreSelectPath, BOOL bIgnoreCorrupted)
{
	NotifyUserActivity();

	CString strFile, strFilter, strText;
	DWORD dwFlags;
	int nRet = IDCANCEL, nOpenAttempts, nAllowedAttempts, nErr;
	const TCHAR *pSuffix = _T("");
	CPasswordDlg *pDlgPass = NULL;
	PWDB_REPAIR_INFO repairInfo;
	CPwManager *pMgr;
	DWORD dwOpFlags = 0;
	DWORD aOpParams[7];

	if(pDbMgr == NULL) pMgr = &m_mgr;
	else pMgr = pDbMgr;

	ZeroMemory(&repairInfo, sizeof(PWDB_REPAIR_INFO));

	if(bOpenLocked == TRUE) dwOpFlags |= 1;
	if(bIgnoreCorrupted == TRUE) dwOpFlags |= 2;
	aOpParams[0] = 0; // Currently unused
	aOpParams[1] = (DWORD)pDbMgr;
	aOpParams[2] = (DWORD)pszFile;
	aOpParams[3] = (DWORD)pszPassword;
	aOpParams[4] = (DWORD)pszKeyFile;
	aOpParams[5] = (DWORD)lpPreSelectPath;
	aOpParams[6] = dwOpFlags;
	if(_CALLPLUGINS(KPM_OPENDB_PRE, aOpParams, 0) == FALSE) return;

	strFilter = TRL("Password Safe files");
	strFilter += _T(" (*.kdb/*.pwd)|*.kdb;*.pwd|");
	strFilter += TRL("All files");
	strFilter += _T(" (*.*)|*.*||");

	dwFlags = OFN_LONGNAMES | OFN_EXTENSIONDIFFERENT;
	// OFN_EXPLORER = 0x00080000, OFN_ENABLESIZING = 0x00800000
	dwFlags |= 0x00080000 | 0x00800000;
	dwFlags |= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; // | OFN_HIDEREADONLY
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
			if(m_bFileOpen == TRUE)
			{
				// MessageBox(TRL("First close the open file before opening another one!"), TRL("Password Safe"),
				//	MB_OK | MB_ICONWARNING);
				return;
			}
		}

		if(pszFile == NULL) strFile = dlg.GetPathName();

		if(pDbMgr == NULL) m_strLastDb = strFile;

		if((bOpenLocked == TRUE) && (pDbMgr == NULL))
		{
			m_strFile = strFile;

			// m_strFileAbsolute = GetShortestAbsolutePath(m_strFile);
			const FullPathName database((LPCTSTR)m_strFile);
			m_strFileAbsolute = database.getFullPathName().c_str();            

			m_bLocked = TRUE;
			CString strExtended = TRL("&Unlock Workspace");
			strExtended += _T("\t");
			strExtended += _GetCmdAccelExt(_T("&Lock Workspace"));
			m_menu.SetMenuText(ID_FILE_LOCK, strExtended, MF_BYCOMMAND);
			SetStatusTextEx(TRL("Workspace locked"));
			m_btnTbLock.SetTooltipText(TRL("&Unlock Workspace"));
		}
		else
		{
			if((pszPassword != NULL) || (pszKeyFile != NULL)) nAllowedAttempts = 1;
			else nAllowedAttempts = 3;

			for(nOpenAttempts = 0; nOpenAttempts < nAllowedAttempts; nOpenAttempts++)
			{
				ASSERT(pDlgPass == NULL); if(pDlgPass != NULL) { pDlgPass->FreePasswords(); delete pDlgPass; }
				pDlgPass = new CPasswordDlg();
				ASSERT(pDlgPass != NULL); if(pDlgPass == NULL) continue;

				pDlgPass->m_bLoadMode = TRUE;
				pDlgPass->m_bConfirm = FALSE;
				pDlgPass->m_hWindowIcon = m_hIcon;
				pDlgPass->m_lpPreSelectPath = lpPreSelectPath;

				if(pszFile == NULL) pDlgPass->m_strDescriptiveName = dlg.GetFileName();
				else pDlgPass->m_strDescriptiveName = CsFileOnly(&strFile);

				if((pszPassword == NULL) && (pszKeyFile == NULL))
				{
					m_bDisplayDialog = TRUE;
					if(pDlgPass->DoModal() == IDCANCEL)
					{
						delete pDlgPass; pDlgPass = NULL;
						m_bDisplayDialog = FALSE;
						return;
					}
					m_bDisplayDialog = FALSE;
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

				if(pDlgPass->m_bKeyMethod == PWM_KEYMETHOD_OR)
					nErr = pMgr->SetMasterKey(pDlgPass->m_lpKey, pDlgPass->m_bKeyFile, NULL, NULL, FALSE);
				else
					nErr = pMgr->SetMasterKey(pDlgPass->m_lpKey, pDlgPass->m_bKeyFile, pDlgPass->m_lpKey2, NULL, FALSE);

				pDlgPass->FreePasswords(); delete pDlgPass; pDlgPass = NULL;

				if(nErr != PWE_SUCCESS)
				{
					MessageBox(PWM_FormatStaticError(nErr, 0), TRL("Password Safe"), MB_OK | MB_ICONWARNING);
					continue;
				}

				if(bIgnoreCorrupted == TRUE)
					nErr = pMgr->OpenDatabase(strFile, &repairInfo);
				else
					nErr = pMgr->OpenDatabase(strFile, NULL);

				if(bIgnoreCorrupted == TRUE)
				{
					CString strRepaired = PWM_FormatStaticError(nErr, PWFF_NO_INTRO);
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
					MessageBox(PWM_FormatStaticError(nErr, 0), TRL("Password Safe"),
						MB_ICONWARNING | MB_OK);
				}
				else if(pDbMgr == NULL)
				{
					m_bHashValid = SHA256_HashFile(strFile, (BYTE *)m_aHashOfFile);

					if(pszFile == NULL) m_bFileReadOnly = dlg.GetReadOnlyPref();
					else m_bFileReadOnly = FALSE;

					if(FileLock_IsLocked(strFile) == TRUE)
					{
						CString strMessage = TRL("The file you are trying to open is locked, i.e. someone else has currently opened the file and is editing it.");
						strMessage += _T("\r\n\r\n");
						strMessage += TRL("Open the file in read-only mode?");
						strMessage += _T("\r\n\r\n");
						strMessage += TRL("Click [Yes] to open the file in read-only mode.");
						strMessage += _T("\r\n");
						strMessage += TRL("Click [No] to open the file in normal writing mode.");

						int nOpenRet = MessageBox(strMessage, TRL("Open the file in read-only mode?"), MB_YESNO | MB_ICONQUESTION);

						if(nOpenRet == IDYES) m_bFileReadOnly = TRUE;
						else
						{
							m_bFileReadOnly = FALSE;

							FileLock_Lock(strFile, TRUE);
							m_nFileRelockCountdown = FL_TIME_RELOCK_AFTER * 60;
						}
					}
					else
					{
						FileLock_Lock(strFile, TRUE);
						m_nFileRelockCountdown = FL_TIME_RELOCK_AFTER * 60;
					}

					m_strFile = strFile;

					// m_strFileAbsolute = GetShortestAbsolutePath(m_strFile);
					const FullPathName database((LPCTSTR)m_strFile);
					m_strFileAbsolute = database.getFullPathName().c_str();

					m_bFileOpen = TRUE;

					m_bModified = FALSE;
					m_cList.EnableWindow(TRUE);
					m_cGroups.EnableWindow(TRUE);

					m_bLocked = FALSE;

					pSuffix = _GetCmdAccelExt(_T("&Lock Workspace"));
					strText = TRL("&Lock Workspace");
					if(_tcslen(pSuffix) != 0) { strText += _T("\t"); strText += pSuffix; }
					m_menu.SetMenuText(ID_FILE_LOCK, strText, MF_BYCOMMAND);
					m_btnTbLock.SetTooltipText(TRL("&Lock Workspace"));

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
						m_cList.EnsureVisible((int)dwEntryPos, FALSE);
					}

					dwEntryPos = _EntryUuidToListPos(pMgr->m_aLastSelectedEntryUuid);
					if(dwEntryPos != DWORD_MAX) m_cList.SetItemState((int)dwEntryPos, LVIS_SELECTED, LVIS_SELECTED);

					SetStatusTextEx(TRL("Ready."));

					_CALLPLUGINS(KPM_OPENDB_COMMITTED, (LPARAM)pMgr->GetLastDatabaseHeader(), 0);
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
			m_systray.SetIcon(m_hTrayIconNormal);

			if((m_bAutoShowExpired == TRUE) || (m_bAutoShowExpiredSoon == TRUE))
				_ShowExpiredEntries(FALSE, m_bAutoShowExpired, m_bAutoShowExpiredSoon);

			_CALLPLUGINS(KPM_OPENDB_POST, 0, 0);
		}
		else m_systray.SetIcon(m_hTrayIconLocked);

		_UpdateToolBar(TRUE); // Updates titlebar, too
	}
	else { _CALLPLUGINS(KPM_OPENDB_POST, 1, 0); }

	NotifyUserActivity();
	m_cList.SetFocus();
}

void CPwSafeDlg::OnFileOpen()
{
	NotifyUserActivity();

	if(m_bLocked == TRUE) return;

	m_bDisplayDialog = TRUE;

	if(_CALLPLUGINS(KPM_FILE_OPEN_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	_OpenDatabase(NULL, NULL, NULL, NULL, FALSE, NULL, FALSE);
	_UpdateToolBar();
	m_cGroups.SetFocus();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::_UpdateGuiToManager()
{
	m_mgr.m_dwLastSelectedGroupId = GetSelectedGroupId();
	m_mgr.m_dwLastTopVisibleGroupId = m_cGroups.GetItemData(m_cGroups.GetFirstVisibleItem());

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
	if((idGroup != 0) && (idGroup != DWORD_MAX)) { VERIFY(m_mgr.DeleteGroupById(idGroup)); bUpd = TRUE; }

	idGroup = m_mgr.GetGroupId(PWS_BACKUPGROUP_SRC);
	if((idGroup != 0) && (idGroup != DWORD_MAX)) { VERIFY(m_mgr.DeleteGroupById(idGroup)); bUpd = TRUE; }

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

	if(_CALLPLUGINS(KPM_FILE_SAVE_PRE, 0, 0) == FALSE)
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

	int nErr = m_mgr.SaveDatabase(m_strFile);
	if(nErr != PWE_SUCCESS)
	{
		MessageBox(PWM_FormatStaticError(nErr, 0), TRL("Password Safe"), MB_ICONWARNING | MB_OK);
		m_bDisplayDialog = FALSE;
		return;
	}

	_CALLPLUGINS(KPM_SAVEDB_POST, (LPCTSTR)m_strFile, 0);

	// Update file contents hash
	m_bHashValid = SHA256_HashFile(m_strFile, (BYTE *)m_aHashOfFile);

	m_strLastDb = m_strFile;
	m_bModified = FALSE;

	_UpdateToolBar(); // Updates titlebar, too
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnFileSaveAs()
{
	NotifyUserActivity();

	CString strFile;
	DWORD dwFlags;
	CString strFilter;

	if(m_bFileOpen == FALSE) return;

	m_bDisplayDialog = TRUE;

	if(_CALLPLUGINS(KPM_FILE_SAVEAS_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	GroupSyncStates(TRUE);

	strFilter = TRL("Password Safe files");
	strFilter += _T(" (*.kdb)|*.kdb|");
	strFilter += TRL("All files");
	strFilter += _T(" (*.*)|*.*||");

	dwFlags = OFN_LONGNAMES | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	dwFlags |= OFN_EXTENSIONDIFFERENT;
	// OFN_EXPLORER = 0x00080000, OFN_ENABLESIZING = 0x00800000
	dwFlags |= 0x00080000 | 0x00800000 | OFN_NOREADONLYRETURN;
	CFileDialog dlg(FALSE, _T("kdb"), _T("Database.kdb"), dwFlags, strFilter, this);

	if(dlg.DoModal() == IDOK)
	{
		// strFile = dlg.GetPathName();
		const FullPathName database((LPCTSTR)dlg.GetPathName());
		strFile = database.getFullPathName().c_str();

		if(m_bDeleteBackupsOnSave == TRUE) _DeleteBackupEntries();
		_RemoveSearchGroup();
		_UpdateGuiToManager();

		int nErr = m_mgr.SaveDatabase(strFile);
		if(nErr != PWE_SUCCESS)
		{
			MessageBox(PWM_FormatStaticError(nErr, 0), TRL("Password Safe"),
				MB_ICONWARNING | MB_OK);
		}
		else
		{
			_CALLPLUGINS(KPM_SAVEDB_AS_POST, (LPCTSTR)strFile, 0);

			if(m_bFileReadOnly == FALSE)
			{
				m_nFileRelockCountdown = -1;
				FileLock_Lock(m_strFile, FALSE); // Unlock the previous database file
			}

			// Update file contents hash
			m_bHashValid = SHA256_HashFile(strFile, (BYTE *)m_aHashOfFile);

			m_strFile = strFile;

			FileLock_Lock(m_strFile, TRUE);
			m_nFileRelockCountdown = FL_TIME_RELOCK_AFTER * 60;

			// m_strFileAbsolute = GetShortestAbsolutePath(m_strFile);
			m_strFileAbsolute = m_strFile;

			m_bModified = FALSE;
			m_strLastDb = strFile;

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

	int nRes;

	m_bDisplayDialog = TRUE;

	if(_CALLPLUGINS(KPM_FILE_CLOSE_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	GroupSyncStates(TRUE);

	if((m_bFileOpen == TRUE) && (m_bModified == TRUE))
	{
		if((m_bExiting == TRUE) && (m_bAutoSaveDb == TRUE))
		{
			nRes = IDYES;
		}
		else
		{
			CString str;

			str = TRL("The current file has been modified.");
			str += _T("\r\n\r\n");
			str += TRL("Do you want to save the changes before closing?");
			SetForegroundWindow();
			nRes = MessageBox(str,
				TRL("KeePass - Save Before Close/Lock?"), MB_YESNOCANCEL | MB_ICONQUESTION);
		}

		if(nRes == IDCANCEL) { m_bDisplayDialog = FALSE; return; }
		else if(nRes == IDYES)
		{
			OnFileSave();

			if((m_bModified == TRUE) && (m_bExiting == TRUE))
			{
				CString strMsg;

				strMsg = TRL("The file couldn't be saved.");
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

	m_cQuickFind.SetWindowText(_T(""));

	ClearClipboardIfOwner();

	if(m_bFileReadOnly == FALSE)
	{
		m_nFileRelockCountdown = -1;
		FileLock_Lock(m_strFile, FALSE); // Unlock the database file
	}

	m_cList.DeleteAllItems();
	m_cGroups.DeleteAllItems();
	ShowEntryDetails(NULL);
	m_mgr.NewDatabase();

	m_strFile.Empty(); m_strFileAbsolute.Empty();
	m_bFileOpen = FALSE;
	m_bFileReadOnly = FALSE;
	m_bModified = FALSE;
	m_cList.EnableWindow(FALSE);
	m_cGroups.EnableWindow(FALSE);

	_DeleteTemporaryFiles();

	m_systray.SetIcon(m_hTrayIconNormal);
	// _UpdateTitleBar(); // Updated by _UpdateToolBar()
	_UpdateToolBar();

	_CALLPLUGINS(KPM_FILE_CLOSE_POST, 0, 0);
	m_bDisplayDialog = FALSE;
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
	dlg.m_bMinimizeToTray = m_bMinimizeToTray;
	dlg.m_bCloseMinimizes = m_bCloseMinimizes;
	dlg.m_bLockAfterTime = (m_nLockTimeDef != -1) ? TRUE : FALSE;

	if(m_nLockTimeDef != -1) dlg.m_nLockAfter = (UINT)m_nLockTimeDef;
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
	dlg.m_bAutoPwGen = m_bAutoPwGen;
	dlg.m_bQuickFindIncBackup = m_bQuickFindIncBackup;
	dlg.m_bMinimizeBeforeAT = (m_nAutoTypeMethod == ATM_MINIMIZE) ? TRUE : FALSE;
	dlg.m_bDeleteBackupsOnSave = m_bDeleteBackupsOnSave;
	dlg.m_bShowFullPath = m_bShowFullPath;
	dlg.m_bDisableAutoType = m_bDisableAutoType;
	dlg.m_bCopyURLs = m_bCopyURLs;
	dlg.m_bExitInsteadOfLockAT = m_bExitInsteadOfLockAT;
	dlg.m_bAllowSaveIfModifiedOnly = m_bAllowSaveIfModifiedOnly;
	dlg.m_bCheckForUpdate = m_bCheckForUpdate;

	if(_CALLPLUGINS(KPM_OPTIONS_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	if(dlg.DoModal() == IDOK)
	{
		m_bWindowsNewLine = (dlg.m_nNewlineSequence == 0) ? TRUE : FALSE;
		m_dwClipboardSecs = dlg.m_uClipboardSeconds + 1;
		m_bOpenLastDb = dlg.m_bOpenLastDb;
		m_bStartMinimized = dlg.m_bStartMinimized;
		m_bImgButtons = dlg.m_bImgButtons;
		m_bEntryGrid = dlg.m_bEntryGrid;
		m_bAutoSaveDb = dlg.m_bAutoSave;
		m_bLockOnMinimize = dlg.m_bLockOnMinimize;
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
		m_bAutoPwGen = dlg.m_bAutoPwGen;
		m_bQuickFindIncBackup = dlg.m_bQuickFindIncBackup;
		m_bDeleteBackupsOnSave = dlg.m_bDeleteBackupsOnSave;
		m_bShowFullPath = dlg.m_bShowFullPath;
		m_bDisableAutoType = dlg.m_bDisableAutoType;
		m_bExitInsteadOfLockAT = dlg.m_bExitInsteadOfLockAT;
		m_bAllowSaveIfModifiedOnly = dlg.m_bAllowSaveIfModifiedOnly;
		m_bCheckForUpdate = dlg.m_bCheckForUpdate;

		m_nAutoTypeMethod = (dlg.m_bMinimizeBeforeAT == TRUE) ? ATM_MINIMIZE : ATM_DROPBACK;

		if(dlg.m_bDefaultExpire == FALSE) m_dwDefaultExpire = 0;
		else m_dwDefaultExpire = dlg.m_dwDefaultExpire;

		if(dlg.m_bStartWithWindows != bStartWithWindows)
		{
			if(CPwSafeApp::SetStartWithWindows(dlg.m_bStartWithWindows) == FALSE)
			{
				MessageBox(TRL("Unable to create/remove the auto-start key of KeePass in the Windows registry!"), TRL("Stop"), MB_ICONWARNING | MB_OK);
			}
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

		if(dlg.m_bLockAfterTime == TRUE) m_nLockTimeDef = (long)dlg.m_nLockAfter;
		else m_nLockTimeDef = -1;

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

		_CALLPLUGINS(KPM_OPTIONS_POST, 0, 0);

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
	std::string strFile = Executable::instance().getFullPathName();

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
	int nRes;

	ASSERT(dwGroupId != DWORD_MAX); if(dwGroupId == DWORD_MAX) return;

	m_bDisplayDialog = TRUE;

	DWORD dwGroupIndex = m_mgr.GetGroupByIdN(dwGroupId);
	PW_GROUP *p = m_mgr.GetGroup(dwGroupIndex);
	ASSERT(p != NULL); if(p == NULL) return;
	USHORT usTopLevel = p->usLevel;

	while(1)
	{
		aGroupIds.Add(dwGroupId);

		dwGroupIndex++;
		if(dwGroupIndex == m_mgr.GetNumberOfGroups()) break;

		p = m_mgr.GetGroup(dwGroupIndex);
		ASSERT(p != NULL); if(p == NULL) break;

		if(p->usLevel <= usTopLevel) break;

		dwGroupId = p->uGroupId;
	}

	if(_CALLPLUGINS(KPM_GROUP_REMOVE_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	CString str;
	str = TRL("Deleting a group will delete all items and subgroups in that group, too.");
	str += _T("\r\n\r\n");
	str += TRL("Are you sure you want to delete this group?");
	nRes = MessageBox(str, TRL("Delete Group Confirmation"), MB_ICONQUESTION | MB_YESNO);

	if(nRes == IDYES)
	{
		int i, nCount = aGroupIds.GetSize();
		for(i = 0; i < nCount; i++)
		{
			VERIFY(m_mgr.DeleteGroupById(aGroupIds.GetAt(nCount - i - 1)));
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

	if(_CALLPLUGINS(KPM_FILE_CHANGE_MASTER_KEY_PRE, 0, 0) == FALSE) return;

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
	pCmdUI->Enable(((m_dwLastFirstSelectedItem != DWORD_MAX) && (m_dwLastNumSelectedItems == 1)) ? TRUE : FALSE);
}

void CPwSafeDlg::OnUpdatePwlistCopyUser(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(((m_dwLastFirstSelectedItem != DWORD_MAX) && (m_dwLastNumSelectedItems == 1)) ? TRUE : FALSE);
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
	if(m_bCopyURLs == FALSE)
		pCmdUI->Enable((m_dwLastFirstSelectedItem != DWORD_MAX) && (m_dwLastNumSelectedItems >= 1 ? TRUE : FALSE));
	else
		pCmdUI->Enable((m_dwLastFirstSelectedItem != DWORD_MAX) && (m_dwLastNumSelectedItems == 1 ? TRUE : FALSE));
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
	pCmdUI->Enable(m_bFileOpen);
}

#define LCL_CHKOPT_PARAM_COUNT 19

BOOL CPwSafeDlg::GetExportOptions(PWEXPORT_OPTIONS *pOptions, CPwExport *pPwExport)
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

	pa[++n].lpString = TRL("Encode/replace newline characters by '\\n'");
	pa[n].pbValue = &pOptions->bEncodeNewlines;
	pa[++n].lpString = TRL("Export backup entries (entries in the 'Backup' group)");
	pa[n].pbValue = &pOptions->bExportBackups;

	pa[++n].lpString = _T("");
	pa[n].nIcon = 0;

	pa[++n].lpString = TRL("Fields to export");
	pa[n].nIcon = 8;

	pa[++n].lpString = TRL("Password Groups");
	pa[n].pbValue = &pOptions->bGroup;
	pa[++n].lpString = TRL("Group Tree");
	pa[n].pbValue = &pOptions->bGroupTree;
	pa[++n].lpString = TRL("Title");
	pa[n].pbValue = &pOptions->bTitle;
	pa[++n].lpString = TRL("User Name");
	pa[n].pbValue = &pOptions->bUserName;
	pa[++n].lpString = TRL("URL");
	pa[n].pbValue = &pOptions->bURL;
	pa[++n].lpString = TRL("Password");
	pa[n].pbValue = &pOptions->bPassword;
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

	ASSERT(n == (LCL_CHKOPT_PARAM_COUNT - 1));

	dlg.m_strTitle = TRL("Export Options");
	dlg.m_strDescription = TRL("Here you can customize the exported files.");
	dlg.m_dwNumParams = LCL_CHKOPT_PARAM_COUNT;
	dlg.m_pParams = (CHKOPT_PARAM *)pa;

	if(dlg.DoModal() == IDOK) { m_bDisplayDialog = FALSE; return TRUE; }
	m_bDisplayDialog = FALSE;
	return FALSE;
}

CString CPwSafeDlg::GetExportFile(int nFormat, LPCTSTR lpBaseFileName, BOOL bFixFileName)
{
	DWORD dwFlags;
	LPTSTR lp = _T("exp");
	CString strSample;
	CString strFilter;

	if(m_bFileOpen == FALSE) return CString("");

	m_bDisplayDialog = TRUE;

	if(nFormat == PWEXP_TXT) lp = _T("txt");
	else if(nFormat == PWEXP_HTML) lp = _T("html");
	else if(nFormat == PWEXP_XML) lp = _T("xml");
	else if(nFormat == PWEXP_CSV) lp = _T("csv");
	else if(nFormat == PWEXP_KEEPASS) lp = _T("kdb");
	else { ASSERT(FALSE); }

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

	strFilter = TRL("All files");
	strFilter += _T(" (*.*)|*.*||");

	dwFlags = OFN_LONGNAMES | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	dwFlags |= OFN_EXTENSIONDIFFERENT;
	// OFN_EXPLORER = 0x00080000, OFN_ENABLESIZING = 0x00800000
	dwFlags |= 0x00080000 | 0x00800000 | OFN_NOREADONLYRETURN;
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
	if(_IsUnsafeAllowed() == FALSE) return;
	cExp.SetManager(&m_mgr);
	cExp.SetNewLineSeq(m_bWindowsNewLine);
	cExp.SetFormat(PWEXP_TXT);
	strFile = GetExportFile(PWEXP_TXT, CsFileOnly(&m_strFile), FALSE);
	if(strFile.GetLength() != 0)
		if(GetExportOptions(&pwo, &cExp) == TRUE)
			cExp.ExportAll(strFile, &pwo, NULL);
}

void CPwSafeDlg::OnExportHtml()
{
	NotifyUserActivity();
	CPwExport cExp; PWEXPORT_OPTIONS pwo;
	CString strFile;
	if(m_bFileOpen == FALSE) return;
	if(_IsUnsafeAllowed() == FALSE) return;
	cExp.SetManager(&m_mgr);
	cExp.SetNewLineSeq(m_bWindowsNewLine);
	cExp.SetFormat(PWEXP_HTML);
	strFile = GetExportFile(PWEXP_HTML, CsFileOnly(&m_strFile), FALSE);
	if(strFile.GetLength() != 0)
		if(GetExportOptions(&pwo, &cExp) == TRUE)
			cExp.ExportAll(strFile, &pwo, NULL);
}

void CPwSafeDlg::OnExportXml()
{
	NotifyUserActivity();
	CPwExport cExp; PWEXPORT_OPTIONS pwo;
	CString strFile;
	if(m_bFileOpen == FALSE) return;
	if(_IsUnsafeAllowed() == FALSE) return;
	cExp.SetManager(&m_mgr);
	cExp.SetNewLineSeq(m_bWindowsNewLine);
	cExp.SetFormat(PWEXP_XML);
	strFile = GetExportFile(PWEXP_XML, CsFileOnly(&m_strFile), FALSE);
	if(strFile.GetLength() != 0)
		if(GetExportOptions(&pwo, &cExp) == TRUE)
			cExp.ExportAll(strFile, &pwo, NULL);
}

void CPwSafeDlg::OnExportCsv()
{
	NotifyUserActivity();
	CPwExport cExp; PWEXPORT_OPTIONS pwo;
	CString strFile;
	if(m_bFileOpen == FALSE) return;
	if(_IsUnsafeAllowed() == FALSE) return;
	cExp.SetManager(&m_mgr);
	cExp.SetNewLineSeq(m_bWindowsNewLine);
	cExp.SetFormat(PWEXP_CSV);
	strFile = GetExportFile(PWEXP_CSV, CsFileOnly(&m_strFile), FALSE);
	if(strFile.GetLength() != 0)
		if(GetExportOptions(&pwo, &cExp) == TRUE)
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

	CPwExport cExp; PWEXPORT_OPTIONS pwo;
	TCHAR szFile[MAX_PATH * 2];

	_DeleteTemporaryFiles();

	if(m_bFileOpen == FALSE) return;
	if(_IsUnsafeAllowed() == FALSE) return;

	cExp.SetManager(&m_mgr);
	cExp.SetNewLineSeq(m_bWindowsNewLine);
	cExp.SetFormat(PWEXP_HTML);

	GetTempPath(MAX_PATH * 2 - 2, szFile);
	if(szFile[_tcslen(szFile) - 1] != _T('\\')) _tcscat_s(szFile, _countof(szFile), _T("\\"));
	_tcscat_s(szFile, _countof(szFile), _T("pwsafetmp.html"));

	if(GetExportOptions(&pwo, &cExp) == FALSE) return;

	BOOL bRet;
	bRet = cExp.ExportGroup(szFile, dwGroupId, &pwo, NULL);

	if(bRet == FALSE)
	{
		MessageBox(TRL("Cannot open temporary file for printing!"), TRL("Stop"),
			MB_OK | MB_ICONWARNING);
		return;
	}

	ShellExecute(m_hWnd, _T("print"), szFile, NULL, NULL, SW_SHOW);

	m_strTempFile = szFile;
}

void CPwSafeDlg::OnFilePrint()
{
	NotifyUserActivity();
	if(_CALLPLUGINS(KPM_FILE_PRINT_PRE, 0, 0) == FALSE) return;
	_PrintGroup(DWORD_MAX); // _PrintGroup handles everything, like unsafe check, etc.
}

void CPwSafeDlg::OnUpdateFilePrint(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnExtrasGenPw()
{
	NotifyUserActivity();

	CPwGeneratorDlg dlg;
	DWORD dwGroupId = GetSelectedGroupId();
	BYTE aUuid[16];

	m_bDisplayDialog = TRUE;

	dlg.m_bHidePw = m_bPasswordStars;

	if(_CALLPLUGINS(KPM_GEN_PASSWORD_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	if((m_bFileOpen == FALSE) || (dwGroupId == DWORD_MAX))
	{
		dlg.m_bCanAccept = FALSE;
		dlg.DoModal();
	}
	else // m_bFileOpen == TRUE
	{
		dlg.m_bCanAccept = TRUE;

		if(dlg.DoModal() == IDOK)
		{
			PW_TIME tNow;
			PW_ENTRY pwTemplate;

			_GetCurrentPwTime(&tNow);
			memset(&pwTemplate, 0, sizeof(PW_ENTRY));
			pwTemplate.pszAdditional = _T("");
			pwTemplate.pszPassword = dlg.m_lpPassword;
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

				m_cList.EnsureVisible((int)dwListIndex, FALSE);
				m_cList.SetItemState((int)dwListIndex, LVIS_SELECTED, LVIS_SELECTED);
			}

			CSecureEditEx::DeletePassword(dlg.m_lpPassword); dlg.m_lpPassword = NULL;
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

	if(_CALLPLUGINS(KPM_GROUP_MODIFY_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	if(dlg.DoModal() == IDOK)
	{
		_GetCurrentPwTime(&tNow);
		pwTemplate = *p; // Copy previous standard values like group ID, etc.
		pwTemplate.tLastAccess = tNow; // Update times
		pwTemplate.tLastMod = tNow;
		pwTemplate.pszGroupName = (TCHAR *)(LPCTSTR)dlg.m_strGroupName;
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
	pCmdUI->Enable(((m_hLastSelectedGroup != NULL) ? TRUE : FALSE));
}

void CPwSafeDlg::OnPwlistFind()
{
	NotifyUserActivity();
	if(_CALLPLUGINS(KPM_PWLIST_FIND_PRE, 0, 0) == FALSE) return;
	_Find(DWORD_MAX);
}

void CPwSafeDlg::_Find(DWORD dwFindGroupId)
{
	CFindInDbDlg dlg;
	DWORD dwMaxItems;
	PW_ENTRY *p;
	CString strTemp;
	PW_TIME tNow;
	DWORD dwFindGroupIndex;

	if(m_bFileOpen == FALSE) return;

	dwMaxItems = m_mgr.GetNumberOfEntries();
	if(dwMaxItems == 0) return; // Nothing to search for

	m_bDisplayDialog = TRUE;

	if(dwFindGroupId != DWORD_MAX) dwFindGroupIndex = m_mgr.GetGroupByIdN(dwFindGroupId);
	else dwFindGroupIndex = DWORD_MAX;

	if(dwFindGroupIndex != DWORD_MAX)
	{
		PW_GROUP *pg = m_mgr.GetGroup(dwFindGroupIndex);
		if(pg != NULL) dlg.m_lpGroupName = pg->pszGroupName;
	}

	if(dlg.DoModal() == IDOK)
	{
		DWORD dwFlags = 0;

		m_cList.DeleteAllItems();

		_GetCurrentPwTime(&tNow);

		if(dlg.m_bTitle == TRUE)      dwFlags |= PWMF_TITLE;
		if(dlg.m_bUserName == TRUE)   dwFlags |= PWMF_USER;
		if(dlg.m_bURL == TRUE)        dwFlags |= PWMF_URL;
		if(dlg.m_bPassword == TRUE)   dwFlags |= PWMF_PASSWORD;
		if(dlg.m_bAdditional == TRUE) dwFlags |= PWMF_ADDITIONAL;
		if(dlg.m_bGroupName == TRUE)  dwFlags |= PWMF_GROUPNAME;

		DWORD dw = 0;
		DWORD cnt = 0;
		DWORD dwGroupId;
		DWORD dwGroupInx;

		// Create the search group if it doesn't exist already
		dwGroupId = m_mgr.GetGroupId(PWS_SEARCHGROUP);
		if(dwGroupId == DWORD_MAX)
		{
			PW_GROUP pwTemplate;
			pwTemplate.pszGroupName = (TCHAR *)PWS_SEARCHGROUP;
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

		while(1)
		{
			dw = m_mgr.Find(dlg.m_strFind, dlg.m_bCaseSensitive, dwFlags, cnt);

			if(dw == DWORD_MAX) break;
			else
			{
				p = m_mgr.GetEntry(dw);
				ASSERT_ENTRY(p);
				if(p == NULL) break;

				if(p->uGroupId != dwGroupId)
				{
					dwGroupInx = m_mgr.GetGroupByIdN(p->uGroupId);
					ASSERT(dwGroupInx != DWORD_MAX);

					if((dwFindGroupIndex == DWORD_MAX) || (dwFindGroupIndex == dwGroupInx))
					{
						if(dlg.m_bExcludeBackups == TRUE)
						{
							if(_tcscmp(m_mgr.GetGroup(dwGroupInx)->pszGroupName, PWS_BACKUPGROUP) == 0)
							{
								cnt = dw + 1;
								if((DWORD)cnt >= dwMaxItems) break;
								continue;
							}
							if(_tcscmp(m_mgr.GetGroup(dwGroupInx)->pszGroupName, PWS_BACKUPGROUP_SRC) == 0)
							{
								cnt = dw + 1;
								if((DWORD)cnt >= dwMaxItems) break;
								continue;
							}
						}

						// The entry could get reallocated by AddEntry, therefor
						// save it to a local CString object
						m_mgr.UnlockEntryPassword(p);
						strTemp = p->pszPassword;
						m_mgr.LockEntryPassword(p);

						_List_SetEntry(m_cList.GetItemCount(), p, TRUE, &tNow);

						EraseCString(&strTemp); // Destroy the plaintext password
					}
				}
			}

			cnt = dw + 1;
			if((DWORD)cnt >= dwMaxItems) break;
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

		// m_bModified = TRUE;
		ShowEntryDetails(NULL);
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
	if(_CALLPLUGINS(KPM_PWLIST_FIND_IN_GROUP_PRE, 0, 0) == FALSE) return;
	DWORD dwCurGroup = GetSelectedGroupId();
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

	DWORD dwGroupId = m_cGroups.GetItemData(m_hLastSelectedGroup);
	DWORD dwRefId = m_mgr.GetGroupId(PWS_SEARCHGROUP);
	BOOL bEnable = (dwGroupId != dwRefId) ? TRUE : FALSE;
	pCmdUI->Enable(bEnable && (m_mgr.GetNumberOfEntries() != 0));
}

void CPwSafeDlg::OnPwlistDuplicate()
{
	NotifyUserActivity();

	PW_ENTRY *p;
	PW_ENTRY pwTemplate;
	PW_TIME tNow;
	int nItemCount = m_cList.GetItemCount();
	int i;
	DWORD dwEntryIndex;
	UINT uState;
	BYTE aUuid[16];

	if(_CALLPLUGINS(KPM_DUPLICATE_ENTRY_PRE, 0, 0) == FALSE) return;

	_GetCurrentPwTime(&tNow);

	for(i = 0; i < nItemCount; i++)
	{
		uState = m_cList.GetItemState(i, UINT_MAX);

		if((uState & LVIS_SELECTED) != 0)
		{
			dwEntryIndex = _ListSelToEntryIndex((DWORD)i);
			ASSERT(dwEntryIndex != DWORD_MAX); if(dwEntryIndex == DWORD_MAX) continue;

			p = m_mgr.GetEntry(dwEntryIndex);
			ASSERT_ENTRY(p); if(p == NULL) continue;

			pwTemplate = *p; // Duplicate

			memset(pwTemplate.uuid, 0, 16); // We need a new UUID
			pwTemplate.tCreation = tNow; // Set new times
			pwTemplate.tLastMod = tNow;
			pwTemplate.tLastAccess = tNow;

			m_mgr.UnlockEntryPassword(&pwTemplate);
			VERIFY(m_mgr.AddEntry(&pwTemplate));
			m_mgr.LockEntryPassword(&pwTemplate);

			PW_ENTRY *pNew = m_mgr.GetLastEditedEntry(); ASSERT_ENTRY(pNew);
			memcpy(aUuid, pNew->uuid, 16);
		}
	}

	m_bModified = TRUE;

	_SortListIfAutoSort();
	if(m_nAutoSort == 0) UpdatePasswordList();

	DWORD dwListItem = _EntryUuidToListPos(aUuid);
	if(dwListItem != DWORD_MAX)
	{
		m_cList.EnsureVisible((int)dwListItem, FALSE);
		m_cList.SetItemState((int)dwListItem, LVIS_SELECTED, LVIS_SELECTED);
	}

	_UpdateToolBar();
}

void CPwSafeDlg::OnUpdatePwlistDuplicate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(((m_dwLastFirstSelectedItem != DWORD_MAX) ? TRUE : FALSE));
}

void CPwSafeDlg::OnInfoHomepage()
{
	NotifyUserActivity();
	ShellExecute(NULL, _T("open"), PWM_HOMEPAGE, NULL, NULL, SW_SHOW);
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
	if(_IsUnsafeAllowed() == FALSE) return;

	cExp.SetManager(&m_mgr);
	cExp.SetNewLineSeq(m_bWindowsNewLine);
	cExp.SetFormat(nFormat);
	strFile = GetExportFile(nFormat, m_mgr.GetGroupById(dwSelectedGroup)->pszGroupName, TRUE);
	if(strFile.GetLength() != 0)
		if(GetExportOptions(&pwo, &cExp) == TRUE)
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
	pCmdUI->Enable(((m_hLastSelectedGroup != NULL) ? TRUE : FALSE));
}

void CPwSafeDlg::OnUpdateSafeExportGroupXml(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(((m_hLastSelectedGroup != NULL) ? TRUE : FALSE));
}

void CPwSafeDlg::OnUpdateSafeExportGroupCsv(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(((m_hLastSelectedGroup != NULL) ? TRUE : FALSE));
}

void CPwSafeDlg::OnSafePrintGroup()
{
	NotifyUserActivity();

	DWORD dwGroup = GetSelectedGroupId();
	if(dwGroup == DWORD_MAX) return;

	if(_CALLPLUGINS(KPM_GROUP_PRINT_PRE, dwGroup, 0) == FALSE) return;

	_PrintGroup(dwGroup);
}

void CPwSafeDlg::OnUpdateSafePrintGroup(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(((m_hLastSelectedGroup != NULL) ? TRUE : FALSE));
}

void CPwSafeDlg::OnPwlistMoveUp()
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
		MessageBox(TRL("Auto-sorting of the password list is enabled, you cannot move entries manually."), TRL("Stop"), MB_OK | MB_ICONWARNING);
		return;
	}

	p = m_mgr.GetEntry(dwEntryIndex);
	ASSERT(p != NULL); if(p == NULL) return;
	m_mgr.MoveInGroup(p->uGroupId, dwRelativeEntry, dwRelativeEntry - 1);

	_List_SaveView();
	UpdatePasswordList();
	_List_RestoreView();
	m_cList.SetItemState(dwRelativeEntry - 1,
		LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	m_cList.EnsureVisible(dwRelativeEntry - 1, FALSE);
	m_bModified = TRUE;
	_UpdateToolBar();
}

void CPwSafeDlg::OnUpdatePwlistMoveUp(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((((m_dwLastFirstSelectedItem != DWORD_MAX) && (m_dwLastNumSelectedItems == 1)) ? TRUE : FALSE));
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
		MessageBox(TRL("Auto-sorting of the password list is enabled, you cannot move entries manually."), TRL("Stop"), MB_OK | MB_ICONWARNING);
		return;
	}

	p = m_mgr.GetEntry(dwEntryIndex);
	ASSERT(p != NULL); if(p == NULL) return;
	m_mgr.MoveInGroup(p->uGroupId, dwRelativeEntry, 0);

	_List_SaveView();
	UpdatePasswordList();
	_List_RestoreView();
	m_cList.SetItemState(0,
		LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	m_cList.EnsureVisible(0, FALSE);
	m_bModified = TRUE;
	_UpdateToolBar();
}

void CPwSafeDlg::OnUpdatePwlistMoveTop(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((((m_dwLastFirstSelectedItem != DWORD_MAX) && (m_dwLastNumSelectedItems == 1)) ? TRUE : FALSE));
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
		MessageBox(TRL("Auto-sorting of the password list is enabled, you cannot move entries manually."), TRL("Stop"), MB_OK | MB_ICONWARNING);
		return;
	}

	p = m_mgr.GetEntry(dwEntryIndex);
	ASSERT(p != NULL); if(p == NULL) return;
	m_mgr.MoveInGroup(p->uGroupId, dwRelativeEntry, dwRelativeEntry + 1);

	_List_SaveView();
	UpdatePasswordList();
	_List_RestoreView();
	int nSel = min(dwRelativeEntry + 1, dwItemCount - 1);
	m_cList.SetItemState(nSel,
		LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	m_cList.EnsureVisible(nSel, FALSE);
	m_bModified = TRUE;
	_UpdateToolBar();
}

void CPwSafeDlg::OnUpdatePwlistMoveDown(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((((m_dwLastFirstSelectedItem != DWORD_MAX) && (m_dwLastNumSelectedItems == 1)) ? TRUE : FALSE));
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
		MessageBox(TRL("Auto-sorting of the password list is enabled, you cannot move entries manually."), TRL("Stop"), MB_OK | MB_ICONWARNING);
		return;
	}

	p = m_mgr.GetEntry(dwEntryIndex);
	ASSERT(p != NULL); if(p == NULL) return;
	m_mgr.MoveInGroup(p->uGroupId, dwRelativeEntry, dwItemCount - 1);

	_List_SaveView();
	UpdatePasswordList();
	_List_RestoreView();
	int nSel = (int)dwItemCount - 1;
	m_cList.SetItemState(nSel,
		LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	m_cList.EnsureVisible(nSel, FALSE);
	m_bModified = TRUE;
	_UpdateToolBar();
}

void CPwSafeDlg::OnUpdatePwlistMoveBottom(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((((m_dwLastFirstSelectedItem != DWORD_MAX) && (m_dwLastNumSelectedItems == 1)) ? TRUE : FALSE));
}

void CPwSafeDlg::OnBeginDragPwlist(NMHDR* pNMHDR, LRESULT* pResult)
{
	NotifyUserActivity();

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;

	if(m_bFileOpen == FALSE) return;

	DWORD dwEntryIndex = _ListSelToEntryIndex();
	if(dwEntryIndex == DWORD_MAX) return;

	PW_ENTRY *p = m_mgr.GetEntry(dwEntryIndex);
	ASSERT_ENTRY(p); if(p == NULL) return;

	m_bDisplayDialog = TRUE;

	m_cGroups.m_drop.SetDragAccept(TRUE);

	COleDropSource *pDropSource = new COleDropSource;
	COleDataSource *pDataSource = new COleDataSource;

	TRY
	{
		CSharedFile fileShared;
		TRY
		{
			CArchive ar(&fileShared, CArchive::store);
			TRY
			{
				CString strData;

				switch(pNMListView->iSubItem)
				{
				case 0:
					strData = p->pszTitle;
					break;
				case 1:
					strData = p->pszUserName;
					break;
				case 2:
					strData = p->pszURL;
					break;
				case 3:
					m_mgr.UnlockEntryPassword(p);
					strData = p->pszPassword;
					m_mgr.LockEntryPassword(p);
					break;
				case 4:
					strData = p->pszAdditional;
					break;
				case 5:
					_PwTimeToString(p->tCreation, &strData);
					break;
				case 6:
					_PwTimeToString(p->tLastMod, &strData);
					break;
				case 7:
					_PwTimeToString(p->tLastAccess, &strData);
					break;
				case 8:
					_PwTimeToString(p->tExpire, &strData);
					break;
				case 9:
					_UuidToString(p->uuid, &strData);
					break;
				case 10:
					strData = p->pszBinaryDesc;
					break;
				default:
					ASSERT(FALSE);
					break;
				}

				ar.Write((LPCTSTR)strData, strData.GetLength() + sizeof(TCHAR));
				ar.Close();
			}
			CATCH_ALL(eInner)
			{
				ASSERT(FALSE);
			}
			END_CATCH_ALL;
		}
		CATCH_ALL(eMiddle)
		{
			ASSERT(FALSE);
		}
		END_CATCH_ALL;

		pDataSource->CacheGlobalData(CF_TEXT, fileShared.Detach());
		pDataSource->DoDragDrop(DROPEFFECT_MOVE | DROPEFFECT_COPY, NULL, pDropSource);
	}
	CATCH_ALL(eOuter)
	{
		ASSERT(FALSE);
	}
	END_CATCH_ALL;

	SAFE_DELETE(pDataSource);
	SAFE_DELETE(pDropSource);

	m_cGroups.m_drop._RemoveDropSelection();
	m_cGroups.m_drop.SetDragAccept(FALSE);

	_TouchEntry(GetSelectedEntry(), FALSE);
	_UpdateToolBar();

	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnFileChangeLanguage()
{
	NotifyUserActivity();

	CLanguagesDlg dlg;

	m_bDisplayDialog = TRUE;
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
	_OpenLocalFile(PWM_README_FILE, OLF_OPEN);
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
	int i;
	UINT uID = ID_VIEW_TITLE, uState;
	BOOL *pFlag;

	for(i = 0; i < 11; i++)
	{
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

		uState = (*pFlag == TRUE) ? MF_CHECKED : MF_UNCHECKED;
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

	if(_CALLPLUGINS(KPM_FILE_LOCK_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	CString strMenuItem;
	CString strExtended;
	const TCHAR *pSuffix = _T("");

	if((m_bFileOpen == FALSE) && (m_bLocked == FALSE)) { m_bDisplayDialog = FALSE; return; }

	m_menu.GetMenuText(ID_FILE_LOCK, strMenuItem, MF_BYCOMMAND);

	pSuffix = _GetCmdAccelExt(_T("&Lock Workspace"));
	strExtended = TRL("&Lock Workspace");
	strExtended += _T("\t");
	strExtended += pSuffix;

	if((strMenuItem == TRL("&Lock Workspace")) || (strMenuItem == strExtended))
	{
		_DeleteTemporaryFiles();

		// m_nLockedViewParams[0] = (long)m_cGroups.GetFirstVisibleItem();
		m_nLockedViewParams[1] = GetSelectedEntry();
		_Groups_SaveView();
		m_nLockedViewParams[2] = m_cList.GetTopIndex();

		DWORD dwLastSelected = _ListSelToEntryIndex(DWORD_MAX);
		if(dwLastSelected != DWORD_MAX) memcpy(m_pPreLockItemUuid, m_mgr.GetEntry(dwLastSelected)->uuid, 16);
		else memset(m_pPreLockItemUuid, 0, 16);

		m_bExiting = TRUE;
		OnFileClose();
		m_bExiting = FALSE;
		if(m_bFileOpen == TRUE)
		{
			// MessageBox(TRL("First close the open file before opening another one!"), TRL("Password Safe"),
			//	MB_OK | MB_ICONWARNING);
			m_bDisplayDialog = FALSE; return;
		}

		if(m_strLastDb.IsEmpty() == TRUE) { m_bDisplayDialog = FALSE; return; }

		m_bLocked = TRUE;
		strExtended = TRL("&Unlock Workspace");
		strExtended += _T("\t");
		strExtended += _GetCmdAccelExt(_T("&Lock Workspace"));
		m_menu.SetMenuText(ID_FILE_LOCK, strExtended, MF_BYCOMMAND);
		SetStatusTextEx(TRL("Workspace locked"));
		m_btnTbLock.SetTooltipText(TRL("&Unlock Workspace"));

		// _UpdateTitleBar(); // Updated by _UpdateToolBar()
		m_systray.SetIcon(m_hTrayIconLocked);

		ShowEntryDetails(NULL);

		if(m_bEntryView == TRUE) m_reEntryView.SetFocus();
		else if(m_bShowToolBar == TRUE) m_cQuickFind.SetFocus();
		else m_cList.SetFocus();
	}
	else
	{
		_OpenDatabase(NULL, m_strLastDb, NULL, NULL, FALSE, NULL, FALSE);

		if(m_bFileOpen == FALSE)
		{
			// strExtended = PWM_PRODUCT_NAME; strExtended += _T(" [");
			// strExtended += TRL("Workspace locked"); strExtended += _T("]");
			// SetWindowText(strExtended);
			// m_systray.SetTooltipText(strExtended); // Done in _OpenDatabase already
			// m_systray.SetIcon(m_hTrayIconLocked);

			// MessageBox(TRL("Workspace cannot be unlocked!"), TRL("Password Safe"), MB_ICONINFORMATION | MB_OK);
			SetStatusTextEx(TRL("Workspace cannot be unlocked!"));

			m_bDisplayDialog = FALSE; return;
		}

		NotifyUserActivity();
		m_bLocked = FALSE;
		strExtended = TRL("&Lock Workspace");
		strExtended += _T("\t");
		strExtended += _GetCmdAccelExt(_T("&Lock Workspace"));
		m_menu.SetMenuText(ID_FILE_LOCK, strExtended, MF_BYCOMMAND);
		m_btnTbLock.SetTooltipText(TRL("&Lock Workspace"));

		// m_cGroups.SelectSetFirstVisible((HTREEITEM)m_nLockedViewParams[0]);
		// m_cGroups.SelectItem((HTREEITEM)m_nLockedViewParams[1]);
		_Groups_RestoreView();
		UpdatePasswordList();

		m_cList.EnsureVisible(m_cList.GetItemCount() - 1, FALSE);
		m_cList.EnsureVisible(m_nLockedViewParams[2], FALSE);

		if(m_nLockedViewParams[1] != DWORD_MAX)
			m_cList.SetItemState((int)m_nLockedViewParams[1], LVIS_SELECTED, LVIS_SELECTED);

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
			ASSERT(p != NULL);
			ShowEntryDetails(p);
		}

		m_cList.SetFocus();
	}

	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdateFileLock(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((m_bFileOpen || m_bLocked) && (!m_bDisplayDialog));
}

void CPwSafeDlg::OnGroupMoveTop()
{
	NotifyUserActivity();

	DWORD dwGroupId = GetSelectedGroupId();
	if(dwGroupId == DWORD_MAX) return;

	_TouchGroup(dwGroupId, FALSE);
	DWORD dwGroup = m_mgr.GetGroupByIdN(dwGroupId);
	m_mgr.MoveGroup(dwGroup, 0);
	UpdateGroupList();
	_List_SaveView();
	m_cGroups.SelectItem(m_cGroups.GetRootItem());
	UpdatePasswordList();
	_List_RestoreView();
	m_cGroups.SelectSetFirstVisible(m_cGroups.GetRootItem());
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

	DWORD dwGroup = m_mgr.GetGroupByIdN(dwGroupId);
	m_mgr.MoveGroup(dwGroup, (DWORD)m_cGroups.GetCount() - 1);

	UpdateGroupList();
	HTREEITEM hItem = _GroupIdToHTreeItem(dwGroupId);
	m_cGroups.EnsureVisible(hItem);
	m_cGroups.SelectItem(hItem);

	_List_SaveView();
	UpdatePasswordList();
	_List_RestoreView();
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

	DWORD dwGroup = m_mgr.GetGroupByIdN(dwGroupId);
	if(dwGroup == 0) return; // Already is the top item
	VERIFY(m_mgr.MoveGroup(dwGroup, dwGroup - 1));
	_Groups_SaveView(); UpdateGroupList(); _Groups_RestoreView();
	_List_SaveView();
	UpdatePasswordList();
	_List_RestoreView();
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

	DWORD dwGroup = m_mgr.GetGroupByIdN(dwGroupId);
	if(dwGroup == (m_mgr.GetNumberOfGroups() - 1)) return; // Already is last group
	VERIFY(m_mgr.MoveGroup(dwGroup, dwGroup + 1));
	_Groups_SaveView(); UpdateGroupList(); _Groups_RestoreView();
	_List_SaveView();
	UpdatePasswordList();
	_List_RestoreView();
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
		iSpinner++;
		PostMessage(WM_MY_UPDATECLIPBOARD); // Failed to own clipboard so try again
	}
	else
		iSpinner = 0; // Stop posting WM_MY_UPDATECLIPBOARD messages

	return 0;
}

void CPwSafeDlg::ViewHideHandler()
{
	NotifyUserActivity();

	if(m_bMinimized == TRUE)
	{
		BOOL bWasMaximized = m_bWasMaximized;

		// m_systray.MaximiseFromTray(this);
		// OnSysCommand(SC_RESTORE, 0);
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);

		if(bWasMaximized == TRUE) ShowWindow(SW_MAXIMIZE);

		if(m_bAlwaysOnTop == TRUE)
			SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);

		return;
	}

	if(m_bShowWindow == TRUE)
	{
		m_bShowWindow = FALSE;

		if(m_bMinimizeToTray == FALSE)
			m_systray.MinimiseToTray(this);
		else
			OnSysCommand(SC_MINIMIZE, 0);

		if((m_bLockOnMinimize == TRUE) && (m_bLocked == FALSE)) OnFileLock();
	}
	else
	{
		m_bShowWindow = TRUE;

		if(m_bMinimizeToTray == FALSE)
			m_systray.MaximiseFromTray(this);
		else
			OnSysCommand(SC_RESTORE, 0);

		if(m_bWasMaximized == TRUE) ShowWindow(SW_MAXIMIZE);

		if(m_bLocked == TRUE) OnFileLock();
	}
}

void CPwSafeDlg::OnViewHide()
{
	NotifyUserActivity();
	if(m_bDisplayDialog == TRUE) return;
	ViewHideHandler();
}

void CPwSafeDlg::OnImportCsv()
{
	NotifyUserActivity();

	CPwImport pvi;
	CString strFile;
	DWORD dwFlags;
	CString strFilter;
	int nRet;
	DWORD dwGroupId, dwResult;

	m_bDisplayDialog = TRUE;

	strFilter = TRL("CSV files");
	strFilter += _T(" (*.csv)|*.csv|");
	strFilter += TRL("All files");
	strFilter += _T(" (*.*)|*.*||");

	dwFlags = OFN_LONGNAMES | OFN_EXTENSIONDIFFERENT;
	// OFN_EXPLORER = 0x00080000, OFN_ENABLESIZING = 0x00800000
	dwFlags |= 0x00080000 | 0x00800000;
	dwFlags |= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	CFileDialog dlg(TRUE, _T("csv"), _T("*.csv"), dwFlags, strFilter, this);

	nRet = dlg.DoModal();
	if(nRet == IDOK)
	{
		strFile = dlg.GetPathName();

		dwGroupId = GetSelectedGroupId();
		ASSERT(dwGroupId != DWORD_MAX);

		dwResult = pvi.ImportCsvToDb(strFile, &m_mgr, dwGroupId);

		if(dwResult == DWORD_MAX)
		{
			_SortListIfAutoSort();
			if(m_nAutoSort == 0) UpdatePasswordList();
			m_bModified = TRUE;
		}
		else
		{
			CString str = TRL("An error occured while importing the file. File cannot be imported."); str += _T("\r\n\r\n");
			str += TRL("Entry"); str += _T(": #");
			CString strTemp; strTemp.Format(_T("%u"), dwResult);
			str += strTemp;

			MessageBox(str, TRL("An error occured"), MB_ICONWARNING | MB_OK);
		}
	}
	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdateImportCsv(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
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
	DWORD dwGroupId = GetSelectedGroupId();

	if(dwGroupId == DWORD_MAX) return;
	if(dwGroupId == m_mgr.GetGroupId(PWS_SEARCHGROUP))
	{
		_RemoveSearchGroup();
		m_cList.DeleteAllItems();
		return;
	}

	int nTop = m_cList.GetTopIndex();

	m_mgr.SortGroup(dwGroupId, (DWORD)dwByField);
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

	CPwImport pvi;
	CString strFile;
	DWORD dwFlags;
	CString strFilter;
	int nRet;

	m_bDisplayDialog = TRUE;

	strFilter = TRL("Text files");
	strFilter += _T(" (*.txt)|*.txt|");
	strFilter += TRL("All files");
	strFilter += _T(" (*.*)|*.*||");

	dwFlags = OFN_LONGNAMES | OFN_EXTENSIONDIFFERENT;
	// OFN_EXPLORER = 0x00080000, OFN_ENABLESIZING = 0x00800000
	dwFlags |= 0x00080000 | 0x00800000;
	dwFlags |= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	CFileDialog dlg(TRUE, _T("txt"), _T("*.txt"), dwFlags, strFilter, this);

	nRet = dlg.DoModal();
	if(nRet == IDOK)
	{
		strFile = dlg.GetPathName();

		if(pvi.ImportCWalletToDb(strFile, &m_mgr) == TRUE)
		{
			UpdateGroupList();
			m_cGroups.EnsureVisible(_GetLastGroupItem(&m_cGroups));
			_SortListIfAutoSort();
			if(m_nAutoSort == 0) UpdatePasswordList();
			m_bModified = TRUE;
		}
		else
		{
			MessageBox(TRL("An error occured while importing the file. File cannot be imported."),
				TRL("Password Safe"), MB_ICONWARNING);
		}
	}
	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdateImportCWallet(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
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
		else if(pFocusWnd == &m_cQuickFind)
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

	if(((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_KEYUP)) && (pMsg->wParam == VK_RETURN))
	{
		if(GetFocus() == &m_cQuickFind)
		{
			if(pMsg->message == WM_KEYDOWN) _DoQuickFind();
			return TRUE; // Also ignores WM_KEYUP sent to the edit control
		}
	}

	m_tip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}

void CPwSafeDlg::OnUpdateFileNew(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bLocked == FALSE);
}

void CPwSafeDlg::OnUpdateFileOpen(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bLocked == FALSE);
}

void CPwSafeDlg::OnImportPwSafe()
{
	NotifyUserActivity();

	CPwImport pvi;
	CString strFile;
	DWORD dwFlags;
	CString strFilter;
	int nRet;
	DWORD dwGroupId;

	m_bDisplayDialog = TRUE;

	strFilter = TRL("Text files");
	strFilter += _T(" (*.txt)|*.txt|");
	strFilter += TRL("All files");
	strFilter += _T(" (*.*)|*.*||");

	dwFlags = OFN_LONGNAMES | OFN_EXTENSIONDIFFERENT;
	// OFN_EXPLORER = 0x00080000, OFN_ENABLESIZING = 0x00800000
	dwFlags |= 0x00080000 | 0x00800000;
	dwFlags |= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	CFileDialog dlg(TRUE, _T("txt"), _T("*.txt"), dwFlags, strFilter, this);

	nRet = dlg.DoModal();
	if(nRet == IDOK)
	{
		strFile = dlg.GetPathName();

		dwGroupId = GetSelectedGroupId();
		ASSERT(dwGroupId != DWORD_MAX);

		if(pvi.ImportPwSafeToDb(strFile, &m_mgr) == TRUE)
		{
			_Groups_SaveView(); UpdateGroupList(); _Groups_RestoreView();
			_SortListIfAutoSort();
			if(m_nAutoSort == 0) UpdatePasswordList();
			m_bModified = TRUE;
		}
		else
		{
			MessageBox(TRL("An error occured while importing the file. File cannot be imported."),
				TRL("Password Safe"), MB_ICONWARNING);
		}
	}
	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdateImportPwSafe(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
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
	PW_GROUP *pGroup;
	PW_TIME tNow;

	pGroup = m_mgr.GetGroupById(dwGroupId);
	ASSERT(pGroup != NULL);

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
	PW_ENTRY *pEntry;

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

	pEntry = m_mgr.GetEntryByUuid(aUuid);
	ASSERT_ENTRY(pEntry);

	_GetCurrentPwTime(&tNow);

	pEntry->tLastAccess = tNow;
	if(bEdit == TRUE) pEntry->tLastMod = tNow;

	_List_SetEntry(dwListIndex, pEntry, FALSE, &tNow);

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

	if((m_bMinimized == TRUE) || (m_bShowWindow == FALSE)) return;

	CWnd *pFocusWnd = GetFocus();
	BOOL bNeedSetFocus = FALSE;

	DWORD dwSelectedEntry = GetSelectedEntry();
	DWORD dwNumSelectedEntries = GetSelectedEntriesCount();
	DWORD dwNumberOfGroups = m_mgr.GetNumberOfGroups();
	DWORD dwNumberOfEntries = m_mgr.GetNumberOfEntries();

	ULONGLONG ullListParams;
	ullListParams = ((ULONGLONG)(dwSelectedEntry & 0x0000FFFF) << 48) |
		((ULONGLONG)(dwNumSelectedEntries & 0x000000FF) << 40) |
		((ULONGLONG)(dwNumberOfGroups & 0x000000FF) << 32) |
		((ULONGLONG)(dwNumberOfEntries & 0x0000FFFF) << 16) |
		((ULONGLONG)(m_bLocked & 0x00000001) << 15) |
		((ULONGLONG)(m_bModified & 0x00000001) << 14) |
		((ULONGLONG)(m_bMinimized & 0x00000001) << 13);

	// ullListParams = (((ULONGLONG)dwSelectedEntry) << 34) ^
	//	(((ULONGLONG)dwNumSelectedEntries) << 2) ^ (m_bLocked << 1) ^
	//	m_bModified ^ (((ULONGLONG)dwNumberOfEntries) << 14);

	// Update the rest (toolbar, entry view, ...) only if needed
	if((ullListParams == m_ullLastListParams) && (bForceUpdate == FALSE)) return;
	m_ullLastListParams = ullListParams;

	CString strStatus;
	strStatus.Format(TRL("Total: %u groups / %u entries"), dwNumberOfGroups, dwNumberOfEntries);
	SetStatusTextEx(strStatus, 0);
	strStatus.Format(TRL("%u of %u selected"), (dwNumSelectedEntries == DWORD_MAX) ? 0 : dwNumSelectedEntries,
		(m_bFileOpen == TRUE) ? m_cList.GetItemCount() : 0);
	SetStatusTextEx(strStatus, 1);

	DWORD dwFirstEntryIndex = _ListSelToEntryIndex(dwSelectedEntry);
	PW_ENTRY *p = NULL;

	if(dwFirstEntryIndex != DWORD_MAX) p = m_mgr.GetEntry(dwFirstEntryIndex);
	if(p != NULL) ShowEntryDetails(p);

	if(m_bFileOpen == TRUE)
	{
		UTB_ENSURE_ENABLED_STATE(&m_btnTbAddEntry, TRUE);
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

	if(m_bLocked == FALSE)
	{
		UTB_ENSURE_ENABLED_STATE(&m_btnTbNew, TRUE);
		UTB_ENSURE_ENABLED_STATE(&m_btnTbOpen, TRUE);
	}
	else
	{
		UTB_ENSURE_ENABLED_STATE(&m_btnTbNew, FALSE);
		UTB_ENSURE_ENABLED_STATE(&m_btnTbOpen, FALSE);
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

	UINT uState;
	BOOL bChecked;

	uState = m_menu.GetMenuState(ID_VIEW_SHOWTOOLBAR, MF_BYCOMMAND);
	ASSERT(uState != 0xFFFFFFFF);

	if(uState & MF_CHECKED) bChecked = TRUE;
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
	int nCommand;

	if(bShow == FALSE) nCommand = SW_HIDE;
	else nCommand = SW_SHOW;

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
	DWORD dwSearchGroupId = m_mgr.GetGroupId(PWS_SEARCHGROUP);

	if(dwSearchGroupId != DWORD_MAX)
	{
		DWORD dwSearchItemsCount = m_mgr.GetNumberOfItemsInGroupN(dwSearchGroupId);

		if(dwSearchItemsCount == 0) // Delete only if the group is empty
		{
			m_mgr.DeleteGroupById(dwSearchGroupId); // Remove from password manager
			UpdateGroupList();
			ShowEntryDetails(NULL);

			return TRUE;
		}
	}

	return FALSE;
}

void CPwSafeDlg::OnPwlistMassModify()
{
	NotifyUserActivity();

	CEntryPropertiesDlg dlg;
	UINT uState;
	DWORD i;
	DWORD dwIndex;
	PW_ENTRY *p;
	DWORD dwGroupId;

	if(m_bFileOpen == FALSE) return;

	m_bDisplayDialog = TRUE;

	dlg.m_pMgr = &m_mgr;
	dlg.m_pParentIcons = &m_ilIcons;
	dlg.m_dwDefaultExpire = m_dwDefaultExpire;

	if(_CALLPLUGINS(KPM_MASSMODIFY_ENTRIES_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	if(dlg.DoModal() == TRUE)
	{
		dwGroupId = m_mgr.GetGroupIdByIndex((DWORD)dlg.m_nGroupInx);
		ASSERT(dwGroupId != DWORD_MAX);

		for(i = 0; i < (DWORD)m_cList.GetItemCount(); i++)
		{
			uState = m_cList.GetItemState((int)i, LVIS_SELECTED);
			if(uState & LVIS_SELECTED)
			{
				dwIndex = _ListSelToEntryIndex(i); // Uses UUID to get the entry
				ASSERT(dwIndex != DWORD_MAX); if(dwIndex == DWORD_MAX) continue;

				p = m_mgr.GetEntry(dwIndex);
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
	CString str, str2;
	int i;
	TCHAR tch;

	str.Empty();

	ASSERT(lptString != NULL); if(lptString == NULL) return str;

	for(i = 0; i < (int)_tcslen(lptString); i++)
	{
		tch = lptString[i];

		if(tch == _T('\\')) str += _T("\\\\");
		else if(tch == _T('\r')) { }
		else if(tch == _T('{')) { str += _T("\\{"); }
		else if(tch == _T('}')) { str += _T("\\}"); }
		else if(tch == _T('\n')) { str += _T("\\par "); }
#ifndef _UNICODE
		else str += tch;
#else
		else if((tch < 128) && (tch >= 32)) str += tch;
		else
		{
			str2.Format(_T("\\u%d?"), tch);
			str += str2;
		}
#endif

		// else if((tch < 128) && (tch >= 32)) str += tch;
		// else
		// {
		//	str2.Format(_T("%02x"), (BYTE)(tch & 0xFF));
		//	str += _T("\\'"); str += str2;
		// }
	}

	return str;
}

void CPwSafeDlg::ShowEntryDetails(PW_ENTRY *p)
{
	CString str;
	CString str2;
	PPW_GROUP pg;

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

	m_mgr.UnlockEntryPassword(p);

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
	pg = m_mgr.GetGroupById(p->uGroupId); ASSERT(pg != NULL);
	if(pg != NULL) str += _MakeRtfString(pg->pszGroupName);

	str += _T(", \\b ");
	str += TRL("Title:"); str += _T("\\b0  ");
	str += _MakeRtfString(p->pszTitle);

	str += _T(", \\b ");
	str += TRL("User Name:"); str += _T("\\b0  ");
	if(m_bUserStars == FALSE) str += _MakeRtfString(p->pszUserName);
	else str += _T("********");

	str += _T(", \\b ");
	str += TRL("URL:"); str += _T("\\b0  ");
	str += _MakeRtfString(p->pszURL);

	str += _T(", \\b ");
	str += TRL("Password:"); str += _T("\\b0  ");
	CString strTempPassword = _MakeRtfString(p->pszPassword);
	if(m_bPasswordStars == FALSE) str += strTempPassword;
	else str += _T("********");
	EraseCString(&strTempPassword);

	str += _T(", \\b ");
	str += TRL("Creation Time"); str += _T(":\\b0  ");
	_PwTimeToString(p->tCreation, &str2); str += _MakeRtfString(str2);

	str += _T(", \\b ");
	str += TRL("Last Modification"); str += _T(":\\b0  ");
	_PwTimeToString(p->tLastMod, &str2); str += _MakeRtfString(str2);

	str += _T(", \\b ");
	str += TRL("Last Access"); str += _T(":\\b0  ");
	_PwTimeToString(p->tLastAccess, &str2); str += _MakeRtfString(str2);

	if(memcmp(&p->tExpire, &g_tNeverExpire, sizeof(PW_TIME)) != 0)
	{
		str += _T(", \\b ");
		str += TRL("Expires"); str += _T(":\\b0  ");
		_PwTimeToString(p->tExpire, &str2); str += _MakeRtfString(str2);
	}

	if(_tcslen(p->pszBinaryDesc) != 0)
	{
		str += _T(", \\b ");
		str += TRL("Attachment"); str += _T(":\\b0  ");
		str += _MakeRtfString((LPCTSTR)(p->pszBinaryDesc));
	}

	if(p->pszAdditional != NULL)
	{
		if(_tcslen(p->pszAdditional) != 0)
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
	}

	str += _T("\\pard }");

	// === End entry view RTF assembly ===

	m_mgr.LockEntryPassword(p);

#ifndef _UNICODE
	m_reEntryView.SetRTF(str, SF_RTF);
#else
	m_reEntryView.SetRTF(str, SF_RTF | SF_UNICODE);
#endif

	if(_tcslen(p->pszURL) != 0) // Make the URL clickable
	{
		CHARRANGE crURL;
		CHARFORMAT cfURL;

		crURL.cpMin = _tcslen(TRL("Group:")) + _tcslen(TRL("Title")) + _tcslen(TRL("User Name:")) + _tcslen(TRL("URL:")) + 11;
		crURL.cpMin += _tcslen(pg->pszGroupName) + _tcslen(p->pszTitle);
		if(m_bUserStars == FALSE) crURL.cpMin += _tcslen(p->pszUserName); else crURL.cpMin += 8;
		crURL.cpMax = crURL.cpMin + _tcslen(p->pszURL);

		ZeroMemory(&cfURL, sizeof(CHARFORMAT));
		cfURL.cbSize = sizeof(CHARFORMAT);
		cfURL.dwMask = CFM_LINK | CFM_COLOR | CFM_UNDERLINE;
		cfURL.dwEffects = CFE_LINK | CFE_UNDERLINE;
		cfURL.crTextColor = RGB(0, 0, 255);

		m_reEntryView.SetSel(crURL);
		m_reEntryView.SetSelectionCharFormat(cfURL);
	}

	m_reEntryView.SetSel(0, 0);

	EraseCString(&str);
}

void CPwSafeDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	NotifyUserActivity();

	LONG nAddTop;
	RECT rectWindow;

	if(m_hDraggingGroup != NULL)
	{
		CPoint pt; // Local copy of 'point', because we will modify it here

		pt = point;
		ClientToScreen(&pt);

		CImageList::DragMove(pt);
		CImageList::DragShowNolock(FALSE);

		if((CWnd::WindowFromPoint(pt) != &m_cGroups) || (m_bCanDragGroup == FALSE))
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

	GetWindowRect(&rectWindow);

	if(m_bShowToolBar == TRUE) nAddTop = 26;
	else nAddTop = 0;

	int cyMenu = GetSystemMetrics(SM_CYMENU);

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
	else
	{
		SetCursor(m_hArrowCursor);
	}

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
		if(m_bCanDragGroup == TRUE)
		{
			CPoint pt = point;
			ClientToScreen(&pt);

			BOOL bCopy = ((GetKeyState(VK_CONTROL) & 0x1000) > 0) ? TRUE : FALSE;

			HTREEITEM hItemDropTo = m_cGroups.GetDropHilightItem();

			if(hItemDropTo != m_hDraggingGroup)
			{
				DWORD dwDragGroupId = m_cGroups.GetItemData(m_hDraggingGroup);
				DWORD dwDragGroupPos = m_mgr.GetGroupByIdN(dwDragGroupId);
				DWORD dwNewGroupId = dwDragGroupId;

				PW_GROUP *pNew = NULL;

				ASSERT(dwDragGroupPos != DWORD_MAX);
				if(dwDragGroupPos != DWORD_MAX)
				{
					PW_GROUP grpNew = *m_mgr.GetGroup(dwDragGroupPos);
					grpNew.usLevel = 0;
					grpNew.uGroupId = 0; // Create new group
					m_mgr.AddGroup(&grpNew);
					pNew = m_mgr.GetGroup(m_mgr.GetNumberOfGroups() - 1);
					dwNewGroupId = pNew->uGroupId;
				}

				if(hItemDropTo != NULL) // Dropped on item
				{
					DWORD dwDragToGroupId = m_cGroups.GetItemData(hItemDropTo);
					ASSERT(dwDragToGroupId != DWORD_MAX);
					DWORD dwDragToGroupPos = m_mgr.GetGroupByIdN(dwDragToGroupId);
					ASSERT(dwDragToGroupPos != DWORD_MAX);

					DWORD dwVParent = m_mgr.GetLastChildGroup(dwDragToGroupPos);

					if(pNew != NULL)
						pNew->usLevel = (USHORT)(m_mgr.GetGroup(dwDragToGroupPos)->usLevel + 1);

					m_mgr.GetGroup(dwDragToGroupPos)->dwFlags |= PWGF_EXPANDED;
					m_mgr.MoveGroup(m_mgr.GetNumberOfGroups() - 1, dwVParent + 1);
				}
				else // Dropped on empty space
				{
				}

				// Fix group ID, unassociate all entries from the group that we will delete
				if(bCopy == FALSE) m_mgr.SubstEntryGroupIds(dwDragGroupId, dwNewGroupId);

				// If moving, delete source group
				if(bCopy == FALSE) m_mgr.DeleteGroupById(dwDragGroupId);

				if(hItemDropTo != NULL) _SyncItem(&m_cGroups, hItemDropTo, FALSE);
				m_cGroups.SelectDropTarget(NULL);
				_Groups_SaveView(FALSE); UpdateGroupList(); _Groups_RestoreView();
				m_bModified = TRUE;
			}
		}

		m_cGroups.SelectDropTarget(NULL);
		_FinishDragging(TRUE);
		RedrawWindow();
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
				if(strSelectedURL == pCmpEntry->pszURL) OnPwlistVisitUrl();
				else ParseAndOpenURLWithEntryInfo(strSelectedURL, pCmpEntry);
			}
			else OpenUrlEx(strSelectedURL);

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

				BCMenu *psub = (BCMenu *)m_menuColView.GetSubMenu(0);
				_TranslateMenu(psub, TRUE, NULL);
				_EnableViewMenuItems(psub);
				psub->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, AfxGetMainWnd());
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

	CTanWizardDlg dlg;
	PW_ENTRY pwTemplate;
	PW_TIME tNow;
	CString strSubString, strTemp;
	BOOL bValidSubString, bAcceptable;
	int i;
	TCHAR tch;
	DWORD dwCurGroupId = GetSelectedGroupId();
	DWORD dwNumber = 0;

	ASSERT(dwCurGroupId != DWORD_MAX); if(dwCurGroupId == DWORD_MAX) return;

	m_bDisplayDialog = TRUE;

	if(_CALLPLUGINS(KPM_TANWIZARD_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	if(dlg.DoModal() == IDOK)
	{
		memset(&pwTemplate, 0, sizeof(PW_ENTRY));
		_GetCurrentPwTime(&tNow);
		pwTemplate.tCreation = tNow;
		pwTemplate.tLastMod = tNow;
		pwTemplate.tLastAccess = tNow;
		pwTemplate.pszTitle = (TCHAR *)(PWS_TAN_ENTRY);
		pwTemplate.pszURL = _T("");
		pwTemplate.pszUserName = _T("");
		m_mgr.GetNeverExpireTime(&pwTemplate.tExpire);
		pwTemplate.uImageId = 29;
		pwTemplate.uGroupId = dwCurGroupId;
		pwTemplate.pBinaryData = NULL;
		pwTemplate.pszBinaryDesc = _T("");
		pwTemplate.uBinaryDataLen = 0;

		dwNumber = dlg.m_dwStartNumber;

		dlg.m_strTans += _T(" "); // Append terminating char

		bValidSubString = FALSE;
		for(i = 0; i < dlg.m_strTans.GetLength(); i++)
		{
			tch = dlg.m_strTans.GetAt(i);

			bAcceptable = FALSE;
			if((tch >= _T('0')) && (tch <= _T('9'))) bAcceptable = TRUE;
			if((tch >= _T('a')) && (tch <= _T('z'))) bAcceptable = TRUE;
			if((tch >= _T('A')) && (tch <= _T('Z'))) bAcceptable = TRUE;

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
				memset(pwTemplate.uuid, 0, 16);
				pwTemplate.pszPassword = (TCHAR *)(LPCTSTR)strSubString;
				pwTemplate.uPasswordLen = strSubString.GetLength();

				if(dlg.m_bAssignNumbers == FALSE)
					pwTemplate.pszAdditional = _T("");
				else
				{
					strTemp.Format(_T("%03u"), dwNumber);
					pwTemplate.pszAdditional = (LPTSTR)(LPCTSTR)strTemp;
					dwNumber++;
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
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnFilePrintPreview()
{
	NotifyUserActivity();

	CPwExport cExp;
	PWEXPORT_OPTIONS pwo;
	TCHAR szFile[MAX_PATH * 2];

	_DeleteTemporaryFiles();

	if(m_bFileOpen == FALSE) return;
	if(_IsUnsafeAllowed() == FALSE) return;

	m_bDisplayDialog = TRUE;

	cExp.SetManager(&m_mgr);
	cExp.SetNewLineSeq(m_bWindowsNewLine);
	cExp.SetFormat(PWEXP_HTML);

	if(_CALLPLUGINS(KPM_FILE_PRINTPREVIEW_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	GetTempPath(MAX_PATH * 2, szFile);
	if(szFile[_tcslen(szFile) - 1] != _T('\\')) _tcscat_s(szFile, _countof(szFile), _T("\\"));
	_tcscat_s(szFile, _countof(szFile), _T("pwsafetmp.html"));

	if(GetExportOptions(&pwo, &cExp) == FALSE) { m_bDisplayDialog = FALSE; return; }

	BOOL bRet;
	bRet = cExp.ExportGroup(szFile, DWORD_MAX, &pwo, NULL); // Export all: set group ID to DWORD_MAX

	if(bRet == FALSE)
	{
		MessageBox(TRL("Cannot open temporary file for printing!"), TRL("Stop"),
			MB_OK | MB_ICONWARNING);
		m_bDisplayDialog = FALSE;
		return;
	}

	ShellExecute(m_hWnd, _T("open"), szFile, NULL, NULL, SW_SHOW);

	m_strTempFile = szFile;
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnInfoTranslation()
{
	NotifyUserActivity();

	CString str;

	str = TRL("Currently used language"); str += _T(": ");
	if(_tcscmp(TRL("~LANGUAGENAME"), _T("~LANGUAGENAME")) != 0)
	{
		str += TRL("~LANGUAGENAME");
		str += _T(" (");
		LPCTSTR lpName = GetCurrentTranslationTable();
		str += (lpName != NULL) ? lpName : _T("<?>");
		str += _T(".lng)");
	}
	else str += _T("Unknown or English version");
	str += _T("\r\n");

	str += TRL("Language file version"); str += _T(": ");
	if(_tcscmp(TRL("~LANGUAGEVERSION"), _T("~LANGUAGEVERSION")) != 0)
		str += TRL("~LANGUAGEVERSION");
	else str += _T("Unknown or English version");
	str += _T("\r\n");

	str += TRL("Author"); str += _T(": ");
	if(_tcscmp(TRL("~LANGUAGEAUTHOR"), _T("~LANGUAGEAUTHOR")) != 0)
		str += TRL("~LANGUAGEAUTHOR");
	else str += _T("Unknown or English version");
	str += _T("\r\n");

	str += TRL("Translation author contact"); str += _T(": ");
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

	DWORD dwGroupId = pTree->GetItemData(hItem);
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
			if(pGroup->dwFlags & PWGF_EXPANDED)
				pTree->Expand(hItem, TVE_EXPAND);
			else
				pTree->Expand(hItem, TVE_COLLAPSE);
		}
	}
}

HTREEITEM CPwSafeDlg::_GroupIdToHTreeItem(DWORD dwGroupId)
{
	return _FindSelectInTree(&m_cGroups, m_cGroups.GetRootItem(), dwGroupId);
}

HTREEITEM CPwSafeDlg::_FindSelectInTree(CTreeCtrl *pTree, HTREEITEM hRoot, DWORD dwGroupId)
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
	if(_CALLPLUGINS(KPM_GROUP_ADD_SUBGROUP_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }
	m_bDisplayDialog = FALSE;

	DWORD dwGroupCount = m_mgr.GetNumberOfGroups();
	ASSERT(dwGroupCount >= 1); if(dwGroupCount == 0) return;

	HTREEITEM hItem = m_cGroups.GetSelectedItem();
	ASSERT(hItem != NULL); if(hItem == NULL) return;

	DWORD dwRealParentId = m_cGroups.GetItemData(hItem);
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
	pCmdUI->Enable(((m_hLastSelectedGroup != NULL) ? TRUE : FALSE));
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
		int nX, nY;

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

	SetFocus();
	InvalidateRect(&rcItem, TRUE);
	UpdateWindow();
	SetCapture();

	m_hDraggingGroup = pNMTreeView->itemNew.hItem;
	if(m_cGroups.ItemHasChildren(m_hDraggingGroup) == TRUE) m_bCanDragGroup = FALSE;
	else m_bCanDragGroup = TRUE;

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

	if(_CALLPLUGINS(KPM_GROUP_SORT_PRE, 0, 0) == FALSE) return;

	m_mgr.SortGroupList();
	UpdateGroupList();
	UpdatePasswordList();
	m_bModified = TRUE;
}

void CPwSafeDlg::OnUpdateGroupSort(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

BOOL CPwSafeDlg::_CheckIfCanSort()
{
	if(m_nAutoSort == 0) return TRUE;

	MessageBox(TRL("Auto-sorting of the password list is enabled, you cannot sort the list manually now."), TRL("Stop"), MB_ICONWARNING | MB_OK);
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
	pCmdUI->Enable(((m_hLastSelectedGroup != NULL) ? TRUE : FALSE));
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
	pCmdUI->Enable(((m_hLastSelectedGroup != NULL) ? TRUE : FALSE));
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
		if(_IsUnsafeAllowed() == FALSE) return;
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
		for(nItem = 0; nItem < m_cList.GetItemCount(); nItem++)
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
	CString strSample;
	CString strFilter;
	PW_ENTRY *pEntry;
	DWORD dwSelectedInx = _ListSelToEntryIndex();

	if(m_bFileOpen == FALSE) return;
	if(dwSelectedInx == DWORD_MAX) return;

	pEntry = m_mgr.GetEntry(dwSelectedInx);
	ASSERT(pEntry != NULL); if(pEntry == NULL) return;

	m_bDisplayDialog = TRUE;

	if(_tcslen(pEntry->pszBinaryDesc) == 0)
	{
		MessageBox(TRL("There is no file attached with this entry."), TRL("Password Safe"), MB_ICONINFORMATION);
		m_bDisplayDialog = FALSE;
		return;
	}

	strSample = pEntry->pszBinaryDesc;

	strFilter = TRL("All files");
	strFilter += _T(" (*.*)|*.*||");

	dwFlags = OFN_LONGNAMES | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	dwFlags |= OFN_EXTENSIONDIFFERENT;
	// OFN_EXPLORER = 0x00080000, OFN_ENABLESIZING = 0x00800000
	dwFlags |= 0x00080000 | 0x00800000 | OFN_NOREADONLYRETURN;
	CFileDialog dlg(FALSE, NULL, strSample, dwFlags, strFilter, this);

	if(dlg.DoModal() == IDOK) { m_mgr.SaveBinaryData(pEntry, dlg.GetPathName()); }
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdatePwlistSaveAttach(CCmdUI* pCmdUI)
{
	BOOL bEnable = TRUE;

	if(m_dwLastFirstSelectedItem == DWORD_MAX) bEnable = FALSE;
	if(m_dwLastNumSelectedItems != 1) bEnable = FALSE;

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

BOOL CPwSafeDlg::_IsUnsafeAllowed()
{
	if(m_bDisableUnsafeAtStart == FALSE) return TRUE;

	CString str = TRL("Unsafe operations are disabled.");
	str += _T("\r\n\r\n");
	str += TRL("To execute this operation you must enable unsafe operations in the options dialog.");
	MessageBox(str, TRL("Stop"), MB_ICONWARNING | MB_OK);

	return FALSE;
}

void CPwSafeDlg::OnFileShowDbInfo()
{
	NotifyUserActivity();

	CDbSettingsDlg dlg;
	int nAlgorithmOld;
	DWORD dwOldKeyEncRounds;

	m_bDisplayDialog = TRUE;

	dlg.m_nAlgorithm = m_mgr.GetAlgorithm();
	nAlgorithmOld = dlg.m_nAlgorithm;

	dlg.m_dwNumKeyEnc = m_mgr.GetKeyEncRounds();
	dwOldKeyEncRounds = dlg.m_dwNumKeyEnc;

	if(_CALLPLUGINS(KPM_FILE_DBSETTINGS_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	if(dlg.DoModal() == IDOK)
	{
		m_mgr.SetAlgorithm(dlg.m_nAlgorithm);
		if(nAlgorithmOld != dlg.m_nAlgorithm) m_bModified = TRUE;

		m_mgr.SetKeyEncRounds(dlg.m_dwNumKeyEnc);
		if(dwOldKeyEncRounds != dlg.m_dwNumKeyEnc) m_bModified = TRUE;
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

	CEntryListDlg dlg;

	if(m_bFileOpen == FALSE) return;

	m_bDisplayDialog = TRUE;

	PW_ENTRY *pe;
	DWORD dwDateNow, dwDate;
	PW_TIME tNow;

	_GetCurrentPwTime(&tNow);
	dwDateNow = ((DWORD)tNow.shYear << 16) | ((DWORD)tNow.btMonth << 8) | ((DWORD)tNow.btDay & 0xff);

	dlg.m_pMgr = &m_mgr;
	dlg.m_bPasswordStars = m_bPasswordStars;
	dlg.m_bUserStars = m_bUserStars;
	dlg.m_pImgList = &m_ilIcons;
	dlg.m_strBannerTitle = TRL("Expired Entries");
	dlg.m_strBannerCaption = TRL("This is a list of all expired entries");
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
		DWORD i;
		BOOL bAtLeastOneExpired = FALSE;
		DWORD dwInvalidGroup1 = m_mgr.GetGroupId(PWS_BACKUPGROUP_SRC);
		DWORD dwInvalidGroup2 = m_mgr.GetGroupId(PWS_BACKUPGROUP);

		for(i = 0; i < m_mgr.GetNumberOfEntries(); i++)
		{
			pe = m_mgr.GetEntry(i);
			ASSERT(pe != NULL); if(pe == NULL) continue;

			if((pe->uGroupId == dwInvalidGroup1) || (pe->uGroupId == dwInvalidGroup2)) continue;

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
				dwDate = ((DWORD)pe->tExpire.shYear << 16) | ((DWORD)pe->tExpire.btMonth << 8) | ((DWORD)pe->tExpire.btDay & 0xff);

				if((dwDate >= dwDateNow) && ((dwDate - dwDateNow) <= PWV_SOONTOEXPIRE_DAYS))
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

				m_cList.EnsureVisible((int)dwPos, FALSE);
				m_cList.SetItemState((int)dwPos, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);

				m_cList.SetFocus();
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

	CPwImport pvi;
	CString strFile;
	DWORD dwFlags;
	CString strFilter;
	int nRet;
	DWORD dwGroupId;

	m_bDisplayDialog = TRUE;

	strFilter = TRL("Text files");
	strFilter += _T(" (*.txt)|*.txt|");
	strFilter += TRL("All files");
	strFilter += _T(" (*.*)|*.*||");

	dwFlags = OFN_LONGNAMES | OFN_EXTENSIONDIFFERENT;
	// OFN_EXPLORER = 0x00080000, OFN_ENABLESIZING = 0x00800000
	dwFlags |= 0x00080000 | 0x00800000;
	dwFlags |= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	CFileDialog dlg(TRUE, _T("txt"), _T("*.txt"), dwFlags, strFilter, this);

	nRet = dlg.DoModal();
	if(nRet == IDOK)
	{
		strFile = dlg.GetPathName();

		dwGroupId = GetSelectedGroupId();
		ASSERT(dwGroupId != DWORD_MAX);

		if(pvi.ImportPVaultToDb(strFile, &m_mgr) == TRUE)
		{
			_Groups_SaveView(); UpdateGroupList(); _Groups_RestoreView();
			_SortListIfAutoSort();
			if(m_nAutoSort == 0) UpdatePasswordList();
			m_bModified = TRUE;
		}
		else
		{
			MessageBox(TRL("An error occured while importing the file. File cannot be imported."),
				TRL("Password Safe"), MB_ICONWARNING);
		}
	}
	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdateImportPvault(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::_HandleEntryDrop(DWORD dwDropType, HTREEITEM hTreeItem)
{
	DWORD dwToGroupId;
	PW_ENTRY pwT;
	PW_ENTRY *p;
	DWORD i;
	UINT uState;
	DWORD dwIndex;
	CString strPasswordCopy;
	PW_TIME tNow;

	dwToGroupId = m_cGroups.GetItemData(hTreeItem);
	ASSERT(m_mgr.GetGroupById(dwToGroupId) != NULL); if(m_mgr.GetGroupById(dwToGroupId) == NULL) return;

	if((dwDropType != DROPEFFECT_MOVE) && (dwDropType != DROPEFFECT_COPY)) { ASSERT(FALSE); return; }

	CString strGroupTest;
	PW_GROUP *pGroupTest = m_mgr.GetGroupById(dwToGroupId);
	if(CPwManager::IsAllowedStoreGroup(pGroupTest->pszGroupName, PWS_SEARCHGROUP) == FALSE)
	{
		MessageBox(TRL("The group you selected cannot store entries. Please select an other group."),
			TRL("Stop"), MB_ICONWARNING | MB_OK);
		return;
	}

	_GetCurrentPwTime(&tNow);
	for(i = 0; i < (DWORD)m_cList.GetItemCount(); i++)
	{
		uState = m_cList.GetItemState((int)i, LVIS_SELECTED);

		if((uState & LVIS_SELECTED) > 0)
		{
			dwIndex = _ListSelToEntryIndex(i); // Uses UUID to get the entry
			ASSERT(dwIndex != DWORD_MAX); if(dwIndex == DWORD_MAX) continue;

			p = m_mgr.GetEntry(dwIndex);
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
				strPasswordCopy = p->pszPassword;
				m_mgr.LockEntryPassword(p);

				memcpy(&pwT, p, sizeof(PW_ENTRY)); // Copy entry
				ZeroMemory(pwT.uuid, 16 * sizeof(BYTE)); // Create new UUID
				pwT.uGroupId = dwToGroupId; // Set group ID
				pwT.pszPassword = (TCHAR *)(LPCTSTR)strPasswordCopy;
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
	_List_RestoreView();
	UpdatePasswordList();

	m_bModified = TRUE;
}

void CPwSafeDlg::SetStatusTextEx(LPCTSTR lpStatusText, int nPane)
{
	if(nPane == -1) nPane = 2;
	ASSERT(nPane < 3);

	m_sbStatus.SetText(lpStatusText, nPane, 0);
}

void CPwSafeDlg::_DoQuickFind()
{
	const DWORD dwFlags = PWMF_TITLE | PWMF_USER | PWMF_URL | PWMF_PASSWORD | PWMF_ADDITIONAL | PWMF_GROUPNAME;

	UpdateData(TRUE);

	m_cList.DeleteAllItems();

	PW_TIME tNow;
	_GetCurrentPwTime(&tNow);

	DWORD dw = 0;
	DWORD cnt = 0;
	DWORD dwGroupId;
	DWORD dwGroupInx;
	PW_ENTRY *p;
	CString strTemp;
	DWORD dwMaxItems = m_mgr.GetNumberOfEntries();

	// Create the search group if it doesn't exist already
	dwGroupId = m_mgr.GetGroupId(PWS_SEARCHGROUP);
	if(dwGroupId == DWORD_MAX)
	{
		PW_GROUP pwTemplate;
		pwTemplate.pszGroupName = (TCHAR *)PWS_SEARCHGROUP;
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

	DWORD dwBackup1 = m_mgr.GetGroupId(PWS_BACKUPGROUP);
	DWORD dwBackup2 = m_mgr.GetGroupId(PWS_BACKUPGROUP_SRC);

	while(1)
	{
		dw = m_mgr.Find(m_strQuickFind, FALSE, dwFlags, cnt);

		if(dw == DWORD_MAX) break;
		else
		{
			p = m_mgr.GetEntry(dw);
			ASSERT_ENTRY(p);
			if(p == NULL) break;

			if(p->uGroupId != dwGroupId)
			{
				if((m_bQuickFindIncBackup == TRUE) || ((p->uGroupId != dwBackup1) && (p->uGroupId != dwBackup2)))
				{
					dwGroupInx = m_mgr.GetGroupByIdN(p->uGroupId);
					ASSERT(dwGroupInx != DWORD_MAX);

					// The entry could get reallocated by AddEntry, therefore
					// save it to a local CString object
					m_mgr.UnlockEntryPassword(p);
					strTemp = p->pszPassword;
					m_mgr.LockEntryPassword(p);

					_List_SetEntry(m_cList.GetItemCount(), p, TRUE, &tNow);

					EraseCString(&strTemp); // Destroy the plaintext password
				}
			}
		}

		cnt = dw + 1;
		if((DWORD)cnt >= dwMaxItems) break;
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

	// m_bModified = TRUE;
	ShowEntryDetails(NULL);
	_UpdateToolBar();
}

inline void CPwSafeDlg::NotifyUserActivity()
{
	m_nLockCountdown = m_nLockTimeDef;
}

void CPwSafeDlg::OnSafeExportGroupTxt()
{
	NotifyUserActivity();
	ExportSelectedGroup(PWEXP_TXT);
}

void CPwSafeDlg::OnUpdateSafeExportGroupTxt(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(((m_hLastSelectedGroup != NULL) ? TRUE : FALSE));
}

void CPwSafeDlg::OnPwlistSelectAll()
{
	NotifyUserActivity();

	int i;

	ASSERT(m_bFileOpen == TRUE); if(m_bFileOpen == FALSE) return;

	m_cList.SetFocus();
	for(i = 0; i < m_cList.GetItemCount(); i++)
		m_cList.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);

	_UpdateToolBar(TRUE);
}

void CPwSafeDlg::OnUpdatePwlistSelectAll(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::UpdateAutoSortMenuItems()
{
	int i;
	UINT uCmdID = ID_VIEW_AUTOSORT_NOSORT;

	for(i = 0; i < 10; i++)
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
	_SortList((DWORD)(m_nAutoSort - 1), TRUE);
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
	int i;
	LV_ITEM lvi;
	TCHAR szTemp[66];
	BYTE aUuid[16];

	ZeroMemory(&lvi, sizeof(LV_ITEM));
	lvi.iSubItem = 9;
	lvi.cchTextMax = 65;
	lvi.pszText = szTemp;
	lvi.mask = LVIF_TEXT;

	for(i = 0; i < m_cList.GetItemCount(); i++)
	{
		lvi.iItem = i;
		lvi.pszText = szTemp;
		m_cList.GetItem(&lvi);

		_StringToUuid(lvi.pszText, aUuid);
		if(memcmp(aUuid, pUuid, 16) == 0) return (DWORD)i;
	}

	return DWORD_MAX;
}

void CPwSafeDlg::OnUpdateSafeOptions(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(((m_bLocked == FALSE) ? TRUE : FALSE) && (!m_bDisplayDialog));
}

void CPwSafeDlg::_AutoType(PW_ENTRY *pEntry, BOOL bLoseFocus)
{
	CString str;
	BOOL bRefreshView = FALSE;

	ASSERT_ENTRY(pEntry); if(pEntry == NULL) return;

	if(m_bDisableAutoType == TRUE) return;

	BlockInput(TRUE);

	str = ExtractParameterFromString(pEntry->pszAdditional, _T("auto-type:"), 0);
	if(str.GetLength() == 0)
	{
		if(_tcscmp(pEntry->pszTitle, PWS_TAN_ENTRY) == 0) // Is it a TAN entry?
		{
			str = _T("{PASSWORD}");

			_GetCurrentPwTime(&pEntry->tExpire);
			m_bModified = TRUE;
			bRefreshView = TRUE;
		}
		else str = _T("{USERNAME}{TAB}{PASSWORD}{ENTER}");
	}

	m_mgr.UnlockEntryPassword(pEntry);
	ParseURL(&str, pEntry, TRUE);
	m_mgr.LockEntryPassword(pEntry);

	if(bLoseFocus == TRUE)
	{
		if(m_nAutoTypeMethod == ATM_DROPBACK)
		{
			HWND hWnd = ::GetWindow(this->m_hWnd, GW_HWNDFIRST);
			LONG lStyle;
			WINDOWPLACEMENT wp;

			ZeroMemory(&wp, sizeof(WINDOWPLACEMENT));
			wp.length = sizeof(WINDOWPLACEMENT);

			while(1)
			{
				if(hWnd != this->m_hWnd)
				{
					lStyle = ::GetWindowLong(hWnd, GWL_STYLE);
					::GetWindowPlacement(hWnd, &wp);

					if(((lStyle & WS_VISIBLE) == WS_VISIBLE) && (wp.showCmd != SW_SHOWMINIMIZED))
					{
						// TCHAR tszWindowBuf[256];
						// ::GetWindowText(hWnd, tszWindowBuf, 255);
						// if(tszWindowBuf[0] != 0) break;

						if(::GetWindowTextLength(hWnd) != 0) break;
					}
				}

				hWnd = ::GetWindow(hWnd, GW_HWNDNEXT);
				if(hWnd == NULL) break;
			}

			SetWindowPos(&CWnd::wndBottom, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
			if(hWnd != NULL) ::SetForegroundWindow(hWnd);
		}
		else // m_nAutoTypeMethod == ATM_MINIMIZE
		{
			HWND hMe = ::GetForegroundWindow();

			SetWindowPos(&CWnd::wndBottom, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);

			if(m_bMinimized == FALSE)
			{
				if(m_bMinimizeToTray == FALSE)
				{
					ShowWindow(SW_MINIMIZE);
					m_bMinimized = TRUE;
				}
				else if(m_bShowWindow == TRUE)
				{
					ShowWindow(SW_MINIMIZE);
					ViewHideHandler();
				}
			}

			HWND h;
			DWORD dwStartTime = timeGetTime();

			while(1)
			{
				h = ::GetForegroundWindow();
				if(h != hMe) break;

				if((timeGetTime() - dwStartTime) > 750) break;
			}
		}
	}

	Sleep(50);

	CSendKeys sk;
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

	BlockInput(FALSE);

	if((bLoseFocus == TRUE) && (m_bAlwaysOnTop == TRUE))
		SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);

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

	p = m_mgr.GetEntry(dwEntry);
	ASSERT(p != NULL); if(p == NULL) return;

	_AutoType(p, TRUE);
}

void CPwSafeDlg::OnUpdatePwlistAutoType(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((((m_dwLastFirstSelectedItem != DWORD_MAX) && (m_dwLastNumSelectedItems == 1)) ? TRUE : FALSE)
		&& (!m_bDisableAutoType));
}

void CPwSafeDlg::OnExtrasPluginMgr()
{
	NotifyUserActivity();

	CPluginsDlg dlg;

	m_bDisplayDialog = TRUE;

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
	CPluginManager::Instance().CallPlugins(KPM_DIRECT_EXEC, (LPARAM)nID, 0);
}

void CPwSafeDlg::OnUpdatePluginMessage(CCmdUI* pCmdUI)
{
	int i, j;
	KP_MENU_ITEM *pMenuItem;

	for(i = 0; i < (int)CPluginManager::Instance().m_plugins.size(); i++)
	{
		for(j = 0; j < (int)CPluginManager::Instance().m_plugins[i].info.dwNumCommands; j++)
		{
			pMenuItem = &CPluginManager::Instance().m_plugins[i].info.pMenuItems[j];

			if(pMenuItem->dwCommandID == (DWORD)pCmdUI->m_nID)
			{
				pCmdUI->Enable(((pMenuItem->dwFlags & KPMIF_DISABLED) > 0) ? FALSE : TRUE);

				if(pMenuItem->dwFlags & KPMIF_CHECKBOX)
					pCmdUI->SetCheck((pMenuItem->dwState & KPMIS_CHECKED) ? TRUE : FALSE);

				return;
			}
		}
	}
}

void CPwSafeDlg::BuildPluginMenu()
{
	unsigned int i, j;
	KP_MENU_ITEM *p;
	UINT uFlags;
	std::vector<BCMenu *> ptrs;
	BCMenu *psub = (BCMenu *)m_menu.GetSubMenu((TCHAR *)TRL("&Tools"));
	CString strItem;
	BOOL bMakeSeparator = TRUE;

	ASSERT(psub != NULL); if(psub == NULL) return;

	ptrs.push_back(psub);

	for(i = 0; i < (unsigned int)CPluginManager::Instance().m_plugins.size(); i++)
	{
		if((CPluginManager::Instance().m_plugins[i].hinstDLL == NULL) || (CPluginManager::Instance().m_plugins[i].bEnabled == FALSE)) continue;

		for(j = 0; j < (unsigned int)CPluginManager::Instance().m_plugins[i].info.dwNumCommands; j++)
		{
			psub = ptrs[ptrs.size() - 1];
			p = &CPluginManager::Instance().m_plugins[i].info.pMenuItems[j];

			uFlags = MF_STRING | MF_OWNERDRAW;

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
			else
			{
			}

			if((p->dwFlags & KPMIF_DISABLED) > 0)
				uFlags |= MF_GRAYED;
			else
				uFlags |= MF_ENABLED;

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
	BOOL b;

	if(((dwHotKey >> 16) & HOTKEYF_ALT) > 0) uModifiers |= MOD_ALT;
	if(((dwHotKey >> 16) & HOTKEYF_SHIFT) > 0) uModifiers |= MOD_SHIFT;
	if(((dwHotKey >> 16) & HOTKEYF_CONTROL) > 0) uModifiers |= MOD_CONTROL;

	if(bReleasePrevious == TRUE) UnregisterHotKey(this->m_hWnd, nHotKeyID);

	if(dwHotKey == 0) return TRUE; // Nothing to register

	b = RegisterHotKey(this->m_hWnd, nHotKeyID, uModifiers, (UINT)(dwHotKey & 0x0000FFFF));

	if(b == FALSE)
	{
		CString str;

		str = TRL("Cannot register the global hot key.");

		if(bMessageBoxOnFail == TRUE)
		{
			str += _T("\r\n\r\n");
			str += TRL("Most probably another application has reserved this hot key already."); str += _T("\r\n\r\n");
			str += TRL("You can choose an other hot key combination in 'Edit - Options - Setup'.");

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

	if(m_bDisableAutoType == TRUE) return 0;

	ASSERT((wParam == HOTKEYID_AUTOTYPE) || (wParam == HOTKEYID_RESTORE));
	if(wParam == HOTKEYID_AUTOTYPE)
	{
		NotifyUserActivity();

		if(m_bLocked == TRUE)
		{
			HWND hWndSave = ::GetForegroundWindow(); // Save currently active window

			SetForegroundWindow();
			OnFileLock();

			if((hWndSave == NULL) || (hWndSave == this->m_hWnd)) return 0;
			::SetForegroundWindow(hWndSave);
			Sleep(10);

			if(m_bLocked == TRUE) return 0;
		}
		if(m_bFileOpen == FALSE) return 0;

		HWND hWnd = ::GetForegroundWindow();
		int nLen = ::GetWindowTextLength(hWnd);
		if((nLen == 0) || (nLen == -1)) return 0;

		TCHAR *pszWindow = new TCHAR[nLen + 3];
		ASSERT(pszWindow != NULL); if(pszWindow == NULL) return 0;

		m_bDisplayDialog = TRUE;

		::GetWindowText(hWnd, pszWindow, nLen + 2);

		DWORD i;
		PW_ENTRY *pe = NULL;
		CString strCurWindow = pszWindow;
		CString strWindowExp;
		bool bLeft, bRight;
		int nSubLen;
		DWORD dwWindowField;
		CEntryListDlg dlg;
		PW_UUID_STRUCT pwUuid;

		dlg.m_nDisplayMode = ELDMODE_LIST;
		dlg.m_pMgr = &m_mgr;
		dlg.m_pImgList = &m_ilIcons;
		dlg.m_bPasswordStars = m_bPasswordStars;
		dlg.m_bUserStars = m_bUserStars;
		ZeroMemory(dlg.m_aUuid, 16);
		dlg.m_strBannerTitle = TRL("Auto-Type Entry Selection");
		dlg.m_strBannerCaption = TRL("Multiple entries exist for the current window. Please select the entry to auto-type.");

		DWORD dwInvalidId1 = m_mgr.GetGroupId(PWS_BACKUPGROUP_SRC);
		DWORD dwInvalidId2 = m_mgr.GetGroupId(PWS_BACKUPGROUP);
		strCurWindow.MakeLower();

		for(i = 0; i < m_mgr.GetNumberOfEntries(); i++)
		{
			pe = m_mgr.GetEntry(i); ASSERT(pe != NULL); if(pe == NULL) continue;

			if((pe->uGroupId == dwInvalidId1) || (pe->uGroupId == dwInvalidId2)) continue;

			dwWindowField = 0;
			while(1) // Search all auto-type-window strings
			{
				strWindowExp = ExtractParameterFromString(pe->pszAdditional, _T("auto-type-window:"), dwWindowField);
				strWindowExp.MakeLower();

				if(strWindowExp.GetLength() != 0) // An auto-type-window definition has been found
				{
					bLeft = (strWindowExp.Left(1) == _T("*")) ? true : false;
					bRight = (strWindowExp.Right(1) == _T("*")) ? true : false;

					if(bLeft) strWindowExp.Delete(0, 1);
					if(bRight) strWindowExp.Delete(strWindowExp.GetLength() - 1, 1);

					nSubLen = strWindowExp.GetLength();

					if(bLeft && bRight && (nSubLen <= nLen))
					{
						if(strCurWindow.Find(strWindowExp, 0) != -1)
							{ memcpy(pwUuid.uuid, pe->uuid, 16); dlg.m_vEntryList.push_back(pwUuid); }
					}
					else if(bLeft && (nSubLen <= nLen))
					{
						if(strCurWindow.Right(nSubLen) == strWindowExp)
							{ memcpy(pwUuid.uuid, pe->uuid, 16); dlg.m_vEntryList.push_back(pwUuid); }
					}
					else if(bRight && (nSubLen <= nLen))
					{
						if(strCurWindow.Left(nSubLen) == strWindowExp)
							{ memcpy(pwUuid.uuid, pe->uuid, 16); dlg.m_vEntryList.push_back(pwUuid); }
					}
					else if(nSubLen == nLen)
					{
						if(strCurWindow == strWindowExp)
							{ memcpy(pwUuid.uuid, pe->uuid, 16); dlg.m_vEntryList.push_back(pwUuid); }
					}
				}
				else if(dwWindowField == 0) // Entry doesn't have an auto-type-window definition
				{
					strWindowExp = pe->pszTitle;
					nSubLen = strWindowExp.GetLength();

					if((nSubLen != 0) && (nSubLen <= nLen))
					{
						strWindowExp.MakeLower();
						if(strCurWindow.Find(strWindowExp, 0) != -1)
						{
							memcpy(pwUuid.uuid, pe->uuid, 16);
							dlg.m_vEntryList.push_back(pwUuid);
						}
					}

					break;
				}
				else break;

				dwWindowField++;
			}
		}

		DWORD dwMatchingEntriesCount = (DWORD)dlg.m_vEntryList.size();
		if(dwMatchingEntriesCount != 0)
		{
			if(dwMatchingEntriesCount == 1)
				pe = m_mgr.GetEntryByUuid(dlg.m_vEntryList[0].uuid);
			else
			{
				SetForegroundWindow();
				dlg.DoModal();

				// Check if the target window still exists / is valid
				if(::IsWindow(hWnd) == TRUE)
				{
					VERIFY(::SetForegroundWindow(hWnd) == TRUE); // Restore window focus

					if(CPwManager::IsZeroUUID(dlg.m_aUuid) == FALSE)
						pe = m_mgr.GetEntryByUuid(dlg.m_aUuid);
					else pe = NULL;
				}
				else pe = NULL;
			}

			if(pe != NULL) _AutoType(pe, FALSE);
		}

		dlg.m_vEntryList.clear();
		SAFE_DELETE_ARRAY(pszWindow);

		m_bDisplayDialog = FALSE;
	}
	else if(wParam == HOTKEYID_RESTORE)
	{
		NotifyUserActivity();
		SetForegroundWindow();
		if((m_bMinimized == TRUE) || (m_bShowWindow == FALSE)) ViewHideHandler();
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

	DWORD dwIndex = _ListSelToEntryIndex();
	PW_ENTRY *p;
	CString str;

	if(dwIndex == DWORD_MAX) return;
	p = m_mgr.GetEntry(dwIndex);
	ASSERT_ENTRY(p); if(p == NULL) return;

	str = TRL("Field has been pasted."); str += _T(" ");
	str += TRL("Clipboard cleared.");

	// CDialog::OnRenderFormat(nFormat);
	switch(m_nClipboardState)
	{
		case CLIPBOARD_DELAYED_USERNAME:
			m_nClipboardState = CLIPBOARD_DELAYED_NONE;
			CopyDelayRenderedClipboardData(p->pszUserName);
			m_nClipboardState = CLIPBOARD_DELAYED_NONE;
			SetStatusTextEx(str);
			PostMessage(WM_MY_UPDATECLIPBOARD);
			break;

		case CLIPBOARD_DELAYED_PASSWORD:
			m_nClipboardState = CLIPBOARD_DELAYED_NONE;
			m_mgr.UnlockEntryPassword(p);
			CopyDelayRenderedClipboardData(p->pszPassword);
			m_mgr.LockEntryPassword(p);
			m_nClipboardState = CLIPBOARD_DELAYED_NONE;
			SetStatusTextEx(str);
			PostMessage(WM_MY_UPDATECLIPBOARD);
			break;

		case CLIPBOARD_DELAYED_NONE:
			CopyDelayRenderedClipboardData(_T(""));
			{
				CString str2;
				str2 = TRL("Paste operation completed."); str2 += _T(" ");
				str2 += TRL("Clipboard cleared.");
				SetStatusTextEx(str2);
			}
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
	CopyDelayRenderedClipboardData(_T(""));
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
	return TRUE;
}

void CPwSafeDlg::OnEndSession(BOOL bEnding)
{
	if(bEnding == TRUE)
	{
		SaveOptions();
		OnFileExit();
	}

	CWnd::OnEndSession(bEnding);
}

void CPwSafeDlg::OnImportGetMore()
{
	ShellExecute(NULL, _T("open"), PWM_URL_PLUGINS, NULL, NULL, SW_SHOW);
}

BOOL CPwSafeDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	DWORD dwLen, dwPasswordLen, dwKeyFileLen;
	CString strData, strPassword, strKeyFile, strFile;

	UNREFERENCED_PARAMETER(pWnd);

	NotifyUserActivity();
	if(pCopyDataStruct == NULL) return TRUE;

	if(m_bDisplayDialog == TRUE) return TRUE; // Do nothing if a modal window is displayed

	// if(pCopyDataStruct->dwData == 0xF0FFFFE0)
	// {
	//	if(m_bFileOpen == FALSE) return TRUE;
	//	m_remObj.ProcessConnectionRequest((LPCTSTR)pCopyDataStruct->lpData);
	//	return TRUE;
	// }

	if(pCopyDataStruct->lpData != NULL)
	{
		dwLen = (DWORD)_tcslen((LPCTSTR)pCopyDataStruct->lpData);
		if((dwLen == 0) && (pCopyDataStruct->dwData == 0xF0FFFFF0))
		{
			if((m_bMinimized == TRUE) || (m_bShowWindow == FALSE)) ViewHideHandler();
			return TRUE;
		}
		else if(dwLen == 0) return TRUE;

		strData = (LPCTSTR)pCopyDataStruct->lpData;
		dwPasswordLen = pCopyDataStruct->dwData >> 16;
		dwKeyFileLen = pCopyDataStruct->dwData & 0xFFFF;

		if((dwPasswordLen + dwKeyFileLen) >= dwLen) return TRUE;

		if(dwPasswordLen != 0) strPassword = strData.Mid(0, dwPasswordLen);
		if(dwKeyFileLen != 0) strKeyFile = strData.Mid(dwPasswordLen, dwKeyFileLen);

		if((dwPasswordLen + dwKeyFileLen) != 0)
			strFile = strData.Right(strData.GetLength() - (int)dwPasswordLen - (int)dwKeyFileLen);
		else strFile = strData;

		if(strFile.GetLength() == 0) return TRUE;

		if(m_strFile.GetLength() != 0)
		{
			CString strThis = GetShortestAbsolutePath(m_strFile);
			CString strNew = GetShortestAbsolutePath(strFile);

			strThis.MakeLower();
			strNew.MakeLower();

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

		if((m_bMinimized == TRUE) || (m_bShowWindow == FALSE)) ViewHideHandler();
	}

	return TRUE;
}

void CPwSafeDlg::OnInfoDonate()
{
	NotifyUserActivity();
	ShellExecute(NULL, _T("open"), PWM_URL_DONATE, NULL, NULL, SW_SHOW);
}

void CPwSafeDlg::OnExtrasRepairDb()
{
	NotifyUserActivity();

	if(m_bLocked == TRUE) return;

	m_bDisplayDialog = TRUE;

	if(_CALLPLUGINS(KPM_FILE_OPEN_PRE, 0, 0) == FALSE)
		{ m_bDisplayDialog = FALSE; return; }

	_OpenDatabase(NULL, NULL, NULL, NULL, FALSE, NULL, TRUE);
	_UpdateToolBar();
	m_cGroups.SetFocus();
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
	str.TrimRight(_T(':'));
	str += _T("\r\n(");
	str += TRL("Press Shift-Home or Shift-End to clear this edit control");
	str += _T(")");
	return str;
}

void CPwSafeDlg::_UpdateTitleBar()
{
	CString strFull = PWM_PRODUCT_NAME, strShort = PWM_PRODUCT_NAME_SHORT;
	CString str, strRef;

	if(m_bLocked == TRUE)
	{
		str = _T(" - [");
		str += TRL("Workspace locked");
		str += _T("]");

		strFull += str; strShort += str;
	}
	else if(m_bFileOpen == TRUE)
	{
		str = _T(" - [");

		if(m_bShowFullPath == FALSE) str += CsFileOnly(&m_strFile);
		else str += m_strFileAbsolute;

		if(m_bModified == TRUE) str += _T(" *");

		str += _T("]");

		strFull += str; strShort += str;
	}

	this->GetWindowText(strRef);
	if(strRef != strFull)
	{
		this->SetWindowText(strFull);

		// NOTIFYICONDATA only supports tooltip texts up to 63 characters
		if(strShort.GetLength() > 63) strShort = strShort.Left(63);
		m_systray.SetTooltipText(strShort);
	}
}

void CPwSafeDlg::OnImportKeePass()
{
	CImportOptionsDlg dlg;
	CPwManager mgrImport;

	NotifyUserActivity();

	_OpenDatabase(&mgrImport, NULL, NULL, NULL, FALSE, NULL, FALSE);

	m_bDisplayDialog = TRUE;

	if(mgrImport.GetNumberOfGroups() != 0)
	{
		_Groups_SaveView(TRUE);
		_List_SaveView();

		if(dlg.DoModal() == IDCANCEL) { m_bDisplayDialog = FALSE; return; }

		BOOL bNewIDs = (dlg.m_nImportMethod == 0) ? TRUE : FALSE;
		BOOL bCompareTimes = (dlg.m_nImportMethod == 2) ? TRUE : FALSE;
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
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::ExportGroupToKeePass(DWORD dwGroupId)
{
	CPwManager pwTempMgr;
	CPwExport cExp;

	NotifyUserActivity();

	if(_IsUnsafeAllowed() == FALSE) return;

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
	int nUse = MessageBox(strText, TRL("Password Safe"), MB_ICONQUESTION | MB_YESNO);
	if(nUse == IDNO)
	{
		if(_ChangeMasterKey(&pwTempMgr, TRUE) != TRUE) return;
	}
	else
	{
		BYTE aMasterKey[32]; // Copy the master key
		m_mgr.GetRawMasterKey(aMasterKey);
		pwTempMgr.SetRawMasterKey(aMasterKey);
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

	pwTempMgr.SetRawMasterKey(NULL);
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
	pCmdUI->Enable(((m_hLastSelectedGroup != NULL) ? TRUE : FALSE));
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
	return 0;
}

void CPwSafeDlg::_CalcColumnSizes()
{
	RECT rect;
	int nColumnWidth, nColumns = 0, i;

	for(i = 0; i < 11; i++) // Count active columns and reset all widths
	{
		if(m_bShowColumn[i] == TRUE) nColumns++;
		m_nColumnWidths[i] = 0;
	}
	if(nColumns == 0) return;

	m_cList.GetClientRect(&rect); // Calculate and set new widths
	nColumnWidth = ((rect.right - 9) / nColumns) - (GetSystemMetrics(SM_CXVSCROLL) / nColumns);
	for(i = 0; i < 11; i++)
		if(m_bShowColumn[i] == TRUE)
			m_nColumnWidths[i] = nColumnWidth;
}

void CPwSafeDlg::_SetColumnWidths()
{
	if(m_dwPwListMode == LVS_REPORT)
	{
		for(int i = 0; i < 11; i++)
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
	LPARAM dw;
	dw = LVS_EX_SI_REPORT | LVS_EX_FULLROWSELECT | LVS_EX_ONECLICKACTIVATE | LVS_EX_UNDERLINEHOT;
	dw |= LVS_EX_HEADERDRAGDROP | LVS_EX_INFOTIP;
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

	if((wParam == WTS_SESSION_LOGOFF) || (wParam == WTS_SESSION_LOCK))
		if((m_bFileOpen == TRUE) && (m_bLocked == FALSE))
		{
			SaveOptions();
			OnFileLock();
		}

	return 0;
}

void CPwSafeDlg::OnInfoChkForUpd()
{
	CheckForUpdateAsync(this->m_hWnd, PWM_VERSION_DW, PWM_URL_VERSION, TRUE);
}
