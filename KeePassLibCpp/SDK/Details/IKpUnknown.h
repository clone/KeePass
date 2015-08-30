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

#ifndef ___IKPUNKNOWN_H___
#define ___IKPUNKNOWN_H___

#pragma once

#include "../../SysDefEx.h"
#include <objbase.h>

#pragma pack(1)

/// Base interface from which all other interfaces must derive.
struct KP_DECL_INTERFACE("011653D1-D7BC-429C-8632-BAA212F2BBC5") IKpUnknown
{
public:
	// *** IKpUnknown methods ***

	/// COM infrastructure, provides access to all interfaces supported
	/// by this object.
	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject) = 0;

	/// COM infrastructure, increments the reference count for this object.
	/// @return Returns the new number of references, only for debugging purposes.
	STDMETHOD_(ULONG, AddRef)() = 0;

	/// COM infrastructure, decrements the reference count for this object
	/// and eventually deletes it.
	/// If the number of references reaches zero, the object should delete itself
	/// (exception: singletons).
	/// @return Returns the new number of references, only for debugging purposes.
	STDMETHOD_(ULONG, Release)() = 0;
};

#pragma pack()

#endif // ___IKPUNKNOWN_H___
