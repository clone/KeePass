/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2006 Dominik Reichl <dominik.reichl@t-online.de>

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

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <afxwin.h>
#include <afxext.h>
#include <afxmt.h>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>
#endif // _AFX_NO_AFXCMN_SUPPORT

// #define CR_BACK RGB(216,216,235) /* Blue */
// #define CR_BACK RGB(240,236,224) /* WinXP */
#define CR_BACK RGB(208,208,208) /* Win2k */

#define CR_FRONT RGB(0,0,0)

#include <afxdisp.h>
#include <afxole.h>

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0500

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_STDAFX_H__206CC2C5_063D_11D8_BF16_0050BF14F5CC__INCLUDED_)
