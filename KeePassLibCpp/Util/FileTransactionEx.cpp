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

#include "StdAfx.h"
#include "FileTransactionEx.h"
#include "PwUtil.h"

LPCTSTR g_lpTempSuffix = _T(".tmp");

CFileTransactionEx::CFileTransactionEx(LPCTSTR lpBaseFile, bool bTransacted) :
	m_bMadeUnhidden(false)
{
	m_bTransacted = bTransacted;

	if(lpBaseFile != NULL)
	{
		m_strBaseFile = lpBaseFile;

		if(m_bTransacted) m_strTempFile = m_strBaseFile + g_lpTempSuffix;
		else m_strTempFile = m_strBaseFile;
	}
}

// Must set last error before returning false
bool CFileTransactionEx::OpenWrite(std_string& strOutBufferFile)
{
	if(!m_bTransacted) m_bMadeUnhidden = CPwUtil::UnhideFile(m_strTempFile.c_str());

	strOutBufferFile = m_strTempFile;
	return true;
}

// Must set last error before returning false
bool CFileTransactionEx::CommitWrite()
{
	if(m_bTransacted) return CommitWriteTransaction();
	else // !m_bTransacted
	{
		if(m_bMadeUnhidden) CPwUtil::HideFile(m_strTempFile.c_str(), true);
	}

	return true;
}

bool CFileTransactionEx::CommitWriteTransaction()
{
	const bool bMadeUnhidden = CPwUtil::UnhideFile(m_strBaseFile.c_str());

	const DWORD dwBaseAttrib = GetFileAttributes(m_strBaseFile.c_str());
	if(dwBaseAttrib != INVALID_FILE_ATTRIBUTES) // File exists
	{
		CNullableEx<FILETIME> tCreation = CPwUtil::GetFileCreationTime(m_strBaseFile.c_str());
		if(tCreation.HasValue())
			CPwUtil::SetFileCreationTime(m_strTempFile.c_str(), tCreation.GetValuePtr());

		if(DeleteFile(m_strBaseFile.c_str()) == FALSE) return false;
	}

	if(MoveFile(m_strTempFile.c_str(), m_strBaseFile.c_str()) == FALSE)
		return false;

	if((dwBaseAttrib != INVALID_FILE_ATTRIBUTES) &&
		((dwBaseAttrib & FILE_ATTRIBUTE_ENCRYPTED) != 0))
		CPwUtil::EfsEncryptFile(m_strBaseFile.c_str());
	if(bMadeUnhidden)
		CPwUtil::HideFile(m_strBaseFile.c_str(), true);

	return true;
}
