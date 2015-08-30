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
#include "SprEncoding.h"
#include "../../../KeePassLibCpp/Util/StrUtil.h"

CString SprTagSimString(LPCTSTR lpString);
CString SprEncodeHighAnsi(LPCTSTR lpText);

CString SprMakeAutoTypeSequence(LPCTSTR lpText)
{
	ASSERT(lpText != NULL); if(lpText == NULL) return CString();

	CString strText = lpText; // Local copy, lpText may mutate

	CString strEscaped = SprTagSimString(strText);
	CString strEncHA = SprEncodeHighAnsi(strEscaped);

	return strEncHA;
}

CString SprTagSimString(LPCTSTR lpString)
{
	CStringBuilderEx sb;

	ASSERT(lpString != NULL); if(lpString == NULL) return CString();

	DWORD i = 0;
	while(lpString[i] != 0)
	{
		const TCHAR tch = lpString[i];

		switch(tch)
		{
			case _T('+'): sb += _T("{PLUS}"); break;
			case _T('@'): sb += _T("{AT}"); break;
			// case _T('~'): sb += _T("{TILDE}"); break;
			case _T('~'): sb += _T("%({NUMPAD0}{NUMPAD1}{NUMPAD2}{NUMPAD6})"); break;
			case _T('^'): sb += _T("%({NUMPAD0}{NUMPAD9}{NUMPAD4})"); break;
			case _T('\''): sb += _T("%({NUMPAD0}{NUMPAD3}{NUMPAD9})"); break;
			case _T('"'): sb += _T("%({NUMPAD0}{NUMPAD3}{NUMPAD4})"); break;
			case _T('´'): sb += _T("%({NUMPAD0}{NUMPAD1}{NUMPAD8}{NUMPAD0})"); break;
			case _T('`'): sb += _T("%({NUMPAD0}{NUMPAD9}{NUMPAD6})"); break;
			case _T('%'): sb += _T("{PERCENT}"); break;
			case _T('{'): sb += _T("{LEFTBRACE}"); break;
			case _T('}'): sb += _T("{RIGHTBRACE}"); break;
			case _T('('): sb += _T("{LEFTPAREN}"); break;
			case _T(')'): sb += _T("{RIGHTPAREN}"); break;
			default: sb += tch; break;
		}

		++i;
	}

	return CString(sb.ToString().c_str());
}

CString SprEncodeHighAnsi(LPCTSTR lpText)
{
	CStringBuilderEx sb;

	DWORD dwPos = 0;
	while(lpText[dwPos] != 0)
	{
		const TCHAR tch = lpText[dwPos];
		const unsigned char uch = static_cast<unsigned char>(tch);

		if(uch >= 0x7F)
		{
			sb += _T("%({NUMPAD0}");

			CString strConv;
			strConv.Format(_T("%u"), uch);
			ASSERT(strConv.GetLength() == 3);

			for(int i = 0; i < strConv.GetLength(); ++i)
			{
				sb += _T("{NUMPAD");
				sb += strConv.GetAt(i);
				sb += _T("}");
			}

			sb += _T(")");
		}
		else sb += tch;

		++dwPos;
	}

	return CString(sb.ToString().c_str());
}

CString SprMakeCmdQuotes(LPCTSTR lpText)
{
	ASSERT(lpText != NULL); if(lpText == NULL) return CString();

	CString str = lpText;

	// See SHELLEXECUTEINFO structure documentation
	str.Replace(_T("\""), _T("\"\"\""));

	return str;
}
