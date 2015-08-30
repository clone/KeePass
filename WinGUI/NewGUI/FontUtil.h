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

#ifndef ___FONT_UTIL_H___
#define ___FONT_UTIL_H___

#pragma once

#include <afxwin.h>
#include <boost/utility.hpp>

#define CFU_MONO_FONT_FACE _T("Courier New")

class CFontUtil : boost::noncopyable
{
public:
	static void Release();

	static void SetDefaultFont(CFont* pf);
	static void SetDefaultFontFrom(CWnd* pWnd);

	static void SetPasswordFont(LPCTSTR lpFont, HWND hWndParent);

	static CFont* GetMonoFont(CWnd* pWndParent);

	static void AssignBold(CWnd* pWnd, CWnd* pWndParent);
	static void AssignMono(CWnd* pWnd, CWnd* pWndParent);
	static void AssignSymbol(CWnd* pWnd, CWnd* pWndParent);
	static void AssignPassword(CWnd* pWnd, CWnd* pWndParent);

	static CString Serialize(const CFontDialog& dlg);
	static bool Deserialize(LOGFONT* pFont, LPCTSTR lpFont, HWND hWnd);

private:
	CFontUtil();

	static bool EnsureBold(CWnd* pWndParent);
	static bool EnsureMono(CWnd* pWndParent);
	static bool EnsureSymbol(CWnd* pWndParent);

	static LOGFONT* g_plfDefault;
	static CFont* g_pfBold;
	static CFont* g_pfMono;
	static CFont* g_pfSymbol;
	static CFont* g_pfPassword;
};

#endif // ___FONT_UTIL_H___
