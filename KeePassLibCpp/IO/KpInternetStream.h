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

#ifndef ___KP_INTERNET_STREAM_H___
#define ___KP_INTERNET_STREAM_H___

#include "KpStream.h"
#include <wininet.h>
#include "../PwManager.h"

class CKpInternetStream : public CKpStream
{
public:
	CKpInternetStream(LPCTSTR lpUrl, bool bWrite) : CKpStream(),
		m_bWrite(bWrite)
	{
		if(lpUrl != NULL) m_strUrl = lpUrl;
		else { ASSERT(FALSE); }

		m_hNet = InternetOpen(PWM_PRODUCT_NAME_SHORT, INTERNET_OPEN_TYPE_PRECONFIG,
			NULL, NULL, 0);
		if(m_hNet == NULL) { m_hFile = NULL; return; }

		m_hFile = InternetOpenUrl(m_hNet, lpUrl, NULL, 0, INTERNET_FLAG_NO_CACHE_WRITE |
			INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD, 0);
	}

	virtual ~CKpInternetStream() { Close(); }

	virtual HRESULT Close();

	virtual HRESULT ReadPartial(BYTE* pbBuffer, UINT64 uCount, UINT64* puRead);
	virtual HRESULT WritePartial(const BYTE* pbBuffer, UINT64 uCount, UINT64* puWritten);

private:
	std_string m_strUrl;
	bool m_bWrite;

	HINTERNET m_hNet;
	HINTERNET m_hFile;
};

#endif // ___KP_INTERNET_STREAM_H___
