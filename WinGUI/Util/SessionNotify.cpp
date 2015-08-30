/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2012 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "SessionNotify.h"
#include "../../KeePassLibCpp/Util/AppUtil.h"

CSessionNotify::CSessionNotify()
{
	m_hTarget = NULL;

	m_hWTSAPI = NULL;
	m_lpWTSRegisterSessionNotification = NULL;
	m_lpWTSUnRegisterSessionNotification = NULL;
}

CSessionNotify::~CSessionNotify()
{
	Unregister();
}

BOOL CSessionNotify::Register(HWND hWnd)
{
	ASSERT(m_hWTSAPI == NULL); Unregister();

	m_hWTSAPI = AU_LoadLibrary(_T("Wtsapi32.dll"));
	if(m_hWTSAPI == NULL) return TRUE; // We're running on Win9x/Win2000?

	m_hTarget = hWnd;

	m_lpWTSRegisterSessionNotification = (LPWTSREGISTERSESSIONNOTIFICATION)GetProcAddress(m_hWTSAPI,
		"WTSRegisterSessionNotification");
	m_lpWTSUnRegisterSessionNotification = (LPWTSUNREGISTERSESSIONNOTIFICATION)GetProcAddress(m_hWTSAPI,
		"WTSUnRegisterSessionNotification");

	if(m_lpWTSRegisterSessionNotification != NULL)
		return m_lpWTSRegisterSessionNotification(hWnd, 0); // 0 = NOTIFY_FOR_THIS_SESSION

	return TRUE;
}

void CSessionNotify::Unregister()
{
	if(m_hWTSAPI != NULL)
	{
		if(m_lpWTSUnRegisterSessionNotification != NULL)
			m_lpWTSUnRegisterSessionNotification(m_hTarget);

		m_lpWTSRegisterSessionNotification = NULL;
		m_lpWTSUnRegisterSessionNotification = NULL;

		FreeLibrary(m_hWTSAPI);
		m_hWTSAPI = NULL;
	}

	m_hTarget = NULL;
}
