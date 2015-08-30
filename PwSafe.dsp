# Microsoft Developer Studio Project File - Name="PwSafe" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PwSafe - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PwSafe.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PwSafe.mak" CFG="PwSafe - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PwSafe - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "PwSafe - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PwSafe - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /out:"Release/KeePass.exe"

!ELSEIF  "$(CFG)" == "PwSafe - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/KeePass.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "PwSafe - Win32 Release"
# Name "PwSafe - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AddEntryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AddGroupDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DbSettingsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EntryListDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EntryPropertiesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FindInDbDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GetRandomDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\IconPickerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LanguagesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PasswordDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PluginsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PwGeneratorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PwSafe.cpp
# End Source File
# Begin Source File

SOURCE=.\PwSafe.rc
# End Source File
# Begin Source File

SOURCE=.\PwSafeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TanWizardDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AddEntryDlg.h
# End Source File
# Begin Source File

SOURCE=.\AddGroupDlg.h
# End Source File
# Begin Source File

SOURCE=.\DbSettingsDlg.h
# End Source File
# Begin Source File

SOURCE=.\EntryListDlg.h
# End Source File
# Begin Source File

SOURCE=.\EntryPropertiesDlg.h
# End Source File
# Begin Source File

SOURCE=.\FindInDbDlg.h
# End Source File
# Begin Source File

SOURCE=.\GetRandomDlg.h
# End Source File
# Begin Source File

SOURCE=.\IconPickerDlg.h
# End Source File
# Begin Source File

SOURCE=.\LanguagesDlg.h
# End Source File
# Begin Source File

SOURCE=.\OptionsDlg.h
# End Source File
# Begin Source File

SOURCE=.\PasswordDlg.h
# End Source File
# Begin Source File

SOURCE=.\PluginsDlg.h
# End Source File
# Begin Source File

SOURCE=.\PwGeneratorDlg.h
# End Source File
# Begin Source File

SOURCE=.\PwSafe.h
# End Source File
# Begin Source File

SOURCE=.\PwSafeDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TanWizardDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bin1.bin
# End Source File
# Begin Source File

SOURCE=.\res\cancel.bmp
# End Source File
# Begin Source File

SOURCE=.\res\clientic.bmp
# End Source File
# Begin Source File

SOURCE=.\res\disk.bmp
# End Source File
# Begin Source File

SOURCE=.\res\document.bmp
# End Source File
# Begin Source File

SOURCE=.\res\file.bmp
# End Source File
# Begin Source File

SOURCE=.\res\iconpic.ico
# End Source File
# Begin Source File

SOURCE=.\res\infoicon.bmp
# End Source File
# Begin Source File

SOURCE=.\res\key.ico
# End Source File
# Begin Source File

SOURCE=.\res\key_smal.bmp
# End Source File
# Begin Source File

SOURCE=.\res\keyhole.ico
# End Source File
# Begin Source File

SOURCE=.\res\language.bmp
# End Source File
# Begin Source File

SOURCE=.\res\locked.ico
# End Source File
# Begin Source File

SOURCE=.\res\mouse_sm.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ok.bmp
# End Source File
# Begin Source File

SOURCE=.\res\optionic.bmp
# End Source File
# Begin Source File

SOURCE=.\res\options.ico
# End Source File
# Begin Source File

SOURCE=.\res\plugins.ico
# End Source File
# Begin Source File

SOURCE=.\res\PwSafe.ico
# End Source File
# Begin Source File

SOURCE=.\res\PwSafe.rc2
# End Source File
# Begin Source File

SOURCE=.\res\random_b.bmp
# End Source File
# Begin Source File

SOURCE=.\res\search.ico
# End Source File
# Begin Source File

SOURCE=.\res\tb_about.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tb_adden.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tb_copyp.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tb_copyu.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tb_delet.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tb_edite.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tb_find.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tb_lock.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tb_new.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tb_open.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tb_save.bmp
# End Source File
# Begin Source File

SOURCE=.\res\unlocked.ico
# End Source File
# Begin Source File

SOURCE=.\res\world.ico
# End Source File
# End Group
# Begin Group "PwSafe"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PwSafe\PwCompat.cpp
# End Source File
# Begin Source File

SOURCE=.\PwSafe\PwCompat.h
# End Source File
# Begin Source File

SOURCE=.\PwSafe\PwExport.cpp
# End Source File
# Begin Source File

SOURCE=.\PwSafe\PwExport.h
# End Source File
# Begin Source File

SOURCE=.\PwSafe\PwImport.cpp
# End Source File
# Begin Source File

SOURCE=.\PwSafe\PwImport.h
# End Source File
# Begin Source File

SOURCE=.\PwSafe\PwManager.cpp
# End Source File
# Begin Source File

SOURCE=.\PwSafe\PwManager.h
# End Source File
# Begin Source File

SOURCE=.\PwSafe\PwUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\PwSafe\PwUtil.h
# End Source File
# End Group
# Begin Group "Util"

# PROP Default_Filter ""
# Begin Group "Plugins"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Util\KeePluginDef.h
# End Source File
# Begin Source File

SOURCE=.\Util\PluginMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\PluginMgr.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Util\base64.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\base64.h
# End Source File
# Begin Source File

SOURCE=.\Util\MemUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\MemUtil.h
# End Source File
# Begin Source File

SOURCE=.\Util\NewRandom.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\NewRandom.h
# End Source File
# Begin Source File

SOURCE=.\Util\PrivateConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\PrivateConfig.h
# End Source File
# Begin Source File

SOURCE=.\Util\SendKeys.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Util\SendKeys.h
# End Source File
# Begin Source File

SOURCE=.\Util\StrUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\StrUtil.h
# End Source File
# Begin Source File

SOURCE=.\Util\SysDefEx.h
# End Source File
# Begin Source File

SOURCE=.\Util\WinUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\WinUtil.h
# End Source File
# End Group
# Begin Group "Crypto"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Crypto\arcfour.cpp
# End Source File
# Begin Source File

SOURCE=.\Crypto\arcfour.h
# End Source File
# Begin Source File

SOURCE=.\Crypto\rijndael.cpp
# End Source File
# Begin Source File

SOURCE=.\Crypto\rijndael.h
# End Source File
# Begin Source File

SOURCE=.\Crypto\sha2.cpp
# End Source File
# Begin Source File

SOURCE=.\Crypto\sha2.h
# End Source File
# Begin Source File

SOURCE=.\Crypto\testimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\Crypto\testimpl.h
# End Source File
# Begin Source File

SOURCE=.\Crypto\twoclass.cpp
# End Source File
# Begin Source File

SOURCE=.\Crypto\twoclass.h
# End Source File
# Begin Source File

SOURCE=.\Crypto\twofish.cpp
# End Source File
# Begin Source File

SOURCE=.\Crypto\twofish.h
# End Source File
# End Group
# Begin Group "NewGUI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\NewGUI\amsEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\NewGUI\amsEdit.h
# End Source File
# Begin Source File

SOURCE=.\NewGUI\AutoRichEditCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\NewGUI\AutoRichEditCtrl.h
# End Source File
# Begin Source File

SOURCE=.\NewGUI\BCMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\NewGUI\BCMenu.h
# End Source File
# Begin Source File

SOURCE=.\NewGUI\BtnST.cpp
# End Source File
# Begin Source File

SOURCE=.\NewGUI\BtnST.h
# End Source File
# Begin Source File

SOURCE=.\NewGUI\CeXDib.cpp
# End Source File
# Begin Source File

SOURCE=.\NewGUI\CeXDib.h
# End Source File
# Begin Source File

SOURCE=.\NewGUI\ColourPickerXP.cpp
# End Source File
# Begin Source File

SOURCE=.\NewGUI\ColourPickerXP.h
# End Source File
# Begin Source File

SOURCE=.\NewGUI\CustomListCtrlEx.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\NewGUI\CustomListCtrlEx.h
# End Source File
# Begin Source File

SOURCE=.\NewGUI\CustomTreeCtrlEx.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\NewGUI\CustomTreeCtrlEx.h
# End Source File
# Begin Source File

SOURCE=.\NewGUI\GradientProgressCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\NewGUI\GradientProgressCtrl.h
# End Source File
# Begin Source File

SOURCE=.\NewGUI\HyperEdit.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\NewGUI\HyperEdit.h
# End Source File
# Begin Source File

SOURCE=.\NewGUI\KCSideBannerWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\NewGUI\KCSideBannerWnd.h
# End Source File
# Begin Source File

SOURCE=.\NewGUI\KCWndUtil.h
# End Source File
# Begin Source File

SOURCE=.\NewGUI\MemDC.h
# End Source File
# Begin Source File

SOURCE=.\NewGUI\NewGUICommon.cpp
# End Source File
# Begin Source File

SOURCE=.\NewGUI\NewGUICommon.h
# End Source File
# Begin Source File

SOURCE=.\NewGUI\OptionsList.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\NewGUI\OptionsList.h
# End Source File
# Begin Source File

SOURCE=.\NewGUI\SystemTray.cpp
# End Source File
# Begin Source File

SOURCE=.\NewGUI\SystemTray.h
# End Source File
# Begin Source File

SOURCE=.\NewGUI\SystemTrayEx.cpp
# End Source File
# Begin Source File

SOURCE=.\NewGUI\SystemTrayEx.h
# End Source File
# Begin Source File

SOURCE=.\NewGUI\ThemeHelperST.cpp
# End Source File
# Begin Source File

SOURCE=.\NewGUI\ThemeHelperST.h
# End Source File
# Begin Source File

SOURCE=.\NewGUI\TranslateEx.cpp
# End Source File
# Begin Source File

SOURCE=.\NewGUI\TranslateEx.h
# End Source File
# Begin Source File

SOURCE=.\NewGUI\WindowGroups.cpp
# End Source File
# Begin Source File

SOURCE=.\NewGUI\WindowGroups.h
# End Source File
# Begin Source File

SOURCE=.\NewGUI\XHyperLink.cpp
# End Source File
# Begin Source File

SOURCE=.\NewGUI\XHyperLink.h
# End Source File
# Begin Source File

SOURCE=.\NewGUI\XPStyleButtonST.cpp
# End Source File
# Begin Source File

SOURCE=.\NewGUI\XPStyleButtonST.h
# End Source File
# End Group
# End Target
# End Project
