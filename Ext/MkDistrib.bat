DEL German.lng
Util\CprLang.exe GermanSrc.lng German.lng

DEL ..\Debug\German.lng
COPY /B German.lng /B ..\Debug\German.lng

DEL ..\Release\German.lng
COPY /B German.lng /B ..\Release\German.lng

DEL ..\Distrib\German.lng
COPY /B German.lng /B ..\Distrib\German.lng

IF EXIST ..\Debug\KeePass.exe COPY /B ..\Debug\KeePass.exe /B ..\Distrib\KeePass.exe
IF EXIST ..\Release\KeePass.exe COPY /B ..\Release\KeePass.exe /B ..\Distrib\KeePass.exe

DEL ..\Distrib\KeePass.html
COPY /B KeePass.html /B ..\Distrib\KeePass.html

DEL ..\Distrib\License.html
COPY /B License.html /B ..\Distrib\License.html

DEL ..\Distrib\KeePass.ini
COPY /B KeePass.ini /B ..\Distrib\KeePass.ini
