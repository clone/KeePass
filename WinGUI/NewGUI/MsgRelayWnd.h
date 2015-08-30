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

#ifndef ___CMSGRELAYWND_H___
#define ___CMSGRELAYWND_H___

#pragma once

#include <vector>

class CMsgRelayWnd : public CWnd
{
public:
	static void EnableRelaying(BOOL bEnable);
	static void SetRelayTarget(HWND hWndTarget);
	static void AddRelayedMessage(UINT uMessage);

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	static BOOL m_bRelayEnabled;
	static HWND m_hRelayTarget;
	static std::vector<UINT> m_vRelayedMessages;
};

#endif // ___CMSGRELAYWND_H___
