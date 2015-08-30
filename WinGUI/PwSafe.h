/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2009 Dominik Reichl <dominik.reichl@t-online.de>

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

#if !defined(AFX_PWSAFE_H__206CC2C1_063D_11D8_BF16_0050BF14F5CC__INCLUDED_)
#define AFX_PWSAFE_H__206CC2C1_063D_11D8_BF16_0050BF14F5CC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include <afxmt.h>
#include "Resource.h"
#include "../KeePassLibCpp/SysDefEx.h"

#define MTXNAME_LOCAL  _T("KeePassApplicationMutex")
#define MTXNAME_GLOBAL _T("KeePassAppMutexExI")

typedef BOOL(WINAPI *LPINITIALIZESECURITYDESCRIPTOR)(
	PSECURITY_DESCRIPTOR pSecurityDescriptor, DWORD dwRevision);
typedef BOOL(WINAPI *LPSETSECURITYDESCRIPTORDACL)(
	PSECURITY_DESCRIPTOR pSecurityDescriptor, BOOL bDaclPresent,
	PACL pDacl, BOOL bDaclDefaulted);

/////////////////////////////////////////////////////////////////////////////

class CPwSafeApp : public CWinApp
{
public:
	CPwSafeApp();

	static BOOL RegisterShellAssociation();
	static BOOL UnregisterShellAssociation();

	static BOOL GetStartWithWindows();
	static BOOL SetStartWithWindows(BOOL bAutoStart);

	// static BOOL ParseCurrentCommandLine(CString *psFile, LPCTSTR *lpPassword, LPCTSTR *lpKeyFile, LPCTSTR *lpPreSelectPath);

	static void CreateHiColorImageList(CImageList *pImageList, WORD wResourceID, int czSize);

	static BOOL IsMBThreadACP();
	static TCHAR GetPasswordCharacter();
	static LPCTSTR GetPasswordFont();

private:
	static BOOL ProcessControlCommands();
	static HANDLE CreateGlobalMutex();

	void _App_CleanUp();

	CMutex *m_pAppMutex;
	HANDLE m_hGlobalMutex;

	//{{AFX_VIRTUAL(CPwSafeApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CPwSafeApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PWSAFE_H__206CC2C1_063D_11D8_BF16_0050BF14F5CC__INCLUDED_)
