/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2008 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "FieldRefDlg.h"

#include "../KeePassLibCpp/Util/StrUtil.h"
#include "../KeePassLibCpp/Util/MemUtil.h"
#include "../KeePassLibCpp/Util/TranslateEx.h"
#include "NewGUI/NewGUICommon.h"
#include "Util/WinUtil.h"

IMPLEMENT_DYNAMIC(CFieldRefDlg, CDialog)

CFieldRefDlg::CFieldRefDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFieldRefDlg::IDD, pParent)
{
	m_pMgr = NULL;
	m_pImages = NULL;
	m_bClosing = FALSE;
}

CFieldRefDlg::~CFieldRefDlg()
{
}

void CFieldRefDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_LIST_ENTRIES, m_lvEntries);
	DDX_Control(pDX, IDC_RADIO_IDTITLE, m_radioId);
	DDX_Control(pDX, IDC_RADIO_REFTITLE, m_radioRef);
	DDX_Control(pDX, IDC_BTN_HELP, m_btHelp);
}

BEGIN_MESSAGE_MAP(CFieldRefDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CFieldRefDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CFieldRefDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_HELP, &CFieldRefDlg::OnBnClickedBtnHelp)
	ON_NOTIFY(NM_CLICK, IDC_LIST_ENTRIES, &CFieldRefDlg::OnNMClickListEntries)
END_MESSAGE_MAP()

BOOL CFieldRefDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	ASSERT(m_pMgr != NULL);

	NewGUI_TranslateCWnd(this);
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);
	
	NewGUI_XPButton(m_btOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(m_btCancel, IDB_CANCEL, IDB_CANCEL);
	NewGUI_XPButton(m_btHelp, IDB_HELP_SMALL, IDB_HELP_SMALL);

	m_lvEntries.SetImageList(m_pImages, LVSIL_SMALL);

	RECT rectClient;
	m_lvEntries.GetClientRect(&rectClient);
	const int nColWidth = (rectClient.right - rectClient.left -
		GetSystemMetrics(SM_CXVSCROLL) - 6) / 5;
	m_lvEntries.InsertColumn(0, TRL("Password Groups"), LVCFMT_LEFT, nColWidth + 2, 0);
	m_lvEntries.InsertColumn(1, TRL("Title"), LVCFMT_LEFT, nColWidth, 1);
	m_lvEntries.InsertColumn(2, TRL("User Name"), LVCFMT_LEFT, nColWidth, 2);
	m_lvEntries.InsertColumn(3, TRL("URL"), LVCFMT_LEFT, nColWidth, 3);
	m_lvEntries.InsertColumn(4, TRL("Notes"), LVCFMT_LEFT, nColWidth, 4);
	m_lvEntries.InsertColumn(5, TRL("UUID"), LVCFMT_LEFT, 0, 5);

	LPARAM dw = 0;
	dw |= LVS_EX_SI_REPORT | LVS_EX_FULLROWSELECT;
	dw |= LVS_EX_HEADERDRAGDROP | LVS_EX_INFOTIP | LVS_EX_GRIDLINES;
	m_lvEntries.PostMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dw);

	for(DWORD i = 0; i < m_pMgr->GetNumberOfEntries(); ++i)
		_AddEntryToList(m_pMgr->GetEntry(i));

	NewGUI_SortList(&m_lvEntries);

	this->CheckRadioButton(IDC_RADIO_REFTITLE, IDC_RADIO_REFNOTES, IDC_RADIO_REFPW);
	this->CheckRadioButton(IDC_RADIO_IDTITLE, IDC_RADIO_IDUUID, IDC_RADIO_IDUUID);

	this->EnableChildControls();
	return TRUE; // return TRUE unless you set the focus to a control
}

void CFieldRefDlg::InitEx(CPwManager* pMgr, CImageList* pImages)
{
	m_pMgr = pMgr;
	m_pImages = pImages;
}

void CFieldRefDlg::_AddEntryToList(PW_ENTRY *p)
{
	LV_ITEM lvi;
	ZeroMemory(&lvi, sizeof(LV_ITEM));
	lvi.iItem = m_lvEntries.GetItemCount(); // Insert at end
	lvi.iSubItem = 0;
	lvi.iImage = static_cast<int>(p->uImageId);

	lvi.mask = LVIF_TEXT | LVIF_IMAGE;
	PW_GROUP *pwg = m_pMgr->GetGroupById(p->uGroupId);
	ASSERT(pwg != NULL); if(pwg == NULL) return; // Must be non-virtual

	if((_tcscmp(pwg->pszGroupName, PWS_BACKUPGROUP) == 0) ||
		(_tcscmp(pwg->pszGroupName, PWS_BACKUPGROUP_SRC) == 0)) return;
	lvi.pszText = pwg->pszGroupName;

	m_lvEntries.InsertItem(&lvi); // Add

	lvi.mask = LVIF_TEXT;
	lvi.iImage = 0;

	lvi.iSubItem = 1;
	lvi.pszText = p->pszTitle;
	m_lvEntries.SetItem(&lvi);

	lvi.iSubItem = 2;
	lvi.pszText = p->pszUserName;
	m_lvEntries.SetItem(&lvi);

	lvi.iSubItem = 3;
	lvi.pszText = p->pszURL;
	m_lvEntries.SetItem(&lvi);

	lvi.iSubItem = 4;
	lvi.pszText = p->pszAdditional;
	m_lvEntries.SetItem(&lvi);

	CString strTemp;
	_UuidToString(p->uuid, &strTemp);
	lvi.iSubItem = 5;
	lvi.pszText = (LPTSTR)(LPCTSTR)strTemp;
	m_lvEntries.SetItem(&lvi);
}

PW_ENTRY* CFieldRefDlg::GetSelectedEntry()
{
	BOOL bFound = FALSE;
	int i = 0;
	for( ; i < m_lvEntries.GetItemCount(); ++i)
	{
		const UINT uState = m_lvEntries.GetItemState(i, LVIS_SELECTED);
		if((uState & LVIS_SELECTED) != 0) { bFound = TRUE; break; }
	}

	if(bFound == FALSE) return NULL;

	TCHAR tszTemp[42];
	LV_ITEM lvi;
	ZeroMemory(&lvi, sizeof(LV_ITEM));
	lvi.iItem = i;
	lvi.iSubItem = 5;
	lvi.mask = LVIF_TEXT;
	lvi.pszText = tszTemp;
	lvi.cchTextMax = 40;
	if(m_lvEntries.GetItem(&lvi) == FALSE) { ASSERT(FALSE); return NULL; }

	CString strTemp = lvi.pszText;
	BYTE aUuid[16];
	_StringToUuid((LPCTSTR)strTemp, &aUuid[0]);

	return m_pMgr->GetEntryByUuid(&aUuid[0]);
}

void CFieldRefDlg::OnBnClickedOk()
{
	PW_ENTRY* p = GetSelectedEntry();
	if(p == NULL) { ASSERT(FALSE); return; }

	CString str = _T("{REF:");
	
	const int nRef = this->GetCheckedRadioButton(IDC_RADIO_REFTITLE, IDC_RADIO_REFNOTES);
	if(nRef == IDC_RADIO_REFTITLE) str += _T("T");
	else if(nRef == IDC_RADIO_REFUSER) str += _T("U");
	else if(nRef == IDC_RADIO_REFPW) str += _T("P");
	else if(nRef == IDC_RADIO_REFURL) str += _T("A");
	else if(nRef == IDC_RADIO_REFNOTES) str += _T("N");
	else { ASSERT(FALSE); return; }

	str += _T("@");

	CString strId;
	const int nID = this->GetCheckedRadioButton(IDC_RADIO_IDTITLE, IDC_RADIO_IDUUID);
	if(nID == IDC_RADIO_IDTITLE) strId = CString(_T("T:")) + CString(p->pszTitle);
	else if(nID == IDC_RADIO_IDUSER) strId = CString(_T("U:")) + CString(p->pszUserName);
	else if(nID == IDC_RADIO_IDPASSWORD)
	{
		m_pMgr->UnlockEntryPassword(p);
		strId = CString(_T("P:")) + CString(p->pszPassword);
		m_pMgr->LockEntryPassword(p);
	}
	else if(nID == IDC_RADIO_IDURL) strId = CString(_T("A:")) + CString(p->pszURL);
	else if(nID == IDC_RADIO_IDNOTES) strId = CString(_T("N:")) + CString(p->pszAdditional);
	else if(nID == IDC_RADIO_IDUUID)
	{
		CString strUuid;
		_UuidToString(p->uuid, &strUuid);
		strId = CString(_T("I:")) + strUuid;
	}
	else { ASSERT(FALSE); return; }

	if((strId.Find(_T('{')) >= 0) || (strId.Find(_T('}')) >= 0) ||
		(strId.Find(_T('\r')) >= 0) || (strId.Find(_T('\n')) >= 0))
	{
		MessageBox(TRL("The selected field, that identifies the source entry, contains disallowed characters (like '{', '}', newline characters, ...)."),
			PWM_PRODUCT_NAME_SHORT, MB_ICONWARNING | MB_OK);
		return;
	}

	CString strIdData = strId.Right(strId.GetLength() - 2);
	if(_IdMatchesMultipleTimes(strIdData, strId.GetAt(0)) == TRUE)
	{
		CString strMsg = TRL("Multiple entries match the specified identifying field.");
		strMsg += _T("\r\n\r\n");
		strMsg += TRL("To avoid ambiguity, entries can be identified by their UUIDs, which are unique.");
		MessageBox(strMsg, PWM_PRODUCT_NAME_SHORT, MB_ICONWARNING | MB_OK);
		return;
	}

	str += strId + CString(_T("}"));

	m_strFieldRef = str;
	EraseCString(&str);

	m_bClosing = TRUE;
	OnOK();
}

BOOL CFieldRefDlg::_IdMatchesMultipleTimes(LPCTSTR lpText, TCHAR tchScan)
{
	if((lpText == NULL) || (lpText[0] == 0)) return TRUE;

	DWORD dwFlags = 0;
	if(tchScan == _T('T')) dwFlags |= PWMF_TITLE;
	else if(tchScan == _T('U')) dwFlags |= PWMF_USER;
	else if(tchScan == _T('A')) dwFlags |= PWMF_URL;
	else if(tchScan == _T('P')) dwFlags |= PWMF_PASSWORD;
	else if(tchScan == _T('N')) dwFlags |= PWMF_ADDITIONAL;
	else if(tchScan == _T('I')) dwFlags |= PWMF_UUID;
	else return FALSE;

	const DWORD dwIndex = m_pMgr->Find(lpText, FALSE, dwFlags, 0);
	if(dwIndex == DWORD_MAX) { ASSERT(FALSE); return FALSE; }
	if((dwIndex + 1) == m_pMgr->GetNumberOfEntries()) return FALSE;

	const DWORD dwMulti = m_pMgr->Find(lpText, FALSE, dwFlags, dwIndex + 1);
	if(dwMulti != DWORD_MAX) return TRUE;

	return FALSE;
}

void CFieldRefDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CFieldRefDlg::EnableChildControls()
{
	if(m_bClosing == TRUE) return;

	m_btOK.EnableWindow((GetSelectedEntry() != NULL) ? TRUE : FALSE);
}

void CFieldRefDlg::OnBnClickedBtnHelp()
{
	WU_OpenAppHelp(PWM_HELP_FIELDREFS);
}

void CFieldRefDlg::OnNMClickListEntries(NMHDR *pNMHDR, LRESULT *pResult)
{
	UNREFERENCED_PARAMETER(pNMHDR);
	*pResult = 0;
	this->EnableChildControls();
}
