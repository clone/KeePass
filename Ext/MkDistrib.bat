ECHO OFF
CLS

IF EXIST ..\Debug\KeePass.exe COPY /B ..\Debug\KeePass.exe /B ..\Distrib\KeePass.exe
IF EXIST ..\Release\KeePass.exe COPY /B ..\Release\KeePass.exe /B ..\Distrib\KeePass.exe

DEL KeePass.lib
COPY /B ..\Release\KeePass.lib /B KeePass.lib
DEL KeePass.exp
COPY /B ..\Release\KeePass.exp /B KeePass.exp

DEL ..\Distrib\KeePass.chm
COPY /B Doc\KeePass.chm /B ..\Distrib\KeePass.chm

DEL ..\Distrib\License.txt
COPY /B License.txt /B ..\Distrib\License.txt

DEL ..\Distrib\KeePass.ini
COPY /B KeePass.ini /B ..\Distrib\KeePass.ini

MkTrans.bat
