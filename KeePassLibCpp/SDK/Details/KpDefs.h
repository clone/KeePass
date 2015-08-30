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

#ifndef ___KP_DEFS_H___
#define ___KP_DEFS_H___

#pragma once

#include "../../SysDefEx.h"
#include "IKpUnknown.h"

// Interface names
#ifndef _UNICODE
#define KP_I_CREATEINSTANCE "KpCreateInstance"
#define KP_I_INITIALIZELIB  "KpInitializeLibrary"
#define KP_I_RELEASELIB     "KpReleaseLibrary"
#else
#define KP_I_CREATEINSTANCE "KpCreateInstanceW"
#define KP_I_INITIALIZELIB  "KpInitializeLibraryW"
#define KP_I_RELEASELIB     "KpReleaseLibraryW"
#endif

#define KP_I_CREATEINSTANCE_DECL KpCreateInstance
#define KP_I_INITIALIZELIB_DECL  KpInitializeLibrary
#define KP_I_RELEASELIB_DECL     KpReleaseLibrary

/////////////////////////////////////////////////////////////////////////////
// KeePass menu item flags

#define KPMIF_NORMAL      0
#define KPMIF_CHECKBOX    1
#define KPMIF_ENABLED     0
#define KPMIF_DISABLED    2
#define KPMIF_POPUP_START 4
#define KPMIF_POPUP_END   8
// To make a separator, set lpCommandString to ""

/////////////////////////////////////////////////////////////////////////////
// KeePass menu item states

#define KPMIS_UNCHECKED   0
#define KPMIS_CHECKED     1

/////////////////////////////////////////////////////////////////////////////
// Callback-like communication functions

// typedef DWORD_PTR(KP_API *LPKP_CALL)(DWORD dwCode, LPARAM lParamW, LPARAM lParamL, LPARAM lParamM);
// typedef DWORD_PTR(KP_API *LPKP_QUERY)(DWORD dwCode, LPARAM lParam);

/////////////////////////////////////////////////////////////////////////////
// KeePass plugin structures

// #pragma pack(1)

// typedef struct
// {
//	DWORD dwAppVersion; // 0.98b would be 0x00090802, 1.04 is 0x01000401
//	HWND hwndMain;
//	void *pMainDlg; // Pointer to current CPwSafeDlg class, cast it
//	void *pPwMgr;   // Pointer to current CPwManager class, cast it
//
//	LPKP_CALL lpKPCall; // KeePass call function (see KP_Call)
//	LPKP_QUERY lpKPQuery; // KeePass query function (see KP_Query)
// } KP_APP_INFO, *LPKP_APP_INFO;

// typedef struct
// {
//	DWORD dwForAppVer; // Plugin has been designed for this KeePass version
//	DWORD dwPluginVer;
//	TCHAR tszPluginName[64];
//	TCHAR tszAuthor[64]; // Author of the plugin
//	const TCHAR* cmdLineArgPrefix; // See Note 1 below.
//	DWORD dwNumCommands; // Number of menu items
//	KP_MENU_ITEM *pMenuItems;
// } KP_PLUGIN_INFO, *LPKP_PLUGIN_INFO;

// #pragma pack()

// Note 1:
// If the plugin does not define command line options, set cmdLineArgPrefix to null.
// If the plugin defines command line options, cmdLineArgPrefix should point to
// a null terminated lower case string whose last character is dot _T('.').
// This string represents the prefix for the command line arguments.

/////////////////////////////////////////////////////////////////////////////
// Functions called by KeePass (must be exported by the plugin DLL)

// KP_EXP BOOL KP_API KeePluginInit(const KP_APP_INFO *pAppInfo, KP_PLUGIN_INFO *pPluginInfo);
// typedef BOOL(KP_API *LPKEEPLUGININIT)(const KP_APP_INFO *pAppInfo, KP_PLUGIN_INFO *pPluginInfo);

// KP_EXP BOOL KP_API KeePluginCall(DWORD dwCode, LPARAM lParamW, LPARAM lParamL);
// typedef BOOL(KP_API *LPKEEPLUGINCALL)(DWORD dwCode, LPARAM lParamW, LPARAM lParamL);

// KP_EXP BOOL KP_API KeePluginExit(LPARAM lParamW, LPARAM lParamL);
// typedef BOOL(KP_API *LPKEEPLUGINEXIT)(LPARAM lParamW, LPARAM lParamL);

typedef HRESULT(KP_API *LPKPCREATEINSTANCE)(REFIID riid, void** ppvObject,
	IKpUnknown* pAPI);

typedef HRESULT(KP_API *LPKPLIBFUNC)(IKpUnknown* pAPI);

/////////////////////////////////////////////////////////////////////////////
// The structure that holds all information about one single plugin

// #pragma pack(1)

// typedef struct
// {
//	DWORD dwPluginID; // Assigned by KeePass, used internally
//	TCHAR tszFile[MAX_PATH];
//	BOOL bEnabled;
//	HINSTANCE hinstDLL;
//
//	KP_PLUGIN_INFO info;
//
//	LPKEEPLUGININIT lpInit;
//	LPKEEPLUGINCALL lpCall;
//	LPKEEPLUGINEXIT lpExit;
// } KP_PLUGIN_INSTANCE, *LPKP_PLUGIN_INSTANCE;

// #pragma pack()

/////////////////////////////////////////////////////////////////////////////
// KeePass plugin message codes

#define KPM_NULL 0
#define KPM_DIRECT_EXEC 1
#define KPM_DIRECT_CONFIG 2
#define KPM_PLUGIN_INFO 3

#define KPM_DELAYED_INIT 58

#define KPM_CLEANUP 72

// General notifications

#define KPM_INIT_MENU_POPUP 4

#define KPM_WND_INIT_POST 6
#define KPM_READ_COMMAND_ARGS 54

#define KPM_DELETE_TEMP_FILES_PRE 7
#define KPM_WM_CANCEL 12

#define KPM_PWLIST_RCLICK 18
#define KPM_GROUPLIST_RCLICK 20

#define KPM_OPENDB_PRE 25
#define KPM_OPENDB_POST 26
#define KPM_OPENDB_COMMITTED 53

#define KPM_SAVEDB_POST 49
#define KPM_SAVEDB_AS_POST 51

#define KPM_CHANGE_MASTER_KEY_PRE 50

// File menu commands

#define KPM_FILE_NEW_PRE 23
#define KPM_FILE_NEW_POST 24
#define KPM_FILE_OPEN_PRE 27
#define KPM_FILE_SAVE_PRE 28
#define KPM_FILE_SAVEAS_PRE 29
#define KPM_FILE_CLOSE_PRE 30
#define KPM_FILE_CLOSE_POST 52

#define KPM_FILE_PRINT_PRE 35
#define KPM_FILE_PRINTPREVIEW_PRE 45

#define KPM_FILE_DBSETTINGS_PRE 48
#define KPM_FILE_CHANGE_MASTER_KEY_PRE 34

#define KPM_FILE_LOCK_PRE 42
#define KPM_FILE_LOCK_CLOSE_PRE 55
#define KPM_FILE_LOCK_CLOSE_POST 56
#define KPM_FILE_UNLOCK_FAILED 57

#define KPM_FILE_EXIT_PRE 8

// Other menu commands

#define KPM_OPTIONS_PRE 31
#define KPM_OPTIONS_POST 32
#define KPM_VIEW_HIDE_STARS_PRE 11
#define KPM_GEN_PASSWORD_PRE 36
#define KPM_TANWIZARD_PRE 44
#define KPM_INFO_ABOUT_PRE 9

// Password list commands

#define KPM_ADD_ENTRY_PRE 5
#define KPM_ADD_ENTRY 14
#define KPM_EDIT_ENTRY_PRE 15
// #define KPM_EDIT_ENTRY 16
#define KPM_DELETE_ENTRY_PRE 17
#define KPM_DUPLICATE_ENTRY_PRE 40

#define KPM_PWLIST_FIND_PRE 38
#define KPM_PWLIST_FIND_IN_GROUP_PRE 39

#define KPM_MASSMODIFY_ENTRIES_PRE 43

// Direct entry commands

#define KPM_PW_COPY 19
#define KPM_USER_COPY 21
#define KPM_URL_VISIT 22

// Group list commands

#define KPM_GROUP_ADD_PRE 10
#define KPM_GROUP_ADD 13
#define KPM_GROUP_ADD_SUBGROUP_PRE 46
#define KPM_GROUP_MODIFY_PRE 37
#define KPM_GROUP_REMOVE_PRE 33
#define KPM_GROUP_SORT_PRE 47
#define KPM_GROUP_PRINT_PRE 41

// Key provider messages

#define KPM_KEYPROV_QUERY_INFO_FIRST 59
#define KPM_KEYPROV_QUERY_INFO_NEXT  60
#define KPM_KEYPROV_QUERY_KEY        61
#define KPM_KEYPROV_QUERY_KEY_EX     70
#define KPM_KEYPROV_FINALIZE         62

// Other messages

#define KPM_OPTIONS_SAVE_GLOBAL      63

#define KPM_VALIDATE_MASTERPASSWORD  64
#define KPM_VALIDATE_ENTRY           65
#define KPM_VALIDATE_GENPASSWORD     66

#define KPM_SELECTAPP_ASURL          67

#define KPM_USERAPP_GETFIRST         68
#define KPM_USERAPP_GETNEXT          69

#define KPM_TRAY_NOTIFY              71
#define KPM_TRAY_CUSTOMIZE_PRE       73
#define KPM_TRAY_CUSTOMIZE_POST      74

// The following is unused; it's always the last command ID + 1;
// do not use this value under any circumstances (it will break
// upward compatibility, if you do!)
#if (0 == 1)
#define KPM_NEXT                     75
#error Your build will not be upward-compatible.
#endif

// FREELY AVAILABLE MESSAGE CODES:
// Plugins may internally use message codes in the range
// 0xFF000000 to 0xFFFFFF00. It is guaranteed that KeePass will
// never call your message handler with any of the codes in this range.

/////////////////////////////////////////////////////////////////////////////
// KeePass query IDs (used in function KP_Query)

// #define KPQUERY_NULL    0 /* Deprecated, use KPQ_NULL */
// #define KPQUERY_VERSION 1 /* Deprecated, use KPQ_VERSION */

// #define KPQ_NULL               0
// #define KPQ_VERSION            1
// #define KPQ_FILEOPEN           2
// #define KPQ_PWLIST_ITEMCOUNT   3
// #define KPQ_SELECTEDGROUPID    4
// #define KPQ_NEVER_EXPIRE_TIME  5
// #define KPQ_GET_GROUP          6
// #define KPQ_ABSOLUTE_DB_PATH   7
// #define KPQ_TRANSLATION_NAME   8
// #define KPQ_GET_CUSTOMKVP      9
// #define KPQ_MAINWINDOW_HWND   10

/////////////////////////////////////////////////////////////////////////////
// KeePass call result codes (return value from KP_Call)

// #define KPR_SUCCESS 0
// #define KPR_FAILED  1

/////////////////////////////////////////////////////////////////////////////
// KeePass call IDs (used in function KP_Call)

// #define KPC_NULL                   0
// #define KPC_INSERT_IMPORTFROM_ITEM 1 // w: Name, l: ID, m: Icon index
// #define KPC_MODIFIED               2
// #define KPC_DISPLAYDIALOG          3
// #define KPC_PWLIST_ENSUREVISIBLE   4
// #define KPC_UPDATE_TOOLBAR         5
// #define KPC_UPDATE_GROUPLIST       6
// #define KPC_UPDATE_PASSWORDLIST    7
// #define KPC_STATUSBARTEXT          8
// #define KPC_UPDATE_UI              9
// #define KPC_OPENFILE_DIALOG       10
// #define KPC_ADD_ENTRY             11
// #define KPC_EDIT_ENTRY            12
// #define KPC_DELETE_ENTRY          13
// #define KPC_AUTOTYPE              14
// #define KPC_SET_CUSTOMKVP         15

/////////////////////////////////////////////////////////////////////////////
// KeePass call flags (used in function IKpAPI::AutoType)

#define KPF_AUTOTYPE_LOSEFOCUS  0x10000

/////////////////////////////////////////////////////////////////////////////
// Plugin string identifiers (used for GetProperty method)

#define KPPS_COMMANDLINEARGPREFIX _T("CommandLineArgPrefix")
#define KPPS_UNLOAD_LATE          _T("UnloadLate")

/////////////////////////////////////////////////////////////////////////////
// Open mode for IKpUtilities::ShellOpenLocalFile

#define OLF_OPEN    0
#define OLF_PRINT   1
#define OLF_EXPLORE 2

/////////////////////////////////////////////////////////////////////////////
// Flags for IKpUtilities::ValidatePath

#define KPVPF_DEFAULT    0
#define KPVPF_MUST_EXIST 1

// The following additional flags are only valid if KPVPF_MUST_EXIST is
// specified:
#define KPVPF_TYPE_DIRECTORY 2
#define KPVPF_TYPE_FILE      4

// Other optional checks:
#define KPVPF_REGULAR_NAME   8

/////////////////////////////////////////////////////////////////////////////
// Base64 utility method definitions

#define CB64_PROTOCOL     "base64://"
#define CB64_PROTOCOL_LEN 9

/////////////////////////////////////////////////////////////////////////////
// Configuration definitions

#define SI_REGSIZE 2048

/////////////////////////////////////////////////////////////////////////////
// IKpFullPathName state definitions

#define KPFPN_UNINITIALIZED     1
#define KPFPN_INVALID_PATH      2
#define KPFPN_PATH_ONLY         4
#define KPFPN_PATH_AND_FILENAME 8

#endif // ___KP_DEFS_H___
