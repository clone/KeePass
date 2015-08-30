/*
  Copyright (c) 2003, Dominik Reichl <dominik.reichl@t-online.de>
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

#include "StdAfx.h"
#include "testimpl.h"

#include "sha2.h"
#include "rijndael.h"
#include "arcfour.h"

static const char g_szVectABCX[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
static const unsigned char g_uVectABCX_SHA256[32] =
{
	0xDB, 0x4B, 0xFC, 0xBD, 0x4D, 0xA0, 0xCD, 0x85,
	0xA6, 0x0C, 0x3C, 0x37, 0xD3, 0xFB, 0xD8, 0x80,
	0x5C, 0x77, 0xF1, 0x5F, 0xC6, 0xB1, 0xFD, 0xFE,
	0x61, 0x4E, 0xE0, 0xA7, 0xC8, 0xFD, 0xB4, 0xC0
};
static const unsigned char g_uVectABCX_SHA512[64] =
{
	0x1E, 0x07, 0xBE, 0x23, 0xC2, 0x6A, 0x86, 0xEA,
	0x37, 0xEA, 0x81, 0x0C, 0x8E, 0xC7, 0x80, 0x93,
	0x52, 0x51, 0x5A, 0x97, 0x0E, 0x92, 0x53, 0xC2,
	0x6F, 0x53, 0x6C, 0xFC, 0x7A, 0x99, 0x96, 0xC4,
	0x5C, 0x83, 0x70, 0x58, 0x3E, 0x0A, 0x78, 0xFA,
	0x4A, 0x90, 0x04, 0x1D, 0x71, 0xA4, 0xCE, 0xAB,
	0x74, 0x23, 0xF1, 0x9C, 0x71, 0xB9, 0xD5, 0xA3,
	0xE0, 0x12, 0x49, 0xF0, 0xBE, 0xBD, 0x58, 0x94
};

static const char g_szVectABCXPM[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";
static const unsigned char g_uVectABCXPM_SHA256[32] =
{
	0x3B, 0xD1, 0xF4, 0x34, 0x2B, 0xDF, 0x37, 0xA9,
	0x07, 0xC2, 0x46, 0xD6, 0x35, 0x09, 0x07, 0x3F,
	0x83, 0x59, 0x5A, 0x87, 0xB4, 0xA8, 0x96, 0x11,
	0x11, 0x4B, 0xFE, 0x38, 0xD4, 0xC7, 0xB8, 0x67
};
static const unsigned char g_uVectABCXPM_SHA512[64] =
{
	0x68, 0x16, 0x51, 0x3A, 0x4C, 0x3A, 0x8B, 0x45,
	0xCB, 0x1F, 0xB1, 0xB3, 0xFE, 0x43, 0x35, 0xAA,
	0x87, 0x9E, 0x82, 0x93, 0x67, 0x1E, 0xAE, 0xCD,
	0x00, 0x0D, 0xD1, 0x84, 0xDF, 0xE6, 0xEA, 0xFB,
	0x8B, 0x89, 0x16, 0x4D, 0x8D, 0xC7, 0xB5, 0x1F,
	0x4F, 0xE5, 0x49, 0x69, 0x18, 0x7C, 0x9C, 0x65,
	0x03, 0xBC, 0x76, 0x1F, 0x84, 0xCC, 0x51, 0x2B,
	0x57, 0xAF, 0x7E, 0x94, 0x3D, 0x21, 0xF1, 0x8F
};

static const unsigned char g_uVectRijndaelZero[16] =
{
	0xdc, 0x95, 0xc0, 0x78, 0xa2, 0x40, 0x89, 0x89,
	0xad, 0x48, 0xa2, 0x14, 0x92, 0x84, 0x20, 0x87
};

static const unsigned char g_uVectRijndaelZeroDec[16] =
{
	0x67, 0x67, 0x1c, 0xe1, 0xfa, 0x91, 0xdd, 0xeb,
	0x0f, 0x8f, 0xbb, 0xb3, 0x66, 0xb5, 0x31, 0xb4
};

unsigned long testCryptoImpl()
{
	unsigned long uTestMask;
	sha2_32t sha32;
	sha2_64t sha64;
	sha256_ctx hash256;
	sha512_ctx hash512;
	RD_UINT8 aHash[128];
	RD_UINT8 aTemp[128];
	RD_UINT8 aTemp2[128];
	Rijndael rdcrypt;
	int i, j;

	uTestMask = 0;

	sha32 = n_u32(ffffffff);
	sha32++;
	if(sha32 != 0) uTestMask |= TI_ERR_SHAVAR32;
	sha32 = 0;
	sha32--;
	if(sha32 != n_u32(ffffffff)) uTestMask |= TI_ERR_SHAVAR32;

	sha64 = n_u64(ffffffffffffffff);
	sha64++;
	if(sha64 != 0) uTestMask |= TI_ERR_SHAVAR64;
	sha64 = 0;
	sha64--;
	if(sha64 != n_u64(ffffffffffffffff)) uTestMask |= TI_ERR_SHAVAR64;

	sha256_begin(&hash256);
	sha256_hash((const unsigned char *)g_szVectABCX,
		strlen(g_szVectABCX), &hash256);
	sha256_end((unsigned char *)aHash, &hash256);
	if(memcmp(aHash, g_uVectABCX_SHA256, 32) != 0)
		uTestMask |= TI_ERR_SHACMP256;

	sha512_begin(&hash512);
	sha512_hash((const unsigned char *)g_szVectABCX,
		strlen(g_szVectABCX), &hash512);
	sha512_end((unsigned char *)aHash, &hash512);
	if(memcmp(aHash, g_uVectABCX_SHA512, 64) != 0)
		uTestMask |= TI_ERR_SHACMP512;

	sha256_begin(&hash256);
	sha256_hash((const unsigned char *)g_szVectABCXPM,
		strlen(g_szVectABCXPM), &hash256);
	sha256_end((unsigned char *)aHash, &hash256);
	if(memcmp(aHash, g_uVectABCXPM_SHA256, 32) != 0)
		uTestMask |= TI_ERR_SHACMP256;

	sha512_begin(&hash512);
	sha512_hash((const unsigned char *)g_szVectABCXPM,
		strlen(g_szVectABCXPM), &hash512);
	sha512_end((unsigned char *)aHash, &hash512);
	if(memcmp(aHash, g_uVectABCXPM_SHA512, 64) != 0)
		uTestMask |= TI_ERR_SHACMP512;

	if(rdcrypt.init(Rijndael::CBC, Rijndael::Encrypt, aHash,
		Rijndael::Key32Bytes, aHash) != RIJNDAEL_SUCCESS)
	{
		uTestMask |= TI_ERR_RIJNDAEL_ENCRYPT;
	}
	else
	{
		// 128 is the size of the buffer, 32 the key and 16 the pad space
		i = rdcrypt.padEncrypt(aHash + 32, 128 - 32 - 16, aTemp);
		ASSERT(i != 0);

		if(rdcrypt.init(Rijndael::CBC, Rijndael::Decrypt, aHash,
			Rijndael::Key32Bytes, aHash) != RIJNDAEL_SUCCESS)
		{
			uTestMask |= TI_ERR_RIJNDAEL_DECRYPT;
		}
		else
		{
			j = rdcrypt.padDecrypt(aTemp, i, aTemp2);

			if(j != (128 - 32 - 16)) uTestMask |= TI_ERR_RIJNDAEL_DECRYPT;

			// Test if the buffer has been correctly decrypted
			if(memcmp(aHash + 32, aTemp2, 128 - 32 - 16) != 0)
				uTestMask |= TI_ERR_RIJNDAEL_DECRYPT;

			// Test if the Rijndael has encrypted the buffer
			if(memcmp(aHash + 32, aTemp, 128 - 32 - 16) == 0)
				uTestMask |= TI_ERR_RIJNDAEL_ENCRYPT;
		}
	}

	memset(aHash, 0, 32); // Key of zeros
	if(rdcrypt.init(Rijndael::ECB, Rijndael::Encrypt, aHash,
		Rijndael::Key32Bytes, NULL) != RIJNDAEL_SUCCESS)
	{
		uTestMask |= TI_ERR_RIJNDAEL_ENCRYPT;
	}
	else
	{
		rdcrypt.blockEncrypt(aHash, 128, aTemp);
		if(memcmp(aTemp, g_uVectRijndaelZero, 16) != 0)
			uTestMask |= TI_ERR_RIJNDAEL_ENCRYPT;

	}

	memset(aHash, 0, 32); // Key of zeros
	if(rdcrypt.init(Rijndael::ECB, Rijndael::Decrypt, aHash,
		Rijndael::Key32Bytes, NULL) != RIJNDAEL_SUCCESS)
	{
		uTestMask |= TI_ERR_RIJNDAEL_DECRYPT;
	}
	else
	{
		rdcrypt.blockDecrypt(aHash, 128, aTemp);
		if(memcmp(aTemp, g_uVectRijndaelZeroDec, 16) != 0)
			uTestMask |= TI_ERR_RIJNDAEL_DECRYPT;
	}

	memset(aHash, 0, 32);
	memset(aTemp, 0, 32);
	memset(aTemp2, 0, 32);
	arcfourCrypt((BYTE *)aTemp, 32, (BYTE *)"abcdef", 6);
	arcfourCrypt((BYTE *)aTemp2, 32, (BYTE *)"abcdef", 6);
	arcfourCrypt((BYTE *)aTemp2, 32, (BYTE *)"abcdef", 6);
	if(memcmp(aHash, aTemp2, 32) != 0) uTestMask |= TI_ERR_ARCFOUR_CRYPT;
	if(memcmp(aHash, aTemp, 32) == 0) uTestMask |= TI_ERR_ARCFOUR_CRYPT;

	// A return value of zero means no error
	return uTestMask;
}
