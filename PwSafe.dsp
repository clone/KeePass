# Microsoft Developer Studio Project File - Name="PwSafe" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PwSafe - Win32 Debug
!MESSAGE Dies ist kein g�ltiges Makefile. Zum Erstellen dieses Projekts mit\
 NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und f�hren Sie den\
 Befehl
!MESSAGE 
!MESSAGE NMAKE /f "PwSafe.mak".
!MESSAGE 
!MESSAGE Sie k�nnen beim Ausf�hren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "PwSafe.mak" CFG="PwSafe - Win32 Debug"
!MESSAGE 
!MESSAGE F�r die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "PwSafe - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "PwSafe - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
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
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
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
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
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

SOURCE=.\PwGeneratorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PwSafe.cpp
# End Source File
# Begin Source File

SOURCE=.\PwSafe.rc

!IF  "$(CFG)" == "PwSafe - Win32 Release"

!ELSEIF  "$(CFG)" == "PwSafe - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PwSafeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
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
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\cancel.bmp
# End Source File
# Begin Source File

SOURCE=.\res\clientic.bmp
# End Source File
# Begin Source File

SOURCE=.\res\document.bmp
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

SOURCE=.\res\language.bmp
# End Source File
# Begin Source File

SOURCE=.\res\mainfram.ico
# End Source File
# Begin Source File

SOURCE=.\res\mouse_sm.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ok.bmp
# End Source File
# Begin Source File

SOURCE=.\res\options.ico
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

SOURCE=.\res\world.ico
# End Source File
# End Group
# Begin Group "PwSafe"

# PROP Default_Filter ""
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
# End Group
# Begin Group "Util"

# PROP Default_Filter ""
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
# End Group
# Begin Group "NewGUI"

# PROP Default_Filter ""
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

SOURCE=.\NewGUI\CustomListCtrlEx.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\NewGUI\CustomListCtrlEx.h
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

SOURCE=.\NewGUI\NewGUICommon.cpp
# End Source File
# Begin Source File

SOURCE=.\NewGUI\NewGUICommon.h
# End Source File
# Begin Source File

SOURCE=.\NewGUI\ShadeButtonST.cpp
# End Source File
# Begin Source File

SOURCE=.\NewGUI\ShadeButtonST.h
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

SOURCE=.\NewGUI\TranslateEx.cpp
# End Source File
# Begin Source File

SOURCE=.\NewGUI\TranslateEx.h
# End Source File
# Begin Source File

SOURCE=.\NewGUI\WzComboBox.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\NewGUI\WzComboBox.h
# End Source File
# End Group
# End Target
# End Project
