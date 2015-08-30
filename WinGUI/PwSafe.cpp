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

#include "StdAfx.h"
#include "PwSafe.h"
#include "PwSafeDlg.h"

#include "../KeePassLibCpp/Util/TranslateEx.h"
#include "../KeePassLibCpp/Util/AppUtil.h"
#include "../KeePassLibCpp/Util/MemUtil.h"
#include "../KeePassLibCpp/Util/StrUtil.h"
#include "../KeePassLibCpp/Crypto/MemoryProtectionEx.h"
#include "../KeePassLibCpp/Crypto/KeyTransform_BCrypt.h"
#include "Util/WinUtil.h"
#include "Util/CmdLine/CmdArgs.h"
#include "Util/CmdLine/Executable.h"
#include "Plugins/KpApiImpl.h"
#include "Plugins/KpDatabaseImpl.h"
#include "Plugins/KpCommandLineImpl.h"
#include "Plugins/KpUtilitiesImpl.h"
#include "NewGUI/TaskbarListEx/TaskbarListEx.h"
#include "NewGUI/GradientUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static UINT g_uThreadACP = 0;
static BOOL g_bForceSimpleAsterisks = FALSE;
static TCHAR g_pFontNameNormal[12];
static TCHAR g_pFontNameSymbol[8];

static CRITICAL_SECTION g_csLockTimer;

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CPwSafeApp, CWinApp)
	//{{AFX_MSG_MAP(CPwSafeApp)
	//}}AFX_MSG

	// ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CPwSafeApp::CPwSafeApp()
{
	_tcscpy_s(g_pFontNameNormal, _countof(g_pFontNameNormal), _T("MS Serif"));
	_tcscpy_s(g_pFontNameSymbol, _countof(g_pFontNameSymbol), _T("Symbol"));

	m_hGlobalMutex = NULL;
}

/////////////////////////////////////////////////////////////////////////////

CPwSafeApp theApp;

/////////////////////////////////////////////////////////////////////////////

BOOL CPwSafeApp::InitInstance()
{
#if (_MFC_VER < 0x0500)
#ifdef _AFXDLL
	Enable3dControls();
#else
	Enable3dControlsStatic();
#endif
#endif

	AU_EnsureInitialized();

	if(ProcessControlCommands() == TRUE)
	{
		this->_App_CleanUp();
		return FALSE;
	}

	// Create application's mutex object to make our presence public
	m_pAppMutex = new CMutex(FALSE, MTXNAME_LOCAL, NULL);
	ASSERT(m_pAppMutex != NULL);

	m_hGlobalMutex = CPwSafeApp::CreateGlobalMutex();
	ASSERT(m_hGlobalMutex != NULL);

	VERIFY(AfxOleInit());
	AfxEnableControlContainer();

#ifndef _UNICODE
	AfxInitRichEdit();
#else
	AfxInitRichEditEx();
#endif	

	InitCommonControls();

	// SetDialogBkColor(NewGUI_GetBgColor(), CR_FRONT); // Setup the "new" dialog look

	ASSERT(TRUE == 1); ASSERT(FALSE == 0);

	g_uThreadACP = GetACP();

	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
	if((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 0)) // Windows 2000
		g_bForceSimpleAsterisks = TRUE;
	if((osvi.dwMajorVersion <= 4) || ((osvi.dwMajorVersion == 5) &&
		(osvi.dwMinorVersion == 0))) // Restore old banner style if running on < XP
		NewGUI_SetWin32Banner();

	NewGUI_InitGDIPlus();

	CPwSafeDlg dlg;
	m_pMainWnd = &dlg;

	CPrivateConfigEx* pc = new CPrivateConfigEx(FALSE);
	if(pc != NULL)
	{
		pc->LoadStaticConfigFileOverrides();

		dlg.m_bCheckForInstance = pc->GetBool(PWMKEY_SINGLEINSTANCE, FALSE);
		CMemoryProtectionEx::SetEnabledAtStart(pc->GetBool(PWMKEY_USEDPAPIFORMEMPROT, TRUE));
		*CKeyTransformBCrypt::GetEnabledPtr() = pc->GetBool(PWMKEY_USECNGBCRYPTFORKEYT, TRUE);
		delete pc; pc = NULL;
	}
	else { ASSERT(FALSE); }

	if(dlg.m_bCheckForInstance == TRUE)
	{
		dlg.m_instanceChecker.ActivateChecker();

		if(dlg.m_instanceChecker.PreviousInstanceRunning())
		{
			/* CString strFile;
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
			else dlg.m_instanceChecker.ActivatePreviousInstance(_T(""), 0xF0FFFFF0); */

			const FullPathName& database = CmdArgs::instance().getDatabase();
			if(database.getState() == FullPathName::PATH_AND_FILENAME)			
			{
				std_string string(database.getFullPathName());
				DWORD dwData = 0;
				if(!CmdArgs::instance().getPassword().empty())
				{
					dwData |= ((DWORD)CmdArgs::instance().getPassword().length() << 16);
					string = CmdArgs::instance().getPassword() + string;
				}

				const FullPathName& keyfile = CmdArgs::instance().getKeyfile();
				enum {PATH_EXISTS = FullPathName::PATH_ONLY | FullPathName::PATH_AND_FILENAME};
				if(keyfile.getState() & PATH_EXISTS && !CmdArgs::instance().preselectIsInEffect())                
				{
					dwData |= (DWORD)keyfile.getFullPathName().length();
					string = keyfile.getFullPathName() + string;
				}

				dlg.m_instanceChecker.ActivatePreviousInstance(string.c_str(), dwData);
			}
			else dlg.m_instanceChecker.ActivatePreviousInstance(_T(""), 0xF0FFFFF0);

			m_pMainWnd = NULL;

			this->_App_CleanUp();
			return FALSE;
		}
	}

	InitializeCriticalSection(&g_csLockTimer);
	NSCAPI_Initialize(); // Initialize natural string comparison API
	CTaskbarListEx::Initialize();

	dlg.DoModal(); // IDOK, IDCANCEL

	CPluginManager::Instance().UnloadAllPlugins(FALSE);

	KP_ASSERT_REFS(CKpApiImpl::Instance(), 1);
	KP_ASSERT_REFS(CKpDatabaseImpl::Instance(), 1);
	KP_ASSERT_REFS(CKpUtilitiesImpl::Instance(), 1);
	KP_ASSERT_REFS(CKpCommandLineImpl::Instance(), 1);
	dlg._AssertDisplayCounts(0, 0);

	CTaskbarListEx::Release(false);
	NSCAPI_Exit(); // Clean up natural string comparison API
	DeleteCriticalSection(&g_csLockTimer);

	this->_App_CleanUp();
	return FALSE;
}

void CPwSafeApp::_App_CleanUp()
{
	NewGUI_TerminateGDIPlus();
	NewGUI_CleanUp();
	CGradientUtil::Release();
	CKpCommandLineImpl::ClearStatic();
	CMemoryProtectionEx::Release();
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

	if(m_hGlobalMutex != NULL)
	{
		VERIFY(::CloseHandle(m_hGlobalMutex));
		m_hGlobalMutex = NULL;
	}

	NewGUI_CleanUp();
	return CWinApp::ExitInstance();
}

void CPwSafeApp::ChangeKdbShellAssociation(BOOL bRegister, HWND hParent)
{
	CPwSafeApp::LoadTranslationEx(NULL);

	if(bRegister == TRUE)
	{
		if(CPwSafeApp::RegisterShellAssociation() == TRUE)
		{
			CPwSafeApp::NotifyAssocChanged();

			::MessageBox(hParent, TRL("Successfully associated KeePass with .kdb files! A double-click on a .kdb file will now start KeePass automatically."),
				PWM_PRODUCT_NAME_SHORT, MB_OK | MB_ICONINFORMATION);
		}
		else
		{
			::MessageBox(hParent, TRL("Failed to change the .kdb file association. Make sure you have the rights to write to the registry and change file associations."),
				PWM_PRODUCT_NAME_SHORT, MB_OK | MB_ICONWARNING);
		}
	}
	else // Unregister
	{
		if(CPwSafeApp::UnregisterShellAssociation() == TRUE)
		{
			CPwSafeApp::NotifyAssocChanged();

			::MessageBox(hParent, TRL("Successfully removed association! KeePass won't be started anymore when double-clicking on a .kdb file."),
				PWM_PRODUCT_NAME_SHORT, MB_OK | MB_ICONINFORMATION);
		}
		else
		{
			::MessageBox(hParent, TRL("Failed to change the .kdb file association. Make sure you have the rights to write to the registry and change file associations."),
				PWM_PRODUCT_NAME_SHORT, MB_OK | MB_ICONWARNING);
		}
	}
}

BOOL CPwSafeApp::RegisterShellAssociation()
{
	HKEY hBase, hShell, hTemp, hTemp2;
	// TCHAR tszTemp[MAX_PATH * 2];
	// TCHAR tszMe[MAX_PATH * 2];

	// VERIFY(GetModuleFileName(NULL, tszMe, MAX_PATH * 2 - 2) != 0);
	std_string strMe = Executable::instance().getFullPathName();

	// HKEY_CLASSES_ROOT/.kdb

	LONG l = RegCreateKey(HKEY_CLASSES_ROOT, _T(".kdb"), &hBase);
	if(l != ERROR_SUCCESS) return FALSE;

	std_string strTemp = _T("kdbfile");
	DWORD dw = static_cast<DWORD>((strTemp.length() + 1) * sizeof(TCHAR));
	l = RegSetValueEx(hBase, _T(""), 0, REG_SZ, (CONST BYTE *)strTemp.c_str(), dw);
	ASSERT(l == ERROR_SUCCESS); if(l != ERROR_SUCCESS) { RegCloseKey(hBase); return FALSE; }

	RegCloseKey(hBase);

	// HKEY_CLASSES_ROOT/kdbfile

	l = RegCreateKey(HKEY_CLASSES_ROOT, _T("kdbfile"), &hBase);
	ASSERT(l == ERROR_SUCCESS); if(l != ERROR_SUCCESS) return FALSE;

	// _tcscpy_s(tszTemp, _countof(tszTemp), TRL("KeePass Password Database"));
	strTemp = TRL("KeePass Password Database");

	dw = static_cast<DWORD>((strTemp.length() + 1) * sizeof(TCHAR));
	l = RegSetValueEx(hBase, _T(""), 0, REG_SZ, (CONST BYTE *)strTemp.c_str(), dw);
	ASSERT(l == ERROR_SUCCESS); if(l != ERROR_SUCCESS) { RegCloseKey(hBase); return FALSE; }

	// _tcscpy_s(tszTemp, _countof(tszTemp), _T(""));
	strTemp = _T("");

	dw = static_cast<DWORD>((strTemp.length() + 1) * sizeof(TCHAR));
	l = RegSetValueEx(hBase, _T("AlwaysShowExt"), 0, REG_SZ, (CONST BYTE *)strTemp.c_str(), dw);
	ASSERT(l == ERROR_SUCCESS); if(l != ERROR_SUCCESS) { RegCloseKey(hBase); return FALSE; }

	l = RegCreateKey(hBase, _T("DefaultIcon"), &hTemp);
	ASSERT(l == ERROR_SUCCESS); if(l != ERROR_SUCCESS) return FALSE;

	// _tcscpy_s(tszTemp, _countof(tszTemp), tszMe);
	strTemp = strMe;
	// _tcscat_s(tszTemp, _countof(tszTemp), _T(",0"));
	strTemp += _T(",0");
	dw = static_cast<DWORD>((strTemp.length() + 1) * sizeof(TCHAR));
	l = RegSetValueEx(hTemp, _T(""), 0, REG_SZ, (CONST BYTE *)strTemp.c_str(), dw);
	ASSERT(l == ERROR_SUCCESS); if(l != ERROR_SUCCESS) { RegCloseKey(hTemp); RegCloseKey(hBase); return FALSE; }

	RegCloseKey(hTemp);

	// HKEY_CLASSES_ROOT/kdbfile/shell

	l = RegCreateKey(hBase, _T("shell"), &hShell);
	ASSERT(l == ERROR_SUCCESS); if(l != ERROR_SUCCESS) return FALSE;

	// HKEY_CLASSES_ROOT/kdbfile/shell/open

	l = RegCreateKey(hShell, _T("open"), &hTemp);

	// _tcscpy_s(tszTemp, _countof(tszTemp), TRL("&Open with KeePass"));
	strTemp = TRL("&Open with KeePass");
	dw = static_cast<DWORD>((strTemp.length() + 1) * sizeof(TCHAR));
	l = RegSetValueEx(hTemp, _T(""), 0, REG_SZ, (CONST BYTE *)strTemp.c_str(), dw);
	ASSERT(l == ERROR_SUCCESS); if(l != ERROR_SUCCESS) { RegCloseKey(hTemp); RegCloseKey(hShell); RegCloseKey(hBase); return FALSE; }

	l = RegCreateKey(hTemp, _T("command"), &hTemp2);
	ASSERT(l == ERROR_SUCCESS); if(l != ERROR_SUCCESS) return FALSE;

	// _tcscpy_s(tszTemp, _countof(tszTemp), _T("\""));
	strTemp = _T("\"");
	// _tcscat_s(tszTemp, _countof(tszTemp), tszMe);
	strTemp += strMe;
	// _tcscat_s(tszTemp, _countof(tszTemp), _T("\" \"%1\""));
	strTemp += _T("\" \"%1\"");
	dw = static_cast<DWORD>((strTemp.length() + 1) * sizeof(TCHAR));
	l = RegSetValueEx(hTemp2, _T(""), 0, REG_SZ, (CONST BYTE *)strTemp.c_str(), dw);
	ASSERT(l == ERROR_SUCCESS); if(l != ERROR_SUCCESS) { RegCloseKey(hTemp); RegCloseKey(hShell); RegCloseKey(hBase); return FALSE; }

	VERIFY(RegCloseKey(hTemp2) == ERROR_SUCCESS);
	VERIFY(RegCloseKey(hTemp) == ERROR_SUCCESS);

	VERIFY(RegCloseKey(hShell) == ERROR_SUCCESS);

	VERIFY(RegCloseKey(hBase) == ERROR_SUCCESS);

	return TRUE;
}

BOOL CPwSafeApp::UnregisterShellAssociation()
{
	HKEY hBase, hShell, hOpen, hCommand, hDefaultIcon;
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

	l = RegOpenKeyEx(hBase, _T("DefaultIcon"), 0, KEY_WRITE, &hDefaultIcon);
	if(l != ERROR_SUCCESS) return FALSE;

	RegDeleteValue(hCommand, _T(""));
	VERIFY(RegCloseKey(hCommand) == ERROR_SUCCESS);
	VERIFY(RegDeleteKey(hOpen, _T("command")) == ERROR_SUCCESS);

	RegDeleteValue(hOpen, _T(""));
	VERIFY(RegCloseKey(hOpen) == ERROR_SUCCESS);
	VERIFY(RegDeleteKey(hShell, _T("open")) == ERROR_SUCCESS);

	RegDeleteValue(hShell, _T(""));
	VERIFY(RegCloseKey(hShell) == ERROR_SUCCESS);
	VERIFY(RegDeleteKey(hBase, _T("shell")) == ERROR_SUCCESS);

	RegDeleteValue(hDefaultIcon, _T(""));
	VERIFY(RegCloseKey(hDefaultIcon) == ERROR_SUCCESS);
	VERIFY(RegDeleteKey(hBase, _T("DefaultIcon")) == ERROR_SUCCESS);

	RegDeleteValue(hBase, _T(""));
	RegDeleteValue(hBase, _T("AlwaysShowExt"));
	VERIFY(RegCloseKey(hBase) == ERROR_SUCCESS);
	VERIFY(RegDeleteKey(HKEY_CLASSES_ROOT, _T("kdbfile")) == ERROR_SUCCESS);

	return TRUE;
}

void CPwSafeApp::NotifyAssocChanged()
{
	HINSTANCE hShell32 = AU_LoadLibrary(_T("Shell32.dll"));
	if(hShell32 != NULL)
	{
		LPSHCHANGENOTIFY lpSHChangeNotify = (LPSHCHANGENOTIFY)GetProcAddress(
			hShell32, "SHChangeNotify");

		if(lpSHChangeNotify != NULL)
			lpSHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
		else { ASSERT(FALSE); }

		FreeLibrary(hShell32);
	}
	else { ASSERT(FALSE); }
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
	// TCHAR tszBuf[512];
	LONG l;

	l = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_WRITE, &h);
	if(l != ERROR_SUCCESS) return FALSE;

	if(bAutoStart == TRUE)
	{
		// GetModuleFileName(NULL, tszBuf, 510);
		std_string strPath = Executable::instance().getFullPathName();

		DWORD uPathLen = static_cast<DWORD>(strPath.length());
		if(uPathLen > 0)
		{
			if(strPath[0] != _T('\"'))
			{
				strPath = std_string(_T("\"")) + strPath + std_string(_T("\""));
				uPathLen = static_cast<DWORD>(strPath.length()); // Update length
			}
		}

		DWORD dwSize = (uPathLen + 1) * sizeof(TCHAR);
		l = RegSetValueEx(h, _T("KeePass Password Safe"), 0, REG_SZ, (LPBYTE)strPath.c_str(), dwSize);
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

/* BOOL CPwSafeApp::ParseCurrentCommandLine(CString *psFile, LPCTSTR *lpPassword, LPCTSTR *lpKeyFile, LPCTSTR *lpPreSelectPath)
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
		if((_tcsnicmp(__targv[i], _T("-pw:"), 4) == 0) && (_tcslen(__targv[i]) > 4))
			*lpPassword = &__targv[i][4];
		else if((_tcsnicmp(__targv[i], _T("/pw:"), 4) == 0) && (_tcslen(__targv[i]) > 4))
			*lpPassword = &__targv[i][4];
		else if((_tcsnicmp(__targv[i], _T("-keyfile:"), 9) == 0) && (_tcslen(__targv[i]) > 9))
			*lpKeyFile = &__targv[i][9];
		else if((_tcsnicmp(__targv[i], _T("/keyfile:"), 9) == 0) && (_tcslen(__targv[i]) > 9))
			*lpKeyFile = &__targv[i][9];
		else if((_tcsnicmp(__targv[i], _T("-preselect:"), 11) == 0) && (_tcslen(__targv[i]) > 11))
			*lpPreSelectPath = &__targv[i][11];
		else if((_tcsnicmp(__targv[i], _T("/preselect:"), 11) == 0) && (_tcslen(__targv[i]) > 11))
			*lpPreSelectPath = &__targv[i][11];
		else if((_tcsnicmp(__targv[i], _T("-ext:"), 5) == 0) && (_tcslen(__targv[i]) > 5))
		{ // Ignore this parameter
		}
		else if((_tcsnicmp(__targv[i], _T("/ext:"), 5) == 0) && (_tcslen(__targv[i]) > 5))
		{ // Ignore this parameter
		}
		else
		{
			if(bFirst != TRUE) *psFile += _T(" ");
			*psFile += __targv[i];
			bFirst = FALSE;
		}
	}

	psFile->Trim();

	if(psFile->GetLength() == 0) return FALSE;
	if(psFile->Left(1) == _T("\"")) *psFile = psFile->Right(psFile->GetLength() - 1);
	if(psFile->GetLength() == 0) return FALSE;
	psFile->Trim();
	if(psFile->GetLength() == 0) return FALSE;
	if(psFile->Right(1) == _T("\"")) *psFile = psFile->Left(psFile->GetLength() - 1);
	if(psFile->GetLength() == 0) return FALSE;
	psFile->Trim();
	if(psFile->GetLength() == 0) return FALSE;

	return TRUE;
} */

void CPwSafeApp::CreateHiColorImageList(CImageList *pImageList, WORD wResourceID, int czSize)
{
	ASSERT(pImageList != NULL); if(pImageList == NULL) return;

	CBitmap bmpImages;
	VERIFY(bmpImages.LoadBitmap(MAKEINTRESOURCE(wResourceID)));

	VERIFY(pImageList->Create(czSize, czSize, ILC_COLOR24 | ILC_MASK, 0, 0));
	VERIFY(pImageList->Add(&bmpImages, RGB(255, 0, 255)) >= 0);

	VERIFY(bmpImages.DeleteObject());
}

BOOL CPwSafeApp::IsMBThreadACP()
{
	if((g_uThreadACP == 932) || (g_uThreadACP == 936) || (g_uThreadACP == 950)) return TRUE;
	return FALSE;
}

#pragma warning(push)
#pragma warning(disable: 4310) // Type cast shortens constant value

TCHAR CPwSafeApp::GetPasswordCharacter()
{
	if((IsMBThreadACP() == TRUE) || (g_bForceSimpleAsterisks == TRUE))
		return _T('*');
	return (TCHAR)0xB7;
}

#pragma warning(pop)

LPCTSTR CPwSafeApp::GetPasswordFont()
{
	if((IsMBThreadACP() == TRUE) || (g_bForceSimpleAsterisks == TRUE))
		return g_pFontNameNormal;
	return (LPCTSTR)g_pFontNameSymbol;
}

BOOL CPwSafeApp::ProcessControlCommands()
{
	LPCTSTR lpTrimChars = _T("\"' \t\r\n\\$%");

	CString strCmdLine = (LPCTSTR)GetCommandLine();
	strCmdLine = strCmdLine.Trim(lpTrimChars);
	strCmdLine = strCmdLine.MakeLower();

	if((strCmdLine.Right(9) == _T("-exit-all")) ||
		(strCmdLine.Right(9) == _T("/exit-all")))
	{
		::PostMessage(HWND_BROADCAST, CPwSafeDlg::GetKeePassControlMessageID(),
			KPCM_EXIT, 0);
		return TRUE;
	}

	if((strCmdLine.Right(10) == _T("-auto-type")) ||
		(strCmdLine.Right(10) == _T("/auto-type")))
	{
		::PostMessage(HWND_BROADCAST, CPwSafeDlg::GetKeePassControlMessageID(),
			KPCM_AUTOTYPE, 0);
		return TRUE;
	}

	if((strCmdLine.Right(9) == _T("-lock-all")) ||
		(strCmdLine.Right(9) == _T("/lock-all")))
	{
		::PostMessage(HWND_BROADCAST, CPwSafeDlg::GetKeePassControlMessageID(),
			KPCM_LOCK, 0);
		return TRUE;
	}

	if((strCmdLine.Right(11) == _T("-unlock-all")) ||
		(strCmdLine.Right(11) == _T("/unlock-all")))
	{
		::PostMessage(HWND_BROADCAST, CPwSafeDlg::GetKeePassControlMessageID(),
			KPCM_UNLOCK, 0);
		return TRUE;
	}

	if((strCmdLine.Right(18).CompareNoCase(KPCLOPT_FILEEXT_UNREG) == 0) ||
		(strCmdLine.Right(18).CompareNoCase(KPCLOPT_FILEEXT_UNREG_ALT) == 0))
	{
		ChangeKdbShellAssociation(FALSE, NULL);
		return TRUE;
	}

	if((strCmdLine.Right(16).CompareNoCase(KPCLOPT_FILEEXT_REG) == 0) ||
		(strCmdLine.Right(16).CompareNoCase(KPCLOPT_FILEEXT_REG_ALT) == 0))
	{
		ChangeKdbShellAssociation(TRUE, NULL);
		return TRUE;
	}

	if((strCmdLine.Right(18) == _T("-clear-urloverride")) ||
		(strCmdLine.Right(18) == _T("/clear-urloverride")))
	{
		CPrivateConfigEx cfg(TRUE);
		cfg.Set(PWMKEY_URLOVERRIDE, _T(""));
		return TRUE;
	}

	if((strCmdLine.Right(16) == _T("-get-urloverride")) ||
		(strCmdLine.Right(16) == _T("/get-urloverride")))
	{
		CPrivateConfigEx cfg(FALSE);
		TCHAR tszUrlOverride[SI_REGSIZE]; tszUrlOverride[0] = 0;
		cfg.Get(PWMKEY_URLOVERRIDE, tszUrlOverride);

		std::basic_string<TCHAR> strOutFile = WU_GetTempDirectory();
		strOutFile += _T("KeePass_UrlOverride.tmp");

		std::basic_string<TCHAR> strContent = _T("[KeePass]\r\n");
		strContent += PWMKEY_URLOVERRIDE;
		strContent += _T("=");
		strContent += tszUrlOverride;
		strContent += _T("\r\n");
		VERIFY(AU_WriteBigFile(strOutFile.c_str(), (const BYTE*)strContent.c_str(),
			static_cast<DWORD>(strContent.size() * sizeof(TCHAR)), FALSE) == PWE_SUCCESS);
		return TRUE;
	}

	// Pre-check whether the option is present or not; if the option is not present,
	// the CmdArgs instance must not be created at this point of time (plugins
	// haven't added option prefixes yet, so creating the CmdArgs instance leads
	// to unknown option errors here)
	int nSetUrl = strCmdLine.Find(_T("-set-urloverride:"));
	if(nSetUrl < 0) nSetUrl = strCmdLine.Find(_T("-set-urloverride="));
	if(nSetUrl < 0) nSetUrl = strCmdLine.Find(_T("/set-urloverride:"));
	if(nSetUrl < 0) nSetUrl = strCmdLine.Find(_T("/set-urloverride="));
	if(nSetUrl >= 0)
	{
		const std_string strUrlOverride = CmdArgs::instance().getUrlOverride();

		CPrivateConfigEx cfg(TRUE);
		cfg.Set(PWMKEY_URLOVERRIDE, strUrlOverride.c_str());
		return TRUE;
	}

	return FALSE;
}

HANDLE CPwSafeApp::CreateGlobalMutex()
{
	std::basic_string<TCHAR> strName = _T("Global\\");
	strName += MTXNAME_GLOBAL;

	HMODULE hInst = AU_LoadLibrary(_T("AdvApi32.dll"));
	if(hInst == NULL) { ASSERT(FALSE); return NULL; }

	LPINITIALIZESECURITYDESCRIPTOR lpInit = (LPINITIALIZESECURITYDESCRIPTOR)
		::GetProcAddress(hInst, "InitializeSecurityDescriptor");
	LPSETSECURITYDESCRIPTORDACL lpSet = (LPSETSECURITYDESCRIPTORDACL)
		::GetProcAddress(hInst, "SetSecurityDescriptorDacl");
	if((lpInit == NULL) || (lpSet == NULL))
	{
		::FreeLibrary(hInst);
		return NULL;
	}

	SECURITY_DESCRIPTOR sd;
	ZeroMemory(&sd, sizeof(SECURITY_DESCRIPTOR));
	VERIFY(lpInit(&sd, SECURITY_DESCRIPTOR_REVISION));
	VERIFY(lpSet(&sd, TRUE, NULL, FALSE));

	SECURITY_ATTRIBUTES sa;
	ZeroMemory(&sa, sizeof(SECURITY_ATTRIBUTES));
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = &sd;
	sa.bInheritHandle = FALSE;

	HANDLE hMutex = ::CreateMutex(&sa, FALSE, strName.c_str());

	::FreeLibrary(hInst);
	return hMutex;
}

void CPwSafeApp::LoadTranslationEx(CPrivateConfigEx* pConfig)
{
	const bool bAlloc = (pConfig == NULL);
	CPrivateConfigEx* pCfg = (bAlloc ? new CPrivateConfigEx(FALSE) : pConfig);

	TCHAR szTemp[SI_REGSIZE];
	pCfg->Get(PWMKEY_LANG, szTemp);
	VERIFY(LoadTranslationTable(szTemp));

	if(bAlloc) { delete pCfg; pCfg = NULL; }
}

LPCRITICAL_SECTION CPwSafeApp::GetLockTimerCS()
{
	return &g_csLockTimer;
}
