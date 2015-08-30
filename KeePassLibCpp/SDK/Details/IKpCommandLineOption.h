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

#ifndef ___IKPCOMMANDLINEOPTION_H___
#define ___IKPCOMMANDLINEOPTION_H___

#pragma once

#include "../../SysDefEx.h"
#include "IKpUnknown.h"

#pragma pack(1)

/// Interface to an object wrapping a command line option.
/// Use IKpAPI::CreateInstanceEx to create an object providing
/// this interface; use CLSID_KpCommandLineOption as CLSID, 0 as lParamW,
/// and a command line option name string pointer as lParamL.
struct KP_DECL_INTERFACE("8831B009-0194-4919-A880-67AC170A5DF1") IKpCommandLineOption :
	public IKpUnknown
{
public:
	// *** IKpUnknown methods ***
	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject) = 0;
	STDMETHOD_(ULONG, AddRef)() = 0;
	STDMETHOD_(ULONG, Release)() = 0;

	// *** IKpCommandLineOption methods ***

	/// Returns the option's name (with which the object was initialized).
	/// @kpstaticstring
	STDMETHOD_(LPCTSTR, GetOptionName)() = 0;

	/// Determine whether a given command line argument is in the
	/// form of the wrapped option.
	STDMETHOD_(BOOL, IsOption)(LPCTSTR lpArgument) = 0;

	/// If the given argument is in the form of the wrapped option,
	/// this method returns its value. @kpdeletestring
	STDMETHOD_(LPTSTR, GetOptionValue)(LPCTSTR lpArgument) = 0;

	/// Returns the zero-based offset of possible values for this option.
	STDMETHOD_(DWORD, GetOptionValueOffset)() = 0;
};

#pragma pack()

#endif // ___IKPCOMMANDLINEOPTION_H___
