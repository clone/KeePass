/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2014 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "NewDialogsEx.h"
#include "TaskDialog/VistaTaskDialog.h"
#include "../Util/WinUtil.h"
#include "../../KeePassLibCpp/Util/PwUtil.h"
#include "../../KeePassLibCpp/Util/TranslateEx.h"

HRESULT CALLBACK NDE_HelpManualCallback(HWND hwnd, UINT uNotification,
	WPARAM wParam, LPARAM lParam, LONG_PTR dwRefData)
{
	UNREFERENCED_PARAMETER(hwnd);
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(dwRefData);

	if(uNotification == TDN_HYPERLINK_CLICKED)
	{
		LPCWSTR lpHRef = (LPCWSTR)lParam;
		if((lpHRef != NULL) && (*lpHRef == L'h') && (lpHRef[1] == 0))
			WU_OpenAppHelp(PWM_HELP_ERRORS, NULL);
		else { ASSERT(FALSE); }
	}

	return S_OK;
}

CNewDialogsEx::CNewDialogsEx()
{
}

void CNewDialogsEx::ShowError(HWND hParent, int nErrorCode, DWORD dwFlags)
{
	CString strText = CPwUtil::FormatError(nErrorCode, dwFlags | PWFF_MAIN_TEXT_ONLY);

	TCHAR tszTemp[24];
	_stprintf_s(tszTemp, _countof(tszTemp), _T("%08X"),
		static_cast<unsigned int>(nErrorCode));

	CString strMore = TRL("Error code");
	strMore += _T(": 0x");
	strMore += tszTemp;
	strMore += _T(", ");
	strMore += TRL("see the <A HREF=\"h\">help manual</A> for details.");

	CVistaTaskDialog dlg(hParent, AfxGetInstanceHandle(), false);
	dlg.SetWindowTitle(PWM_PRODUCT_NAME_SHORT);
	dlg.SetContent(strText);
	dlg.SetIcon(TD_WARNING_ICON);
	dlg.SetFooterText(strMore);
	dlg.EnableHyperLinks(true);
	dlg.SetCallback(NDE_HelpManualCallback);

	if(dlg.ShowDialog(NULL) < 0)
		MessageBox(hParent, CPwUtil::FormatError(nErrorCode, dwFlags),
			PWM_PRODUCT_NAME_SHORT, MB_ICONWARNING | MB_OK);
}
