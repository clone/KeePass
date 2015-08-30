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

/*
#include "UpdateCheck.h"
#include "../../KeePassLibCpp/PwManager.h"
#include "../../KeePassLibCpp/Util/TranslateEx.h"
#include "../Plugins/KpApiImpl.h"
#include <string>

DWORD WINAPI CFU_Thread(LPVOID lpParameter);
void CFU_Report(LPCTSTR lpInfo);

static DWORD g_dwCacheDummy = 0;
static HWND g_hCacheParent = NULL;
static DWORD g_dwCacheCurVer = 0;
static LPCTSTR g_lpCacheLatestURL = NULL;
static BOOL g_bCacheForeground = FALSE;

void CheckForUpdateAsync(HWND hParent, DWORD dwCurVer, LPCTSTR lpLatestURL, BOOL bForeground)
{
	g_dwCacheDummy = 0;
	g_hCacheParent = hParent;
	g_dwCacheCurVer = dwCurVer;
	g_lpCacheLatestURL = lpLatestURL;
	g_bCacheForeground = bForeground;

	HANDLE h = CreateThread(NULL, 0, CFU_Thread, NULL, 0, &g_dwCacheDummy);

	if((h != NULL) && (h != INVALID_HANDLE_VALUE)) CloseHandle(h);
	h = NULL;
}

void CFU_Report(LPCTSTR lpInfo, int nIcon)
{
	if(g_bCacheForeground == TRUE)
		MessageBox(g_hCacheParent, lpInfo, TRL("Check For Updates"), nIcon | MB_OK);
	else
	{
		std::basic_string<TCHAR> str = TRL("Check For Updates");
		str += _T(": ");
		str += lpInfo;
		CKpApiImpl::Instance().SetStatusBarText(str.c_str());
	}
}

DWORD WINAPI CFU_Thread(LPVOID lpParameter)
{
	TCHAR tszFile[MAX_PATH + 34];

	UNREFERENCED_PARAMETER(lpParameter);

	tszFile[0] = 0; tszFile[1] = 0;
	URLDownloadToCacheFile(NULL, g_lpCacheLatestURL, tszFile, URLOSTRM_GETNEWESTVERSION,
		0, NULL);

	if(_tcslen(tszFile) != 0)
	{
		FILE *fp = NULL;
		_tfopen_s(&fp, tszFile, _T("rb"));
		if(fp != NULL)
		{
			fseek(fp, 0, SEEK_END);
			unsigned long uFileSize = (unsigned long)ftell(fp);
			fseek(fp, 0, SEEK_SET);

			if(uFileSize != 0)
			{
				char *pDataBuf = new char[uFileSize + 2];
				char ch;

				fread(pDataBuf, 1, uFileSize, fp);
				pDataBuf[uFileSize] = 0; pDataBuf[uFileSize + 1] = 0;

				for(DWORD i = 0; i < uFileSize; i++)
				{
					ch = pDataBuf[i];

					if((ch == '\r') || (ch == '\n') || (ch == ' ') || (ch == '\t') || (ch == 0))
					{
						pDataBuf[i] = 0;
						break;
					}
				}

				DWORD dwVer = strtoul(pDataBuf, NULL, 16);

				if((pDataBuf[0] != _T('0')) || (pDataBuf[1] != _T('x')))
					CFU_Report(TRL("Loading error"), MB_ICONSTOP);
				// else if(dwVer > g_dwCacheCurVer)
				//	CFU_Report(TRL("New KeePass version available!"), MB_ICONINFORMATION);
				else if(dwVer == g_dwCacheCurVer)
					CFU_Report(TRL("You have the latest version."), MB_ICONINFORMATION);
				else if((g_dwCacheCurVer >= 0x01000001) && (dwVer == (g_dwCacheCurVer - 1)))
					CFU_Report(TRL("You have the latest version."), MB_ICONINFORMATION);
				else if(dwVer < g_dwCacheCurVer)
					CFU_Report(TRL("Loading error"), MB_ICONSTOP);

				if(dwVer > g_dwCacheCurVer)
				{
					std::basic_string<TCHAR> str = TRL("New KeePass version available!");
					str += _T("\r\n\r\n");
					str += TRL("Do you want to visit the KeePass homepage now?");

					int n = MessageBox(g_hCacheParent, str.c_str(),
						TRL("Check For Updates"), MB_ICONINFORMATION | MB_YESNO);

					if(n == IDYES)
						ShellExecute(g_hCacheParent, NULL, PWM_HOMEPAGE, NULL, NULL, 0);
				}

				fclose(fp); fp = NULL;
				delete []pDataBuf; pDataBuf = NULL;
			}
			else CFU_Report(TRL("Cache error (cannot open cached file)."), MB_ICONSTOP);

			if(fp != NULL) fclose(fp); fp = NULL;
		}
		else CFU_Report(TRL("Cache error (cannot open cached file)."), MB_ICONSTOP);
	}
	else CFU_Report(TRL("Connect failed, cannot check for updates."), MB_ICONSTOP);

	return 0;
}
*/
