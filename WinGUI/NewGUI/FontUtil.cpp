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
#include "FontUtil.h"
#include "NewGUICommon.h"
#include "../PwSafe.h"

LOGFONT* CFontUtil::g_plfDefault = NULL;
CFont* CFontUtil::g_pfBold = NULL;
CFont* CFontUtil::g_pfMono = NULL;
CFont* CFontUtil::g_pfSymbol = NULL;

CFontUtil::CFontUtil()
{
}

void CFontUtil::Release()
{
	if(g_plfDefault != NULL)
	{
		delete g_plfDefault;
		g_plfDefault = NULL;
	}

	if(g_pfBold != NULL)
	{
		VERIFY(g_pfBold->DeleteObject());
		delete g_pfBold;
		g_pfBold = NULL;
	}

	if(g_pfMono != NULL)
	{
		VERIFY(g_pfMono->DeleteObject());
		delete g_pfMono;
		g_pfMono = NULL;
	}

	if(g_pfSymbol != NULL)
	{
		VERIFY(g_pfSymbol->DeleteObject());
		delete g_pfSymbol;
		g_pfSymbol = NULL;
	}
}

void CFontUtil::SetDefaultFont(CFont* pf)
{
	if(pf == NULL) { ASSERT(FALSE); return; }

	if(g_plfDefault != NULL) return;

	g_plfDefault = new LOGFONT();
	ZeroMemory(g_plfDefault, sizeof(LOGFONT));
	if(pf->GetLogFont(g_plfDefault) == 0)
	{
		ASSERT(FALSE);
		delete g_plfDefault;
		g_plfDefault = NULL;
		return;
	}
}

void CFontUtil::SetDefaultFontFrom(CWnd* pWnd)
{
	if(pWnd == NULL) { ASSERT(FALSE); return; }

	CFontUtil::SetDefaultFont(pWnd->GetFont());
}

bool CFontUtil::EnsureBold(CWnd* pWndParent)
{
	UNREFERENCED_PARAMETER(pWndParent);

	if(g_pfBold == NULL)
	{
		if(g_plfDefault == NULL) { ASSERT(FALSE); return false; }

		g_pfBold = new CFont();
		if(g_pfBold->CreateFont(g_plfDefault->lfHeight, 0, 0, 0, FW_BOLD,
			FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, g_plfDefault->lfPitchAndFamily,
			g_plfDefault->lfFaceName) == FALSE)
		{
			ASSERT(FALSE);
			delete g_pfBold;
			g_pfBold = NULL;
			return false;
		}
	}

	return true;
}

bool CFontUtil::EnsureMono(CWnd* pWndParent)
{
	if(g_pfMono == NULL)
	{
		if(g_plfDefault == NULL) { ASSERT(FALSE); return false; }

		LONG nHeight = g_plfDefault->lfHeight;
		const LONG nAdd = NewGUI_Scale(1, pWndParent);
		if(nHeight < 0) nHeight -= nAdd;
		else if(nHeight > 0) nHeight += nAdd;

		g_pfMono = new CFont();
		if(g_pfMono->CreateFont(nHeight, 0, 0, 0, FW_NORMAL,
			FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, (DEFAULT_PITCH | FF_MODERN),
			_T("Courier New")) == FALSE)
		{
			ASSERT(FALSE);
			delete g_pfMono;
			g_pfMono = NULL;
			return false;
		}
	}

	return true;
}

bool CFontUtil::EnsureSymbol(CWnd* pWndParent)
{
	if(g_pfSymbol == NULL)
	{
		if(g_plfDefault == NULL) { ASSERT(FALSE); return false; }

		LONG nHeight = g_plfDefault->lfHeight;
		const LONG nAdd = NewGUI_Scale(2, pWndParent);
		if(nHeight < 0) nHeight -= nAdd;
		else if(nHeight > 0) nHeight += nAdd;

		g_pfSymbol = new CFont();
		if(g_pfSymbol->CreateFont(nHeight, 0, 0, 0, FW_NORMAL,
			FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, (DEFAULT_PITCH | FF_DONTCARE),
			CPwSafeApp::GetPasswordFont()) == FALSE)
		{
			ASSERT(FALSE);
			delete g_pfSymbol;
			g_pfSymbol = NULL;
			return false;
		}
	}

	return true;
}

void CFontUtil::AssignBold(CWnd* pWnd, CWnd* pWndParent)
{
	if(pWnd == NULL) { ASSERT(FALSE); return; }
	if(!CFontUtil::EnsureBold(pWndParent)) return;

	pWnd->SetFont(g_pfBold);
}

void CFontUtil::AssignMono(CWnd* pWnd, CWnd* pWndParent)
{
	if(pWnd == NULL) { ASSERT(FALSE); return; }
	if(!CFontUtil::EnsureMono(pWndParent)) return;

	pWnd->SetFont(g_pfMono);
}

void CFontUtil::AssignSymbol(CWnd* pWnd, CWnd* pWndParent)
{
	if(pWnd == NULL) { ASSERT(FALSE); return; }
	if(!CFontUtil::EnsureSymbol(pWndParent)) return;

	pWnd->SetFont(g_pfSymbol);
}
