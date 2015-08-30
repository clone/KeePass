; *** Inno Setup version 4.1.8 Czech messages ***
;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/is3rdparty.php
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;
; $jrsoftware: issrc/Files/Default.isl,v 1.53 2004/02/25 01:55:24 jr Exp $
;
; {***********************************************}
; { Translated by Ing. Ivo Bauer                  }
; {                                               }
; { Web site: http://www.ozm.cz/ivobauer/         }
; { E-mail:   ivo.bauer@tiscali.cz | bauer@ozm.cz }
; {***********************************************}
;
; {***********************************************}
; { Prepared for KeePass from language contrib of }
; { jrsoftware.org/files/istrans/                 }
; {                                               }
; { modified and updated by Lubos Stanek          }
; {                                               }
; { Web site: http://sourceforge.net/users/lubek/ }
; { E-mail:   lubek@users.sourceforge.net         }
; {***********************************************}

[LangOptions]
LanguageName=�e�tina
LanguageID=$0405
; If the language you are translating to requires special font faces or
; sizes, uncomment any of the following entries and change them accordingly.
;DialogFontName=
;DialogFontSize=8
;WelcomeFontName=Verdana
;WelcomeFontSize=12
;TitleFontName=Arial
;TitleFontSize=29
;CopyrightFontName=Arial
;CopyrightFontSize=8

[Messages]

; *** Application titles
SetupAppTitle=Pr�vodce instalac�
SetupWindowTitle=Pr�vodce instalac� - %1
UninstallAppTitle=Pr�vodce odinstalac�
UninstallAppFullTitle=Pr�vodce odinstalac� - %1

; *** Misc. common
InformationTitle=Informace
ConfirmTitle=Potvrzen�
ErrorTitle=Chyba

; *** SetupLdr messages
SetupLdrStartupMessage=Tento pr�vodce nainstaluje produkt %1. P�ejete si pokra�ovat?
LdrCannotCreateTemp=Nelze vytvo�it do�asn� soubor. Pr�vodce instalac� bude ukon�en
LdrCannotExecTemp=Nelze spustit soubor v do�asn� slo�ce. Pr�vodce instalac� bude ukon�en

; *** Startup error messages
LastErrorMessage=%1.%n%nChyba %2: %3
SetupFileMissing=Instala�n� slo�ka neobsahuje soubor %1. Opravte, pros�m, tuto chybu nebo si po�i�te novou kopii tohoto produktu.
SetupFileCorrupt=Soubory pr�vodce instalac� jsou po�kozeny. Po�i�te si, pros�m, novou kopii tohoto produktu.
SetupFileCorruptOrWrongVer=Soubory pr�vodce instalac� jsou po�kozeny nebo se neslu�uj� s touto verz� pr�vodce. Opravte, pros�m, tuto chybu nebo si po�i�te novou kopii tohoto produktu.
NotOnThisPlatform=Tento produkt nelze spustit ve %1.
OnlyOnThisPlatform=Tento produkt mus� b�t spu�t�n ve %1.
WinVersionTooLowError=Tento produkt vy�aduje %1 verzi %2 nebo vy���.
WinVersionTooHighError=Tento produkt nelze nainstalovat ve %1 verzi %2 nebo vy���.
AdminPrivilegesRequired=P�i instalaci tohoto produktu mus�te b�t p�ihl�eni s pr�vy administr�tora.
PowerUserPrivilegesRequired=P�i instalaci tohoto produktu mus�te b�t p�ihl�eni s pr�vy administr�tora nebo �lena skupiny Power Users.
SetupAppRunningError=Pr�vodce instalac� zjistil, �e aplikace %1 je ji� spu�t�na.%n%nUkon�ete, pros�m, v�echny spu�t�n� instance produktu a pokra�ujte tla��tkem OK nebo instalaci ukon�ete tla��tkem Storno.
UninstallAppRunningError=Pr�vodce odinstalac� zjistil, �e aplikace %1 je ji� spu�t�na.%n%nUkon�ete, pros�m, v�echny spu�t�n� instance produktu a pokra�ujte tla��tkem OK nebo instalaci ukon�ete tla��tkem Storno.

; *** Misc. errors
ErrorCreatingDir=Pr�vodce instalac� nemohl vytvo�it slo�ku "%1"
ErrorTooManyFilesInDir=Nelze vytvo�it soubor ve slo�ce "%1", proto�e tato slo�ka ji� obsahuje p��li� mnoho soubor�

; *** Setup common messages
ExitSetupTitle=Ukon�it pr�vodce instalac�
ExitSetupMessage=Instalace nebyla zcela dokon�ena. Jestli�e nyn� ukon��te pr�vodce instalac�, produkt nebude nainstalov�n.%n%nPr�vodce instalac� m��ete znovu spustit jindy a dokon�it instalaci.%n%nChcete ukon�it pr�vodce instalac�?
AboutSetupMenuItem=&O pr�vodci instalac�...
AboutSetupTitle=O pr�vodci instalac�
AboutSetupMessage=%1 verze %2%n%3%n%n%1 dom�c� str�nka:%n%4
AboutSetupNote=

; *** Buttons
ButtonBack=< &Zp�t
ButtonNext=&Dal�� >
ButtonInstall=&Instalovat
ButtonOK=OK
ButtonCancel=Storno
ButtonYes=&Ano
ButtonYesToAll=Ano &v�em
ButtonNo=&Ne
ButtonNoToAll=N&e v�em
ButtonFinish=&Dokon�it
ButtonBrowse=&Proch�zet...
ButtonWizardBrowse=&Proch�zet...
ButtonNewFolder=&Vytvo�it novou slo�ku

; *** "Select Language" dialog messages
SelectLanguageTitle=V�b�r jazyka pr�vodce instalac�
SelectLanguageLabel=Zvolte jazyk, kter� se m� pou��t p�i instalaci:

; *** Common wizard text
ClickNext=Pokra�ujte tla��tkem Dal�� nebo tla��tkem Storno ukon�ete pr�vodce instalac�.
BeveledLabel=
BrowseDialogTitle=Vyhledat slo�ku
BrowseDialogLabel=Z n�e uveden�ho seznamu vyberte slo�ku a klikn�te na OK.
NewFolderName=Nov� slo�ka

; *** "Welcome" wizard page
WelcomeLabel1=V�t� V�s pr�vodce instalac� produktu [name].
WelcomeLabel2=Produkt [name/ver] bude nainstalov�n na V� po��ta�.%n%nD��ve ne� budete pokra�ovat, je doporu�eno ukon�it ve�ker� spu�t�n� aplikace.

; *** "Password" wizard page
WizardPassword=Heslo
PasswordLabel1=Tato instalace je chr�n�na heslem.
PasswordLabel3=Pros�m, zadejte heslo a pokra�ujte tla��tkem Dal��. P�i zad�v�n� hesla rozli�ujte mal� a velk� p�smena.
PasswordEditLabel=&Heslo:
IncorrectPassword=Zadan� heslo nen� spr�vn�. Pros�m, zkuste to znovu.

; *** "License Agreement" wizard page
WizardLicense=Licen�n� smlouva
LicenseLabel=Pros�m, p�e�t�te si pozorn� n�sleduj�c� d�le�it� informace d��ve, ne� budete pokra�ovat.
LicenseLabel3=Pros�m, p�e�t�te si tuto licen�n� smlouvu. Pr�vodce instalac� bude pokra�ovat jen po va�em souhlasu s podm�nkami t�to smlouvy.
LicenseAccepted=&Souhlas�m s podm�nkami licen�n� smlouvy
LicenseNotAccepted=&Nesouhlas�m s podm�nkami licen�n� smlouvy

; *** "Information" wizard pages
WizardInfoBefore=Informace
InfoBeforeLabel=Pros�m, p�e�t�te si pozorn� n�sleduj�c� d�le�it� informace d��ve, ne� budete pokra�ovat.
InfoBeforeClickLabel=Tla��tkem Dal�� pokra�ujte v instala�n�m procesu.
WizardInfoAfter=Informace
InfoAfterLabel=Pros�m, p�e�t�te si pozorn� n�sleduj�c� d�le�it� informace d��ve, ne� budete pokra�ovat.
InfoAfterClickLabel=Tla��tkem Dal�� pokra�ujte v instala�n�m procesu.

; *** "User Information" wizard page
WizardUserInfo=Informace o u�ivateli
UserInfoDesc=Pros�m, zadejte va�e �daje.
UserInfoName=&U�ivatelsk� jm�no:
UserInfoOrg=&Spole�nost:
UserInfoSerial=S�&riov� ��slo:
UserInfoNameRequired=U�ivatelsk� jm�no mus� b�t zad�no.

; *** "Select Destination Location" wizard page
WizardSelectDir=Zvolte c�lov� um�st�n�
SelectDirDesc=Kam m� b�t produkt [name] nainstalov�n?
SelectDirLabel3=Pr�vodce nainstaluje produkt [name] do n�sleduj�c� slo�ky.
SelectDirBrowseLabel=Pokra�ujte tla��tkem Dal��. Chcete-li zvolit jinou slo�ku, vyberte ji stiskem tla��tka Proch�zet.
DiskSpaceMBLabel=Instalace vy�aduje nejm�n� [mb] MB voln�ho m�sta na disku.
ToUNCPathname=Pr�vodce instalac� nem��e instalovat do cesty UNC. Pokud se pokou��te instalovat na s�, budete pot�ebovat p�ipojit s�ovou jednotku.
InvalidPath=Mus�te zadat �plnou cestu v�etn� p�smene jednotky; nap��klad:%n%nC:\Aplikace%n%nnebo cestu UNC ve tvaru:%n%n\\server\sd�len� slo�ka
InvalidDrive=V�mi zvolen� jednotka nebo cesta UNC neexistuje nebo nen� dostupn�. Pros�m, zvolte jin� um�st�n�.
DiskSpaceWarningTitle=Nedostatek m�sta na disku
DiskSpaceWarning=Pr�vodce instalac� vy�aduje nejm�n� %1 KB voln�ho m�sta pro instalaci produktu, ale na zvolen� jednotce je dostupn�ch pouze %2 KB.%n%nChcete p�esto pokra�ovat?
DirNameTooLong=N�zev slo�ky nebo cesta jsou p��li� dlouh�.
InvalidDirName=N�zev slo�ky nen� platn�.
BadDirName32=N�zvy slo�ek nemohou obsahovat ��dn� z n�sleduj�c�ch znak�:%n%n%1
DirExistsTitle=Slo�ka existuje
DirExists=Slo�ka:%n%n%1%n%nji� existuje. M� se p�esto instalovat do t�to slo�ky?
DirDoesntExistTitle=Slo�ka neexistuje
DirDoesntExist=Slo�ka:%n%n%1%n%nneexistuje. M� b�t tato slo�ka vytvo�ena?

; *** "Select Components" wizard page
WizardSelectComponents=Zvolte sou��sti
SelectComponentsDesc=Jak� sou��sti maj� b�t nainstalov�ny?
SelectComponentsLabel2=Za�krtn�te sou��sti, kter� maj� b�t nainstalov�ny; sou��sti, kter� se nemaj� instalovat, ponechte neza�krtnut�. Pokra�ujte tla��tkem Dal��.
FullInstallation=�pln� instalace
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Kompaktn� instalace
CustomInstallation=Voliteln� instalace
NoUninstallWarningTitle=Sou��sti existuj�
NoUninstallWarning=Pr�vodce instalac� zjistil, �e n�sleduj�c� sou��sti jsou ji� na Va�em po��ta�i nainstalov�ny:%n%n%1%n%nJejich nezahrnut� do v�b�ru je neodinstaluje.%n%nChcete p�esto pokra�ovat?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Vybran� sou��sti vy�aduj� nejm�n� [mb] MB m�sta na disku.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Zvolte dal�� �lohy
SelectTasksDesc=Kter� dal�� �lohy maj� b�t provedeny?
SelectTasksLabel2=Zvolte dal�� �lohy, kter� maj� b�t provedeny v pr�b�hu instalace produktu [name] a pokra�ujte tla��tkem Dal��.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Vyberte slo�ku v nab�dce Start
SelectStartMenuFolderDesc=Kam m� pr�vodce um�stit z�stupce aplikace?
SelectStartMenuFolderLabel3=Pr�vodce vytvo�� z�stupce aplikace v n�sleduj�c� slo�ce nab�dky Start.
SelectStartMenuFolderBrowseLabel=Pokra�ujte tla��tkem Dal��. Chcete-li zvolit jinou slo�ku, vyberte ji stiskem tla��tka Proch�zet.
NoIconsCheck=&Nevytv��et ��dn� z�stupce
MustEnterGroupName=Mus�te zadat n�zev slo�ky.
GroupNameTooLong=N�zev slo�ky nebo cesta jsou p��li� dlouh�.
InvalidGroupName=N�zev slo�ky nen� platn�.
BadGroupName=N�zev slo�ky nem��e obsahovat ��dn� z n�sleduj�c�ch znak�:%n%n%1
NoProgramGroupCheck2=&Nevytv��et slo�ku v nab�dce Start

; *** "Ready to Install" wizard page
WizardReady=Instalace je p�ipravena
ReadyLabel1=Pr�vodce instalac� je nyn� p�ipraven nainstalovat aplikaci [name] na V� po��ta�.
ReadyLabel2a=Pokra�ujte v instala�n�m procesu tla��tkem Instalovat nebo pou�ijte tla��tko Zp�t, p�ejete-li si zm�nit n�kter� nastaven� instalace.
ReadyLabel2b=Pokra�ujte v instala�n�m procesu tla��tkem Instalovat.
ReadyMemoUserInfo=Informace o u�ivateli:
ReadyMemoDir=C�lov� um�st�n�:
ReadyMemoType=Typ instalace:
ReadyMemoComponents=Vybran� sou��sti:
ReadyMemoGroup=Slo�ka v nab�dce Start:
ReadyMemoTasks=Dal�� �lohy:

; *** "Preparing to Install" wizard page
WizardPreparing=P��prava k instalaci
PreparingDesc=Pr�vodce instalac� p�ipravuje instalaci produktu [name] na V� po��ta�.
PreviousInstallNotCompleted=Proces instalace/odinstalace p�edchoz�ho produktu nebyl zcela dokon�en. Dokon�en� p�edchoz�ho procesu instalace vy�aduje restart tohoto po��ta�e.%n%nPo proveden� restartu po��ta�e spus�te znovu tohoto pr�vodce instalac�, aby bylo mo�n� dokon�it instalaci produktu [name].
CannotContinue=Pr�vodce instalac� nem��e pokra�ovat. Pros�m, tla��tkem Storno ukon�ete pr�vodce instalac�.

; *** "Installing" wizard page
WizardInstalling=Instalov�n�
InstallingLabel=�ekejte pros�m, dokud pr�vodce instalac� nedokon�� instalaci produktu [name] na V� po��ta�.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Dokon�uje se instalace produktu [name]
FinishedLabelNoIcons=Pr�vodce instalac� dokon�il instalaci produktu [name] na V� po��ta�.
FinishedLabel=Pr�vodce instalac� dokon�il instalaci produktu [name] na V� po��ta�. Produkt lze spustit pomoc� nainstalovan�ch z�stupc�.
ClickFinish=Tla��tkem Dokon�it ukon��te pr�vodce instalac�.
FinishedRestartLabel=Pro dokon�en� instalace produktu [name] je nezbytn�, aby pr�vodce instalac� restartoval V� po��ta�. Chcete jej restartovat nyn�?
FinishedRestartMessage=Pro dokon�en� instalace produktu [name] je nezbytn�, aby pr�vodce instalac� restartoval V� po��ta�.%n%nChcete jej restartovat nyn�?
ShowReadmeCheck=Ano, chci zobrazit dokument "�TIMNE"
YesRadio=&Ano, chci nyn� restartovat po��ta�
NoRadio=&Ne, po��ta� restartuji pozd�ji
; used for example as 'Run MyProg.exe'
RunEntryExec=Spustit %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Zobrazit %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Pr�vodce instalac� vy�aduje dal�� disk
SelectDiskLabel2=Vlo�te, pros�m, disk %1 a klikn�te na OK.%n%nPokud se soubory z tohoto disku nach�zej� v jin� slo�ce, ne� v t�, kter� je zobrazena n�e, pak zadejte spr�vnou cestu nebo ji vyhledejte tla��tkem Proch�zet.
PathLabel=&Cesta:
FileNotInDir2=Soubor "%1" nelze naj�t v "%2". Vlo�te, pros�m, spr�vn� disk nebo zvolte jinou slo�ku.
SelectDirectoryLabel=Pros�m, specifikujte um�st�n� dal��ho disku.

; *** Installation phase messages
SetupAborted=Instalace nebyla zcela dokon�ena.%n%nPros�m, opravte chybu a spus�te pr�vodce instalac� znovu.
EntryAbortRetryIgnore=Tla��tkem Opakovat akci zopakujete, tla��tkem P�esko�it akci vynech�te nebo tla��tkem P�eru�it instalaci p�eru��te.

; *** Installation status messages
StatusCreateDirs=Vytv��ej� se slo�ky...
StatusExtractFiles=Extrahuj� se soubory...
StatusCreateIcons=Vytv��ej� se z�stupci...
StatusCreateIniEntries=Vytv��ej� se INI polo�ky...
StatusCreateRegistryEntries=Vytv��ej� se z�znamy v registru...
StatusRegisterFiles=Registruj� se soubory...
StatusSavingUninstall=Ukl�daj� se informace pro odinstalaci...
StatusRunProgram=Dokon�uje se instalace...
StatusRollback=Vracej� se zp�t proveden� zm�ny...

; *** Misc. errors
ErrorInternal2=Intern� chyba: %1
ErrorFunctionFailedNoCode=%1 selhala
ErrorFunctionFailed=%1 selhala; k�d %2
ErrorFunctionFailedWithMessage=%1 selhala; k�d %2.%n%3
ErrorExecutingProgram=Nelze spustit soubor:%n%1

; *** Registry errors
ErrorRegOpenKey=Chyba p�i otev�r�n� kl��e registru:%n%1\%2
ErrorRegCreateKey=Chyba p�i vytv��en� kl��e registru:%n%1\%2
ErrorRegWriteKey=Chyba p�i z�pisu do kl��e registru:%n%1\%2

; *** INI errors
ErrorIniEntry=Chyba p�i vytv��en� INI z�znamu v souboru "%1".

; *** File copying errors
FileAbortRetryIgnore=Tla��tkem Opakovat akci zopakujete, tla��tkem P�esko�it akci tohoto souboru vynech�te (nedoporu�uje se) nebo tla��tkem P�eru�it instalaci p�eru��te.
FileAbortRetryIgnore2=Tla��tkem Opakovat akci zopakujete, tla��tkem P�esko�it p�esto pokra�ujete (nedoporu�uje se) nebo tla��tkem P�eru�it instalaci p�eru��te.
SourceIsCorrupted=Zdrojov� soubor je po�kozen
SourceDoesntExist=Zdrojov� soubor "%1" neexistuje
ExistingFileReadOnly=Existuj�c� soubor je ur�en pouze pro �ten�.%n%nTla��tkem Opakovat odstran�te atribut "pouze pro �ten�" a akci zopakujete, tla��tkem P�esko�it tento soubor p�esko��te nebo tla��tkem P�eru�it instalaci p�eru��te.
ErrorReadingExistingDest=Do�lo k chyb� p�i pokusu o �ten� existuj�c�ho souboru:
FileExists=Soubor ji� existuje.%n%nM� b�t pr�vodcem instalace p�eps�n?
ExistingFileNewer=Existuj�c� soubor je nov�j�� ne� ten, kter� se pr�vodce instalac� pokou�� nainstalovat. Je doporu�eno ponechat existuj�c� soubor.%n%nChcete ponechat existuj�c� soubor?
ErrorChangingAttr=Do�lo k chyb� p�i pokusu o zm�nu atribut� existuj�c�ho souboru:
ErrorCreatingTemp=Do�lo k chyb� p�i pokusu o vytvo�en� souboru v c�lov� slo�ce:
ErrorReadingSource=Do�lo k chyb� p�i pokusu o �ten� zdrojov�ho souboru:
ErrorCopying=Do�lo k chyb� p�i pokusu o zkop�rov�n� souboru:
ErrorReplacingExistingFile=Do�lo k chyb� p�i pokusu o nahrazen� existuj�c�ho souboru:
ErrorRestartReplace=Funkce "RestartReplace" instal�toru selhala:
ErrorRenamingTemp=Do�lo k chyb� p�i pokusu o p�ejmenov�n� souboru v c�lov� slo�ce:
ErrorRegisterServer=Nelze zaregistrovat DLL/OCX: %1
ErrorRegisterServerMissingExport=Nelze nal�zt export DllRegisterServer
ErrorRegisterTypeLib=Nelze zaregistrovat typovou knihovnu: %1

; *** Post-installation errors
ErrorOpeningReadme=Do�lo k chyb� p�i pokusu o otev�en� dokumentu "�TIMNE".
ErrorRestartingComputer=Pr�vodci instalace se nepoda�ilo restartovat V� po��ta�. Prove�te, pros�m, restart ru�n�.

; *** Uninstaller messages
UninstallNotFound=Soubor "%1" neexistuje. Produkt nelze odinstalovat.
UninstallOpenError=Soubor "%1" nelze otev��t. Produkt nelze odinstalovat.
UninstallUnsupportedVer=Pr�vodci odinstalac� se nepoda�ilo rozpoznat form�t souboru obsahuj�c�ho informace k odinstalaci produktu "%1". Produkt nelze odinstalovat
UninstallUnknownEntry=V souboru obsahuj�c�m informace pro odinstalaci produktu byla zji�t�na nezn�m� polo�ka (%1)
ConfirmUninstall=Ur�it� chcete �pln� odinstalovat produkt %1 a v�echny jeho sou��sti?
OnlyAdminCanUninstall=Odinstalace tohoto produktu vy�aduje pr�va administr�tora.
UninstallStatusLabel=�ekejte, pros�m, dokud produkt %1 nebude odinstalov�n z Va�eho po��ta�e.
UninstalledAll=Produkt %1 byl �sp�n� odinstalov�n z Va�eho po��ta�e.
UninstalledMost=Produkt %1 byl odinstalov�n z Va�eho po��ta�e.%n%nN�kter� jeho sou��sti se v�ak nepoda�ilo odinstalovat. Ty lze odstranit ru�n�.
UninstalledAndNeedsRestart=K dokon�en� odinstalace produktu %1 je nezbytn�, aby pr�vodce odinstalac� restartoval V� po��ta�.%n%nChcete jej restartovat nyn�?
UninstallDataCorrupted=Soubor "%1" je po�kozen. Produkt nelze odinstalovat

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Odebrat sd�len� soubor?
ConfirmDeleteSharedFile2=Syst�m indikuje, �e n�sleduj�c� sd�len� soubor nen� pou��v�n ��dn�mi jin�mi aplikacemi. M� b�t tento sd�len� soubor pr�vodcem odinstalac� odstran�n?%n%nPokud n�kter� aplikace tento soubor pou��vaj�, pak po jeho odstran�n� nemus� pracovat spr�vn�. Nejste-li si jisti, zvolte Ne. Ponech�n� tohoto souboru ve Va�em syst�mu nezp�sob� ��dnou �kodu.
SharedFileNameLabel=N�zev souboru:
SharedFileLocationLabel=Um�st�n�:
WizardUninstalling=Stav odinstalace
StatusUninstalling=Prob�h� odinstalace %1...
