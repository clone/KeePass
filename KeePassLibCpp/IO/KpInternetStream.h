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

#ifndef ___KP_INTERNET_STREAM_H___
#define ___KP_INTERNET_STREAM_H___

#include "KpStream.h"
#include <wininet.h>
#include "../PwManager.h"

class CKpInternetStream : public CKpStream
{
public:
	CKpInternetStream(LPCTSTR lpUrl, bool bWrite) : CKpStream(),
		m_bWrite(bWrite), m_hNet(NULL), m_hFile(NULL),
		m_nConnectTimeOut(-1), m_nConnectRetries(-1), m_nTransferTimeOut(-1),
		m_bInitialized(false)
	{
		if(lpUrl != NULL) m_strUrl = lpUrl;
		else { ASSERT(FALSE); }

		// Do not call Internet* functions within the constructor,
		// because the object might be global and Internet* functions
		// cannot be called safely within constructors of global objects:
		// http://msdn.microsoft.com/en-us/library/windows/desktop/aa385098.aspx
	}

	virtual ~CKpInternetStream() { Close(); }

	// This optional method is only effective if being called before any
	// read/write call (e.g. immediately after constructing the object).
	void SetConnectOptions(int msTimeOut = -1, int nRetries = -1)
	{
		m_nConnectTimeOut = msTimeOut;
		m_nConnectRetries = nRetries;
	}

	// This optional method is only effective if being called before any
	// read/write call (e.g. immediately after constructing the object).
	void SetTransferOptions(int msTimeOut = -1)
	{
		m_nTransferTimeOut = msTimeOut;
	}

	virtual HRESULT Close();

	virtual HRESULT ReadPartial(BYTE* pbBuffer, UINT64 uCount, UINT64* puRead);
	virtual HRESULT WritePartial(const BYTE* pbBuffer, UINT64 uCount, UINT64* puWritten);

private:
	void EnsureInitialized();

	std_string m_strUrl;
	bool m_bWrite;
	bool m_bInitialized;

	HINTERNET m_hNet;
	HINTERNET m_hFile;

	int m_nConnectTimeOut;
	int m_nConnectRetries;
	int m_nTransferTimeOut;
};

#endif // ___KP_INTERNET_STREAM_H___
