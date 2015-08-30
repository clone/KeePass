/*
  Copyright (c) 2008-2014, Dominik Reichl
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
	the documentation and/or other materials provided with the
	distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef ___SYS_SPEC_WIN_H___
#define ___SYS_SPEC_WIN_H___

#pragma once

#if (defined(_WIN32) || defined(_WIN64))

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

// #ifndef KP_NO_DEFAULT_SHARE

// #ifdef CPP_CLASS_SHARE
// #error CPP_CLASS_SHARE must not be defined.
// #else
// #ifdef COMPILE_DLL_EX
// #define CPP_CLASS_SHARE __declspec(dllimport)
// #else
// #define CPP_CLASS_SHARE __declspec(dllexport)
// #endif
// #endif

// #ifdef CPP_FN_SHARE
// #error CPP_FN_SHARE must not be defined.
// #else
// #ifdef COMPILE_DLL_EX
// #define CPP_FN_SHARE __declspec(dllimport)
// #else
// #define CPP_FN_SHARE __declspec(dllexport)
// #endif
// #endif

// #ifdef C_FN_SHARE
// #error C_FN_SHARE must not be defined.
// #else
// #ifdef COMPILE_DLL_EX
// #define C_FN_SHARE extern "C" __declspec(dllimport)
// #else
// #define C_FN_SHARE extern "C" __declspec(dllexport)
// #endif
// #endif

// #ifndef KP_EXP
// #ifdef COMPILE_DLL_EX
// #define KP_EXP extern "C" __declspec(dllexport)
// #else
// #define KP_EXP extern "C" __declspec(dllimport)
// #endif
// #endif

#ifndef KP_EXPORT
#define KP_EXPORT extern "C" __declspec(dllexport)
#endif

#ifndef KP_API
#define KP_API __cdecl
#endif

// #else // KP_NO_DEFAULT_SHARE

// #define CPP_CLASS_SHARE
// #define CPP_FN_SHARE
// #define KP_EXP
// #define KP_API

// #ifdef KP_FORCE_C_FN_SHARE
// #define C_FN_SHARE extern "C" __declspec(dllexport)
// #else
// #define C_FN_SHARE
// #endif

// #endif // !KP_NO_DEFAULT_SHARE

// Warning C4251: Class 'CArray<TYPE,ARG_TYPE>' requires a DLL interface that will be used by the clients of the class
// #pragma warning(disable: 4251)

// Warning C4275: Class 'CClass' isn't a DLL interface and has been declared
// as base class for the DLL interface class 'CClass2'
// #pragma warning(disable: 4275)

#endif // (defined(_WIN32) || defined(_WIN64))
#endif // !___SYS_SPEC_WIN_H___
