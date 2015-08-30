/*
  Copyright (c) 2003/2004, Dominik Reichl <dominik.reichl@t-online.de>
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

#include "../Util/NewRandom.h"
#include "../Crypto/rijndael.h"

// General product information
#define PWM_PRODUCT_NAME _T("KeePass Password Safe")
#define PWM_VERSION_STR  _T("0.86")

// The signature constants were chosen randomly
#define PWM_DBSIG_1      0x9AA2D903
#define PWM_DBSIG_2      0xB54BFB65
#define PWM_DBVER_DW     0x00010001

#define PWM_HOMEPAGE     _T("http://keepass.sourceforge.net")
#define PWM_URL_TRL      _T("http://keepass.sourceforge.net/translations.php")

#define PWM_README_FILE  _T("KeePass.html")
#define PWM_LICENSE_FILE _T("License.html")

#define PWM_EXENAME      _T("KeePass")
#define PWMKEY_LANG      _T("KeeLanguage")
#define PWMKEY_CLIPSECS  _T("KeeClipboardSeconds")
#define PWMKEY_NEWLINE   _T("KeeNewLine")
#define PWMKEY_LASTDIR   _T("KeeLastDir")
#define PWMKEY_OPENLASTB _T("KeeAutoOpen")
#define PWMKEY_LASTDB    _T("KeeLastDb")
#define PWMKEY_AUTOSAVEB _T("KeeAutoSave")
#define PWMKEY_IMGBTNS   _T("KeeImgButtons")
#define PWMKEY_ENTRYGRID _T("KeeEntryGrid")
#define PWMKEY_ALWAYSTOP _T("KeeAlwaysOnTop")
#define PWMKEY_SHOWTITLE _T("KeeShowTitle")
#define PWMKEY_SHOWUSER  _T("KeeShowUser")
#define PWMKEY_SHOWURL   _T("KeeShowURL")
#define PWMKEY_SHOWPASS  _T("KeeShowPassword")
#define PWMKEY_SHOWNOTES _T("KeeShowNotes")
#define PWMKEY_HIDESTARS _T("KeeHideStars")
#define PWMKEY_LISTFONT  _T("KeeListFont")
#define PWMKEY_WINDOWPX  _T("KeeWindowPX")
#define PWMKEY_WINDOWPY  _T("KeeWindowPY")
#define PWMKEY_WINDOWDX  _T("KeeWindowDX")
#define PWMKEY_WINDOWDY  _T("KeeWindowDY")
#define PWMKEY_SCREENID  _T("KeeLastScreenId")
#define PWMKEY_COLWIDTH0 _T("KeeColumnWidth0")
#define PWMKEY_COLWIDTH1 _T("KeeColumnWidth1")
#define PWMKEY_COLWIDTH2 _T("KeeColumnWidth2")
#define PWMKEY_COLWIDTH3 _T("KeeColumnWidth3")
#define PWMKEY_COLWIDTH4 _T("KeeColumnWidth4")

#define PWM_NUM_INITIAL_ENTRIES 256
#define PWM_NUM_INITIAL_GROUPS  32
#define PWM_PASSWORD_STRING     _T("********")

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

	TCHAR *pszTitle;
	TCHAR *pszURL;
	TCHAR *pszUserName;

	DWORD uPasswordLen;
	TCHAR *pszPassword;

	TCHAR *pszAdditional;
} PW_ENTRY, *PPW_ENTRY;

typedef struct _PW_GROUP
{
	DWORD uImageId;
	TCHAR *pszGroupName;
} PW_GROUP, *PPW_GROUP;

typedef struct _PW_DBHEADER
{
	DWORD dwSignature1;
	DWORD dwSignature2;
	DWORD dwFlags;
	DWORD dwVersion;

	BYTE aMasterSeed[16];
	RD_UINT8 aEncryptionIV[16];

	DWORD dwGroups;
	DWORD dwEntries;
} PW_DBHEADER, *PPW_DBHEADER;
#pragma pack()

#ifdef _DEBUG
#define ASSERT_ENTRY(pp) ASSERT(pp != NULL); ASSERT(pp->pszTitle != NULL); \
	ASSERT(pp->pszUserName != NULL); ASSERT(pp->pszURL != NULL); \
	ASSERT(pp->pszPassword != NULL); ASSERT(pp->pszAdditional != NULL);
#else
#define ASSERT_ENTRY(pp)
#endif

class CPwManager
{
public:
	CPwManager();
	virtual ~CPwManager();

	void CleanUp();

	BOOL SetMasterKey(const TCHAR *pszMasterKey, BOOL bDiskDrive, const CNewRandomInterface *pARI);

	DWORD GetNumberOfEntries();
	DWORD GetNumberOfGroups();

	int GetNumberOfItemsInGroup(const TCHAR *pszGroup);
	int GetNumberOfItemsInGroupN(int idGroup);
	PW_ENTRY *GetEntry(DWORD dwIndex);
	PW_ENTRY *GetEntryByGroup(int idGroup, DWORD dwIndex);
	int GetEntryByGroupN(int idGroup, DWORD dwIndex);
	PW_GROUP *GetGroup(DWORD dwIndex);

	int GetGroupId(const TCHAR *pszGroupName);

	BOOL AddGroup(DWORD uImageId, const TCHAR *pszGroupName);
	BOOL AddEntry(DWORD uGroupId, DWORD uImageId, const TCHAR *pszTitle,
		const TCHAR *pszURL, const TCHAR *pszUserName, const TCHAR *pszPassword,
		const TCHAR *pszAdditional);
	BOOL DeleteEntry(DWORD dwIndex);
	BOOL DeleteGroup(int nGroupId);

	BOOL SetGroup(DWORD dwIndex, DWORD uImageId, const TCHAR *pszGroupName);
	BOOL SetEntry(DWORD dwIndex, DWORD uGroupId, DWORD uImageId,
		const TCHAR *pszTitle, const TCHAR *pszURL, const TCHAR *pszUserName,
		const TCHAR *pszPassword, const TCHAR *pszAdditional);

	void LockEntryPassword(PW_ENTRY *pEntry);
	void UnlockEntryPassword(PW_ENTRY *pEntry);

	void NewDatabase();
	BOOL OpenDatabase(const TCHAR *pszFile);
	BOOL SaveDatabase(const TCHAR *pszFile);

	void MoveInternal(int nFrom, int nTo);
	void MoveInGroup(int nGroup, int nFrom, int nTo);
	BOOL MoveGroup(int nFrom, int nTo);
	void SortGroup(int nGroup, DWORD dwSortByField);

	int Find(const TCHAR *pszFindString, BOOL bCaseSensitive, int fieldFlags, int nStart);

	// Unicode/Ascii conversion helpers
	char *_ToAscii(const TCHAR *lptString);
	TCHAR *_ToUnicode(const char *pszString);

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
