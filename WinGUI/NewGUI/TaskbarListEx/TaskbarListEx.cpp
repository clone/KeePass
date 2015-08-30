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
#include "TaskbarListEx.h"

static ITaskbarList3* g_pTbl = NULL;

static const GUID TBL_IID_ITaskbarList3 = { 0xEA1AFB91, 0x9E28, 0x4B86,
	{ 0x90, 0xE9, 0x9E, 0x9F, 0x8A, 0x5E, 0xEF, 0xAF } };
static const GUID TBL_CLSID_TaskbarList = { 0x56FDF344, 0xFD6D, 0x11D0,
	{ 0x95, 0x8A, 0x00, 0x60, 0x97, 0xC9, 0xA0, 0x90 } };

CTaskbarListEx::CTaskbarListEx()
{
}

void CTaskbarListEx::Initialize()
{
	CTaskbarListEx::Release(false);

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	if(CoCreateInstance(TBL_CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER,
		TBL_IID_ITaskbarList3, (LPVOID*)&g_pTbl) != S_OK)
	{
		CTaskbarListEx::Release(true);
	}

	if(g_pTbl != NULL) { VERIFY(g_pTbl->HrInit() == S_OK); }
}

void CTaskbarListEx::Release(bool bForceComUninitialize)
{
	if(g_pTbl != NULL)
	{
		g_pTbl->Release();
		g_pTbl = NULL;

		CoUninitialize();
	}
	else if(bForceComUninitialize) CoUninitialize();
}

void CTaskbarListEx::SetOverlayIcon(HWND hWnd, HICON hIcon, LPCWSTR pszDescription)
{
	if(g_pTbl != NULL) g_pTbl->SetOverlayIcon(hWnd, hIcon, pszDescription);
}

void CTaskbarListEx::SetProgressState(HWND hWnd, TBPFLAG tbpFlags)
{
	if(g_pTbl != NULL) g_pTbl->SetProgressState(hWnd, tbpFlags);
}
