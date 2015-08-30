/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2013 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "../../../KeePassLibCpp/Util/StrUtil.h"
#include "../../../KeePassLibCpp/Util/MemUtil.h"
#include "../../../KeePassLibCpp/PwManager.h"

CVistaTaskDialog::CVistaTaskDialog(HWND hParent, HINSTANCE hInstance,
	bool bUseCommandLinks) :
	m_lpIcon(NULL), m_hIcon(NULL), m_bHyperLinks(false), m_pfCallback(NULL),
	m_lpFooterIcon(NULL)
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
	const std::basic_string<WCHAR> strCommand = ((lpCommand != NULL) ?
		_StringToUnicodeStl(lpCommand) : std::basic_string<WCHAR>());
	const std::basic_string<WCHAR> strDescription = ((lpDescription != NULL) ?
		_StringToUnicodeStl(lpDescription) : std::basic_string<WCHAR>());

	std::basic_string<WCHAR> strID = strCommand;
	if(lpDescription != NULL)
	{
		strID += L"\n";
		strID += strDescription;
	}

	MY_TASKDIALOG_BUTTON btn;
	btn.nButtonID = nResult;
	btn.strButtonText = strID;

	m_vButtons.push_back(btn);
}

void CVistaTaskDialog::SetWindowTitle(LPCTSTR lpTitle)
{
	if(lpTitle == NULL) m_strTitle.clear();
	else m_strTitle = _StringToUnicodeStl(lpTitle);
}

void CVistaTaskDialog::SetMainInstruction(LPCTSTR lpText)
{
	if(lpText == NULL) m_strInstr.clear();
	else m_strInstr = _StringToUnicodeStl(lpText);
}

void CVistaTaskDialog::SetContent(LPCTSTR lpText)
{
	if(lpText == NULL) m_strContent.clear();
	else m_strContent = _StringToUnicodeStl(lpText);
}

void CVistaTaskDialog::SetIcon(PCWSTR lpIcon)
{
	m_lpIcon = lpIcon;
}

void CVistaTaskDialog::SetIcon(MY_TASKDIALOG_ICON tdIcon)
{
	CWinApp* pApp = AfxGetApp();
	if(pApp == NULL) { ASSERT(FALSE); return; }

	if(tdIcon == MTDI_QUESTION)
		m_hIcon = pApp->LoadStandardIcon(IDI_QUESTION);
	else { ASSERT(FALSE); } // Unknown icon ID
}

void CVistaTaskDialog::SetVerification(LPCTSTR lpText)
{
	if(lpText == NULL) m_strConfirmation.clear();
	else m_strConfirmation = _StringToUnicodeStl(lpText);
}

void CVistaTaskDialog::SetExpandedText(LPCTSTR lpText)
{
	if(lpText == NULL) m_strExpandedText.clear();
	else m_strExpandedText = _StringToUnicodeStl(lpText);
}

void CVistaTaskDialog::SetFooterText(LPCTSTR lpText)
{
	if(lpText == NULL) m_strFooter.clear();
	else m_strFooter = _StringToUnicodeStl(lpText);
}

void CVistaTaskDialog::SetFooterIcon(PCWSTR lpIcon)
{
	m_lpFooterIcon = lpIcon;
}

void CVistaTaskDialog::EnableHyperLinks(bool bEnable)
{
	m_bHyperLinks = bEnable;
}

void CVistaTaskDialog::SetCallback(V_PFTASKDIALOGCALLBACK pf)
{
	m_pfCallback = pf;
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

	TASKDIALOGCONFIG cfg;
	ZeroMemory(&cfg, sizeof(TASKDIALOGCONFIG));

	cfg.cbSize = sizeof(TASKDIALOGCONFIG);
	cfg.hwndParent = m_hParent;
	cfg.hInstance = m_hInstance;
	// cfg.dwCommonButtons = TDCBF_CANCEL_BUTTON;
	cfg.pfCallback = m_pfCallback;

	if(m_strTitle.size() > 0) cfg.pszWindowTitle = m_strTitle.c_str();
	if(m_strInstr.size() > 0) cfg.pszMainInstruction = m_strInstr.c_str();
	if(m_strContent.size() > 0) cfg.pszContent = m_strContent.c_str();
	if(m_strConfirmation.size() > 0) cfg.pszVerificationText = m_strConfirmation.c_str();
	if(m_strExpandedText.size() > 0) cfg.pszExpandedInformation = m_strExpandedText.c_str();
	if(m_strFooter.size() > 0) cfg.pszFooter = m_strFooter.c_str();

	std::vector<TASKDIALOG_BUTTON> vButtons = MyButtonsToVStruct(m_vButtons);
	cfg.cButtons = static_cast<UINT>(vButtons.size());
	if(vButtons.size() > 0) cfg.pButtons = &vButtons[0];

	if(m_bCommandLinks) cfg.dwFlags |= TDF_USE_COMMAND_LINKS;
	if(m_bHyperLinks) cfg.dwFlags |= TDF_ENABLE_HYPERLINKS;

	cfg.pszMainIcon = m_lpIcon;
	if(m_hIcon != NULL)
	{
		cfg.hMainIcon = m_hIcon;
		cfg.dwFlags |= TDF_USE_HICON_MAIN;
	}

	cfg.pszFooterIcon = m_lpFooterIcon;

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

inline std::vector<TASKDIALOG_BUTTON> CVistaTaskDialog::MyButtonsToVStruct(
	const std::vector<MY_TASKDIALOG_BUTTON>& vButtons)
{
	std::vector<TASKDIALOG_BUTTON> v;

	for(size_t i = 0; i < vButtons.size(); ++i)
	{
		TASKDIALOG_BUTTON btn;
		btn.nButtonID = vButtons[i].nButtonID;
		btn.pszButtonText = vButtons[i].strButtonText.c_str();
		v.push_back(btn);
	}

	return v;
}

int CVistaTaskDialog::ShowMessageBox(HWND hParent, LPCTSTR lpMainInstruction,
	LPCTSTR lpContent, MY_TASKDIALOG_ICON tdIcon, LPCTSTR lpButton1, int nResult1,
	LPCTSTR lpButton2, int nResult2)
{
	CVistaTaskDialog dlg(hParent, AfxGetInstanceHandle(), false);
	dlg.AddButton(lpButton1, NULL, nResult1);
	dlg.AddButton(lpButton2, NULL, nResult2);
	dlg.SetContent(lpContent);
	dlg.SetIcon(tdIcon);
	dlg.SetMainInstruction(lpMainInstruction);
	dlg.SetWindowTitle(PWM_PRODUCT_NAME_SHORT);

	return dlg.ShowDialog(NULL);
}
