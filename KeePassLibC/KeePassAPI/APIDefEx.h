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

#ifndef ___KEEPASS_API_EX_H___
#define ___KEEPASS_API_EX_H___

#ifndef KP_IMPORT_LIB
#define KP_SHARE extern "C" __declspec(dllexport)
#else
#define KP_SHARE extern "C" __declspec(dllimport)
#endif

#define DECL_MFC_INIT AFX_MANAGE_STATE(AfxGetStaticModuleState())

#define DECL_MGR(__p) DECL_MFC_INIT; CPwManager *p = (CPwManager *)(__p)
#define DECL_MGR_V(__p) DECL_MGR(__p); if(p == NULL) return
#define DECL_MGR_N(__p) DECL_MGR(__p); if(p == NULL) return 0
#define DECL_MGR_P(__p) DECL_MGR(__p); if(p == NULL) return NULL
#define DECL_MGR_B(__p) DECL_MGR_N(__p)

#endif
