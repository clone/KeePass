/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2005 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "AutoRichEditCtrlFx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoRichEditCtrl

CAutoRichEditCtrlFx::CAutoRichEditCtrlFx()
{
}

CAutoRichEditCtrlFx::~CAutoRichEditCtrlFx()
{
}

BEGIN_MESSAGE_MAP(CAutoRichEditCtrlFx, CRichEditCtrl)
	//{{AFX_MSG_MAP(CAutoRichEditCtrlFx)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CString CAutoRichEditCtrlFx::GetRTF()
{
	EDITSTREAM es;

	ZeroMemory(&es, sizeof(EDITSTREAM));
	es.pfnCallback = CBStreamOut;

	CString sRTF;
	es.dwCookie = (DWORD)&sRTF;

	StreamOut(SF_RTF, es);
	return sRTF;
}

void CAutoRichEditCtrlFx::SetRTF(CString sRTF, int nStreamType)
{
	EDITSTREAM es;

	ZeroMemory(&es, sizeof(EDITSTREAM));
	es.pfnCallback = CBStreamIn;

	m_strStreamInCache = sRTF;
	es.dwCookie = (DWORD)&m_strStreamInCache;

	StreamIn(nStreamType, es);
}

DWORD CALLBACK CAutoRichEditCtrlFx::CBStreamIn(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	CString *pstr = (CString *)dwCookie;
	ASSERT(pstr != NULL); if(pstr == NULL) return 0;

	if(pstr->GetLength() < cb)
	{
		*pcb = pstr->GetLength();
		memcpy(pbBuff, (LPCSTR)*pstr, *pcb);
		pstr->Empty();
	}
	else
	{
		*pcb = cb;
		memcpy(pbBuff, (LPCSTR)*pstr, *pcb);
		*pstr = pstr->Right(pstr->GetLength() - cb);
	}

	return 0;
}

DWORD CALLBACK CAutoRichEditCtrlFx::CBStreamOut(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	UNREFERENCED_PARAMETER(pcb);

	CString *psEntry = (CString *)dwCookie;
	ASSERT(psEntry != NULL); if(psEntry == NULL) return 0;

	CString tmpEntry = (LPCSTR)pbBuff;

	if(cb != 0) *psEntry += tmpEntry.Left(cb);
	return 0;
}
