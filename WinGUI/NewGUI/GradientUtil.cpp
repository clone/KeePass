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

#include "StdAfx.h"
#include "GradientUtil.h"
#include "BCMenu.h"

bool CGradientUtil::m_bInitialized = false;
HMODULE CGradientUtil::m_hImgLib = NULL;
LPFNGRADIENTFILL CGradientUtil::m_lpGradientFill = NULL;

CGradientUtil::CGradientUtil()
{
}

void CGradientUtil::Release()
{
	m_bInitialized = false;
	if(m_hImgLib != NULL) { VERIFY(FreeLibrary(m_hImgLib)); m_hImgLib = NULL; }
	m_lpGradientFill = NULL;
}

bool CGradientUtil::EnsureInitialized()
{
	if(m_bInitialized) return (m_hImgLib != NULL);

	m_bInitialized = true;

	m_hImgLib = LoadLibrary(GU_GF_LIB_NAME);
	if(m_hImgLib != NULL)
	{
		m_lpGradientFill = (LPFNGRADIENTFILL)GetProcAddress(m_hImgLib, GU_GF_FN_NAME);
		if(m_lpGradientFill == NULL) { ASSERT(FALSE); CGradientUtil::Release(); return false; }
	}
	else return false;

	return true;
}

bool CGradientUtil::IsSupported()
{
	return CGradientUtil::EnsureInitialized();
}

bool CGradientUtil::DrawGradient(HDC hdc, LONG x1, LONG y1, LONG x2, LONG y2,
	COLORREF clrBase, bool bVertical)
{
	if(!CGradientUtil::EnsureInitialized()) return false;

	const COLORREF clrTop = BCMenu::LightenColor(clrBase, 0.75);
	const COLORREF clrBottom = BCMenu::DarkenColor(clrBase, 0.05);

	TRIVERTEX v[2];
	ZeroMemory(v, sizeof(TRIVERTEX) * 2);

	v[0].x = x1;
	v[0].y = y1;
	v[0].Red = (GetRValue(clrTop) << 8);
	v[0].Green = (GetGValue(clrTop) << 8);
	v[0].Blue = (GetBValue(clrTop) << 8);

	v[1].x = x2;
	v[1].y = y2;
	v[1].Red = (GetRValue(clrBottom) << 8);
	v[1].Green = (GetGValue(clrBottom) << 8);
	v[1].Blue = (GetBValue(clrBottom) << 8);

	GRADIENT_RECT r;
	ZeroMemory(&r, sizeof(GRADIENT_RECT));
	r.UpperLeft = 0;
	r.LowerRight = 1;

	if(m_lpGradientFill(hdc, v, 2, &r, 1, bVertical ? GRADIENT_FILL_RECT_V :
		GRADIENT_FILL_RECT_H) == FALSE) { ASSERT(FALSE); return false; }

	return true;
}
