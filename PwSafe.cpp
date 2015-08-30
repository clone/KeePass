/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2005 Dominik Reichl <dominik.reichl@t-online.de>

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

#include "StdAfx.h"
#include "PwSafe.h"
#include "PwSafeDlg.h"

#include "NewGUI/TranslateEx.h"
#include "Util/MemUtil.h"
#include "Util/PrivateConfig.h"

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
	InitCommonControls();

	// SetDialogBkColor(NewGUI_GetBgColor(), CR_FRONT); // Setup the "new" dialog look

	ASSERT(TRUE == 1); ASSERT(FALSE == 0);

	CPwSafeDlg dlg;
	m_pMainWnd = &dlg;

	{
		CPrivateConfig *pc = new CPrivateConfig(FALSE);

		if(pc != NULL)
		{
			dlg.m_bCheckForInstance = pc->GetBool(PWMKEY_SINGLEINSTANCE, FALSE);
			delete pc; pc = NULL;
		}
	}

	if(dlg.m_bCheckForInstance == TRUE)
	{
		dlg.m_instanceChecker.ActivateChecker();

		if(dlg.m_instanceChecker.PreviousInstanceRunning())
		{
			CString strFile;
			LPCTSTR lpPassword = NULL;
			LPCTSTR lpKeyFile = NULL;
			LPCTSTR lpPreSelectPath = NULL;
			DWORD dwData = 0;

			ParseCurrentCommandLine(&strFile, &lpPassword, &lpKeyFile, &lpPreSelectPath);

			if(strFile.GetLength() != 0)
			{
				if(lpPassword != NULL)
				{
					dwData |= (DWORD)_tcslen(lpPassword) << 16;
					strFile = CString(lpPassword) + strFile;
				}

				if(lpKeyFile != NULL)
				{
					dwData |= (DWORD)_tcslen(lpKeyFile);
					strFile = CString(lpKeyFile) + strFile;
				}

				dlg.m_instanceChecker.ActivatePreviousInstance((LPCTSTR)strFile, dwData);
			}
			else
			{
				dlg.m_instanceChecker.ActivatePreviousInstance();
			}

			m_pMainWnd = NULL;
			return FALSE;
		}
	}

	int nResponse = dlg.DoModal();

	if(nResponse == IDOK) { }
	else if(nResponse == IDCANCEL) { }

	return FALSE;
}

int CPwSafeApp::ExitInstance() 
{
	// Release application's mutex object
	if(m_pAppMutex != NULL)
	{
		m_pAppMutex->Unlock();
		delete m_pAppMutex;
		m_pAppMutex = NULL;
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

BOOL CPwSafeApp::GetStartWithWindows()
{
	HKEY h = NULL;
	LONG l;
	TCHAR tszBuf[512];
	DWORD dwSize = 510;

	l = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_QUERY_VALUE, &h);
	if(l != ERROR_SUCCESS) return FALSE;

	DWORD dwType = REG_SZ;
	if(RegQueryValueEx(h, _T("KeePass Password Safe"), NULL, &dwType, (LPBYTE)tszBuf, &dwSize) != ERROR_SUCCESS)
	{
		RegCloseKey(h); h = NULL;
		return FALSE;
	}

	RegCloseKey(h); h = NULL;

	if((_tcslen(tszBuf) > 0) && (tszBuf[0] != _T('-'))) return TRUE;
	return FALSE;
}

BOOL CPwSafeApp::SetStartWithWindows(BOOL bAutoStart)
{
	HKEY h = NULL;
	TCHAR tszBuf[512];
	LONG l;

	l = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_WRITE, &h);
	if(l != ERROR_SUCCESS) return FALSE;

	if(bAutoStart == TRUE)
	{
		GetModuleFileName(NULL, tszBuf, 510);

		DWORD dwSize = (_tcslen(tszBuf) + 1) * sizeof(TCHAR);
		l = RegSetValueEx(h, _T("KeePass Password Safe"), 0, REG_SZ, (LPBYTE)tszBuf, dwSize);
		if(l != ERROR_SUCCESS)
		{
			RegCloseKey(h); h = NULL;
			return FALSE;
		}
	}
	else // bAutoStart == FALSE)
	{
		l = RegDeleteValue(h, _T("KeePass Password Safe"));
		if(l != ERROR_SUCCESS)
		{
			RegCloseKey(h); h = NULL;
			return FALSE;
		}
	}

	RegCloseKey(h); h = NULL;
	return TRUE;
}

BOOL CPwSafeApp::ParseCurrentCommandLine(CString *psFile, LPCTSTR *lpPassword, LPCTSTR *lpKeyFile, LPCTSTR *lpPreSelectPath)
{
	long i;
	BOOL bFirst = TRUE;

	ASSERT(psFile != NULL); if(psFile == NULL) return FALSE;
	psFile->Empty();

	ASSERT(lpPassword != NULL); if(lpPassword == NULL) return FALSE;
	*lpPassword = NULL;
	ASSERT(lpKeyFile != NULL); if(lpKeyFile == NULL) return FALSE;
	*lpKeyFile = NULL;
	ASSERT(lpPreSelectPath != NULL); if(lpPreSelectPath == NULL) return FALSE;
	*lpPreSelectPath = NULL;

	if(__argc <= 1) return FALSE;

	for(i = 1; i < (long)__argc; i++)
	{
		if((_tcsnicmp(__argv[i], _T("-pw:"), 4) == 0) && (_tcslen(__argv[i]) > 4))
			*lpPassword = &__argv[i][4];
		else if((_tcsnicmp(__argv[i], _T("/pw:"), 4) == 0) && (_tcslen(__argv[i]) > 4))
			*lpPassword = &__argv[i][4];
		else if((_tcsnicmp(__argv[i], _T("-keyfile:"), 9) == 0) && (_tcslen(__argv[i]) > 9))
			*lpKeyFile = &__argv[i][9];
		else if((_tcsnicmp(__argv[i], _T("/keyfile:"), 9) == 0) && (_tcslen(__argv[i]) > 9))
			*lpKeyFile = &__argv[i][9];
		else if((_tcsnicmp(__argv[i], _T("-preselect:"), 11) == 0) && (_tcslen(__argv[i]) > 11))
			*lpPreSelectPath = &__argv[i][11];
		else if((_tcsnicmp(__argv[i], _T("/preselect:"), 11) == 0) && (_tcslen(__argv[i]) > 11))
			*lpPreSelectPath = &__argv[i][11];
		else if((_tcsnicmp(__argv[i], _T("-ext:"), 5) == 0) && (_tcslen(__argv[i]) > 5))
		{ // Ignore this parameter
		}
		else if((_tcsnicmp(__argv[i], _T("/ext:"), 5) == 0) && (_tcslen(__argv[i]) > 5))
		{ // Ignore this parameter
		}
		else
		{
			if(bFirst != TRUE) *psFile += _T(" ");
			*psFile += __argv[i];
			bFirst = FALSE;
		}
	}

	psFile->TrimLeft(); psFile->TrimRight();

	if(psFile->GetLength() == 0) return FALSE;
	if(psFile->Left(1) == _T("\"")) *psFile = psFile->Right(psFile->GetLength() - 1);
	if(psFile->GetLength() == 0) return FALSE;
	psFile->TrimLeft(); psFile->TrimRight();
	if(psFile->GetLength() == 0) return FALSE;
	if(psFile->Right(1) == _T("\"")) *psFile = psFile->Left(psFile->GetLength() - 1);
	if(psFile->GetLength() == 0) return FALSE;
	psFile->TrimLeft(); psFile->TrimRight();
	if(psFile->GetLength() == 0) return FALSE;

	return TRUE;
}
