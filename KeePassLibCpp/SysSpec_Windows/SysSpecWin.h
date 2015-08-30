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

#ifndef ___SYS_SPEC_WIN_H___
#define ___SYS_SPEC_WIN_H___

#pragma once

#if (defined(_WIN32) || defined(_WIN64))

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN // Exclude rarely-used stuff from Windows headers
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#ifndef KP_NO_DEFAULT_SHARE

#ifdef CPP_CLASS_SHARE
#error CPP_CLASS_SHARE must not be defined.
#else
#ifdef COMPILE_DLL_EX
#define CPP_CLASS_SHARE __declspec(dllimport)
#else
#define CPP_CLASS_SHARE __declspec(dllexport)
#endif
#endif

#ifdef CPP_FN_SHARE
#error CPP_FN_SHARE must not be defined.
#else
#ifdef COMPILE_DLL_EX
#define CPP_FN_SHARE __declspec(dllimport)
#else
#define CPP_FN_SHARE __declspec(dllexport)
#endif
#endif

#ifdef C_FN_SHARE
#error C_FN_SHARE must not be defined.
#else
#ifdef COMPILE_DLL_EX
#define C_FN_SHARE extern "C" __declspec(dllimport)
#else
#define C_FN_SHARE extern "C" __declspec(dllexport)
#endif
#endif

#ifndef KP_EXP
#ifdef COMPILE_DLL_EX
#define KP_EXP extern "C" __declspec(dllexport)
#else
#define KP_EXP extern "C" __declspec(dllimport)
#endif
#endif

#ifndef KP_API
#define KP_API __cdecl
#endif

#else // KP_NO_DEFAULT_SHARE

#define CPP_CLASS_SHARE
#define CPP_FN_SHARE
#define KP_EXP
#define KP_API

#ifdef KP_FORCE_C_FN_SHARE
#define C_FN_SHARE extern "C" __declspec(dllexport)
#else
#define C_FN_SHARE
#endif

#endif // !KP_NO_DEFAULT_SHARE

// Warning C4251: Class 'CArray<TYPE,ARG_TYPE>' requires a DLL interface that will be used by the clients of the class
#pragma warning(disable: 4251)
// Warning C4275: Class 'CClass' isn't a DLL interface and has been declared
// as base class for the DLL interface class 'CClass2'
#pragma warning(disable: 4275)

#endif // (defined(_WIN32) || defined(_WIN64))
#endif // !___SYS_SPEC_WIN_H___
