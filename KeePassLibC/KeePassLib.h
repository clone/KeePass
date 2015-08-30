/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2008 Dominik Reichl <dominik.reichl@t-online.de>

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

#if !defined(AFX_KEEPASSLIB_H__36D52416_E37B_401E_840F_3B39336FE5FB__INCLUDED_)
#define AFX_KEEPASSLIB_H__36D52416_E37B_401E_840F_3B39336FE5FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error Include 'StdAfx.h' before including this file for PCH
#endif

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CKeePassLibApp
// See KeePassLib.cpp for the implementation of this class

class CKeePassLibApp : public CWinApp
{
public:
	CKeePassLibApp();

	//{{AFX_VIRTUAL(CKeePassLibApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CKeePassLibApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_KEEPASSLIB_H__36D52416_E37B_401E_840F_3B39336FE5FB__INCLUDED_)
