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
#include "PwSafeDlg.h"
#include "EntryListDlg.h"

#include "../KeePassLibCpp/Util/TranslateEx.h"
#include "../KeePassLibCpp/Util/MemUtil.h"
#include "../KeePassLibCpp/Util/StrUtil.h"
#include "../KeePassLibCpp/Util/PwUtil.h"
#include "NewGUI/NewGUICommon.h"
#include "Util/PrivateConfigEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CEntryListDlg::CEntryListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEntryListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEntryListDlg)
	//}}AFX_DATA_INIT

	m_nDisplayMode = ELDMODE_NONE;
	m_pMgr = NULL;
	ZeroMemory(m_aUuid, 16);
	m_bPasswordStars = TRUE;
	m_bUserStars = FALSE;
	m_vEntryList.clear();
}

void CEntryListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEntryListDlg)
	DDX_Control(pDX, IDCANCEL, m_btClose);
	DDX_Control(pDX, IDC_ENTRYLIST, m_cList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEntryListDlg, CDialog)
	//{{AFX_MSG_MAP(CEntryListDlg)
	ON_NOTIFY(NM_CLICK, IDC_ENTRYLIST, OnClickEntryList)
	ON_NOTIFY(NM_RETURN, IDC_ENTRYLIST, OnReturnEntryList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CEntryListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	ASSERT(m_pMgr != NULL);
	ASSERT(m_nDisplayMode != ELDMODE_NONE);
	ASSERT(m_pImgList != NULL);

	// Translate all windows
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	NewGUI_XPButton(m_btClose, IDB_CANCEL, IDB_CANCEL);

	// Configure banner control
	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_KEY), KCSB_ICON_LEFT | KCSB_ICON_VCENTER);

	m_banner.SetTitle(m_strBannerTitle);
	m_banner.SetCaption(m_strBannerCaption);

	CString strWindowText = m_strBannerTitle;
	if(m_nDisplayMode == ELDMODE_LIST_ATITEMS) strWindowText = PWM_PRODUCT_NAME;
	SetWindowText(strWindowText);

	m_cList.SetImageList(m_pImgList, LVSIL_SMALL);

	const int nColWidth = 160;
	int aColWidths[5];

	if(m_nDisplayMode != ELDMODE_LIST_ATITEMS)
	{
		aColWidths[0] = nColWidth / 2 + 37;
		aColWidths[1] = nColWidth - 8 - 6;
		aColWidths[2] = nColWidth - 8 - 6;
		aColWidths[3] = nColWidth / 2;
		aColWidths[4] = nColWidth / 2;
	}
	else // ELDMODE_LIST_ATITEMS
	{
		aColWidths[0] = nColWidth / 2 + 37;
		aColWidths[1] = nColWidth - 8 + 50 - 6;
		aColWidths[2] = nColWidth - 8 + 35 - 6;
		aColWidths[3] = 0;
		aColWidths[4] = 0;
	}

	m_cList.InsertColumn(0, TRL("Password Groups"), LVCFMT_LEFT, aColWidths[0], 0);
	m_cList.InsertColumn(1, TRL("Title"), LVCFMT_LEFT, aColWidths[1], 1);
	m_cList.InsertColumn(2, TRL("User Name"), LVCFMT_LEFT, aColWidths[2], 2);
	m_cList.InsertColumn(3, TRL("Password"), LVCFMT_LEFT, aColWidths[3], 3);
	m_cList.InsertColumn(4, TRL("Expires"), LVCFMT_LEFT, aColWidths[4], 4);
	m_cList.InsertColumn(5, TRL("UUID"), LVCFMT_LEFT, 0, 5);

	LPARAM dwStyle = (LVS_EX_SI_REPORT | LVS_EX_FULLROWSELECT | LVS_EX_ONECLICKACTIVATE |
		LVS_EX_UNDERLINEHOT | LVS_EX_HEADERDRAGDROP | LVS_EX_INFOTIP | LVS_EX_GRIDLINES);
	m_cList.PostMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dwStyle);

	PW_ENTRY *p;
	DWORD i;

	if(m_nDisplayMode == ELDMODE_LIST_ATITEMS)
	{
		GetDlgItem(IDC_STATIC_LISTTEXT)->SetWindowText(TRL("Click on an entry to auto-type it."));
		m_btClose.SetWindowText(TRL("&Cancel"));

		m_bUserStars = FALSE;

		RECT rect;
		m_cList.GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.bottom -= 200; rect.right -= 80;
		m_cList.MoveWindow(&rect);

		m_btClose.GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.top -= 200; rect.bottom -= 200; rect.left -= 80; rect.right -= 80;
		m_btClose.MoveWindow(&rect);

		GetWindowRect(&rect);
		rect.bottom -= 200; rect.right -= 80;
		MoveWindow(&rect);

		for(i = 0; i < static_cast<DWORD>(m_vEntryList.size()); ++i)
		{
			p = m_pMgr->GetEntryByUuid(m_vEntryList[i].uuid);
			if(p == NULL) continue;

			_AddEntryToList(p, FALSE);
		}
	}
	else // Expired entries mode
	{
		const DWORD dwInvalid1 = m_pMgr->GetGroupId(PWS_BACKUPGROUP_SRC);
		const DWORD dwInvalid2 = m_pMgr->GetGroupId(PWS_BACKUPGROUP);

		PW_TIME tNow;
		_GetCurrentPwTime(&tNow);
		const DWORD dwDateNow = ((DWORD)tNow.shYear * 13 * 32) +
			((DWORD)tNow.btMonth * 32) + ((DWORD)tNow.btDay & 0xFF);
		const DWORD dwSoonToExpireDays = _GetSoonToExpireDays();

		for(i = 0; i < m_pMgr->GetNumberOfEntries(); ++i)
		{
			p = m_pMgr->GetEntry(i);
			ASSERT(p != NULL); if(p == NULL) continue;

			if((p->uGroupId == dwInvalid1) || (p->uGroupId == dwInvalid2)) continue;
			if(CPwUtil::IsTANEntry(p) == TRUE) continue;

			BOOL bAdded = FALSE;
			if((m_nDisplayMode == ELDMODE_EXPIRED) || (m_nDisplayMode == ELDMODE_EXPSOONEXP))
			{
				if(_pwtimecmp(&tNow, &p->tExpire) > 0)
				{
					_AddEntryToList(p, TRUE);
					bAdded = TRUE;
				}
			}

			if(bAdded == FALSE)
			{
				if((m_nDisplayMode == ELDMODE_SOONTOEXP) || (m_nDisplayMode == ELDMODE_EXPSOONEXP))
				{
					const DWORD dwDate = ((DWORD)p->tExpire.shYear * 13 * 32) +
						((DWORD)p->tExpire.btMonth * 32) + ((DWORD)p->tExpire.btDay & 0xFF);

					if((dwDate >= dwDateNow) && ((dwDate - dwDateNow) <= dwSoonToExpireDays))
						_AddEntryToList(p, FALSE);
				}
			}
		}
	}

	if(m_nDisplayMode == ELDMODE_LIST_ATITEMS)
	{
		if(CPwSafeDlg::m_bSortAutoTypeSelItems == TRUE) NewGUI_SortList(&m_cList);

		if(m_cList.GetItemCount() != 0)
			m_cList.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

		m_cList.SetFocus();
	}
	else m_btClose.SetFocus();

	return FALSE; // return TRUE unless you set the focus to a control
}

void CEntryListDlg::OnOK() 
{
	SaveSelectedEntry();
	m_cList.SetImageList(NULL, LVSIL_SMALL);
	CDialog::OnOK();
}

void CEntryListDlg::OnCancel() 
{
	m_cList.SetImageList(NULL, LVSIL_SMALL);
	CDialog::OnCancel();
}

void CEntryListDlg::_AddEntryToList(PW_ENTRY *p, BOOL bExpiredIcon)
{
	PW_ENTRY *pwe = p;
	ASSERT(pwe != NULL);

	const DWORD dwInsertPos = static_cast<DWORD>(m_cList.GetItemCount());

	LV_ITEM lvi;
	ZeroMemory(&lvi, sizeof(LV_ITEM));
	lvi.iItem = static_cast<int>(dwInsertPos);
	lvi.iSubItem = 0;
	lvi.iImage = ((bExpiredIcon == TRUE) ? 45 : p->uImageId);

	lvi.mask = (LVIF_TEXT | LVIF_IMAGE);
	PW_GROUP *pwg = m_pMgr->GetGroupById(pwe->uGroupId);
	ASSERT(pwg != NULL); if(pwg == NULL) return;
	lvi.pszText = pwg->pszGroupName;

	m_cList.InsertItem(&lvi); // Add

	lvi.mask = LVIF_TEXT;

	lvi.iSubItem = 1;
	lvi.pszText = pwe->pszTitle;
	lvi.iImage = 0;
	m_cList.SetItem(&lvi);

	lvi.iSubItem = 2;
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

	lvi.iSubItem = 3;
	// Hide passwords behind ******** if the user has selected this option
	if(m_bPasswordStars == TRUE)
	{
		lvi.pszText = PWM_PASSWORD_STRING;
		m_cList.SetItem(&lvi);
	}
	else // Don't hide, display them
	{
		m_pMgr->UnlockEntryPassword(pwe);
		lvi.pszText = pwe->pszPassword;
		m_cList.SetItem(&lvi);
		m_pMgr->LockEntryPassword(pwe);
	}

	CString strTemp;
	_PwTimeToStringEx(pwe->tExpire, strTemp, CPwSafeDlg::m_bUseLocalTimeFormat);
	lvi.iSubItem = 4;
	lvi.pszText = const_cast<LPTSTR>((LPCTSTR)strTemp);
	m_cList.SetItem(&lvi);

	// Ignore m_bShowUUID, the UUID field is needed in all cases
	_UuidToString(pwe->uuid, &strTemp);
	lvi.iSubItem = 5;
	lvi.pszText = (LPTSTR)(LPCTSTR)strTemp;
	m_cList.SetItem(&lvi);
}

void CEntryListDlg::OnClickEntryList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	UNREFERENCED_PARAMETER(pNMHDR);

	OnOK(); // Saves the selected entry and closes the dialog
}

void CEntryListDlg::SaveSelectedEntry()
{
	DWORD i;
	UINT uState;
	BOOL bFound = FALSE;
	LV_ITEM lvi;
	TCHAR tszTemp[40];

	ZeroMemory(m_aUuid, 16);

	for(i = 0; i < (DWORD)m_cList.GetItemCount(); i++)
	{
		uState = m_cList.GetItemState((int)i, LVIS_SELECTED);
		if(uState & LVIS_SELECTED) { bFound = TRUE; break; }
	}

	if(bFound == FALSE) return;

	ZeroMemory(&lvi, sizeof(LV_ITEM));
	lvi.iItem = static_cast<int>(i);
	lvi.iSubItem = 5;
	lvi.mask = LVIF_TEXT;
	lvi.pszText = tszTemp;
	lvi.cchTextMax = 40;
	if(m_cList.GetItem(&lvi) == FALSE) { ASSERT(FALSE); return; }

	CString strTemp = lvi.pszText;
	_StringToUuid((LPCTSTR)strTemp, m_aUuid);
}

void CEntryListDlg::OnReturnEntryList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UNREFERENCED_PARAMETER(pNMHDR);

	*pResult = 0;
	OnOK();
}

DWORD _GetSoonToExpireDays()
{
	CPrivateConfigEx cfg(FALSE);

	std::basic_string<TCHAR> str = cfg.GetSafe(PWMKEY_SOONTOEXPIREDAYS);
	if(str.size() == 0) return PWV_SOONTOEXPIRE_DAYS;

	return static_cast<DWORD>(_ttol(str.c_str()));
}
