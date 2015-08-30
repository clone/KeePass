/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2007 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___PASSWORD_GENERATOR_MAIN_H___
#define ___PASSWORD_GENERATOR_MAIN_H___

#include "../SysDefEx.h"
#include "../Util/NewRandom.h"
#include "../Util/MemUtil.h"

#include "PwCharSet.h"

#define PWG_ERROR USHORT

#define PWGE_SUCCESS 0
#define PWGE_NULL_PTR 1
#define PWGE_UNKNOWN_GENERATOR 2
#define PWGE_TOO_FEW_CHARACTERS 3

#define PWGD_VERSION_BYTE 2

#define PWGT_NULL 0
#define PWGT_CHARSET 1
#define PWGT_PATTERN 2

#pragma pack(1)

/* typedef struct _PW_GEN_SETTINGS
{
	LPTSTR lpName;

	BYTE btGeneratorType;
	BOOL bCollectUserEntropy;

	DWORD dwLength;

	LPWSTR lpCharSet;

	LPWSTR lpPattern;
	BOOL bPatternPermute;

	BOOL bNoConfusing;
} PW_GEN_SETTINGS; */

typedef struct _PW_GEN_SETTINGS_EX
{
	std::basic_string<TCHAR> strName;

	BYTE btGeneratorType;
	BOOL bCollectUserEntropy;

	DWORD dwLength;

	std::basic_string<WCHAR> strCharSet;

	std::basic_string<WCHAR> strPattern;
	BOOL bPatternPermute;

	BOOL bNoConfusing;
} PW_GEN_SETTINGS_EX;

#pragma pack()

// Note: the returned vector contains at least one Null character at the end
PWG_ERROR PwgGenerateEx(std::vector<TCHAR>& vOutPassword,
	const PW_GEN_SETTINGS_EX* pSettings, CNewRandom* pRandomSource);

WCHAR PwgGenerateCharacter(const PW_GEN_SETTINGS_EX* pSettings,
	CNewRandom* pRandom, PwCharSet* pCharSet);

void PwgPrepareCharSet(PwCharSet* pCharSet, const PW_GEN_SETTINGS_EX* pSettings);

void PwgShufflePassword(std::vector<WCHAR>& vBuffer, CNewRandom* pRandom);

LPCTSTR PwgErrorToString(PWG_ERROR uError);

// std::basic_string<WCHAR> HexStrToWCharStr(LPCTSTR lpString);
// std::basic_string<TCHAR> WCharVecToHexStr(const std::vector<WCHAR>& str);

std::basic_string<TCHAR> PwgProfileToString(const PW_GEN_SETTINGS_EX* pSettings);
void PwgStringToProfile(const std::basic_string<TCHAR>& strProfile,
	PW_GEN_SETTINGS_EX* s);

void PwgGetDefaultProfile(PW_GEN_SETTINGS_EX* s);

#endif // ___PASSWORD_GENERATOR_MAIN_H___
