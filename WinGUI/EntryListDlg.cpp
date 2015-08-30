/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2007 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "NewGUI/NewGUICommon.h"

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

	m_nDisplayMode = ELDMODE_UNKNOWN;
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
	DDX_Control(pDX, IDC_STATIC_PRODUCTNAME, m_stcProductName);
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
	ASSERT(m_nDisplayMode != ELDMODE_UNKNOWN);
	ASSERT(m_pImgList != NULL);

	// Translate all windows
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	NewGUI_XPButton(&m_btClose, IDB_CANCEL, IDB_CANCEL);

	// Configure banner control
	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_KEY), KCSB_ICON_LEFT | KCSB_ICON_VCENTER);

	m_banner.SetTitle(m_strBannerTitle); // TRL("Expired Entries")
	m_banner.SetCaption(m_strBannerCaption); // TRL("This is a list of all expired entries")

	CString strWindowText = m_strBannerTitle;
	if(m_nDisplayMode == ELDMODE_LIST) strWindowText = PWM_PRODUCT_NAME;
	SetWindowText(strWindowText);

	m_stcProductName.SetWindowText(PWM_PRODUCT_NAME);

	m_cList.SetImageList(m_pImgList, LVSIL_SMALL);

	int nColWidth = 160;
	int aColWidths[5];

	if(m_nDisplayMode != ELDMODE_LIST)
	{
		aColWidths[0] = nColWidth / 2 + 20;
		aColWidths[1] = nColWidth - 8;
		aColWidths[2] = nColWidth - 8;
		aColWidths[3] = nColWidth / 2;
		aColWidths[4] = nColWidth / 2;
	}
	else
	{
		aColWidths[0] = nColWidth / 2 + 25;
		aColWidths[1] = nColWidth - 8 + 50;
		aColWidths[2] = nColWidth - 8 + 35;
		aColWidths[3] = 0;
		aColWidths[4] = 0;
	}

	m_cList.InsertColumn(0, TRL("Password Groups"), LVCFMT_LEFT, aColWidths[0], 0);
	m_cList.InsertColumn(1, TRL("Title"), LVCFMT_LEFT, aColWidths[1], 1);
	m_cList.InsertColumn(2, TRL("User Name"), LVCFMT_LEFT, aColWidths[2], 2);
	m_cList.InsertColumn(3, TRL("Password"), LVCFMT_LEFT, aColWidths[3], 3);
	m_cList.InsertColumn(4, TRL("Expires"), LVCFMT_LEFT, aColWidths[4], 4);
	m_cList.InsertColumn(5, TRL("UUID"), LVCFMT_LEFT, 0, 5);

	LPARAM dw = 0;
	dw |= LVS_EX_SI_REPORT | LVS_EX_FULLROWSELECT | LVS_EX_ONECLICKACTIVATE | LVS_EX_UNDERLINEHOT;
	dw |= LVS_EX_HEADERDRAGDROP | LVS_EX_INFOTIP | LVS_EX_GRIDLINES;
	m_cList.PostMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dw);

	PW_ENTRY *p;
	DWORD i;

	if(m_nDisplayMode == ELDMODE_LIST)
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

		m_stcProductName.GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.top -= 200; rect.bottom -= 200;
		m_stcProductName.MoveWindow(&rect);

		GetWindowRect(&rect);
		rect.bottom -= 200; rect.right -= 80;
		MoveWindow(&rect);

		for(i = 0; i < (DWORD)m_vEntryList.size(); i++)
		{
			p = m_pMgr->GetEntryByUuid(m_vEntryList[i].uuid);
			if(p == NULL) continue;

			_AddEntryToList(p, FALSE);
		}
	}
	else // Expired entries mode
	{
		DWORD dwDateNow, dwDate;
		PW_TIME tNow;
		BOOL bAdded;
		DWORD dwInvalid1, dwInvalid2;

		dwInvalid1 = m_pMgr->GetGroupId(PWS_BACKUPGROUP_SRC);
		dwInvalid2 = m_pMgr->GetGroupId(PWS_BACKUPGROUP);

		_GetCurrentPwTime(&tNow);
		dwDateNow = ((DWORD)tNow.shYear << 16) | ((DWORD)tNow.btMonth << 8) | ((DWORD)tNow.btDay & 0xff);

		for(i = 0; i < m_pMgr->GetNumberOfEntries(); i++)
		{
			p = m_pMgr->GetEntry(i);
			ASSERT(p != NULL); if(p == NULL) continue;

			if((p->uGroupId == dwInvalid1) || (p->uGroupId == dwInvalid2)) continue;

			bAdded = FALSE;
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
					dwDate = ((DWORD)p->tExpire.shYear << 16) | ((DWORD)p->tExpire.btMonth << 8) | ((DWORD)p->tExpire.btDay & 0xff);

					if((dwDate >= dwDateNow) && ((dwDate - dwDateNow) <= PWV_SOONTOEXPIRE_DAYS))
						_AddEntryToList(p, FALSE);
				}
			}
		}
	}

	if(m_nDisplayMode == ELDMODE_LIST)
	{
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
	CDialog::OnOK();
}

void CEntryListDlg::OnCancel() 
{
	CDialog::OnCancel();
}

void CEntryListDlg::_AddEntryToList(PW_ENTRY *p, BOOL bExpiredIcon)
{
	LV_ITEM lvi;
	CString strTemp;
	PW_ENTRY *pwe = p;
	PW_GROUP *pwg;

	ASSERT(pwe != NULL);

	DWORD dwInsertPos = (DWORD)m_cList.GetItemCount();

	ZeroMemory(&lvi, sizeof(LV_ITEM));
	lvi.iItem = (int)dwInsertPos;
	lvi.iSubItem = 0;
	lvi.iImage = (bExpiredIcon == TRUE) ? 45 : p->uImageId;

	lvi.mask = LVIF_TEXT | LVIF_IMAGE;
	pwg = m_pMgr->GetGroupById(pwe->uGroupId);
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

	lvi.iSubItem = 4;
	_PwTimeToStringEx(pwe->tExpire, strTemp, CPwSafeDlg::m_bUseLocalTimeFormat);
	lvi.pszText = (LPTSTR)(LPCTSTR)strTemp;
	m_cList.SetItem(&lvi);

	// Ignore m_bShowUUID, the UUID field is needed in all cases
	lvi.iSubItem = 5;
	_UuidToString(pwe->uuid, &strTemp);
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
	lvi.iItem = (int)i;
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
