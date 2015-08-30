/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2013 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "RemoteControl.h"
#include "../../KeePassLibCpp/Util/MemUtil.h"
#include "../../KeePassLibCpp/Util/StrUtil.h"
#include "../../KeePassLibCpp/Util/TranslateEx.h"
#include "../Resource.h"
#include "../PermissionDlg.h"
#include "../CheckOptionsDlg.h"

static CRemoteControl *g_pRCInstance = NULL;

CRemoteControl::CRemoteControl()
{
	m_bEnableRC = FALSE;
	m_bAlwaysAllowFullAccess = FALSE;
	m_pDefaultMgr = NULL;
	m_hWndMain = NULL;

	m_bRequiresGUIUpdate = FALSE;

	m_hMailslot = NULL;
}

CRemoteControl::~CRemoteControl()
{
	m_bEnableRC = FALSE;
	m_pDefaultMgr = NULL;
	m_hWndMain = NULL;

	m_vClients.clear();
}

void CRemoteControl::InitStatic(CPwManager *pDefaultMgr, HWND hWndMain)
{
	ASSERT(g_pRCInstance == NULL);
	g_pRCInstance = this;

	m_pDefaultMgr = pDefaultMgr;
	m_hWndMain = hWndMain;

	RC_STRING strMailslotName = KEEPASS_MAILSLOT;
	m_hMailslot = CreateMailslot(strMailslotName.c_str(), 0, 0, NULL);
	if(m_hMailslot == INVALID_HANDLE_VALUE) m_hMailslot = NULL;
}

void CRemoteControl::FreeStatic()
{
	ASSERT(g_pRCInstance != NULL);
	g_pRCInstance = NULL;

	if(m_hMailslot != NULL)
	{
		CloseHandle(m_hMailslot);
		m_hMailslot = NULL;
	}

	m_hWndMain = NULL;
}

void CRemoteControl::SetAlwaysAllowFullAccess(BOOL bAllow)
{
	m_bAlwaysAllowFullAccess = bAllow;
}

BOOL CRemoteControl::GetAlwaysAllowFullAccess() const
{
	return m_bAlwaysAllowFullAccess;
}

void CRemoteControl::EnableRemoteControl(BOOL bEnable)
{
	m_bEnableRC = bEnable;
}

BOOL CRemoteControl::IsEnabled() const
{
	return m_bEnableRC;
}

BOOL CRemoteControl::RequiresGUIUpdate()
{
	BOOL b = m_bRequiresGUIUpdate;
	m_bRequiresGUIUpdate = FALSE;
	return b;
}

DWORD CRemoteControl::GetClientPermission(const RC_STRING& strUser, RC_STRING& strRetrievedName) const
{
	for(unsigned int i = 0; i < m_vClients.size(); ++i)
		if(m_vClients[i].strID == strUser)
		{
			strRetrievedName = m_vClients[i].strName;
			return m_vClients[i].dwPermission;
		}

	return RC_PERMISSION_UNKNOWN_CLIENT;
}

void CRemoteControl::ProcessAllRequests()
{
	if((m_bEnableRC == FALSE) || (m_hMailslot == NULL)) return;

	DWORD dwNextMsgSize = 0;
	while(true)
	{
		GetMailslotInfo(m_hMailslot, NULL, &dwNextMsgSize, NULL, NULL);
		if(dwNextMsgSize == MAILSLOT_NO_MESSAGE) break;

		std::vector<BYTE> vData(dwNextMsgSize + 3);
		DWORD dwRead = dwNextMsgSize;
		BOOL bResult = ReadFile(m_hMailslot, &vData[0], dwNextMsgSize, &dwRead, NULL);

		if(bResult == FALSE) { ASSERT(FALSE); }
		if(dwRead != dwNextMsgSize) { ASSERT(FALSE); }

		vData[dwNextMsgSize] = 0;
		vData[dwNextMsgSize + 1] = 0;
		vData[dwNextMsgSize + 2] = 0;
		ProcessRequest((LPCTSTR)&vData[0]);
	}
}

BOOL CRemoteControl::ProcessRequest(LPCTSTR lpIncomingRequest)
{
	if(m_bEnableRC == FALSE) return FALSE;
	if(lpIncomingRequest == NULL) return FALSE;

	const RC_STRING strRequest = lpIncomingRequest;
	if(strRequest.size() == 0) return TRUE;

	RC_QUERY rcQuery;
	if(RCUnpackQuery(rcQuery, strRequest) == FALSE)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	RC_STRING strRetrievedName, strReturn;

	DWORD dwPermission = GetClientPermission(rcQuery.strClientID, strRetrievedName);
	if(dwPermission == RC_PERMISSION_UNKNOWN_CLIENT)
	{
		if(rcQuery.strCommand != RCCMD_CONNECT) return FALSE;
		if(m_vClients.size() >= RC_MAX_CLIENTS) return FALSE;

		CPermissionDlg dlg;
		dlg.m_strApp = rcQuery.strParamW.c_str();

		if(m_bAlwaysAllowFullAccess == FALSE)
		{
			dlg.DoModal();
			dwPermission = static_cast<DWORD>(dlg.m_nPermission);

			if((dwPermission == RC_PERMISSION_FULLACCESS) &&
				(dlg.m_bAlwaysAllowFullAccess != FALSE))
			{
				m_bAlwaysAllowFullAccess = TRUE;
			}
		}
		else dwPermission = RC_PERMISSION_FULLACCESS;

		RC_CLIENT rcClient;
		rcClient.strID = rcQuery.strClientID;
		rcClient.strName = rcQuery.strParamW;
		rcClient.dwPermission = dwPermission;
		m_vClients.push_back(rcClient);

		if(dwPermission == RC_PERMISSION_DENYACCESS)
			strReturn = _T("Denied");
		else if(dwPermission == RC_PERMISSION_READONLYACCESS)
			strReturn = _T("ReadOnly");
		else if(dwPermission == RC_PERMISSION_FULLACCESS)
			strReturn = _T("Full");

		SendMailslotMessage(rcQuery.strClientID, strReturn, 0);
		return TRUE;
	}

	if(rcQuery.strCommand == RCCMD_GETPERMLEVEL)
	{
		if(dwPermission == RC_PERMISSION_DENYACCESS)
			strReturn = _T("Denied");
		else if(dwPermission == RC_PERMISSION_READONLYACCESS)
			strReturn = _T("ReadOnly");
		else if(dwPermission == RC_PERMISSION_FULLACCESS)
			strReturn = _T("Full");

		SendMailslotMessage(rcQuery.strClientID, strReturn, 0);
		return TRUE;
	}

	if(dwPermission == RC_PERMISSION_DENYACCESS) return FALSE;
	if(dwPermission != RC_PERMISSION_UNKNOWN_CLIENT)
	{
		if(rcQuery.strCommand == RCCMD_ENTRY_QUERY_FIELD)
		{
			BYTE vUUID[16];
			_StringToUuid(rcQuery.strParamW.c_str(), vUUID);
			PW_ENTRY *p = m_pDefaultMgr->GetEntryByUuid(vUUID);
			
			if(p != NULL)
			{
				CString strIndex = rcQuery.strParamL.c_str();
				strIndex = strIndex.MakeLower();
				strIndex += _T(": ");

				strReturn = (LPCTSTR)ExtractParameterFromString(p->pszAdditional, (LPCTSTR)strIndex, 0);
			}
			else { ASSERT(FALSE); }
		}
		else if(rcQuery.strCommand == RCCMD_ENTRY_FIND_BYURL)
		{
			const PW_ENTRY *pFound = NULL;

			for(DWORD i = 0; i < m_pDefaultMgr->GetNumberOfEntries(); ++i)
			{
				const PW_ENTRY *pEntry = m_pDefaultMgr->GetEntry(i);
				ASSERT(pEntry != NULL); if(pEntry == NULL) continue;

				const PW_GROUP *pgContainer = m_pDefaultMgr->GetGroupById(pEntry->uGroupId);
				if(pgContainer == NULL) { ASSERT(FALSE); continue; }

				if((_tcscmp(pgContainer->pszGroupName, PWS_BACKUPGROUP) == 0) ||
					(_tcscmp(pgContainer->pszGroupName, PWS_BACKUPGROUP_SRC) == 0))
				{
					continue; // Ignore backup entries
				}

				if(_tcscmp(rcQuery.strParamW.c_str(), pEntry->pszURL) == 0)
				{
					pFound = pEntry;
					break;
				}
			}

			if(pFound != NULL)
			{
				CString strTemp;
				_UuidToString(pFound->uuid, &strTemp);

				strReturn = (LPCTSTR)strTemp;
			}
		}

		if(dwPermission == RC_PERMISSION_FULLACCESS)
		{
			if(rcQuery.strCommand == RCCMD_ENTRY_ADD_BYURL)
				this->AddEntry(strRetrievedName, rcQuery);
		}
	}

	SendMailslotMessage(rcQuery.strClientID, strReturn, 0);
	return TRUE;
}

void CRemoteControl::SplitFields(const RC_STRING& strAsm, std::vector<RC_STRING>& vIDs, std::vector<RC_STRING>& vValues)
{
	unsigned int uIDLen = 0, uValueLen = 0;
	std::basic_stringstream<TCHAR> s(strAsm);
	TCHAR tchSeparator;

	s >> std::noskipws;

	while(true)
	{
		RC_STRING rcID, rcValue;

		s >> uIDLen;
		s >> tchSeparator;

		if(uIDLen == 0) break;

		if(ReadFixedString(rcID, s, uIDLen) == FALSE) break;

		s >> uValueLen;
		s >> tchSeparator;

		if(uValueLen > 0)
			if(ReadFixedString(rcValue, s, uValueLen) == FALSE) break;

		vIDs.push_back(rcID);
		vValues.push_back(rcValue);
	}
}

void CRemoteControl::AddEntry(const RC_STRING& strRetrievedName, const RC_QUERY& rcQuery)
{
	DWORD dwGroupID = m_pDefaultMgr->GetGroupId(strRetrievedName.c_str());
	if((dwGroupID == 0) || (dwGroupID == DWORD_MAX))
	{
		PW_GROUP pg;
		ZeroMemory(&pg, sizeof(PW_GROUP));
		m_pDefaultMgr->GetNeverExpireTime(&pg.tExpire);

		pg.pszGroupName = (TCHAR *)strRetrievedName.c_str();
		m_pDefaultMgr->AddGroup(&pg);

		dwGroupID = m_pDefaultMgr->GetGroupIdByIndex(m_pDefaultMgr->GetNumberOfGroups() - 1);
	}

	const DWORD dwInvalid1 = m_pDefaultMgr->GetGroupId(PWS_BACKUPGROUP);
	const DWORD dwInvalid2 = m_pDefaultMgr->GetGroupId(PWS_BACKUPGROUP_SRC);

	for(DWORD dwEntryIndex = 0; dwEntryIndex < m_pDefaultMgr->GetNumberOfEntries(); ++dwEntryIndex)
	{
		PW_ENTRY *ppe = m_pDefaultMgr->GetEntry(dwEntryIndex);

		if((ppe->uGroupId == dwInvalid1) || (ppe->uGroupId == dwInvalid2))
			continue;

		if((ppe->uGroupId == dwGroupID) && (_tcscmp(ppe->pszURL, rcQuery.strParamW.c_str()) == 0))
		{
			SetForegroundWindow(m_hWndMain);

			CString strMsg = TRL("An entry already exists for the current URL.");
			strMsg += _T("\r\n\r\n");
			strMsg += rcQuery.strParamW.c_str();
			strMsg += _T("\r\n\r\n");
			strMsg += TRL("Do you want to overwrite it?");
			if(MessageBox(m_hWndMain, strMsg, TRL("Overwrite?"), MB_ICONQUESTION | MB_YESNO) == IDNO)
				return;
			else m_pDefaultMgr->DeleteEntry(dwEntryIndex);
		}
	}

	std::vector<RC_STRING> vIDs, vValues;
	SplitFields(rcQuery.strParamL, vIDs, vValues);

	if(vIDs.size() > 0)
	{
		std::vector<BOOL> vRemFlags(vIDs.size(), TRUE);
		std::vector<CHKOPT_PARAM> params;
		CHKOPT_PARAM chkOpt;

		ZeroMemory(&chkOpt, sizeof(CHKOPT_PARAM));

		chkOpt.lpString = TRL("Fields to remember");
		chkOpt.lpSubString = NULL;
		chkOpt.nIcon = 8;
		chkOpt.pbValue = NULL;
		params.push_back(chkOpt);

		for(unsigned int iParam = 0; iParam < vIDs.size(); ++iParam)
		{
			chkOpt.lpString = vIDs[iParam].c_str();
			chkOpt.lpSubString = vValues[iParam].c_str();
			chkOpt.pbValue = &vRemFlags[iParam];
			
			params.push_back(chkOpt);
		}

		CCheckOptionsDlg dlg;
		dlg.m_bTwoColumns = TRUE;
		dlg.m_bForceForeground = TRUE;
		dlg.m_strTitle = TRL("Add Entry");
		dlg.m_strDescription = TRL("Select the fields KeePass should remember");
		dlg.m_dwNumParams = (DWORD)params.size();
		dlg.m_pParams = &params[0];

		if(dlg.DoModal() == IDOK)
		{
			RC_STRING strNotes;

			for(unsigned int iSelParam = 0; iSelParam < vRemFlags.size(); ++iSelParam)
			{
				if(vRemFlags[iSelParam] == TRUE)
				{
					strNotes += params[iSelParam + 1].lpString;
					strNotes += _T(": ");
					strNotes += params[iSelParam + 1].lpSubString;
					strNotes += _T("\r\n");
				}
			}

			PW_ENTRY pe;
			ZeroMemory(&pe, sizeof(PW_ENTRY));

			pe.pszTitle = const_cast<TCHAR *>(rcQuery.strParamW.c_str());
			pe.pszUserName = const_cast<TCHAR *>(strRetrievedName.c_str());
			pe.uGroupId = dwGroupID;
			pe.pszURL = pe.pszTitle;
			pe.pszAdditional = const_cast<TCHAR *>(strNotes.c_str());
			m_pDefaultMgr->GetNeverExpireTime(&pe.tExpire);

			PW_TIME tNow;
			_GetCurrentPwTime(&tNow);
			memcpy(&pe.tCreation, &tNow, sizeof(PW_TIME));
			memcpy(&pe.tLastMod, &tNow, sizeof(PW_TIME));
			memcpy(&pe.tLastAccess, &tNow, sizeof(PW_TIME));

			m_pDefaultMgr->AddEntry(&pe);
			m_bRequiresGUIUpdate = TRUE;
		}
	}
}
