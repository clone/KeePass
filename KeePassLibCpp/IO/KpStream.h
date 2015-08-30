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

#ifndef ___KP_STREAM_H___
#define ___KP_STREAM_H___

#include "../SysDefEx.h"
#include <vector>

class CKpStream
{
public:
	CKpStream();
	virtual ~CKpStream();

	virtual HRESULT Close();

	// Read up to uCount bytes. The method may read less than the requested
	// uCount bytes, but must read at least 1 byte when uCount > 0 (if it returns
	// 0 bytes, this indicates EOS). The number of bytes actually read is stored
	// in puRead, if it is not NULL.
	virtual HRESULT ReadPartial(BYTE* pbBuffer, UINT64 uCount, UINT64* puRead);

	virtual HRESULT WritePartial(const BYTE* pbBuffer, UINT64 uCount, UINT64* puWritten);

	// Read uCount bytes. If less bytes are available, the method fails.
	HRESULT Read(BYTE* pbBuffer, UINT64 uCount);

	HRESULT Write(const BYTE* pbBuffer, UINT64 uCount);

	HRESULT ReadToEnd(std::vector<BYTE>& v);
};

#endif // ___KP_STREAM_H___
