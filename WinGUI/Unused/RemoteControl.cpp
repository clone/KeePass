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

#include "StdAfx.h"
#include "RemoteControl.h"
#include "../../KeePassLibCpp/Util/MemUtil.h"
#include "../../KeePassLibCpp/Util/StrUtil.h"
#include "../Resource.h"
#include "../PermissionDlg.h"
#include "../Plugins/MsgInterface.h"
#include <stdio.h>
#include <winsock2.h>

static CRemoteControl *g_pRCInstance = NULL;

CRemoteControl::CRemoteControl()
{
	m_bEnableRC = FALSE;
	m_pDefaultMgr = NULL;
}

CRemoteControl::~CRemoteControl()
{
	m_bEnableRC = FALSE;
	m_pDefaultMgr = NULL;

	m_arClients.RemoveAll();
	m_arPermissions.RemoveAll();
}

void CRemoteControl::InitStatic(CPwManager *pDefaultMgr)
{
	WSADATA wsa;

	ASSERT(g_pRCInstance == NULL);
	g_pRCInstance = this;

	m_pDefaultMgr = pDefaultMgr;

	ZeroMemory(&wsa, sizeof(WSADATA));
	WSAStartup(MAKEWORD(1, 1), &wsa);
}

void CRemoteControl::FreeStatic()
{
	WSACleanup();

	ASSERT(g_pRCInstance != NULL);
	g_pRCInstance = NULL;
}

void CRemoteControl::EnableRemoteControl(BOOL bEnable)
{
	m_bEnableRC = bEnable;
}

BOOL CRemoteControl::IsEnabled() const
{
	return m_bEnableRC;
}

int CRemoteControl::GetClientPermission(CString strUser) const
{
	for(int i = 0; i < m_arClients.GetCount(); i++)
		if(m_arClients.GetAt(i) == strUser)
			return m_arPermissions.GetAt(i);

	return RC_PERMISSION_UNKNOWN_CLIENT;
}

BOOL CRemoteControl::ProcessConnectionRequest(LPCTSTR lpRequest)
{
	if(m_bEnableRC == FALSE) return FALSE;
	if(lpRequest == NULL) return FALSE;

	CString strReq = lpRequest;
	if(strReq.GetLength() <= 18) return FALSE;

	CString strAppID = strReq.Left(8);
	CString strCommand = strReq.Mid(8, 8);
	CString strParam;
	UINT uListenPort, uSendPort;

	strParam = strReq.Right(strReq.GetLength() - 16);

	int nAt = strParam.ReverseFind(_T('@'));
	if(nAt <= -1) return FALSE;

	int nSharp = strParam.ReverseFind(_T('#'));
	if(nSharp <= -1) { ASSERT(FALSE); return FALSE; }
	if(nSharp <= (nAt + 1)) { ASSERT(FALSE); return FALSE; }

	strParam.Replace(_T('#'), _T(' '));
	ASSERT(strParam.GetAt(nSharp) == _T(' '));

	LPCTSTR lp = (LPCTSTR)strParam;
	uListenPort = strtoul(&lp[nAt + 1], NULL, 10);
	uSendPort = strtoul(&lp[nSharp + 1], NULL, 10);

	if((uListenPort == 0) || (uSendPort == 0)) { ASSERT(FALSE); return FALSE; }

	int nPermission = GetClientPermission(strAppID);
	if(nPermission == RC_PERMISSION_UNKNOWN_CLIENT)
	{
		if(strCommand != _T("ConnectI")) { ASSERT(FALSE); return FALSE; }

		CPermissionDlg dlg;
		dlg.m_strApp = strParam.Left(nAt);
		dlg.DoModal();

		nPermission = dlg.m_nPermission;

		m_arClients.Add(strAppID);
		m_arPermissions.Add(nPermission);
	}
	else { ASSERT(FALSE); return FALSE; }

	if(nPermission == RC_PERMISSION_DENYACCESS) return FALSE;

	if(nPermission != RC_PERMISSION_UNKNOWN_CLIENT)
	{
		UINT uPorts = (uSendPort << 16) | uListenPort;
		AfxBeginThread(RCConnectionThread, (LPVOID)uPorts);
	}
	return TRUE;
}

BOOL CRemoteControl::ProcessRequest(LPCTSTR lpRequest)
{
	if(m_bEnableRC == FALSE) return FALSE;
	if(lpRequest == NULL) return FALSE;
	ASSERT(m_pDefaultMgr != NULL); if(m_pDefaultMgr == NULL) return FALSE;

	CString strReq = lpRequest;
	if(strReq.GetLength() < 16) return FALSE;

	CString strAppID = strReq.Left(8);
	CString strCommand = strReq.Mid(8, 8);
	CString strParam;

	if(strReq.GetLength() > 16) strParam = strReq.Right(strReq.GetLength() - 16);

	int nPermission = GetClientPermission(strAppID);
	if(nPermission == RC_PERMISSION_UNKNOWN_CLIENT) return FALSE;
	else if(nPermission == RC_PERMISSION_DENYACCESS) return FALSE;

	PW_ENTRY pe;
	ZeroMemory(&pe, sizeof(PW_ENTRY));

	_GetCurrentPwTime(&pe.tCreation);
	pe.tLastAccess = pe.tCreation;
	pe.tLastMod = pe.tCreation;
	m_pDefaultMgr->GetNeverExpireTime(&pe.tExpire);

	if((strCommand == _T("EAdd_IE0")) && (nPermission != RC_PERMISSION_READONLYACCESS))
	{
		DWORD dwIEGroupID = m_pDefaultMgr->GetGroupId(_T("Internet Explorer"));
		if((dwIEGroupID == DWORD_MAX) && (nPermission != RC_PERMISSION_READONLYACCESS))
		{
			PW_GROUP pg;
			ZeroMemory(&pg, sizeof(PW_GROUP));
			pg.pszGroupName = _T("Internet Explorer");
			_GetCurrentPwTime(&pg.tCreation);
			pg.tLastAccess = pg.tCreation;
			pg.tLastMod = pg.tCreation;
			m_pDefaultMgr->GetNeverExpireTime(&pg.tExpire);
			if(m_pDefaultMgr->AddGroup(&pg) == FALSE) { ASSERT(FALSE); return FALSE; }
			dwIEGroupID = m_pDefaultMgr->GetGroupIdByIndex(m_pDefaultMgr->GetNumberOfGroups() - 1);
		}

		StringToPwEntry(&pe, strParam);
		if(pe.uGroupId == 0) pe.uGroupId = dwIEGroupID;
		m_pDefaultMgr->AddEntry(&pe);
	}
	else if(strCommand == _T("EQry_Std")) // Query entry
	{
	}
	else if(strCommand == _T("UIUpdate")) KP_Call(KPC_UPDATE_UI, 0, 0, 0);

	SAFE_DELETE_ARRAY(pe.pszTitle);
	SAFE_DELETE_ARRAY(pe.pszUserName);
	SAFE_DELETE_ARRAY(pe.pszURL);
	SAFE_DELETE_ARRAY(pe.pszPassword);
	SAFE_DELETE_ARRAY(pe.pszAdditional);
	return TRUE;
}

UINT __cdecl RCConnectionThread(LPVOID pParam)
{
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s == INVALID_SOCKET) return 0;

	u_short uListenPort = (u_short)((UINT32)pParam & 0xFFFF);
	u_short uSendPort = (u_short)(((UINT32)pParam >> 16) & 0xFFFF);

	sockaddr_in sAddr;
	sAddr.sin_family = AF_INET;
	sAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sAddr.sin_port = htons(uListenPort);

	if(bind(s, (SOCKADDR *)&sAddr, sizeof(sAddr)) == SOCKET_ERROR)
	{
		closesocket(s); return 0;
	}

	SOCKET sAccept = NULL;
	if(listen(s, 1) != SOCKET_ERROR)
	{
		while(sAccept == SOCKET_ERROR) sAccept = accept(s, NULL, NULL);

		SOCKET sSend;
		sSend = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(sSend == INVALID_SOCKET)
		{
			closesocket(s); return 0;
		}

		sockaddr_in sClient;
		sClient.sin_family = AF_INET;
		sClient.sin_addr.s_addr = inet_addr("127.0.0.1");
		sClient.sin_port = htons(uSendPort);

		if(connect(sSend, (SOCKADDR *)&sClient, sizeof(sClient)) == SOCKET_ERROR)
		{
			closesocket(s); return 0;
		}

		int bytesRecv = 0;
		char pBuf8[9];
		while(bytesRecv != SOCKET_ERROR)
		{
			bytesRecv = recv(sAccept, pBuf8, 8, 0);
			if(bytesRecv != 8) continue;

			pBuf8[8] = 0; // Terminate string
			unsigned long uLen = strtoul(pBuf8, NULL, 16);
			char *pBuf = new char[uLen + 1];

			bytesRecv = recv(sAccept, pBuf, uLen, 0);
			if(bytesRecv == (int)uLen)
			{
				pBuf[uLen] = 0; // Terminate string

				if(g_pRCInstance != NULL)
					g_pRCInstance->ProcessRequest(pBuf);
			}

			delete []pBuf; pBuf = NULL;
		}
	}

	closesocket(s);
	return 0;
}

// The entry must be unlocked already!
C_FN_SHARE LPTSTR PwEntryToString(const PW_ENTRY *lpEntry)
{
	ASSERT(lpEntry != NULL); if(lpEntry == NULL) return NULL;

	CString str, strTemp;

	strTemp.Format(_T("%u"), lpEntry->uGroupId);
	str += strTemp + _T("@");

	strTemp.Format(_T("%u"), lpEntry->uImageId);
	str += strTemp + _T("@");

	strTemp.Format(_T("%u"), _tcslen(lpEntry->pszTitle));
	str = strTemp + _T("@");
	str += lpEntry->pszTitle;

	strTemp.Format(_T("%u"), _tcslen(lpEntry->pszUserName));
	str += strTemp + _T("@");
	str += lpEntry->pszUserName;

	strTemp.Format(_T("%u"), _tcslen(lpEntry->pszURL));
	str += strTemp + _T("@");
	str += lpEntry->pszURL;

	ASSERT(lpEntry->uPasswordLen == _tcslen(lpEntry->pszPassword));
	strTemp.Format(_T("%u"), _tcslen(lpEntry->pszPassword));
	str += strTemp + _T("@");
	str += lpEntry->pszPassword;

	strTemp.Format(_T("%u"), _tcslen(lpEntry->pszAdditional));
	str += strTemp + _T("@");
	str += lpEntry->pszAdditional;

	LPTSTR lpRetBuf = new TCHAR[str.GetLength() + 1];
	_tcscpy_s(lpRetBuf, str.GetLength() + 1, (LPCTSTR)str);
	return lpRetBuf;
}

// The returned entry contains the password in plain-text
C_FN_SHARE BOOL StringToPwEntry(PW_ENTRY *pEntry, LPCTSTR lpEntryString)
{
	ASSERT(pEntry != NULL); if(pEntry == NULL) return FALSE;
	ASSERT(lpEntryString != NULL); if(lpEntryString == NULL) return FALSE;

	CString strTitle, strUserName, strURL, strPassword, strNotes, str;
	DWORD pos = 0;
	TCHAR tch;

	while(1)
	{
		tch = lpEntryString[pos];
		pos++;

		if(tch == 0) return FALSE;
		else if(tch == _T('@')) break;
		else str += tch;
	}
	pEntry->uGroupId = _tcstoul((LPCTSTR)str, NULL, 10);
	str.Empty();

	while(1)
	{
		tch = lpEntryString[pos];
		pos++;

		if(tch == 0) return FALSE;
		else if(tch == _T('@')) break;
		else str += tch;
	}
	pEntry->uImageId = _tcstoul((LPCTSTR)str, NULL, 10);
	str.Empty();

	if(ReadPwStringItem(lpEntryString, &pos, &strTitle) == FALSE) return FALSE;
	if(ReadPwStringItem(lpEntryString, &pos, &strUserName) == FALSE) return FALSE;
	if(ReadPwStringItem(lpEntryString, &pos, &strURL) == FALSE) return FALSE;
	if(ReadPwStringItem(lpEntryString, &pos, &strPassword) == FALSE) return FALSE;
	if(ReadPwStringItem(lpEntryString, &pos, &strNotes) == FALSE) return FALSE;
	ASSERT(lpEntryString[pos] == 0);

	pEntry->pszTitle = _TcsSafeDupAlloc((LPCTSTR)strTitle);
	pEntry->pszUserName = _TcsSafeDupAlloc((LPCTSTR)strUserName);
	pEntry->pszURL = _TcsSafeDupAlloc((LPCTSTR)strURL);
	pEntry->pszPassword = _TcsSafeDupAlloc((LPCTSTR)strPassword);
	pEntry->pszAdditional = _TcsSafeDupAlloc((LPCTSTR)strNotes);
	return TRUE;
}

BOOL ReadPwStringItem(LPCTSTR lpEntryString, DWORD *pdwPos, CString *pStore)
{
	ASSERT(lpEntryString != NULL); if(lpEntryString == NULL) return FALSE;
	ASSERT(pdwPos != NULL); if(pdwPos == NULL) return FALSE;
	ASSERT(pStore != NULL); if(pStore == NULL) return FALSE;

	DWORD i, cnt, pos = *pdwPos;
	TCHAR tch;
	CString str;

	pStore->Empty();

	while(1)
	{
		tch = lpEntryString[pos];
		pos++;

		if(tch == 0) return FALSE;
		else if(tch == _T('@')) break;
		else str += tch;
	}
	cnt = _tcstoul((LPCTSTR)str, NULL, 10);

	for(i = 0; i < cnt; i++)
	{
		tch = lpEntryString[pos];
		if(tch == 0) return FALSE;
		*pStore += tch;
		pos++;
	}

	*pdwPos = pos;
	return TRUE;
}
