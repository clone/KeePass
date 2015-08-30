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
#include "ShutdownBlocker.h"
#include "../../KeePassLibCpp/Util/AppUtil.h"
#include "../../KeePassLibCpp/Util/StrUtil.h"

CShutdownBlocker* CShutdownBlocker::g_psdbPrimary = NULL;

CShutdownBlocker::CShutdownBlocker(HWND hWnd, LPCTSTR lpReason)
{
	ASSERT(hWnd != NULL);
	m_hWnd = hWnd;

	if(AU_IsAtLeastWinVistaSystem() == FALSE) { m_hLib = NULL; return; }

	m_hLib = LoadLibrary(_T("User32.dll"));
	if(m_hLib == NULL) { ASSERT(FALSE); return; }

	m_lpSdrCreate = (LPSHUTDOWNBLOCKREASONCREATE)GetProcAddress(m_hLib,
		"ShutdownBlockReasonCreate");
	m_lpSdrDestroy = (LPSHUTDOWNBLOCKREASONDESTROY)GetProcAddress(m_hLib,
		"ShutdownBlockReasonDestroy");
	if((m_lpSdrCreate == NULL) || (m_lpSdrDestroy == NULL))
	{
		ASSERT(FALSE);
		Release();
		return;
	}

	if(g_psdbPrimary != NULL) return;

	std::basic_string<WCHAR> str;
	if((lpReason != NULL) && (lpReason[0] != 0))
		str = _StringToUnicodeStl(lpReason);
	else { ASSERT(FALSE); str = L"..."; }

	if(m_lpSdrCreate(hWnd, str.c_str()) != FALSE)
		g_psdbPrimary = this;
	else { ASSERT(FALSE); }
}

CShutdownBlocker::~CShutdownBlocker()
{
	Release();
}

void CShutdownBlocker::Release()
{
	if(this == g_psdbPrimary)
	{
		if(m_hLib != NULL)
		{
			VERIFY(m_lpSdrDestroy(m_hWnd));
		}
		else { ASSERT(FALSE); }

		g_psdbPrimary = NULL;
	}

	if(m_hLib != NULL)
	{
		VERIFY(FreeLibrary(m_hLib));
		m_hLib = NULL;

		m_lpSdrCreate = NULL;
		m_lpSdrDestroy = NULL;
	}
}

#ifdef _DEBUG
CShutdownBlocker* CShutdownBlocker::GetInstance()
{
	return g_psdbPrimary;
}
#endif
