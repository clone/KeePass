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

#ifndef ___IKPAPI3_H___
#define ___IKPAPI3_H___

#pragma once

#include "../../SysDefEx.h"
#include "IKpUnknown.h"
#include "../../PwStructs.h"
#include "IKpAPI2.h"

#pragma pack(1)

/// KeePass API for plugins, provides access to all objects in
/// KeePass (singletons, instantiable objects, utility method classes, etc).
struct KP_DECL_INTERFACE("25BA9CB9-DE60-4076-AAD9-ABC74663A872") IKpAPI3 :
	public IKpAPI2
{
public:
	/// Get whether the current file/workspace is locked or not.
	STDMETHOD_(BOOL, IsFileLocked)() = 0;

	/// Lock or unlock the workspace.
	STDMETHOD(LockFile)(BOOL bLock) = 0;

	STDMETHOD_(BOOL, IsFileReadOnly)() = 0;

	STDMETHOD(NotifyUserActivity)() = 0;

	STDMETHOD(ParseAndOpenUrlWithEntryInfo)(LPCTSTR lpUrl, PW_ENTRY* pEntry) = 0;

	STDMETHOD_(BOOL, CanSort)() = 0;
	STDMETHOD(SortListIfAutoSort)() = 0;
	STDMETHOD(SortList)(DWORD dwByField, BOOL bAutoSortCall) = 0;

	STDMETHOD(EntryListSaveView)() = 0;
	STDMETHOD(EntryListRestoreView)() = 0;

	STDMETHOD(GroupTreeSaveView)(BOOL bSaveSelection) = 0;
	STDMETHOD(GroupTreeRestoreView)() = 0;

	STDMETHOD_(BOOL, RemoveSearchGroup)() = 0;

	STDMETHOD(UpdateTitleBar)() = 0;
	STDMETHOD(UpdateTrayIcon)() = 0;

	/// Save various view parameters (last selected group and
	/// entry, ...) into the database.
	STDMETHOD(UpdateGuiToManager)() = 0;
	
	STDMETHOD(UpdateCachedGroupIDs)() = 0;

	STDMETHOD_(BOOL, IsUnsafeAllowed)(HWND hWndParent) = 0;

	STDMETHOD(Find)(DWORD dwFindGroupId) = 0;
	STDMETHOD(QuickFind)(LPCTSTR lpText) = 0;

	STDMETHOD(ShowToolBar)(BOOL bShow) = 0;

	STDMETHOD_(UINT, GetControlMessageID)() = 0;

	STDMETHOD_(BOOL, IsInMiniMode)() = 0;
};

#pragma pack()

#endif // ___IKPAPI3_H___
