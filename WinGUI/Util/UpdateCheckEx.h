/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2014 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___UPDATE_CHECK_EX_H___
#define ___UPDATE_CHECK_EX_H___

#pragma once

#include <boost/utility.hpp>
#include <string>
#include <vector>
#include <tchar.h>
#include "../NewGUI/StatusDialogEx.h"

typedef std::basic_string<TCHAR> std_string;

typedef struct
{
	std_string strName;

	std_string strStatus;
	int iStatusImage;

	UINT64 qwVerInstalled;
	UINT64 qwVerAvailable;
} UC_COMPONENT_INFO;

typedef std::vector<UC_COMPONENT_INFO> UC_COMPONENTS_LIST;

class CUpdateCheckEx : boost::noncopyable
{
public:
	CUpdateCheckEx(HWND hParent, CImageList* pImages, BOOL bUIOnlyOnUpdate);
	void _RunCheck(void* pDlg);

	static HRESULT Check(BOOL bRunInThread, HWND hParent, CImageList* pImages,
		BOOL bUIOnlyOnUpdate);

	static void EnsureConfigured(BOOL* pCheck, BOOL* pConfig, HWND hParent,
		HINSTANCE hInstance);

private:
	static HRESULT DownloadInfoFile(BYTE** ppbData, std_string& strError);
	static HRESULT ParseInfoFile(BYTE* pbData, UC_COMPONENTS_LIST& vList);
	static void AddComponent(const std_string& strLine, UC_COMPONENTS_LIST& vList);
	static void GetInstalledComponents(UC_COMPONENTS_LIST& vList,
		bool bDefaultToUnknownStatus);
	static void SortComponentsList(UC_COMPONENTS_LIST& vList);
	static void CopyAvailableVersions(const UC_COMPONENTS_LIST& vAvailable,
		UC_COMPONENTS_LIST& vOut);
	static void CompareVersions(UC_COMPONENTS_LIST& v, bool bSetStatus,
		std_string& strReport, DWORD& dwUpdates);

	void _FinalReport(const UC_COMPONENTS_LIST& vInstalled, LPCTSTR lpResult,
		bool bError, DWORD dwUpdates, void* pDlg);

	HWND m_hParent;
	CImageList* m_pImages;
	BOOL m_bUIOnlyOnUpdate;
};

#endif // ___UPDATE_CHECK_EX_H___
