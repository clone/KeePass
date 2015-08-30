/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2014 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___APPLICATION_UTILITIES_H___
#define ___APPLICATION_UTILITIES_H___

#pragma once

#include "../SysDefEx.h"

// Maximum temporary buffer for SecureDeleteFile
#define SDF_BUF_SIZE 4096

#define AU_MAX_WRITE_BLOCK 65535

#ifdef _UNICODE
#define SETDLLDIRECTORY_FNNAME "SetDllDirectoryW"
#else
#define SETDLLDIRECTORY_FNNAME "SetDllDirectoryA"
#endif
typedef BOOL(WINAPI *LPSETDLLDIRECTORY)(LPCTSTR lpPathName);

#ifndef PROCESS_DEP_ENABLE
#define PROCESS_DEP_ENABLE 0x00000001
#endif
#define SETPROCESSDEPPOLICY_FNNAME "SetProcessDEPPolicy"
typedef BOOL(WINAPI *LPSETPROCESSDEPPOLICY)(DWORD dwFlags);

void AU_EnsureInitialized();

HMODULE AU_LoadLibrary(LPCTSTR lpFileName);

// Get the application's directory; without \\ at the end
BOOL AU_GetApplicationDirectory(LPTSTR lpStoreBuf, DWORD dwBufLen, BOOL bFilterSpecial, BOOL bMakeURL);

#ifndef _WIN32_WCE
BOOL AU_SecureDeleteFile(LPCTSTR pszFilePath);
#endif // _WIN32_WCE

int AU_WriteBigFile(LPCTSTR lpFilePath, const BYTE* pData, DWORD dwDataSize,
	BOOL bTransacted);

void Priv_AU_EnsureOSInitialized();

BOOL AU_IsWin9xSystem();
BOOL AU_IsAtLeastWinVistaSystem();
BOOL AU_IsAtLeastWin7System();

#ifndef _WIN32_WCE
BOOL AU_RemoveZoneIdentifier(LPCTSTR lpFile);
#endif // _WIN32_WCE

#endif // ___APPLICATION_UTILITIES_H___
