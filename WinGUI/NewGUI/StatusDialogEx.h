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

#ifndef ___STATUS_DIALOG_EX_H___
#define ___STATUS_DIALOG_EX_H___

#pragma once

/*
#include "../../KeePassLibCpp/SysDefEx.h"
#include "NewGUICommon.h"
#include <shlobj.h>
#include <boost/utility.hpp>

class CStatusDialogEx : boost::noncopyable
{
public:
	CStatusDialogEx(HWND hParent, bool bCanCancel, bool bMarquee) :
		m_pDlg(NULL), m_hParent(hParent), m_bCanCancel(bCanCancel),
		m_bMarquee(bMarquee)
	{
	}

	virtual ~CStatusDialogEx();

	bool Show();
	void Release();

	void SetStatus(bool bPrimary, LPCTSTR lpText);

	void SetProgress(ULONGLONG ullCompleted, ULONGLONG ullTotal);
	bool HasCancelled();

private:
	HWND m_hParent;
	bool m_bCanCancel;
	bool m_bMarquee;

	IProgressDialog* m_pDlg;
};
*/

#endif // ___STATUS_DIALOG_EX_H___
