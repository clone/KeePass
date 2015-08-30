/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2011 Dominik Reichl <dominik.reichl@t-online.de>

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

#if !defined(AFX_STDAFX_H__47C113B5_2F98_41CE_A5E6_1E85AC0DEE11__INCLUDED_)
#define AFX_STDAFX_H__47C113B5_2F98_41CE_A5E6_1E85AC0DEE11__INCLUDED_

#pragma once

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
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

#include <afxwin.h> // MFC core and standard components
#include <afxext.h> // MFC extensions

// #ifndef _AFX_NO_OLE_SUPPORT
// #include <afxole.h>         // MFC OLE classes
// #include <afxodlgs.h>       // MFC OLE dialog classes
// #include <afxdisp.h>        // MFC Automation classes
// #endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#error KeePass Library doesn't support ODBC.
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

// #include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__47C113B5_2F98_41CE_A5E6_1E85AC0DEE11__INCLUDED_)
