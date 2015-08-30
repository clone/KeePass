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

#include "StdAfx.h"
#include "KpInternetStream.h"

HRESULT CKpInternetStream::Close()
{
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
