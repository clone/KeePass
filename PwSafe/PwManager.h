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

// When making a Windows build, don't forget to update the verinfo resource
#define PWM_VERSION_STR  _T("0.97b")

// The signature constants were chosen randomly
#define PWM_DBSIG_1      0x9AA2D903
#define PWM_DBSIG_2      0xB54BFB65
#define PWM_DBVER_DW     0x00030001

#define PWM_HOMEPAGE     _T("http://keepass.sourceforge.net")
#define PWM_URL_TRL      _T("http://keepass.sourceforge.net/translations.php")

#define PWM_README_FILE  _T("KeePass.html")
#define PWM_LICENSE_FILE _T("License.html")

#define PWM_EXENAME       _T("KeePass")
#define PWMKEY_LANG       _T("KeeLanguage")
#define PWMKEY_CLIPSECS   _T("KeeClipboardSeconds")
#define PWMKEY_NEWLINE    _T("KeeNewLine")
#define PWMKEY_LASTDIR    _T("KeeLastDir")
#define PWMKEY_OPENLASTB  _T("KeeAutoOpen")
#define PWMKEY_LASTDB     _T("KeeLastDb")
#define PWMKEY_AUTOSAVEB  _T("KeeAutoSave")
#define PWMKEY_IMGBTNS    _T("KeeImgButtons")
#define PWMKEY_ENTRYGRID  _T("KeeEntryGrid")
#define PWMKEY_ALWAYSTOP  _T("KeeAlwaysOnTop")
#define PWMKEY_SHOWTITLE  _T("KeeShowTitle")
#define PWMKEY_SHOWUSER   _T("KeeShowUser")
#define PWMKEY_SHOWURL    _T("KeeShowURL")
#define PWMKEY_SHOWPASS   _T("KeeShowPassword")
#define PWMKEY_SHOWNOTES  _T("KeeShowNotes")
#define PWMKEY_SHOWATTACH _T("KeeShowAttach")
#define PWMKEY_HIDESTARS  _T("KeeHideStars")
#define PWMKEY_HIDEUSERS  _T("KeeHideUserStars")
#define PWMKEY_LISTFONT   _T("KeeListFont")
#define PWMKEY_WINDOWPX   _T("KeeWindowPX")
#define PWMKEY_WINDOWPY   _T("KeeWindowPY")
#define PWMKEY_WINDOWDX   _T("KeeWindowDX")
#define PWMKEY_WINDOWDY   _T("KeeWindowDY")
#define PWMKEY_SCREENID   _T("KeeLastScreenId")
#define PWMKEY_COLWIDTH0  _T("KeeColumnWidth0")
#define PWMKEY_COLWIDTH1  _T("KeeColumnWidth1")
#define PWMKEY_COLWIDTH2  _T("KeeColumnWidth2")
#define PWMKEY_COLWIDTH3  _T("KeeColumnWidth3")
#define PWMKEY_COLWIDTH4  _T("KeeColumnWidth4")
#define PWMKEY_COLWIDTH5  _T("KeeColumnWidth5")
#define PWMKEY_COLWIDTH6  _T("KeeColumnWidth6")
#define PWMKEY_COLWIDTH7  _T("KeeColumnWidth7")
#define PWMKEY_COLWIDTH8  _T("KeeColumnWidth8")
#define PWMKEY_COLWIDTH9  _T("KeeColumnWidth9")
#define PWMKEY_COLWIDTH10 _T("KeeColumnWidth10")
#define PWMKEY_SPLITTERX  _T("KeeSplitterX")
#define PWMKEY_SPLITTERY  _T("KeeSplitterY")
#define PWMKEY_ENTRYVIEW  _T("KeeEntryView")
#define PWMKEY_LOCKMIN    _T("KeeLockOnMinimize")
#define PWMKEY_MINTRAY    _T("KeeMinimizeToTray")
#define PWMKEY_CLOSEMIN   _T("KeeCloseMinimizes")
#define PWMKEY_LOCKTIMER  _T("KeeLockAfterTime")
#define PWMKEY_ROWCOLOR   _T("KeeRowColor")
#define PWMKEY_SHOWCREATION     _T("KeeShowCreation")
#define PWMKEY_SHOWLASTMOD      _T("KeeShowLastMod")
#define PWMKEY_SHOWLASTACCESS   _T("KeeShowLastAccess")
#define PWMKEY_SHOWEXPIRE       _T("KeeShowExpire")
#define PWMKEY_SHOWUUID         _T("KeeShowUUID")
#define PWMKEY_SHOWTOOLBAR      _T("KeeShowToolBar")
#define PWMKEY_COLAUTOSIZE      _T("KeeColAutoSize")
#define PWMKEY_PWGEN_OPTIONS    _T("KeePwGenOptions")
#define PWMKEY_PWGEN_CHARS      _T("KeePwGenChars")
#define PWMKEY_PWGEN_NUMCHARS   _T("KeePwGenNumChars")
#define PWMKEY_DISABLEUNSAFE    _T("KeeDisableUnsafe")
#define PWMKEY_REMEMBERLAST     _T("KeeRememberLast")
#define PWMKEY_HEADERORDER      _T("KeeHeaderItemOrder")
#define PWMKEY_USEPUTTYFORURLS  _T("KeeUsePutty")
#define PWMKEY_SAVEONLATMOD     _T("KeeSaveOnLATMod")
#define PWMKEY_WINSTATE_MAX     _T("KeeWindowMaximized")

#define PWM_NUM_INITIAL_ENTRIES 256
#define PWM_NUM_INITIAL_GROUPS  32

#define PWM_PASSWORD_STRING     _T("********")
#define PWS_SEARCHGROUP         TRL("Search results")

#define PWM_FLAG_SHA2           1
#define PWM_FLAG_RIJNDAEL       2
#define PWM_FLAG_ARCFOUR        4
#define PWM_FLAG_TWOFISH        8

#define PWM_SESSION_KEY_SIZE    12

#define PWM_STD_KEYENCROUNDS    6000

// Field flags (for example in Find function)
#define PWMF_TITLE        1
#define PWMF_USER         2
#define PWMF_URL          4
#define PWMF_PASSWORD     8
#define PWMF_ADDITIONAL  16
#define PWMF_GROUPNAME   32
#define PWMF_CREATION    64
#define PWMF_LASTMOD    128
#define PWMF_LASTACCESS 256
#define PWMF_EXPIRE     512
#define PWMF_UUID      1024

#define PWGF_EXPANDED     1

#define ALGO_AES          0
#define ALGO_TWOFISH      1

#pragma pack(1)

typedef struct _PW_TIME
{
	USHORT shYear; // Year, 2004 means 2004
	BYTE btMonth;  // Month, ranges from 1 = Jan to 12 = Dec
	BYTE btDay;    // Day, the first day is 1
	BYTE btHour;   // Hour, begins with hour 0, max value is 23
	BYTE btMinute; // Minutes, begins at 0, max value is 59
	BYTE btSecond; // Seconds, begins at 0, max value is 59
} PW_TIME, *PPW_TIME;

typedef struct _PW_DBHEADER // The database header
{
	DWORD dwSignature1; // = PWM_DBSIG_1
	DWORD dwSignature2; // = PWM_DBSIG_2
	DWORD dwFlags;
	DWORD dwVersion;

	BYTE aMasterSeed[16]; // Seed that gets hashed with the userkey to form the final key
	RD_UINT8 aEncryptionIV[16]; // IV used for content encryption

	DWORD dwGroups; // Number of groups in the database
	DWORD dwEntries; // Number of entries in the database

	BYTE aContentsHash[32]; // SHA-256 hash of the database, used for integrity check

	BYTE aMasterSeed2[32]; // Used for the dwKeyEncRounds AES transformations
	DWORD dwKeyEncRounds;
} PW_DBHEADER, *PPW_DBHEADER;

typedef struct _PW_GROUP // Structure containing information about one group
{
	DWORD uGroupId;
	DWORD uImageId;
	TCHAR *pszGroupName;

	PW_TIME tCreation;
	PW_TIME tLastMod;
	PW_TIME tLastAccess;
	PW_TIME tExpire;

	USHORT usLevel;

	DWORD dwFlags;
} PW_GROUP, *PPW_GROUP;

typedef struct _PW_ENTRY // Structure containing information about one entry
{
	BYTE uuid[16];
	DWORD uGroupId;
	DWORD uImageId;

	TCHAR *pszTitle;
	TCHAR *pszURL;
	TCHAR *pszUserName;

	DWORD uPasswordLen;
	TCHAR *pszPassword;

	TCHAR *pszAdditional;

	PW_TIME tCreation;
	PW_TIME tLastMod;
	PW_TIME tLastAccess;
	PW_TIME tExpire;

	TCHAR *pszBinaryDesc; // A string describing what is in pBinaryData
	BYTE *pBinaryData;
	DWORD uBinaryDataLen;
} PW_ENTRY, *PPW_ENTRY;

#pragma pack()

#ifdef _DEBUG
#define ASSERT_ENTRY(pp) ASSERT((pp) != NULL); ASSERT((pp)->pszTitle != NULL); \
	ASSERT((pp)->pszUserName != NULL); ASSERT((pp)->pszURL != NULL); \
	ASSERT((pp)->pszPassword != NULL); ASSERT((pp)->pszAdditional != NULL); \
	ASSERT((pp)->pszBinaryDesc != NULL); if(((pp)->uBinaryDataLen != 0) && \
	((pp)->pBinaryData == NULL)) { ASSERT(FALSE); }
#else
#define ASSERT_ENTRY(pp)
#endif
#ifndef DWORD_MAX
#define DWORD_MAX 0xFFFFFFFF
#endif

class CPwManager
{
public:
	CPwManager();
	virtual ~CPwManager();

	void CleanUp(); // Delete everything and release all allocated memory

	// Set the master key for the database.
	BOOL SetMasterKey(const TCHAR *pszMasterKey, BOOL bDiskDrive, const CNewRandomInterface *pARI, BOOL bOverwrite = TRUE);

	DWORD GetNumberOfEntries(); // Returns number of entries in database
	DWORD GetNumberOfGroups(); // Returns number of groups in database

	// Count items in groups
	DWORD GetNumberOfItemsInGroup(const TCHAR *pszGroup);
	DWORD GetNumberOfItemsInGroupN(DWORD idGroup);

	// Access entry information
	PW_ENTRY *GetEntry(DWORD dwIndex);
	PW_ENTRY *GetEntryByGroup(DWORD idGroup, DWORD dwIndex);
	DWORD GetEntryByGroupN(DWORD idGroup, DWORD dwIndex);
	PW_ENTRY *GetEntryByUuid(BYTE *pUuid);
	DWORD GetEntryByUuidN(BYTE *pUuid); // Returns the index of the item with pUuid
	DWORD GetEntryPosInGroup(PW_ENTRY *pEntry);
	PW_ENTRY *GetLastEditedEntry();

	// Access group information
	PW_GROUP *GetGroup(DWORD dwIndex);
	PW_GROUP *GetGroupById(DWORD idGroup);
	DWORD GetGroupByIdN(DWORD idGroup);
	DWORD GetGroupId(const TCHAR *pszGroupName);
	DWORD GetGroupIdByIndex(DWORD uGroupIndex);
	DWORD GetLastChildGroup(DWORD dwParentGroupIndex);

	// Add entries and groups
	BOOL AddGroup(const PW_GROUP *pTemplate);
	BOOL AddEntry(const PW_ENTRY *pTemplate);

	// Delete entries and groups
	BOOL DeleteEntry(DWORD dwIndex);
	BOOL DeleteGroupById(DWORD uGroupId);

	BOOL SetGroup(DWORD dwIndex, PW_GROUP *pTemplate);
	BOOL SetEntry(DWORD dwIndex, PW_ENTRY *pTemplate);
	// DWORD MakeGroupTree(LPCTSTR lpTreeString, TCHAR tchSeparator);

	// Use these functions to make passwords in PW_ENTRY structures readable
	void LockEntryPassword(PW_ENTRY *pEntry); // Lock password, encrypt it
	void UnlockEntryPassword(PW_ENTRY *pEntry); // Make password readable

	void NewDatabase();
	BOOL OpenDatabase(const TCHAR *pszFile);
	BOOL SaveDatabase(const TCHAR *pszFile);

	// Move entries and groups
	void MoveInternal(DWORD nFrom, DWORD nTo);
	void MoveInGroup(DWORD idGroup, DWORD nFrom, DWORD nTo);
	BOOL MoveGroup(DWORD nFrom, DWORD nTo);

	// Sort entry and group lists
	void SortGroup(DWORD idGroup, DWORD dwSortByField);
	void SortGroupList();

	// Find an item
	DWORD Find(const TCHAR *pszFindString, BOOL bCaseSensitive, DWORD fieldFlags, DWORD nStart);

	// Get and set the algorithm used to encrypt the database
	BOOL SetAlgorithm(int nAlgorithm);
	int GetAlgorithm();

	DWORD GetKeyEncRounds();
	void SetKeyEncRounds(DWORD dwRounds);

	// Convert PW_TIME to 5-byte compressed structure and the other way round
	static void _TimeToPwTime(BYTE *pCompressedTime, PW_TIME *pPwTime);
	static void _PwTimeToTime(PW_TIME *pPwTime, BYTE *pCompressedTime);

	// Get the never-expire time
	static void _GetNeverExpireTime(PW_TIME *pPwTime);

	// Checks and corrects the group tree (level order, etc.)
	void FixGroupTree();
	void DeleteLostEntries();

	void SubstEntryGroupIds(DWORD dwExistingId, DWORD dwNewId);

	BOOL AttachFileAsBinaryData(PW_ENTRY *pEntry, const TCHAR *lpFile);
	BOOL SaveBinaryData(PW_ENTRY *pEntry, const TCHAR *lpFile);
	BOOL RemoveBinaryData(PW_ENTRY *pEntry);

	static BOOL IsAllowedStoreGroup(LPCTSTR lpGroupName, LPCTSTR lpSearchGroupName);

protected:
	virtual BOOL ReadGroupField(USHORT usFieldType, DWORD dwFieldSize, BYTE *pData, PW_GROUP *pGroup);
	virtual BOOL ReadEntryField(USHORT usFieldType, DWORD dwFieldSize, BYTE *pData, PW_ENTRY *pEntry);

private:
	void _AllocEntries(DWORD uEntries);
	void _DeleteEntryList(BOOL bFreeStrings);
	void _AllocGroups(DWORD uGroups);
	void _DeleteGroupList(BOOL bFreeStrings);

	BOOL _OpenDatabaseV1(const TCHAR *pszFile);
	BOOL _OpenDatabaseV2(const TCHAR *pszFile);
	BOOL _ReadGroupFieldV2(USHORT usFieldType, DWORD dwFieldSize, BYTE *pData, PW_GROUP *pGroup);
	BOOL _ReadEntryFieldV2(USHORT usFieldType, DWORD dwFieldSize, BYTE *pData, PW_ENTRY *pEntry);

	// Encrypt the master key a few times to make brute-force key-search harder
	BOOL _TransformMasterKey(BYTE *pKeySeed);

	PW_ENTRY *m_pEntries; // List containing all entries
	DWORD m_dwMaxEntries; // Maximum number of items that can be stored in the list
	DWORD m_dwNumEntries; // Current number of items stored in the list

	PW_GROUP *m_pGroups; // List containing all groups
	DWORD m_dwMaxGroups; // Maximum number of groups that can be stored in the list
	DWORD m_dwNumGroups; // Current number of groups stored in the list

	PW_ENTRY *m_pLastEditedEntry; // Last modified entry, use GetLastEditedEntry() to get it

	CNewRandom m_random; // Pseudo-random number generator

	BYTE m_pSessionKey[PWM_SESSION_KEY_SIZE]; // Used for in-memory encryption of passwords
	BYTE m_pMasterKey[32]; // Master key used to encrypt the whole database
	BYTE m_pTransformedMasterKey[32]; // Master key encrypted several times
	int m_nAlgorithm; // Algorithm used to encrypt the database
	DWORD m_dwKeyEncRounds;
};

#endif
