CLS
MKDIR WinGUI_Distrib
COPY /B WinGUI_Bin\Debug\KeePass.exe /B WinGUI_Distrib\KeePass.exe

DEL ..\Ext\WinGUI_API\KeePass.lib
COPY /B WinGUI_Bin\Debug\KeePass.lib /B ..\Ext\WinGUI_API\KeePass.lib
DEL ..\Ext\WinGUI_API\KeePass.exp
COPY /B WinGUI_Bin\Debug\KeePass.exp /B ..\Ext\WinGUI_API\KeePass.exp

DEL WinGUI_Distrib\KeePass.chm
COPY /B ..\Docs\WinGUI_CHM\KeePass.chm /B WinGUI_Distrib\KeePass.chm

DEL WinGUI_Distrib\License.txt
COPY /B ..\Docs\License.txt /B WinGUI_Distrib\License.txt

DEL WinGUI_Distrib\KeePass.ini
COPY /B ..\Ext\KeePass.ini /B WinGUI_Distrib\KeePass.ini
