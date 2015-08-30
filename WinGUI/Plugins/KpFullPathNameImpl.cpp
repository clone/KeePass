/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2010 Dominik Reichl <dominik.reichl@t-online.de>

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
#include <tchar.h>
#include "KpFullPathNameImpl.h"

KP_IMPL_STDREFIMPL_SUB(CKpFullPathNameImpl, m_pFpn)

CKpFullPathNameImpl::CKpFullPathNameImpl(LPCTSTR lpPath)
{
	KP_IMPL_CONSTRUCT;

	std_string str((lpPath != NULL) ? lpPath : _T(""));
	m_pFpn = new FullPathName(str);
}

STDMETHODIMP CKpFullPathNameImpl::QueryInterface(REFIID riid, void** ppvObject)
{
	KP_REQ_OUT_PTR(ppvObject);

	KP_SUPPORT_INTERFACE(IID_IKpUnknown, IKpUnknown);
	KP_SUPPORT_INTERFACE(IID_IKpFullPathName, IKpFullPathName);

	*ppvObject = NULL;
	return E_NOINTERFACE;
}

STDMETHODIMP_(DWORD) CKpFullPathNameImpl::GetState()
{
	return static_cast<DWORD>(m_pFpn->getState());
}

STDMETHODIMP_(LPCTSTR) CKpFullPathNameImpl::GetFullPathName()
{
	return m_pFpn->getFullPathName().c_str();
}

STDMETHODIMP_(LPCTSTR) CKpFullPathNameImpl::GetPathOnly()
{
	return m_pFpn->getPathOnly().c_str();
}

STDMETHODIMP_(LPCTSTR) CKpFullPathNameImpl::GetFileName()
{
	return m_pFpn->getFileName().c_str();
}
