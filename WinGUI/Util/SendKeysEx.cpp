/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2011 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "SendKeysEx.h"

CSendKeysEx::CSendKeysEx()
{
	m_p = NULL;
	m_bReleasedOnce = false;

	ZeroMemory(&m_s, sizeof(SKSTATEEX));
}

CSendKeysEx::~CSendKeysEx()
{
	m_bReleasedOnce = false;
	Release();
}

void CSendKeysEx::Release()
{
	ASSERT(!m_bReleasedOnce);
	if(m_p == NULL) return;

	_RestoreKeyboardLayout();

	ZeroMemory(&m_s, sizeof(SKSTATEEX));

	delete m_p; m_p = NULL;
	m_bReleasedOnce = true;
}

void CSendKeysEx::_EnsureInitialized()
{
	if(m_p != NULL) return;

	m_p = new CSendKeys();

	m_s.dwThisThreadID = GetCurrentThreadId();

	m_s.hWndTarget = GetForegroundWindow();
	m_s.dwTargetThreadID = GetWindowThreadProcessId(m_s.hWndTarget,
		&m_s.dwTargetProcessID);

	_EnsureSameKeyboardLayout();
}

void CSendKeysEx::SendKeyUp(BYTE vKey)
{
	_EnsureInitialized();
	m_p->SendKeyUp(vKey);
}

bool CSendKeysEx::SendKeys(LPCTSTR lpKeysString, bool bWait)
{
	if(lpKeysString == NULL) { ASSERT(FALSE); return false; }

	_EnsureInitialized();
	return m_p->SendKeys(lpKeysString, bWait);
}

void CSendKeysEx::SetDelay(DWORD dwDelay)
{
	_EnsureInitialized();
	m_p->SetDelay(dwDelay);
}

void CSendKeysEx::_EnsureSameKeyboardLayout()
{
	HKL hklSelf = GetKeyboardLayout(0);
	HKL hklTarget = GetKeyboardLayout(m_s.dwTargetThreadID);

	m_s.hklCurrent = hklSelf;

	if(hklSelf != hklTarget)
	{
		m_s.hklOriginal = ActivateKeyboardLayout(hklTarget, 0);
		m_s.hklCurrent = hklTarget;

		ASSERT(m_s.hklOriginal == hklSelf);
	}
}

void CSendKeysEx::_RestoreKeyboardLayout()
{
	if(m_s.hklOriginal != NULL)
		ActivateKeyboardLayout(m_s.hklOriginal, 0);
}
