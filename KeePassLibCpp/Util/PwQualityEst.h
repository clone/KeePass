/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2014 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___PW_QUALITY_EST_H___
#define ___PW_QUALITY_EST_H___

#pragma once

#include "../SysDefEx.h"
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <tchar.h>
#include "StrUtil.h"
#include "MemUtil.h"

class CPwQualityEst : boost::noncopyable
{
private:
	CPwQualityEst();

public:
	static DWORD EstimatePasswordBits(LPCTSTR lpPassword);

private:
	static DWORD _EstimateQuality(LPCWSTR lpw);

	static void _EnsureInitialized();
};

#endif // ___PW_QUALITY_EST_H___
