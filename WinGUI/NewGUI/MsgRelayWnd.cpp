/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2007 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "MsgRelayWnd.h"

#include <algorithm>

BOOL CMsgRelayWnd::m_bRelayEnabled = FALSE;
HWND CMsgRelayWnd::m_hRelayTarget = NULL;
std::vector<UINT> CMsgRelayWnd::m_vRelayedMessages;

void CMsgRelayWnd::EnableRelaying(BOOL bEnable)
{
	m_bRelayEnabled = bEnable;
}

void CMsgRelayWnd::SetRelayTarget(HWND hWndTarget)
{
	m_hRelayTarget = hWndTarget;
}

void CMsgRelayWnd::AddRelayedMessage(UINT uMessage)
{
	m_vRelayedMessages.push_back(uMessage);
}

LRESULT CMsgRelayWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_bRelayEnabled != FALSE) // Relaying enabled
	{
		if(std::find(m_vRelayedMessages.begin(), m_vRelayedMessages.end(),
			message) != m_vRelayedMessages.end())
		{
			::PostMessage(m_hRelayTarget, message, wParam, lParam);
		}
	}

	return CWnd::WindowProc(message, wParam, lParam);
}
