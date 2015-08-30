/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2015 Dominik Reichl <dominik.reichl@t-online.de>

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
#include <sstream>
#include "ComUtil.h"

#ifndef ASSERT
#define ASSERT(x) ATLASSERT(x)
#endif

// Do not validate any members of the query in this function
BOOL RCPackQuery(RC_STRING& strQueryBuffer, const RC_QUERY& rcQuery)
{
	std::basic_stringstream<TCHAR> ss;

	ss << rcQuery.strClientID.size() << _T(" ");
	ss << rcQuery.strCommand.size() << _T(" ");
	ss << rcQuery.strParamW.size() << _T(" ");
	ss << rcQuery.strParamL.size() << _T(" @");

	ss << rcQuery.strClientID;
	ss << rcQuery.strCommand;
	ss << rcQuery.strParamW;
	ss << rcQuery.strParamL;

	strQueryBuffer = ss.str();
	return TRUE;
}

// Do not validate any members of the query in this function
BOOL RCUnpackQuery(RC_QUERY& rcQueryBuffer, const RC_STRING& strQuery)
{
	std::basic_stringstream<TCHAR> ss(strQuery + _T("@"));
	unsigned int uClientIDSize = 0, uCommandSize = 0, uParamWSize = 0, uParamLSize = 0;

	try
	{
		ss >> uClientIDSize;
		ss >> uCommandSize;
		ss >> uParamWSize;
		ss >> uParamLSize;

		ss >> std::noskipws;

		TCHAR tch = _T(' ');
		while(tch != _T('@'))
		{
			if(!(ss >> tch)) { tch = 0; break; }
		}
		if(tch == 0) { ASSERT(FALSE); return FALSE; }

		ReadFixedString(rcQueryBuffer.strClientID, ss, uClientIDSize);
		if(rcQueryBuffer.strClientID.size() != uClientIDSize) return FALSE;
		ReadFixedString(rcQueryBuffer.strCommand, ss, uCommandSize);
		if(rcQueryBuffer.strCommand.size() != uCommandSize) return FALSE;
		ReadFixedString(rcQueryBuffer.strParamW, ss, uParamWSize);
		if(rcQueryBuffer.strParamW.size() != uParamWSize) return FALSE;
		ReadFixedString(rcQueryBuffer.strParamL, ss, uParamLSize);
		if(rcQueryBuffer.strParamL.size() != uParamLSize) return FALSE;

		RC_STRING strTest;
		ReadFixedString(strTest, ss, 1);
		ASSERT((strTest.size() == 1) && (strTest[0] == _T('@')));
	}
	catch(...)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL ReadFixedString(RC_STRING& strBuffer, std::basic_stringstream<TCHAR>& streamSource, unsigned int uCount)
{
	strBuffer.clear();

	if(uCount == 0) return TRUE;

	TCHAR tch;
	for(unsigned int i = 0; i < uCount; ++i)
	{
		streamSource >> tch;

		if(tch == 0) return FALSE;

		strBuffer += tch;
	}

	return TRUE;
}

BOOL SendMailslotMessage(const RC_STRING& strMailslotName, const RC_STRING& strMessage, UINT uEventMessage)
{
	const RC_STRING strFullName = RC_STRING(_T("\\\\.\\mailslot\\")) + strMailslotName;

	HANDLE hMailslot = CreateFile(strFullName.c_str(), GENERIC_WRITE, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if((hMailslot == INVALID_HANDLE_VALUE) || (hMailslot == NULL)) return FALSE;

	DWORD dwWritten = 0;
	if(WriteFile(hMailslot, strMessage.c_str(), static_cast<DWORD>(strMessage.size() + 1),
		&dwWritten, NULL) == FALSE)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	ASSERT(dwWritten == (strMessage.size() + 1));

	CloseHandle(hMailslot);

	if((uEventMessage != 0) && (uEventMessage != static_cast<UINT>(-1)))
		SendMessage(HWND_BROADCAST, uEventMessage, 0, 0);

	return TRUE;
}
