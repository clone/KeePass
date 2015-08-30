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

#ifndef ___SESSION_NOTIFY_H___
#define ___SESSION_NOTIFY_H___

#include <boost/utility.hpp>

typedef BOOL(WINAPI *LPWTSREGISTERSESSIONNOTIFICATION)(HWND hWnd, DWORD dwFlags);
typedef BOOL(WINAPI *LPWTSUNREGISTERSESSIONNOTIFICATION)(HWND hWnd);

class CSessionNotify : boost::noncopyable
{
public:
	CSessionNotify();
	~CSessionNotify();

	BOOL Register(HWND hWnd);
	void Unregister();

private:
	HWND m_hTarget;
	HMODULE m_hWTSAPI;

	LPWTSREGISTERSESSIONNOTIFICATION m_lpWTSRegisterSessionNotification;
	LPWTSUNREGISTERSESSIONNOTIFICATION m_lpWTSUnRegisterSessionNotification;
};

#endif
