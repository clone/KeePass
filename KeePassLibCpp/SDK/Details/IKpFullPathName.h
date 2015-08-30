/*
  Copyright (c) 2008-2012, Dominik Reichl
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

#ifndef ___IKPFULLPATHNAME_H___
#define ___IKPFULLPATHNAME_H___

#pragma once

#include "../../SysDefEx.h"
#include "IKpUnknown.h"

#pragma pack(1)

/// Interface to an object wrapping a path.
/// Use IKpAPI::CreateInstanceEx to create an object providing
/// this interface; use CLSID_KpFullPathName as CLSID, 0 as lParamW,
/// and a path string pointer as lParamL.
struct KP_DECL_INTERFACE("D55D9ADD-49DD-4704-BED1-539A5797B16C") IKpFullPathName :
	public IKpUnknown
{
public:
	/// Get the state of this object.
	/// @return Possible return values are:
	/// - KPFPN_UNINITIALIZED. The object has not been initialized yet.
	/// - KPFPN_INVALID_PATH. The wrapped path is invalid.
	/// - KPFPN_PATH_ONLY. The wrapped path ends in "\".
	/// - KPFPN_PATH_AND_FILENAME. The wrapped path does not end in "\";
	///   last part is filename.
	STDMETHOD_(DWORD, GetState)() = 0;

	/// Get the full path and file name.
	/// @kpstaticstring
	STDMETHOD_(LPCTSTR, GetFullPathName)() = 0;

	/// Get only the path.
	/// @kpstaticstring
	STDMETHOD_(LPCTSTR, GetPathOnly)() = 0;

	/// Get only the file name.
	/// @kpstaticstring
	STDMETHOD_(LPCTSTR, GetFileName)() = 0;
};

#pragma pack()

#endif // ___IKPFULLPATHNAME_H___
