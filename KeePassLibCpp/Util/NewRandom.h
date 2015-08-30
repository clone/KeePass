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

#ifndef ___NEW_RANDOM_H___
#define ___NEW_RANDOM_H___

#include "../SysDefEx.h"
#include "../Crypto/SHA2/SHA2.h"
#include <vector>

class CPP_CLASS_SHARE CNewRandom
{
public:
	CNewRandom();
	virtual ~CNewRandom();

	void Reset();
	void Initialize();

	void GetRandomBuffer(__out_bcount(dwSize) BYTE *pBuf, DWORD dwSize);

private:
	void AddRandomObject(__in_bcount(uSize) const void *pObj, size_t uSize);

	std::vector<BYTE> m_vPseudoRandom;
	DWORD m_dwCounter;
};

class CPP_CLASS_SHARE CNewRandomInterface
{
public:
	virtual ~CNewRandomInterface() { }

	virtual BOOL GenerateRandomSequence(unsigned long uRandomSeqSize, unsigned char *pBuffer) const = 0;
};

// Seed is 128 bits = 4 32-bit DWORDS
void srandXorShift(unsigned long *pSeed128);
unsigned long randXorShift();

// Must be able to hold at least 16 bytes
CPP_FN_SHARE void randCreateUUID(BYTE *pUUID16, CNewRandom *pRandomSource);

#endif
