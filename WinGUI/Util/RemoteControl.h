/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2008 Dominik Reichl <dominik.reichl@t-online.de>

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

#include <vector>
#include <boost/utility.hpp>
#include "../../KeePassLibCpp/PwManager.h"
#include "../../KeePassLibCpp/Util/ComUtil.h"

#define RC_PERMISSION_DENYACCESS 0
#define RC_PERMISSION_UNKNOWN_CLIENT 1
#define RC_PERMISSION_READONLYACCESS 2
#define RC_PERMISSION_FULLACCESS 3

#define RC_MAX_CLIENTS 1024

typedef struct _RC_CLIENT
{
	RC_STRING strID;
	RC_STRING strName;
	DWORD dwPermission;
} RC_CLIENT;

class CRemoteControl : boost::noncopyable
{
public:
	CRemoteControl();
	~CRemoteControl();

	void InitStatic(CPwManager *pDefaultMgr, HWND hWndMain);
	void FreeStatic();

	void EnableRemoteControl(BOOL bEnable);
	BOOL IsEnabled() const;

	void ProcessAllRequests();

	BOOL RequiresGUIUpdate();

private:
	BOOL ProcessRequest(LPCTSTR lpIncomingRequest);
	DWORD GetClientPermission(const RC_STRING& strUser, RC_STRING& strRetrievedName) const;

	void AddEntry(const RC_STRING& strRetrievedName, const RC_QUERY& rcQuery);

	void SplitFields(const RC_STRING& strAsm, std::vector<RC_STRING>& vIDs, std::vector<RC_STRING>& vValues);

	BOOL m_bEnableRC;
	CPwManager *m_pDefaultMgr;
	HWND m_hWndMain;

	BOOL m_bRequiresGUIUpdate;

	HANDLE m_hMailslot;

	std::vector<RC_CLIENT> m_vClients;
};

#endif // ___IPC_SERVER_H___
