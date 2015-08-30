/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2013 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___PATTERN_BASED_GENERATOR_H___
#define ___PATTERN_BASED_GENERATOR_H___

#include "PasswordGenerator.h"

#define PbgWString std::basic_string<WCHAR>

PWG_ERROR PbgGenerate(std::vector<WCHAR>& vOutBuffer,
	const PW_GEN_SETTINGS_EX* pSettings, CNewRandom* pRandomSource);

PbgWString PbgExpandPattern(const PbgWString& strPattern);

void PbgAppendChar(std::vector<WCHAR>& vOutBuffer, WCHAR wch, DWORD& rdwPos);

#endif // ___PATTERN_BASED_GENERATOR_H___
