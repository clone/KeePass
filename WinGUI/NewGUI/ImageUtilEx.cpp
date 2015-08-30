/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2011 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "ImageUtilEx.h"
#include <olectl.h>

using namespace Gdiplus;

std::vector<BYTE> NewGUI_SaveIcon(HICON hIcon)
{
	std::vector<BYTE> v;
	if(hIcon == NULL) { ASSERT(FALSE); return v; }

	IStream* pStream = NULL;
	if(FAILED(CreateStreamOnHGlobal(NULL, TRUE, &pStream))) { ASSERT(FALSE); return v; }
	if(pStream == NULL) { ASSERT(FALSE); return v; }

	PICTDESC pd;
	ZeroMemory(&pd, sizeof(PICTDESC));
	pd.cbSizeofstruct = sizeof(PICTDESC);
	pd.picType = PICTYPE_ICON;
	pd.icon.hicon = hIcon;

	IPicture* pPic = NULL;
	LONG lSize = 0;
	bool bPic = SUCCEEDED(OleCreatePictureIndirect(&pd, IID_IPicture,
		FALSE, (LPVOID *)&pPic));
	bPic &= (pPic != NULL);
	if(bPic)
	{
		if(SUCCEEDED(pPic->SaveAsFile(pStream, TRUE, &lSize)))
		{
			LARGE_INTEGER liMove;
			ZeroMemory(&liMove, sizeof(LARGE_INTEGER));
			VERIFY(SUCCEEDED(pStream->Seek(liMove, STREAM_SEEK_SET, NULL)));

			v.resize(static_cast<size_t>(lSize));

			ULONG cbRead = 0;
			VERIFY(SUCCEEDED(pStream->Read(&v[0], static_cast<ULONG>(
				lSize), &cbRead)));
			if(cbRead == static_cast<ULONG>(lSize)) { } // Success
			else { ASSERT(FALSE); v.clear(); }
		}

		pPic->Release();
	}

	pStream->Release();
	return v;
}

bool NewGUI_ExtractVistaIcon(HICON hIcon, Bitmap** pOutBmp)
{
	if(hIcon == NULL) { ASSERT(FALSE); return false; }
	if(pOutBmp == NULL) { ASSERT(FALSE); return false; }
	*pOutBmp = NULL;

	std::vector<BYTE> v = NewGUI_SaveIcon(hIcon);

	const size_t SizeICONDIR = 6;
	const size_t SizeICONDIRENTRY = 16;

	const size_t cImages = *(WORD *)&v[4];
	for(size_t i = 0; i < cImages; ++i)
	{
		const BYTE bWidth = v[SizeICONDIR + (i * SizeICONDIRENTRY)];
		const BYTE bHeight = v[SizeICONDIR + (i * SizeICONDIRENTRY) + 1];
		const WORD wBitCount = *(WORD *)&v[SizeICONDIR + (i *
			SizeICONDIRENTRY) + 6];

		if((bWidth == 0) && (bHeight == 0) && (wBitCount == 32))
		{
			const size_t uSize = *(size_t *)&v[SizeICONDIR + (i *
				SizeICONDIRENTRY) + 8];
			const size_t uOffset = *(size_t *)&v[SizeICONDIR + (i *
				SizeICONDIRENTRY) + 12];

			IStream* pStream = NULL;
			if(FAILED(CreateStreamOnHGlobal(NULL, TRUE, &pStream))) { ASSERT(FALSE); return false; }
			if(pStream == NULL) { ASSERT(FALSE); return false; }

			VERIFY(SUCCEEDED(pStream->Write(&v[uOffset], uSize, NULL)));

			LARGE_INTEGER liMove;
			ZeroMemory(&liMove, sizeof(LARGE_INTEGER));
			VERIFY(SUCCEEDED(pStream->Seek(liMove, STREAM_SEEK_SET, NULL)));

			*pOutBmp = Bitmap::FromStream(pStream, FALSE);

			pStream->Release();
			return (*pOutBmp != NULL);
		}
	}

	return false;
}
