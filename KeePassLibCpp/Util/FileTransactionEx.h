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

#ifndef ___FILE_TRANSACTION_EX_H___
#define ___FILE_TRANSACTION_EX_H___

#pragma once

#include <string>
#include <tchar.h>

typedef std::basic_string<TCHAR> std_string;

class CFileTransactionEx
{
public:
	CFileTransactionEx(LPCTSTR lpBaseFile, bool bTransacted);

	bool OpenWrite(std_string& strOutBufferFile);
	bool CommitWrite();

private:
	bool CommitWriteTransaction();

	bool m_bTransacted;
	std_string m_strBaseFile;
	std_string m_strTempFile;

	bool m_bMadeUnhidden;
};

#endif // ___FILE_TRANSACTION_EX_H___
