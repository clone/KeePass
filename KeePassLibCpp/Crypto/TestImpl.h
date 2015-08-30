/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2008 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___TEST_IMPL_H___
#define ___TEST_IMPL_H___

#pragma once

#include "../SysDefEx.h"

#define TI_ERR_SHAVAR32          1
#define TI_ERR_SHAVAR64          2
#define TI_ERR_SHACMP256         4
#define TI_ERR_SHACMP512         8
#define TI_ERR_RIJNDAEL_ENCRYPT 16
#define TI_ERR_RIJNDAEL_DECRYPT 32
#define TI_ERR_ARCFOUR_CRYPT    64
#define TI_ERR_TWOFISH         128
#define TI_ERR_UINT_TYPE       256
#define TI_ERR_INT_TYPE        512

UINT32 TestCryptoImpl();
UINT32 TestTypeDefs();

#endif
