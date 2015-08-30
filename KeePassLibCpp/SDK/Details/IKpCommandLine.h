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

#ifndef ___IKPCOMMANDLINE_H___
#define ___IKPCOMMANDLINE_H___

#pragma once

#include "../../SysDefEx.h"
#include "IKpUnknown.h"

#pragma pack(1)

/// Interface to a command line handling object.
/// Use IKpAPI::QueryInstance to retrieve an interface to a singleton
/// object in KeePass handling command line queries.
struct KP_DECL_INTERFACE("0934C282-6EDC-4073-913F-28B98C0420F8") IKpCommandLine :
	public IKpUnknown
{
public:
	// *** IKpUnknown methods ***
	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject) = 0;
	STDMETHOD_(ULONG, AddRef)() = 0;
	STDMETHOD_(ULONG, Release)() = 0;

	// *** IKpCommandLine methods ***

	/// Get the complete command line string for the KeePass process.
	/// @kpstaticstring
	STDMETHOD_(LPCTSTR, GetFullCommandLine)() = 0;

	/// Get the path of the KeePass executable file.
	/// @return Full executable path.
	/// @kpstaticstring
	STDMETHOD_(LPCTSTR, GetExecutablePath)() = 0;

	/// Get a command line option's value.
	/// @param lpName Name of the command line option/switch.
	/// @return NULL, if the option is not present in the command line.
	/// @return Non-NULL, the option's value, if the option is present in the command
	/// line. If no value is specified (command line switch), the string is empty.
	/// @kpstaticstring
	STDMETHOD_(LPCTSTR, GetOption)(LPCTSTR lpName) = 0;

	/// Get a command line option's value; for options that may appear multiple
	/// times in the command line.
	/// To enumerate all options, call this method with increasing index until
	/// you get back NULL.
	/// @param lpName Name of the command line option/switch.
	/// @param dwRelIndex Zero-based relative index of the requested command line
	/// option/switch. Relative means the index of the requested option in the
	/// subset of all options with the specified name.
	/// @return NULL, if the option is not present in the command line.
	/// @return Non-NULL, the option's value, if the option is present in the command
	/// line. If no value is specified (command line switch), the string is empty.
	/// @kpstaticstring
	STDMETHOD_(LPCTSTR, GetMultiOption)(LPCTSTR lpName, DWORD dwRelIndex) = 0;

	/// Get a command line token (option name and value).
	/// To enumerate all tokens, call this method with increasing index until
	/// you get back NULL.
	/// @param dwIndex Index of the command line token to get.
	/// @return NULL, if the token is not present in the command line.
	/// @return Non-NULL, the token, if the token is present in the command
	/// line.
	/// @kpstaticstring
	STDMETHOD_(LPCTSTR, GetToken)(DWORD dwIndex) = 0;

	/// Get a command line token (option name and value); tokens are identified
	/// relatively by an index into the subset of tokens with the
	/// specified command line option name.
	/// To enumerate all tokens, call this method with increasing index until
	/// you get back NULL.
	/// @param lpName Name of the command line option/switch.
	/// @param dwRelIndex Zero-based relative index of the requested command line
	/// token. Relative means the index of the requested token in the
	/// subset of all token with the specified option name.
	/// @return NULL, if the token is not present in the command line.
	/// @return Non-NULL, the token's value, if the token is present in the command
	/// line.
	/// @kpstaticstring
	STDMETHOD_(LPCTSTR, GetTokenRel)(LPCTSTR lpName, DWORD dwRelIndex) = 0;

	/// Get the file path specified on the command line.
	/// This typically is a path to a KDB database file.
	/// @return Full path to the file. If no file has been specified, the string is empty.
	/// @kpstaticstring
	STDMETHOD_(LPCTSTR, GetFilePath)() = 0;

	/// Get the password specified on the command line.
	/// @return The specified password.
	/// @kpstaticstring
	STDMETHOD_(LPCTSTR, GetPassword)() = 0;

	/// Get the key file path specified on the command line.
	/// @return The key file path.
	/// @kpstaticstring
	STDMETHOD_(LPCTSTR, GetKeyFilePath)() = 0;

	/// Returns whether the '-preselect' option is present in the command
	/// line or not.
	STDMETHOD_(BOOL, IsPreselectEnabled)() = 0;

	/// Returns whether the '-readonly' option is present in the command
	/// line or not.
	STDMETHOD_(BOOL, IsReadOnlyEnabled)() = 0;

	/// Determine whether an argument begins with a prefix character.
	STDMETHOD_(BOOL, ArgumentBeginsWithPrefix)(LPCTSTR lpArgument) = 0;
};

#pragma pack()

#endif // ___IKPCOMMANDLINE_H___
