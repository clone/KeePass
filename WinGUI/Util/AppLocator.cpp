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
#include "AppLocator.h"
#include "WinUtil.h"
#include "../../KeePassLibCpp/Util/StrUtil.h"
#include <boost/algorithm/string/predicate.hpp>

bool AppLocator::m_bPathsQueried = false;

std::basic_string<TCHAR> AppLocator::m_strIEPath;
std::basic_string<TCHAR> AppLocator::m_strFirefoxPath;
std::basic_string<TCHAR> AppLocator::m_strOperaPath;
std::basic_string<TCHAR> AppLocator::m_strChromePath;
std::basic_string<TCHAR> AppLocator::m_strSafariPath;

AppLocator::AppLocator()
{
}

AppLocator::~AppLocator()
{
}

void AppLocator::FillPlaceholders(CString* pString, const SPR_CONTENT_FLAGS* pcf)
{
	ASSERT(pString != NULL); if(pString == NULL) return;

	AppLocator::GetPaths();

	AppLocator::ReplacePath(pString, _T("{INTERNETEXPLORER}"), m_strIEPath, pcf);
	AppLocator::ReplacePath(pString, _T("{FIREFOX}"), m_strFirefoxPath, pcf);
	AppLocator::ReplacePath(pString, _T("{OPERA}"), m_strOperaPath, pcf);
	AppLocator::ReplacePath(pString, _T("{GOOGLECHROME}"), m_strChromePath, pcf);
	AppLocator::ReplacePath(pString, _T("{SAFARI}"), m_strSafariPath, pcf);
}

void AppLocator::ReplacePath(CString* p, LPCTSTR lpPlaceholder,
	const std::basic_string<TCHAR>& strFill, const SPR_CONTENT_FLAGS* pcf)
{
	if(p == NULL) { ASSERT(FALSE); return; }
	if(lpPlaceholder == NULL) { ASSERT(FALSE); return; }
	if(lpPlaceholder[0] == 0) { ASSERT(FALSE); return; }

	std::basic_string<TCHAR> str;
	if((pcf != NULL) && pcf->bMakeCmdQuotes)
	{
		str = _T("\"");
		str += SprTransformContent(strFill.c_str(), pcf);
		str += _T("\"");
	}
	else
	{
		std::basic_string<TCHAR> strWithQ = _T("\"");
		strWithQ += strFill;
		strWithQ += _T("\"");

		str = SprTransformContent(strWithQ.c_str(), pcf);
	}

	p->Replace(lpPlaceholder, str.c_str());
}

void AppLocator::GetPaths()
{
	if(m_bPathsQueried) return;

	if(m_strIEPath.size() == 0) AppLocator::FindInternetExplorer();
	if(m_strFirefoxPath.size() == 0) AppLocator::FindFirefox();
	if(m_strOperaPath.size() == 0) AppLocator::FindOpera();
	if(m_strChromePath.size() == 0) AppLocator::FindChrome();
	if(m_strSafariPath.size() == 0) AppLocator::FindSafari();

	m_bPathsQueried = true;
}

void AppLocator::FindInternetExplorer()
{
	for(int i = 0; i < 4; ++i)
	{
		std::basic_string<TCHAR> str;
		if(i == 0)
			str = GetRegStrEx(HKEY_LOCAL_MACHINE,
				_T("SOFTWARE\\Clients\\StartMenuInternet\\IEXPLORE.EXE\\shell\\open\\command"),
				_T(""), 0);
		else if(i == 1)
			str = GetRegStrEx(HKEY_LOCAL_MACHINE,
				_T("SOFTWARE\\Wow6432Node\\Clients\\StartMenuInternet\\IEXPLORE.EXE\\shell\\open\\command"),
				_T(""), 0);
		else if(i == 2)
			str = GetRegStrEx(HKEY_CLASSES_ROOT,
				_T("IE.AssocFile.HTM\\shell\\open\\command"), _T(""), 0);
		else if(i == 3)
			str = GetRegStrEx(HKEY_CLASSES_ROOT,
				_T("Applications\\iexplore.exe\\shell\\open\\command"), _T(""), 0);

		str = AppLocator::Fix(str);
		if(str.size() == 0) continue;
		// https://sourceforge.net/p/keepass/discussion/329221/thread/6b292ede/
		if(boost::algorithm::starts_with(str, _T("iexplore.exe"))) continue;

		m_strIEPath = str;
		break;
	}
}

void AppLocator::FindFirefox()
{
	LPCTSTR lpRoot = _T("SOFTWARE\\Mozilla\\Mozilla Firefox");
	LPCTSTR lpRoot32 = _T("SOFTWARE\\Wow6432Node\\Mozilla\\Mozilla Firefox");
	LPCTSTR lpRootESR = _T("SOFTWARE\\Mozilla\\Mozilla Firefox ESR");
	LPCTSTR lpRootESR32 = _T("SOFTWARE\\Wow6432Node\\Mozilla\\Mozilla Firefox ESR");

	std::basic_string<TCHAR> strVer = GetRegStrEx(HKEY_LOCAL_MACHINE,
		lpRoot, _T("CurrentVersion"), 0);
	LPCTSTR lpInfoRoot = lpRoot;

	if(strVer.size() == 0)
	{
		strVer = GetRegStrEx(HKEY_LOCAL_MACHINE, lpRoot32, _T("CurrentVersion"), 0);
		lpInfoRoot = lpRoot32;
	}

	// The ESR version stores the 'CurrentVersion' value under
	// 'Mozilla Firefox ESR', but the version-specific info
	// under 'Mozilla Firefox\\<Version>' (without 'ESR')

	if(strVer.size() == 0)
	{
		strVer = GetRegStrEx(HKEY_LOCAL_MACHINE, lpRootESR, _T("CurrentVersion"), 0);
		lpInfoRoot = lpRoot; // Not 'ESR'
	}

	if(strVer.size() == 0)
	{
		strVer = GetRegStrEx(HKEY_LOCAL_MACHINE, lpRootESR32, _T("CurrentVersion"), 0);
		lpInfoRoot = lpRoot32; // Not 'ESR'
	}

	if(strVer.size() == 0) return;

	std::basic_string<TCHAR> strCur = lpInfoRoot;
	strCur += _T("\\");
	strCur += strVer;
	strCur += _T("\\Main");

	m_strFirefoxPath = AppLocator::Fix(GetRegStrEx(HKEY_LOCAL_MACHINE,
		strCur.c_str(), _T("PathToExe"), 0));
}

void AppLocator::FindOpera()
{
	for(int i = 0; i < 2; ++i)
	{
		std::basic_string<TCHAR> str;
		if(i == 0) // Opera 20.0.1387.77
			str = GetRegStrEx(HKEY_LOCAL_MACHINE,
				_T("SOFTWARE\\Clients\\StartMenuInternet\\OperaStable\\shell\\open\\command"),
				_T(""), 0);
		else if(i == 1) // Old
			str = GetRegStrEx(HKEY_LOCAL_MACHINE,
				_T("SOFTWARE\\Clients\\StartMenuInternet\\Opera\\shell\\open\\command"),
				_T(""), 0);

		str = AppLocator::Fix(str);
		if(str.size() == 0) continue;

		m_strOperaPath = str;
		break;
	}
}

void AppLocator::FindChrome()
{
	LPCTSTR lpPath = _T("ChromeHTML\\shell\\open\\command");
	m_strChromePath = AppLocator::Fix(GetRegStrEx(HKEY_CLASSES_ROOT,
		lpPath, _T(""), 0));
	if(m_strChromePath.size() > 0) return;

	const size_t ccNameBuf = MAX_PATH + 2;
	TCHAR tszName[ccNameBuf];
	ZeroMemory(&tszName[0], ccNameBuf * sizeof(TCHAR));
	DWORD dwIndex = 0;
	while(true)
	{
		DWORD dwNameLen = ccNameBuf - 1;
		const LONG lRes = RegEnumKeyEx(HKEY_CLASSES_ROOT, dwIndex, &tszName[0],
			&dwNameLen, NULL, NULL, NULL, NULL);
		++dwIndex;

		if(lRes == ERROR_NO_MORE_ITEMS) break;
		if(lRes != ERROR_SUCCESS) continue;

		if((_tcslen(&tszName[0]) >= 11) && (_tcsnicmp(&tszName[0],
			_T("ChromeHTML."), 11) == 0))
		{
			std_string strKey = &tszName[0];
			strKey += _T("\\shell\\open\\command");

			m_strChromePath = AppLocator::Fix(GetRegStrEx(HKEY_CLASSES_ROOT,
				strKey.c_str(), _T(""), 0));
			if(m_strChromePath.size() > 0) return;
		}
	}

	lpPath = _T("Applications\\chrome.exe\\shell\\open\\command");
	m_strChromePath = AppLocator::Fix(GetRegStrEx(HKEY_CLASSES_ROOT,
		lpPath, _T(""), 0));
}

void AppLocator::FindSafari()
{
	LPCTSTR lpPath = _T("SOFTWARE\\Apple Computer, Inc.\\Safari");
	m_strSafariPath = AppLocator::Fix(GetRegStrEx(HKEY_LOCAL_MACHINE,
		lpPath, _T("BrowserExe"), 0));
}

std::basic_string<TCHAR> AppLocator::Fix(const std::basic_string<TCHAR>& strPath)
{
	return SU_GetQuotedPath(strPath);
}
