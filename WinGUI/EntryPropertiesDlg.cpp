/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2015 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "EntryPropertiesDlg.h"

#include "IconPickerDlg.h"
#include "../KeePassLibCpp/Util/TranslateEx.h"
#include "../KeePassLibCpp/Util/PwUtil.h"
#include "NewGUI/NewGUICommon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CEntryPropertiesDlg::CEntryPropertiesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEntryPropertiesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEntryPropertiesDlg)
	m_bModExpire = FALSE;
	m_bModGroup = FALSE;
	m_bModIcon = FALSE;
	//}}AFX_DATA_INIT

	m_pMgr = NULL;
	m_pParentIcons = NULL;
	m_dwDefaultExpire = 0;
	m_nIconId = 0;
}

void CEntryPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEntryPropertiesDlg)
	DDX_Control(pDX, IDC_SETDEFAULTEXPIRE_BTN, m_btSetToDefaultExpire);
	DDX_Control(pDX, IDC_SELDEFEXPIRES_BTN, m_btSelDefExpires);
	DDX_Control(pDX, IDC_COMBO_GROUPS, m_cbGroups);
	DDX_Control(pDX, IDC_BUTTON_SELECT_ICON, m_btSelectIcon);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDC_EDIT_EXPIRE_TIME, m_editTime);
	DDX_Control(pDX, IDC_EDIT_EXPIRE_DATE, m_editDate);
	DDX_Check(pDX, IDC_CHECK_MODEXPIRE, m_bModExpire);
	DDX_Check(pDX, IDC_CHECK_MODGROUP, m_bModGroup);
	DDX_Check(pDX, IDC_CHECK_MODICON, m_bModIcon);
	DDX_Control(pDX, IDC_RADIO_NOEXPIRE, m_btRadioNoExpire);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEntryPropertiesDlg, CDialog)
	//{{AFX_MSG_MAP(CEntryPropertiesDlg)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_ICON, OnButtonSelectIcon)
	ON_BN_CLICKED(IDC_CHECK_MODEXPIRE, OnCheckModExpire)
	ON_BN_CLICKED(IDC_CHECK_MODGROUP, OnCheckModGroup)
	ON_BN_CLICKED(IDC_CHECK_MODICON, OnCheckModIcon)
	ON_BN_CLICKED(IDC_SETDEFAULTEXPIRE_BTN, OnSetDefaultExpireBtn)
	ON_COMMAND(ID_EXPIRES_1WEEK, OnExpires1Week)
	ON_COMMAND(ID_EXPIRES_2WEEKS, OnExpires2Weeks)
	ON_COMMAND(ID_EXPIRES_1MONTH, OnExpires1Month)
	ON_COMMAND(ID_EXPIRES_3MONTHS, OnExpires3Months)
	ON_COMMAND(ID_EXPIRES_6MONTHS, OnExpires6Months)
	ON_COMMAND(ID_EXPIRES_12MONTHS, OnExpires12Months)
	ON_COMMAND(ID_EXPIRES_NOW, OnExpiresNow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CEntryPropertiesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	ASSERT(m_pMgr != NULL); // You must set the manager before using this class
	ASSERT(m_pParentIcons != NULL); // You must set the image list first!

	// Translate all windows
	EnumChildWindows(this->m_hWnd, NewGUI_TranslateWindowCb, 0);

	NewGUI_XPButton(m_btOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(m_btCancel, IDB_CANCEL, IDB_CANCEL);
	NewGUI_XPButton(m_btSelectIcon, -1, -1);
	NewGUI_XPButton(m_btSetToDefaultExpire, IDB_TB_DEFAULTEXPIRE, IDB_TB_DEFAULTEXPIRE, TRUE);
	NewGUI_XPButton(m_btSelDefExpires, IDB_CLOCK, IDB_CLOCK, TRUE);

	CString strTT = TRL("Change expiration time:");
	strTT.Remove(_T(':')); strTT += _T(".");
	m_btSelDefExpires.SetTooltipText(strTT);

	m_btSelDefExpires.SetMenu(IDR_EXPIRESMENU, this->m_hWnd, TRUE, NULL, CSize(16, 15));

	m_btSelectIcon.SetTooltipText(TRL("Choose an icon."), TRUE);
	if((m_nIconId >= 0) && (m_pParentIcons != NULL))
		m_btSelectIcon.SetIcon(m_pParentIcons->ExtractIcon(m_nIconId));

	if(m_dwDefaultExpire != 0)
	{
		CString str, strTemp;
		str.Format(TRL("Click to expire the entry in %u days"), m_dwDefaultExpire);

		CTime t = CTime::GetCurrentTime();
		t += CTimeSpan((LONG)m_dwDefaultExpire, 0, 0, 0);

		strTemp.Format(_T(" (%04d-%02d-%02d)"), t.GetYear(), t.GetMonth(), t.GetDay());
		str += strTemp + _T(".");

		m_btSetToDefaultExpire.SetTooltipText(str);
	}

	// Set the imagelist for the group selector combo box
	m_cbGroups.SetImageList(m_pParentIcons);

	ASSERT(m_pMgr != NULL); // Must have been initialized by parent
	for(DWORD i = 0; i < m_pMgr->GetNumberOfGroups(); i++) // Add groups to combo box
	{
		PW_GROUP *p = m_pMgr->GetGroup(i);
		ASSERT(p != NULL);

		COMBOBOXEXITEM cbi;
		ZeroMemory(&cbi, sizeof(COMBOBOXEXITEM));
		cbi.mask = CBEIF_IMAGE | CBEIF_TEXT | CBEIF_INDENT | CBEIF_SELECTEDIMAGE;
		cbi.iItem = static_cast<int>(i);
		cbi.pszText = p->pszGroupName;
		cbi.cchTextMax = static_cast<int>(_tcslen(p->pszGroupName));
		cbi.iImage = cbi.iSelectedImage = static_cast<int>(p->uImageId);
		cbi.iIndent = static_cast<int>(p->usLevel);
		m_cbGroups.InsertItem(&cbi);
	}

	m_cbGroups.SetCurSel(0);

	// Configure banner control
	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_KEY), KCSB_ICON_LEFT | KCSB_ICON_VCENTER);

	m_banner.SetTitle(TRL("Mass Modify"));
	CString strBannerCap = TRL("Change fields of all selected entries"); strBannerCap += _T(".");
	m_banner.SetCaption(strBannerCap);
	SetWindowText(TRL("Mass Modify"));

	SYSTEMTIME tNow;
	ZeroMemory(&tNow, sizeof(SYSTEMTIME));
	::GetLocalTime(&tNow);
	COleDateTime oleNow(tNow);

	COleDateTime oleMin = AMS_MIN_OLEDATETIME;
	COleDateTime oleMax(2999, 12, 28, 23, 59, 59);
	m_editDate.SetRange(oleMin, oleMax);
	m_editTime.SetRange(oleMin, oleMax);

	m_editDate.SetDate(oleNow);
	m_editTime.SetAMPM(true);
	m_editTime.Show24HourFormat(true);
	m_editTime.ShowSeconds(true);
	m_editTime.SetTime(oleNow);

	m_bModGroup = FALSE;
	m_cbGroups.EnableWindow(FALSE);
	m_bModIcon = FALSE;
	GetDlgItem(IDC_BUTTON_SELECT_ICON)->EnableWindow(FALSE);
	m_bModExpire = FALSE;

	UpdateData(FALSE);
	::SendMessage(GetDlgItem(IDC_RADIO_EXPIRES)->m_hWnd, BM_SETCHECK, BST_CHECKED, 0);
	OnCheckModExpire();

	if(m_dwDefaultExpire == 0) m_btSetToDefaultExpire.EnableWindow(FALSE); // Unchangable
	else m_btSetToDefaultExpire.EnableWindow(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CEntryPropertiesDlg::OnOK() 
{
	UpdateData(TRUE);

	if(m_bModGroup == TRUE)
	{
		CString strGroupTest;
		m_cbGroups.GetLBText(m_cbGroups.GetCurSel(), strGroupTest);
		if(CPwUtil::IsAllowedStoreGroup((LPCTSTR)strGroupTest, PWS_SEARCHGROUP) == FALSE)
		{
			MessageBox(TRL("The group you selected cannot store entries. Please select a different group."),
				PWM_PRODUCT_NAME_SHORT, MB_ICONWARNING | MB_OK);
			return;
		}
	}

	if(m_editDate.GetWindowTextLength() == 0) m_editDate.SetDate(2999, 12, 28);
	if(m_editTime.GetWindowTextLength() == 0) m_editTime.SetTime(23, 59, 59);

	if((m_btRadioNoExpire.GetCheck() & BST_CHECKED) != 0)
		CPwManager::GetNeverExpireTime(&m_tExpire);
	else
	{
		m_tExpire.shYear = static_cast<USHORT>(m_editDate.GetYear());
		m_tExpire.btMonth = static_cast<BYTE>(m_editDate.GetMonth());
		m_tExpire.btDay = static_cast<BYTE>(m_editDate.GetDay());
		m_tExpire.btHour = static_cast<BYTE>(m_editTime.GetHour());
		m_tExpire.btMinute = static_cast<BYTE>(m_editTime.GetMinute());
		m_tExpire.btSecond = static_cast<BYTE>(m_editTime.GetSecond());
	}

	m_nGroupInx = m_cbGroups.GetCurSel();

	CDialog::OnOK();
}

void CEntryPropertiesDlg::OnCancel() 
{
	CDialog::OnCancel();
}

void CEntryPropertiesDlg::OnButtonSelectIcon() 
{
	CIconPickerDlg dlg;
	dlg.m_pImageList = m_pParentIcons;
	dlg.m_uNumIcons = static_cast<UINT>(m_pParentIcons->GetImageCount());

	if(NewGUI_DoModal(&dlg) == IDOK)
	{
		m_nIconId = dlg.m_nSelectedIcon;

		if((m_nIconId >= 0) && (m_pParentIcons != NULL))
			m_btSelectIcon.SetIcon(m_pParentIcons->ExtractIcon(m_nIconId));
	}
}

void CEntryPropertiesDlg::OnCheckModExpire() 
{
	UpdateData(TRUE);

	m_btRadioNoExpire.EnableWindow(m_bModExpire);
	GetDlgItem(IDC_RADIO_EXPIRES)->EnableWindow(m_bModExpire);
	m_editTime.EnableWindow(m_bModExpire);
	m_editDate.EnableWindow(m_bModExpire);
	m_btSelDefExpires.EnableWindow(m_bModExpire);
}

void CEntryPropertiesDlg::OnCheckModGroup() 
{
	UpdateData(TRUE);
	
	m_cbGroups.EnableWindow(m_bModGroup);
}

void CEntryPropertiesDlg::OnCheckModIcon() 
{
	UpdateData(TRUE);
	
	m_btSelectIcon.EnableWindow(m_bModIcon);
}

void CEntryPropertiesDlg::OnSetDefaultExpireBtn() 
{
	if(m_dwDefaultExpire == 0) return;
	SetExpireDays(m_dwDefaultExpire, FALSE);
}

void CEntryPropertiesDlg::SetExpireDays(DWORD dwDays, BOOL bSetTime)
{
	UpdateData(TRUE);

	m_bModExpire = TRUE;

	CTime t = CTime::GetCurrentTime();
	t += CTimeSpan((LONG)dwDays, 0, 0, 0);

	m_editDate.SetDate(t); // Beware of daylight saving
	if(bSetTime == TRUE) m_editTime.SetTime(t);

	UpdateData(FALSE);
	OnCheckModExpire();
}

void CEntryPropertiesDlg::OnExpiresNow() 
{
	SetExpireDays(0, TRUE);
}

void CEntryPropertiesDlg::OnExpires1Week() 
{
	SetExpireDays(7, FALSE);
}

void CEntryPropertiesDlg::OnExpires2Weeks() 
{
	SetExpireDays(14, FALSE);
}

void CEntryPropertiesDlg::OnExpires1Month() 
{
	SetExpireDays(30, FALSE);
}

void CEntryPropertiesDlg::OnExpires3Months() 
{
	SetExpireDays(91, FALSE);
}

void CEntryPropertiesDlg::OnExpires6Months() 
{
	SetExpireDays(182, FALSE);
}

void CEntryPropertiesDlg::OnExpires12Months() 
{
	SetExpireDays(365, FALSE);
}
