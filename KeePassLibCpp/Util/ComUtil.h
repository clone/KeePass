/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2008 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___KP_COM_UTIL_H___
#define ___KP_COM_UTIL_H___

#pragma once

#include <string>
#include <sstream>
#include <tchar.h>

#define RCCMD_CONNECT           _T("Connect")
#define RCCMD_GETPERMLEVEL      _T("GetPermissionLevel")
#define RCCMD_ENTRY_FIND_BYURL  _T("Entry.Find.ByURL")
#define RCCMD_ENTRY_QUERY_FIELD _T("Entry.Field.Query")
#define RCCMD_ENTRY_ADD_BYURL   _T("Entry.Add.ByURL")
#define RCCMD_ENTRY_ADD_FIELD   _T("Entry.Field.Add")

#define KEEPASS_MAILSLOT        _T("\\\\.\\mailslot\\KeePassMail")
#define KEEPASS_MAILSLOT_NAME   _T("KeePassMail")

typedef std::basic_string<TCHAR> RC_STRING;

typedef struct _RC_QUERY
{
	RC_STRING strClientID;
	RC_STRING strCommand;
	RC_STRING strParamW;
	RC_STRING strParamL;
} RC_QUERY;

BOOL RCPackQuery(RC_STRING& strQueryBuffer, const RC_QUERY& rcQuery);
BOOL RCUnpackQuery(RC_QUERY& rcQueryBuffer, const RC_STRING& strQuery);

BOOL ReadFixedString(RC_STRING& strBuffer, std::basic_stringstream<TCHAR>& streamSource, unsigned int uCount);

BOOL SendMailslotMessage(const RC_STRING& strMailslotName, const RC_STRING& strMessage, UINT uEventMessage);

#endif // ___KP_COM_UTIL_H___
