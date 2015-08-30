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

#ifndef ___QUALITY_PROGRESS_CTRL_H___
#define ___QUALITY_PROGRESS_CTRL_H___

#pragma once

#include "../../KeePassLibCpp/SysDefEx.h"
#include "../../KeePassLibCpp/Util/StrUtil.h"
#include <gdiplus.h>

// CQualityProgressCtrl

class CQualityProgressCtrl : public CProgressCtrl
{
	DECLARE_DYNAMIC(CQualityProgressCtrl)

public:
	CQualityProgressCtrl();
	virtual ~CQualityProgressCtrl();

	void SetProgressText(LPCTSTR lpText)
	{
		if(lpText != NULL) m_strProgressText = lpText;
		else m_strProgressText = _T("");

		Invalidate();
	}

protected:
	DECLARE_MESSAGE_MAP()

private:
	void PaintText(HDC hDC, Gdiplus::Graphics* pg, const Gdiplus::Rect& rectDraw);
	bool PaintThemeBackground(HDC hDC, const RECT& rectClient, Gdiplus::Rect& rectDraw);

	std::basic_string<TCHAR> m_strProgressText;

public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#endif // ___QUALITY_PROGRESS_CTRL_H___
