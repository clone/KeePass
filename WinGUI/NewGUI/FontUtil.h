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

#ifndef ___FONT_UTIL_H___
#define ___FONT_UTIL_H___

#pragma once

#include <afxwin.h>
#include <boost/utility.hpp>

class CFontUtil : boost::noncopyable
{
public:
	static void Release();

	static void SetDefaultFont(CFont* pf);
	static void SetDefaultFontFrom(CWnd* pWnd);

	static void AssignBold(CWnd* pWnd, CWnd* pWndParent);
	static void AssignMono(CWnd* pWnd, CWnd* pWndParent);
	static void AssignSymbol(CWnd* pWnd, CWnd* pWndParent);

private:
	CFontUtil();

	static bool EnsureBold(CWnd* pWndParent);
	static bool EnsureMono(CWnd* pWndParent);
	static bool EnsureSymbol(CWnd* pWndParent);

	static LOGFONT* g_plfDefault;
	static CFont* g_pfBold;
	static CFont* g_pfMono;
	static CFont* g_pfSymbol;
};

#endif // ___FONT_UTIL_H___
