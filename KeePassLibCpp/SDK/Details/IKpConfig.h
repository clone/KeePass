/*
  Copyright (c) 2008-2009, Dominik Reichl
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
	the documentation and/or other materials provided with the
	distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef ___IKPCONFIG_H___
#define ___IKPCONFIG_H___

#pragma once

#include "../../SysDefEx.h"
#include "IKpUnknown.h"

#pragma pack(1)

/// Interface to a configuration object.
/// Instances of classes supporting this interface can be created using
/// IKpAPI::CreateInstance.
struct KP_DECL_INTERFACE("6F7B9713-FC6E-4FD6-BF55-044DF9E701B0") IKpConfig :
	public IKpUnknown
{
public:
	// *** IKpUnknown methods ***
	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject) = 0;
	STDMETHOD_(ULONG, AddRef)() = 0;
	STDMETHOD_(ULONG, Release)() = 0;

	// *** IKpConfig methods ***

	/// Get the string value of a configuration item.
	/// @param pszValue This output buffer must be able to hold at
	/// least SI_REGSIZE characters (TCHARs).
	STDMETHOD_(BOOL, Get)(LPCTSTR pszField, LPTSTR pszValue) = 0;

	/// Set the string value of a configuration item.
	/// @param pszField Name of the configuration item.
	/// @param pszValue New value of the configuration item. If NULL,
	/// the configuration item is deleted.
	/// @return TRUE, if setting the item has succeeded.
	/// @return FALSE, otherwise.
	STDMETHOD_(BOOL, Set)(LPCTSTR pszField, LPCTSTR pszValue) = 0;

	/// Get the boolean value of a configuration item.
	STDMETHOD_(BOOL, GetBool)(LPCTSTR pszField, BOOL bDefault) = 0;

	/// Set the boolean value of a configuration item.
	/// @return TRUE, if setting the item has succeeded.
	/// @return FALSE, otherwise.
	STDMETHOD_(BOOL, SetBool)(LPCTSTR pszField, BOOL bValue) = 0;

	/// Get the KeePass application data path (directory, not INI
	/// file), without a terminating separator.
	STDMETHOD(GetUserPath)(LPTSTR lpBuf, DWORD dwBufMaxChars) = 0;
};

#pragma pack()

#endif // ___IKPCONFIG_H___
