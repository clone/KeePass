/*
  Copyright (c) 2008-2011, Dominik Reichl
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

#ifndef ___SYS_DEF_EX_H___
#define ___SYS_DEF_EX_H___

#pragma once

// The following types and functions must be implemented for each
// supported platform:

// Types:
// UINT8  -- 8-bit unsigned integer.
// UINT16 -- 16-bit unsigned integer.
// UINT32 -- 32-bit unsigned integer.
// UINT64 -- 64-bit unsigned integer.
// INT8   -- 8-bit unsigned integer.
// INT16  -- 16-bit unsigned integer.
// INT32  -- 32-bit unsigned integer.
// INT64  -- 64-bit unsigned integer.

// Defines:
// CPP_CLASS_SHARE -- Define to a keyword that imports or exports a C++ class.
// CPP_FN_SHARE    -- Define to a keyword that imports or exports a function
//                    that uses C++ types (class in parameters, STL type).
// C_FN_SHARE      -- Define to a keyword that imports or exports a function.
// KP_EXP          -- Define to the opposite of C_FN_SHARE.
// KP_API          -- Define to a function call type (STDCALL / CDECL).

// Include all system-specific definitions
#include "SysSpec_Windows/SysSpecWin.h"
// #include "SysSpec/SysSpecLinux.h"
#if defined _WIN32_WCE
#include "wce_wrap.h"
#endif

// Implications, standard macros and other definitions follow.

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(p) (void)0
#endif

#ifndef DWORD_MAX
#define DWORD_MAX 0xFFFFFFFF
#endif

#ifndef UINT32_MAX
#define UINT32_MAX 0xFFFFFFFF
#endif

#ifndef UINT64_MAX
#define UINT64_MAX 0xFFFFFFFFFFFFFFFFui64
#endif

#ifndef UTF8_BYTE
#define UTF8_BYTE BYTE
#endif

#ifndef ROTL32UE
#define ROTL32UE(__uv,__nv) ((((__uv) << (__nv)) | ((__uv) >> (32 - (__nv)))) & 0xFFFFFFFF)
#endif

// Safely delete pointers
#ifndef SAFE_DELETE
/// Delete the given object in the CRT of the caller.
/// Plugins: you can use this macro to safely delete objects that you allocated
/// with 'new', however do not use it to delete objects returned by KeePass
/// interface methods! For this, use IKpAPI::DeleteObject instead.
#define SAFE_DELETE(__kp_p)       { if((__kp_p) != NULL) { delete (__kp_p); (__kp_p) = NULL; } }

/// Delete the given array/string in the CRT of the caller.
/// Plugins: you can use this macro to safely delete arrays/strings that you allocated
/// with 'new', however do not use it to delete arrays/strings returned by KeePass
/// interface methods! For this, use IKpAPI::DeleteArray instead.
#define SAFE_DELETE_ARRAY(__kp_p) { if((__kp_p) != NULL) { delete [](__kp_p); (__kp_p) = NULL; } }

/// Safely release a COM interface.
/// If the given interface pointer is valid (i.e. non-NULL), the macro will call
/// the COM Release method of the object and set the interface pointer to NULL.
#define SAFE_RELEASE(__kp_p)      { if((__kp_p) != NULL) { (__kp_p)->Release(); (__kp_p) = NULL; } }
#endif

#ifndef KP_DEFINE_GUID
#define KP_DEFINE_GUID(__kp_name,__kp_l,__kp_w1,__kp_w2,__kp_b1,__kp_b2,__kp_b3,__kp_b4,__kp_b5,__kp_b6,__kp_b7,__kp_b8) \
	EXTERN_C const GUID DECLSPEC_SELECTANY __kp_name \
	= { __kp_l, __kp_w1, __kp_w2, { __kp_b1, __kp_b2, __kp_b3, __kp_b4, \
	__kp_b5, __kp_b6, __kp_b7, __kp_b8 } }
#endif

#ifndef KP_DECLSPEC_UUID
#if ((_MSC_VER >= 1100) && defined(__cplusplus))
#define KP_DECLSPEC_UUID(__kp_uuid) __declspec(uuid(__kp_uuid))
#else
#define KP_DECLSPEC_UUID(__kp_uuid)
#endif
#endif

#ifndef KP_DECLSPEC_NOVTABLE
#if ((_MSC_VER >= 1100) && defined(__cplusplus))
#define KP_DECLSPEC_NOVTABLE __declspec(novtable)
#else
#define KP_DECLSPEC_NOVTABLE
#endif
#endif

#ifndef KP_DECL_INTERFACE
#define KP_DECL_INTERFACE(__kp_if_uuid) KP_DECLSPEC_UUID(__kp_if_uuid) KP_DECLSPEC_NOVTABLE
#endif

#ifndef KP_DECL_IUNKNOWN
#define KP_DECL_IUNKNOWN STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject); \
	STDMETHODIMP_(ULONG) AddRef(); STDMETHODIMP_(ULONG) Release()
#endif

#ifndef KP_DECL_STDREFIMPL
#define KP_DECL_STDREFIMPL ULONG __m_uInterfaceRefCount
#endif

#ifndef KP_IMPL_STDREFIMPL
#define KP_IMPL_STDREFIMPL(__kp_class_name) STDMETHODIMP_(ULONG) \
	__kp_class_name##::AddRef() { return ++__m_uInterfaceRefCount; } \
	STDMETHODIMP_(ULONG) __kp_class_name##::Release() { \
	ULONG __kp_ref = __m_uInterfaceRefCount; \
	if(__m_uInterfaceRefCount != 0) { __kp_ref = --__m_uInterfaceRefCount; \
		if(__m_uInterfaceRefCount == 0) delete this; } \
	else { assert(false); } return __kp_ref; }
#endif

#ifndef KP_IMPL_STDREFIMPL_SUB
#define KP_IMPL_STDREFIMPL_SUB(__kp_class_name,__kp_sub_name) STDMETHODIMP_(ULONG) \
	__kp_class_name##::AddRef() { return ++__m_uInterfaceRefCount; } \
	STDMETHODIMP_(ULONG) __kp_class_name##::Release() { \
	ULONG __kp_ref = __m_uInterfaceRefCount; \
	if(__m_uInterfaceRefCount != 0) { __kp_ref = --__m_uInterfaceRefCount; \
		if(__m_uInterfaceRefCount == 0) { SAFE_DELETE(__kp_sub_name); delete this; } } \
	else { assert(false); } return __kp_ref; }
#endif

#ifndef KP_IMPL_STDREFIMPL_NODELETE
#define KP_IMPL_STDREFIMPL_NODELETE(__kp_class_name) STDMETHODIMP_(ULONG) \
	__kp_class_name##::AddRef() { return ++__m_uInterfaceRefCount; } \
	STDMETHODIMP_(ULONG) __kp_class_name##::Release() { \
	if(__m_uInterfaceRefCount != 0) --__m_uInterfaceRefCount; \
	else { assert(false); } return __m_uInterfaceRefCount; }
#endif

#ifndef KP_IMPL_CONSTRUCT
#define KP_IMPL_CONSTRUCT { __m_uInterfaceRefCount = 1; }
#endif

#ifndef KP_SUPPORT_INTERFACE
#define KP_SUPPORT_INTERFACE(__kp_iid,__kp_int) { assert(__m_uInterfaceRefCount > 0); \
	if(riid == __kp_iid) { __kp_int##* __kp_p = this; *ppvObject = __kp_p; \
	this->AddRef(); return S_OK; } }
#endif

/* #ifndef KP_IMPL_SUPPORT_INTERFACES2
#define KP_IMPL_SUPPORT_INTERFACES2(__kp_class,__kp_iid1,__kp_int1,__kp_iid2,__kp_int2) \
	STDMETHODIMP __kp_class##::QueryInterface(REFIID riid, void** ppvObject) { \
	KP_SUPPORT_INTERFACE(__kp_iid1, __kp_int1); \
	KP_SUPPORT_INTERFACE(__kp_iid2, __kp_int2); \
	*ppvObject = NULL; return E_NOINTERFACE; }
#endif */

#ifndef KP_ASSERT_REFS
#define KP_ASSERT_REFS(__kp_comobj,__kp_refcnt) ASSERT((__kp_comobj).AddRef() \
	> 0); ASSERT((__kp_comobj).Release() == (__kp_refcnt));
#endif

#ifndef KP_REQ_OUT_PTR
#define KP_REQ_OUT_PTR(__kp_ppv) { if((__kp_ppv) == NULL) { assert(false); \
	return E_POINTER; } }
#endif

#ifndef KP_REQ_OUT_PTR_INIT
#define KP_REQ_OUT_PTR_INIT(__kp_ppvx) { KP_REQ_OUT_PTR(__kp_ppvx); \
	*(__kp_ppvx) = NULL; }
#endif

// For use in CreateInstanceEx:
#ifndef KP_QUERY_INTERFACE_RELEASE_LOCAL_RET
#define KP_QUERY_INTERFACE_RELEASE_LOCAL_RET(__kp_comobj,__kp_riid,__kp_ppvout) { \
	void* __kp_ip = NULL; if((__kp_comobj)->QueryInterface((__kp_riid), &__kp_ip) == \
	S_OK) { *(__kp_ppvout) = __kp_ip; VERIFY((__kp_comobj)->Release() == 1); \
	return S_OK; } else { VERIFY((__kp_comobj)->Release() == 0); return E_NOINTERFACE; } }
#endif

#endif // ___SYS_DEF_EX_H___
