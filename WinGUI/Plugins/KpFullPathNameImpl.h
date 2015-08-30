/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2013 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___KPFULLPATHNAMEIMPL_H___
#define ___KPFULLPATHNAMEIMPL_H___

#include "../../KeePassLibCpp/SDK/KpSDK.h"
#include "../Util/CmdLine/FullPathName.h"

#pragma pack(1)

class CKpFullPathNameImpl : public IKpFullPathName
{
public:
	CKpFullPathNameImpl(LPCTSTR lpPath);

	KP_DECL_IUNKNOWN;

	STDMETHODIMP_(DWORD) GetState();
	STDMETHODIMP_(LPCTSTR) GetFullPathName();
	STDMETHODIMP_(LPCTSTR) GetPathOnly();
	STDMETHODIMP_(LPCTSTR) GetFileName();

private:
	KP_DECL_STDREFIMPL;

	FullPathName* m_pFpn;
};

#pragma pack()

#endif // ___KPFULLPATHNAMEIMPL_H___
