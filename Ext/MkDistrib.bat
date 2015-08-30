ECHO OFF
CLS

DEL German.lng
Util\CprLang.exe GermanSrc.lng German.lng

DEL Swedish.lng
Util\CprLang.exe SwedishSrc.lng Swedish.lng

DEL French.lng
Util\CprLang.exe FrenchSrc.lng French.lng

DEL Dutch.lng
Util\CprLang.exe DutchSrc.lng Dutch.lng

DEL ..\Debug\German.lng
COPY /B German.lng /B ..\Debug\German.lng
DEL ..\Debug\French.lng
COPY /B French.lng /B ..\Debug\French.lng
DEL ..\Debug\Dutch.lng
COPY /B Dutch.lng /B ..\Debug\Dutch.lng
DEL ..\Debug\Swedish.lng
COPY /B Swedish.lng /B ..\Debug\Swedish.lng

DEL ..\Release\German.lng
COPY /B German.lng /B ..\Release\German.lng
DEL ..\Release\French.lng
COPY /B French.lng /B ..\Release\French.lng
DEL ..\Release\Dutch.lng
COPY /B Dutch.lng /B ..\Release\Dutch.lng
DEL ..\Release\Swedish.lng
COPY /B Swedish.lng /B ..\Release\Swedish.lng

IF EXIST ..\Debug\KeePass.exe COPY /B ..\Debug\KeePass.exe /B ..\Distrib\KeePass.exe
IF EXIST ..\Release\KeePass.exe COPY /B ..\Release\KeePass.exe /B ..\Distrib\KeePass.exe

DEL ..\Distrib\KeePass.html
COPY /B KeePass.html /B ..\Distrib\KeePass.html

DEL ..\Distrib\License.html
COPY /B License.html /B ..\Distrib\License.html

DEL ..\Distrib\KeePass.ini
COPY /B KeePass.ini /B ..\Distrib\KeePass.ini
