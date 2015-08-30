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

#include "StdAfx.h"
#include "PasswordGenerator.h"
#include "CharSetBasedGenerator.h"
#include "PatternBasedGenerator.h"

#include <algorithm>

#include "../Util/Base64.h"
#include "../Util/PwUtil.h"
#include "../Util/StrUtil.h"

PWG_ERROR PwgGenerateEx(std::vector<TCHAR>& vOutPassword,
	const PW_GEN_SETTINGS_EX* pSettings, CNewRandom* pRandomSource)
{
	ASSERT(pSettings != NULL);
	if(pSettings == NULL) return PWGE_NULL_PTR;

	EraseTCharVector(vOutPassword);

	CNewRandom* pAllocatedRandom = NULL;
	CNewRandom* pRandom = pRandomSource;

	if(pRandom == NULL)
	{
		pAllocatedRandom = new CNewRandom();
		pRandom = pAllocatedRandom;
	}

	std::vector<WCHAR> vOutBuffer;

	PWG_ERROR pwgErr = PWGE_UNKNOWN_GENERATOR;
	if(pSettings->btGeneratorType == PWGT_CHARSET)
		pwgErr = CsbgGenerate(vOutBuffer, pSettings, pRandom);
	else if(pSettings->btGeneratorType == PWGT_PATTERN)
	{
		// Fill output buffer with dummy password
		PbgGenerate(vOutBuffer, pSettings, pRandom);

		// Now generate the real password (this will overwrite
		// the characters in the buffer and won't do any memory
		// relocations, i.e. the final characters will be in
		// this place only and can be erased securely later)
		pwgErr = PbgGenerate(vOutBuffer, pSettings, pRandom);
	}

	if((vOutBuffer.size() == 0) || (std::find(vOutBuffer.begin(),
		vOutBuffer.end(), 0) == vOutBuffer.end()))
	{
		vOutBuffer.push_back(0); // Terminate string
		vOutBuffer.push_back(0);
	}

#ifdef _UNICODE
	vOutPassword.resize(vOutBuffer.size());
	for(DWORD dwCopy = 0; dwCopy < vOutBuffer.size(); ++dwCopy)
		vOutPassword[dwCopy] = vOutBuffer[dwCopy];
#else
	char *pFinalString = _StringToAnsi(&vOutBuffer[0]);
	vOutPassword.resize(szlen(pFinalString) + 1);
	for(DWORD dwCopy = 0; dwCopy <= szlen(pFinalString); ++dwCopy)
		vOutPassword[dwCopy] = pFinalString[dwCopy];
	mem_erase((unsigned char *)pFinalString, szlen(pFinalString));
	SAFE_DELETE_ARRAY(pFinalString);
#endif

	EraseWCharVector(vOutBuffer);
	SAFE_DELETE(pAllocatedRandom);
	return pwgErr;
}

WCHAR PwgGenerateCharacter(const PW_GEN_SETTINGS_EX* pSettings,
	CNewRandom* pRandom, PwCharSet* pCharSet)
{
	ASSERT(pSettings != NULL); if(pSettings == NULL) return 0;
	ASSERT(pRandom != NULL); if(pRandom == NULL) return 0;
	ASSERT(pCharSet != NULL); if(pCharSet == NULL) return 0;

	if(pCharSet->Size() == 0) return 0;

	ASSERT(sizeof(UINT64) == 8);
	UINT64 uIndex;
	pRandom->GetRandomBuffer((BYTE *)&uIndex, sizeof(UINT64));

	uIndex %= static_cast<UINT64>(pCharSet->Size());

	return pCharSet->GetAt(static_cast<unsigned int>(uIndex));
}

void PwgPrepareCharSet(PwCharSet* pCharSet, const PW_GEN_SETTINGS_EX* pSettings)
{
	ASSERT(pCharSet != NULL); if(pCharSet == NULL) return;
	ASSERT(pSettings != NULL); if(pSettings == NULL) return;

	pCharSet->Remove(PDCS_INVALID);

	if(pSettings->bNoConfusing == TRUE)
		pCharSet->Remove(PDCS_CONFUSING);
}

void PwgShufflePassword(std::vector<WCHAR>& vBuffer, CNewRandom* pRandom)
{
	ASSERT(pRandom != NULL); if(pRandom == NULL) return;

	DWORD dwLength = static_cast<DWORD>(vBuffer.size());

	if(dwLength <= 1) return; // Nothing to permute

	// Update length by finding the first 0 character
	for(DWORD dwScan = 0; dwScan < vBuffer.size(); ++dwScan)
	{
		if(vBuffer[dwScan] == 0)
		{
			dwLength = dwScan;
			break;
		}
	}

	ASSERT(sizeof(UINT64) == 8);
	UINT64 uRandomIndex;
	WCHAR wchTemp;
	for(DWORD dwSelect = 0; dwSelect < (dwLength - 1); ++dwSelect)
	{
		pRandom->GetRandomBuffer((BYTE *)&uRandomIndex, sizeof(UINT64));
		uRandomIndex %= (UINT64)(dwLength - dwSelect);

		ASSERT((dwSelect + (DWORD)uRandomIndex) < dwLength);

		wchTemp = vBuffer[dwSelect];
		vBuffer[dwSelect] = vBuffer[dwSelect + (DWORD)uRandomIndex];
		vBuffer[dwSelect + (DWORD)uRandomIndex] = wchTemp;
	}

	ASSERT(wcslen(&vBuffer[0]) == dwLength);
}

LPCTSTR PwgErrorToString(PWG_ERROR uError)
{
	if(uError == PWGE_SUCCESS) return _T("Success");
	if(uError == PWGE_NULL_PTR) return _T("Internal error");
	if(uError == PWGE_UNKNOWN_GENERATOR) return _T("Internal error");
	if(uError == PWGE_TOO_FEW_CHARACTERS)
		return _T("There are too few characters in the character set to build up a password matching the specified rules");

	return _T("Unknown error");
}

/* std::basic_string<WCHAR> HexStrToWCharStr(LPCTSTR lpString)
{
	std::basic_string<WCHAR> str;

	ASSERT(lpString != NULL); if(lpString == NULL) return str;

	DWORD dwLength = _tcslen(lpString), i = 0;
	if((dwLength & 3) != 0) { ASSERT(FALSE); return str; }

	BYTE bt1, bt2;

	while(true)
	{
		TCHAR ch1 = lpString[i], ch2 = lpString[i + 1];
		TCHAR ch3 = lpString[i + 2], ch4 = lpString[i + 3];

		ConvertStrToHex(ch1, ch2, bt1);
		ConvertStrToHex(ch3, ch4, bt2);

		str += (WCHAR)((((WCHAR)bt1) << 8) | ((WCHAR)bt2));

		i += 4;
		if(lpString[i] == 0) break;
	}

	return str;
}

std::basic_string<TCHAR> WCharVecToHexStr(const std::vector<WCHAR>& vec)
{
	std::basic_string<TCHAR> strOut;
	TCHAR ch1, ch2;

	for(DWORD i = 0; i < vec.size(); ++i)
	{
		ConvertHexToStr((BYTE)(vec[i] >> 8), ch1, ch2);
		strOut += ch1;
		strOut += ch2;

		ConvertHexToStr((BYTE)(vec[i] & 0xFF), ch1, ch2);
		strOut += ch1;
		strOut += ch2;
	}

	return strOut;
} */

std::basic_string<TCHAR> PwgProfileToString(const PW_GEN_SETTINGS_EX* pSettings)
{
	std::basic_string<TCHAR> strEmpty;

	ASSERT(pSettings != NULL);
	if(pSettings == NULL) return strEmpty;

	std::vector<BYTE> s;

	s.push_back(PWGD_VERSION_BYTE);

	UTF8_BYTE *pbName = _StringToUTF8(pSettings->strName.c_str());
	UTF8_BYTE *pb = pbName;
	while(*pb != 0) { s.push_back(*pb); ++pb; }
	s.push_back(0);
	SAFE_DELETE_ARRAY(pbName);

	s.push_back(pSettings->btGeneratorType);
	s.push_back((pSettings->bCollectUserEntropy == TRUE) ? (BYTE)'U' : (BYTE)'N');
	s.push_back((BYTE)((pSettings->dwLength >> 24) & 0xFF));
	s.push_back((BYTE)((pSettings->dwLength >> 16) & 0xFF));
	s.push_back((BYTE)((pSettings->dwLength >> 8) & 0xFF));
	s.push_back((BYTE)(pSettings->dwLength & 0xFF));

	PwCharSet pcs(pSettings->strCharSet.c_str());
	USHORT usFlags = pcs.PackAndRemoveCharRanges();
	s.push_back((BYTE)((usFlags >> 8) & 0xFF));
	s.push_back((BYTE)(usFlags & 0xFF));

	std::basic_string<WCHAR> strRemChars = pcs.ToString();
	for(unsigned int uCS = 0; uCS < strRemChars.size(); ++uCS)
	{
		s.push_back((BYTE)(strRemChars[uCS] >> 8));
		s.push_back((BYTE)(strRemChars[uCS] & 0xFF));
	}
	s.push_back(0);
	s.push_back(0);

	for(unsigned int uPat = 0; uPat < pSettings->strPattern.size(); ++uPat)
	{
		s.push_back((BYTE)(pSettings->strPattern[uPat] >> 8));
		s.push_back((BYTE)(pSettings->strPattern[uPat] & 0xFF));
	}
	s.push_back(0);
	s.push_back(0);

	s.push_back((BYTE)((pSettings->bNoConfusing == TRUE) ? 'N' : 'A'));
	s.push_back((BYTE)((pSettings->bPatternPermute == TRUE) ? 'P' : 'N'));

	DWORD dwOutSize = static_cast<DWORD>(s.size() * 4 + 12);
	BYTE *pBase64 = new BYTE[dwOutSize];
	if(CBase64Codec::Encode(&s[0], (DWORD)s.size(), pBase64, &dwOutSize) == false)
	{
		ASSERT(FALSE);
		return strEmpty;
	}

#ifdef _UNICODE
	TCHAR *lpFinal = _StringToUnicode((char *)pBase64);
#else
	TCHAR *lpFinal = (TCHAR *)pBase64;
#endif

	std::basic_string<TCHAR> strFinal = lpFinal;

	SAFE_DELETE_ARRAY(pBase64);
#ifdef _UNICODE
	SAFE_DELETE_ARRAY(lpFinal);
#endif

	return strFinal;
}

void PwgStringToProfile(const std::basic_string<TCHAR>& strProfile,
	PW_GEN_SETTINGS_EX* s)
{
	ASSERT(s != NULL); if(s == NULL) return;

#ifdef _UNICODE
	const char *lpEncoded = _StringToAnsi(strProfile.c_str());
#else
	const char *lpEncoded = strProfile.c_str();
#endif

	DWORD dwDecodedSize = static_cast<DWORD>(strProfile.size() + 120);
	BYTE *pDecoded = new BYTE[dwDecodedSize];
	memset(pDecoded, 0, dwDecodedSize);

	if(CBase64Codec::Decode((BYTE *)lpEncoded, szlen(lpEncoded), pDecoded,
		&dwDecodedSize) == false) { ASSERT(FALSE); return; }

	ASSERT(pDecoded[0] <= PWGD_VERSION_BYTE);

	TCHAR *lpName = _UTF8ToString(&pDecoded[1]);

	s->strName = lpName;

	BYTE *pb = (BYTE *)memchr(pDecoded, 0, dwDecodedSize);
	if(pb == NULL) { ASSERT(FALSE); return; }

	++pb;
	s->btGeneratorType = *pb; ++pb;
	s->bCollectUserEntropy = ((*pb == (BYTE)'U') ? TRUE : FALSE); ++pb;

	s->dwLength = (DWORD)(*pb) << 24; ++pb;
	s->dwLength |= (DWORD)(*pb) << 16; ++pb;
	s->dwLength |= (DWORD)(*pb) << 8; ++pb;
	s->dwLength |= (DWORD)(*pb); ++pb;

	USHORT usFlags = (USHORT)(*pb) << 8; ++pb;
	usFlags |= (USHORT)(*pb); ++pb;

	PwCharSet pcs;
	pcs.UnpackCharRanges(usFlags);

	while(true)
	{
		BYTE bt1 = *pb; ++pb;
		BYTE bt2 = *pb; ++pb;
		if((bt1 == 0) && (bt2 == 0)) break;
		pcs.Add(((WCHAR)bt1 << 8) | (WCHAR)bt2);
	}
	s->strCharSet = pcs.ToString();

	while(true)
	{
		BYTE bt1 = *pb; ++pb;
		BYTE bt2 = *pb; ++pb;
		if((bt1 == 0) && (bt2 == 0)) break;
		s->strPattern += (WCHAR)(((WCHAR)bt1 << 8) | (WCHAR)bt2);
	}

	ASSERT((*pb == (BYTE)'N') || (*pb == (BYTE)'A'));
	s->bNoConfusing = ((*pb == (BYTE)'N') ? TRUE : FALSE); ++pb;
	ASSERT((*pb == (BYTE)'P') || (*pb == (BYTE)'N') || (*pb == 0));
	s->bPatternPermute = ((*pb == (BYTE)'P') ? TRUE : FALSE);

	SAFE_DELETE_ARRAY(lpName);
	SAFE_DELETE_ARRAY(pDecoded);
}

void PwgGetDefaultProfile(PW_GEN_SETTINGS_EX* s)
{
	ASSERT(s != NULL); if(s == NULL) return;

	s->btGeneratorType = PWGT_CHARSET;
	s->dwLength = 20;

	PwCharSet pcs;
	pcs.Add(PDCS_UPPER_CASE, PDCS_LOWER_CASE, PDCS_NUMERIC);
	s->strCharSet = pcs.ToString();

	s->bCollectUserEntropy = FALSE;
	s->bNoConfusing = FALSE;

	s->bPatternPermute = FALSE;
}
