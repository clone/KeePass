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

#ifndef ___DWM_UTIL_H___
#define ___DWM_UTIL_H___

#pragma once

#include "../../KeePassLibCpp/SysDefEx.h"
#include <boost/utility.hpp>
#include <dwmapi.h>

#define DWMAPI_LIB_NAME _T("DwmApi.dll")
#define DWMAPI_SETWINDOWATTRIBUTE "DwmSetWindowAttribute"
#define DWMAPI_SETICONICTHUMBNAIL "DwmSetIconicThumbnail"
#define DWMAPI_SETICONICLIVEPREVIEWBITMAP "DwmSetIconicLivePreviewBitmap"

#ifndef DWMWA_HAS_ICONIC_BITMAP
#define DWMWA_HAS_ICONIC_BITMAP 10
#endif
// #ifndef DWMWA_DISALLOW_PEEK
// #define DWMWA_DISALLOW_PEEK 11
// #endif

#ifndef DWM_SIT_DISPLAYFRAME
#define DWM_SIT_DISPLAYFRAME 1
#endif

#ifndef WM_DWMSENDICONICTHUMBNAIL
#define WM_DWMSENDICONICTHUMBNAIL (0x0323)
#define WM_DWMSENDICONICLIVEPREVIEWBITMAP (0x0326)
#endif

typedef HRESULT(WINAPI* LPDWMSETWINDOWATTRIBUTE)(HWND hwnd, DWORD dwAttribute,
	LPCVOID pvAttribute, DWORD cbAttribute);
typedef HRESULT(WINAPI* LPDWMSETICONICTHUMBNAIL)(HWND hwnd, HBITMAP hbmp,
	DWORD dwSITFlags);
typedef HRESULT(WINAPI* LPDWMSETICONICLIVEPREVIEWBITMAP)(HWND hwnd, HBITMAP hbmp,
	POINT *pptClient, DWORD dwSITFlags);

class CDwmUtil : boost::noncopyable
{
public:
	static void Initialize();
	static void Release();

	static void EnableWindowPeekPreview(HWND hWnd, bool bEnable);

	static void SetIconicThumbnail(HWND hWnd, LPARAM lParam);
	static void SetIconicPreview(HWND hWnd, const SIZE& szLastContent);

private:
	CDwmUtil();

	static void SetIconicBitmap(HWND hWnd, int sw, int sh, bool bThumbnail);

	static HMODULE m_hLib;

	static LPDWMSETWINDOWATTRIBUTE m_lpSetWindowAttribute;
	static LPDWMSETICONICTHUMBNAIL m_lpSetIconicThumbnail;
	static LPDWMSETICONICLIVEPREVIEWBITMAP m_lpSetIconicLivePreviewBitmap;
};

#endif // ___DWM_UTIL_H___
