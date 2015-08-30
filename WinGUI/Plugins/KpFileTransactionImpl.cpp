/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2013 Dominik Reichl <dominik.reichl@t-online.de>

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
#include <assert.h>
#include "KpFileTransactionImpl.h"

KP_IMPL_STDREFIMPL_SUB(CKpFileTransactionImpl, m_pTx)

CKpFileTransactionImpl::CKpFileTransactionImpl(LPCTSTR lpBaseFile)
{
	KP_IMPL_CONSTRUCT;

	m_pTx = new CFileTransactionEx(lpBaseFile, true);
}

STDMETHODIMP CKpFileTransactionImpl::QueryInterface(REFIID riid, void** ppvObject)
{
	KP_REQ_OUT_PTR(ppvObject);

	KP_SUPPORT_INTERFACE(IID_IKpUnknown, IKpUnknown);
	KP_SUPPORT_INTERFACE(IID_IKpFileTransaction, IKpFileTransaction);

	*ppvObject = NULL;
	return E_NOINTERFACE;
}

#pragma warning(push)
#pragma warning(disable: 4996) // _tcscpy deprecated

STDMETHODIMP_(BOOL) CKpFileTransactionImpl::OpenWrite(LPTSTR lpOutBufferFilePath)
{
	std_string strOut;
	const BOOL bResult = (m_pTx->OpenWrite(strOut) ? TRUE : FALSE);

	if(lpOutBufferFilePath != NULL)
		_tcscpy(lpOutBufferFilePath, strOut.c_str());

	return bResult;
}

#pragma warning(pop) // _tcscpy deprecated

STDMETHODIMP_(BOOL) CKpFileTransactionImpl::CommitWrite()
{
	return (m_pTx->CommitWrite() ? TRUE : FALSE);
}
