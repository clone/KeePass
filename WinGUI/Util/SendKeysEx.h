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

#ifndef ___SEND_KEYS_EX_H___
#define ___SEND_KEYS_EX_H___

#pragma once

#include "../../KeePassLibCpp/SysDefEx.h"
#include "SendKeys.h"

typedef struct _SKSTATEEX
{
	HWND hWndTarget;
	DWORD dwTargetProcessID;
	DWORD dwTargetThreadID;

	DWORD dwThisThreadID;

	HKL hklOriginal;
	HKL hklCurrent;
} SKSTATEEX;

class CSendKeysEx
{
public:
	CSendKeysEx();
	~CSendKeysEx();

	void Release();

	void SendKeyUp(BYTE vKey);
	bool SendKeys(LPCTSTR lpKeysString, bool bWait);

	void SetDelay(DWORD dwDelay);

	void SetEnsureSameKeyboardLayout(bool bEnable);

private:
	void _EnsureInitialized();

	void _EnsureSameKeyboardLayout();
	void _RestoreKeyboardLayout();

	CSendKeys* m_p;
	bool m_bReleasedOnce;

	bool m_bSameKL;

	SKSTATEEX m_s;
};

#endif // ___SEND_KEYS_EX_H___
