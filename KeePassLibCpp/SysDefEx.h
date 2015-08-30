/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2007 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___SYS_DEF_EX_H___
#define ___SYS_DEF_EX_H___

// The following types and functions must be implemented for each
// supported platform:

// Types:
// UINT8  -- 8-bit unsigned integer.
// UINT16 -- 16-bit unsigned integer.
// UINT32 -- 32-bit unsigned integer.
// UINT64 -- 64-bit unsigned integer.
// INT8   -- 8-bit unsigned integer.
// INT16  -- 16-bit unsigned integer.
// INT32  -- 32-bit unsigned integer.
// INT64  -- 64-bit unsigned integer.

// Defines:
// CPP_CLASS_SHARE -- Define to a keyword that imports or exports a C++ class.
// CPP_FN_SHARE    -- Define to a keyword that imports or exports a function
//                    that uses C++ types (class in parameters, STL type).
// C_FN_SHARE      -- Define to a keyword that imports or exports a function.
// KP_EXP          -- Define to the opposite of C_FN_SHARE.
// KP_API          -- Define to a function call type (STDCALL / CDECL).

// Include all system-specific definitions
#include "SysSpec_Windows/SysSpecWin.h"
// #include "SysSpec/SysSpecLinux.h"

// Implications, standard macros and other definitions follow.

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(p) (void)0
#endif

#ifndef UINT32_MAX
#define UINT32_MAX 0xFFFFFFFF
#endif

#ifdef KPP_NO_MFC
#define CString LPCSTR
#define CWnd    LPCSTR
#endif

#endif
