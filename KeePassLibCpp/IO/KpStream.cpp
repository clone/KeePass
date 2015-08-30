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
#include "KpStream.h"

CKpStream::CKpStream()
{
}

CKpStream::~CKpStream()
{
	Close();
}

HRESULT CKpStream::Close()
{
	return S_OK;
}

HRESULT CKpStream::ReadPartial(BYTE* pbBuffer, UINT64 uCount, UINT64* puRead)
{
	UNREFERENCED_PARAMETER(pbBuffer);
	UNREFERENCED_PARAMETER(uCount);

	if(puRead != NULL) *puRead = 0;
	return E_NOTIMPL;
}

HRESULT CKpStream::WritePartial(const BYTE* pbBuffer, UINT64 uCount, UINT64* puWritten)
{
	UNREFERENCED_PARAMETER(pbBuffer);
	UNREFERENCED_PARAMETER(uCount);

	if(puWritten != NULL) *puWritten = 0;
	return E_NOTIMPL;
}

HRESULT CKpStream::Read(BYTE* pbBuffer, UINT64 uCount)
{
	UINT64 uPosition = 0, uRead = 0;
	while(uCount > 0)
	{
		const HRESULT res = ReadPartial(&pbBuffer[uPosition], uCount, &uRead);
		if(FAILED(res)) return res;
		if(uRead == 0) return STG_E_INCOMPLETE;

		uCount -= uRead;
		uPosition += uRead;
	}

	return S_OK;
}

HRESULT CKpStream::Write(const BYTE* pbBuffer, UINT64 uCount)
{
	UINT64 uPosition = 0, uWritten = 0;
	while(uCount > 0)
	{
		const HRESULT res = WritePartial(&pbBuffer[uPosition], uCount, &uWritten);
		if(FAILED(res)) return res;
		if(uWritten == 0) return STG_E_INCOMPLETE;

		uCount -= uWritten;
		uPosition += uWritten;
	}

	return S_OK;
}

HRESULT CKpStream::ReadToEnd(std::vector<BYTE>& v)
{
	v.clear();

	const size_t uBufSize = 1024 * 8;
	BYTE* pbBuf = new BYTE[uBufSize];
	if(pbBuf == NULL) { ASSERT(FALSE); return E_OUTOFMEMORY; }

	HRESULT r = S_OK;
	while(true)
	{
		UINT64 uRead = 0;
		HRESULT hrRead = ReadPartial(pbBuf, uBufSize, &uRead);
		if(FAILED(hrRead)) { r = hrRead; break; }
		if(uRead == 0) break;

		v.insert(v.end(), pbBuf, &pbBuf[uRead]);
	}

	delete[] pbBuf;
	return r;
}
