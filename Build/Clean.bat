RMDIR /S /Q WinGUI_Bin
RMDIR /S /Q WinGUI_Distrib
RMDIR /S /Q KeePassLibC

DEL ..\WinGUI\PwSafe.aps
DEL ..\WinGUI\PwSafe.ncb
DEL /A:H ..\WinGUI\PwSafe.suo
DEL ..\WinGUI\*.user
DEL ..\KeePassLibC\KeePassLib.ncb
DEL /A:H ..\KeePassLibC\KeePassLib.suo
DEL ..\KeePassLibC\*.user

DEL ..\KeePass.ncb
DEL /A:H ..\KeePass.suo

CLS