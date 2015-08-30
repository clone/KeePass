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

#ifndef ___PASSWORD_MANAGER_H___
#define ___PASSWORD_MANAGER_H___

#include <windows.h>
#include "../Util/NewRandom.h"

// General product information
#define PWM_PRODUCT_NAME "KeePass Password Safe"
#define PWM_VERSION_STR  "0.83b"

// The signature constants were chosen randomly
#define PWM_DBSIG_1      0x9AA2D903
#define PWM_DBSIG_2      0xB54BFB65
#define PWM_DBVER_DW     0x00010001

#define PWM_HOMEPAGE     "http://keepass.sourceforge.net"

// The executable name must be lowercase!
#define PWM_README_FILE  "KeePass.html"
#define PWM_LICENSE_FILE "License.html"

#define PWM_EXENAME      "KeePass"
#define PWMKEY_LANG      "KeeLanguage"
#define PWMKEY_CLIPSECS  "KeeClipboardSeconds"
#define PWMKEY_NEWLINE   "KeeNewLine"
#define PWMKEY_LASTDIR   "KeeLastDir"
#define PWMKEY_OPENLASTB "KeeAutoOpen"
#define PWMKEY_LASTDB    "KeeLastDb"
#define PWMKEY_IMGBTNS   "KeeImgButtons"

#define PWM_NUM_INITIAL_ENTRIES 256
#define PWM_NUM_INITIAL_GROUPS  32
#define PWM_PASSWORD_STRING     "********"

#define PWM_FLAG_SHA2           1
#define PWM_FLAG_RIJNDAEL       2
#define PWM_FLAG_ARCFOUR        4

#define PWM_SESSION_KEY_SIZE    12

// Field flags (for example in Find function)
#define PWMF_TITLE       1
#define PWMF_USER        2
#define PWMF_URL         4
#define PWMF_PASSWORD    8
#define PWMF_ADDITIONAL 16

#pragma pack(1)
typedef struct _PW_ENTRY
{
	DWORD uGroupId;
	DWORD uImageId;

	CHAR *pszTitle;
	CHAR *pszURL;
	CHAR *pszUserName;

	DWORD uPasswordLen;
	BYTE *pszPassword;

	CHAR *pszAdditional;
} PW_ENTRY;

typedef struct _PW_GROUP
{
	DWORD uImageId;
	char *pszGroupName;
} PW_GROUP;

typedef struct _PW_DBHEADER
{
	DWORD dwSignature1;
	DWORD dwSignature2;
	DWORD dwFlags;
	DWORD dwVersion;

	BYTE aMasterSeed[16];
	BYTE aEncryptionIV[16];

	DWORD dwGroups;
	DWORD dwEntries;
} PW_DBHEADER;
#pragma pack()

class CPwManager
{
public:
	CPwManager();
	virtual ~CPwManager();

	void CleanUp();

	BOOL SetMasterKey(const char *pszMasterKey, BOOL bDiskDrive, BOOL bCreate);

	DWORD GetNumberOfEntries();
	DWORD GetNumberOfGroups();

	int GetNumberOfItemsInGroup(const char *pszGroup);
	PW_ENTRY *GetEntry(DWORD dwIndex);
	PW_ENTRY *GetEntryByGroup(int idGroup, DWORD dwIndex);
	int GetEntryByGroupN(int idGroup, DWORD dwIndex);
	PW_GROUP *GetGroup(DWORD dwIndex);

	int GetGroupId(const char *pszGroupName);

	BOOL AddGroup(DWORD uImageId, const char *pszGroupName);
	BOOL AddEntry(DWORD uGroupId, DWORD uImageId, const char *pszTitle,
		const char *pszURL, const char *pszUserName, const char *pszPassword,
		const char *pszAdditional);
	BOOL DeleteEntry(DWORD dwIndex);
	BOOL DeleteGroup(int nGroupId);

	BOOL SetGroup(DWORD dwIndex, DWORD uImageId, const char *pszGroupName);
	BOOL SetEntry(DWORD dwIndex, DWORD uGroupId, DWORD uImageId,
		const char *pszTitle, const char *pszURL, const char *pszUserName,
		const char *pszPassword, const char *pszAdditional);

	void LockEntryPassword(PW_ENTRY *pEntry);
	void UnlockEntryPassword(PW_ENTRY *pEntry);

	void NewDatabase();
	BOOL OpenDatabase(const char *pszFile);
	BOOL SaveDatabase(const char *pszFile);

	void MoveInternal(int nFrom, int nTo);
	void MoveInGroup(int nGroup, int nFrom, int nTo);
	BOOL MoveGroup(int nFrom, int nTo);

	int Find(const char *pszFindString, BOOL bCaseSensitive, int fieldFlags, int nStart);

private:
	void _AllocEntries(unsigned long uEntries);
	void _DeleteEntryList(BOOL bFreeStrings);
	void _AllocGroups(unsigned long uGroups);
	void _DeleteGroupList(BOOL bFreeStrings);

	PW_ENTRY *m_pEntries;
	DWORD m_dwMaxEntries;
	DWORD m_dwNumEntries;

	PW_GROUP *m_pGroups;
	DWORD m_dwMaxGroups;
	DWORD m_dwNumGroups;

	CNewRandom m_random;

	BYTE m_pSessionKey[PWM_SESSION_KEY_SIZE];
	BYTE m_pMasterKey[32];
};

#endif
