/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2008 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___AUTORICHEDITCTRL_FX_H___
#define ___AUTORICHEDITCTRL_FX_H___

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _UNICODE	
class _AFX_RICHEDITEX_STATE
{
public:
	_AFX_RICHEDITEX_STATE();
	virtual ~_AFX_RICHEDITEX_STATE();

	HINSTANCE m_hInstRichEdit20;
};

BOOL PASCAL AfxInitRichEditEx();
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoRichEditCtrl window

class CAutoRichEditCtrlFx : public CRichEditCtrl
{
// Construction
public:
	CAutoRichEditCtrlFx();
	virtual ~CAutoRichEditCtrlFx();

	//{{AFX_VIRTUAL(CAutoRichEditCtrlFx)
	//}}AFX_VIRTUAL

	void SetRTF(CString sRTF, int nStreamType);
	CString GetRTF();

	CString m_strStreamInCache;

protected:
	//{{AFX_MSG(CAutoRichEditCtrlFx)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	static DWORD CALLBACK CBStreamIn(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);
	static DWORD CALLBACK CBStreamOut(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);

#ifdef _UNICODE	
	static DWORD CALLBACK CBStreamInRTF(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);
#endif	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(___AUTORICHEDITCTRL_FX_H___)
