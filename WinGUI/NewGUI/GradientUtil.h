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

#ifndef ___GRADIENT_UTIL_H___
#define ___GRADIENT_UTIL_H___

#include <boost/utility.hpp>
#include <windows.h>

#define GU_GF_LIB_NAME _T("MsImg32.dll")
#define GU_GF_FN_NAME "GradientFill"

typedef BOOL(WINAPI* LPFNGRADIENTFILL)(HDC hdc, PTRIVERTEX pVertex, ULONG dwNumVertex,
	PVOID pMesh, ULONG dwNumMesh, ULONG dwMode);

class CGradientUtil : boost::noncopyable
{
public:
	static void Release();

	static bool IsSupported();

	static bool DrawGradient(HDC hdc, LONG x1, LONG y1, LONG x2, LONG y2,
		COLORREF clrBase, bool bVertical);

private:
	CGradientUtil();

	static bool EnsureInitialized();

	static bool m_bInitialized;
	static HMODULE m_hImgLib;
	static LPFNGRADIENTFILL m_lpGradientFill;
};

#endif // ___GRADIENT_UTIL_H___
