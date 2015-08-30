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

#include "StdAfx.h"
#include "MemUtil.h"
#include "FileLock.h"

C_FN_SHARE BOOL FileLock_Lock(LPCTSTR lpFile, BOOL bLock)
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

		FILE *fp = _tfopen(str, _T("wb"));
		if(fp == NULL) return FALSE;

		VERIFY(fwrite(&t, sizeof(PW_TIME), 1, fp) == 1);
		fclose(fp); fp = NULL;
	}
	else DeleteFile(str);

	return TRUE;
}

C_FN_SHARE BOOL FileLock_IsLocked(LPCTSTR lpFile)
{
	if(lpFile == NULL) return FALSE;
	if(lpFile[0] == 0) return FALSE;

	CString str = lpFile;
	str += FL_LOCK_SUFFIX;

	FILE *fp = _tfopen(str, _T("rb"));
	if(fp == NULL) return FALSE; // No locking file -> not locked

	PW_TIME t, tNow;
	ZeroMemory(&t, sizeof(PW_TIME));
	VERIFY(fread(&t, sizeof(PW_TIME), 1, fp) == 1);
	fclose(fp); fp = NULL;

	_GetCurrentPwTime(&tNow);

	if((t.shYear == 0) || (_pwtimecmp(&t, &tNow) < 0))
	{
		FileLock_Lock(lpFile, FALSE); // Unlock the file
		return FALSE; // It's not locked any more
	}

	return TRUE; // File is actively locked
}
