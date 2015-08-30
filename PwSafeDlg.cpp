/*
  Copyright (c) 2003/2004, Dominik Reichl <dominik.reichl@t-online.de>
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

#include "StdAfx.h"
#include <afxadv.h>

#include "PwSafe.h"
#include "PwSafeDlg.h"

#include "PwSafe/PwManager.h"
#include "PwSafe/PwExport.h"
#include "PwSafe/PwImport.h"
#include "Crypto/testimpl.h"
#include "Util/MemUtil.h"
#include "Util/StrUtil.h"
#include "Util/PrivateConfig.h"
#include "NewGUI/TranslateEx.h"
#include "NewGUI/HyperLink.h"

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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_MY_SYSTRAY_NOTIFY (WM_APP+10)

#ifdef _DEBUG
// #define or #undef sample group and entries
#define ___PWSAFE_SAMPLE_DATA
#endif

static char g_pNullString[4] = { 0, 0, 0, 0 };

/////////////////////////////////////////////////////////////////////////////

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	CKCSideBannerWnd m_banner;
	HICON m_hWindowIcon;

	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CHyperLink	m_hlHomepage;
	CShadeButtonST	m_btReadMe;
	CShadeButtonST	m_btLicense;
	CShadeButtonST	m_btOK;
	CString	m_strDbVersion;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnReadmeBtn();
	afx_msg void OnLicenseBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_strDbVersion = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_HLINK_HOMEPAGE, m_hlHomepage);
	DDX_Control(pDX, IDC_README_BTN, m_btReadMe);
	DDX_Control(pDX, IDC_LICENSE_BTN, m_btLicense);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Text(pDX, IDC_STATIC_DBFORMAT, m_strDbVersion);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDC_README_BTN, OnReadmeBtn)
	ON_BN_CLICKED(IDC_LICENSE_BTN, OnLicenseBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

class CPwSafeAppRI : public CNewRandomInterface
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
	: CDialog(CPwSafeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPwSafeDlg)
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_bExiting = FALSE;

	m_hLastSelectedGroup = 0;
	m_dwLastNumSelectedItems = 0;
	m_dwLastFirstSelectedItem = 0;
}

void CPwSafeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPwSafeDlg)
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
	DDX_Control(pDX, IDC_MENULINE, m_stcMenuLine);
	DDX_Control(pDX, IDC_PWLIST, m_cList);
	DDX_Control(pDX, IDC_RE_ENTRYVIEW, m_reEntryView);
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
	ON_COMMAND(ID_INFO_LICENSE, OnInfoLicense)
	ON_COMMAND(ID_INFO_PRINTLICENSE, OnInfoPrintLicense)
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
	ON_COMMAND(ID_VIEW_HIDE, OnViewHide)
	ON_COMMAND(ID_IMPORT_CSV, OnImportCsv)
	ON_UPDATE_COMMAND_UI(ID_IMPORT_CSV, OnUpdateImportCsv)
	ON_NOTIFY(NM_CLICK, IDC_PWLIST, OnClickPwlist)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_PWLIST, OnColumnClickPwlist)
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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CString str;

	NewGUI_Button(&m_btOK, IDB_OK, IDB_OK);
	NewGUI_Button(&m_btReadMe, IDB_DOCUMENT_SMALL, IDB_DOCUMENT_SMALL);
	NewGUI_Button(&m_btLicense, IDB_DOCUMENT_SMALL, IDB_DOCUMENT_SMALL);

	m_strDbVersion.Format(TRL("%u.%u.%u.%u, compatible with %u.%u.x.x"),
		PWM_DBVER_DW >> 24, (PWM_DBVER_DW >> 16) & 0xFF, (PWM_DBVER_DW >> 8) & 0xFF,
		PWM_DBVER_DW & 0xFF, PWM_DBVER_DW >> 24, (PWM_DBVER_DW >> 16) & 0xFF);

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	m_banner.Attach(this, KCSB_ATTACH_TOP);

	m_banner.SetColBkg(RGB(255,255,255));
	m_banner.SetColBkg2(NewGUI_GetBgColor());
	m_banner.SetColEdge(RGB(0,0,0));
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);

	str = TRL("Version ");
	str += PWM_VERSION_STR;
	m_banner.SetTitle(PWM_PRODUCT_NAME);
	m_banner.SetCaption((LPCTSTR)str);

	m_hWindowIcon = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(m_hWindowIcon, TRUE);
	SetIcon(m_hWindowIcon, FALSE);

	m_hlHomepage.SetURL(CString(PWM_HOMEPAGE));
	m_hlHomepage.SetVisited(FALSE);
	m_hlHomepage.SetAutoSize(TRUE);
	m_hlHomepage.SetUnderline(CHyperLink::ulAlways);
	m_hlHomepage.SetColours(RGB(0,0,255), RGB(0,0,255), RGB(100,100,255));

	UpdateData(FALSE);

	return TRUE;
}

void CAboutDlg::OnReadmeBtn() 
{
	_OpenLocalFile(PWM_README_FILE, OLF_OPEN);
}

void CAboutDlg::OnLicenseBtn() 
{
	_OpenLocalFile(PWM_LICENSE_FILE, OLF_OPEN);
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

	m_bWindowsNewLine = TRUE;
	m_bLocked = FALSE;
	m_dwOldListParameters = 0;
	m_bMinimized = FALSE;
	m_bMaximized = FALSE;
	m_bCachedToolBarUpdate = FALSE;
	m_bDragging = FALSE;
	m_bDisplayDialog = FALSE;
	m_hDraggingGroup = NULL;

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
	pDest = (BCMenu *)m_menu.GetSubMenu(1);
	pSrc = (BCMenu *)m_popmenu.GetSubMenu(0);
	pDest->AppendMenu(MF_SEPARATOR);
	for(i = 0; i < pSrc->GetMenuItemCount(); i++)
	{
		uID = pSrc->GetMenuItemID(i);
		uState = pSrc->GetMenuState(i, MF_BYPOSITION);
		pSrc->GetMenuText(i, str, MF_BYPOSITION);
		if(str == _T("&Rearrange")) continue;
		// if((uID == ID_GROUP_MOVETOP) || (uID == ID_GROUP_MOVEBOTTOM)) continue;
		// if((uID == ID_GROUP_MOVEUP) || (uID == ID_GROUP_MOVEDOWN)) continue;
		if(uLastID != uID)
			pDest->AppendMenu(uState, uID, (LPCTSTR)str);
		uLastID = uID;
	}
	m_popmenu.DestroyMenu();

	m_popmenu.LoadMenu(IDR_PWLIST_MENU);
	pDest = (BCMenu *)m_menu.GetSubMenu(1);
	pSrc = (BCMenu *)m_popmenu.GetSubMenu(0);
	pDest->AppendMenu(MF_SEPARATOR);
	// The last 5 items are the moving commands, don't append them to the
	// main menu.
	for(i = 0; i < pSrc->GetMenuItemCount() - 2; i++)
	{
		uID = pSrc->GetMenuItemID(i);
		uState = pSrc->GetMenuState(i, MF_BYPOSITION);
		pSrc->GetMenuText(i, str, MF_BYPOSITION);
		if(str == _T("&Rearrange")) continue;
		// if((uID == ID_PWLIST_MOVETOP) || (uID == ID_PWLIST_MOVEBOTTOM)) continue;
		// if((uID == ID_PWLIST_MOVEUP) || (uID == ID_PWLIST_MOVEDOWN)) continue;
		if(uLastID != uID)
			pDest->AppendMenu(uState, uID, (LPCTSTR)str);
		uLastID = uID;
	}
	m_popmenu.DestroyMenu();

	// Load the translation file
	CPrivateConfig cConfig;
	TCHAR szTemp[SI_REGSIZE];
	szTemp[0] = 0; szTemp[1] = 0;
	cConfig.Get(PWMKEY_LANG, szTemp);
	LoadTranslationTable(szTemp);

	cConfig.Get(PWMKEY_LASTDIR, szTemp);
	if(_tcslen(szTemp) != 0)
		SetCurrentDirectory(szTemp);

	cConfig.Get(PWMKEY_CLIPSECS, szTemp);
	if(_tcslen(szTemp) > 0)
	{
		m_dwClipboardSecs = (DWORD)_ttol(szTemp);
		if(m_dwClipboardSecs == 0) m_dwClipboardSecs = 10 + 1;
		if(m_dwClipboardSecs == (DWORD)(-1)) m_dwClipboardSecs = 10 + 1;
	}
	else m_dwClipboardSecs = 10 + 1;

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

	cConfig.Get(PWMKEY_ENTRYGRID, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		if(_tcscmp(szTemp, _T("True")) == 0) m_bEntryGrid = TRUE;
		else m_bEntryGrid = FALSE;
	}
	else m_bEntryGrid = FALSE;

	cConfig.Get(PWMKEY_ALWAYSTOP, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		if(_tcscmp(szTemp, _T("True")) == 0) m_bAlwaysOnTop = TRUE;
		else m_bAlwaysOnTop = FALSE;
	}
	else m_bAlwaysOnTop = FALSE;

	cConfig.Get(PWMKEY_SHOWTITLE, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		if(_tcscmp(szTemp, _T("False")) == 0) m_bShowTitle = FALSE;
		else m_bShowTitle = TRUE;
	}
	else m_bShowTitle = TRUE;

	cConfig.Get(PWMKEY_SHOWUSER, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		if(_tcscmp(szTemp, _T("False")) == 0) m_bShowUserName = FALSE;
		else m_bShowUserName = TRUE;
	}
	else m_bShowUserName = TRUE;

	cConfig.Get(PWMKEY_SHOWURL, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		if(_tcscmp(szTemp, _T("False")) == 0) m_bShowURL = FALSE;
		else m_bShowURL = TRUE;
	}
	else m_bShowURL = TRUE;

	cConfig.Get(PWMKEY_SHOWPASS, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		if(_tcscmp(szTemp, _T("False")) == 0) m_bShowPassword = FALSE;
		else m_bShowPassword = TRUE;
	}
	else m_bShowPassword = TRUE;

	cConfig.Get(PWMKEY_SHOWNOTES, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		if(_tcscmp(szTemp, _T("False")) == 0) m_bShowNotes = FALSE;
		else m_bShowNotes = TRUE;
	}
	else m_bShowNotes = TRUE;

	cConfig.Get(PWMKEY_SHOWCREATION, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		if(_tcscmp(szTemp, _T("True")) == 0) m_bShowCreation = TRUE;
		else m_bShowCreation = FALSE;
	}
	else m_bShowCreation = FALSE;

	cConfig.Get(PWMKEY_SHOWLASTMOD, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		if(_tcscmp(szTemp, _T("True")) == 0) m_bShowLastMod = TRUE;
		else m_bShowLastMod = FALSE;
	}
	else m_bShowLastMod = FALSE;

	cConfig.Get(PWMKEY_SHOWLASTACCESS, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		if(_tcscmp(szTemp, _T("True")) == 0) m_bShowLastAccess = TRUE;
		else m_bShowLastAccess = FALSE;
	}
	else m_bShowLastAccess = FALSE;

	cConfig.Get(PWMKEY_SHOWEXPIRE, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		if(_tcscmp(szTemp, _T("False")) == 0) m_bShowExpire = FALSE;
		else m_bShowExpire = TRUE;
	}
	else m_bShowExpire = TRUE;

	cConfig.Get(PWMKEY_SHOWUUID, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		if(_tcscmp(szTemp, _T("True")) == 0) m_bShowUUID = TRUE;
		else m_bShowUUID = FALSE;
	}
	else m_bShowUUID = FALSE;

	cConfig.Get(PWMKEY_SHOWATTACH, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		if(_tcscmp(szTemp, _T("True")) == 0) m_bShowAttach = TRUE;
		else m_bShowAttach = FALSE;
	}
	else m_bShowAttach = FALSE;

	cConfig.Get(PWMKEY_HIDESTARS, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		if(_tcscmp(szTemp, _T("False")) == 0) m_bPasswordStars = FALSE;
		else m_bPasswordStars = TRUE;
	}
	else m_bPasswordStars = TRUE;

	cConfig.Get(PWMKEY_HIDEUSERS, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		if(_tcscmp(szTemp, _T("True")) == 0) m_bUserStars = TRUE;
		else m_bUserStars = FALSE;
	}
	else m_bUserStars = FALSE;

	cConfig.Get(PWMKEY_LOCKMIN, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		if(_tcscmp(szTemp, _T("True")) == 0) m_bLockOnMinimize = TRUE;
		else m_bLockOnMinimize = FALSE;
	}
	else m_bLockOnMinimize = FALSE;

	cConfig.Get(PWMKEY_MINTRAY, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		if(_tcscmp(szTemp, _T("True")) == 0) m_bMinimizeToTray = TRUE;
		else m_bMinimizeToTray = FALSE;
	}
	else m_bMinimizeToTray = FALSE;

	cConfig.Get(PWMKEY_CLOSEMIN, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		if(_tcscmp(szTemp, _T("True")) == 0) m_bCloseMinimizes = TRUE;
		else m_bCloseMinimizes = FALSE;
	}
	else m_bCloseMinimizes = FALSE;

	cConfig.Get(PWMKEY_ROWCOLOR, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		COLORREF cref = (COLORREF)_ttol(szTemp);
		m_cList.SetColorEx(cref);
	}

	cConfig.Get(PWMKEY_SHOWTOOLBAR, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		if(_tcscmp(szTemp, _T("False")) == 0) m_bShowToolBar = FALSE;
		else m_bShowToolBar = TRUE;
	}
	else m_bShowToolBar = TRUE;

	m_nLockTimeDef = -1;
	cConfig.Get(PWMKEY_LOCKTIMER, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		m_nLockTimeDef = _ttol(szTemp);
	}
	m_nLockCountdown = m_nLockTimeDef;

	cConfig.Get(PWMKEY_ENTRYVIEW, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		if(_tcscmp(szTemp, _T("False")) == 0) m_bEntryView = FALSE;
		else m_bEntryView = TRUE;
	}
	else m_bEntryView = TRUE;

	cConfig.Get(PWMKEY_COLAUTOSIZE, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		if(_tcscmp(szTemp, _T("False")) == 0) m_bColAutoSize = FALSE;
		else m_bColAutoSize = TRUE;
	}
	else m_bColAutoSize = TRUE;

	// Translate the menu
	BCMenu *pSubMenu = &m_menu;
	const TCHAR *pSuffix = _T("");
	CString strItem, strNew;
	int nItem = 0, nSub = 0;
	UINT nID;
	while(1)
	{
		if(pSubMenu->GetMenuString((UINT)nItem, strItem, MF_BYPOSITION) == FALSE) break;
		pSuffix = _GetCmdAccelExt((LPCTSTR)strItem);
		strNew = TRL((LPCTSTR)strItem);
		if(_tcslen(pSuffix) != 0) { strNew += _T("\t"); strNew += pSuffix; }
		nID = pSubMenu->GetMenuItemID(nItem);
		if(pSubMenu->ModifyMenu(nItem, MF_BYPOSITION | MF_STRING, nID, strNew) == FALSE) { ASSERT(FALSE); }
		nItem++;
	}
	pSubMenu = (BCMenu *)m_menu.GetSubMenu(nSub);
	while(1)
	{
		_TranslateMenu(pSubMenu);

		pSubMenu = (BCMenu *)m_menu.GetSubMenu(nSub);
		nSub++;
		if(pSubMenu == NULL) break;
	}

	m_menu.LoadToolbar(IDR_INFOICONS);

	m_bMenu = SetMenu(&m_menu);
	ASSERT(m_bMenu == TRUE);

	RECT rectClient;
	RECT rectSB;
	GetClientRect(&rectClient);
	rectSB.top = rectClient.bottom - rectClient.top - 8;
	rectSB.bottom = rectClient.bottom - rectClient.top;
	rectSB.left = 0;
	rectSB.right = rectClient.right - rectClient.left;
	m_sbStatus.Create(WS_CHILD | WS_VISIBLE | CCS_BOTTOM | SBARS_SIZEGRIP,
		rectSB, this, AFX_IDW_STATUS_BAR);
	m_sbStatus.SetSimple(TRUE);
	m_sbStatus.SetText(TRL("Ready."), 255, 0);

	m_ilIcons.Create(IDR_CLIENTICONS, 16, 1, RGB(255,0,255)); // purple is transparent
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

	_CalcColumnSizes();
	// Windows computes the sizes wrong the first time because there aren't
	// any 16x16 icons in the list yet
	m_cList.InsertColumn(0, TRL("Title"), LVCFMT_LEFT, 10 - 4, 0);
	m_cList.InsertColumn(1, TRL("UserName"), LVCFMT_LEFT, 10 - 4, 1);
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
	ul = testCryptoImpl();
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

	m_strFile.Empty();
	m_bFileOpen = FALSE;
	m_bModified = FALSE;
	m_cList.EnableWindow(FALSE);
	m_cGroups.EnableWindow(FALSE);

	// "Initialize" the xorshift pseudo-random number generator
	CNewRandom nr;
	nr.Initialize();
	DWORD dwSkip, dwPos;
	nr.GetRandomBuffer((BYTE *)&dwSkip, 4);
	dwSkip &= 0x07FF;
	for(dwPos = 0; dwPos < dwSkip; dwPos++) randXorShift();

	str = PWM_PRODUCT_NAME;
	SetWindowText(str);

	ProcessResize();
	UpdateGroupList();
	UpdatePasswordList();

	m_strTempFile.Empty();
	m_bTimer = TRUE;
	m_nClipboardCountdown = -1;
	SetTimer(APPWND_TIMER_ID, 1000, NULL);
	SetTimer(APPWND_TIMER_ID_UPDATER, 500, NULL);

	m_strLastDb.Empty();
	cConfig.Get(PWMKEY_OPENLASTB, szTemp);
	m_bOpenLastDb = FALSE;
	if(_tcsicmp(szTemp, _T("True")) == 0) m_bOpenLastDb = TRUE;

	cConfig.Get(PWMKEY_AUTOSAVEB, szTemp);
	m_bAutoSaveDb = FALSE;
	if(_tcsicmp(szTemp, _T("True")) == 0) m_bAutoSaveDb = TRUE;

	m_reEntryView.LimitText(0);
	m_reEntryView.SetEventMask(ENM_MOUSEEVENTS);
	m_reEntryView.SetBackgroundColor(FALSE, GetSysColor(COLOR_3DFACE));

	m_hTrayIconNormal = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hTrayIconLocked = AfxGetApp()->LoadIcon(IDI_LOCKED);

	m_bShowWindow = TRUE;
	VERIFY(m_systray.Create(this, WM_MY_SYSTRAY_NOTIFY, PWM_PRODUCT_NAME,
		m_hTrayIconNormal, IDR_SYSTRAY_MENU, FALSE,
		NULL, NULL, NIIF_NONE, 0));
	m_systray.SetMenuDefaultItem(0, TRUE);
	m_systray.MoveToRight();

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

	int px, py, dx, dy, idScreenNow, idScreenSaved; // Restore window position
	cConfig.Get(PWMKEY_WINDOWPX, szTemp);
	if(_tcslen(szTemp) > 0) px = _ttoi(szTemp); else px = -1;
	cConfig.Get(PWMKEY_WINDOWPY, szTemp);
	if(_tcslen(szTemp) > 0) py = _ttoi(szTemp); else py = -1;
	cConfig.Get(PWMKEY_WINDOWDX, szTemp);
	if(_tcslen(szTemp) > 0) dx = _ttoi(szTemp); else dx = -1;
	cConfig.Get(PWMKEY_WINDOWDY, szTemp);
	if(_tcslen(szTemp) > 0) dy = _ttoi(szTemp); else dy = -1;
	cConfig.Get(PWMKEY_SCREENID, szTemp);
	if(_tcslen(szTemp) > 0) idScreenSaved = _ttoi(szTemp); else idScreenSaved = -1;

	idScreenNow = GetSystemMetrics(SM_CXSCREEN) ^ (GetSystemMetrics(SM_CYSCREEN) << 12);

	m_lSplitterPosHoriz = GUI_GROUPLIST_EXT + 1;
	m_lSplitterPosVert = ((rectClient.bottom - rectClient.top - PWS_DEFAULT_SPLITTER_Y) * 3) >> 2;
	ASSERT(m_lSplitterPosVert > 0);
	if((px != -1) && (py != -1) && (dx != -1) && (dy != -1) && (idScreenNow == idScreenSaved))
	{
		SetWindowPos(&wndNoTopMost, px, py, dx, dy, SWP_NOOWNERZORDER | SWP_NOZORDER);

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
		if(_tcslen(szTemp) != 0)
		{
			m_lSplitterPosHoriz = _ttol(szTemp);
		} else m_lSplitterPosHoriz = GUI_GROUPLIST_EXT + 1;
		cConfig.Get(PWMKEY_SPLITTERY, szTemp);
		if(_tcslen(szTemp) != 0)
		{
			m_lSplitterPosVert = _ttol(szTemp);
		} else m_lSplitterPosVert = PWS_DEFAULT_SPLITTER_Y;
	}

	if(_ParseCommandLine() == FALSE)
	{
		if(m_bOpenLastDb == TRUE)
		{
			cConfig.Get(PWMKEY_LASTDB, szTemp);
			if(_tcslen(szTemp) != 0)
			{
				_OpenDatabase(szTemp);
			}
		}
	}

	_UpdateToolBar();
	ProcessResize();

	// PostMessage(WM_NULL, 0, 0);

	return TRUE;
}

void CPwSafeDlg::_TranslateMenu(BCMenu *pBCMenu)
{
	CString strItem, strNew;
	UINT nItem = 0;
	BCMenu *pNext;
	const TCHAR *pSuffix = _T("");

	ASSERT(pBCMenu != NULL);
	if(pBCMenu == NULL) return;

	while(1)
	{
		if(pBCMenu->GetMenuText((UINT)nItem, strItem, MF_BYPOSITION) == FALSE) break;
		pSuffix = _GetCmdAccelExt((LPCTSTR)strItem);

		if((strItem == _T("Export Complete Database")) || (strItem == _T("Import To Database")) ||
			(strItem == _T("Show &Columns")) || (strItem == _T("&Rearrange")))
		{
			pNext = pBCMenu->GetSubBCMenu((TCHAR *)(LPCTSTR)strItem);
			if(pNext != NULL) _TranslateMenu(pNext);
		}

		strNew = TRL((LPCTSTR)strItem);
		if(_tcslen(pSuffix) != 0) { strNew += _T("\t"); strNew += pSuffix; }
		if(pBCMenu->SetMenuText(nItem, strNew, MF_BYPOSITION) == FALSE) { ASSERT(FALSE); }

		nItem++;
	}
}

BOOL CPwSafeDlg::_ParseCommandLine()
{
	CString str;
	long i;

	str.Empty();
	if(__argc <= 1) return FALSE;

	str = __argv[1];
	if(__argc != 2)
	{
		for(i = 2; i < __argc; i++)
		{
			str += _T(" ");
			str += __argv[i];
		}
	}

	str.TrimLeft(); str.TrimRight();

	if((str == _T("/?")) || (str == _T("-?")))
	{
		str = TRL("KeePass usage:");
		str += _T("\r\n\r\n");
		str += "KeePass.exe [<db>]";
		str += _T("\r\n\r\n");
		str += TRL("db = Path to database you wish to open on KeePass startup");
		MessageBox(str, TRL("Password Safe"), MB_OK | MB_ICONINFORMATION);
		return TRUE;
	}

	if(str.GetLength() == 0) return FALSE;
	if(str.Left(1) == _T("\"")) str = str.Right(str.GetLength() - 1);
	if(str.GetLength() == 0) return FALSE;
	str.TrimLeft(); str.TrimRight();
	if(str.GetLength() == 0) return FALSE;
	if(str.Right(1) == _T("\"")) str = str.Left(str.GetLength() - 1);
	if(str.GetLength() == 0) return FALSE;
	str.TrimLeft(); str.TrimRight();
	if(str.GetLength() == 0) return FALSE;

	_OpenDatabase((LPCTSTR)str);
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
		nSize = _ttoi((LPCTSTR)strSize);
		int nWeight = FW_NORMAL;
		if(strFlags.GetAt(0) == _T('1')) nWeight = FW_BOLD;
		BYTE bItalic = (BYTE)((strFlags.GetAt(1) == _T('1')) ? TRUE : FALSE);
		BYTE bUnderlined = (BYTE)((strFlags.GetAt(2) == _T('1')) ? TRUE : FALSE);
		BYTE bStrikeOut = (BYTE)((strFlags.GetAt(3) == _T('1')) ? TRUE : FALSE);

		if(nSize < 0) nSize = -nSize;
		if(strFace.GetLength() >= 32) strFace = strFace.Left(31);

		lf.lfCharSet = ANSI_CHARSET; lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lf.lfEscapement = 0; lf.lfItalic = bItalic; lf.lfOrientation = 0;
		lf.lfOutPrecision = OUT_DEFAULT_PRECIS; lf.lfQuality = DEFAULT_QUALITY;
		lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
		lf.lfStrikeOut = bStrikeOut; lf.lfUnderline = bUnderlined;
		lf.lfWeight = nWeight; lf.lfWidth = 0;
		_tcscpy(lf.lfFaceName, (LPCTSTR)strFace);
		lf.lfHeight = -MulDiv(nSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);

		m_fListFont.CreateFontIndirect(&lf);
	}
	else
	{
		nSize = 8;
		m_fListFont.CreateFont(-nSize, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, _T("MS Sans Serif"));
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

	ASSERT(m_fListFont.m_hObject != NULL);
	m_cGroups.SetFont(&m_fListFont, TRUE);
	m_cList.SetFont(&m_fListFont, TRUE);

	ReleaseDC(pDC);
}

// This should be replaced by a function that scans the accelerator table
const TCHAR *CPwSafeDlg::_GetCmdAccelExt(const TCHAR *psz)
{
	const TCHAR *pEmpty = _T("");

	ASSERT(psz != NULL);
	if(psz == NULL) return pEmpty;

	if(_tcsicmp(psz, _T("&New Database...")) == 0) return _T("Ctrl+N");
	if(_tcsicmp(psz, _T("&Open Database...")) == 0) return _T("Ctrl+O");
	if(_tcsicmp(psz, _T("&Save Database")) == 0) return _T("Ctrl+S");
	if(_tcsicmp(psz, _T("&Print Complete Password List")) == 0) return _T("Ctrl+P");
	if(_tcsicmp(psz, _T("&Lock Workspace")) == 0) return _T("Ctrl+L");
	if(_tcsicmp(psz, _T("&Exit")) == 0) return _T("Ctrl+X");

	if(_tcsicmp(psz, _T("&Add Entry...")) == 0) return _T("Ctrl+A");
	if(_tcsicmp(psz, _T("&Edit/View Entry...")) == 0) return _T("Ctrl+E");
	if(_tcsicmp(psz, _T("&Delete Entry")) == 0) return _T("Ctrl+D");
	if(_tcsicmp(psz, _T("&Find In Database...")) == 0) return _T("Ctrl+F");
	if(_tcsicmp(psz, _T("&Add Password Group...")) == 0) return _T("Ctrl+G");
	if(_tcsicmp(psz, _T("Open &URL")) == 0) return _T("Ctrl+U");
	if(_tcsicmp(psz, _T("Copy &Password To Clipboard")) == 0) return _T("Ctrl+C");
	if(_tcsicmp(psz, _T("Copy User&Name To Clipboard")) == 0) return _T("Ctrl+B");
	if(_tcsicmp(psz, _T("Dupli&cate Entry")) == 0) return _T("Ctrl+K");
	if(_tcsicmp(psz, _T("&Close Database")) == 0) return _T("Ctrl+Q");
	if(_tcsicmp(psz, _T("&Options")) == 0) return _T("Ctrl+M");

	if(_tcsicmp(psz, _T("Password &Generator")) == 0) return _T("Ctrl+Z");

	if(_tcsicmp(psz, _T("Move Entry To &Top")) == 0) return _T("Alt+Home");
	if(_tcsicmp(psz, _T("Move Entry &One Up")) == 0) return _T("Alt+Up");
	if(_tcsicmp(psz, _T("Mo&ve Entry One Down")) == 0) return _T("Alt+Down");
	if(_tcsicmp(psz, _T("Move Entry To &Bottom")) == 0) return _T("Alt+End");

	if(_tcsicmp(psz, _T("Move Group To &Top")) == 0) return _T("Shift+Alt+Home");
	if(_tcsicmp(psz, _T("Move Group One &Up")) == 0) return _T("Shift+Alt+Up");
	if(_tcsicmp(psz, _T("Move Group &One Down")) == 0) return _T("Shift+Alt+Down");
	if(_tcsicmp(psz, _T("Move Group To &Bottom")) == 0) return _T("Shift+Alt+End");
	if(_tcsicmp(psz, _T("Move Group One &Left")) == 0) return _T("Shift+Alt+Left");
	if(_tcsicmp(psz, _T("Move Group One &Right")) == 0) return _T("Shift+Alt+Right");

	return pEmpty;
}

void CPwSafeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	BOOL m_bRestore = FALSE;

	if(m_bDisplayDialog == TRUE) return; // No dialog must be displayed at this time

	// Map close button to minimize button if the user wants this
	if((nID == SC_CLOSE) && (m_bCloseMinimizes == TRUE)) nID = SC_MINIMIZE;

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
			m_bMaximized = TRUE;
		}
		else if((nID == SC_MINIMIZE) || (nID == SC_RESTORE))
		{
			if(((nID == SC_MINIMIZE) && (m_bLocked == FALSE)) || (m_bMinimized == TRUE))
			{
				if(m_bLockOnMinimize == TRUE)
				{
					OnFileLock(); // Lock or unlock, toggle lock state

					// Was the locking successful? If not: restore window
					if((m_bFileOpen == TRUE) && (m_bLocked == FALSE) && (nID == SC_MINIMIZE)) m_bRestore = TRUE;
				}
			}

			m_bMinimized = (nID == SC_MINIMIZE) ? TRUE : FALSE;
			m_bMaximized = FALSE;
		}
	}

	_UpdateToolBar();

	if(m_bRestore == TRUE) SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
}

void CPwSafeDlg::OnPaint() 
{
	if (IsIconic())
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
	return (HCURSOR) m_hIcon;
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
	m_dwLastFirstSelectedItem = GetSelectedEntry();
	m_dwLastNumSelectedItems = GetSelectedEntriesCount();
	m_hLastSelectedGroup = m_cGroups.GetSelectedItem();

	CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	OnUpdateFlush(pPopupMenu);

	if(!bSysMenu)
	{
		if(BCMenu::IsMenu(pPopupMenu)) BCMenu::UpdateMenu(pPopupMenu);
	}
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
	if (AfxGetThreadState()->m_hTrackingMenu == pMenu->m_hMenu)
		state.m_pParentMenu = pMenu;    // parent == child for tracking popup
	else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
	{
		CWnd* pParent = GetTopLevelParent();
		// child windows don't have menus -- need to go to the top!
		if (pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
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
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
		state.m_nIndex++)
	{
		state.m_nID = pMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // menu separator or invalid cmd - ignore it

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// possibly a popup menu, route to first item of that popup
			state.m_pSubMenu = pMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
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
	RECT rectClient;
	RECT rectList;
	int cyMenu = GetSystemMetrics(SM_CYMENU);
	LONG nAddTop;

	if(m_bShowToolBar == TRUE) nAddTop = 26;
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

	if(IsWindow(m_cList.m_hWnd)) // Resize password list box
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

	m_nLockCountdown = m_nLockTimeDef; // Resize = user action = no idle time
}

void CPwSafeDlg::CleanUp()
{
	CPrivateConfig pcfg;
	TCHAR szTemp[1024];
	CString strTemp;

	if(m_bTimer == TRUE)
	{
		KillTimer(APPWND_TIMER_ID);
		m_bTimer = FALSE;
	}

	if(m_bMenu == TRUE)
	{
		// Auto-destroyed in BCMenu destructor
		// m_menu.DestroyMenu();

		m_bMenu = FALSE;
	}

	if(m_nClipboardCountdown >= 0)
	{
		ClearClipboardIfOwner(); // This clears the clipboard if we own it
		m_nClipboardCountdown = -1; // Disable clipboard clear countdown
	}

	_DeleteTemporaryFiles();
	FreeCurrentTranslationTable();

	// Save clipboard auto-clear time
	ultoa(m_dwClipboardSecs, szTemp, 10);
	pcfg.Set(PWMKEY_CLIPSECS, szTemp);

	GetCurrentDirectory(1024, szTemp);
	if(_tcslen(szTemp) != 0)
	{
		pcfg.Set(PWMKEY_LASTDIR, szTemp);
	}

	// Save newline sequence
	if(m_bWindowsNewLine == TRUE) _tcscpy(szTemp, _T("Windows"));
	else _tcscpy(szTemp, _T("Unix"));
	pcfg.Set(PWMKEY_NEWLINE, szTemp);

	if(m_bOpenLastDb == TRUE) _tcscpy(szTemp, _T("True"));
	else _tcscpy(szTemp, _T("False"));
	pcfg.Set(PWMKEY_OPENLASTB, szTemp);

	if(m_bAutoSaveDb == TRUE) _tcscpy(szTemp, _T("True"));
	else _tcscpy(szTemp, _T("False"));
	pcfg.Set(PWMKEY_AUTOSAVEB, szTemp);

	pcfg.Set(PWMKEY_LASTDB, (LPCTSTR)m_strLastDb);

	if(m_bImgButtons == FALSE) _tcscpy(szTemp, _T("False"));
	else _tcscpy(szTemp, _T("True"));
	pcfg.Set(PWMKEY_IMGBTNS, szTemp);

	if(m_bEntryGrid == FALSE) _tcscpy(szTemp, _T("False"));
	else _tcscpy(szTemp, _T("True"));
	pcfg.Set(PWMKEY_ENTRYGRID, szTemp);

	if(m_bShowTitle == FALSE) _tcscpy(szTemp, _T("False"));
	else _tcscpy(szTemp, _T("True"));
	pcfg.Set(PWMKEY_SHOWTITLE, szTemp);
	if(m_bShowUserName == FALSE) _tcscpy(szTemp, _T("False"));
	else _tcscpy(szTemp, _T("True"));
	pcfg.Set(PWMKEY_SHOWUSER, szTemp);
	if(m_bShowURL == FALSE) _tcscpy(szTemp, _T("False"));
	else _tcscpy(szTemp, _T("True"));
	pcfg.Set(PWMKEY_SHOWURL, szTemp);
	if(m_bShowPassword == FALSE) _tcscpy(szTemp, _T("False"));
	else _tcscpy(szTemp, _T("True"));
	pcfg.Set(PWMKEY_SHOWPASS, szTemp);
	if(m_bShowNotes == FALSE) _tcscpy(szTemp, _T("False"));
	else _tcscpy(szTemp, _T("True"));
	pcfg.Set(PWMKEY_SHOWNOTES, szTemp);

	if(m_bShowCreation == FALSE) _tcscpy(szTemp, _T("False"));
	else _tcscpy(szTemp, _T("True"));
	pcfg.Set(PWMKEY_SHOWCREATION, szTemp);
	if(m_bShowLastMod == FALSE) _tcscpy(szTemp, _T("False"));
	else _tcscpy(szTemp, _T("True"));
	pcfg.Set(PWMKEY_SHOWLASTMOD, szTemp);
	if(m_bShowLastAccess == FALSE) _tcscpy(szTemp, _T("False"));
	else _tcscpy(szTemp, _T("True"));
	pcfg.Set(PWMKEY_SHOWLASTACCESS, szTemp);
	if(m_bShowExpire == FALSE) _tcscpy(szTemp, _T("False"));
	else _tcscpy(szTemp, _T("True"));
	pcfg.Set(PWMKEY_SHOWEXPIRE, szTemp);
	if(m_bShowUUID == FALSE) _tcscpy(szTemp, _T("False"));
	else _tcscpy(szTemp, _T("True"));
	pcfg.Set(PWMKEY_SHOWUUID, szTemp);

	if(m_bShowAttach == FALSE) _tcscpy(szTemp, _T("False"));
	else _tcscpy(szTemp, _T("True"));
	pcfg.Set(PWMKEY_SHOWATTACH, szTemp);

	if(m_bEntryView == TRUE) _tcscpy(szTemp, _T("True"));
	else _tcscpy(szTemp, _T("False"));
	pcfg.Set(PWMKEY_ENTRYVIEW, szTemp);

	if(m_bPasswordStars == FALSE) _tcscpy(szTemp, _T("False"));
	else _tcscpy(szTemp, _T("True"));
	pcfg.Set(PWMKEY_HIDESTARS, szTemp);
	if(m_bUserStars == FALSE) _tcscpy(szTemp, _T("False"));
	else _tcscpy(szTemp, _T("True"));
	pcfg.Set(PWMKEY_HIDEUSERS, szTemp);
	if(m_bAlwaysOnTop == FALSE) _tcscpy(szTemp, _T("False"));
	else _tcscpy(szTemp, _T("True"));
	pcfg.Set(PWMKEY_ALWAYSTOP, szTemp);
	if(m_bLockOnMinimize == FALSE) _tcscpy(szTemp, _T("False"));
	else _tcscpy(szTemp, _T("True"));
	pcfg.Set(PWMKEY_LOCKMIN, szTemp);
	if(m_bMinimizeToTray == FALSE) _tcscpy(szTemp, _T("False"));
	else _tcscpy(szTemp, _T("True"));
	pcfg.Set(PWMKEY_MINTRAY, szTemp);
	if(m_bCloseMinimizes == FALSE) _tcscpy(szTemp, _T("False"));
	else _tcscpy(szTemp, _T("True"));
	pcfg.Set(PWMKEY_CLOSEMIN, szTemp);

	if(m_bShowToolBar == TRUE) _tcscpy(szTemp, _T("True"));
	else _tcscpy(szTemp, _T("False"));
	pcfg.Set(PWMKEY_SHOWTOOLBAR, szTemp);

	if(m_bColAutoSize == TRUE) _tcscpy(szTemp, _T("True"));
	else _tcscpy(szTemp, _T("False"));
	pcfg.Set(PWMKEY_COLAUTOSIZE, szTemp);

	pcfg.Set(PWMKEY_LISTFONT, (LPCTSTR)m_strFontSpec);

	if((m_bMinimized == FALSE) && (m_bMaximized == FALSE))
	{
		int idScreen;
		RECT rect;
		idScreen = GetSystemMetrics(SM_CXSCREEN) ^ (GetSystemMetrics(SM_CYSCREEN) << 12);
		_itot(idScreen, szTemp, 10);
		pcfg.Set(PWMKEY_SCREENID, szTemp);

		GetWindowRect(&rect);
		_itot(rect.left, szTemp, 10);
		pcfg.Set(PWMKEY_WINDOWPX, szTemp);
		_itot(rect.top, szTemp, 10);
		pcfg.Set(PWMKEY_WINDOWPY, szTemp);
		_itot(rect.right - rect.left, szTemp, 10);
		pcfg.Set(PWMKEY_WINDOWDX, szTemp);
		_itot(rect.bottom - rect.top, szTemp, 10);
		pcfg.Set(PWMKEY_WINDOWDY, szTemp);

		// Save all column widths
		_itot(m_cList.GetColumnWidth(0), szTemp, 10);
		pcfg.Set(PWMKEY_COLWIDTH0, szTemp);
		_itot(m_cList.GetColumnWidth(1), szTemp, 10);
		pcfg.Set(PWMKEY_COLWIDTH1, szTemp);
		_itot(m_cList.GetColumnWidth(2), szTemp, 10);
		pcfg.Set(PWMKEY_COLWIDTH2, szTemp);
		_itot(m_cList.GetColumnWidth(3), szTemp, 10);
		pcfg.Set(PWMKEY_COLWIDTH3, szTemp);
		_itot(m_cList.GetColumnWidth(4), szTemp, 10);
		pcfg.Set(PWMKEY_COLWIDTH4, szTemp);
		_itot(m_cList.GetColumnWidth(5), szTemp, 10);
		pcfg.Set(PWMKEY_COLWIDTH5, szTemp);
		_itot(m_cList.GetColumnWidth(6), szTemp, 10);
		pcfg.Set(PWMKEY_COLWIDTH6, szTemp);
		_itot(m_cList.GetColumnWidth(7), szTemp, 10);
		pcfg.Set(PWMKEY_COLWIDTH7, szTemp);
		_itot(m_cList.GetColumnWidth(8), szTemp, 10);
		pcfg.Set(PWMKEY_COLWIDTH8, szTemp);
		_itot(m_cList.GetColumnWidth(9), szTemp, 10);
		pcfg.Set(PWMKEY_COLWIDTH9, szTemp);
		_itot(m_cList.GetColumnWidth(10), szTemp, 10);
		pcfg.Set(PWMKEY_COLWIDTH10, szTemp);

		_ltot(m_lSplitterPosHoriz, szTemp, 10);
		pcfg.Set(PWMKEY_SPLITTERX, szTemp);
		_ltot(m_lSplitterPosVert, szTemp, 10);
		pcfg.Set(PWMKEY_SPLITTERY, szTemp);
	}

	_ltot((long)m_cList.GetColorEx(), szTemp, 10);
	pcfg.Set(PWMKEY_ROWCOLOR, szTemp);

	_ltot(m_nLockTimeDef, szTemp, 10);
	pcfg.Set(PWMKEY_LOCKTIMER, szTemp);

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

	m_cList.DeleteAllItems();
	m_cGroups.DeleteAllItems();

	m_ilIcons.DeleteImageList();
}

void CPwSafeDlg::_DeleteTemporaryFiles()
{
	if(m_strTempFile.IsEmpty() == FALSE)
	{
		if(SecureDeleteFile(m_strTempFile) == TRUE)
			m_strTempFile.Empty();
	}
}

void CPwSafeDlg::OnOK() 
{
	// Ignore enter in dialog

	// CleanUp();
	// CDialog::OnOK();
}

void CPwSafeDlg::OnCancel() 
{
	if(m_hDraggingGroup != NULL)
	{
		SendMessage(WM_CANCELMODE);
		return;
	}

	// Do not accept the escape key
	if(GetKeyState(27) & 0x8000) return;

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
	OnCancel();
}

void CPwSafeDlg::OnInfoAbout() 
{
	CAboutDlg dlg;
	m_bDisplayDialog = TRUE;
	dlg.DoModal();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnSafeAddGroup() 
{
	CString strGroupName;
	CAddGroupDlg dlg;
	PW_GROUP pwTemplate;
	PW_TIME pwTime;

	if(m_bFileOpen == FALSE) return;

	m_bDisplayDialog = TRUE;

	dlg.m_nIconId = 0;
	dlg.m_strGroupName.Empty();
	dlg.m_pParentImageList = &m_ilIcons;
	dlg.m_bEditMode = FALSE;

	if(dlg.DoModal() == IDOK)
	{
		_GetCurrentPwTime(&pwTime);
		pwTemplate.pszGroupName = (LPSTR)(LPCTSTR)dlg.m_strGroupName;
		pwTemplate.tCreation = pwTime;
		CPwManager::_GetNeverExpireTime(&pwTemplate.tExpire);
		pwTemplate.tLastAccess = pwTime;
		pwTemplate.tLastMod = pwTime;
		pwTemplate.uGroupId = 0; // 0 = create new group
		pwTemplate.uImageId = (DWORD)dlg.m_nIconId;
		pwTemplate.usLevel = 0; pwTemplate.dwFlags = 0;
		VERIFY(m_mgr.AddGroup(&pwTemplate));
		UpdateGroupList();

		HTREEITEM hLast = _GetLastVisibleItem(&m_cGroups);
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
	UINT uState;
	BOOL bChecked;
	int nItem;
	LV_ITEM lvi;

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

	m_bCachedToolBarUpdate = TRUE;
}

DWORD CPwSafeDlg::GetSelectedEntry()
{
	DWORD i;
	UINT uState;

	m_nLockCountdown = m_nLockTimeDef;

	if(m_bFileOpen == FALSE) return DWORD_MAX;

	// LVIS_FOCUSED is not enough here, it must be LVIS_SELECTED
	for(i = 0; i < (DWORD)m_cList.GetItemCount(); i++)
	{
		uState = m_cList.GetItemState((int)i, LVIS_SELECTED);
		if(uState & LVIS_SELECTED) return i;
	}

	return DWORD_MAX;
}

DWORD CPwSafeDlg::GetSelectedEntriesCount()
{
	DWORD i, uSelectedItems = 0;
	UINT uState;

	m_nLockCountdown = m_nLockTimeDef;

	if(m_bFileOpen == FALSE) return DWORD_MAX;

	for(i = 0; i < (DWORD)m_cList.GetItemCount(); i++)
	{
		uState = m_cList.GetItemState((int)i, LVIS_SELECTED);
		if(uState & LVIS_SELECTED) uSelectedItems++;
	}

	return uSelectedItems;
}

DWORD CPwSafeDlg::GetSelectedGroupId()
{
	HTREEITEM h = m_cGroups.GetSelectedItem();
	if(h == NULL) return DWORD_MAX;

	return m_cGroups.GetItemData(h);
}

void CPwSafeDlg::UpdateGroupList()
{
	DWORD i;
	TVINSERTSTRUCT tvis;
	PW_GROUP *pgrp;
	HTREEITEM hParent = TVI_ROOT;
	HTREEITEM hLastItem = TVI_ROOT;
	DWORD usLevel = 0;

	m_nLockCountdown = m_nLockTimeDef;

	m_cGroups.SetRedraw(FALSE);
	GroupSyncStates(TRUE); // Synchronizize 'expanded'-flag from GUI to list manager
	m_cGroups.SelectItem(NULL);
	m_cGroups.DeleteAllItems();
	m_cGroups.SetRedraw(TRUE);

	if(m_bFileOpen == FALSE) return;

	m_cGroups.SetRedraw(FALSE);

	ZeroMemory(&tvis, sizeof(TVINSERTSTRUCT));
	tvis.hInsertAfter = TVI_LAST;
	tvis.item.mask = TVIF_IMAGE | TVIF_TEXT | TVIF_PARAM | TVIF_STATE | TVIF_SELECTEDIMAGE;

	for(i = 0; i < m_mgr.GetNumberOfGroups(); i++)
	{
		pgrp = m_mgr.GetGroup(i);
		ASSERT(pgrp != NULL);
		if(pgrp == NULL) continue;
		ASSERT(pgrp->pszGroupName != NULL);

		while(1)
		{
			if(usLevel == pgrp->usLevel) break;
			else if(usLevel == (USHORT)(pgrp->usLevel - 1))
			{
				hParent = hLastItem;
				usLevel++;
			}
			else if(usLevel < pgrp->usLevel) { ASSERT(FALSE); }
			else if(usLevel > pgrp->usLevel)
			{
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
	DWORD i, j = 0;
	DWORD dwGroupId;
	PW_ENTRY *pwe;
	PW_TIME tNow;

	m_nLockCountdown = m_nLockTimeDef;

	if(m_bFileOpen == FALSE) return;

	dwGroupId = GetSelectedGroupId();
	if(dwGroupId == DWORD_MAX) return;

	m_cList.SetRedraw(FALSE);

	m_cList.DeleteAllItems();
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

	m_cList.SetRedraw(TRUE);
	m_cList.Invalidate();
}

void CPwSafeDlg::_List_SetEntry(DWORD dwInsertPos, PW_ENTRY *pwe, BOOL bIsNewEntry, PW_TIME *ptNow)
{
	LV_ITEM lvi;
	DWORD t;
	CString strTemp;

	if((dwInsertPos == DWORD_MAX) && (bIsNewEntry == TRUE))
		dwInsertPos = (DWORD)m_cList.GetItemCount();

	ZeroMemory(&lvi, sizeof(LV_ITEM));
	lvi.iItem = (int)dwInsertPos;
	lvi.iSubItem = 0;

	// Set 'expired' image if necessary
	if(_pwtimecmp(&pwe->tExpire, ptNow) <= 0) pwe->uImageId = 45;

	lvi.mask = LVIF_TEXT | LVIF_IMAGE;
	if(m_bShowTitle == TRUE)
		lvi.pszText = pwe->pszTitle;
	else
		lvi.pszText = g_pNullString;
	lvi.iImage = pwe->uImageId;

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
		lvi.pszText = (LPSTR)(LPCTSTR)strTemp;
	}
	else
		lvi.pszText = g_pNullString;
	m_cList.SetItem(&lvi);

	lvi.iSubItem = 5;
	if(m_bShowCreation == TRUE)
	{
		_PwTimeToString(pwe->tCreation, &strTemp);
		lvi.pszText = (LPSTR)(LPCTSTR)strTemp;
	}
	else
		lvi.pszText = g_pNullString;
	m_cList.SetItem(&lvi);

	lvi.iSubItem = 6;
	if(m_bShowLastMod == TRUE)
	{
		_PwTimeToString(pwe->tLastMod, &strTemp);
		lvi.pszText = (LPSTR)(LPCTSTR)strTemp;
	}
	else
		lvi.pszText = g_pNullString;
	m_cList.SetItem(&lvi);

	lvi.iSubItem = 7;
	if(m_bShowLastAccess == TRUE)
	{
		_PwTimeToString(pwe->tLastAccess, &strTemp);
		lvi.pszText = (LPSTR)(LPCTSTR)strTemp;
	}
	else
		lvi.pszText = g_pNullString;
	m_cList.SetItem(&lvi);

	lvi.iSubItem = 8;
	if(m_bShowExpire == TRUE)
	{
		_PwTimeToString(pwe->tExpire, &strTemp);
		lvi.pszText = (LPSTR)(LPCTSTR)strTemp;
	}
	else
		lvi.pszText = g_pNullString;
	m_cList.SetItem(&lvi);

	// Ignore m_bShowUUID, the UUID field is needed in all cases
	lvi.iSubItem = 9;
	_UuidToString(pwe->uuid, &strTemp);
	lvi.pszText = (LPSTR)(LPCTSTR)strTemp;
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
	DWORD i, j = 0;
	PW_ENTRY *pwe;
	LV_ITEM lvi;
	TCHAR szTemp[1024];
	BYTE aUuid[16];
	CString strTemp;
	PW_TIME tNow;

	m_nLockCountdown = m_nLockTimeDef;

	if(m_bFileOpen == FALSE) return;

	_GetCurrentPwTime(&tNow);

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

		_StringToUuid((LPCTSTR)strTemp, aUuid);
		pwe = m_mgr.GetEntryByUuid(aUuid);
		ASSERT_ENTRY(pwe);

		if(pwe != NULL)
		{
			_List_SetEntry(j, pwe, FALSE, &tNow);
			j++;
		}
	}
}

void CPwSafeDlg::OnPwlistAdd() 
{
	CAddEntryDlg dlg;
	DWORD uGroupId = GetSelectedGroupId();
	PW_ENTRY pwTemplate;
	PW_TIME tNow;
	DWORD dwInitialGroup; // ID

	if(m_bFileOpen == FALSE) return;
	if(uGroupId == DWORD_MAX) return; // No group selected or other error

	m_bDisplayDialog = TRUE;

	dlg.m_pMgr = &m_mgr;
	dlg.m_dwEntryIndex = DWORD_MAX;
	dlg.m_pParentIcons = &m_ilIcons;
	if((m_bShowPassword == FALSE) || (m_bPasswordStars == TRUE)) dlg.m_bStars = TRUE;
	else dlg.m_bStars = FALSE;
	dlg.m_nGroupId = (int)m_mgr.GetGroupByIdN(uGroupId); // m_nGroupId of the dialog is an index, not an ID
	dlg.m_nIconId = 0;
	dlg.m_bEditMode = FALSE;
	CPwManager::_GetNeverExpireTime(&dlg.m_tExpire);
	dwInitialGroup = uGroupId; // ID

	if(dlg.DoModal() == IDOK)
	{
		_GetCurrentPwTime(&tNow);
		memset(&pwTemplate, 0, sizeof(PW_ENTRY));
		pwTemplate.pszAdditional = (TCHAR *)(LPCTSTR)dlg.m_strNotes;
		pwTemplate.pszPassword = (TCHAR *)(LPCTSTR)dlg.m_strPassword;
		pwTemplate.pszTitle = (TCHAR *)(LPCTSTR)dlg.m_strTitle;
		pwTemplate.pszURL = (TCHAR *)(LPCTSTR)dlg.m_strURL;
		pwTemplate.pszUserName = (TCHAR *)(LPCTSTR)dlg.m_strUserName;
		pwTemplate.tCreation = tNow;
		pwTemplate.tExpire = dlg.m_tExpire;
		pwTemplate.tLastAccess = tNow;
		pwTemplate.tLastMod = tNow;
		pwTemplate.uGroupId = m_mgr.GetGroupIdByIndex((DWORD)dlg.m_nGroupId);
		pwTemplate.uImageId = (DWORD)dlg.m_nIconId;
		pwTemplate.uPasswordLen = (DWORD)dlg.m_strPassword.GetLength();
		pwTemplate.pszBinaryDesc = _T("");

		// Add the entry to the password manager
		VERIFY(m_mgr.AddEntry(&pwTemplate));

		PW_ENTRY *pNew = m_mgr.GetLastEditedEntry();

		int nAttachLen = dlg.m_strAttachment.GetLength();
		int nEscapeLen = (int)_tcslen(PWS_NEW_ATTACHMENT);

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
			m_cList.EnsureVisible(m_cList.GetItemCount() - 1, FALSE);
		}

		m_bModified = TRUE; // Haven't we? :)
	}

	// Cleanup dialog data
	EraseCString(&dlg.m_strTitle);
	EraseCString(&dlg.m_strUserName);
	EraseCString(&dlg.m_strURL);
	EraseCString(&dlg.m_strPassword);
	EraseCString(&dlg.m_strNotes);
	EraseCString(&dlg.m_strAttachment);
	ASSERT(dlg.m_strPassword.GetLength() == 0);

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

	ZeroMemory(&lvi, sizeof(LV_ITEM));
	lvi.iItem = (int)dwSel;
	lvi.iSubItem = 9;
	lvi.mask = LVIF_TEXT;
	lvi.pszText = tszTemp;
	lvi.cchTextMax = 40;
	if(m_cList.GetItem(&lvi) == FALSE) return DWORD_MAX;

	strTemp = lvi.pszText;
	_StringToUuid((LPCTSTR)strTemp, aUuid);

	EraseCString(&strTemp);

	dwSel = m_mgr.GetEntryByUuidN(aUuid);
	ASSERT(dwSel != DWORD_MAX);
	return dwSel;
}

void CPwSafeDlg::OnPwlistEdit() 
{
	DWORD dwEntryIndex;
	PW_ENTRY *pEntry;
	CAddEntryDlg dlg;
	PW_ENTRY pwTemplate;
	PW_TIME tNow;
	DWORD dwNewGroupId;
	BOOL bNeedFullUpdate = FALSE;
	DWORD dwSelectedEntry = GetSelectedEntry();

	if(m_bFileOpen == FALSE) return;

	ASSERT(dwSelectedEntry != DWORD_MAX);
	if(dwSelectedEntry == DWORD_MAX) return;

	dwEntryIndex = _ListSelToEntryIndex();
	if(dwEntryIndex == DWORD_MAX) return;
	pEntry = m_mgr.GetEntry(dwEntryIndex);
	ASSERT_ENTRY(pEntry);
	if(pEntry == NULL) return;

	m_bDisplayDialog = TRUE;

	dlg.m_pMgr = &m_mgr;
	dlg.m_dwEntryIndex = dwEntryIndex;
	dlg.m_pParentIcons = &m_ilIcons;
	dlg.m_bEditMode = TRUE;
	if((m_bShowPassword == FALSE) || (m_bPasswordStars == TRUE)) dlg.m_bStars = TRUE;
	else dlg.m_bStars = FALSE;

	dlg.m_nGroupId = (int)m_mgr.GetGroupByIdN(pEntry->uGroupId); // ID to index
	dlg.m_strTitle = pEntry->pszTitle;
	dlg.m_strUserName = pEntry->pszUserName;
	dlg.m_strURL = pEntry->pszURL;
	m_mgr.UnlockEntryPassword(pEntry);
	dlg.m_strPassword = pEntry->pszPassword;
	dlg.m_strRepeatPw = pEntry->pszPassword;
	m_mgr.LockEntryPassword(pEntry);
	dlg.m_strNotes = pEntry->pszAdditional;
	dlg.m_strAttachment = pEntry->pszBinaryDesc;
	dlg.m_nIconId = (int)pEntry->uImageId;
	dlg.m_tExpire = pEntry->tExpire;

	if(dlg.DoModal() == IDOK)
	{
		pwTemplate = *pEntry;

		_GetCurrentPwTime(&tNow);

		pwTemplate.pszAdditional = (TCHAR *)(LPCTSTR)dlg.m_strNotes;
		pwTemplate.pszPassword = (TCHAR *)(LPCTSTR)dlg.m_strPassword;
		pwTemplate.pszTitle = (TCHAR *)(LPCTSTR)dlg.m_strTitle;
		pwTemplate.pszURL = (TCHAR *)(LPCTSTR)dlg.m_strURL;
		pwTemplate.pszUserName = (TCHAR *)(LPCTSTR)dlg.m_strUserName;
		// pwTemplate.tCreation = pEntry->tCreation;
		pwTemplate.tExpire = dlg.m_tExpire;
		pwTemplate.tLastAccess = tNow;
		pwTemplate.tLastMod = tNow;
		pwTemplate.uImageId = (DWORD)dlg.m_nIconId;
		pwTemplate.uPasswordLen = (DWORD)dlg.m_strPassword.GetLength();

		// If the entry has been moved to a different group, a full
		// update of the list is needed.
		dwNewGroupId = m_mgr.GetGroupIdByIndex((DWORD)dlg.m_nGroupId);
		if(dwNewGroupId != pwTemplate.uGroupId) bNeedFullUpdate = TRUE;
		pwTemplate.uGroupId = dwNewGroupId;

		VERIFY(m_mgr.SetEntry(dwEntryIndex, &pwTemplate));

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

		if(bNeedFullUpdate == TRUE) // Complete list update(!) needed
		{
			_List_SaveView();
			UpdatePasswordList(); // Refresh is not enough!
			_List_RestoreView();
		}
		else // Just update the selected item, not the whole list
		{
			m_mgr.UnlockEntryPassword(&pwTemplate);
			_List_SetEntry(dwSelectedEntry, &pwTemplate, FALSE, &tNow);
			m_mgr.LockEntryPassword(&pwTemplate);
		}

		m_bModified = TRUE;
	}
	else
		_TouchEntry(GetSelectedEntry(), FALSE); // User had viewed it only

	// Cleanup dialog data
	EraseCString(&dlg.m_strTitle);
	EraseCString(&dlg.m_strUserName);
	EraseCString(&dlg.m_strURL);
	EraseCString(&dlg.m_strPassword);
	EraseCString(&dlg.m_strNotes);
	ASSERT(dlg.m_strPassword.GetLength() == 0);

	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnPwlistDelete() 
{
	DWORD dwIndex;
	DWORD dwSel;

	if(m_bFileOpen == FALSE) return;

	while(1)
	{
		dwSel = GetSelectedEntry();
		if(dwSel == DWORD_MAX) break;

		dwIndex = _ListSelToEntryIndex();
		ASSERT(dwIndex != DWORD_MAX);
		if(dwIndex == DWORD_MAX) break;

		VERIFY(m_mgr.DeleteEntry(dwIndex)); // Delete from password manager
		VERIFY(m_cList.DeleteItem((int)dwSel)); // Delete from GUI
	}

	m_cList.RedrawWindow();
	m_bModified = TRUE;

	_UpdateToolBar();
}

void CPwSafeDlg::OnRclickPwlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POINT pt;

	UNREFERENCED_PARAMETER(pNMHDR);

	if(m_bFileOpen == FALSE) return;

	GetCursorPos(&pt);

	m_popmenu.LoadMenu(IDR_PWLIST_MENU);

	m_popmenu.SetMenuDrawMode(BCMENU_DRAWMODE_XP); // <<<!=>>> BCMENU_DRAWMODE_ORIGINAL
	m_popmenu.SetSelectDisableMode(FALSE);
	m_popmenu.SetXPBitmap3D(TRUE);
	m_popmenu.SetBitmapBackground(RGB(255, 0, 255));
	m_popmenu.SetIconSize(16, 16);

	m_popmenu.LoadToolbar(IDR_INFOICONS);

	BCMenu *psub = (BCMenu *)m_popmenu.GetSubMenu(0);
	_TranslateMenu(psub);
	m_bDisplayDialog = TRUE;
	psub->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y,
		AfxGetMainWnd());
	m_popmenu.DestroyMenu();
	m_bDisplayDialog = FALSE;

	*pResult = 0;

	_UpdateToolBar();
}

void CPwSafeDlg::OnClickGroupList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POINT pt;

	UNREFERENCED_PARAMETER(pNMHDR);

	*pResult = 0;

	GetCursorPos(&pt);
	m_cGroups.ScreenToClient(&pt);
	HTREEITEM h = m_cGroups.HitTest(CPoint(pt));
	m_cGroups.SelectItem(h);

	_RemoveSearchGroup();

	UpdatePasswordList();
	_UpdateToolBar();
}

void CPwSafeDlg::OnPwlistCopyPw() 
{
	DWORD dwIndex = _ListSelToEntryIndex();
	PW_ENTRY *p;

	if(dwIndex == DWORD_MAX) return;
	p = m_mgr.GetEntry(dwIndex);
	ASSERT_ENTRY(p);
	if(p == NULL) return;

	m_mgr.UnlockEntryPassword(p);
	CopyStringToClipboard(p->pszPassword);
	m_mgr.LockEntryPassword(p);
	m_nClipboardCountdown = (int)m_dwClipboardSecs;

	if(_tcscmp(p->pszTitle, PWS_TAN_ENTRY) == 0) // If it is a TAN entry, expire it
	{
		_GetCurrentPwTime(&p->tExpire);
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
	}
	else if(nIDEvent == APPWND_TIMER_ID)
	{
		if(m_nClipboardCountdown != -1)
		{
			m_nClipboardCountdown--;

			if(m_nClipboardCountdown == -1)
			{
				m_sbStatus.SetText(TRL("Ready."), 255, 0);
				ClearClipboardIfOwner();
			}
			else if(m_nClipboardCountdown == 0)
			{
				m_sbStatus.SetText(TRL("Clipboard cleared."), 255, 0);
			}
			else
			{
				CString str;
				str.Format(TRL("Password copied to clipboard. Clipboard will be cleared in %d seconds."), m_nClipboardCountdown);
				m_sbStatus.SetText((LPCTSTR)str, 255, 0);
			}
		}

		if((m_bLocked == FALSE) && (m_bFileOpen == TRUE) && (m_bDisplayDialog == FALSE))
		{
			if(m_nLockTimeDef != -1)
			{
				if(m_nLockCountdown != 0)
				{
					m_nLockCountdown--;
					if(m_nLockCountdown == 0) OnFileLock();
				}
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CPwSafeDlg::OnDblclkPwlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CString strData;
	DWORD dwEntryIndex = _ListSelToEntryIndex();
	PW_ENTRY *p;

	*pResult = 0;

	if(dwEntryIndex == DWORD_MAX) return;
	p = m_mgr.GetEntry(dwEntryIndex);
	ASSERT_ENTRY(p);
	if(p == NULL) return;

	switch(pNMListView->iSubItem)
	{
	case 0:
		OnPwlistEdit();
		break;
	case 1:
		OnPwlistCopyUser();
		m_sbStatus.SetText(TRL("Field copied to clipboard."), 255, 0);
		break;
	case 2:
		OnPwlistVisitUrl();
		break;
	case 3:
		OnPwlistCopyPw();
		m_sbStatus.SetText(TRL("Field copied to clipboard."), 255, 0);
		break;
	case 4:
		CopyStringToClipboard(p->pszAdditional);
		m_nClipboardCountdown = -1;
		m_sbStatus.SetText(TRL("Field copied to clipboard."), 255, 0);
		break;
	case 5:
		_PwTimeToString(p->tCreation, &strData);
		CopyStringToClipboard((LPCTSTR)strData);
		m_nClipboardCountdown = -1;
		m_sbStatus.SetText(TRL("Field copied to clipboard."), 255, 0);
		break;
	case 6:
		_PwTimeToString(p->tLastMod, &strData);
		CopyStringToClipboard((LPCTSTR)strData);
		m_nClipboardCountdown = -1;
		m_sbStatus.SetText(TRL("Field copied to clipboard."), 255, 0);
		break;
	case 7:
		_PwTimeToString(p->tLastAccess, &strData);
		CopyStringToClipboard((LPCTSTR)strData);
		m_nClipboardCountdown = -1;
		m_sbStatus.SetText(TRL("Field copied to clipboard."), 255, 0);
		break;
	case 8:
		_PwTimeToString(p->tExpire, &strData);
		CopyStringToClipboard((LPCTSTR)strData);
		m_nClipboardCountdown = -1;
		m_sbStatus.SetText(TRL("Field copied to clipboard."), 255, 0);
		break;
	case 9:
		_UuidToString(p->uuid, &strData);
		CopyStringToClipboard((LPCTSTR)strData);
		m_nClipboardCountdown = -1;
		m_sbStatus.SetText(TRL("Field copied to clipboard."), 255, 0);
		break;
	case 10:
		CopyStringToClipboard(p->pszBinaryDesc);
		m_nClipboardCountdown = -1;
		m_sbStatus.SetText(TRL("Field copied to clipboard."), 255, 0);
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
	POINT pt;

	UNREFERENCED_PARAMETER(pNMHDR);

	if(m_bFileOpen == FALSE) return;

	GetCursorPos(&pt);

	UINT uFlags;
	POINT ptClient = pt;
	m_cGroups.ScreenToClient(&ptClient);
	// HTREEITEM hItem = m_cGroups.HitTest(CPoint(ptClient), &uFlags);
	// if(uFlags & (TVHT_ONITEM | TVHT_ONITEMINDENT)) m_cGroups.SelectItem(hItem);
	// else m_cGroups.SelectItem(NULL);
	m_cGroups.SelectItem(m_cGroups.HitTest(CPoint(ptClient), &uFlags));

	m_popmenu.LoadMenu(IDR_GROUPLIST_MENU);

	m_popmenu.SetMenuDrawMode(BCMENU_DRAWMODE_XP); // <<<!=>>> BCMENU_DRAWMODE_ORIGINAL
	m_popmenu.SetSelectDisableMode(FALSE);
	m_popmenu.SetXPBitmap3D(TRUE);
	m_popmenu.SetBitmapBackground(RGB(255, 0, 255));
	m_popmenu.SetIconSize(16, 16);

	m_popmenu.LoadToolbar(IDR_INFOICONS);

	BCMenu *psub = (BCMenu *)m_popmenu.GetSubMenu(0);
	_TranslateMenu(psub);
	psub->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, AfxGetMainWnd());
	m_popmenu.DestroyMenu();

	*pResult = 0;
	_UpdateToolBar();
}

void CPwSafeDlg::OnPwlistCopyUser() 
{
	DWORD dwEntryIndex;
	PW_ENTRY *p;

	dwEntryIndex = _ListSelToEntryIndex();
	if(dwEntryIndex == DWORD_MAX) return;

	p = m_mgr.GetEntry(dwEntryIndex);
	ASSERT_ENTRY(p);
	if(p == NULL) return;

	if(_tcscmp(p->pszTitle, PWS_TAN_ENTRY) == 0) // Is it a TAN entry?
	{
		// It is a TAN entry, so copy the password instead of the user name
		OnPwlistCopyPw();
		return;
	}

	CopyStringToClipboard(p->pszUserName);
	m_nClipboardCountdown = -1;

	_TouchEntry(GetSelectedEntry(), FALSE);
	_UpdateToolBar();
}

void CPwSafeDlg::OnPwlistVisitUrl() 
{
	DWORD dwEntryIndex;
	PW_ENTRY *p;
	CString strURL;

	dwEntryIndex = _ListSelToEntryIndex();
	if(dwEntryIndex == DWORD_MAX) return;

	p = m_mgr.GetEntry(dwEntryIndex);
	ASSERT_ENTRY(p);
	if(p == NULL) return;

	strURL = p->pszURL;
	if(strURL.GetLength() == 0) return;
	FixURL(&strURL);

	m_mgr.UnlockEntryPassword(p);
	ParseURL(&strURL, p);
	m_mgr.LockEntryPassword(p);

	if(strURL.GetLength() != 0)
		ShellExecute(NULL, _T("open"), (LPCTSTR)strURL, NULL, NULL, SW_SHOW);

	_TouchEntry(GetSelectedEntry(), FALSE);
	_UpdateToolBar();
}

void CPwSafeDlg::OnFileNew() 
{
	CPasswordDlg dlg;
	CString strFirstPassword;

	dlg.m_bLoadMode = FALSE;
	dlg.m_bConfirm = FALSE;

	if(m_bLocked == TRUE) return;

	if(m_bFileOpen == TRUE) OnFileClose();
	if(m_bFileOpen == TRUE)
	{
		MessageBox(TRL("First close the open file before opening another one!"), TRL("Password Safe"),
			MB_OK | MB_ICONWARNING);
		return;
	}

	m_bDisplayDialog = TRUE;

	if(dlg.DoModal() == IDCANCEL) { m_bDisplayDialog = FALSE; return; }

	if(dlg.m_bKeyFile == FALSE)
	{
		strFirstPassword = dlg.m_strRealKey;
		dlg.m_bConfirm = TRUE;
		EraseCString(&dlg.m_strPassword);
		EraseCString(&dlg.m_strRealKey);
		if(dlg.DoModal() == IDCANCEL) { m_bDisplayDialog = FALSE; return; }
		if((dlg.m_strRealKey != strFirstPassword) || (dlg.m_bKeyFile == TRUE))
		{
			MessageBox(TRL("Password and repeated password aren't identical!"),
				TRL("Password Safe"), MB_ICONWARNING);
			m_bDisplayDialog = FALSE; return;
		}
		EraseCString(&strFirstPassword);
	}

	m_mgr.NewDatabase();
	CPwSafeAppRI ri;
	if(m_mgr.SetMasterKey(dlg.m_strRealKey, dlg.m_bKeyFile, &ri, FALSE) == FALSE)
	{
		if(dlg.m_bKeyFile == TRUE)
		{
			int nMsg = MessageBox(TRL("A key-file already exists on this drive. Do you want to overwrite it?"),
				TRL("Overwrite?"), MB_ICONQUESTION | MB_YESNO);

			if(nMsg == IDYES)
			{
				if(m_mgr.SetMasterKey(dlg.m_strRealKey, dlg.m_bKeyFile, &ri, TRUE) == FALSE)
				{
					MessageBox(TRL("Failed to set the master key!"), TRL("Password Safe"),
						MB_OK | MB_ICONWARNING);
					m_bDisplayDialog = FALSE; return;
				}
			}
			else { m_bDisplayDialog = FALSE; return; }
		}
		else
		{
			MessageBox(TRL("Failed to set the master key!"), TRL("Password Safe"),
				MB_OK | MB_ICONWARNING);
			m_bDisplayDialog = FALSE; return;
		}
	}

	m_bFileOpen = TRUE;
	m_cList.EnableWindow(TRUE);
	m_cGroups.EnableWindow(TRUE);
	m_bModified = TRUE;

	EraseCString(&dlg.m_strRealKey);

	PW_GROUP pwTemplate;
	PW_TIME tNow;
	ZeroMemory(&pwTemplate, sizeof(PW_GROUP));
	_GetCurrentPwTime(&tNow);
	pwTemplate.tCreation = tNow;
	CPwManager::_GetNeverExpireTime(&pwTemplate.tExpire);
	pwTemplate.tLastAccess = tNow;
	pwTemplate.tLastMod = tNow;

	// Add standard groups
	pwTemplate.uImageId = 0; pwTemplate.pszGroupName = (TCHAR *)TRL("General");
	pwTemplate.usLevel = 0; pwTemplate.uGroupId = 0; // 0 = create new group ID
	VERIFY(m_mgr.AddGroup(&pwTemplate));
	pwTemplate.uImageId = 38; pwTemplate.pszGroupName = (TCHAR *)TRL("Windows");
	pwTemplate.usLevel = 1; pwTemplate.uGroupId = 0; // 0 = create new group ID
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

	// TESTING CODE, uncomment if you want to add sample groups and entries
#ifdef ___PWSAFE_SAMPLE_DATA
	PW_ENTRY pwT;
	pwT.pBinaryData = NULL; pwT.pszBinaryDesc = NULL; pwT.uBinaryDataLen = 0;
	pwT.pszAdditional = _T("Some Notes");
	pwT.pszPassword = _T("The Password");
	pwT.pszURL = _T("google.com");
	pwT.pszUserName = _T("Anonymous");
	pwT.tCreation = tNow; CPwManager::_GetNeverExpireTime(&pwT.tExpire); pwT.tLastAccess = tNow;
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
		str.Format("Sample #%d", iy);
		pwT.pszTitle = (TCHAR *)(LPCTSTR)str;
		pwT.uGroupId = m_mgr.GetGroupIdByIndex((DWORD)rand() % 8);
		pwT.uImageId = (DWORD)rand() % 30;
		CPwManager::_GetNeverExpireTime(&pwT.tExpire);
		ZeroMemory(pwT.uuid, 16);
		VERIFY(m_mgr.AddEntry(&pwT));
	}
#endif

	m_mgr.FixGroupTree();
	UpdateGroupList();
	UpdatePasswordList();
	_UpdateToolBar();
	m_cGroups.SetFocus();

	m_bDisplayDialog = FALSE;
}

// When pszFile == NULL a file selection dialog is displayed
void CPwSafeDlg::_OpenDatabase(const TCHAR *pszFile)
{
	CString strFile;
	DWORD dwFlags;
	CString strFilter;
	int nRet;
	CString strText;
	const TCHAR *pSuffix = _T("");

	strFilter = TRL("Password Safe files");
	strFilter += _T(" v1/v2");
	strFilter += _T(" (*.kdb)|*.kdb;*.pwd|");
	strFilter += TRL("All files");
	strFilter += _T(" (*.*)|*.*||");

	dwFlags = OFN_LONGNAMES | OFN_EXTENSIONDIFFERENT;
	// OFN_EXPLORER = 0x00080000, OFN_ENABLESIZING = 0x00800000
	dwFlags |= 0x00080000 | 0x00800000;
	dwFlags |= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	CFileDialog dlg(TRUE, NULL, NULL, dwFlags, strFilter, this);

	if(pszFile == NULL) nRet = dlg.DoModal();
	else strFile = pszFile;

	if((pszFile != NULL) || (nRet == IDOK))
	{
		BOOL bSaveDialogState = m_bDisplayDialog;
		if(m_bFileOpen == TRUE) OnFileClose();
		m_bDisplayDialog = bSaveDialogState;
		if(m_bFileOpen == TRUE)
		{
			MessageBox(TRL("First close the open file before opening another one!"), TRL("Password Safe"),
				MB_OK | MB_ICONWARNING);
			return;
		}

		if(pszFile == NULL) strFile = dlg.GetPathName();
		m_strLastDb = strFile;

		CPasswordDlg dlgPass;
		dlgPass.m_bLoadMode = TRUE;
		dlgPass.m_bConfirm = FALSE;

		if(dlgPass.DoModal() == IDCANCEL) return;
		if(m_mgr.SetMasterKey(dlgPass.m_strRealKey, dlgPass.m_bKeyFile, NULL) == FALSE)
		{
			MessageBox(TRL("Failed to set the master key!"), TRL("Password Safe"), MB_OK | MB_ICONWARNING);
			return;
		}
		EraseCString(&dlgPass.m_strRealKey);

		if(m_mgr.OpenDatabase((LPCTSTR)strFile) == FALSE)
		{
			MessageBox(TRL("File cannot be opened!"), TRL("Password Safe"),
				MB_ICONWARNING | MB_OK);
		}
		else
		{
			m_strFile = strFile;
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
			UpdatePasswordList();
		}
	}

	strText = PWM_PRODUCT_NAME;
	strText += _T(" - ");
	strText += CsFileOnly(&m_strFile);
	SetWindowText(strText);
	m_systray.SetIcon(m_hTrayIconNormal);
	m_systray.SetTooltipText(strText);
	m_cGroups.SetFocus();
}

void CPwSafeDlg::OnFileOpen() 
{
	if(m_bLocked == TRUE) return;

	m_bDisplayDialog = TRUE;
	_OpenDatabase(NULL);
	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnFileSave() 
{
	if(m_bFileOpen == FALSE) return;

	m_bDisplayDialog = TRUE;

	GroupSyncStates(TRUE);

	if(m_strFile.IsEmpty())
	{
		OnFileSaveAs();
		m_bDisplayDialog = FALSE;
		return;
	}

	_RemoveSearchGroup();
	if(m_mgr.SaveDatabase((LPCTSTR)m_strFile) == FALSE)
	{
		MessageBox(TRL("File cannot be saved!"), TRL("Password Safe"),
			MB_ICONWARNING | MB_OK);
		m_bDisplayDialog = FALSE;
		return;
	}

	m_strLastDb = m_strFile;
	m_bModified = FALSE;

	CString strText;
	strText = PWM_PRODUCT_NAME;
	strText += _T(" - ");
	strText += CsFileOnly(&m_strFile);
	SetWindowText(strText);

	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnFileSaveAs() 
{
	CString strFile;
	DWORD dwFlags;
	CString strFilter;

	if(m_bFileOpen == FALSE) return;

	m_bDisplayDialog = TRUE;

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
		strFile = dlg.GetPathName();

		_RemoveSearchGroup();
		if(m_mgr.SaveDatabase((LPCTSTR)strFile) == FALSE)
		{
			MessageBox(TRL("File cannot be saved!"), TRL("Password Safe"),
				MB_ICONWARNING | MB_OK);
		}
		else
		{
			m_strFile = strFile;
			m_bModified = FALSE;
			m_strLastDb = strFile;

			CString strText;
			strText = PWM_PRODUCT_NAME;
			strText += _T(" - ");
			strText += CsFileOnly(&m_strFile);
			SetWindowText(strText);
		}
	}

	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnFileClose() 
{
	int nRes;

	m_bDisplayDialog = TRUE;

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
				TRL("Save Before Close?"), MB_YESNOCANCEL | MB_ICONQUESTION);
		}

		if(nRes == IDCANCEL) { m_bDisplayDialog = FALSE; return; }
		else if(nRes == IDYES) OnFileSave();
		else m_bModified = FALSE; // nRes == IDNO
	}

	if(m_bModified == TRUE) { m_bDisplayDialog = FALSE; return; }

	m_cList.DeleteAllItems();
	m_cGroups.DeleteAllItems();
	m_mgr.NewDatabase();

	m_strFile.Empty();
	m_bFileOpen = FALSE;
	m_bModified = FALSE;
	m_cList.EnableWindow(FALSE);
	m_cGroups.EnableWindow(FALSE);

	_DeleteTemporaryFiles();

	m_systray.SetIcon(m_hTrayIconNormal);
	CString str = PWM_PRODUCT_NAME;
	SetWindowText(str);
	m_systray.SetTooltipText(str);
	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnSafeOptions() 
{
	COptionsDlg dlg;
	int nAlgorithmOld;

	m_bDisplayDialog = TRUE;

	if(m_bWindowsNewLine == TRUE) dlg.m_nNewlineSequence = 0;
	else dlg.m_nNewlineSequence = 1;

	dlg.m_uClipboardSeconds = m_dwClipboardSecs - 1;
	dlg.m_bOpenLastDb = m_bOpenLastDb;
	dlg.m_bImgButtons = m_bImgButtons;
	dlg.m_bEntryGrid = m_bEntryGrid;
	dlg.m_bAutoSave = m_bAutoSaveDb;
	dlg.m_strFontSpec = m_strFontSpec;
	dlg.m_bLockOnMinimize = m_bLockOnMinimize;
	dlg.m_bMinimizeToTray = m_bMinimizeToTray;
	dlg.m_bCloseMinimizes = m_bCloseMinimizes;
	dlg.m_nAlgorithm = m_mgr.GetAlgorithm();
	dlg.m_bLockAfterTime = (m_nLockTimeDef != -1) ? TRUE : FALSE;
	if(m_nLockTimeDef != -1)
		dlg.m_nLockAfter = (UINT)m_nLockTimeDef;
	else
		dlg.m_nLockAfter = 0;
	dlg.m_rgbRowHighlight = m_cList.GetColorEx();
	dlg.m_bColAutoSize = m_bColAutoSize;

	nAlgorithmOld = dlg.m_nAlgorithm;

	if(dlg.DoModal() == IDOK)
	{
		m_bWindowsNewLine = (dlg.m_nNewlineSequence == 0) ? TRUE : FALSE;
		m_dwClipboardSecs = dlg.m_uClipboardSeconds + 1;
		m_bOpenLastDb = dlg.m_bOpenLastDb;
		m_bImgButtons = dlg.m_bImgButtons;
		m_bEntryGrid = dlg.m_bEntryGrid;
		m_bAutoSaveDb = dlg.m_bAutoSave;
		m_bLockOnMinimize = dlg.m_bLockOnMinimize;
		m_bMinimizeToTray = dlg.m_bMinimizeToTray;
		m_bCloseMinimizes = dlg.m_bCloseMinimizes;
		m_mgr.SetAlgorithm(dlg.m_nAlgorithm);
		if(dlg.m_rgbRowHighlight == 0xFF000000) dlg.m_rgbRowHighlight = RGB(238,238,255);
		m_cList.SetColorEx(dlg.m_rgbRowHighlight);
		m_bColAutoSize = dlg.m_bColAutoSize;

		if(dlg.m_bLockAfterTime == TRUE)
			m_nLockTimeDef = (long)dlg.m_nLockAfter;
		else
			m_nLockTimeDef = -1;

		if(dlg.m_strFontSpec != m_strFontSpec)
		{
			_ParseSpecAndSetFont((LPCTSTR)dlg.m_strFontSpec);
		}

		NewGUI_SetImgButtons(m_bImgButtons);
		_SetListParameters();

		// If the algorithm has been changed, mark db as modified
		if(nAlgorithmOld != dlg.m_nAlgorithm) m_bModified = TRUE;

		ProcessResize();
	}

	m_nLockCountdown = m_nLockTimeDef;
	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnSafeRemoveGroup() 
{
	DWORD dwGroupId = GetSelectedGroupId();
	int nRes;

	if(dwGroupId == DWORD_MAX) return;

	m_bDisplayDialog = TRUE;

	CString str;
	str = TRL("Deleting a group will delete all items in that group, too.");
	str += _T("\r\n\r\n");
	str += TRL("Are you sure you want to delete this group?");
	nRes = MessageBox(str, TRL("Delete Group Confirmation"),
		MB_OK | MB_ICONQUESTION | MB_YESNO);
	if(nRes == IDYES)
	{
		VERIFY(m_mgr.DeleteGroupById(dwGroupId));

		_Groups_SaveView(FALSE);
		UpdateGroupList();
		_Groups_RestoreView();
		UpdatePasswordList();

		m_bModified = TRUE;
	}
	_UpdateToolBar();

	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnFileChangeMasterPw() 
{
	if(m_bFileOpen == FALSE) return;

	m_bDisplayDialog = TRUE;

	CPasswordDlg dlg;
	CString strFirstPassword;

	dlg.m_bConfirm = FALSE;
	dlg.m_bLoadMode = FALSE;

	if(dlg.DoModal() == IDCANCEL) { m_bDisplayDialog = FALSE; return; }

	if(dlg.m_bKeyFile == TRUE)
	{
		CPwSafeAppRI ri;
		m_bDisplayDialog = TRUE;
		if(m_mgr.SetMasterKey(dlg.m_strRealKey, dlg.m_bKeyFile, &ri, FALSE) == FALSE)
		{
			int nMsg = MessageBox(TRL("A key-file already exists on this drive. Do you want to overwrite it?"),
				TRL("Overwrite?"), MB_ICONQUESTION | MB_YESNO);

			if(nMsg == IDYES)
			{
				if(m_mgr.SetMasterKey(dlg.m_strRealKey, dlg.m_bKeyFile, &ri, TRUE) == FALSE)
				{
					MessageBox(TRL("Failed to set the master key!"), TRL("Password Safe"),
						MB_OK | MB_ICONWARNING);
					EraseCString(&dlg.m_strRealKey);
					m_bDisplayDialog = FALSE;
					return;
				}
			}
			else { m_bDisplayDialog = FALSE; return; }
		}

		EraseCString(&dlg.m_strRealKey);

		m_bModified = TRUE;
	}
	else
	{
		dlg.m_bConfirm = TRUE;
		dlg.m_bLoadMode = FALSE;

		strFirstPassword = dlg.m_strRealKey;

		EraseCString(&dlg.m_strPassword);
		EraseCString(&dlg.m_strRealKey);

		m_bDisplayDialog = TRUE;
		if(dlg.DoModal() == IDCANCEL) { EraseCString(&strFirstPassword); m_bDisplayDialog = FALSE; return; }
		m_bDisplayDialog = FALSE;

		if((dlg.m_strRealKey != strFirstPassword) || (dlg.m_bKeyFile == TRUE))
		{
			MessageBox(TRL("Password and repeated password aren't identical!"),
				TRL("Password Safe"), MB_ICONWARNING);
			EraseCString(&dlg.m_strRealKey);
			EraseCString(&strFirstPassword);
			m_bDisplayDialog = FALSE;
			return;
		}

		CPwSafeAppRI ri;
		m_bDisplayDialog = TRUE;
		if(m_mgr.SetMasterKey(dlg.m_strRealKey, dlg.m_bKeyFile, &ri, TRUE) == FALSE)
		{
			MessageBox(TRL("Failed to set the master key!"), TRL("Stop"), MB_OK | MB_ICONWARNING);
			m_bDisplayDialog = FALSE;
			return;
		}
		m_bDisplayDialog = FALSE;

		EraseCString(&dlg.m_strRealKey);
		EraseCString(&strFirstPassword);

		m_bModified = TRUE;
	}
	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	if((m_bFileOpen == TRUE) && (m_bModified == TRUE))
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
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
	pCmdUI->Enable((((m_dwLastFirstSelectedItem != DWORD_MAX) && (m_dwLastNumSelectedItems == 1)) ? TRUE : FALSE));
}

void CPwSafeDlg::OnUpdatePwlistCopyUser(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((((m_dwLastFirstSelectedItem != DWORD_MAX) && (m_dwLastNumSelectedItems == 1)) ? TRUE : FALSE));
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
	pCmdUI->Enable((((m_dwLastFirstSelectedItem != DWORD_MAX) && (m_dwLastNumSelectedItems == 1)) ? TRUE : FALSE));
}

void CPwSafeDlg::OnUpdateSafeRemoveGroup(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(((m_hLastSelectedGroup != NULL) ? TRUE : FALSE));
}

void CPwSafeDlg::OnUpdateSafeAddGroup(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnUpdatePwlistAdd(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bFileOpen);
}

CString CPwSafeDlg::GetExportFile(int nFormat, LPCTSTR lpFileName)
{
	DWORD dwFlags;
	LPTSTR lp;
	CString strSample;
	CString strFilter;

	if(m_bFileOpen == FALSE) return CString("");

	m_bDisplayDialog = TRUE;

	if(nFormat == PWEXP_TXT) lp = _T("txt");
	else if(nFormat == PWEXP_HTML) lp = _T("html");
	else if(nFormat == PWEXP_XML) lp = _T("xml");
	else if(nFormat == PWEXP_CSV) lp = _T("csv");
	else { ASSERT(FALSE); }

	if(lpFileName == NULL) strSample = _T("Export");
	else strSample = lpFileName;
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
	CPwExport cExp;
	CString strFile;
	if(m_bFileOpen == FALSE) return;
	cExp.SetManager(&m_mgr);
	cExp.SetNewLineSeq(m_bWindowsNewLine);
	cExp.SetFormat(PWEXP_TXT);
	strFile = GetExportFile(PWEXP_TXT, CsFileOnly(&m_strFile));
	if(strFile.GetLength() != 0) cExp.ExportAll(strFile);
}

void CPwSafeDlg::OnExportHtml() 
{
	CPwExport cExp;
	CString strFile;
	if(m_bFileOpen == FALSE) return;
	cExp.SetManager(&m_mgr);
	cExp.SetNewLineSeq(m_bWindowsNewLine);
	cExp.SetFormat(PWEXP_HTML);
	strFile = GetExportFile(PWEXP_HTML, CsFileOnly(&m_strFile));
	if(strFile.GetLength() != 0) cExp.ExportAll(strFile);
}

void CPwSafeDlg::OnExportXml() 
{
	CPwExport cExp;
	CString strFile;
	if(m_bFileOpen == FALSE) return;
	cExp.SetManager(&m_mgr);
	cExp.SetNewLineSeq(m_bWindowsNewLine);
	cExp.SetFormat(PWEXP_XML);
	strFile = GetExportFile(PWEXP_XML, CsFileOnly(&m_strFile));
	if(strFile.GetLength() != 0) cExp.ExportAll(strFile);
}

void CPwSafeDlg::OnExportCsv() 
{
	CPwExport cExp;
	CString strFile;
	if(m_bFileOpen == FALSE) return;
	cExp.SetManager(&m_mgr);
	cExp.SetNewLineSeq(m_bWindowsNewLine);
	cExp.SetFormat(PWEXP_CSV);
	strFile = GetExportFile(PWEXP_CSV, CsFileOnly(&m_strFile));
	if(strFile.GetLength() != 0) cExp.ExportAll(strFile);
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
	CPwExport cExp;
	TCHAR szFile[MAX_PATH * 2];

	_DeleteTemporaryFiles();

	if(m_bFileOpen == FALSE) return;

	cExp.SetManager(&m_mgr);
	cExp.SetNewLineSeq(m_bWindowsNewLine);
	cExp.SetFormat(PWEXP_HTML);

	GetTempPath(MAX_PATH * 2, szFile);
	if(szFile[_tcslen(szFile) - 1] != _T('\\')) _tcscat(szFile, _T("\\"));
	_tcscat(szFile, _T("pwsafetmp.html"));

	BOOL bRet;
	bRet = cExp.ExportGroup(szFile, dwGroupId);

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
	_PrintGroup(DWORD_MAX);
}

void CPwSafeDlg::OnUpdateFilePrint(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnExtrasGenPw() 
{
	CPwGeneratorDlg dlg;

	m_bDisplayDialog = TRUE;
	dlg.m_bCanAccept = FALSE;
	dlg.DoModal();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnSafeModifyGroup() 
{
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
	_Find(DWORD_MAX);
	_UpdateToolBar();
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

	if(dwFindGroupId != DWORD_MAX) dwFindGroupIndex = m_mgr.GetGroupByIdN(dwFindGroupId);
	else dwFindGroupIndex = DWORD_MAX;

	m_bDisplayDialog = TRUE;

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
			pwTemplate.tCreation = tNow; CPwManager::_GetNeverExpireTime(&pwTemplate.tExpire);
			pwTemplate.tLastAccess = tNow; pwTemplate.tLastMod = tNow;
			pwTemplate.uGroupId = 0; // 0 = create new group ID
			pwTemplate.uImageId = 40; // 40 = 'search' icon
			pwTemplate.usLevel = 0; pwTemplate.dwFlags = 0;

			VERIFY(m_mgr.AddGroup(&pwTemplate));
			dwGroupId = m_mgr.GetGroupId(PWS_SEARCHGROUP);
		}
		ASSERT((dwGroupId != DWORD_MAX) && (dwGroupId != 0));
		if((dwGroupId == DWORD_MAX) || (dwGroupId == 0)) return;

		while(1)
		{
			dw = m_mgr.Find((LPCTSTR)dlg.m_strFind, dlg.m_bCaseSensitive, dwFlags, cnt);

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

		m_bModified = TRUE;
	}

	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdatePwlistFind(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bFileOpen && (m_mgr.GetNumberOfEntries() != 0));
}

void CPwSafeDlg::OnPwlistFindInGroup() 
{
	DWORD dwCurGroup = GetSelectedGroupId();
	if(dwCurGroup == DWORD_MAX) return;
	_Find(dwCurGroup);
}

void CPwSafeDlg::OnUpdatePwlistFindInGroup(CCmdUI* pCmdUI) 
{
	if(m_bFileOpen == FALSE) { pCmdUI->Enable(FALSE); return; }

	DWORD dwGroupId = m_cGroups.GetItemData(m_hLastSelectedGroup);
	DWORD dwRefId = m_mgr.GetGroupId(PWS_SEARCHGROUP);
	BOOL bEnable = (dwGroupId != dwRefId) ? TRUE : FALSE;
	pCmdUI->Enable(bEnable && (m_mgr.GetNumberOfEntries() != 0));
}

void CPwSafeDlg::OnPwlistDuplicate() 
{
	PW_ENTRY *p;
	PW_ENTRY pwTemplate;
	PW_TIME tNow;
	int nItemCount = m_cList.GetItemCount();
	int i;
	DWORD dwEntryIndex;
	UINT uState;

	_GetCurrentPwTime(&tNow);

	for(i = 0; i < nItemCount; i++)
	{
		uState = m_cList.GetItemState(i, UINT_MAX);

		if(uState & LVIS_SELECTED)
		{
			dwEntryIndex = _ListSelToEntryIndex((DWORD)i);

			p = m_mgr.GetEntry(dwEntryIndex);
			ASSERT_ENTRY(p);
			if(p == NULL) continue;

			pwTemplate = *p; // Duplicate

			memset(pwTemplate.uuid, 0, 16); // We need a new UUID
			pwTemplate.tCreation = tNow; // Set new times
			pwTemplate.tLastMod = tNow;
			pwTemplate.tLastAccess = tNow;

			m_mgr.UnlockEntryPassword(&pwTemplate);
			VERIFY(m_mgr.AddEntry(&pwTemplate));
			m_mgr.LockEntryPassword(&pwTemplate);
		}
	}

	UpdatePasswordList();
	m_cList.EnsureVisible(m_cList.GetItemCount() - 1, FALSE);

	_UpdateToolBar();
}

void CPwSafeDlg::OnUpdatePwlistDuplicate(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(((m_dwLastFirstSelectedItem != DWORD_MAX) ? TRUE : FALSE));
}

void CPwSafeDlg::OnInfoHomepage() 
{
	ShellExecute(NULL, _T("open"), PWM_HOMEPAGE, NULL, NULL, SW_SHOW);
}

void CPwSafeDlg::OnViewAlwaysOnTop() 
{
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
	CPwExport cExp;
	CString strFile;
	DWORD dwSelectedGroup = GetSelectedGroupId();

	ASSERT(dwSelectedGroup != DWORD_MAX);
	if(dwSelectedGroup == DWORD_MAX) return;

	cExp.SetManager(&m_mgr);
	cExp.SetNewLineSeq(m_bWindowsNewLine);
	cExp.SetFormat(nFormat);
	strFile = GetExportFile(nFormat, m_mgr.GetGroupById(dwSelectedGroup)->pszGroupName);
	if(strFile.GetLength() != 0) cExp.ExportGroup(strFile, dwSelectedGroup);
}

void CPwSafeDlg::OnSafeExportGroupHtml() 
{
	ExportSelectedGroup(PWEXP_HTML);
}

void CPwSafeDlg::OnSafeExportGroupXml() 
{
	ExportSelectedGroup(PWEXP_XML);
}

void CPwSafeDlg::OnSafeExportGroupCsv() 
{
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
	DWORD dwGroup = GetSelectedGroupId();
	if(dwGroup == DWORD_MAX) return;

	_PrintGroup(dwGroup);
}

void CPwSafeDlg::OnUpdateSafePrintGroup(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(((m_hLastSelectedGroup != NULL) ? TRUE : FALSE));
}

void CPwSafeDlg::OnPwlistMoveUp() 
{
	DWORD dwEntryIndex = _ListSelToEntryIndex();
	DWORD dwRelativeEntry = GetSelectedEntry();
	PW_ENTRY *p;

	if(dwEntryIndex == DWORD_MAX) return;
	if(dwRelativeEntry == 0) return;

	_TouchEntry(GetSelectedEntry(), FALSE);

	p = m_mgr.GetEntry(dwEntryIndex);
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
	DWORD dwEntryIndex = _ListSelToEntryIndex();
	DWORD dwRelativeEntry = GetSelectedEntry();
	PW_ENTRY *p;

	if(dwEntryIndex == DWORD_MAX) return;
	if(dwRelativeEntry == 0) return;

	_TouchEntry(GetSelectedEntry(), FALSE);

	p = m_mgr.GetEntry(dwEntryIndex);
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
	DWORD dwEntryIndex = _ListSelToEntryIndex();
	DWORD dwRelativeEntry = GetSelectedEntry();
	DWORD dwItemCount = (DWORD)m_cList.GetItemCount();
	PW_ENTRY *p;

	if(dwEntryIndex == DWORD_MAX) return;
	if(dwRelativeEntry == (dwItemCount - 1)) return;

	_TouchEntry(GetSelectedEntry(), FALSE);

	p = m_mgr.GetEntry(dwEntryIndex);
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
	DWORD dwEntryIndex = _ListSelToEntryIndex();
	DWORD dwRelativeEntry = GetSelectedEntry();
	DWORD dwItemCount = (DWORD)m_cList.GetItemCount();
	PW_ENTRY *p;

	if(dwEntryIndex == DWORD_MAX) return;
	if(dwRelativeEntry == (dwItemCount - 1)) return;

	_TouchEntry(GetSelectedEntry(), FALSE);

	p = m_mgr.GetEntry(dwEntryIndex);
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
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;

	if(m_bFileOpen == FALSE) return;
	DWORD dwEntryIndex = _ListSelToEntryIndex();
	if(dwEntryIndex == DWORD_MAX) return;
	PW_ENTRY *p = m_mgr.GetEntry(dwEntryIndex);
	ASSERT_ENTRY(p);
	if(p == NULL) return;

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

	_TouchEntry(GetSelectedEntry(), FALSE);
	_UpdateToolBar();
}

void CPwSafeDlg::OnFileChangeLanguage() 
{
	CLanguagesDlg dlg;
	TCHAR szFile[1024];
	STARTUPINFO sui;
	PROCESS_INFORMATION pi;

	ZeroMemory(&sui, sizeof(STARTUPINFO));
	sui.cb = sizeof(STARTUPINFO);
	ZeroMemory(&pi, sizeof(pi));

	m_bDisplayDialog = TRUE;

	if(dlg.DoModal() == IDOK)
	{
		m_bDisplayDialog = FALSE;

		GetModuleFileName(NULL, szFile, 1024);
		if(CreateProcess(szFile, NULL, NULL, NULL, FALSE, 0, NULL, NULL,
			&sui, &pi) == FALSE)
		{
			CString str;
			str = TRL("Application cannot be restarted automatically!");
			str += _T("\r\n\r\n");
			str += TRL("Please restart KeePass manually.");
			MessageBox(str, TRL("Loading error"), MB_OK | MB_ICONWARNING);
		}
		else
		{
			OnFileExit();
		}
	}

	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnInfoReadme() 
{
	_OpenLocalFile(PWM_README_FILE, OLF_OPEN);
}

void CPwSafeDlg::OnInfoLicense() 
{
	_OpenLocalFile(PWM_LICENSE_FILE, OLF_OPEN);
}

void CPwSafeDlg::OnInfoPrintLicense() 
{
	_OpenLocalFile(PWM_LICENSE_FILE, OLF_PRINT);
}

void CPwSafeDlg::_SelChangeView(UINT uID)
{
	UINT uState;
	BOOL bChecked;
	BOOL *pFlag;
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
	else { ASSERT(FALSE); }

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

void CPwSafeDlg::OnViewTitle() 
{
	_SelChangeView(ID_VIEW_TITLE);
}

void CPwSafeDlg::OnViewUsername() 
{
	_SelChangeView(ID_VIEW_USERNAME);
}

void CPwSafeDlg::OnViewUrl() 
{
	_SelChangeView(ID_VIEW_URL);
}

void CPwSafeDlg::OnViewPassword() 
{
	_SelChangeView(ID_VIEW_PASSWORD);
}

void CPwSafeDlg::OnViewNotes() 
{
	_SelChangeView(ID_VIEW_NOTES);
}

void CPwSafeDlg::OnFileLock() 
{
	if(m_bDisplayDialog == TRUE) return; // Cannot do anything while displaying a dialog

	// This is a thread-critical function, therefor do so as we show a dialog, this
	// prevents the timer function from doing anything that could interfer with us
	m_bDisplayDialog = TRUE;

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
			MessageBox(TRL("First close the open file before opening another one!"), TRL("Password Safe"),
				MB_OK | MB_ICONWARNING);
			m_bDisplayDialog = FALSE; return;
		}

		if(m_strLastDb.IsEmpty() == TRUE) { m_bDisplayDialog = FALSE; return; }

		m_bLocked = TRUE;
		strExtended = TRL("&Unlock Workspace");
		strExtended += _T("\t");
		strExtended += _GetCmdAccelExt(_T("&Lock Workspace"));
		m_menu.SetMenuText(ID_FILE_LOCK, strExtended, MF_BYCOMMAND);
		m_sbStatus.SetText(TRL("Workspace locked"), 255, 0);
		m_btnTbLock.SetTooltipText(TRL("&Unlock Workspace"));

		strExtended = PWM_PRODUCT_NAME; strExtended += _T(" [");
		strExtended += TRL("Workspace locked"); strExtended += _T("]");
		SetWindowText(strExtended);
		m_systray.SetIcon(m_hTrayIconLocked);
		m_systray.SetTooltipText(strExtended);

		ShowEntryDetails(NULL);
	}
	else
	{
		_OpenDatabase((LPCTSTR)m_strLastDb);

		if(m_bFileOpen == FALSE)
		{
			// strExtended = PWM_PRODUCT_NAME; strExtended += _T(" [");
			// strExtended += TRL("Workspace locked"); strExtended += _T("]");
			// SetWindowText(strExtended);
			// m_systray.SetTooltipText(strExtended);
			// m_systray.SetIcon(m_hTrayIconLocked);
			MessageBox(TRL("Workspace cannot be unlocked!"), TRL("Password Safe"), MB_ICONINFORMATION | MB_OK);
			m_bDisplayDialog = FALSE; return;
		}

		m_nLockCountdown = m_nLockTimeDef;
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

		m_cList.SetItemState((int)m_nLockedViewParams[1], LVIS_SELECTED, LVIS_SELECTED);

		m_sbStatus.SetText(TRL("Ready."), 255, 0);

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
	}

	_UpdateToolBar();
	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdateFileLock(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bFileOpen || m_bLocked);
}

void CPwSafeDlg::OnGroupMoveTop() 
{
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
	DWORD dwGroupId = GetSelectedGroupId();
	if(dwGroupId == DWORD_MAX) return;
	_TouchGroup(dwGroupId, FALSE);

	DWORD dwGroup = m_mgr.GetGroupByIdN(dwGroupId);
	if(dwGroup == 0) return; // Already is the top item
	BOOL b = m_mgr.MoveGroup(dwGroup, dwGroup - 1);
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
	DWORD dwGroupId = GetSelectedGroupId();
	if(dwGroupId == DWORD_MAX) return;
	_TouchGroup(dwGroupId, FALSE);

	DWORD dwGroup = m_mgr.GetGroupByIdN(dwGroupId);
	if(dwGroup == (m_mgr.GetNumberOfGroups() - 1)) return; // Already is last group
	BOOL b = m_mgr.MoveGroup(dwGroup, dwGroup + 1);
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
	return m_systray.OnTrayNotification(wParam, lParam);
}

void CPwSafeDlg::OnViewHide() 
{
	if(m_bShowWindow == TRUE)
	{
		if(m_bMinimizeToTray == FALSE)
			m_systray.MinimiseToTray(this);

		m_bShowWindow = FALSE;

		if(m_bMinimizeToTray == TRUE)
			OnSysCommand(SC_MINIMIZE, 0);

		if((m_bLockOnMinimize == TRUE) && (m_bLocked == FALSE)) OnFileLock();
	}
	else
	{
		if(m_bMinimizeToTray == FALSE)
			m_systray.MaximiseFromTray(this);

		m_bShowWindow = TRUE;

		if(m_bMinimizeToTray == TRUE)
			OnSysCommand(SC_RESTORE, 0);

		if(m_bLocked == TRUE)
		{
			if(m_bLockOnMinimize == TRUE) OnFileLock();
		}
	}
}

void CPwSafeDlg::OnImportCsv() 
{
	CPwImport pvi;
	CString strFile;
	DWORD dwFlags;
	CString strFilter;
	int nRet;
	DWORD dwGroupId;

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

		if(pvi.ImportCsvToDb((LPCTSTR)strFile, &m_mgr, dwGroupId) == TRUE)
		{
			UpdatePasswordList();
			m_bModified = TRUE;
		}
		else
		{
			MessageBox(TRL("An error occured while importing the CSV file. File cannot be imported."),
				TRL("Password Safe"), MB_ICONWARNING);
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
	*pResult = 0;
	UNREFERENCED_PARAMETER(pNMHDR);

	// This would break the multiselect ability of the list, therefor we removed it
	// if((GetKeyState(VK_CONTROL) & 0x8000) > 0) OnPwlistEdit();

	_UpdateToolBar();
}

void CPwSafeDlg::_SortList(DWORD dwByField)
{
	DWORD dwGroupId = GetSelectedGroupId();

	if(dwGroupId == DWORD_MAX) return;
	if(dwGroupId == m_mgr.GetGroupId(PWS_SEARCHGROUP))
	{
		_RemoveSearchGroup();
		m_cList.DeleteAllItems();
		return;
	}

	m_mgr.SortGroup(dwGroupId, (DWORD)dwByField);
	m_bModified = TRUE;
	UpdatePasswordList();
	_UpdateToolBar();
}

void CPwSafeDlg::OnColumnClickPwlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;
	_SortList((DWORD)pNMListView->iSubItem);
}

void CPwSafeDlg::OnImportCWallet() 
{
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

		if(pvi.ImportCWalletToDb((LPCTSTR)strFile, &m_mgr) == TRUE)
		{
			UpdateGroupList();
			m_cGroups.EnsureVisible(_GetLastVisibleItem(&m_cGroups));
			UpdatePasswordList();
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

	if(m_hAccel != NULL)
	{
		CWnd *pWnd = GetFocus();

		if(pWnd != NULL)
		{
			if(pWnd != &m_reEntryView)
			{
				if(TranslateAccelerator(this->m_hWnd, m_hAccel, pMsg)) 
					return(TRUE);
			}
		}
	}

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

		if(pvi.ImportPwSafeToDb((LPCTSTR)strFile, &m_mgr) == TRUE)
		{
			UpdateGroupList();
			UpdatePasswordList();
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
	_SelChangeView(ID_VIEW_CREATION);
}

void CPwSafeDlg::OnViewLastMod() 
{
	_SelChangeView(ID_VIEW_LASTMOD);
}

void CPwSafeDlg::OnViewLastAccess() 
{
	_SelChangeView(ID_VIEW_LASTACCESS);
}

void CPwSafeDlg::OnViewExpire() 
{
	_SelChangeView(ID_VIEW_EXPIRE);
}

void CPwSafeDlg::OnViewUuid() 
{
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

	m_bModified = TRUE;
}

void CPwSafeDlg::_TouchEntry(DWORD dwListIndex, BOOL bEdit)
{
	LV_ITEM lvi;
	TCHAR szTemp[1024];
	CString strTemp;
	BYTE aUuid[16];
	PW_TIME tNow;
	PW_ENTRY *pEntry;

	ASSERT(dwListIndex < (DWORD)m_cList.GetItemCount());
	ASSERT(dwListIndex != DWORD_MAX);
	if(dwListIndex == DWORD_MAX) return;

	ZeroMemory(&lvi, sizeof(LV_ITEM));
	lvi.iItem = (int)dwListIndex;
	lvi.iSubItem = 9;
	lvi.mask = LVIF_TEXT;
	lvi.pszText = szTemp;
	lvi.cchTextMax = 1024;
	m_cList.GetItem(&lvi);

	strTemp = lvi.pszText;
	_StringToUuid((LPCTSTR)strTemp, aUuid);

	EraseCString(&strTemp);

	pEntry = m_mgr.GetEntryByUuid(aUuid);
	ASSERT_ENTRY(pEntry);

	_GetCurrentPwTime(&tNow);

	pEntry->tLastAccess = tNow;
	if(bEdit == TRUE) pEntry->tLastMod = tNow;

	_List_SetEntry(dwListIndex, pEntry, FALSE, &tNow);

	m_bModified = TRUE;
}

void CPwSafeDlg::OnTbOpen() 
{
	OnFileOpen();
}

void CPwSafeDlg::OnTbSave() 
{
	OnFileSave();
}

void CPwSafeDlg::_UpdateToolBar()
{
	if((m_bMinimized == TRUE) || (m_bShowWindow == FALSE)) return;

	CWnd *pFocusWnd = GetFocus();

	DWORD dwSelectedEntry = GetSelectedEntry();
	DWORD dwNumSelectedEntries = GetSelectedEntriesCount();
	DWORD dwNumberOfEntries = m_mgr.GetNumberOfEntries();

	DWORD dwFirstEntryIndex = _ListSelToEntryIndex(dwSelectedEntry);
	PW_ENTRY *p = NULL;
	if(dwFirstEntryIndex != DWORD_MAX) p = m_mgr.GetEntry(dwFirstEntryIndex);
	if(p != NULL) ShowEntryDetails(p);

	if(m_bFileOpen == TRUE)
	{
		m_btnTbAddEntry.EnableWindow(TRUE);
	}
	else
	{
		m_btnTbAddEntry.EnableWindow(FALSE);
	}

	if(m_bLocked == FALSE)
	{
		m_btnTbNew.EnableWindow(TRUE);
		m_btnTbOpen.EnableWindow(TRUE);
	}
	else
	{
		m_btnTbNew.EnableWindow(FALSE);
		m_btnTbOpen.EnableWindow(FALSE);
	}

	if((m_bFileOpen == TRUE) && (m_bModified == TRUE))
	{
		m_btnTbSave.EnableWindow(TRUE);
	}
	else
	{
		m_btnTbSave.EnableWindow(FALSE);
	}

	if((dwSelectedEntry != DWORD_MAX) && (dwNumSelectedEntries == 1))
	{
		m_btnTbCopyUser.EnableWindow(TRUE);
		m_btnTbCopyPw.EnableWindow(TRUE);
		m_btnTbEditEntry.EnableWindow(TRUE);
	}
	else
	{
		m_btnTbCopyUser.EnableWindow(FALSE);
		m_btnTbCopyPw.EnableWindow(FALSE);
		m_btnTbEditEntry.EnableWindow(FALSE);
	}

	if((dwSelectedEntry != DWORD_MAX) && (dwNumSelectedEntries == 1))
	{
		m_btnTbDeleteEntry.EnableWindow(TRUE);
	}
	else
	{
		m_btnTbDeleteEntry.EnableWindow(FALSE);
	}

	if(m_bFileOpen && (m_mgr.GetNumberOfEntries() != 0))
	{
		m_btnTbFind.EnableWindow(TRUE);
	}
	else
	{
		m_btnTbFind.EnableWindow(FALSE);
	}

	if(m_bFileOpen || m_bLocked)
	{
		m_btnTbLock.EnableWindow(TRUE);
	}
	else
	{
		m_btnTbLock.EnableWindow(FALSE);
	}

	if(pFocusWnd != NULL) pFocusWnd->SetFocus(); // Restore the focus!
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
	UINT uState;
	BOOL bChecked;
	UINT uIndex = 0;

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

	GetDlgItem(IDC_STATIC_TBSEP0)->ShowWindow(nCommand); GetDlgItem(IDC_STATIC_TBSEP1)->ShowWindow(nCommand);
	GetDlgItem(IDC_STATIC_TBSEP2)->ShowWindow(nCommand); GetDlgItem(IDC_STATIC_TBSEP3)->ShowWindow(nCommand);
	GetDlgItem(IDC_STATIC_TBSEP4)->ShowWindow(nCommand);
}

void CPwSafeDlg::_RemoveSearchGroup()
{
	DWORD dwSearchGroupId = m_mgr.GetGroupId(PWS_SEARCHGROUP);

	if(dwSearchGroupId != DWORD_MAX)
	{
		DWORD dwSearchItemsCount = m_mgr.GetNumberOfItemsInGroupN(dwSearchGroupId);

		if(dwSearchItemsCount == 0) // Delete only if the group is empty
		{
			m_mgr.DeleteGroupById(dwSearchGroupId); // Remove from password manager
			UpdateGroupList();
		}
	}
}

void CPwSafeDlg::OnPwlistMassModify() 
{
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
				ASSERT(dwIndex != DWORD_MAX);

				p = m_mgr.GetEntry(dwIndex);
				ASSERT(p != NULL);

				if(dlg.m_bModGroup == TRUE) p->uGroupId = dwGroupId;
				if(dlg.m_bModIcon == TRUE) p->uImageId = (DWORD)dlg.m_nIconId;
				if(dlg.m_bModExpire == TRUE) p->tExpire = dlg.m_tExpire;

				_TouchEntry(i, TRUE); // Doesn't change the entry except the time fields
			}
		}

		if(dlg.m_bModGroup == TRUE) // We need a full update
		{
			_List_SaveView();
			UpdatePasswordList();
			_List_RestoreView();
		}
		else // Refresh is enough, no entries have been moved
		{
			RefreshPasswordList();
		}

		m_bModified = TRUE;
	}

	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdatePwlistMassModify(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((m_bFileOpen && (m_dwLastNumSelectedItems > 1)) ? TRUE : FALSE);
}

void CPwSafeDlg::OnKeyDownPwlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN *pLVKeyDow = (LV_KEYDOWN *)pNMHDR;
	*pResult = 0;

	m_bCachedToolBarUpdate = TRUE;
}

CString CPwSafeDlg::_MakeRtfString(LPCTSTR lptString)
{
	CString str;
	int i;
	TCHAR tch;
	CString str2;

	str.Empty();

	ASSERT(lptString != NULL); if(lptString == NULL) return str;

	for(i = 0; i < (int)_tcslen(lptString); i++)
	{
		tch = lptString[i];

		if(tch == _T('\\')) str += _T("\\\\");
		else if(tch == _T('\r')) { }
		else if(tch == _T('\n')) { str += _T("\\par "); }
		else if((tch < 128) && (tch >= 32)) str += tch;
		else
		{
			str2.Format(_T("%02x"), (BYTE)(tch & 0xFF));
			str += _T("\\'"); str += str2;
		}
	}

	return str;
}

void CPwSafeDlg::ShowEntryDetails(PW_ENTRY *p)
{
	CString str;
	CString str2;

	if(m_bEntryView == FALSE) return;

	if(p == NULL) // if p == NULL, just clear the view
	{
		m_reEntryView.SetRTF(CString(_T("")), SF_TEXT);
		return;
	}

	m_mgr.UnlockEntryPassword(p);

	str  = _T("{\\rtf1\\ansi\\ansicpg1252\\deff0\\deftab720{\\fonttbl{\\f0\\fswiss MS Sans Serif;}{\\f1\\froman\\fcharset2 Symbol;}{\\f2\\fswiss Tahoma;}{\\f3\\fswiss Arial;}}");
	str += _T("{\\colortbl\\red0\\green0\\blue0;}");
	str += _T("\\deflang1031\\pard\\plain\\f2\\fs16\\cf0 ");

	str += _T("\\plain\\f2\\fs17\\b ");
	str += TRL("Group:"); str += _T("\\plain\\f2\\fs17  ");
	str += _MakeRtfString(m_mgr.GetGroupById(p->uGroupId)->pszGroupName); str += _T(", ");

	str += _T("\\plain\\f2\\fs17\\b ");
	str += TRL("Title:"); str += _T("\\plain\\f2\\fs17  ");
	str += _MakeRtfString(p->pszTitle); str += _T(", ");

	str += _T("\\plain\\f2\\fs17\\b ");
	str += TRL("UserName:"); str += _T("\\plain\\f2\\fs17  ");
	if(m_bUserStars == FALSE) str += _MakeRtfString(p->pszUserName); else str += _T("********");
	str += _T(", ");

	str += _T("\\plain\\f2\\fs17\\b ");
	str += TRL("URL:"); str += _T("\\plain\\f2\\fs17  ");
	str += _MakeRtfString(p->pszURL); str += _T(", ");

	str += _T("\\plain\\f2\\fs17\\b ");
	str += TRL("Password:"); str += _T("\\plain\\f2\\fs17  ");
	if(m_bPasswordStars == FALSE) str += _MakeRtfString(p->pszPassword); else str += _T("********");
	str += _T(", ");

	str += _T("\\plain\\f2\\fs17\\b ");
	str += TRL("Creation Time"); str += _T(":\\plain\\f2\\fs17  ");
	_PwTimeToString(p->tCreation, &str2); str += _MakeRtfString(str2); str += _T(", ");

	str += _T("\\plain\\f2\\fs17\\b ");
	str += TRL("Last Modification"); str += _T(":\\plain\\f2\\fs17  ");
	_PwTimeToString(p->tLastMod, &str2); str += _MakeRtfString(str2); str += _T(", ");

	str += _T("\\plain\\f2\\fs17\\b ");
	str += TRL("Last Access"); str += _T(":\\plain\\f2\\fs17  ");
	_PwTimeToString(p->tLastAccess, &str2); str += _MakeRtfString(str2); str += _T(", ");

	str += _T("\\plain\\f2\\fs17\\b ");
	str += TRL("Expires"); str += _T(":\\plain\\f2\\fs17  ");
	_PwTimeToString(p->tExpire, &str2); str += _MakeRtfString(str2);

	str += _T("\\par");

	str += _T("\\par ");
	str += _MakeRtfString(p->pszAdditional);

	str += _T("\\pard\\plain\\f2\\fs17 }");

	m_mgr.LockEntryPassword(p);

	m_reEntryView.SetRTF(str);
	str.Empty();
}

void CPwSafeDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	LONG nAddTop;
	RECT rectWindow;

	m_nLockCountdown = m_nLockTimeDef;

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
				DWORD dwNewGroupId;

				PW_GROUP *pNew;

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

					pNew->usLevel = m_mgr.GetGroup(dwDragToGroupPos)->usLevel + 1;
					m_mgr.GetGroup(dwDragToGroupPos)->dwFlags |= PWGF_EXPANDED;
					m_mgr.MoveGroup(m_mgr.GetNumberOfGroups() - 1, dwVParent + 1);
				}
				else // Dropped on empty space
				{
				}

				// Fix group ID, de-associate all entries from the group that we will delete
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

	if((wParam == IDC_RE_ENTRYVIEW) && (lpMsgFilter->nmhdr.code == EN_MSGFILTER)
		&& (lpMsgFilter->msg == WM_RBUTTONDOWN))
	{
		POINT pt;
		GetCursorPos(&pt);

		m_popmenu.LoadMenu(IDR_RECTX_MENU);

		m_popmenu.SetMenuDrawMode(BCMENU_DRAWMODE_XP); // <<<!=>>> BCMENU_DRAWMODE_ORIGINAL
		m_popmenu.SetSelectDisableMode(FALSE);
		m_popmenu.SetXPBitmap3D(TRUE);
		m_popmenu.SetBitmapBackground(RGB(255, 0, 255));
		m_popmenu.SetIconSize(16, 16);

		m_popmenu.LoadToolbar(IDR_INFOICONS);

		BCMenu *psub = (BCMenu *)m_popmenu.GetSubMenu(0);
		_TranslateMenu(psub);
		psub->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, AfxGetMainWnd());
		m_popmenu.DestroyMenu();
	}

	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CPwSafeDlg::OnReCopySel() 
{
	m_reEntryView.Copy();
}

void CPwSafeDlg::OnReCopyAll() 
{
	long lStart, lEnd;
	m_reEntryView.GetSel(lStart, lEnd);
	m_reEntryView.SetSel(0, -1);
	m_reEntryView.Copy();
	m_reEntryView.SetSel(lStart, lEnd);
}

void CPwSafeDlg::OnReSelectAll() 
{
	m_reEntryView.SetSel(0, -1);
}

void CPwSafeDlg::OnExtrasTanWizard() 
{
	CTanWizardDlg dlg;
	PW_ENTRY pwTemplate;
	PW_TIME tNow;
	CString strSubString;
	BOOL bValidSubString, bNumerical;
	int i;
	TCHAR tch;
	DWORD dwCurGroupId = GetSelectedGroupId();

	ASSERT(dwCurGroupId != DWORD_MAX); if(dwCurGroupId == DWORD_MAX) return;

	m_bDisplayDialog = TRUE;

	ZeroMemory(&pwTemplate, sizeof(PW_ENTRY));

	if(dlg.DoModal() == IDOK)
	{
		_GetCurrentPwTime(&tNow);
		pwTemplate.tCreation = tNow;
		pwTemplate.tLastMod = tNow;
		pwTemplate.tLastAccess = tNow;
		pwTemplate.pszTitle = (TCHAR *)(PWS_TAN_ENTRY);
		pwTemplate.pszURL = _T("");
		pwTemplate.pszUserName = _T("");
		m_mgr._GetNeverExpireTime(&pwTemplate.tExpire);
		pwTemplate.uImageId = 29;
		pwTemplate.uGroupId = dwCurGroupId;

		dlg.m_strTans += _T("a"); // Append invalid terminator char

		bValidSubString = FALSE;
		for(i = 0; i < dlg.m_strTans.GetLength(); i++)
		{
			tch = dlg.m_strTans.GetAt(i);

			if((tch >= _T('0')) && (tch <= _T('9'))) bNumerical = TRUE;
			else bNumerical = FALSE;

			if((bNumerical == TRUE) && (bValidSubString == FALSE))
			{
				strSubString = tch;
				bValidSubString = TRUE;
			}
			else if((bNumerical == TRUE) && (bValidSubString == TRUE))
			{
				strSubString += tch;
			}
			else if((bNumerical == FALSE) && (bValidSubString == TRUE))
			{
				pwTemplate.pszPassword = (TCHAR *)(LPCTSTR)strSubString;
				pwTemplate.uPasswordLen = strSubString.GetLength();

				VERIFY(m_mgr.AddEntry(&pwTemplate));

				bValidSubString = FALSE;
				strSubString.Empty();
			}
		}

		UpdatePasswordList();
		m_bModified = TRUE;
	}

	m_bDisplayDialog = FALSE;
}

void CPwSafeDlg::OnUpdateExtrasTanWizard(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnFilePrintPreview() 
{
	CPwExport cExp;
	TCHAR szFile[MAX_PATH * 2];

	_DeleteTemporaryFiles();

	if(m_bFileOpen == FALSE) return;

	m_bDisplayDialog = TRUE;

	cExp.SetManager(&m_mgr);
	cExp.SetNewLineSeq(m_bWindowsNewLine);
	cExp.SetFormat(PWEXP_HTML);

	GetTempPath(MAX_PATH * 2, szFile);
	if(szFile[_tcslen(szFile) - 1] != _T('\\')) _tcscat(szFile, _T("\\"));
	_tcscat(szFile, _T("pwsafetmp.html"));

	BOOL bRet;
	bRet = cExp.ExportGroup(szFile, DWORD_MAX); // Export all: set group ID to DWORD_MAX

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
	CString str;

	str = TRL("Currently used language"); str += _T(": ");
	if(_tcscmp(TRL("~LANGUAGENAME"), _T("~LANGUAGENAME")) != 0)
		str += TRL("~LANGUAGENAME");
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

HTREEITEM CPwSafeDlg::_GetLastVisibleItem(CTreeCtrl *pTree)
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
	ASSERT(pTree != NULL);
	ASSERT(hItem != NULL);

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
	DWORD dwGroupCount = m_mgr.GetNumberOfGroups();
	ASSERT(dwGroupCount >= 1); if(dwGroupCount == 0) return;

	HTREEITEM hItem = m_cGroups.GetSelectedItem();
	ASSERT(hItem != NULL); if(hItem == NULL) return;

	DWORD dwRealParentId = m_cGroups.GetItemData(hItem);
	ASSERT(dwRealParentId != DWORD_MAX); if(dwRealParentId == DWORD_MAX) return;

	PW_GROUP *pRealParent = m_mgr.GetGroupById(dwRealParentId);
	ASSERT(pRealParent != NULL); if(pRealParent == NULL) return;

	DWORD dwVParentPos = m_mgr.GetLastChildGroup(m_mgr.GetGroupByIdN(dwRealParentId));
	ASSERT(dwVParentPos != DWORD_MAX);
	if(dwVParentPos == DWORD_MAX) dwVParentPos = m_mgr.GetGroupByIdN(dwRealParentId);

	PW_GROUP *pVParent = m_mgr.GetGroup(dwVParentPos);
	ASSERT(pVParent != NULL); if(pVParent == NULL) return;

	DWORD dwVParentGroupId = pVParent->uGroupId;
	ASSERT(dwVParentGroupId != DWORD_MAX); if(dwVParentGroupId == DWORD_MAX) return;

	USHORT usVParentLevel = pVParent->usLevel;

	DWORD dwNewGroupPos;

	ASSERT(dwVParentPos <= m_mgr.GetNumberOfGroups());
	ASSERT(m_mgr.GetGroupById(dwVParentGroupId) != NULL);

	OnSafeAddGroup();

	// Have we added a group?
	if(m_mgr.GetNumberOfGroups() == (dwGroupCount + 1))
	{
		// Get the position of the added group
		dwNewGroupPos = m_mgr.GetNumberOfGroups() - 1;

		// Set new level of the added group and expand its parent
		m_mgr.GetGroup(dwNewGroupPos)->usLevel = pRealParent->usLevel + 1;
		pRealParent->dwFlags |= PWGF_EXPANDED;
		GroupSyncStates(FALSE); // 'Send' expanded flag to GUI

		// Move it to the correct position
		m_mgr.MoveGroup(dwNewGroupPos, dwVParentPos + 1);

		UpdateGroupList();
	}
}

void CPwSafeDlg::OnUpdateSafeAddSubgroup(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(((m_hLastSelectedGroup != NULL) ? TRUE : FALSE));
}

void CPwSafeDlg::OnBeginDragGrouplist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW *)pNMHDR;
	*pResult = 0;

	CImageList* pDragImageList = NULL;
	POINT ptOffset;
	RECT rcItem;

	pDragImageList = m_cGroups.CreateDragImage(pNMTreeView->itemNew.hItem);
	ASSERT(pDragImageList != NULL); if(pDragImageList == NULL) return;

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
		SAFE_DELETE(pDragImageList); ASSERT(FALSE); return;
	}

	CPoint ptDragEnter = pNMTreeView->ptDrag;
	ClientToScreen(&ptDragEnter);
	if(pDragImageList->DragEnter(NULL, ptDragEnter) == FALSE)
	{
		SAFE_DELETE(pDragImageList); ASSERT(FALSE); return;
	}

	SAFE_DELETE(pDragImageList);

	SetFocus();
	InvalidateRect(&rcItem, TRUE);
	UpdateWindow();
	SetCapture();

	m_hDraggingGroup = pNMTreeView->itemNew.hItem;
	if(m_cGroups.ItemHasChildren(m_hDraggingGroup) == TRUE) m_bCanDragGroup = FALSE;
	else m_bCanDragGroup = TRUE;
}

void CPwSafeDlg::OnCancelMode() 
{
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
	if((m_bFileOpen == FALSE) || (m_bLocked == TRUE)) return;

	m_mgr.SortGroupList();
	UpdateGroupList();
	UpdatePasswordList();
	m_bModified = TRUE;
}

void CPwSafeDlg::OnUpdateGroupSort(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnPwlistSortTitle() 
{
	_SortList(0);
}

void CPwSafeDlg::OnUpdatePwlistSortTitle(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnPwlistSortUser() 
{
	_SortList(1);
}

void CPwSafeDlg::OnUpdatePwlistSortUser(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnPwlistSortUrl() 
{
	_SortList(2);
}

void CPwSafeDlg::OnUpdatePwlistSortUrl(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnPwlistSortPassword() 
{
	_SortList(3);
}

void CPwSafeDlg::OnUpdatePwlistSortPassword(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnPwlistSortNotes() 
{
	_SortList(4);
}

void CPwSafeDlg::OnUpdatePwlistSortNotes(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnPwlistSortCreation() 
{
	_SortList(5);
}

void CPwSafeDlg::OnUpdatePwlistSortCreation(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnPwlistSortLastmodify() 
{
	_SortList(6);
}

void CPwSafeDlg::OnUpdatePwlistSortLastmodify(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnPwlistSortLastaccess() 
{
	_SortList(7);
}

void CPwSafeDlg::OnUpdatePwlistSortLastaccess(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnPwlistSortExpire() 
{
	_SortList(8);
}

void CPwSafeDlg::OnUpdatePwlistSortExpire(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bFileOpen);
}

void CPwSafeDlg::OnGroupMoveLeft() 
{
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
	UINT uState;
	BOOL bChecked;
	int nItem;
	LV_ITEM lvi;

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

	m_bCachedToolBarUpdate = TRUE;
}

void CPwSafeDlg::OnViewAttach() 
{
	_SelChangeView(ID_VIEW_ATTACH);
}

void CPwSafeDlg::OnPwlistSaveAttach() 
{
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
