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

#ifndef ___IKPFILETRANSACTION_H___
#define ___IKPFILETRANSACTION_H___

#pragma once

#include "../../SysDefEx.h"
#include "IKpUnknown.h"

#pragma pack(1)

/// Interface to a file transaction object.
/// Instances of classes supporting this interface can be created using
/// IKpAPI::CreateInstanceEx.
struct KP_DECL_INTERFACE("723A18EE-9769-4843-A79E-53B82DF5C9CA") IKpFileTransaction :
	public IKpUnknown
{
public:
	/// Begin a file write transaction.
	/// @param lpOutBufferFilePath KeePass will copy the path of the file
	/// into this buffer (which should be able to hold at least MAX_PATH
	/// characters), to which you should write the data that should
	/// be written to the original file when committing.
	STDMETHOD_(BOOL, OpenWrite)(LPTSTR lpOutBufferFilePath) = 0;

	/// Commit a file write transaction.
	STDMETHOD_(BOOL, CommitWrite)() = 0;
};

#pragma pack()

#endif // ___IKPFILETRANSACTION_H___
