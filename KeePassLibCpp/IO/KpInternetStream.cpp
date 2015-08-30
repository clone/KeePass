/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2014 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "KpInternetStream.h"

void CKpInternetStream::EnsureInitialized()
{
	if(m_bInitialized) return;
	m_bInitialized = true;

	m_hNet = InternetOpen(PWM_PRODUCT_NAME_SHORT, INTERNET_OPEN_TYPE_PRECONFIG,
		NULL, NULL, 0);
	if(m_hNet == NULL) { m_hFile = NULL; return; }

	if(m_nConnectTimeOut >= 0)
	{
		DWORD dwTimeOut = static_cast<DWORD>(m_nConnectTimeOut);
		if(m_nConnectTimeOut == 0) dwTimeOut = DWORD_MAX;

		VERIFY(InternetSetOption(m_hNet, INTERNET_OPTION_CONNECT_TIMEOUT,
			&dwTimeOut, sizeof(DWORD)));
	}

	if(m_nConnectRetries >= 0)
	{
		DWORD dwRetries = static_cast<DWORD>(m_nConnectRetries);

		VERIFY(InternetSetOption(m_hNet, INTERNET_OPTION_CONNECT_RETRIES,
			&dwRetries, sizeof(DWORD)));
	}

	if(m_nTransferTimeOut >= 0)
	{
		DWORD dwTimeOut = static_cast<DWORD>(m_nTransferTimeOut);
		if(m_nTransferTimeOut == 0) dwTimeOut = DWORD_MAX;

		VERIFY(InternetSetOption(m_hNet, INTERNET_OPTION_SEND_TIMEOUT,
			&dwTimeOut, sizeof(DWORD)));
		VERIFY(InternetSetOption(m_hNet, INTERNET_OPTION_RECEIVE_TIMEOUT,
			&dwTimeOut, sizeof(DWORD)));
	}

	m_hFile = InternetOpenUrl(m_hNet, m_strUrl.c_str(), NULL, 0,
		INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_PRAGMA_NOCACHE |
		INTERNET_FLAG_RELOAD, 0);
}

HRESULT CKpInternetStream::Close()
{
	// EnsureInitialized();

	if(m_hFile != NULL)
	{
		VERIFY(InternetCloseHandle(m_hFile));
		m_hFile = NULL;
	}

	if(m_hNet != NULL)
	{
		VERIFY(InternetCloseHandle(m_hNet));
		m_hNet = NULL;
	}

	return S_OK;
}

#define CKPIS_R_FAIL(r) { ASSERT(FALSE); if(puRead != NULL) *puRead = 0; return (r); }

HRESULT CKpInternetStream::ReadPartial(BYTE* pbBuffer, UINT64 uCount,
	UINT64* puRead)
{
	EnsureInitialized();

	if(m_hFile == NULL) { if(puRead != NULL) *puRead = 0; return STG_E_INVALIDHANDLE; }
	if(pbBuffer == NULL) CKPIS_R_FAIL(E_POINTER);
	if(uCount > static_cast<UINT64>(DWORD_MAX)) CKPIS_R_FAIL(E_INVALIDARG);

	DWORD dwRead = 0;
	const BOOL bRes = InternetReadFile(m_hFile, pbBuffer, static_cast<DWORD>(
		uCount), &dwRead);
	if(bRes == FALSE) CKPIS_R_FAIL(STG_E_READFAULT);

	if(puRead != NULL) *puRead = dwRead;
	return S_OK;
}

#define CKPIS_W_FAIL(r) { ASSERT(FALSE); if(puWritten != NULL) *puWritten = 0; return (r); }

HRESULT CKpInternetStream::WritePartial(const BYTE* pbBuffer, UINT64 uCount,
	UINT64* puWritten)
{
	EnsureInitialized();

	if(m_hFile == NULL) { if(puWritten != NULL) *puWritten = 0; return STG_E_INVALIDHANDLE; }
	if(pbBuffer == NULL) CKPIS_W_FAIL(E_POINTER);
	if(uCount > static_cast<UINT64>(DWORD_MAX)) CKPIS_W_FAIL(E_INVALIDARG);

	DWORD dwWritten = 0;
	const BOOL bRes = InternetWriteFile(m_hFile, pbBuffer, static_cast<DWORD>(
		uCount), &dwWritten);
	if(bRes == FALSE) CKPIS_W_FAIL(STG_E_WRITEFAULT);

	if(puWritten != NULL) *puWritten = dwWritten;
	return S_OK;
}
