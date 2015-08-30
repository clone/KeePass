/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2012 Dominik Reichl <dominik.reichl@t-online.de>

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

#if !defined(AFX_STDAFX_H__206CC2C5_063D_11D8_BF16_0050BF14F5CC__INCLUDED_)
#define AFX_STDAFX_H__206CC2C5_063D_11D8_BF16_0050BF14F5CC__INCLUDED_

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef WINVER
#define WINVER 0x0501
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif						
#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410
#endif
#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _AFX_ALL_WARNINGS

#ifndef MMNOMIDI
#define MMNOMIDI
#endif
#ifndef MMNOAUX
#define MMNOAUX
#endif
#ifndef MMNOMIXER
#define MMNOMIXER
#endif

#if (_MSC_VER >= 1400) // Manifest linking
#if defined(_M_IX86)
#pragma comment(linker, "/manifestdependency:\"type='win32' " \
	"name='Microsoft.Windows.Common-Controls' " \
	"version='6.0.0.0' " \
	"processorArchitecture='x86' " \
	"publicKeyToken='6595b64144ccf1df' " \
	"language='*'\"")
#elif defined(_M_AMD64)
#pragma comment(linker, "/manifestdependency:\"type='win32' " \
	"name='Microsoft.Windows.Common-Controls' " \
	"version='6.0.0.0' " \
	"processorArchitecture='amd64' " \
	"publicKeyToken='6595b64144ccf1df' " \
	"language='*'\"")
#elif defined(_M_IA64)
#pragma comment(linker, "/manifestdependency:\"type='win32' " \
	"name='Microsoft.Windows.Common-Controls' " \
	"version='6.0.0.0' " \
	"processorArchitecture='ia64' " \
	"publicKeyToken='6595b64144ccf1df' " \
	"language='*'\"")
#endif
#endif // (_MSC_VER >= 1400)

#include <afxwin.h>
#include <afxext.h>
#include <afxmt.h>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxdisp.h>
#include <afxole.h>

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0500

// Redefine the buggy version of AFXASSUME if we are being compiled on
// VC 2005 which generates the compiler warning "C4189: '__afx_condVal' :
// local variable is initialized but not referenced"; this bug was fixed
// in VC 2008
#if (_MSC_VER < 1500)
#ifdef AFXASSUME
#if !defined(_PREFAST_) && !defined(_DEBUG)
#undef AFXASSUME
#define AFXASSUME(cond) ((void)0)
#endif
#endif
#endif

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_STDAFX_H__206CC2C5_063D_11D8_BF16_0050BF14F5CC__INCLUDED_)
