/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2010 Dominik Reichl <dominik.reichl@t-online.de>

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

#ifndef ___KEEPASS_PASSWORD_MANAGER_H___
#define ___KEEPASS_PASSWORD_MANAGER_H___

#pragma once

#include "SysDefEx.h"
#include <string>
#include <vector>
#include <boost/utility.hpp>

#include "Util/NewRandom.h"
#include "Crypto/Rijndael.h"
#include "PwStructs.h"

// General product information
#define PWM_PRODUCT_NAME       _T("KeePass Password Safe")
#define PWM_PRODUCT_NAME_SHORT _T("KeePass")

// When making a Windows build, don't forget to update the verinfo resource
#ifndef _UNICODE
#define PWM_VERSION_STR  _T("1.18")
#else
#define PWM_VERSION_STR  _T("1.18 Unicode")
#endif
#define PWM_VERSION_DW   0x01010800

// Database file signature bytes
#define PWM_DBSIG_1      0x9AA2D903
#define PWM_DBSIG_2      0xB54BFB65
#define PWM_DBVER_DW     0x00030002

// KeePass 2.x database file signatures (pre-release and release)
#define PWM_DBSIG_1_KDBX_P 0x9AA2D903
#define PWM_DBSIG_2_KDBX_P 0xB54BFB66
#define PWM_DBSIG_1_KDBX_R 0x9AA2D903
#define PWM_DBSIG_2_KDBX_R 0xB54BFB67

#define PWM_HOMEPAGE     _T("http://keepass.info/")
#define PWM_URL_TRL      _T("http://keepass.info/translations.html")
#define PWM_URL_PLUGINS  _T("http://keepass.info/plugins.html")
#define PWM_URL_VERSION  _T("http://keepass.info/update/version1x.txt")
#define PWM_URL_DONATE   _T("http://keepass.info/donate.html")
#define PWM_URL_HELP     _T("http://keepass.info/help/")

#define PWM_EXENAME       _T("KeePass")

#define PWM_README_FILE   _T("KeePass.chm")
#define PWM_LICENSE_FILE  _T("License.txt")

#define PWM_HELP_AUTOTYPE  _T("help/base/autotype.html")
#define PWM_HELP_URLS      _T("help/base/autourl.html")
#define PWM_HELP_CREDITS   _T("help/base/credits.html")
#define PWM_HELP_SECURITY  _T("help/base/security.html")
#define PWM_HELP_PLUGINS   _T("help/v1/plugins.html")
#define PWM_HELP_KEYS      _T("help/base/keys.html")
#define PWM_HELP_TANS      _T("help/base/tans.html")
#define PWM_HELP_PWGEN     _T("help/base/pwgenerator.html")
#define PWM_HELP_PWGEN_ADV _T("help/base/pwgenerator.html#secreduc")
#define PWM_HELP_CSV       _T("help/base/importexport.html#csv")
#define PWM_HELP_FIELDREFS _T("help/base/fieldrefs.html")
#define PWM_HELP_ERRORS    _T("help/v1/errors.html")

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
// #define PWMKEY_PWGEN_OPTIONS    _T("KeePwGenOptions")
// #define PWMKEY_PWGEN_CHARS      _T("KeePwGenChars")
// #define PWMKEY_PWGEN_NUMCHARS   _T("KeePwGenNumChars")
#define PWMKEY_DISABLEUNSAFE    _T("KeeDisableUnsafe")
#define PWMKEY_REMEMBERLAST     _T("KeeRememberLast")
#define PWMKEY_HEADERORDER      _T("KeeHeaderItemOrder")
#define PWMKEY_USEPUTTYFORURLS  _T("KeeUsePutty")
#define PWMKEY_SAVEONLATMOD     _T("KeeSaveOnLATMod")
#define PWMKEY_WINSTATE_MAX     _T("KeeWindowMaximized")
#define PWMKEY_AUTOSORT         _T("KeeAutoSortPwList")
#define PWMKEY_AUTOTYPEHOTKEY   _T("KeeAutoTypeHotKey")
#define PWMKEY_RESTOREHOTKEY    _T("KeeRestoreHotKey")
#define PWMKEY_CLIPBOARDMETHOD  _T("KeeClipboardMethod")
#define PWMKEY_STARTMINIMIZED   _T("KeeStartMinimized")
#define PWMKEY_AUTOSHOWEXPIRED  _T("KeeShowExpiredAtOpen")
#define PWMKEY_AUTOSHOWEXPIREDS _T("KeeShowSoonExpiredAtOpen")
#define PWMKEY_BACKUPENTRIES    _T("KeeBackupEntries")
#define PWMKEY_SINGLEINSTANCE   _T("KeeSingleInstance")
#define PWMKEY_SECUREEDITS      _T("KeeSecureEditControls")
#define PWMKEY_SINGLECLICKTRAY  _T("KeeSingleClickTrayIcon")
#define PWMKEY_DEFAULTEXPIRE    _T("KeeDefaultExpire")
#define PWMKEY_AUTOPWGEN        _T("KeeAutoPwGen")
#define PWMKEY_QUICKFINDINCBK   _T("KeeQuickFindIncBackup")
#define PWMKEY_QUICKFINDINCEXP  _T("KeeQuickFindIncExpired")
#define PWMKEY_AUTOTYPEMETHOD   _T("KeeAutoTypeMethod")
#define PWMKEY_DELETEBKONSAVE   _T("KeeDeleteBackupsOnSave")
#define PWMKEY_SHOWFULLPATH     _T("KeeShowFullPath")
#define PWMKEY_DISABLEAUTOTYPE  _T("KeeDisableAutoType")
#define PWMKEY_COPYURLS         _T("KeeCopyURLs")
#define PWMKEY_EXITINSTEADLOCK  _T("KeeExitInsteadOfLockAT")
#define PWMKEY_URLOVERRIDE      _T("KeeURLOverride")
#define PWMKEY_SIMPLETANVIEW    _T("KeeSimpleTANView")
#define PWMKEY_SHOWTANINDICES   _T("KeeShowTANIndices")
#define PWMKEY_ALLOWSAVEIFMODIFIEDONLY _T("KeeAllowSaveIfModifiedOnly")
#define PWMKEY_CHECKFORUPDATE   _T("KeeCheckForUpdate")
#define PWMKEY_LOCKONWINLOCK    _T("KeeLockOnWinLock")
#define PWMKEY_ENABLEREMOTECTRL _T("KeeEnableRemoteCtrl")
#define PWMKEY_ALWAYSALLOWIPC   _T("KeeAlwaysAllowIPC")
#define PWMKEY_DEFAULTATSEQ     _T("KeeDefaultAutoTypeSequence")
#define PWMKEY_USELOCALTIMEFMT  _T("KeeUseLocalTimeFormat")
#define PWMKEY_TANCHARS         _T("KeeTANCharacters")
#define PWMKEY_HTMURLMETHOD     _T("KeeHTMURLMethod")
#define PWMKEY_MINIMODE         _T("KeeMiniMode")
#define PWMKEY_UNINTRUSIVEMINIMODE _T("KeeUnintrusiveMiniMode")
#define PWMKEY_BANNERCOLORSTART _T("KeeBannerColorStart")
#define PWMKEY_BANNERCOLOREND   _T("KeeBannerColorEnd")
#define PWMKEY_BANNERCOLORTEXT  _T("KeeBannerColorText")
#define PWMKEY_BANNERFLIP       _T("KeeBannerFlip")
#define PWMKEY_ROOTONNEW        _T("KeeRootInNewDb")
#define PWMKEY_GROUPONNEW_PRE   _T("KeeGroupInNewDb")
#define PWMKEY_WNDTITLESUFFIX   _T("KeeWindowTitleSuffix")
#define PWMKEY_MASTERPW_MINLEN  _T("KeeMasterPasswordMinLength")
#define PWMKEY_MASTERPW_MINQUALITY _T("KeeMasterPasswordMinQuality")
#define PWMKEY_FOCUSRESAFTERQUICKFIND _T("KeeFocusResultsAfterQuickFind")
#define PWMKEY_AUTOTYPEIEFIX    _T("KeeAutoTypeIEFix")
#define PWMKEY_DROPTOBACKONCOPY _T("KeeDropToBackOnCopy")
#define PWMKEY_SORTAUTOTYPESELITEMS _T("KeeSortAutoTypeSelItems")
#define PWMKEY_CLEARCLIPONDBCLOSE   _T("KeeClearClipboardOnDbClose")
#define PWMKEY_USEHELPCENTER        _T("KeeUseHelpCenter")
#define PWMKEY_USEDPAPIFORMEMPROT   _T("KeeUseDPAPIForMemoryProtection")
#define PWMKEY_USECNGBCRYPTFORKEYT  _T("KeeUseCNGForKeyTransformations")
#define PWMKEY_FORCEALLOWCHANGEMKEY _T("KeeForceAllowChangeMasterKey")
#define PWMKEY_FORCEALLOWPRINTING   _T("KeeForceAllowPrinting")
#define PWMKEY_FORCEALLOWIMPORT     _T("KeeForceAllowImport")
#define PWMKEY_FORCEALLOWEXPORT     _T("KeeForceAllowExport")
#define PWMKEY_DISALLOWPRINTINGPWS  _T("KeeDisallowPrintingPasswords")
#define PWMKEY_AUTONEWDBBASEPATH    _T("KeeAutoNewDbBasePath")
#define PWMKEY_AUTONEWDBBASENAME    _T("KeeAutoNewDbBaseName")
#define PWMKEY_DELETETANSAFTERUSE   _T("KeeDeleteTANsAfterUse")
#define PWMKEY_SOONTOEXPIREDAYS     _T("KeeSoonToExpireDays")
#define PWMKEY_TRANSACTEDFILEWRITE  _T("KeeUseTransactedFileWrites")

#define PWMKEY_GENPROFILE       _T("KeeGenProfile")
#define PWMKEY_GENPROFILEAUTO   _T("KeeGenProfileAuto")
#define PWMKEY_GENPROFILELASTPR _T("KeeGenProfileLast")

#define PWMKEY_PREFERUSER           _T("KeePreferUserConfiguration")
#define PWMKEY_CFGOVERRIDE_GLOBAL   _T("KeeConfigFileOverrideGlobal")
#define PWMKEY_CFGOVERRIDE_USER     _T("KeeConfigFileOverrideUser")

#define PWM_NUM_INITIAL_ENTRIES 256
#define PWM_NUM_INITIAL_GROUPS  32

#define PWM_PASSWORD_STRING      _T("********")

#define PWM_KEYMETHOD_OR         FALSE
#define PWM_KEYMETHOD_AND        TRUE

#define PWS_SEARCHGROUP          TRL("Search Results")
#define PWS_BACKUPGROUP_SRC      _T("Backup")
#define PWS_BACKUPGROUP          TRL("Backup")
#define PWS_DEFAULT_KEY_FILENAME _T("pwsafe.key")
#define PWV_SOONTOEXPIRE_DAYS    7

#define PWS_TAN_ENTRY            TRL("<TAN>")

#define PWM_FLAG_SHA2            1
#define PWM_FLAG_RIJNDAEL        2
#define PWM_FLAG_ARCFOUR         4
#define PWM_FLAG_TWOFISH         8

#define PWM_SESSION_KEY_SIZE     12

#define PWM_STD_KEYENCROUNDS     6000

#define PWM_STD_ICON_GROUP       48
#define PWM_STD_ICON_GROUP_OPEN  49
#define PWM_STD_ICON_GROUP_EMAIL 50
#define PWM_STD_ICON_GROUP_PKG   25

#define PWM_STD_MAX_HISTORYITEMS 12

#define PWM_STD_HRSLINK_FCT      2

// Field flags (for example in Find function)
// These flags must be disjoint to PWMS_* flags
#define PWMF_TITLE              1
#define PWMF_USER               2
#define PWMF_URL                4
#define PWMF_PASSWORD           8
#define PWMF_ADDITIONAL        16
#define PWMF_GROUPNAME         32
#define PWMF_CREATION          64
#define PWMF_LASTMOD          128
#define PWMF_LASTACCESS       256
#define PWMF_EXPIRE           512
#define PWMF_UUID            1024

// Search flags
// These flags must be disjoint to PWMF_* flags
#define PWMS_REGEX       0x10000000

// Group flags (dwFlags field of PW_GROUP)
#define PWGF_EXPANDED    1
#define PWGF_RESERVED0   0x1000     /* Reserved for KeePassMobile */
#define PWGF_TEMP_BIT    0x40000000

#define ALGO_AES         0
#define ALGO_TWOFISH     1

// Error codes
#define PWE_UNKNOWN                 0
#define PWE_SUCCESS                 1
#define PWE_INVALID_PARAM           2
#define PWE_NO_MEM                  3
#define PWE_INVALID_KEY             4
#define PWE_NOFILEACCESS_READ       5
#define PWE_NOFILEACCESS_WRITE      6
#define PWE_FILEERROR_READ          7 
#define PWE_FILEERROR_WRITE         8
#define PWE_INVALID_RANDOMSOURCE    9
#define PWE_INVALID_FILESTRUCTURE  10
#define PWE_CRYPT_ERROR            11
#define PWE_INVALID_FILESIZE       12
#define PWE_INVALID_FILESIGNATURE  13
#define PWE_INVALID_FILEHEADER     14
#define PWE_NOFILEACCESS_READ_KEY  15
#define PWE_KEYPROV_INVALID_KEY    16
#define PWE_FILEERROR_VERIFY       17
#define PWE_UNSUPPORTED_KDBX       18
#define PWE_GETLASTERROR           19

// Format flags
#define PWFF_NO_INTRO               1
#define PWFF_INVKEY_WITH_CODE       2
#define PWFF_DATALOSS_WITHOUT_SAVE  0x4000
#define PWFF_MAIN_TEXT_ONLY         0x8000

// Property IDs
#define PWP_DEFAULT_USER_NAME       1

// Array property IDs
#define PWPA_SEARCH_HISTORY         1

// Password meta streams
#define PMS_ID_BINDESC  _T("bin-stream")
#define PMS_ID_TITLE    _T("Meta-Info")
#define PMS_ID_USER     _T("SYSTEM")
#define PMS_ID_URL      _T("$")

#define PMS_STREAM_SIMPLESTATE       _T("Simple UI State")
#define PMS_STREAM_DEFAULTUSER       _T("Default User Name")
#define PMS_STREAM_SEARCHHISTORYITEM _T("Search History Item")
#define PMS_STREAM_CUSTOMKVP         _T("Custom KVP")
#define PMS_STREAM_KPXICON2          _T("KPX_CUSTOM_ICONS_2")

#define PMD_HREL_ID     PWM_PRODUCT_NAME

#ifdef VPA_MODIFY
#error VPA_MODIFY must not be defined already.
#else
#define VPA_MODIFY
#endif

#pragma pack(1)

typedef struct _PMS_SIMPLE_UI_STATE
{
	DWORD uLastSelectedGroupId;
	DWORD uLastTopVisibleGroupId;
	BYTE aLastSelectedEntryUuid[16];
	BYTE aLastTopVisibleEntryUuid[16];
	DWORD dwReserved01;
	DWORD dwReserved02;
	DWORD dwReserved03;
	DWORD dwReserved04;
	DWORD dwReserved05;
	DWORD dwReserved06;
	DWORD dwReserved07;
	DWORD dwReserved08;
	DWORD dwReserved09;
	DWORD dwReserved10;
	DWORD dwReserved11;
	DWORD dwReserved12;
	DWORD dwReserved13;
	DWORD dwReserved14;
	DWORD dwReserved15;
	DWORD dwReserved16;
} PMS_SIMPLE_UI_STATE;

typedef struct _PWDB_META_STREAM
{
	std::basic_string<TCHAR> strName;
	std::vector<BYTE> vData;
} PWDB_META_STREAM;

#pragma pack()

typedef std::pair<std::basic_string<TCHAR>, std::basic_string<TCHAR> > CustomKvp;

#ifdef _DEBUG
#define ASSERT_ENTRY(pp) ASSERT((pp) != NULL); ASSERT((pp)->pszTitle != NULL); \
	ASSERT((pp)->pszUserName != NULL); ASSERT((pp)->pszURL != NULL); \
	ASSERT((pp)->pszPassword != NULL); ASSERT((pp)->pszAdditional != NULL); \
	ASSERT((pp)->pszBinaryDesc != NULL); \
	if(((pp)->uBinaryDataLen != 0) && ((pp)->pBinaryData == NULL)) { ASSERT(FALSE); }
#else
#define ASSERT_ENTRY(pp)
#endif

class CPwManager : boost::noncopyable
{
public:
	CPwManager();
	virtual ~CPwManager();

	void InitPrimaryInstance();

	// Set the master key for the database
	int SetMasterKey(const TCHAR *pszMasterKey, BOOL bDiskDrive, const TCHAR *pszSecondKey,
		const CNewRandomInterface *pARI, BOOL bOverwrite);

	DWORD GetNumberOfEntries() const; // Returns number of entries in database
	DWORD GetNumberOfGroups() const; // Returns number of groups in database

	// Count items in groups
	DWORD GetNumberOfItemsInGroup(const TCHAR *pszGroup) const;
	DWORD GetNumberOfItemsInGroupN(DWORD idGroup) const;

	// Access entry information
	PW_ENTRY *GetEntry(DWORD dwIndex);
	PW_ENTRY *GetEntryByGroup(DWORD idGroup, DWORD dwIndex);
	DWORD GetEntryByGroupN(DWORD idGroup, DWORD dwIndex) const;
	PW_ENTRY *GetEntryByUuid(const BYTE *pUuid);
	DWORD GetEntryByUuidN(const BYTE *pUuid) const; // Returns the index of the item with pUuid
	DWORD GetEntryPosInGroup(__in_ecount(1) const PW_ENTRY *pEntry) const;
	PW_ENTRY *GetLastEditedEntry();

	// Access group information
	PW_GROUP *GetGroup(DWORD dwIndex);
	PW_GROUP *GetGroupById(DWORD idGroup);
	DWORD GetGroupByIdN(DWORD idGroup) const;
	DWORD GetGroupId(const TCHAR *pszGroupName) const;
	DWORD GetGroupIdByIndex(DWORD uGroupIndex) const;
	DWORD GetLastChildGroup(DWORD dwParentGroupIndex) const;
	BOOL GetGroupTree(DWORD idGroup, DWORD *pGroupIndexes) const;

	// Add entries and groups
	BOOL AddGroup(__in_ecount(1) const PW_GROUP *pTemplate);
	BOOL AddEntry(__in_ecount(1) const PW_ENTRY *pTemplate);
	BOOL BackupEntry(__in_ecount(1) const PW_ENTRY *pe, __out_opt
		BOOL *pbGroupCreated); // pe must be unlocked already

	// Delete entries and groups
	BOOL DeleteEntry(DWORD dwIndex);
	BOOL DeleteGroupById(DWORD uGroupId, BOOL bCreateBackupEntries);

	BOOL SetGroup(DWORD dwIndex, __in_ecount(1) const PW_GROUP *pTemplate);
	BOOL SetEntry(DWORD dwIndex, __in_ecount(1) const PW_ENTRY *pTemplate);
	// DWORD MakeGroupTree(LPCTSTR lpTreeString, TCHAR tchSeparator);

	// Use these functions to make passwords in PW_ENTRY structures readable
	void LockEntryPassword(__inout_ecount(1) PW_ENTRY *pEntry); // Lock password, encrypt it
	void UnlockEntryPassword(__inout_ecount(1) PW_ENTRY *pEntry); // Make password readable

	void NewDatabase();
	int OpenDatabase(const TCHAR *pszFile, __out_opt PWDB_REPAIR_INFO *pRepair);
	// int OpenDatabaseEx(const TCHAR *pszFile, __out_opt PWDB_REPAIR_INFO *pRepair,
	//	CPwErrorInfo *pErrorInfo);
	int SaveDatabase(const TCHAR *pszFile, BYTE *pWrittenDataHash32);

	// Move entries and groups
	void MoveEntry(DWORD idGroup, DWORD dwFrom, DWORD dwTo);
	BOOL MoveGroup(DWORD dwFrom, DWORD dwTo);
	BOOL MoveGroupEx(DWORD dwFromId, DWORD dwToId);
	BOOL MoveGroupExDir(DWORD dwGroupId, INT iDirection);

	// Sort entry and group lists
	void SortGroup(DWORD idGroup, DWORD dwSortByField);
	void SortGroupList();

	void MergeIn(__inout_ecount(1) CPwManager *pDataSource, BOOL bCreateNewUUIDs,
		BOOL bCompareTimes);

	// Find an item
	DWORD Find(const TCHAR *pszFindString, BOOL bCaseSensitive,
		DWORD searchFlags, DWORD nStart);

	// Get and set the algorithm used to encrypt the database
	int GetAlgorithm() const;
	BOOL SetAlgorithm(int nAlgorithm);

	DWORD GetKeyEncRounds() const;
	void SetKeyEncRounds(DWORD dwRounds);

	// Get the never-expire time
	static void GetNeverExpireTime(__out_ecount(1) PW_TIME *pPwTime);

	// Checks and corrects the group tree (level order, etc.)
	void FixGroupTree();

	void SubstEntryGroupIds(DWORD dwExistingId, DWORD dwNewId);

	const PW_DBHEADER *GetLastDatabaseHeader() const;
	void GetRawMasterKey(__out_ecount(32) BYTE *pStorage);
	void SetRawMasterKey(__in_ecount(32) const BYTE *pNewKey);
	void ClearMasterKey(BOOL bClearKey, BOOL bClearTransformedKey);

	std::basic_string<TCHAR> GetPropertyString(DWORD dwPropertyId) const;
	BOOL SetPropertyString(DWORD dwPropertyId, LPCTSTR lpValue);

	std::vector<std::basic_string<TCHAR> >* AccessPropertyStrArray(DWORD dwPropertyId);

	BOOL SetCustomKvp(LPCTSTR lpKey, LPCTSTR lpValue);
	LPCTSTR GetCustomKvp(LPCTSTR lpKey) const;

	void SetTransactedFileWrites(BOOL bTransacted) { m_bUseTransactedFileWrites = bTransacted; }

	DWORD m_dwLastSelectedGroupId;
	DWORD m_dwLastTopVisibleGroupId;
	BYTE m_aLastSelectedEntryUuid[16];
	BYTE m_aLastTopVisibleEntryUuid[16];

private:
	void CleanUp(); // Delete everything and release all allocated memory

	void _DetMetaInfo();

	void _AllocEntries(DWORD uEntries);
	void _DeleteEntryList(BOOL bFreeStrings);
	void _AllocGroups(DWORD uGroups);
	void _DeleteGroupList(BOOL bFreeStrings);

	BOOL ReadGroupField(USHORT usFieldType, DWORD dwFieldSize,
		const BYTE *pData, PW_GROUP *pGroup);
	BOOL ReadEntryField(USHORT usFieldType, DWORD dwFieldSize,
		const BYTE *pData, PW_ENTRY *pEntry);

	BOOL _AddAllMetaStreams();
	DWORD _LoadAndRemoveAllMetaStreams(bool bAcceptUnknown);
	BOOL _AddMetaStream(LPCTSTR lpMetaDataDesc, BYTE *pData, DWORD dwLength);
	BOOL _IsMetaStream(const PW_ENTRY *p) const;
	void _ParseMetaStream(PW_ENTRY *p, bool bAcceptUnknown);
	BOOL _CanIgnoreUnknownMetaStream(const PWDB_META_STREAM& msUnknown) const;

	// Encrypt the master key a few times to make brute-force key-search harder
	BOOL _TransformMasterKey(const BYTE *pKeySeed);

	DWORD DeleteLostEntries();

	void MoveInternal(DWORD dwFrom, DWORD dwTo);

	static BYTE* SerializeCustomKvp(const CustomKvp& kvp);
	static bool DeserializeCustomKvp(const BYTE *pStream, CustomKvp& kvpBuffer);

	void ProtectMasterKey(bool bProtectKey);
	void ProtectTransformedMasterKey(bool bProtectKey);

	PW_ENTRY *m_pEntries; // List containing all entries
	DWORD m_dwMaxEntries; // Maximum number of items that can be stored in the list
	DWORD m_dwNumEntries; // Current number of items stored in the list

	PW_GROUP *m_pGroups; // List containing all groups
	DWORD m_dwMaxGroups; // Maximum number of groups that can be stored in the list
	DWORD m_dwNumGroups; // Current number of groups stored in the list

	PW_DBHEADER m_dbLastHeader;
	PW_ENTRY *m_pLastEditedEntry; // Last modified entry, use GetLastEditedEntry() to get it

	CNewRandom m_random; // Pseudo-random number generator

	BYTE m_pSessionKey[PWM_SESSION_KEY_SIZE]; // Used for in-memory encryption of passwords
	BYTE m_pMasterKey[32]; // Master key used to encrypt the whole database
	BYTE m_pTransformedMasterKey[32]; // Master key encrypted several times
	int m_nAlgorithm; // Algorithm used to encrypt the database
	DWORD m_dwKeyEncRounds;

	std::basic_string<TCHAR> m_strDefaultUserName;
	std::vector<std::basic_string<TCHAR> > m_vSearchHistory;
	std::vector<CustomKvp> m_vCustomKVPs;

	std::vector<PWDB_META_STREAM> m_vUnknownMetaStreams;

	BOOL m_bUseTransactedFileWrites;
};

#endif // ___KEEPASS_PASSWORD_MANAGER_H___
