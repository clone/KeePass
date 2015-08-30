RMDIR /S /Q WinGUI_Bin
RMDIR /S /Q WinGUI_Distrib
RMDIR /S /Q KeePassLibC
RMDIR /S /Q KeePassIPC

DEL ..\WinGUI\PwSafe.aps
DEL ..\WinGUI\PwSafe.ncb
DEL /A:H ..\WinGUI\PwSafe.suo
DEL ..\WinGUI\*.user

DEL ..\KeePassLibC\KeePassLib.aps
DEL ..\KeePassLibC\KeePassLib.ncb
DEL /A:H ..\KeePassLibC\KeePassLib.suo
DEL ..\KeePassLibC\*.user

DEL ..\KeePassIPC\*.user

DEL ..\KeePass.ncb
DEL /A:H ..\KeePass.suo

CLS