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
#include "CustomComboBoxEx.h"
#include "../../KeePassLibCpp/Util/AppUtil.h"

bool CCustomComboBoxEx::_CanHandleKey(TCHAR tchKey)
{
	// The feature is supported natively by Windows Vista
	if(AU_IsAtLeastWinVistaSystem() == TRUE) return false;

	return ((tchKey >= _T('A')) && (tchKey <= _T('Z')));
}

bool CCustomComboBoxEx::_HandleKeyDown(TCHAR tchKey)
{
	if(_CanHandleKey(tchKey) == false) return false;

	const int nItems = this->GetCount();

	int nCurSel = this->GetCurSel();
	if(nCurSel == CB_ERR) nCurSel = nItems;

	for(int i = 0; i < nItems; ++i)
	{
		const int iItem = ((nCurSel + i + 1) % nItems);

		CString strText;
		this->GetLBText(iItem, strText);

		if(strText.GetLength() > 0)
		{
			const TCHAR tchStart = static_cast<TCHAR>(toupper(strText.GetAt(0)));

			if(tchStart == tchKey)
			{
				VERIFY(this->SetCurSel(iItem) == iItem);
				break;
			}
		}
	}

	return true;
}

bool CCustomComboBoxEx::_HandleKeyUp(TCHAR tchKey)
{
	return _CanHandleKey(tchKey); // Ignore handled keys
}

BOOL CCustomComboBoxEx::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(_HandleKeyDown(static_cast<TCHAR>(pMsg->wParam))) return TRUE;
	}
	else if((pMsg->message == WM_KEYUP) || (pMsg->message == WM_CHAR))
	{
		if(_HandleKeyUp(static_cast<TCHAR>(pMsg->wParam))) return TRUE;
	}

	return CComboBoxEx::PreTranslateMessage(pMsg);
}
