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
#include "VistaTaskDialog.h"
#include "../../../KeePassLibCpp/Util/MemUtil.h"

CVistaTaskDialog::CVistaTaskDialog(HWND hParent, HINSTANCE hInstance,
	bool bUseCommandLinks) :
	m_lpIcon(NULL), m_hIcon(NULL)
{
	m_hParent = hParent;
	m_hInstance = hInstance;
	m_bCommandLinks = bUseCommandLinks;
}

CVistaTaskDialog::~CVistaTaskDialog()
{
	this->Clear();
}

void CVistaTaskDialog::Clear()
{
	m_vButtons.clear();
}

void CVistaTaskDialog::AddButton(LPCTSTR lpCommand, LPCTSTR lpDescription, int nResult)
{
	USES_CONVERSION;

	LPCWSTR lpCommandW = ((lpCommand != NULL) ? T2CW(lpCommand) : L"");
	LPCWSTR lpDescriptionW = ((lpDescription != NULL) ? T2CW(lpDescription) : NULL);

	std::basic_string<WCHAR> strID = lpCommandW;
	if(lpDescriptionW != NULL)
	{
		strID += L"\n";
		strID += lpDescriptionW;
	}

	MY_V_TASKDIALOG_BUTTON btn;
	btn.nButtonID = nResult;
	btn.strButtonText = strID;

	m_vButtons.push_back(btn);
}

void CVistaTaskDialog::SetWindowTitle(LPCTSTR lpTitle)
{
	USES_CONVERSION;

	if(lpTitle == NULL) m_strTitle.clear();
	else m_strTitle = T2CW(lpTitle);
}

void CVistaTaskDialog::SetMainInstruction(LPCTSTR lpText)
{
	USES_CONVERSION;

	if(lpText == NULL) m_strInstr.clear();
	else m_strInstr = T2CW(lpText);
}

void CVistaTaskDialog::SetContent(LPCTSTR lpText)
{
	USES_CONVERSION;

	if(lpText == NULL) m_strContent.clear();
	else m_strContent = T2CW(lpText);
}

void CVistaTaskDialog::SetIcon(PCWSTR lpIcon)
{
	m_lpIcon = lpIcon;
}

void CVistaTaskDialog::SetIcon(MY_TASKDIALOG_ICON tdIcon)
{
	CWinApp* pApp = AfxGetApp();
	if(pApp == NULL) { ASSERT(FALSE); return; }

	if(tdIcon == V_MTDI_QUESTION)
		m_hIcon = pApp->LoadStandardIcon(IDI_QUESTION);
	else { ASSERT(FALSE); } // Unknown icon ID
}

void CVistaTaskDialog::SetVerification(LPCTSTR lpText)
{
	USES_CONVERSION;

	if(lpText == NULL) m_strConfirmation.clear();
	else m_strConfirmation = T2CW(lpText);
}

int CVistaTaskDialog::ShowDialog(BOOL* pVerificationResult)
{
	HMODULE hLib = LoadLibrary(_T("ComCtl32.dll"));
	if(hLib == NULL) return -1;

	LP_TASKDIALOGINDIRECT lpTaskDialogIndirect = (LP_TASKDIALOGINDIRECT)
		GetProcAddress(hLib, "TaskDialogIndirect");
	if(lpTaskDialogIndirect == NULL) // No task dialogs available?
	{
		VERIFY(FreeLibrary(hLib));
		return -1;
	}

	V_TASKDIALOGCONFIG cfg;
	ZeroMemory(&cfg, sizeof(V_TASKDIALOGCONFIG));

	cfg.cbSize = sizeof(V_TASKDIALOGCONFIG);
	cfg.hwndParent = m_hParent;
	cfg.hInstance = m_hInstance;
	// cfg.dwCommonButtons = V_TDCBF_CANCEL_BUTTON;

	if(m_strTitle.size() > 0) cfg.pszWindowTitle = m_strTitle.c_str();
	if(m_strInstr.size() > 0) cfg.pszMainInstruction = m_strInstr.c_str();
	if(m_strContent.size() > 0) cfg.pszContent = m_strContent.c_str();
	if(m_strConfirmation.size() > 0) cfg.pszVerificationText = m_strConfirmation.c_str();

	std::vector<V_TASKDIALOG_BUTTON> vButtons = MyButtonsToVStruct(m_vButtons);
	cfg.cButtons = static_cast<UINT>(vButtons.size());
	if(vButtons.size() > 0) cfg.pButtons = &vButtons[0];

	if(m_bCommandLinks) cfg.dwFlags |= V_TDF_USE_COMMAND_LINKS;

	cfg.pszMainIcon = m_lpIcon;
	if(m_hIcon != NULL)
	{
		cfg.hMainIcon = m_hIcon;
		cfg.dwFlags |= V_TDF_USE_HICON_MAIN;
	}

	int nResult = 0;
	if(!SUCCEEDED(lpTaskDialogIndirect(&cfg, &nResult, NULL, pVerificationResult)))
	{
		ASSERT(FALSE);
		VERIFY(FreeLibrary(hLib));
		return -1;
	}

	bool bFoundResult = false;
	for(size_t i = 0; i < m_vButtons.size(); ++i)
	{
		if(m_vButtons[i].nButtonID == nResult) bFoundResult = true;
	}
	if(bFoundResult == false) nResult = IDCANCEL;

	VERIFY(FreeLibrary(hLib));
	return nResult;
}

inline std::vector<V_TASKDIALOG_BUTTON> CVistaTaskDialog::MyButtonsToVStruct(
	const std::vector<MY_V_TASKDIALOG_BUTTON>& vButtons)
{
	std::vector<V_TASKDIALOG_BUTTON> v;

	for(size_t i = 0; i < vButtons.size(); ++i)
	{
		V_TASKDIALOG_BUTTON btn;
		btn.nButtonID = vButtons[i].nButtonID;
		btn.pszButtonText = vButtons[i].strButtonText.c_str();
		v.push_back(btn);
	}

	return v;
}
