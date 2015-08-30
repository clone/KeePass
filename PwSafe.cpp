/*
  Copyright (c) 2003/2004, Dominik Reichl <dominik.reichl@t-online.de>
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer. 
  - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  - Neither the name of ReichlSoft nor the names of its contributors may be
    used to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#include "StdAfx.h"
#include "PwSafe.h"
#include "PwSafeDlg.h"

#include "NewGUI/TranslateEx.h"
#include "Util/MemUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CPwSafeApp, CWinApp)
	//{{AFX_MSG_MAP(CPwSafeApp)
	//}}AFX_MSG

	// ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CPwSafeApp::CPwSafeApp()
{
}

/////////////////////////////////////////////////////////////////////////////

CPwSafeApp theApp;

/////////////////////////////////////////////////////////////////////////////

BOOL CPwSafeApp::InitInstance()
{
#ifdef _AFXDLL
	Enable3dControls();
#else
	Enable3dControlsStatic();
#endif

	// Create application's mutex object to make our presence public
	m_pAppMutex = new CMutex(FALSE, _T("KeePassApplicationMutex"), NULL);
	if(m_pAppMutex == NULL) { ASSERT(FALSE); }

	VERIFY(AfxOleInit());
	AfxEnableControlContainer();
	AfxInitRichEdit();

	// SetDialogBkColor(NewGUI_GetBgColor(), CR_FRONT); // Setup the "new" dialog look

	ASSERT(TRUE == 1);
	ASSERT(FALSE == 0);

	CPwSafeDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	return FALSE;
}

int CPwSafeApp::ExitInstance() 
{
	// Release application's mutex object
	if(m_pAppMutex != NULL)
	{
		m_pAppMutex->Unlock();
		delete m_pAppMutex;
	}

	return CWinApp::ExitInstance();
}

BOOL CPwSafeApp::RegisterShellAssociation()
{
	LONG l;
	HKEY hBase, hShell, hTemp, hTemp2;
	TCHAR tszTemp[MAX_PATH * 2];
	TCHAR tszMe[MAX_PATH * 2];
	DWORD dw;

	VERIFY(GetModuleFileName(NULL, tszMe, MAX_PATH * 2 - 2) != 0);

	// HKEY_CLASSES_ROOT/.kdb

	l = RegCreateKey(HKEY_CLASSES_ROOT, _T(".kdb"), &hBase);
	if(l != ERROR_SUCCESS) return FALSE;

	_tcscpy(tszTemp, _T("kdbfile"));
	dw = (_tcslen(tszTemp) + 1) * sizeof(TCHAR);
	l = RegSetValueEx(hBase, _T(""), 0, REG_SZ, (CONST BYTE *)tszTemp, dw);
	ASSERT(l == ERROR_SUCCESS); if(l != ERROR_SUCCESS) { RegCloseKey(hBase); return FALSE; }

	RegCloseKey(hBase);

	// HKEY_CLASSES_ROOT/kdbfile

	l = RegCreateKey(HKEY_CLASSES_ROOT, _T("kdbfile"), &hBase);
	ASSERT(l == ERROR_SUCCESS); if(l != ERROR_SUCCESS) return FALSE;

	_tcscpy(tszTemp, TRL("KeePass Password Database"));
	dw = (_tcslen(tszTemp) + 1) * sizeof(TCHAR);
	l = RegSetValueEx(hBase, _T(""), 0, REG_SZ, (CONST BYTE *)tszTemp, dw);
	ASSERT(l == ERROR_SUCCESS); if(l != ERROR_SUCCESS) { RegCloseKey(hBase); return FALSE; }

	_tcscpy(tszTemp, _T(""));
	dw = (_tcslen(tszTemp) + 1) * sizeof(TCHAR);
	l = RegSetValueEx(hBase, _T("AlwaysShowExt"), 0, REG_SZ, (CONST BYTE *)tszTemp, dw);
	ASSERT(l == ERROR_SUCCESS); if(l != ERROR_SUCCESS) { RegCloseKey(hBase); return FALSE; }

	l = RegCreateKey(hBase, _T("DefaultIcon"), &hTemp);
	ASSERT(l == ERROR_SUCCESS); if(l != ERROR_SUCCESS) return FALSE;

	_tcscpy(tszTemp, tszMe);
	_tcscat(tszTemp, _T(",0"));
	dw = (_tcslen(tszTemp) + 1) * sizeof(TCHAR);
	l = RegSetValueEx(hTemp, _T(""), 0, REG_SZ, (CONST BYTE *)tszTemp, dw);
	ASSERT(l == ERROR_SUCCESS); if(l != ERROR_SUCCESS) { RegCloseKey(hTemp); RegCloseKey(hBase); return FALSE; }

	RegCloseKey(hTemp);

	// HKEY_CLASSES_ROOT/kdbfile/shell

	l = RegCreateKey(hBase, _T("shell"), &hShell);
	ASSERT(l == ERROR_SUCCESS); if(l != ERROR_SUCCESS) return FALSE;

	// HKEY_CLASSES_ROOT/kdbfile/shell/open

	l = RegCreateKey(hShell, _T("open"), &hTemp);

	_tcscpy(tszTemp, TRL("&Open with KeePass"));
	dw = (_tcslen(tszTemp) + 1) * sizeof(TCHAR);
	l = RegSetValueEx(hTemp, _T(""), 0, REG_SZ, (CONST BYTE *)tszTemp, dw);
	ASSERT(l == ERROR_SUCCESS); if(l != ERROR_SUCCESS) { RegCloseKey(hTemp); RegCloseKey(hShell); RegCloseKey(hBase); return FALSE; }

	l = RegCreateKey(hTemp, _T("command"), &hTemp2);
	ASSERT(l == ERROR_SUCCESS); if(l != ERROR_SUCCESS) return FALSE;

	_tcscpy(tszTemp, _T("\""));
	_tcscat(tszTemp, tszMe);
	_tcscat(tszTemp, _T("\" \"%1\""));
	dw = (_tcslen(tszTemp) + 1) * sizeof(TCHAR);
	l = RegSetValueEx(hTemp2, _T(""), 0, REG_SZ, (CONST BYTE *)tszTemp, dw);
	ASSERT(l == ERROR_SUCCESS); if(l != ERROR_SUCCESS) { RegCloseKey(hTemp); RegCloseKey(hShell); RegCloseKey(hBase); return FALSE; }

	VERIFY(RegCloseKey(hTemp2) == ERROR_SUCCESS);
	VERIFY(RegCloseKey(hTemp) == ERROR_SUCCESS);

	VERIFY(RegCloseKey(hShell) == ERROR_SUCCESS);

	VERIFY(RegCloseKey(hBase) == ERROR_SUCCESS);

	return TRUE;
}

BOOL CPwSafeApp::UnregisterShellAssociation()
{
	HKEY hBase, hShell, hOpen, hCommand;
	LONG l;

	l = RegOpenKeyEx(HKEY_CLASSES_ROOT, _T(".kdb"), 0, KEY_WRITE, &hBase);
	if(l != ERROR_SUCCESS) return FALSE;

	RegDeleteValue(hBase, _T(""));
	VERIFY(RegCloseKey(hBase) == ERROR_SUCCESS);

	VERIFY(RegDeleteKey(HKEY_CLASSES_ROOT, _T(".kdb")) == ERROR_SUCCESS);

	l = RegOpenKeyEx(HKEY_CLASSES_ROOT, _T("kdbfile"), 0, KEY_WRITE, &hBase);
	if(l != ERROR_SUCCESS) return FALSE;

	l = RegOpenKeyEx(hBase, _T("shell"), 0, KEY_WRITE, &hShell);
	if(l != ERROR_SUCCESS) return FALSE;

	l = RegOpenKeyEx(hShell, _T("open"), 0, KEY_WRITE, &hOpen);
	if(l != ERROR_SUCCESS) return FALSE;

	l = RegOpenKeyEx(hOpen, _T("command"), 0, KEY_WRITE, &hCommand);
	if(l != ERROR_SUCCESS) return FALSE;

	RegDeleteValue(hCommand, _T(""));
	VERIFY(RegCloseKey(hCommand) == ERROR_SUCCESS);

	RegDeleteValue(hOpen, _T(""));
	VERIFY(RegCloseKey(hOpen) == ERROR_SUCCESS);

	RegDeleteValue(hShell, _T(""));
	VERIFY(RegCloseKey(hShell) == ERROR_SUCCESS);

	RegDeleteValue(hBase, _T(""));
	RegDeleteValue(hBase, _T("AlwaysShowExt"));
	VERIFY(RegCloseKey(hBase) == ERROR_SUCCESS);

	return TRUE;
}
