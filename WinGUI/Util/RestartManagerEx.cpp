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

#include "StdAfx.h"

/*
#include "RestartManagerEx.h"
#include "CmdLine/CommandLineTokens.h"
#include "../Plugins/PluginMgr.h"
#include "../../KeePassLibCpp/Util/StrUtil.h"
#include "../../KeePassLibCpp/Util/MemUtil.h"

CRestartManagerEx::CRestartManagerEx()
{
}

void CRestartManagerEx::Register()
{
	HMODULE hMod = LoadLibrary(_T("Kernel32.dll"));
	if(hMod == NULL) { ASSERT(FALSE); return; }

	LPREGISTERAPPLICATIONRESTART lpFn = (LPREGISTERAPPLICATIONRESTART)
		GetProcAddress(hMod, "RegisterApplicationRestart");

	if(lpFn != NULL)
	{
		std::basic_string<TCHAR> strArgs = DUMMY_COMMAND_LINE_OPT;

		for(CommandLineTokens::const_iterator it = CommandLineTokens::instance().begin();
			it != CommandLineTokens::instance().end(); ++it)
		{
			const std_string argument(*it);
			if(argument.size() > 0)
			{
				strArgs += _T(" ");
				strArgs += argument;
			}
		}

#ifdef _UNICODE
		lpFn(strArgs.c_str(), 0);
#else // ANSI
		WCHAR *pwch = _StringToUnicode(strArgs.c_str());
		if(pwch != NULL)
		{
			lpFn(pwch, 0);
			SAFE_DELETE_ARRAY(pwch);
		}
		else { ASSERT(FALSE); }
#endif
	}
	else { ASSERT(FALSE); }

	VERIFY(FreeLibrary(hMod)); hMod = NULL;
}
*/
