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
#include "UpdateCheckEx.h"

#include <boost/algorithm/string.hpp>

#include "../../KeePassLibCpp/Util/StrUtil.h"
#include "../../KeePassLibCpp/Util/TranslateEx.h"
#include "../../KeePassLibCpp/PwManager.h"
#include "../Plugins/PluginMgr.h"
#include "../Plugins/KpApiImpl.h"

#include "../UpdateInfoDlg.h"

typedef struct
{
	HWND hParent;
	CImageList* pImages;
	BOOL bUIOnlyOnUpdate;
} UC_INIT_STRUCT;

DWORD WINAPI CfuEx_Thread(LPVOID lpParameter);

static UC_INIT_STRUCT g_ucInitStruct;

CUpdateCheckEx::CUpdateCheckEx(HWND hParent, CImageList* pImages,
	BOOL bUIOnlyOnUpdate) :
	m_hParent(hParent), m_pImages(pImages), m_bUIOnlyOnUpdate(bUIOnlyOnUpdate)
{
}

HRESULT CUpdateCheckEx::Check(BOOL bRunInThread, HWND hParent, CImageList* pImages,
	BOOL bUIOnlyOnUpdate)
{
	HRESULT hRes = S_OK;

	if(bRunInThread == TRUE)
	{
		g_ucInitStruct.hParent = hParent;
		g_ucInitStruct.pImages = pImages;
		g_ucInitStruct.bUIOnlyOnUpdate = bUIOnlyOnUpdate;

		DWORD dwDummyID = 0; // Pointer may not be NULL on Windows 9x
		HANDLE h = CreateThread(NULL, 0, CfuEx_Thread, NULL, 0, &dwDummyID);

		if((h != NULL) && (h != INVALID_HANDLE_VALUE)) CloseHandle(h);
		else hRes = E_FAIL;
	}
	else
	{
		CUpdateCheckEx uc(hParent, pImages, bUIOnlyOnUpdate);
		uc._RunCheck();
	}

	return hRes;
}

DWORD WINAPI CfuEx_Thread(LPVOID lpParameter)
{
	UNREFERENCED_PARAMETER(lpParameter);

	CUpdateCheckEx uc(g_ucInitStruct.hParent, g_ucInitStruct.pImages,
		g_ucInitStruct.bUIOnlyOnUpdate);
	uc._RunCheck();

	return 0;
}

void CUpdateCheckEx::_RunCheck()
{
	UC_COMPONENTS_LIST vInstalled;
	CUpdateCheckEx::GetInstalledComponents(vInstalled, true);

	BYTE* pb = NULL;
	std_string strError;
	HRESULT hRes = CUpdateCheckEx::DownloadInfoFile(&pb, strError);
	if(hRes != S_OK)
	{
		SAFE_DELETE_ARRAY(pb);

		_FinalReport(vInstalled, strError.c_str(), true, 0);
		return;
	}

	UC_COMPONENTS_LIST vAvailable;
	hRes = CUpdateCheckEx::ParseInfoFile(pb, vAvailable);
	SAFE_DELETE_ARRAY(pb);
	if(hRes != S_OK)
	{
		_FinalReport(vInstalled, TRL("Loading error"), true, 0);
		return;
	}

	CUpdateCheckEx::CopyAvailableVersions(vAvailable, vInstalled);

	std_string strReport;
	DWORD dwUpdates = 0;
	CUpdateCheckEx::CompareVersions(vInstalled, true, strReport, dwUpdates);
	_FinalReport(vInstalled, strReport.c_str(), false, dwUpdates);
}

void CUpdateCheckEx::_FinalReport(const UC_COMPONENTS_LIST& vInstalled,
	LPCTSTR lpResult, bool bError, DWORD dwUpdates)
{
	UNREFERENCED_PARAMETER(bError);

	LPCTSTR lpMsg = ((lpResult != NULL) ? lpResult : _T(""));

	bool bShowDialog = true;
	if(m_bUIOnlyOnUpdate == TRUE) bShowDialog = (dwUpdates > 0);
	if(m_pImages == NULL) bShowDialog = false;

	if(bShowDialog)
	{
		CUpdateInfoDlg dlg(CWnd::FromHandle(m_hParent));
		dlg.InitEx(m_pImages, &vInstalled, lpResult);
		dlg.DoModal();
	}
	else CKpApiImpl::Instance().SetStatusBarText(lpMsg);
}

HRESULT CUpdateCheckEx::DownloadInfoFile(BYTE** ppbData, std_string& strError)
{
	if(ppbData == NULL) { ASSERT(FALSE); return E_POINTER; }
	*ppbData = NULL;

	TCHAR tszFile[MAX_PATH + 34];
	ZeroMemory(tszFile, sizeof(TCHAR) * (MAX_PATH + 34));

	URLDownloadToCacheFile(NULL, PWM_URL_VERSION, tszFile, URLOSTRM_GETNEWESTVERSION,
		0, NULL);

	if(_tcslen(tszFile) != 0)
	{
		FILE* fp = NULL;
		_tfopen_s(&fp, tszFile, _T("rb"));

		if(fp != NULL)
		{
			fseek(fp, 0, SEEK_END);
			const DWORD uFileSize = static_cast<DWORD>(ftell(fp));
			fseek(fp, 0, SEEK_SET);

			if(uFileSize != 0)
			{
				BYTE* pDataBuf = new BYTE[uFileSize + 2];

				fread(pDataBuf, 1, uFileSize, fp);
				fclose(fp);

				pDataBuf[uFileSize] = 0;
				pDataBuf[uFileSize + 1] = 0;

				*ppbData = pDataBuf;
			}
			else
			{
				strError = TRL("Cache error (cannot open cached file).");
				return E_ACCESSDENIED;
			}
		}
		else
		{
			strError = TRL("Cache error (cannot open cached file).");
			return E_ACCESSDENIED;
		}
	}
	else
	{
		strError = TRL("Connect failed, cannot check for updates.");
		return E_ACCESSDENIED;
	}

	strError.clear();
	return S_OK;
}

HRESULT CUpdateCheckEx::ParseInfoFile(BYTE* pbData, UC_COMPONENTS_LIST& vList)
{
	if(pbData == NULL) { ASSERT(FALSE); return E_POINTER; }

	vList.clear();

	TCHAR* lpData = _UTF8ToString(pbData);
	if(lpData == NULL) { ASSERT(FALSE); return E_FAIL; }
	std_string strData = lpData;
	SAFE_DELETE_ARRAY(lpData);

	std::vector<std_string> vItems;
	boost::algorithm::split(vItems, strData, boost::algorithm::is_any_of(_T("\r\n")));

	for(size_t i = 0; i < vItems.size(); ++i)
		CUpdateCheckEx::AddComponent(vItems[i], vList);

	return S_OK;
}

void CUpdateCheckEx::AddComponent(const std_string& strLine, UC_COMPONENTS_LIST& vList)
{
	if(strLine.size() == 0) return;

	if(strLine[0] == _T('#')) { ASSERT(FALSE); return; } // Reserved for future use

	std::vector<std_string> vInfo;
	boost::algorithm::split(vInfo, strLine, boost::algorithm::is_any_of(_T("#")));
	if(vInfo.size() < 2) { ASSERT(FALSE); return; }

	std::vector<std_string> vVersion;
	boost::algorithm::split(vVersion, vInfo[1], boost::algorithm::is_any_of(_T(".")));
	if(vVersion.size() < 4) { ASSERT(FALSE); return; }

	UC_COMPONENT_INFO c;
	c.strName = vInfo[0];
	c.qwVerAvailable = ((static_cast<UINT64>(_ttol(vVersion[0].c_str())) << 48) |
		(static_cast<UINT64>(_ttol(vVersion[1].c_str())) << 32) |
		(static_cast<UINT64>(_ttol(vVersion[2].c_str())) << 16) |
		static_cast<UINT64>(_ttol(vVersion[3].c_str())));

	vList.push_back(c);
}

void CUpdateCheckEx::GetInstalledComponents(UC_COMPONENTS_LIST& vList,
	bool bDefaultToUnknownStatus)
{
	vList.clear();

	const UINT64 qwVersion = PWM_VERSION_DW;
	UC_COMPONENT_INFO kp;
	kp.strName = PWM_PRODUCT_NAME_SHORT;
	kp.qwVerInstalled = ((((((qwVersion >> 24) & 0xFF) << 16) |
		((qwVersion >> 16) & 0xFF)) << 32) |
		((((qwVersion >> 8) & 0xFF) << 16) | (qwVersion & 0xFF)));
	vList.push_back(kp);

	const CPluginManager& pm = CPluginManager::Instance();

	typedef std::vector<KP_PLUGIN_INSTANCE>::const_iterator PlgIter;
	for(PlgIter it = pm.m_plugins.begin(); it != pm.m_plugins.end(); ++it)
	{
		if((it->hinstDLL == NULL) || (it->pInterface == NULL))
		{
			ASSERT(FALSE);
			continue;
		}

		UC_COMPONENT_INFO c;
		c.strName = it->strName;
		c.qwVerInstalled = it->qwVersion;

		vList.push_back(c);
	}

	for(size_t i = 0; i < vList.size(); ++i)
	{
		if(bDefaultToUnknownStatus)
		{
			if(i > 0) vList[i].strStatus = TRL("Plugin unknown.");
			else { vList[i].strStatus = TRL("Unknown"); vList[i].strStatus += _T("."); }

			vList[i].iStatusImage = 44;
		}

		vList[i].qwVerAvailable = 0;
	}

	SortComponentsList(vList);
}

bool _UC_Component_Compare(UC_COMPONENT_INFO x, UC_COMPONENT_INFO y)
{
	if(x.strName == PWM_PRODUCT_NAME_SHORT) return true;
	if(y.strName == PWM_PRODUCT_NAME_SHORT) return false;

	return (x.strName.compare(y.strName) <= 0);
}

void CUpdateCheckEx::SortComponentsList(UC_COMPONENTS_LIST& vList)
{
	std::sort(vList.begin(), vList.end(), _UC_Component_Compare);
}

void CUpdateCheckEx::CopyAvailableVersions(const UC_COMPONENTS_LIST& vAvailable,
	UC_COMPONENTS_LIST& vOut)
{
	for(size_t i = 0; i < vOut.size(); ++i)
	{
		for(size_t j = 0; j < vAvailable.size(); ++j)
		{
			if(vAvailable[j].strName == vOut[i].strName)
				vOut[i].qwVerAvailable = vAvailable[j].qwVerAvailable;
		}
	}
}

void CUpdateCheckEx::CompareVersions(UC_COMPONENTS_LIST& v, bool bSetStatus,
	std_string& strReport, DWORD& dwUpdates)
{
	DWORD dwAvUpdates = 0;
	for(size_t i = 0; i < v.size(); ++i)
	{
		const UINT64 dwIn = v[i].qwVerInstalled;
		const UINT64 dwAv = v[i].qwVerAvailable;

		if(dwIn == dwAv)
		{
			if(bSetStatus)
			{
				v[i].strStatus = TRL("Up to date.");
				v[i].iStatusImage = 53;
			}
		}
		else if(dwIn < dwAv)
		{
			if(bSetStatus)
			{
				v[i].strStatus = TRL("New version available.");
				v[i].iStatusImage = 61;
			}

			++dwAvUpdates;
		}
		else if((dwIn > dwAv) && (dwAv != 0))
		{
			if(bSetStatus)
			{
				v[i].strStatus = TRL("Pre-release version.");
				v[i].iStatusImage = 39;
			}
		}
	}

	dwUpdates = dwAvUpdates;
	if(dwAvUpdates == 0) strReport = TRL("No updates available.");
	else if(dwAvUpdates == 1) strReport = TRL("1 update is available!");
	else
	{
		CString strFmt;
		strFmt.Format(TRL("%u updates are available!"), dwAvUpdates);
		strReport = (LPCTSTR)strFmt;
	}
}
