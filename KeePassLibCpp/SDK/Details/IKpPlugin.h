/*
  Copyright (C) 2008-2015 Dominik Reichl
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

#ifndef ___IKPPLUGIN_H___
#define ___IKPPLUGIN_H___

#pragma once

#include "../../SysDefEx.h"
#include "IKpUnknown.h"

#pragma pack(1)

/// Interface to a plugin object.
/// Plugins typically provide this interface.
struct KP_DECL_INTERFACE("F0A52511-81F0-4F3F-96CF-89B8D02CAC08") IKpPlugin :
	public IKpUnknown
{
public:
	/// Plugins are notified of events using this method.
	/// @return In general you should always return TRUE. Only return FALSE if
	/// you want to block further code execution in KeePass in some cases.
	STDMETHOD_(BOOL, OnMessage)(DWORD dwCode, LPARAM lParamW, LPARAM lParamL) = 0;

	/// Get a plugin property.
	/// @return Property value.
	/// @return If the plugin doesn't know the property with the specified name,
	/// it should return NULL.
	/// @return The returned string must be stored statically, KeePass will not
	/// delete the returned pointer.
	STDMETHOD_(LPCTSTR, GetProperty)(LPCTSTR lpName) = 0;

	/// Set a plugin property.
	/// This method is allowed to ignore properties it doesn't know
	/// (i.e. it doesn't need to implement a generic string dictionary).
	STDMETHOD(SetProperty)(LPCTSTR lpName, LPCTSTR lpValue) = 0;

	/// Get the number of main menu items provided by the plugin.
	STDMETHOD_(DWORD, GetMenuItemCount)() = 0;

	/// Get a pointer to all main menu items provided by the plugin.
	/// @return Pointer to the first item in an array of menu items.
	/// @return The menu items must be stored statically, KeePass will not
	/// delete the returned pointer.
	STDMETHOD_(KP_MENU_ITEM*, GetMenuItems)() = 0;
};

#pragma pack()

#endif // ___IKPPLUGIN_H___
