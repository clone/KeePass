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

#ifndef ___KEEPASS_LIBRARY_API_H___
#define ___KEEPASS_LIBRARY_API_H___

#include "APIDefEx.h"

// Library build number (independent of underlying KeePass version)
#define KEEPASS_LIBRARY_BUILD 0x00000135

KP_SHARE DWORD GetKeePassVersion();
KP_SHARE LPCTSTR GetKeePassVersionString();

KP_SHARE DWORD GetLibraryBuild();

KP_SHARE BOOL TransformKey256(UINT8* pBuffer256, const UINT8* pKeySeed256, UINT64 qwRounds);
KP_SHARE UINT64 TransformKeyBenchmark256(DWORD dwTimeMs);

#endif
