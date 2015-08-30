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
#include "DwmUtil.h"
#include "NewGUICommon.h"
#include "ImageUtilEx.h"
#include "BCMenu.h"
#include "../../KeePassLibCpp/Util/AppUtil.h"
#include "../Resource.h"

#include <boost/static_assert.hpp>

using namespace Gdiplus;

HMODULE CDwmUtil::m_hLib = NULL;

LPDWMSETWINDOWATTRIBUTE CDwmUtil::m_lpSetWindowAttribute = NULL;
LPDWMSETICONICTHUMBNAIL CDwmUtil::m_lpSetIconicThumbnail = NULL;
LPDWMSETICONICLIVEPREVIEWBITMAP CDwmUtil::m_lpSetIconicLivePreviewBitmap = NULL;

CDwmUtil::CDwmUtil()
{
}

void CDwmUtil::Initialize()
{
	// Static iconic bitmaps only supported by Windows >= 7
	if(AU_IsAtLeastWin7System() == FALSE) return;

	ASSERT(m_hLib == NULL);
	Release();

	m_hLib = LoadLibrary(DWMAPI_LIB_NAME);
	if(m_hLib == NULL) { ASSERT(FALSE); return; }

	m_lpSetWindowAttribute = (LPDWMSETWINDOWATTRIBUTE)GetProcAddress(
		m_hLib, DWMAPI_SETWINDOWATTRIBUTE);
	m_lpSetIconicThumbnail = (LPDWMSETICONICTHUMBNAIL)GetProcAddress(
		m_hLib, DWMAPI_SETICONICTHUMBNAIL);
	m_lpSetIconicLivePreviewBitmap = (LPDWMSETICONICLIVEPREVIEWBITMAP)GetProcAddress(
		m_hLib, DWMAPI_SETICONICLIVEPREVIEWBITMAP);

	if((m_lpSetWindowAttribute == NULL) || (m_lpSetIconicThumbnail == NULL) ||
		(m_lpSetIconicLivePreviewBitmap == NULL))
	{
		ASSERT(FALSE);
		Release();
	}
}

void CDwmUtil::Release()
{
	if(m_hLib == NULL) return;

	m_lpSetWindowAttribute = NULL;
	m_lpSetIconicThumbnail = NULL;
	m_lpSetIconicLivePreviewBitmap = NULL;

	VERIFY(FreeLibrary(m_hLib));
	m_hLib = NULL;
}

void CDwmUtil::EnableWindowPeekPreview(HWND hWnd, bool bEnable)
{
	if(hWnd == NULL) { ASSERT(FALSE); return; }

	if(m_hLib == NULL) return;

	BOOL s = (bEnable ? 0 : 1);

	BOOST_STATIC_ASSERT(sizeof(BOOL) == 4);
	m_lpSetWindowAttribute(hWnd, DWMWA_HAS_ICONIC_BITMAP, &s, sizeof(BOOL));
	m_lpSetWindowAttribute(hWnd, DWMWA_FORCE_ICONIC_REPRESENTATION, &s, sizeof(BOOL));
}

void CDwmUtil::SetIconicThumbnail(HWND hWnd, LPARAM lParam)
{
	const int w = static_cast<int>(lParam >> 16) & 0xFFFF;
	const int h = static_cast<int>(lParam) & 0xFFFF;

	SetIconicBitmap(hWnd, w, h, true);
}

void CDwmUtil::SetIconicPreview(HWND hWnd, const SIZE& szLastContent)
{
	if(hWnd == NULL) { ASSERT(FALSE); return; }

	int w = 0, h = 0;
	BOOST_STATIC_ASSERT(sizeof(int) == sizeof(LONG));

	if((IsIconic(hWnd) == FALSE) && (IsWindowVisible(hWnd) != FALSE))
	{
		const SIZE sz = NewGUI_GetWindowContentSize(hWnd);
		if((sz.cx > 0) && (sz.cy > 0)) { w = sz.cx; h = sz.cy; }
		else { ASSERT(FALSE); }
	}

	if(w <= 0) w = szLastContent.cx;
	else { ASSERT(w == szLastContent.cx); } // szLastContent should be up-to-date
	if(h <= 0) h = szLastContent.cy;
	else { ASSERT(h == szLastContent.cy); } // szLastContent should be up-to-date

	SetIconicBitmap(hWnd, w, h, false);
}

void CDwmUtil::SetIconicBitmap(HWND hWnd, int sw, int sh, bool bThumbnail)
{
	if(hWnd == NULL) { ASSERT(FALSE); return; }

	if(m_hLib == NULL) return;

	if(sw <= 0) { ASSERT(FALSE); sw = 200; } // Default Windows 7
	if(sh <= 0) { ASSERT(FALSE); sh = 109; } // Default Windows 7

	Bitmap* pImg = NULL;
	LPCTSTR lpIcon = MAKEINTRESOURCE(IDB_MAIN);
	HMODULE hInst = AfxGetInstanceHandle();
	HRSRC hrImg = FindResource(hInst, lpIcon, _T("PNG"));
	if(hrImg == NULL) hrImg = FindResource(hInst, lpIcon, RT_BITMAP);
	if(hrImg != NULL)
	{
		HGLOBAL hgImg = LoadResource(hInst, hrImg); // Not a real HGLOBAL
		DWORD dwImgSize = SizeofResource(hInst, hrImg);
		if((hgImg != NULL) && (dwImgSize > 0))
		{
			const LPVOID lpImg = LockResource(hgImg);

			HGLOBAL hgStream = GlobalAlloc((GMEM_MOVEABLE | GMEM_ZEROINIT), dwImgSize);
			if(hgStream != NULL)
			{
				LPVOID lpStream = GlobalLock(hgStream);
				if(lpStream != NULL)
				{
					memcpy(lpStream, lpImg, dwImgSize);
					GlobalUnlock(hgStream);

					IStream* pStream = NULL;
					if(SUCCEEDED(CreateStreamOnHGlobal(hgStream, FALSE, &pStream)) &&
						(pStream != NULL))
					{
						pImg = Bitmap::FromStream(pStream);
						pStream->Release();
					}
					else { ASSERT(FALSE); }
				}
				else { ASSERT(FALSE); }

				VERIFY(GlobalFree(hgStream) == NULL);
			}
			else { ASSERT(FALSE); }
		}
		else { ASSERT(FALSE); }
	}
	else { ASSERT(FALSE); }
	if(pImg == NULL)
	{
		HICON hImg = LoadIcon(hInst, lpIcon);
		if(hImg != NULL) pImg = Bitmap::FromHICON(hImg);
		else { ASSERT(FALSE); }
	}
	if(pImg == NULL) { ASSERT(FALSE); return; }

	int iImgW = min(static_cast<int>(pImg->GetWidth()), 128);
	int iImgH = min(static_cast<int>(pImg->GetHeight()), 128);
	int iImgWMax = (sw * 4) / 6;
	int iImgHMax = (sh * 4) / 6;
	if(iImgW > iImgWMax)
	{
		float fRatio = static_cast<float>(iImgWMax) / static_cast<float>(iImgW);
		iImgW = iImgWMax;
		iImgH = static_cast<int>(static_cast<float>(iImgH) * fRatio);
	}
	if(iImgH > iImgHMax)
	{
		float fRatio = static_cast<float>(iImgHMax) / static_cast<float>(iImgH);
		iImgW = static_cast<int>(static_cast<float>(iImgW) * fRatio);
		iImgH = iImgHMax;
	}
	if((iImgW <= 0) || (iImgH <= 0)) { ASSERT(FALSE); SAFE_DELETE(pImg); return; }
	if(iImgW > sw) { ASSERT(FALSE); iImgW = sw; }
	if(iImgH > sh) { ASSERT(FALSE); iImgH = sh; }

	int iImgX = (sw - iImgW) / 2;
	int iImgY = (sh - iImgH) / 2;

	// 32-bit color depth required by API
	Bitmap* pBmp = new Bitmap(sw, sh, PixelFormat32bppARGB);
	Graphics* pg = Graphics::FromImage(pBmp);

	COLORREF dwClrRef = GetSysColor(COLOR_BTNFACE);
	Color clr(GetRValue(dwClrRef), GetGValue(dwClrRef), GetBValue(dwClrRef));
	pg->Clear(clr);

	Point ptZero(0, 0);
	Point ptMax(sw, sh);
	COLORREF clrRefLight = BCMenu::LightenColor(dwClrRef, 0.25);
	Color clrLight(GetRValue(clrRefLight), GetGValue(clrRefLight), GetBValue(clrRefLight));
	COLORREF clrRefDark = BCMenu::DarkenColor(dwClrRef, 0.25);
	Color clrDark(GetRValue(clrRefDark), GetGValue(clrRefDark), GetBValue(clrRefDark));

	LinearGradientBrush* pbr = new LinearGradientBrush(ptZero, ptMax,
		clrLight, clrDark);
	pg->FillRectangle(pbr, 0, 0, sw, sh);
	SAFE_DELETE(pbr);

	// *After* drawing the gradient (otherwise border bug)
	pg->SetInterpolationMode(InterpolationModeHighQualityBicubic);
	pg->SetSmoothingMode(SmoothingModeHighQuality);

	pg->DrawImage(pImg, iImgX, iImgY, iImgW, iImgH);

	SAFE_DELETE(pg);

	HBITMAP hBmp = NULL;
	pBmp->GetHBITMAP(clr, &hBmp);

	if(hBmp == NULL) { ASSERT(FALSE); }
	else if(bThumbnail)
		m_lpSetIconicThumbnail(hWnd, hBmp, DWM_SIT_DISPLAYFRAME);
	else
		m_lpSetIconicLivePreviewBitmap(hWnd, hBmp, NULL, DWM_SIT_DISPLAYFRAME);

	if(hBmp != NULL) DeleteObject(hBmp);
	SAFE_DELETE(pBmp);
	SAFE_DELETE(pImg);
}
