/*
  Copyright (c) 2008-2011, Dominik Reichl
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
	// *** IKpUnknown methods ***
	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject) = 0;
	STDMETHOD_(ULONG, AddRef)() = 0;
	STDMETHOD_(ULONG, Release)() = 0;

	// *** IKpAPI2 methods ***

	/// Create an object of the class associated with a specified class ID.
	/// Class IDs supported by this method typically start with CLSID_.
	/// @param rclsid Class ID. Supported class IDs include but are not limited to:
	/// - CLSID_KpConfig (configuration object with write access).
	/// - CLSID_KpConfig_ReadOnly (configuration object with read-only access).
	/// - For a complete list of supported class IDs, see the KpGuids.h file.
	/// @param riid ID of the interface requested for the new object.
	/// @param ppvObject If the object has been created successfully and the
	/// requested interface is supported, the interface pointer will be stored
	/// at this address.
	/// @return S_OK, if successful.
	/// @return A COM error code (E_XXX), if unsuccessful.
	/// @return The returned COM object must be freed using its Release method.
	/// @see KpGuids.h file.
	STDMETHOD(CreateInstance)(REFCLSID rclsid, REFIID riid, void** ppvObject) = 0;

	/// Create an object of the class associated with a specified class ID.
	/// Class IDs supported by this method typically start with CLSID_.
	/// Objects with constructors that don't need any parameters should be
	/// created with the IKpAPI::CreateInstance method.
	/// @param rclsid Class ID. Supported class IDs include but are not limited to:
	/// - CLSID_KpFullPathName (object wrapping a path).
	/// - CLSID_KpCommandLineOption (object wrapping a command line option).
	/// - CLSID_KpFileTransaction (file transaction object).
	/// - All CLSIDs that are supported by IKpAPI::CreateInstance.
	/// - For a complete list of supported class IDs, see the KpGuids.h file.
	/// @param riid ID of the interface requested for the new object.
	/// @param ppvObject If the object has been created successfully and the
	/// requested interface is supported, the interface pointer will be stored
	/// at this address.
	/// @param lParamW Parameter depending on object type.
	/// @param lParamL Parameter depending on object type.
	/// @return S_OK, if successful.
	/// @return A COM error code (E_XXX), if unsuccessful.
	/// @return The returned COM object must be freed using its Release method.
	/// @see KpGuids.h file.
	STDMETHOD(CreateInstanceEx)(REFCLSID rclsid, REFIID riid, void** ppvObject,
		LPARAM lParamW, LPARAM lParamL) = 0;

	/// Get an interface to a static or singleton object.
	/// Class IDs supported by this method typically start with SCLSID_.
	/// @param rclsid Class ID. Supported class IDs include but are not limited to:
	/// - SCLSID_KpDatabase (database handling singleton).
	/// - SCLSID_KpUtilities (utility methods singleton).
	/// - SCLSID_KpCommandLine (command line methods singleton).
	/// - For a complete list of supported class IDs, see the KpGuids.h file.
	/// @param riid ID of the interface requested for the specified class.
	/// @param ppvObject If the object is known and the requested interface is
	/// supported, the interface pointer will be stored at this address.
	/// @return S_OK, if successful.
	/// @return A COM error code (E_XXX), if unsuccessful.
	/// @return The returned COM object must be freed using its Release method.
	/// @see KpGuids.h file.
	STDMETHOD(QueryInstance)(REFCLSID rclsid, REFIID riid, void** ppvObject) = 0;

	/// Delete an object that has been allocated by the KeePass CRT.
	/// DeleteObject should be used to free single objects/instances,
	/// DeleteArray should be used to free arrays and strings.
	/// @param pObject Object to be deleted.
	/// @see DeleteArray
	STDMETHOD(DeleteObject)(void* pObject) = 0;

	/// Delete an array/string that has been allocated by the KeePass CRT.
	/// DeleteObject should be used to free single objects/instances,
	/// DeleteArray should be used to free arrays and strings.
	/// @param pArray Array to be deleted.
	/// @see DeleteObject
	STDMETHOD(DeleteArray)(void* pArray) = 0;

	/// Get the KeePass version as word.
	/// The version is encoded as 0xAABBCCDD, where AA is the major version,
	/// BBCC the minor version and DD the build number.
	/// Example: 0x01010400 is version 1.14.
	STDMETHOD_(DWORD, GetVersion)() = 0;

	/// Get the KeePass version as string.
	/// The version string may include additional information / suffixes,
	/// like 'Debug', 'Experimental Release' or others.
	/// @return Version string.
	/// @kpstaticstring
	STDMETHOD_(LPCTSTR, GetVersionStr)() = 0;

	/// Get the size of a TCHAR in bytes (can be used to detect whether KeePass
	/// is compiled in multi-byte or Unicode mode).
	STDMETHOD_(DWORD, GetCharSize)() = 0;

	/// Get the size of a pointer in bytes (can be used to detect whether KeePass
	/// is compiled in 32 or 64 bit mode).
	STDMETHOD_(DWORD, GetPointerSize)() = 0; // Size of a pointer in bytes

	/// Get a value indicating which debugging features are present.
	/// @return 0, if no debugging information is present (release version).
	/// @return 1, if debugging information is available (debug version).
	STDMETHOD_(DWORD, GetDebugLevel)() = 0;

	/// Get a handle to the KeePass main window.
	/// The returned handle should not be stored for later use.
	STDMETHOD_(HWND, GetMainWindowHandle)() = 0;

	/// Get a handle to the client icons image list.
	/// KeePass caches various properties of this image list, therefore
	/// do not modify it -- only use read-only operations.
	STDMETHOD_(HIMAGELIST, GetClientIconsImageList)() = 0;

	/// Get the name of the currently active translation table.
	/// @return Name of the translation.
	/// @kpstaticstring
	STDMETHOD_(LPCTSTR, GetTranslationName)() = 0;

	/// Translate a string using the currently active translation file of KeePass.
	/// @param lpDefString English text (not a string identifier).
	/// @return Translated string.
	/// @kpstaticstring
	STDMETHOD_(LPCTSTR, Translate)(LPCTSTR lpDefString) = 0;

	/// Get the predefined PW_TIME value that indicates infinity.
	/// Entries and groups, which have set their expiry times to this value,
	/// will never expire.
	STDMETHOD(GetNeverExpireTime)(PW_TIME* pBuf) = 0;

	/// Get the current local system time as PW_TIME structure.
	STDMETHOD(GetCurrentPwTime)(PW_TIME* pBuf) = 0;

	/// Indicates whether there's currently a file open (if this is the case,
	/// the methods of the IKpDatabase interface can be used).
	STDMETHOD_(BOOL, IsFileOpen)() = 0;
	STDMETHOD_(LPCTSTR, GetAbsoluteDatabasePath)() = 0;

	STDMETHOD_(DWORD, GetEntryListItemCount)() = 0;
	STDMETHOD_(DWORD, GetSelectedGroupId)() = 0;

	/// Get the 'modified' bit of the current database.
	/// @see SetFileModified
	STDMETHOD_(BOOL, IsFileModified)() = 0;

	/// Set the 'modified' bit of the current database.
	/// If the 'modified' bit is set, KeePass will ask the user whether to
	/// save or not when he tries to close the database (on locking, exiting, etc).
	/// @see IsFileModified
	STDMETHOD(SetFileModified)(BOOL bModified) = 0;

	/// Get a bit indicating whether KeePass is currently showing a modal dialog.
	/// @see SetDisplayDialog
	STDMETHOD_(BOOL, IsDisplayingDialog)() = 0;

	/// Set a bit indicating whether KeePass is currently showing a modal dialog.
	/// If this bit is set, various automation is blocked (for example KeePass will
	/// not automatically lock its workspace when a modal dialog is displayed).
	/// @see IsDisplayingDialog
	STDMETHOD(SetDisplayDialog)(BOOL bDialogDisplayed) = 0;

	STDMETHOD(EntryListEnsureVisible)(INT nItem, BOOL bPartialOK) = 0;

	/// Update group tree and entry list (preserving current view if possible).
	/// This method updates both the group tree and the entry list in the main
	/// window. In contrast to the separate IKpAPI::UpdateGroupTree and
	/// IKpAPI::UpdateEntryList methods, this method tries to save and
	/// restore the current view.
	STDMETHOD(UpdateUI)() = 0;
	STDMETHOD(UpdateGroupTree)() = 0;
	STDMETHOD(UpdateEntryList)() = 0;

	/// Update the toolbar state.
	/// Toolbar buttons will be enabled/disabled depending on the current
	/// main window state (selected group, selected entries, etc).
	STDMETHOD(UpdateToolBar)() = 0;

	STDMETHOD(SetStatusBarText)(LPCTSTR lpText) = 0;

	/// Show one of the KeePass dialogs.
	/// All dialogs shown by this method are modal, the method will return
	/// when the dialog has been closed.
	/// @param dwDialogId ID of the dialog to show.
	/// @param dwVersion Version of the input/output structures.
	/// @param pConfig Input configuration structure for the dialog.
	/// @param pnResult Output buffer that receives the dialog result code.
	/// @param pOutput Output structure.
	/// @return S_OK, if the dialog has been shown correctly.
	/// @return E_NOTIMPL, if the dialog ID is not supported by KeePass.
	/// @return Any other E_XXX error code, if showing the dialog failed.
	STDMETHOD(ShowDialog)(DWORD dwDialogId, DWORD dwVersion, void* pConfig,
		INT* pnResult, void* pOutput) = 0;

	STDMETHOD(AutoType)(PW_ENTRY* pEntry, LONG_PTR lFlags) = 0;

	STDMETHOD(AddImportMenuItem)(LPCTSTR lpText, UINT uId, INT nIconId) = 0;

	STDMETHOD_(BOOL, IsInitialCommandLineFile)() = 0;

	/// Reserved for future use.
	STDMETHOD(GetProperty)(DWORD dwID, void* pOutValue) = 0;

	/// Reserved for future use.
	STDMETHOD(SetProperty)(DWORD dwID, void* pNewValue) = 0;
};

#pragma pack()

#endif // ___IKPAPI2_H___
