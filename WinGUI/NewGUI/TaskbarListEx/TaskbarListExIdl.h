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

#ifndef ___TASKBAR_LIST_EX_IDL_H___
#define ___TASKBAR_LIST_EX_IDL_H___

#pragma once

#include <objbase.h>
#include <shobjidl.h>

#ifndef __ITaskbarList3_INTERFACE_DEFINED__
#define __ITaskbarList3_INTERFACE_DEFINED__

typedef enum TBPFLAG
{
	TBPF_NOPROGRESS = 0,
	TBPF_INDETERMINATE = 0x1,
	TBPF_NORMAL = 0x2,
	TBPF_ERROR = 0x4,
	TBPF_PAUSED = 0x8
} TBPFLAG;

MIDL_INTERFACE("EA1AFB91-9E28-4B86-90E9-9E9F8A5EEFAF")
ITaskbarList3 : public ITaskbarList2
{
public:
	virtual HRESULT STDMETHODCALLTYPE SetProgressValue(HWND hwnd, ULONGLONG ullCompleted,
		ULONGLONG ullTotal) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetProgressState(HWND hwnd, TBPFLAG tbpFlags) = 0;

	virtual HRESULT STDMETHODCALLTYPE RegisterTab(HWND hwndTab, HWND hwndMDI) = 0;
	virtual HRESULT STDMETHODCALLTYPE UnregisterTab(HWND hwndTab) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetTabOrder(HWND hwndTab, HWND hwndInsertBefore) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetTabActive(HWND hwndTab, HWND hwndMDI,
		DWORD dwReserved) = 0;

	virtual HRESULT STDMETHODCALLTYPE ThumbBarAddButtons(HWND hwnd, UINT cButtons,
		void* pButton) = 0;
	virtual HRESULT STDMETHODCALLTYPE ThumbBarUpdateButtons(HWND hwnd, UINT cButtons,
		void* pButton) = 0;
	virtual HRESULT STDMETHODCALLTYPE ThumbBarSetImageList(HWND hwnd, HIMAGELIST himl) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetOverlayIcon(HWND hwnd, HICON hIcon,
		LPCWSTR pszDescription) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetThumbnailTooltip(HWND hwnd, LPCWSTR pszTip) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetThumbnailClip(HWND hwnd, RECT* prcClip) = 0;
};

#endif // __ITaskbarList3_INTERFACE_DEFINED__

#endif // ___TASKBAR_LIST_EX_IDL_H___
