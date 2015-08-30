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

#ifndef ___PLUGIN_MGR_H___
#define ___PLUGIN_MGR_H___

#include "../../KeePassLibCpp/SDK/KpSDK.h"
#include <vector>
#include <boost/utility.hpp>

#define DUMMY_COMMAND_LINE_OPT _T("--nop")

typedef std::basic_string<TCHAR> std_string;

typedef struct
{
	DWORD dwPluginID; // Assigned by KeePass, used internally
	std_string strPath;
	HMODULE hinstDLL;

	IKpPlugin* pInterface;

	std_string strName;
	std_string strVersion;
	std_string strAuthor;

	UINT64 qwVersion;
} KP_PLUGIN_INSTANCE, *LPKP_PLUGIN_INSTANCE;

class CPluginManager : boost::noncopyable
{
public:
    static CPluginManager& Instance();

	// BOOL SetAppInfo(const KP_APP_INFO *pAppInfo);
	BOOL SetDirectCommandRange(DWORD dwFirstCommand, DWORD dwLastCommand);

	// BOOL EnablePluginByID(DWORD dwPluginID, BOOL bEnable = TRUE);
	// BOOL EnablePluginByStr(LPCTSTR lpPluginFile, BOOL bEnable = TRUE);

	KP_PLUGIN_INSTANCE* GetPluginByID(DWORD dwID);

	BOOL LoadAllPlugins();
	BOOL UnloadAllPlugins();

	BOOL CallSinglePlugin(DWORD dwPluginID, DWORD dwCode, LPARAM lParamW, LPARAM lParamL);
	BOOL CallPlugins(DWORD dwCode, LPARAM lParamW, LPARAM lParamL);

    bool UsesCmdArg(const std_string& argument) const;  // Returns true if a plugin uses argument.

	std::vector<KP_PLUGIN_INSTANCE> m_plugins;
	// std::vector<std_string> m_vKnownNames;

private:
	CPluginManager();
	~CPluginManager();

	std::vector<std_string> FindPluginCandidates();

	bool _IsValidPlugin(LPCTSTR lpFile, KP_PLUGIN_INSTANCE* pOutStruct);
	void CleanUp();

	BOOL _AssignPluginCommands();

	static void ClearStructure(KP_PLUGIN_INSTANCE* p);
	static void FreePluginDllEx(HMODULE h);

	DWORD m_dwFreePluginID;
	DWORD m_dwFirstCommand;
	DWORD m_dwLastCommand;

	// KP_APP_INFO m_kpAppInfo;
};

inline BOOL _CallPlugins(DWORD dwCode, LPARAM lParamW, LPARAM lParamL)
{
	return CPluginManager::Instance().CallPlugins(dwCode, lParamW, lParamL);
}

#endif
