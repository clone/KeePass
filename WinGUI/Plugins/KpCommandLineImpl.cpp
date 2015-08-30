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
#include <assert.h>
#include "KpCommandLineImpl.h"
#include "../../KeePassLibCpp/Util/StrUtil.h"
#include "../Util/CmdLine/CmdArgs.h"
#include "../Util/CmdLine/CommandLineOption.h"
#include "../Util/CmdLine/CommandLineTokens.h"
#include "../Util/CmdLine/Executable.h"

static LPCTSTR g_lpEmptyString = _T("");

static CStringSetEx g_setStrings;

KP_IMPL_STDREFIMPL_NODELETE(CKpCommandLineImpl)

CKpCommandLineImpl::CKpCommandLineImpl()
{
	KP_IMPL_CONSTRUCT;
}

CKpCommandLineImpl& CKpCommandLineImpl::Instance()
{
	static CKpCommandLineImpl singletonObject;
	return singletonObject;
}

void CKpCommandLineImpl::ClearStatic()
{
	g_setStrings.Clear();
}

STDMETHODIMP CKpCommandLineImpl::QueryInterface(REFIID riid, void** ppvObject)
{
	KP_REQ_OUT_PTR(ppvObject);

	KP_SUPPORT_INTERFACE(IID_IKpUnknown, IKpUnknown);
	KP_SUPPORT_INTERFACE(IID_IKpCommandLine, IKpCommandLine);

	*ppvObject = NULL;
	return E_NOINTERFACE;
}

STDMETHODIMP_(LPCTSTR) CKpCommandLineImpl::GetFullCommandLine()
{
	return ::GetCommandLine();
}

STDMETHODIMP_(LPCTSTR) CKpCommandLineImpl::GetExecutablePath()
{
	return Executable::instance().getFullPathName().c_str();
}

STDMETHODIMP_(LPCTSTR) CKpCommandLineImpl::GetOption(LPCTSTR lpName)
{
	return this->GetMultiOption(lpName, 0);
}

LPCTSTR CKpCommandLineImpl::GetRelItemEx(LPCTSTR lpName, DWORD dwRelIndex,
	BOOL bReturnFullToken)
{
	if((lpName == NULL) || (lpName[0] == 0)) return NULL;

	const CommandLineOption clOpt(lpName);
	DWORD dwPos = 0;

	typedef CommandLineTokens::const_iterator const_iterator;
	const const_iterator clEnd(CommandLineTokens::instance().end());

	for(const_iterator it = CommandLineTokens::instance().begin(); it != clEnd; ++it)
	{
		const std_string clArg(*it);
		if(clOpt.isOption(clArg))
		{
			if(dwPos == dwRelIndex)
			{
				if(bReturnFullToken == TRUE) return g_setStrings.Add(it->c_str());

				// Just the option's value
				return g_setStrings.Add(clOpt.optionValue(clArg).c_str());
			}
			else ++dwPos;
		}
	}

	return NULL;
}

STDMETHODIMP_(LPCTSTR) CKpCommandLineImpl::GetMultiOption(LPCTSTR lpName, DWORD dwRelIndex)
{
	return this->GetRelItemEx(lpName, dwRelIndex, FALSE);
}

STDMETHODIMP_(LPCTSTR) CKpCommandLineImpl::GetToken(DWORD dwIndex)
{
	typedef CommandLineTokens::const_iterator const_iterator;
	const const_iterator clEnd(CommandLineTokens::instance().end());

	DWORD dwPos = 0;
	for(const_iterator it = CommandLineTokens::instance().begin(); it != clEnd; ++it)
	{
		if(dwPos == dwIndex) return g_setStrings.Add(it->c_str());
		else ++dwPos;
	}

	return NULL;
}

STDMETHODIMP_(LPCTSTR) CKpCommandLineImpl::GetTokenRel(LPCTSTR lpName, DWORD dwRelIndex)
{
	return this->GetRelItemEx(lpName, dwRelIndex, TRUE);
}

STDMETHODIMP_(LPCTSTR) CKpCommandLineImpl::GetFilePath()
{
	const FullPathName& fpn = CmdArgs::instance().getDatabase();
	return (((fpn.getState() == FullPathName::PATH_AND_FILENAME) ||
		(fpn.getState() == FullPathName::PATH_ONLY)) ?
		fpn.getFullPathName().c_str() : g_lpEmptyString);
}

STDMETHODIMP_(LPCTSTR) CKpCommandLineImpl::GetPassword()
{
	return CmdArgs::instance().getPassword().c_str();
}

STDMETHODIMP_(LPCTSTR) CKpCommandLineImpl::GetKeyFilePath()
{
	return CmdArgs::instance().getKeyfile().getFullPathName().c_str();
}

STDMETHODIMP_(BOOL) CKpCommandLineImpl::IsPreselectEnabled()
{
	return (CmdArgs::instance().preselectIsInEffect() ? TRUE : FALSE);
}

STDMETHODIMP_(BOOL) CKpCommandLineImpl::IsReadOnlyEnabled()
{
	return (CmdArgs::instance().readonlyIsInEffect() ? TRUE : FALSE);
}

STDMETHODIMP_(BOOL) CKpCommandLineImpl::ArgumentBeginsWithPrefix(LPCTSTR lpArgument)
{
	if(lpArgument == NULL) return FALSE;

	std_string str(lpArgument);
	return (CommandLineOption::beginsWithPrefix(str) ? TRUE : FALSE);
}
