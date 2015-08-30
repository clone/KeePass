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
#include "StatusDialogEx.h"
#include "../../KeePassLibCpp/Util/StrUtil.h"
#include "../../KeePassLibCpp/PwManager.h"

/*
#ifndef PROGDLG_MARQUEEPROGRESS
#define PROGDLG_MARQUEEPROGRESS 0x00000020
#define PROGDLG_NOCANCEL        0x00000040
#endif

CStatusDialogEx::~CStatusDialogEx()
{
	Release();
}

bool CStatusDialogEx::Show()
{
	Release();

	if(FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
	{
		ASSERT(FALSE);
		return false;
	}

	if(FAILED(CoCreateInstance(CLSID_ProgressDialog, NULL,
		CLSCTX_INPROC_SERVER, IID_IProgressDialog, (LPVOID*)&m_pDlg)))
	{
		ASSERT(FALSE);
		m_pDlg = NULL;
		CoUninitialize();
		return false;
	}

	std::basic_string<WCHAR> strTitle = _StringToUnicodeStl(PWM_PRODUCT_NAME_SHORT);
	VERIFY(SUCCEEDED(m_pDlg->SetTitle(strTitle.c_str())));

	DWORD dwFlags = (PROGDLG_NOTIME | PROGDLG_NOMINIMIZE);
	if(m_bMarquee) dwFlags |= PROGDLG_MARQUEEPROGRESS;
	if(!m_bCanCancel) dwFlags |= PROGDLG_NOCANCEL;

	VERIFY(SUCCEEDED(m_pDlg->StartProgressDialog(m_hParent, NULL, dwFlags, NULL)));
	return true;
}

void CStatusDialogEx::Release()
{
	if(m_pDlg == NULL) return;

	VERIFY(SUCCEEDED(m_pDlg->StopProgressDialog()));
	m_pDlg->Release();
	m_pDlg = NULL;

	CoUninitialize();
}

void CStatusDialogEx::SetStatus(bool bPrimary, LPCTSTR lpText)
{
	if(m_pDlg == NULL) { ASSERT(FALSE); return; }

	std::basic_string<WCHAR> str = _StringToUnicodeStl(lpText);
	m_pDlg->SetLine((bPrimary ? 1 : 2), str.c_str(), FALSE, NULL);
}

void CStatusDialogEx::SetProgress(ULONGLONG ullCompleted, ULONGLONG ullTotal)
{
	if(m_pDlg == NULL) { ASSERT(FALSE); return; }

	VERIFY(SUCCEEDED(m_pDlg->SetProgress64(ullCompleted, ullTotal)));
}

bool CStatusDialogEx::HasCancelled()
{
	if(m_pDlg == NULL) { ASSERT(FALSE); return false; }

	return ((m_pDlg->HasUserCancelled() == FALSE) ? false : true);
}
*/
