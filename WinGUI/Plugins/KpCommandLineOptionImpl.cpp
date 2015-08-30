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
#include <tchar.h>
#include "KpCommandLineOptionImpl.h"
#include "../../KeePassLibCpp/Util/StrUtil.h"

KP_IMPL_STDREFIMPL_SUB(CKpCommandLineOptionImpl, m_pOpt)

CKpCommandLineOptionImpl::CKpCommandLineOptionImpl(LPCTSTR lpOptionName)
{
	KP_IMPL_CONSTRUCT;

	std_string str((lpOptionName != NULL) ? lpOptionName : _T(""));
	m_pOpt = new CommandLineOption(str);
}

STDMETHODIMP CKpCommandLineOptionImpl::QueryInterface(REFIID riid, void** ppvObject)
{
	KP_REQ_OUT_PTR(ppvObject);

	KP_SUPPORT_INTERFACE(IID_IKpUnknown, IKpUnknown);
	KP_SUPPORT_INTERFACE(IID_IKpCommandLineOption, IKpCommandLineOption);

	*ppvObject = NULL;
	return E_NOINTERFACE;
}

STDMETHODIMP_(LPCTSTR) CKpCommandLineOptionImpl::GetOptionName()
{
	return m_pOpt->optionName().c_str();
}

STDMETHODIMP_(BOOL) CKpCommandLineOptionImpl::IsOption(LPCTSTR lpArgument)
{
	if(lpArgument == NULL) return FALSE;

	std_string str(lpArgument);
	return (m_pOpt->isOption(str) ? TRUE : FALSE);
}

STDMETHODIMP_(LPTSTR) CKpCommandLineOptionImpl::GetOptionValue(LPCTSTR lpArgument)
{
	if(lpArgument == NULL) return FALSE;

	std_string str(lpArgument);
	return _TcsSafeDupAlloc(m_pOpt->optionValue(str).c_str());
}

STDMETHODIMP_(DWORD) CKpCommandLineOptionImpl::GetOptionValueOffset()
{
	return static_cast<DWORD>(m_pOpt->optionValueOffset());
}
