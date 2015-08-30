/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2015 Dominik Reichl <dominik.reichl@t-online.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef ___WIN_UTIL_EX_H___
#define ___WIN_UTIL_EX_H___

#include "../../KeePassLibCpp/SysDefEx.h"
#include "../../KeePassLibCpp/PwManager.h"
#include "../../KeePassLibCpp/PasswordGenerator/PasswordGenerator.h"

#include "SprEngine/SprEngine.h"

#include <string>

#define KPSW_SHOWDEFAULT SW_SHOWDEFAULT

// The following definitions must be at least 256, maximum 32767
#define WU_MAX_USER_LEN    1024
#define WU_MAX_MACHINE_LEN 1024

#define CFN_CLIPBOARD_VIEWER_IGNORE _T("Clipboard Viewer Ignore")

#define APPHS_LOCAL  0
#define APPHS_ONLINE 1

typedef struct _AV_APP_INFO
{
	std::basic_string<TCHAR> strDisplayName;
	std::basic_string<TCHAR> strPath;
} AV_APP_INFO;

#ifndef _WIN32_WCE
void CopyStringToClipboard(const TCHAR *lptString, PW_ENTRY *pEntryContext,
	CPwManager *pDatabaseContext);
void ClearClipboardIfOwner();

void RegisterOwnClipboardData(unsigned char* pData, unsigned long dwDataSize);

// Thanks to Gabe Martin for the contribution of the following
// two secure clipboard functions!
// http://sourceforge.net/tracker/index.php?func=detail&aid=1102906&group_id=95013&atid=609910
BOOL MakeClipboardDelayRender(HWND hOwner, HWND *phNextCB);
void CopyDelayRenderedClipboardData(const TCHAR *lptString, CPwManager *pReferenceSource);

void SetClipboardIgnoreFormat();
#endif

CString MakeRelativePathEx(LPCTSTR lpBaseFile, LPCTSTR lpTargetFile);
CString GetShortestAbsolutePath(LPCTSTR lpFilePath);

BOOL GetRegKeyEx(HKEY hkey, LPCTSTR lpSubKey, LPTSTR lpRetData);
std::basic_string<TCHAR> GetRegStrEx(HKEY hkeyBase, LPCTSTR lpSubKey,
	LPCTSTR lpValueName, DWORD dwMaxValueSize);
BOOL OpenUrlInNewBrowser(LPCTSTR lpURL);
BOOL OpenUrlUsingPutty(LPCTSTR lpURL, LPCTSTR lpUser);

// If hParent is not NULL, the function will show an error message if
// the URL cannot be opened
void OpenUrlEx(LPCTSTR lpURL, HWND hParent);

// Internal functions
void OpenUrlShellExec(LPCTSTR lpURL, HWND hParent);
void OpenUrlProcess(LPCTSTR lpURL, HWND hParent);

void WU_SysExecute(LPCTSTR lpFile, LPCTSTR lpParameters, HWND hParent);
BOOL WU_SysCreateProcess(LPCTSTR lpFile, LPCTSTR lpParameters);

BOOL _FileAccessible(LPCTSTR lpFile);
BOOL _FileWritable(LPCTSTR lpFile);

int _OpenLocalFile(LPCTSTR szFile, int nMode);
BOOL WU_GetFileNameSz(BOOL bOpenMode, LPCTSTR lpSuffix, LPTSTR lpStoreBuf, DWORD dwBufLen);

std::vector<std::basic_string<TCHAR> > WU_GetFileNames(BOOL bOpenMode,
	LPCTSTR lpSuffix, LPCTSTR lpFilter, BOOL bAllowMultiSelect,
	CWnd* pParent, BOOL bAddToRecent, BOOL bNoChangeDir);

int WU_GetAppHelpSource();
void WU_SetAppHelpSource(int nSource);
BOOL WU_OpenAppHelp(LPCTSTR lpTopicFile, HWND hParent);
void WU_RemoveAppHelpZoneIdentifier();

UINT TWinExec(LPCTSTR lpCmdLine, WORD uCmdShow);

BOOL WU_SupportsMultiLineTooltips();

std::basic_string<TCHAR> WU_GetTempDirectory();
std::basic_string<TCHAR> WU_GetTempFile(LPCTSTR lpSuffix);
std::basic_string<TCHAR> WU_GetUserName();

// BOOL ContainsChildWindow(HWND hWndContainer, LPCTSTR lpChildWindowText);
// BOOL CALLBACK CcwEnumChildProc(HWND hWnd, LPARAM lParam);

void SafeActivateNextWindow(HWND hWndBase);

HWND WU_ShowWindowInTaskbar(HWND hWndShow, HWND hParent, BOOL bShow);

std::basic_string<TCHAR> WU_ExpandEnvironmentVars(LPCTSTR lpSrc);

BOOL WU_IsAbsolutePath(LPCTSTR lpPath);

void WU_FillPlaceholders(CString* pString, const SPR_CONTENT_FLAGS* pcf);

BOOL WU_FlushStorageBuffers(TCHAR tchDriveLetter, BOOL bOnlyIfRemovable);
BOOL WU_FlushStorageBuffersEx(LPCTSTR lpFileOnStorage, BOOL bOnlyIfRemovable);

std::basic_string<TCHAR> WU_GetCurrentDirectory();
void WU_SetCurrentDirectory(LPCTSTR lpDirectory);
std::basic_string<TCHAR> WU_FreeDriveIfCurrent(TCHAR tchDriveLetter);

PWG_ERROR PwgGenerateWithExtVerify(std::vector<TCHAR>& vOutPassword,
	const PW_GEN_SETTINGS_EX* pSettings, CNewRandom* pRandomSource);

CString FilterTrlComment(LPCTSTR lpEnglishString);

void WU_GetUserApplications(std::vector<AV_APP_INFO>& vStorage);

HRESULT WU_CreateDirectoryTree(LPCTSTR lpDirPath);

// void WU_MouseClick(bool bRightClick);

bool WU_IsCommandLineURL(const CString& strURL);

BOOL WU_RunElevated(LPCTSTR lpExe, LPCTSTR lpArgs, HWND hParent);

// std::basic_string<TCHAR> WU_CreateBackupFile(LPCTSTR lpBaseFile);

bool WU_StdInReadLine(std::basic_string<TCHAR>& strOut);

// Read a password from StdIn. The password is read only once
// and then cached.
std::basic_string<TCHAR> WU_StdInReadPassword();

std::basic_string<TCHAR> WU_GetEnv(LPCTSTR lpVarName, bool bExpand);

#endif
