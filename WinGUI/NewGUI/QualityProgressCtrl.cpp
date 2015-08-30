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
#include "QualityProgressCtrl.h"
#include "NewGUICommon.h"
#include "MemDC.h"

using namespace Gdiplus;

IMPLEMENT_DYNAMIC(CQualityProgressCtrl, CProgressCtrl)

CQualityProgressCtrl::CQualityProgressCtrl()
{
}

CQualityProgressCtrl::~CQualityProgressCtrl()
{
}

BEGIN_MESSAGE_MAP(CQualityProgressCtrl, CProgressCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CQualityProgressCtrl::OnPaint()
{
	CPaintDC dcPaint(this);
	// Do not call CProgressCtrl::OnPaint() for painting messages

	CMemDC dc(&dcPaint);
	HDC hDC = dc.m_hDC;

	Graphics* pg = Graphics::FromHDC(hDC);
	if(pg == NULL) { ASSERT(FALSE); return; }

	int nMinimum = 0, nMaximum = 100;
	GetRange(nMinimum, nMaximum);
	int nNormPos = GetPos() - nMinimum;
	int nNormMax = nMaximum - nMinimum;
	if(nNormMax <= 0) { ASSERT(FALSE); nNormMax = 100; }
	if(nNormPos < 0) { ASSERT(FALSE); nNormPos = 0; }
	if(nNormPos > nNormMax) { ASSERT(FALSE); nNormPos = nNormMax; }

	RECT rectClient;
	GetClientRect(&rectClient);
	const int cx = rectClient.left, cy = rectClient.top,
		cw = rectClient.right, ch = rectClient.bottom;

	CBrush *pbrBack = NULL;

	Rect rectDraw(cx + 1, cy + 1, cw - 2, ch - 2);
	if(!PaintThemeBackground(hDC, rectClient, rectDraw))
	{
		pbrBack = new CBrush();
		pbrBack->CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

		dc.FillRect(&rectClient, pbrBack);
		dc.Draw3dRect(&rectClient, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DLIGHT));
	}

	int nDrawWidth = static_cast<int>(static_cast<float>(rectDraw.Width) *
		static_cast<float>(nNormPos) / static_cast<float>(nNormMax));

	Color clrStart(255, 128, 0);
	Color clrEnd(0, 255, 0);
	if(IsWindowEnabled() == FALSE)
	{
		clrStart.SetFromCOLORREF(NewGUI_ColorToGrayscale(GetSysColor(COLOR_3DSHADOW)));
		clrEnd.SetFromCOLORREF(NewGUI_ColorToGrayscale(GetSysColor(COLOR_3DLIGHT)));
	}

	// Workaround for Windows <= XP
	Rect rectGrad(rectDraw.X, rectDraw.Y, rectDraw.Width, rectDraw.Height);
	if((::GetVersion() & 0xFF) < 6) rectGrad.Inflate(1, 0);

	LinearGradientBrush brGrad(rectGrad, clrStart, clrEnd, LinearGradientModeHorizontal);
	pg->FillRectangle(&brGrad, rectDraw.X, rectDraw.Y, nDrawWidth, rectDraw.Height);

	PaintText(hDC, pg, rectDraw);

	if(pbrBack != NULL) { pbrBack->DeleteObject(); delete pbrBack; }
	delete pg;
}

void CQualityProgressCtrl::PaintText(HDC hDC, Graphics* pg, const Rect& rectDraw)
{
	if(m_strProgressText.size() == 0) return;

	std::basic_string<WCHAR> strText = _StringToUnicodeStl(m_strProgressText.c_str());

	CFont* pf = GetFont();
	if(pf == NULL) { ASSERT(FALSE); return; }
	LOGFONT lf;
	if(pf->GetLogFont(&lf) == 0) { ASSERT(FALSE); return; }
	Font f(hDC, &lf);

	Color clrFG(0, 0, 0);
	Color clrBG(255 ^ 0x20, 255, 255, 255);
	Color clrTrans(0, 255, 255, 255);

	PointF ptOrigin(0.0f, 0.0f);
	RectF rectText;
	pg->MeasureString(strText.c_str(), static_cast<INT>(strText.size()),
		&f, ptOrigin, &rectText);

	const int dx = rectDraw.X, dy = rectDraw.Y, dw = rectDraw.Width,
		dh = rectDraw.Height;

	Rect rectGlow(0, dy, static_cast<INT>(rectText.Width), dh);
	rectGlow.X = ((dw - rectGlow.Width) / 2) + dx;
	rectGlow.Inflate(rectGlow.Width / 2, rectGlow.Height / 2);

	GraphicsPath gpGlow;
	gpGlow.AddEllipse(rectGlow);

	PathGradientBrush gbGlow(&gpGlow);
	PointF ptCenter((dw / 2.0f) + dx, (dh / 2.0f) + dy);
	gbGlow.SetCenterPoint(ptCenter);
	gbGlow.SetCenterColor(clrBG);
	INT iCount = 1;
	gbGlow.SetSurroundColors(&clrTrans, &iCount);

	Region rgOrgClip;
	pg->GetClip(&rgOrgClip);
	pg->SetClip(rectDraw);
	pg->FillPath(&gbGlow, &gpGlow);
	pg->SetClip(&rgOrgClip);

	RectF rectDrawF(static_cast<float>(dx), static_cast<float>(dy),
		static_cast<float>(dw), static_cast<float>(dh));
	StringFormat sf;
	sf.SetAlignment(StringAlignmentCenter);
	sf.SetLineAlignment(StringAlignmentCenter);
	SolidBrush sbFG(clrFG);
	pg->DrawString(strText.c_str(), static_cast<INT>(strText.size()),
		&f, rectDrawF, &sf, &sbFG);
}

bool CQualityProgressCtrl::PaintThemeBackground(HDC hDC, const RECT& rectClient,
	Gdiplus::Rect& rectDraw)
{
	CThemeHelperST th;
	if(th.IsAppThemed() == FALSE) return false;

	HTHEME h = th.OpenThemeData(m_hWnd, L"PROGRESS");
	if(h == NULL) return false;

	if(th.IsThemePartDefined(h, PP_BAR, 0) == FALSE) { th.CloseThemeData(h); return false; }

	// th.DrawThemeBackground includes DrawThemeParentBackground
	VERIFY(SUCCEEDED(th.DrawThemeBackground(h, m_hWnd, hDC, PP_BAR, 0, &rectClient,
		&rectClient)));

	RECT rectBgContent;
	memcpy(&rectBgContent, &rectClient, sizeof(RECT));
	if(FAILED(th.GetThemeBackgroundContentRect(h, hDC, PP_BAR, 0, &rectClient,
		&rectBgContent)))
	{
		ASSERT(FALSE);
		memcpy(&rectBgContent, &rectClient, sizeof(RECT));
	}
	rectDraw.X = rectBgContent.left;
	rectDraw.Y = rectBgContent.top;
	rectDraw.Width = rectBgContent.right - rectBgContent.left;
	rectDraw.Height = rectBgContent.bottom - rectBgContent.top;

	th.CloseThemeData(h);
	return true;
}

BOOL CQualityProgressCtrl::OnEraseBkgnd(CDC* pDC)
{
	UNREFERENCED_PARAMETER(pDC);
	// return CProgressCtrl::OnEraseBkgnd(pDC);
	return TRUE;
}
