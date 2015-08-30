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

#ifndef ___KP_MEMORY_STREAM_H___
#define ___KP_MEMORY_STREAM_H___

#include "KpStream.h"

class CKpMemoryStream : public CKpStream
{
public:
	CKpMemoryStream(const BYTE* pbBuffer, UINT64 uSize) : CKpStream(),
		m_bReading(true), m_uCapacity(uSize), m_uSize(uSize), m_uPosition(0),
		m_bClearMemory(false)
	{
		m_pbData = const_cast<BYTE*>(pbBuffer);
	}

	CKpMemoryStream(bool bClearMemory) : CKpStream(),
		m_bReading(false), m_uSize(0), m_uPosition(0), m_bClearMemory(bClearMemory)
	{
		m_uCapacity = 32;
		m_pbData = new BYTE[static_cast<size_t>(m_uCapacity)];
	}

	virtual ~CKpMemoryStream() { Close(); }

	virtual HRESULT Close();
	virtual HRESULT ReadPartial(BYTE* pbBuffer, UINT64 uCount, UINT64* puRead);
	virtual HRESULT WritePartial(const BYTE* pbBuffer, UINT64 uCount, UINT64* puWritten);

	BYTE* GetBuffer() const
	{
		if(m_bReading) { ASSERT(FALSE); return NULL; }
		return m_pbData;
	}

	UINT64 GetSize() const { return m_uSize; }

private:
	bool m_bReading;
	BYTE* m_pbData;
	UINT64 m_uCapacity;
	UINT64 m_uSize;
	UINT64 m_uPosition;
	bool m_bClearMemory;

	HRESULT EnsureCapacity(UINT64 uMinSize);
};

#endif // ___KP_MEMORY_STREAM_H___
