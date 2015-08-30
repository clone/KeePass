/*
  Copyright (c) 2008-2013, Dominik Reichl
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

#ifndef ___IKPAPI2_H___
#define ___IKPAPI2_H___

#pragma once

#include "../../SysDefEx.h"
#include "IKpUnknown.h"
#include "../../PwStructs.h"
#include "IKpAPI.h"

#pragma pack(1)

/// KeePass API for plugins, provides access to all objects in
/// KeePass (singletons, instantiable objects, utility method classes, etc).
struct KP_DECL_INTERFACE("5E6B4B05-4C29-407A-81D4-2BB0D866BA7F") IKpAPI2 :
	public IKpAPI
{
public:
	STDMETHOD_(BOOL, IsInitialCommandLineFile)() = 0;

	/// Reserved for future use.
	STDMETHOD(GetProperty)(DWORD dwID, void* pOutValue) = 0;

	/// Reserved for future use.
	STDMETHOD(SetProperty)(DWORD dwID, void* pNewValue) = 0;
};

#pragma pack()

#endif // ___IKPAPI2_H___
