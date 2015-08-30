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

#include "StdAfx.h"
#include "../../KeePassLibCpp/PwManager.h"
#include "../../KeePassLibCpp/Crypto/KeyTransform.h"
#include "LibraryAPI.h"

KP_SHARE DWORD GetKeePassVersion()
{
	return PWM_VERSION_DW;
}

KP_SHARE LPCTSTR GetKeePassVersionString()
{
	return PWM_VERSION_STR;
}

KP_SHARE DWORD GetLibraryBuild()
{
	return KEEPASS_LIBRARY_BUILD;
}

KP_SHARE BOOL TransformKey256(UINT8* pBuffer256, const UINT8* pKeySeed256, UINT64 qwRounds)
{
	return (CKeyTransform::Transform256(qwRounds, pBuffer256, pKeySeed256) ? TRUE : FALSE);
}

KP_SHARE UINT64 TransformKeyBenchmark256(DWORD dwTimeMs)
{
	return CKeyTransform::Benchmark(dwTimeMs);
}
