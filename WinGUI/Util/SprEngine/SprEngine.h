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

#ifndef ___SPR_ENGINE_H___
#define ___SPR_ENGINE_H___

#pragma once

#include "../../../KeePassLibCpp/PwManager.h"

#define SPRE_MAX_DEPTH     12
#define SPRE_MAX_PATH_LEN 512

typedef struct _SPR_CONTENT_FLAGS
{
	bool bMakeAutoTypeSequence;
	bool bMakeCmdQuotes;
} SPR_CONTENT_FLAGS;

CString SprCompile(LPCTSTR lpText, bool bIsAutoTypeSequence, PW_ENTRY* pEntry,
	CPwManager* pDatabase, bool bEscapeForAutoType, bool bEscapeQuotesForCommandLine);

CString SprTransformContent(LPCTSTR lpContent, const SPR_CONTENT_FLAGS* pcf);

#endif // ___SPR_ENGINE_H___
