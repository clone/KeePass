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
#include "AppLocator.h"
#include "WinUtil.h"
#include "../../KeePassLibCpp/Util/StrUtil.h"

std::basic_string<TCHAR> AppLocator::m_strIEPath;
std::basic_string<TCHAR> AppLocator::m_strFirefoxPath;
std::basic_string<TCHAR> AppLocator::m_strOperaPath;

AppLocator::AppLocator()
{
}

AppLocator::~AppLocator()
{
}

void AppLocator::FillPlaceholders(CString* pString)
{
	ASSERT(pString != NULL); if(pString == NULL) return;

	AppLocator::GetPaths();

	pString->Replace(_T("{INTERNETEXPLORER}"), m_strIEPath.c_str());
	pString->Replace(_T("{FIREFOX}"), m_strFirefoxPath.c_str());
	pString->Replace(_T("{OPERA}"), m_strOperaPath.c_str());
}

void AppLocator::GetPaths()
{
	if(m_strIEPath.size() == 0) AppLocator::FindInternetExplorer();
	if(m_strFirefoxPath.size() == 0) AppLocator::FindFirefox();
	if(m_strOperaPath.size() == 0) AppLocator::FindOpera();
}

void AppLocator::FindInternetExplorer()
{
	LPCTSTR lpPath = _T("Applications\\iexplore.exe\\shell\\open\\command");
	m_strIEPath = AppLocator::Fix(GetRegStrEx(HKEY_CLASSES_ROOT, lpPath,
		_T(""), 0));
}

void AppLocator::FindFirefox()
{
	LPCTSTR lpVer = _T("SOFTWARE\\Mozilla\\Mozilla Firefox");
	std::basic_string<TCHAR> strVer = GetRegStrEx(HKEY_LOCAL_MACHINE,
		lpVer, _T("CurrentVersion"), 0);

	if(strVer.size() == 0) return;

	std::basic_string<TCHAR> strCur = lpVer;
	strCur += _T("\\");
	strCur += strVer;
	strCur += _T("\\Main");

	m_strFirefoxPath = AppLocator::Fix(GetRegStrEx(HKEY_LOCAL_MACHINE,
		strCur.c_str(), _T("PathToExe"), 0));
}

void AppLocator::FindOpera()
{
	LPCTSTR lpPath = _T("SOFTWARE\\Clients\\Mail\\Opera\\shell\\open\\command");
	m_strOperaPath = AppLocator::Fix(GetRegStrEx(HKEY_LOCAL_MACHINE,
		lpPath, _T(""), 0));
}

std::basic_string<TCHAR> AppLocator::Fix(const std::basic_string<TCHAR>& strPath)
{
	std::basic_string<TCHAR> str = _T("\"");
	str += GetQuotedAppPath(strPath);
	str += _T("\"");

	return str;
}
