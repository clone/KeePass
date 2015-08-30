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
#include "KpMemoryStream.h"
#include "../Util/MemUtil.h"

HRESULT CKpMemoryStream::Close()
{
	if(m_pbData != NULL)
	{
		if(!m_bReading)
		{
			if(m_bClearMemory) mem_erase(m_pbData, static_cast<size_t>(m_uSize));
			delete[] m_pbData;
		}

		m_pbData = NULL;
	}

	return S_OK;
}

#define CKPMS_R_FAIL(r) { ASSERT(FALSE); if(puRead != NULL) *puRead = 0; return (r); }

HRESULT CKpMemoryStream::ReadPartial(BYTE* pbBuffer, UINT64 uCount, UINT64* puRead)
{
	if(!m_bReading) CKPMS_R_FAIL(E_UNEXPECTED);
	if(m_pbData == NULL) CKPMS_R_FAIL(STG_E_INVALIDPOINTER);
	if(uCount > static_cast<UINT64>(UINT32_MAX)) CKPMS_R_FAIL(E_INVALIDARG);
	if(pbBuffer == NULL) CKPMS_R_FAIL(E_POINTER);

	const UINT64 uMaxAvail = m_uSize - m_uPosition;
	const UINT64 uToRead = min(uMaxAvail, uCount);

	if(uToRead > 0)
		memcpy(pbBuffer, &m_pbData[m_uPosition], static_cast<size_t>(uToRead));
	m_uPosition += uToRead;

	if(puRead != NULL) *puRead = uToRead;
	return S_OK;
}

#define CKPMS_W_FAIL(r) { ASSERT(FALSE); if(puWritten != NULL) *puWritten = 0; return (r); }

HRESULT CKpMemoryStream::WritePartial(const BYTE* pbBuffer, UINT64 uCount, UINT64* puWritten)
{
	if(m_bReading) CKPMS_W_FAIL(STG_E_CANTSAVE);
	if(m_pbData == NULL) CKPMS_W_FAIL(STG_E_INVALIDPOINTER);
	if(uCount > static_cast<UINT64>(UINT32_MAX)) CKPMS_W_FAIL(E_INVALIDARG);
	if(pbBuffer == NULL) CKPMS_W_FAIL(E_POINTER);

	const HRESULT res = EnsureCapacity(m_uSize + uCount);
	if(FAILED(res)) CKPMS_W_FAIL(res);

	if(uCount > 0)
		memcpy(&m_pbData[m_uSize], pbBuffer, static_cast<size_t>(uCount));
	m_uSize += uCount;

	if(puWritten != NULL) *puWritten = uCount;
	return S_OK;
}

HRESULT CKpMemoryStream::EnsureCapacity(UINT64 uMinSize)
{
	if(m_bReading) { ASSERT(FALSE); return E_UNEXPECTED; }
	if(m_pbData == NULL) { ASSERT(FALSE); return E_UNEXPECTED; }
	if(uMinSize <= m_uCapacity) return S_OK;
	if(uMinSize >= 0x1000000000000000ULL) { ASSERT(FALSE); return E_OUTOFMEMORY; }

	UINT64 uNewCap = m_uCapacity * 2;
	while(uNewCap < uMinSize) uNewCap *= 2;
	if(uNewCap > static_cast<UINT64>(UINT32_MAX)) uNewCap = uMinSize;
	if(uNewCap > static_cast<UINT64>(UINT32_MAX)) { ASSERT(FALSE); return E_OUTOFMEMORY; }

	BYTE* pbNewBuf = new BYTE[static_cast<size_t>(uNewCap)];
	if(pbNewBuf == NULL) return E_OUTOFMEMORY;

	if(m_uSize > 0)
		memcpy(pbNewBuf, m_pbData, static_cast<size_t>(m_uSize));

	if(m_bClearMemory) mem_erase(m_pbData, static_cast<size_t>(m_uSize));
	delete[] m_pbData;

	m_pbData = pbNewBuf;
	m_uCapacity = uNewCap;
	return S_OK;
}
