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

#ifndef ___VISTA_TASK_DIALOG_H___
#define ___VISTA_TASK_DIALOG_H___

#pragma once

#include <windows.h>
#include <tchar.h>
#include <string>
#include <vector>

enum MY_TASKDIALOG_ICON
{
	MTDI_QUESTION = 0
};

typedef struct _MY_TASKDIALOG_BUTTON
{
	int nButtonID;
	std::basic_string<WCHAR> strButtonText;
} MY_TASKDIALOG_BUTTON;

typedef HRESULT(CALLBACK* V_PFTASKDIALOGCALLBACK)(HWND hwnd, UINT uNotification,
	WPARAM wParam, LPARAM lParam, LONG_PTR dwRefData);
typedef HRESULT(WINAPI *LP_TASKDIALOGINDIRECT)(const TASKDIALOGCONFIG *pTaskConfig,
	int *pnButton, int *pnRadioButton, BOOL *pfVerificationFlagChecked);

class CVistaTaskDialog
{
public:
	CVistaTaskDialog(HWND hParent = NULL, HINSTANCE hInstance = NULL,
		bool bUseCommandLinks = false);
	virtual ~CVistaTaskDialog();

	void AddButton(LPCTSTR lpCommand, LPCTSTR lpDescription, int nResult);

	void SetWindowTitle(LPCTSTR lpTitle);
	void SetMainInstruction(LPCTSTR lpText);
	void SetContent(LPCTSTR lpText);
	void SetIcon(PCWSTR lpIcon);
	void SetIcon(MY_TASKDIALOG_ICON tdIcon);
	void SetCommandLinks(bool bUseCommandLinks) { m_bCommandLinks = bUseCommandLinks; }
	void SetVerification(LPCTSTR lpText);
	void SetExpandedText(LPCTSTR lpText);
	void SetFooterText(LPCTSTR lpText);
	void SetFooterIcon(PCWSTR lpIcon);

	void EnableHyperLinks(bool bEnable);

	void SetCallback(V_PFTASKDIALOGCALLBACK pf);

	int ShowDialog(BOOL* pVerificationResult = NULL);

	static int ShowMessageBox(HWND hParent, LPCTSTR lpMainInstruction,
		LPCTSTR lpContent, MY_TASKDIALOG_ICON tdIcon, LPCTSTR lpButton1, int nResult1,
		LPCTSTR lpButton2, int nResult2);

private:
	void Clear();
	inline static std::vector<TASKDIALOG_BUTTON> MyButtonsToVStruct(const
		std::vector<MY_TASKDIALOG_BUTTON>& vButtons);

	HWND m_hParent;
	HINSTANCE m_hInstance;
	PCWSTR m_lpIcon;
	HICON m_hIcon;
	bool m_bCommandLinks;
	bool m_bHyperLinks;
	std::basic_string<WCHAR> m_strTitle;
	std::basic_string<WCHAR> m_strInstr;
	std::basic_string<WCHAR> m_strContent;
	std::basic_string<WCHAR> m_strConfirmation;
	std::basic_string<WCHAR> m_strExpandedText;
	std::basic_string<WCHAR> m_strFooter;
	PCWSTR m_lpFooterIcon;
	std::vector<MY_TASKDIALOG_BUTTON> m_vButtons;
	V_PFTASKDIALOGCALLBACK m_pfCallback;
};

#endif // ___VISTA_TASK_DIALOG_H___
