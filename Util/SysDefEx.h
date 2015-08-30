/*
  Copyright (c) 2003-2005, Dominik Reichl <dominik.reichl@t-online.de>
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer. 
  - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  - Neither the name of ReichlSoft nor the names of its contributors may be
    used to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef ___SYS_DEF_EX_H___
#define ___SYS_DEF_EX_H___

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

// Disable export warnings
#pragma warning(disable: 4251)
#pragma warning(disable: 4275)

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(p) (void)0
#endif

#ifndef DWORD_MAX
#define DWORD_MAX 0xFFFFFFFF
#endif

#ifndef SHCNE_ASSOCCHANGED
#define SHCNE_ASSOCCHANGED 0x08000000L
#define SHCNF_IDLIST       0x0000
#endif
typedef void(WINAPI *LPSHCHANGENOTIFY)(LONG wEventId, UINT uFlags, LPCVOID dwItem1, LPCVOID dwItem2);

#endif
