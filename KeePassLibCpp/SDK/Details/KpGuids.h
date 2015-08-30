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

#ifndef ___KP_GUIDS_H___
#define ___KP_GUIDS_H___

#pragma once

#include "../../SysDefEx.h"

// {F023A34A-C6A9-41BA-AAF9-8D9EC329A435}
KP_DEFINE_GUID(IID_IKpAPI, 0xf023a34a, 0xc6a9, 0x41ba,
	0xaa, 0xf9, 0x8d, 0x9e, 0xc3, 0x29, 0xa4, 0x35);

// {5E6B4B05-4C29-407A-81D4-2BB0D866BA7F}
KP_DEFINE_GUID(IID_IKpAPI2, 0x5e6b4b05, 0x4c29, 0x407a,
	0x81, 0xd4, 0x2b, 0xb0, 0xd8, 0x66, 0xba, 0x7f);

// {25BA9CB9-DE60-4076-AAD9-ABC74663A872}
KP_DEFINE_GUID(IID_IKpAPI3, 0x25ba9cb9, 0xde60, 0x4076,
	0xaa, 0xd9, 0xab, 0xc7, 0x46, 0x63, 0xa8, 0x72);

// {6979084D-5E9D-43CB-8608-3AD878B8E829}
KP_DEFINE_GUID(CLSID_KpConfig, 0x6979084d, 0x5e9d, 0x43cb,
	0x86, 0x8, 0x3a, 0xd8, 0x78, 0xb8, 0xe8, 0x29);

// {F8B7DF13-C3EE-4F58-95E7-E931EB1FAA9F}
KP_DEFINE_GUID(CLSID_KpConfig_ReadOnly, 0xf8b7df13, 0xc3ee, 0x4f58,
	0x95, 0xe7, 0xe9, 0x31, 0xeb, 0x1f, 0xaa, 0x9f);

// {6F7B9713-FC6E-4FD6-BF55-044DF9E701B0}
KP_DEFINE_GUID(IID_IKpConfig, 0x6f7b9713, 0xfc6e, 0x4fd6,
	0xbf, 0x55, 0x4, 0x4d, 0xf9, 0xe7, 0x1, 0xb0);

// {BAD69DDB-2D69-4797-9D47-E177227F0E61}
KP_DEFINE_GUID(SCLSID_KpDatabase, 0xbad69ddb, 0x2d69, 0x4797,
	0x9d, 0x47, 0xe1, 0x77, 0x22, 0x7f, 0xe, 0x61);

// {29A5C55A-7100-4A1E-B74D-986C0E085AB2}
KP_DEFINE_GUID(IID_IKpDatabase, 0x29a5c55a, 0x7100, 0x4a1e,
	0xb7, 0x4d, 0x98, 0x6c, 0xe, 0x8, 0x5a, 0xb2);

// {F0A52511-81F0-4f3f-96CF-89B8D02CAC08}
KP_DEFINE_GUID(IID_IKpPlugin, 0xf0a52511, 0x81f0, 0x4f3f,
	0x96, 0xcf, 0x89, 0xb8, 0xd0, 0x2c, 0xac, 0x8);

// {011653D1-D7BC-429C-8632-BAA212F2BBC5}
KP_DEFINE_GUID(IID_IKpUnknown, 0x11653d1, 0xd7bc, 0x429c,
	0x86, 0x32, 0xba, 0xa2, 0x12, 0xf2, 0xbb, 0xc5);

// {20803389-8229-4dAF-808E-357D84D9BD90}
KP_DEFINE_GUID(SCLSID_KpUtilities, 0x20803389, 0x8229, 0x4daf,
	0x80, 0x8e, 0x35, 0x7d, 0x84, 0xd9, 0xbd, 0x90);

// {9700114E-9178-4337-A26F-34A6DA91FBB4}
KP_DEFINE_GUID(IID_IKpUtilities, 0x9700114e, 0x9178, 0x4337,
	0xa2, 0x6f, 0x34, 0xa6, 0xda, 0x91, 0xfb, 0xb4);

// {5B35E3F7-0890-4DB6-BC32-622B1F9AE6C5}
KP_DEFINE_GUID(SCLSID_KpCommandLine, 0x5b35e3f7, 0x890, 0x4db6,
	0xbc, 0x32, 0x62, 0x2b, 0x1f, 0x9a, 0xe6, 0xc5);

// {0934C282-6EDC-4073-913F-28B98C0420F8}
KP_DEFINE_GUID(IID_IKpCommandLine, 0x934c282, 0x6edc, 0x4073,
	0x91, 0x3f, 0x28, 0xb9, 0x8c, 0x4, 0x20, 0xf8);

// {78AC53D1-0595-4C31-A1FE-BA0846D04F02}
KP_DEFINE_GUID(CLSID_KpFullPathName, 0x78ac53d1, 0x595, 0x4c31,
	0xa1, 0xfe, 0xba, 0x8, 0x46, 0xd0, 0x4f, 0x2);

// {D55D9ADD-49DD-4704-BED1-539A5797B16C}
KP_DEFINE_GUID(IID_IKpFullPathName, 0xd55d9add, 0x49dd, 0x4704,
	0xbe, 0xd1, 0x53, 0x9a, 0x57, 0x97, 0xb1, 0x6c);

// {8F1C6151-F5BD-4024-B423-72328F97CBA0}
KP_DEFINE_GUID(CLSID_KpCommandLineOption, 0x8f1c6151, 0xf5bd, 0x4024,
	0xb4, 0x23, 0x72, 0x32, 0x8f, 0x97, 0xcb, 0xa0);

// {8831B009-0194-4919-A880-67AC170A5DF1}
KP_DEFINE_GUID(IID_IKpCommandLineOption, 0x8831b009, 0x194, 0x4919,
	0xa8, 0x80, 0x67, 0xac, 0x17, 0xa, 0x5d, 0xf1);

// {C465E854-6E22-42BF-A583-9A8DBE10E3A8}
KP_DEFINE_GUID(CLSID_KpFileTransaction, 0xc465e854, 0x6e22, 0x42bf,
	0xa5, 0x83, 0x9a, 0x8d, 0xbe, 0x10, 0xe3, 0xa8);

// {723A18EE-9769-4843-A79E-53B82DF5C9CA}
KP_DEFINE_GUID(IID_IKpFileTransaction, 0x723a18ee, 0x9769, 0x4843,
	0xa7, 0x9e, 0x53, 0xb8, 0x2d, 0xf5, 0xc9, 0xca);

#endif // ___KP_GUIDS_H___
