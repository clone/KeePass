/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2012 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___PW_CHARSET_H___
#define ___PW_CHARSET_H___

#include <vector>
#include <tchar.h>

#define PDCS_UPPER_CASE       L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define PDCS_LOWER_CASE       L"abcdefghijklmnopqrstuvwxyz"
#define PDCS_NUMERIC          L"0123456789"
#define PDCS_UPPER_CONSONANTS L"BCDFGHJKLMNPQRSTVWXYZ"
#define PDCS_LOWER_CONSONANTS L"bcdfghjklmnpqrstvwxyz"
#define PDCS_UPPER_VOWELS     L"AEIOU"
#define PDCS_LOWER_VOWELS     L"aeiou"
#define PDCS_PUNCTUATION      L",.;:"
#define PDCS_BRACKETS         L"[]{}()<>"
#define PDCS_PRINTASCIISPEC   L"!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"
#define PDCS_LOWER_HEX        L"0123456789abcdef"
#define PDCS_UPPER_HEX        L"0123456789ABCDEF"

#define PDCS_INVALID          L"\t\r\n"
#define PDCS_CONFUSING        L"O0l1I|"

#define PCS_TABSIZE (0x10000 / 8)

class PwCharSet
{
public:
	PwCharSet();
	PwCharSet(LPCWSTR lpCharacters);
	~PwCharSet();

	void Clear();

	unsigned int Size() const;

	bool Contains(WCHAR ch) const;
	bool Contains(LPCWSTR lpCharacters) const;

	WCHAR GetAt(unsigned int uPos) const;

	void Add(WCHAR ch);
	void Add(LPCWSTR lpCharacters);
	void Add(LPCWSTR lpChars1, LPCWSTR lpChars2, LPCWSTR lpChars3 = NULL);

	void AddRange(WCHAR chLow, WCHAR chHigh);

	bool AddCharSet(WCHAR chCharSetIdentifier);

	bool Remove(WCHAR ch);
	bool Remove(LPCWSTR lpCharacters);

	bool RemoveIfAllExist(LPCWSTR lpCharacters);

	std::basic_string<WCHAR> ToString() const;

	USHORT PackAndRemoveCharRanges();
	void UnpackCharRanges(USHORT usRanges);

	static PwCharSet GetSpecialChars();
	static PwCharSet GetHighAnsiChars();

private:
	std::vector<WCHAR> m_vChars;
	BYTE m_vTab[PCS_TABSIZE];
};

#endif // ___PW_CHARSET_H___
