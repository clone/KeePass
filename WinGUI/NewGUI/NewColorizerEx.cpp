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

#include "StdAfx.h"
#include "NewColorizerEx.h"
#include <math.h>
#include "ImageUtilEx.h"

using namespace Gdiplus;

float NewGUI_GetHue(COLORREF clr)
{
	const BYTE r = GetRValue(clr);
	const BYTE g = GetGValue(clr);
	const BYTE b = GetBValue(clr);
	if((r == g) && (g == b)) return 0.0f;

	const float fr = (float)r / 255.0f;
	const float fg = (float)g / 255.0f;
	const float fb = (float)b / 255.0f;
	const float fMax = max(fr, max(fg, fb));
	const float fMin = min(fr, min(fg, fb));

	const float d = fMax - fMin;
	float f = 0.0f;

	if(fr == fMax) f = (fg - fb) / d;
	else if(fg == fMax) f = 2.0f + (fb - fr) / d;
	else if(fb == fMax) f = 4.0f + (fr - fg) / d;

	f *= 60.0f;
	if(f < 0.0f) f += 360.0f;

	ASSERT((f >= 0.0f) && (f <= 360.0f));
	return f;
}

void NewGUI_ColorToHsv(COLORREF clr, float* pfHue, float* pfSaturation,
	float* pfValue)
{
	ASSERT((pfHue != NULL) && (pfSaturation != NULL) && (pfValue != NULL));

	const BYTE nMax = max(GetRValue(clr), max(GetGValue(clr), GetBValue(clr)));
	const BYTE nMin = min(GetRValue(clr), min(GetGValue(clr), GetBValue(clr)));

	*pfHue = NewGUI_GetHue(clr); // In degrees
	*pfSaturation = ((nMax == 0) ? 0.0f : (1.0f - ((float)nMin / nMax)));
	*pfValue = (float)nMax / 255.0f;
}

COLORREF NewGUI_ColorFromHsv(float fHue, float fSaturation, float fValue)
{
	const float d = fHue / 60.0f;
	const float fl = floor(d);
	const float f = d - fl;

	fValue *= 255.0f;
	const BYTE v = (BYTE)fValue;
	const BYTE p = (BYTE)(fValue * (1.0f - fSaturation));
	const BYTE q = (BYTE)(fValue * (1.0f - (fSaturation * f)));
	const BYTE t = (BYTE)(fValue * (1.0f - (fSaturation * (1.0f - f))));

	const int hi = (int)fl % 6;
	if(hi == 0) return RGB(v, t, p);
	if(hi == 1) return RGB(q, v, p);
	if(hi == 2) return RGB(p, v, t);
	if(hi == 3) return RGB(p, q, v);
	if(hi == 4) return RGB(t, p, v);

	return RGB(v, p, q);
}

HICON NewGUI_CreateColorizedIcon(HICON hBase, HICON hOverlay, COLORREF clr, int qSize)
{
	if(hBase == NULL) { ASSERT(FALSE); return NULL; }

	if(qSize <= 0) qSize = 48; // Large shell icon size

	Bitmap bmp(qSize, qSize, PixelFormat32bppARGB);
	Graphics *pg = Graphics::FromImage(&bmp);
	ASSERT(pg != NULL);

	VERIFY(pg->Clear(Color::Transparent) == Ok);
	VERIFY(pg->SetInterpolationMode(InterpolationModeHighQualityBicubic) == Ok);
	VERIFY(pg->SetSmoothingMode(SmoothingModeHighQuality) == Ok);

	if(qSize > 32)
	{
		Bitmap* pbmpIco = NULL;
		if(!NewGUI_ExtractVistaIcon(hBase, &pbmpIco))
			pbmpIco = Bitmap::FromHICON(hBase);

		ASSERT(pbmpIco != NULL);
		pg->DrawImage(pbmpIco, 0, 0, bmp.GetWidth(), bmp.GetHeight());
		delete pbmpIco;
	}
	else
	{
		Bitmap* pbmpIco = Bitmap::FromHICON(hBase);
		ASSERT(pbmpIco != NULL);
		pg->DrawImage(pbmpIco, 0, 0, bmp.GetWidth(), bmp.GetHeight());
		delete pbmpIco;
	}

	if(clr != DWORD_MAX)
	{
		BitmapData bd;
		Rect rect(0, 0, bmp.GetWidth(), bmp.GetHeight());
		VERIFY(bmp.LockBits(&rect, ImageLockModeRead | ImageLockModeWrite,
			PixelFormat32bppARGB, &bd) == Ok);
		const int nBytes = abs(bd.Stride * static_cast<int>(bmp.GetHeight()));
		BYTE* pbArgb = (BYTE *)bd.Scan0;

		float fHue, fSat, fVal;
		NewGUI_ColorToHsv(clr, &fHue, &fSat, &fVal);

		for(int i = 0; i < nBytes; i += 4)
		{
			if(pbArgb[i + 3] == 0) continue; // Transparent
			if((pbArgb[i] == pbArgb[i + 1]) && (pbArgb[i] == pbArgb[i + 2]))
				continue; // Gray

			COLORREF clrPixel = RGB(pbArgb[i + 2], pbArgb[i + 1],
				pbArgb[i]); // BGRA

			float h, s, v;
			NewGUI_ColorToHsv(clrPixel, &h, &s, &v);

			COLORREF clrNew = NewGUI_ColorFromHsv(fHue, s, v);

			pbArgb[i] = GetBValue(clrNew);
			pbArgb[i + 1] = GetGValue(clrNew);
			pbArgb[i + 2] = GetRValue(clrNew);
		}

		VERIFY(bmp.UnlockBits(&bd) == Ok);
	}

	if(hOverlay != NULL)
	{
		Bitmap* pOverlay = Bitmap::FromHICON(hOverlay);
		pg->DrawImage(pOverlay, 0, bmp.GetHeight() / 2, bmp.GetWidth() / 2,
			bmp.GetHeight() / 2);
		delete pOverlay;
	}

	SAFE_DELETE(pg);

	HICON hIcon = NULL;
	VERIFY(bmp.GetHICON(&hIcon) == Ok);
	return hIcon;
}

void NewGUI_UpdateColorizedIcon(HICON hDefault, HICON hOverlay, COLORREF clr, int qSize,
	HICON* phStore, COLORREF* pcStore, HICON* phAssignable, HICON* phDestructible)
{
	if(phAssignable == NULL) { ASSERT(FALSE); return; }
	if(phDestructible == NULL) { ASSERT(FALSE); return; }

	if(hOverlay != NULL)
	{
		*phDestructible = *phStore;
		*phStore = NewGUI_CreateColorizedIcon(hDefault, hOverlay, clr, qSize);
		*pcStore = clr;
		*phAssignable = *phStore;
		return;
	}

	if(clr == *pcStore)
	{
		*phAssignable = ((*phStore != NULL) ? *phStore : hDefault);
		*phDestructible = NULL;
		return;
	}

	*phDestructible = *phStore;

	if(clr == DWORD_MAX)
	{
		*phStore = NULL;
		*phAssignable = hDefault;
	}
	else
	{
		*phStore = NewGUI_CreateColorizedIcon(hDefault, hOverlay, clr, qSize);
		*phAssignable = *phStore;
	}

	*pcStore = clr;
}
