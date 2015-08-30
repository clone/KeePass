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

#include "StdAfx.h"
#include "PwSafe.h"
#include "EntryListDlg.h"

#include "NewGUI/TranslateEx.h"
#include "Util/MemUtil.h"
#include "Util/StrUtil.h"

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
}

void CEntryListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEntryListDlg)
	DDX_Control(pDX, IDOK, m_btClose);
	DDX_Control(pDX, IDC_ENTRYLIST, m_cList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEntryListDlg, CDialog)
	//{{AFX_MSG_MAP(CEntryListDlg)
	ON_NOTIFY(NM_CLICK, IDC_ENTRYLIST, OnClickEntryList)
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

	m_banner.SetTitle(TRL("Expired Entries"));
	m_banner.SetCaption(TRL("This is a list of all expired entries"));
	SetWindowText(TRL("Expired Entries"));

	m_cList.SetImageList(m_pImgList, LVSIL_SMALL);

	int nColWidth = 160;
	m_cList.InsertColumn(0, TRL("Password Groups"), LVCFMT_LEFT, nColWidth / 2 + 20, 0);
	m_cList.InsertColumn(1, TRL("Title"), LVCFMT_LEFT, nColWidth - 8, 1);
	m_cList.InsertColumn(2, TRL("UserName"), LVCFMT_LEFT, nColWidth - 8, 2);
	m_cList.InsertColumn(3, TRL("Password"), LVCFMT_LEFT, nColWidth / 2, 3);
	m_cList.InsertColumn(4, TRL("Expires"), LVCFMT_LEFT, nColWidth / 2, 4);
	m_cList.InsertColumn(5, TRL("UUID"), LVCFMT_LEFT, 0, 5);

	LPARAM dw = 0;
	dw |= LVS_EX_SI_REPORT | LVS_EX_FULLROWSELECT | LVS_EX_ONECLICKACTIVATE | LVS_EX_UNDERLINEHOT;
	dw |= LVS_EX_HEADERDRAGDROP | LVS_EX_INFOTIP | LVS_EX_GRIDLINES;
	m_cList.PostMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dw);

	DWORD i, dwDateNow, dwDate;
	PW_TIME tNow;
	PW_ENTRY *p;
	BOOL bAdded;

	_GetCurrentPwTime(&tNow);
	dwDateNow = ((DWORD)tNow.shYear << 16) | ((DWORD)tNow.btMonth << 8) | ((DWORD)tNow.btDay & 0xff);

	for(i = 0; i < m_pMgr->GetNumberOfEntries(); i++)
	{
		p = m_pMgr->GetEntry(i);
		ASSERT(p != NULL);

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

	return TRUE; // return TRUE unless you set the focus to a control
}

void CEntryListDlg::OnOK() 
{
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
	_PwTimeToString(pwe->tExpire, &strTemp);
	lvi.pszText = (LPSTR)(LPCTSTR)strTemp;
	m_cList.SetItem(&lvi);

	// Ignore m_bShowUUID, the UUID field is needed in all cases
	lvi.iSubItem = 5;
	_UuidToString(pwe->uuid, &strTemp);
	lvi.pszText = (LPSTR)(LPCTSTR)strTemp;
	m_cList.SetItem(&lvi);
}

void CEntryListDlg::OnClickEntryList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	UNREFERENCED_PARAMETER(pNMHDR);

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

	OnOK();
}
