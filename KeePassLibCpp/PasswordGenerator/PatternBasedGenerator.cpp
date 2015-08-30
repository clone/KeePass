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
#include "PatternBasedGenerator.h"
#include "../Util/StrUtil.h"

#define PBG_RANDOM_BUFFER_SIZE 1024

PWG_ERROR PbgGenerate(std::vector<WCHAR>& vOutBuffer,
	const PW_GEN_SETTINGS_EX* pSettings, CNewRandom* pRandomSource)
{
	ASSERT(pSettings != NULL);
	if(pSettings == NULL) return PWGE_NULL_PTR;
	ASSERT(pRandomSource != NULL);
	if(pRandomSource == NULL) return PWGE_NULL_PTR;

	DWORD dwOutBufPos = 0;

	DWORD dwRandomPos = 0;
	BYTE pbRandom[PBG_RANDOM_BUFFER_SIZE];
	pRandomSource->GetRandomBuffer(pbRandom, PBG_RANDOM_BUFFER_SIZE);

	PbgWString strOrgPattern = pSettings->strPattern;
	PbgWString strExpPattern = PbgExpandPattern(strOrgPattern);

	WCharStream csPattern(strExpPattern.c_str());
	WCHAR ch = csPattern.ReadChar();

	PwCharSet pwCustomCharSet, pwUsedCharSet;
	bool bInCharSetDef = false;

	while(ch != 0)
	{
		PwCharSet pwCurrentCharSet;
		bool bGenerateChar = false;

		if(ch == L'\\')
		{
			ch = csPattern.ReadChar();
			if(ch == 0) // Backslash at the end
			{
				PbgAppendChar(vOutBuffer, L'\\', dwOutBufPos);
				break;
			}

			if(bInCharSetDef) pwCustomCharSet.Add(ch);
			else
			{
				PbgAppendChar(vOutBuffer, ch, dwOutBufPos);
				pwUsedCharSet.Add(ch);
			}
		}
		else if(ch == L'^')
		{
			ch = csPattern.ReadChar();
			if(ch == 0) // ^ at the end
			{
				PbgAppendChar(vOutBuffer, L'^', dwOutBufPos);
				break;
			}

			if(bInCharSetDef) pwCustomCharSet.Remove(ch);
		}
		else if(ch == L'[')
		{
			pwCustomCharSet.Clear();
			bInCharSetDef = true;
		}
		else if(ch == L']')
		{
			pwCurrentCharSet.Add(pwCustomCharSet.ToString().c_str());

			bInCharSetDef = false;
			bGenerateChar = true;
		}
		else if(bInCharSetDef)
		{
			if(!pwCustomCharSet.AddCharSet(ch))
				pwCustomCharSet.Add(ch);
		}
		else if(!pwCurrentCharSet.AddCharSet(ch))
		{
			PbgAppendChar(vOutBuffer, ch, dwOutBufPos);
			pwUsedCharSet.Add(ch);
		}
		else bGenerateChar = true;
		
		if(bGenerateChar)
		{
			PwgPrepareCharSet(&pwCurrentCharSet, pSettings);

			if(pSettings->bNoRepeat == TRUE)
				pwCurrentCharSet.Remove(pwUsedCharSet.ToString().c_str());

			if(pwCurrentCharSet.Size() == 0) return PWGE_TOO_FEW_CHARACTERS;

			UINT64 uIndex = *(UINT64 *)&pbRandom[dwRandomPos];

			dwRandomPos += sizeof(UINT64);
			if(dwRandomPos == PBG_RANDOM_BUFFER_SIZE)
			{
				pRandomSource->GetRandomBuffer(pbRandom, PBG_RANDOM_BUFFER_SIZE);
				dwRandomPos = 0;
			}

			uIndex %= static_cast<UINT64>(pwCurrentCharSet.Size());

			WCHAR wch = pwCurrentCharSet.GetAt(static_cast<DWORD>(uIndex));
			PbgAppendChar(vOutBuffer, wch, dwOutBufPos);
			pwUsedCharSet.Add(wch);
		}

		ch = csPattern.ReadChar();
	}

	PbgAppendChar(vOutBuffer, 0, dwOutBufPos);
	PbgAppendChar(vOutBuffer, 0, dwOutBufPos);

	for(DWORD iRem = dwOutBufPos; iRem < vOutBuffer.size(); ++iRem)
	{
		vOutBuffer[iRem] = 0; // Set the rest to zero
	}

	if(pSettings->bPatternPermute != FALSE)
		PwgShufflePassword(vOutBuffer, pRandomSource);

	return PWGE_SUCCESS;
}

PbgWString PbgExpandPattern(const PbgWString& strPattern)
{
	PbgWString str = strPattern;

	while(true)
	{
		const int iOpen = SU_FindUnescapedCharW(str.c_str(), L'{');
		const int iClose = SU_FindUnescapedCharW(str.c_str(), L'}');

		if((iOpen < 0) || (iClose < iOpen)) break;

		const PbgWString::size_type uOpen = static_cast<PbgWString::size_type>(iOpen);
		const PbgWString::size_type uClose = static_cast<PbgWString::size_type>(iClose);

		PbgWString strCount = str.substr(uOpen + 1, uClose - uOpen - 1);
		str.erase(uOpen, uClose - uOpen + 1);

		const long lRepeat = _wtol(strCount.c_str());
		if((lRepeat >= 0) && (uOpen >= 1))
		{
			if(lRepeat == 0)
				str.erase(uOpen - 1, 1);
			else
				str.insert(uOpen, lRepeat - 1, str[uOpen - 1]);
		}
	}

	return str;
}

void PbgAppendChar(std::vector<WCHAR>& vOutBuffer, WCHAR wch, DWORD& rdwPos)
{
	if(rdwPos < vOutBuffer.size())
		vOutBuffer[rdwPos] = wch;
	else
		vOutBuffer.push_back(wch);

	++rdwPos;
}
