/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2010 Dominik Reichl <dominik.reichl@t-online.de>

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

enum _V_TASKDIALOG_FLAGS
{
	V_TDF_ENABLE_HYPERLINKS           = 0x0001,
	V_TDF_USE_HICON_MAIN              = 0x0002,
	V_TDF_USE_HICON_FOOTER            = 0x0004,
	V_TDF_ALLOW_DIALOG_CANCELLATION   = 0x0008,
	V_TDF_USE_COMMAND_LINKS           = 0x0010,
	V_TDF_USE_COMMAND_LINKS_NO_ICON   = 0x0020,
	V_TDF_EXPAND_FOOTER_AREA          = 0x0040,
	V_TDF_EXPANDED_BY_DEFAULT         = 0x0080,
	V_TDF_VERIFICATION_FLAG_CHECKED   = 0x0100,
	V_TDF_SHOW_PROGRESS_BAR           = 0x0200,
	V_TDF_SHOW_MARQUEE_PROGRESS_BAR   = 0x0400,
	V_TDF_CALLBACK_TIMER              = 0x0800,
	V_TDF_POSITION_RELATIVE_TO_WINDOW = 0x1000,
	V_TDF_RTL_LAYOUT                  = 0x2000,
	V_TDF_NO_DEFAULT_RADIO_BUTTON     = 0x4000
};

typedef DWORD V_TASKDIALOG_FLAGS;

enum _V_TASKDIALOG_COMMON_BUTTON_FLAGS
{
	V_TDCBF_OK_BUTTON     = 0x0001, // Return value: IDOK
	V_TDCBF_YES_BUTTON    = 0x0002, // Return value: IDYES
	V_TDCBF_NO_BUTTON     = 0x0004, // Return value: IDNO
	V_TDCBF_CANCEL_BUTTON = 0x0008, // Return value: IDCANCEL
	V_TDCBF_RETRY_BUTTON  = 0x0010, // Return value: IDRETRY
	V_TDCBF_CLOSE_BUTTON  = 0x0020  // Return value: IDCLOSE
};

enum _V_TASKDIALOG_NOTIFICATIONS
{
	V_TDN_CREATED                = 0,
	V_TDN_NAVIGATED              = 1,
	V_TDN_BUTTON_CLICKED         = 2,
	V_TDN_HYPERLINK_CLICKED      = 3,
	V_TDN_TIMER                  = 4,
	V_TDN_DESTROYED              = 5,
	V_TDN_RADIO_BUTTON_CLICKED   = 6,
	V_TDN_DIALOG_CONSTRUCTED     = 7,
	V_TDN_VERIFICATION_CLICKED   = 8,
	V_TDN_HELP                   = 9,
	V_TDN_EXPANDO_BUTTON_CLICKED = 10
};

enum MY_TASKDIALOG_ICON
{
	V_MTDI_QUESTION = 0
};

typedef DWORD V_TASKDIALOG_COMMON_BUTTON_FLAGS;

#define V_TD_WARNING_ICON     MAKEINTRESOURCEW(-1)
#define V_TD_ERROR_ICON       MAKEINTRESOURCEW(-2)
#define V_TD_INFORMATION_ICON MAKEINTRESOURCEW(-3)
#define V_TD_SHIELD_ICON      MAKEINTRESOURCEW(-4)

typedef struct _V_TASKDIALOG_BUTTON
{
	int nButtonID;
	PCWSTR pszButtonText;
} V_TASKDIALOG_BUTTON;

typedef struct _MY_V_TASKDIALOG_BUTTON
{
	int nButtonID;
	std::basic_string<WCHAR> strButtonText;
} MY_V_TASKDIALOG_BUTTON;

typedef HRESULT(CALLBACK* V_PFTASKDIALOGCALLBACK)(HWND hwnd, UINT uNotification,
	WPARAM wParam, LPARAM lParam, LONG_PTR dwRefData);

typedef struct _V_TASKDIALOGCONFIG
{
	UINT cbSize;
	HWND hwndParent;
	HINSTANCE hInstance;
	V_TASKDIALOG_FLAGS dwFlags;
	V_TASKDIALOG_COMMON_BUTTON_FLAGS dwCommonButtons;
	PCWSTR pszWindowTitle;
	union
	{
		HICON hMainIcon;
		PCWSTR pszMainIcon;
	};
	PCWSTR pszMainInstruction;
	PCWSTR pszContent;
	UINT cButtons;
	const V_TASKDIALOG_BUTTON *pButtons;
	int nDefaultButton;
	UINT cRadioButtons;
	const V_TASKDIALOG_BUTTON *pRadioButtons;
	int nDefaultRadioButton;
	PCWSTR pszVerificationText;
	PCWSTR pszExpandedInformation;
	PCWSTR pszExpandedControlText;
	PCWSTR pszCollapsedControlText;
	union
	{
		HICON hFooterIcon;
		PCWSTR pszFooterIcon;
	};
	PCWSTR pszFooter;
	V_PFTASKDIALOGCALLBACK pfCallback;
	LONG_PTR lpCallbackData;
	UINT cxWidth;
} V_TASKDIALOGCONFIG;

typedef HRESULT(WINAPI *LP_TASKDIALOGINDIRECT)(const V_TASKDIALOGCONFIG *pTaskConfig,
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
	void SetFooter(LPCTSTR lpText);

	void EnableHyperLinks(bool bEnable);

	void SetCallback(V_PFTASKDIALOGCALLBACK pf);

	int ShowDialog(BOOL* pVerificationResult = NULL);

private:
	void Clear();
	inline static std::vector<V_TASKDIALOG_BUTTON> MyButtonsToVStruct(const
		std::vector<MY_V_TASKDIALOG_BUTTON>& vButtons);

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
	std::vector<MY_V_TASKDIALOG_BUTTON> m_vButtons;
	V_PFTASKDIALOGCALLBACK m_pfCallback;
};

#endif // ___VISTA_TASK_DIALOG_H___
