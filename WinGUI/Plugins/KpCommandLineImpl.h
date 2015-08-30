/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2009 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___KPCOMMANDLINEIMPL_H___
#define ___KPCOMMANDLINEIMPL_H___

#include "../../KeePassLibCpp/SDK/KpSDK.h"

#pragma pack(1)

class CKpCommandLineImpl : public IKpCommandLine
{
private:
	CKpCommandLineImpl();

public:
	static CKpCommandLineImpl& Instance();

	static void ClearStatic();

	KP_DECL_IUNKNOWN;

	STDMETHODIMP_(LPCTSTR) GetFullCommandLine();

	STDMETHODIMP_(LPCTSTR) GetExecutablePath();

	STDMETHODIMP_(LPCTSTR) GetOption(LPCTSTR lpName);
	STDMETHODIMP_(LPCTSTR) GetMultiOption(LPCTSTR lpName, DWORD dwRelIndex);

	STDMETHODIMP_(LPCTSTR) GetToken(DWORD dwIndex);
	STDMETHODIMP_(LPCTSTR) GetTokenRel(LPCTSTR lpName, DWORD dwRelIndex);

	STDMETHODIMP_(LPCTSTR) GetFilePath(); // Database path

	STDMETHODIMP_(LPCTSTR) GetPassword();
	STDMETHODIMP_(LPCTSTR) GetKeyFilePath();

	STDMETHODIMP_(BOOL) IsPreselectEnabled();
	STDMETHODIMP_(BOOL) IsReadOnlyEnabled();

	STDMETHODIMP_(BOOL) ArgumentBeginsWithPrefix(LPCTSTR lpArgument);

private:
	KP_DECL_STDREFIMPL;

	LPCTSTR GetRelItemEx(LPCTSTR lpName, DWORD dwRelIndex, BOOL bReturnFullToken);
};

#pragma pack()

#endif // ___KPCOMMANDLINEIMPL_H___
