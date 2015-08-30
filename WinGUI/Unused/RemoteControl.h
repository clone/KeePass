/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2006 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___REMOTE_CONTROL_H___
#define ___REMOTE_CONTROL_H___

#include "../../KeePassLibCpp/PwManager.h"

#define RC_PERMISSION_DENYACCESS 0
#define RC_PERMISSION_FULLACCESS 1
#define RC_PERMISSION_READONLYACCESS 2
#define RC_PERMISSION_UNKNOWN_CLIENT 3

class CRemoteControl
{
public:
	CRemoteControl();
	~CRemoteControl();

	void InitStatic(CPwManager *pDefaultMgr);
	void FreeStatic();

	void EnableRemoteControl(BOOL bEnable);
	BOOL IsEnabled() const;

	BOOL ProcessConnectionRequest(LPCTSTR lpRequest);
	BOOL ProcessRequest(LPCTSTR lpRequest);

private:
	int GetClientPermission(CString strUser) const;

	BOOL m_bEnableRC;

	CStringArray m_arClients;
	CDWordArray m_arPermissions;

	CPwManager *m_pDefaultMgr;
};

UINT __cdecl RCConnectionThread(LPVOID pParam);

C_FN_SHARE LPTSTR PwEntryToString(const PW_ENTRY *lpEntry);
C_FN_SHARE BOOL StringToPwEntry(PW_ENTRY *pEntry, LPCTSTR lpEntryString);

BOOL ReadPwStringItem(LPCTSTR lpEntryString, DWORD *pdwPos, CString *pStore);

#endif // ___IPC_SERVER_H___
