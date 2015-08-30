/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2010 Dominik Reichl <dominik.reichl@t-online.de>

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
#include "FileLock.h"

#include "../../KeePassLibCpp/DataExchange/PwImport.h"
#include "../../KeePassLibCpp/Util/MemUtil.h"
#include "../../KeePassLibCpp/Util/StrUtil.h"

#include "WinUtil.h"

BOOL FileLock_Lock(LPCTSTR lpFile, BOOL bLock)
{
	if(lpFile == NULL) return FALSE;
	if(lpFile[0] == 0) return FALSE;

	CString str = lpFile;
	str += FL_LOCK_SUFFIX;

	if(bLock == TRUE)
	{
		PW_TIME t, tAdd;

		ZeroMemory(&tAdd, sizeof(PW_TIME));
		tAdd.btMinute = FL_TIME_LOCKING;

		_GetCurrentPwTime(&t);
		_pwtimeadd(&t, &tAdd);

		FILE *fp = NULL;
		_tfopen_s(&fp, str, _T("wb"));
		if(fp == NULL) return FALSE;

		VERIFY(fwrite(&t, sizeof(PW_TIME), 1, fp) == 1);

		std::basic_string<TCHAR> tszUser = WU_GetUserName();
		BYTE *pbUTF8 = _StringToUTF8(tszUser.c_str());
		fwrite(pbUTF8, 1, szlen((char *)pbUTF8) + 1, fp);

		fclose(fp); fp = NULL;

		SAFE_DELETE_ARRAY(pbUTF8);
	}
	else DeleteFile(str);

	return TRUE;
}

BOOL FileLock_IsLocked(LPCTSTR lpFile, std::basic_string<TCHAR>& strLockingUser)
{
	strLockingUser.clear();

	if(lpFile == NULL) return FALSE;
	if(lpFile[0] == 0) return FALSE;

	CString str = lpFile;
	str += FL_LOCK_SUFFIX;

	unsigned long uFileSize = 0;
	char *pData = CPwImport::FileToMemory((LPCTSTR)str, &uFileSize);
	if(pData == NULL) return FALSE;

	PW_TIME tNow, t;
	_GetCurrentPwTime(&tNow);

	ZeroMemory(&t, sizeof(PW_TIME));
	if(uFileSize >= sizeof(PW_TIME))
		memcpy(&t, pData, sizeof(PW_TIME));

	if((t.shYear == 0) || (_pwtimecmp(&t, &tNow) < 0))
	{
		FileLock_Lock(lpFile, FALSE); // Unlock the file
		SAFE_DELETE_ARRAY(pData);
		return FALSE; // It's not locked any more
	}

	char *pUser = pData + sizeof(PW_TIME); // pData is null-terminated
	TCHAR *tszUser = _UTF8ToString((UTF8_BYTE *)pUser);
	if(tszUser != NULL)
	{
		strLockingUser = tszUser;
		SAFE_DELETE_ARRAY(tszUser);
	}

	SAFE_DELETE_ARRAY(pData);
	return TRUE; // File is actively locked
}
