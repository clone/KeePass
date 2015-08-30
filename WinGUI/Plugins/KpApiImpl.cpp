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

#include "StdAfx.h"
#include <tchar.h>
#include "KpApiImpl.h"
#include "KpDatabaseImpl.h"
#include "KpUtilitiesImpl.h"
#include "KpCommandLineImpl.h"
#include "KpFullPathNameImpl.h"
#include "KpCommandLineOptionImpl.h"
#include "../../KeePassLibCpp/Util/TranslateEx.h"
#include "../../KeePassLibCpp/Util/StrUtil.h"
#include "../../KeePassLibCpp/Util/MemUtil.h"

#include <boost/static_assert.hpp>

#include "../../KeePassLibCpp/Util/TranslateEx.h"

#include "../Resource.h"
#include "../PwSafeDlg.h"
#include "../Util/WinUtil.h"

static CPwSafeDlg* g_pMainDlg = NULL;
static LPCTSTR g_lpVersionStr = PWM_VERSION_STR;

KP_IMPL_STDREFIMPL_NODELETE(CKpApiImpl)
KP_IMPL_STDREFIMPL_SUB(CKpConfigImpl, m_pConfig)

CKpApiImpl::CKpApiImpl()
{
	KP_IMPL_CONSTRUCT;
}

CKpApiImpl& CKpApiImpl::Instance()
{
	static CKpApiImpl singletonObject;
	return singletonObject;
}

STDMETHODIMP CKpApiImpl::QueryInterface(REFIID riid, void** ppvObject)
{
	KP_REQ_OUT_PTR(ppvObject);

	KP_SUPPORT_INTERFACE(IID_IKpUnknown, IKpUnknown);
	KP_SUPPORT_INTERFACE(IID_IKpAPI, IKpAPI);
	KP_SUPPORT_INTERFACE(IID_IKpAPI2, IKpAPI2);

	*ppvObject = NULL;
	return E_NOINTERFACE;
}

STDMETHODIMP CKpApiImpl::CreateInstance(REFCLSID rclsid, REFIID riid, void** ppvObject)
{
	KP_REQ_OUT_PTR_INIT(ppvObject);

	if((rclsid == CLSID_KpConfig) || (rclsid == CLSID_KpConfig_ReadOnly))
	{
		CKpConfigImpl* pConfig = new CKpConfigImpl((rclsid == CLSID_KpConfig) ? TRUE : FALSE);

		void* pInterface = NULL;
		if(pConfig->QueryInterface(riid, &pInterface) == S_OK)
		{
			*ppvObject = pInterface;
			VERIFY(pConfig->Release() == 1);
			return S_OK;
		}
		else { VERIFY(pConfig->Release() == 0); return E_NOINTERFACE; }
	}

	return REGDB_E_CLASSNOTREG;
}

STDMETHODIMP CKpApiImpl::CreateInstanceEx(REFCLSID rclsid, REFIID riid, void** ppvObject,
	LPARAM lParamW, LPARAM lParamL)
{
	KP_REQ_OUT_PTR_INIT(ppvObject);

	if(rclsid == CLSID_KpFullPathName)
	{
		if(lParamW != 0) return E_FAIL;

		CKpFullPathNameImpl* pFpn = new CKpFullPathNameImpl((LPCTSTR)lParamL);
		KP_QUERY_INTERFACE_RELEASE_LOCAL_RET(pFpn, riid, ppvObject);
	}
	if(rclsid == CLSID_KpCommandLineOption)
	{
		if(lParamW != 0) return E_FAIL;

		CKpCommandLineOptionImpl* pOpt = new CKpCommandLineOptionImpl((LPCTSTR)lParamL);
		KP_QUERY_INTERFACE_RELEASE_LOCAL_RET(pOpt, riid, ppvObject);
	}

	return this->CreateInstance(rclsid, riid, ppvObject);
}

STDMETHODIMP CKpApiImpl::QueryInstance(REFCLSID rclsid, REFIID riid, void** ppvObject)
{
	KP_REQ_OUT_PTR_INIT(ppvObject);

	if(rclsid == SCLSID_KpDatabase)
		return CKpDatabaseImpl::Instance().QueryInterface(riid, ppvObject);
	if(rclsid == SCLSID_KpUtilities)
		return CKpUtilitiesImpl::Instance().QueryInterface(riid, ppvObject);
	if(rclsid == SCLSID_KpCommandLine)
		return CKpCommandLineImpl::Instance().QueryInterface(riid, ppvObject);

	return REGDB_E_CLASSNOTREG;
}

STDMETHODIMP CKpApiImpl::DeleteObject(void* pObject)
{
	SAFE_DELETE(pObject);
	return S_OK;
}

STDMETHODIMP CKpApiImpl::DeleteArray(void* pArray)
{
	SAFE_DELETE_ARRAY(pArray);
	return S_OK;
}

STDMETHODIMP_(DWORD) CKpApiImpl::GetVersion()
{
	return PWM_VERSION_DW;
}

STDMETHODIMP_(LPCTSTR) CKpApiImpl::GetVersionStr()
{
	return g_lpVersionStr;
}

STDMETHODIMP_(DWORD) CKpApiImpl::GetCharSize()
{
	return static_cast<DWORD>(sizeof(TCHAR));
}

STDMETHODIMP_(DWORD) CKpApiImpl::GetPointerSize()
{
	return static_cast<DWORD>(sizeof(void*));
}

STDMETHODIMP_(DWORD) CKpApiImpl::GetDebugLevel()
{
#ifdef _DEBUG
	return 1;
#else
	return 0;
#endif
}

STDMETHODIMP_(HWND) CKpApiImpl::GetMainWindowHandle()
{
	CWnd* pWnd = AfxGetMainWnd();
	return ((pWnd != NULL) ? pWnd->m_hWnd : NULL);
}

STDMETHODIMP_(HIMAGELIST) CKpApiImpl::GetClientIconsImageList()
{
	return g_pMainDlg->m_ilIcons.GetSafeHandle();
}

STDMETHODIMP CKpApiImpl::AddImportMenuItem(LPCTSTR lpText, UINT uId, INT nIconId)
{
	if(nIconId > 0) --nIconId;

	BCMenu *p = NewGUI_GetBCMenu(g_pMainDlg->m_menu.GetSubMenu((TCHAR *)TRL("&File")));
	if(p != NULL)
	{
		p = p->GetSubBCMenu((TCHAR *)TRL("&Import From"));
		if(p != NULL)
		{
			if(lpText == NULL) p->AppendMenu(MF_SEPARATOR);
			else p->AppendODMenu(const_cast<LPTSTR>(lpText), MF_STRING |
				MF_OWNERDRAW, uId, &g_pMainDlg->m_ilIcons, nIconId);
		}
		else { ASSERT(FALSE); return E_UNEXPECTED; }
	}
	else { ASSERT(FALSE); return E_UNEXPECTED; }

	return S_OK;
}

STDMETHODIMP_(BOOL) CKpApiImpl::IsFileModified()
{
	return g_pMainDlg->m_bModified;
}

STDMETHODIMP CKpApiImpl::SetFileModified(BOOL bModified)
{
	g_pMainDlg->m_bModified = ((bModified == FALSE) ? FALSE : TRUE);
	return S_OK;
}

STDMETHODIMP_(BOOL) CKpApiImpl::IsDisplayingDialog()
{
	return g_pMainDlg->m_bDisplayDialog;
}

STDMETHODIMP CKpApiImpl::SetDisplayDialog(BOOL bDialogDisplayed)
{
	g_pMainDlg->m_bDisplayDialog = ((bDialogDisplayed == FALSE) ? FALSE : TRUE);
	return S_OK;
}

STDMETHODIMP CKpApiImpl::EntryListEnsureVisible(INT nItem, BOOL bPartialOK)
{
	g_pMainDlg->m_cList.EnsureVisible(nItem, ((bPartialOK == FALSE) ? FALSE : TRUE));
	return S_OK;
}

STDMETHODIMP CKpApiImpl::UpdateToolBar()
{
	g_pMainDlg->_UpdateToolBar();
	return S_OK;
}

STDMETHODIMP CKpApiImpl::UpdateGroupTree()
{
	g_pMainDlg->UpdateGroupList();
	return S_OK;
}

STDMETHODIMP CKpApiImpl::UpdateEntryList()
{
	g_pMainDlg->UpdatePasswordList();
	return S_OK;
}

STDMETHODIMP CKpApiImpl::SetStatusBarText(LPCTSTR lpText)
{
	if(g_pMainDlg == NULL) return E_FAIL;
	g_pMainDlg->SetStatusTextEx(lpText);
	return S_OK;
}

STDMETHODIMP CKpApiImpl::ShowDialog(DWORD dwDialogId, DWORD dwVersion,
	void* pConfig, INT* pnResult, void* pOutput)
{
	UNREFERENCED_PARAMETER(dwDialogId);
	UNREFERENCED_PARAMETER(dwVersion);
	UNREFERENCED_PARAMETER(pConfig);
	UNREFERENCED_PARAMETER(pnResult);
	UNREFERENCED_PARAMETER(pOutput);
	return E_NOTIMPL;
}

STDMETHODIMP CKpApiImpl::UpdateUI()
{
	g_pMainDlg->_Groups_SaveView(TRUE);
	g_pMainDlg->_List_SaveView();
	g_pMainDlg->UpdateGroupList();
	g_pMainDlg->_Groups_RestoreView();
	g_pMainDlg->UpdatePasswordList();
	g_pMainDlg->_List_RestoreView();
	return S_OK;
}

STDMETHODIMP CKpApiImpl::AutoType(PW_ENTRY* pEntry, LONG_PTR lFlags)
{
	const DWORD dwAutoTypeSeq = static_cast<DWORD>(lFlags & 0xFFFF);
	const BOOL bLoseFocus = (((lFlags & KPF_AUTOTYPE_LOSEFOCUS) != 0) ?
		TRUE : FALSE);

	g_pMainDlg->_AutoType(pEntry, bLoseFocus, dwAutoTypeSeq, NULL);
	return S_OK;
}

STDMETHODIMP_(BOOL) CKpApiImpl::IsFileOpen()
{
	return g_pMainDlg->m_bFileOpen;
}

STDMETHODIMP_(DWORD) CKpApiImpl::GetEntryListItemCount()
{
	return static_cast<DWORD>(g_pMainDlg->m_cList.GetItemCount());
}

STDMETHODIMP_(DWORD) CKpApiImpl::GetSelectedGroupId()
{
	return g_pMainDlg->GetSelectedGroupId();
}

STDMETHODIMP CKpApiImpl::GetNeverExpireTime(PW_TIME* pBuf)
{
	if(pBuf == NULL) return E_POINTER;
	CPwManager::GetNeverExpireTime(pBuf);
	return S_OK;
}

STDMETHODIMP CKpApiImpl::GetCurrentPwTime(PW_TIME* pBuf)
{
	if(pBuf == NULL) return E_POINTER;
	_GetCurrentPwTime(pBuf);
	return S_OK;
}

STDMETHODIMP_(LPCTSTR) CKpApiImpl::GetAbsoluteDatabasePath()
{
	return (LPCTSTR)g_pMainDlg->m_strFileAbsolute;
}

STDMETHODIMP_(LPCTSTR) CKpApiImpl::GetTranslationName()
{
	return GetCurrentTranslationTable();
}

/* STDMETHODIMP CKpApiImpl::LoadTranslation(LPCTSTR lpTableName)
{
	if(lpTableName == NULL) return E_POINTER;
	return ((LoadTranslationTable(lpTableName) == FALSE) ? E_FAIL : S_OK);
}

STDMETHODIMP CKpApiImpl::FreeCurrentTranslation()
{
	return ((FreeCurrentTranslationTable() == FALSE) ? E_FAIL : S_OK);
}

STDMETHODIMP CKpApiImpl::UpdateTranslationTable()
{
	_SortTrlTable();
	return S_OK;
} */

STDMETHODIMP_(LPCTSTR) CKpApiImpl::Translate(LPCTSTR lpDefString)
{
	return _TRL(lpDefString);
}

STDMETHODIMP_(BOOL) CKpApiImpl::IsInitialCommandLineFile()
{
	return g_pMainDlg->m_bInitialCmdLineFile;
}

STDMETHODIMP CKpApiImpl::GetProperty(DWORD dwID, void* pOutValue)
{
	UNREFERENCED_PARAMETER(dwID);
	UNREFERENCED_PARAMETER(pOutValue);

	return E_NOTIMPL;
}

STDMETHODIMP CKpApiImpl::SetProperty(DWORD dwID, void* pNewValue)
{
	UNREFERENCED_PARAMETER(dwID);
	UNREFERENCED_PARAMETER(pNewValue);

	return E_NOTIMPL;
}

// ////////////////////////////////////////////////////////////////////////

CKpConfigImpl::CKpConfigImpl(BOOL bRequireWriteAccess)
{
	KP_IMPL_CONSTRUCT;

	m_pConfig = new CPrivateConfigEx(bRequireWriteAccess);
}

STDMETHODIMP CKpConfigImpl::QueryInterface(REFIID riid, void** ppvObject)
{
	KP_REQ_OUT_PTR(ppvObject);

	KP_SUPPORT_INTERFACE(IID_IKpUnknown, IKpUnknown);
	KP_SUPPORT_INTERFACE(IID_IKpConfig, IKpConfig);

	*ppvObject = NULL;
	return E_NOINTERFACE;
}

STDMETHODIMP_(BOOL) CKpConfigImpl::Get(LPCTSTR pszField, LPTSTR pszValue)
{
	return m_pConfig->Get(pszField, pszValue);
}

STDMETHODIMP_(BOOL) CKpConfigImpl::Set(LPCTSTR pszField, LPCTSTR pszValue)
{
	return m_pConfig->Set(pszField, pszValue);
}

STDMETHODIMP_(BOOL) CKpConfigImpl::GetBool(LPCTSTR pszField, BOOL bDefault)
{
	return m_pConfig->GetBool(pszField, bDefault);
}

STDMETHODIMP_(BOOL) CKpConfigImpl::SetBool(LPCTSTR pszField, BOOL bValue)
{
	return m_pConfig->SetBool(pszField, bValue);
}

STDMETHODIMP CKpConfigImpl::GetUserPath(LPTSTR lpBuf, DWORD dwBufMaxChars)
{
	const std::basic_string<TCHAR> str = m_pConfig->GetUserPath();
	return ((_tcscpy_s(lpBuf, dwBufMaxChars, str.c_str()) == 0) ? S_OK : E_OUTOFMEMORY);
}

void KPMI_SetMainDialog(void* pMainPwSafeDlg)
{
	g_pMainDlg = (CPwSafeDlg*)pMainPwSafeDlg;

	KPImplInternal_SetDatabase(&g_pMainDlg->m_mgr);
}
