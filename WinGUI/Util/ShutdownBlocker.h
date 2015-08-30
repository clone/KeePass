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

#ifndef ___SHUTDOWN_BLOCKER_H___
#define ___SHUTDOWN_BLOCKER_H___

#pragma once

#include "../../KeePassLibCpp/SysDefEx.h"
#include <string>
#include <boost/utility.hpp>

typedef BOOL(WINAPI* LPSHUTDOWNBLOCKREASONCREATE)(HWND hWnd, LPCWSTR pwszReason);
typedef BOOL(WINAPI* LPSHUTDOWNBLOCKREASONDESTROY)(HWND hWnd);

class CShutdownBlocker : boost::noncopyable
{
public:
	CShutdownBlocker(HWND hWnd, LPCTSTR lpReason);
	virtual ~CShutdownBlocker();

	void Release();

#ifdef _DEBUG
	static CShutdownBlocker* GetInstance();
#endif

private:
	HWND m_hWnd;
	HMODULE m_hLib;

	LPSHUTDOWNBLOCKREASONCREATE m_lpSdrCreate;
	LPSHUTDOWNBLOCKREASONDESTROY m_lpSdrDestroy;

	static CShutdownBlocker* g_psdbPrimary;
};

#endif // ___SHUTDOWN_BLOCKER_H___
