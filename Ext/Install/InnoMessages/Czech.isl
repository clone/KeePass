; *** Inno Setup version 4.1.8+ Czech messages ***
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
; {                                               }
; { Translated by Ing. Ivo Bauer                  }
; {                                               }
; { Web site: http://www.ozm.cz/ivobauer/         }
; { E-mail:   ivo.bauer@tiscali.cz | bauer@ozm.cz }
; {                                               }
; {***********************************************}
;

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
ConfirmTitle=Dotaz
ErrorTitle=Chyba

; *** SetupLdr messages
SetupLdrStartupMessage=Toto je pr�vodce instalac� produktu %1. P�ejete si pokra�ovat?
LdrCannotCreateTemp=Nelze vytvo�it do�asn� soubor. Pr�vodce instalac� bude ukon�en
LdrCannotExecTemp=Nelze spustit soubor v do�asn� slo�ce. Pr�vodce instalac� bude ukon�en

; *** Startup error messages
LastErrorMessage=%1.%n%nChyba %2: %3
SetupFileMissing=Instala�n� slo�ka neobsahuje soubor %1. Opravte, pros�m, tuto chybu nebo si opat�ete novou kopii tohoto produktu.
SetupFileCorrupt=Soubory pr�vodce instalac� jsou po�kozeny. Opat�ete si, pros�m, novou kopii tohoto produktu.
SetupFileCorruptOrWrongVer=Soubory pr�vodce instalac� jsou po�kozeny nebo se neslu�uj� s touto verz� pr�vodce instalac�. Opravte, pros�m, tuto chybu nebo si opat�ete novou kopii tohoto produktu.
NotOnThisPlatform=Tento produkt nelze spustit pod %1.
OnlyOnThisPlatform=Tento produkt mus� b�t spu�t�n pod %1.
WinVersionTooLowError=Tento produkt vy�aduje %1 verze %2 nebo vy���.
WinVersionTooHighError=Tento produkt nelze nainstalovat ve %1 verze %2 nebo vy���.
AdminPrivilegesRequired=K proveden� instalace tohoto produktu mus�te b�t p�ihl�en(a) jako administr�tor.
PowerUserPrivilegesRequired=K proveden� instalace tohoto produktu mus�te b�t p�ihl�en(a) jako administr�tor nebo �len skupiny Power Users.
SetupAppRunningError=Pr�vodce instalac� zjistil, �e %1 je nyn� spu�t�n.%n%nUkon�ete, pros�m, v�echny spu�t�n� instance tohoto produktu a klepn�te na OK pro pokra�ov�n� nebo na Storno pro ukon�en�.
UninstallAppRunningError=Pr�vodce odinstalac� zjistil, �e %1 je nyn� spu�t�n.%n%nUkon�ete, pros�m, v�echny spu�t�n� instance tohoto produktu a klepn�te na OK pro pokra�ov�n� nebo na Storno pro  ukon�en�.

; *** Misc. errors
ErrorCreatingDir=Pr�vodce instalac� nemohl vytvo�it slo�ku "%1"
ErrorTooManyFilesInDir=Nelze vytvo�it soubor ve slo�ce "%1", proto�e tato slo�ka ji� obsahuje p��li� mnoho soubor�

; *** Setup common messages
ExitSetupTitle=Ukon�it pr�vodce instalac�
ExitSetupMessage=Instalace nebyla zcela dokon�ena. Jestli�e nyn� ukon��te pr�vodce instalac�, produkt nebude nainstalov�n.%n%nPr�vodce instalac� m��ete znovu spustit pozd�ji a dokon�it tak instalaci.%n%nUkon�it pr�vodce instalac�?
AboutSetupMenuItem=&O pr�vodci instalac�...
AboutSetupTitle=O pr�vodci instalac�
AboutSetupMessage=%1 verze %2%n%3%n%n%1 domovsk� str�nka:%n%4
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
SelectLanguageTitle=Zvolit jazyk pr�vodce instalac�
SelectLanguageLabel=Zvolte jazyk, kter� se m� pou��t p�i instalaci:

; *** Common wizard text
ClickNext=Klepn�te na Dal�� pro pokra�ov�n� nebo na Storno pro ukon�en� pr�vodce instalac�.
BeveledLabel=
BrowseDialogTitle=Vyhledat slo�ku
BrowseDialogLabel=Z n�e uveden�ho seznamu vyberte slo�ku a klepn�te na OK.
NewFolderName=Nov� slo�ka

; *** "Welcome" wizard page
WelcomeLabel1=V�t� V�s pr�vodce instalac� produktu [name].
WelcomeLabel2=[name/ver] bude nainstalov�n na V� po��ta�.%n%nDoporu�uje se ukon�it ve�ker� spu�t�n� aplikace p�edt�m, ne� budete pokra�ovat.

; *** "Password" wizard page
WizardPassword=Heslo
PasswordLabel1=Tato instalace je chr�n�na heslem.
PasswordLabel3=Pros�m, zadejte heslo a klepn�te na Dal�� pro pokra�ov�n�. P�i zad�v�n� hesla rozli�ujte mal� a velk� p�smena.
PasswordEditLabel=&Heslo:
IncorrectPassword=Zadan� heslo nen� spr�vn�. Pros�m, zkuste to znovu.

; *** "License Agreement" wizard page
WizardLicense=Licen�n� ujedn�n�
LicenseLabel=Pros�m, p�e�t�te si pozorn� tyto d�le�it� informace p�edt�m, ne� budete pokra�ovat.
LicenseLabel3=Pros�m, p�e�t�te si toto Licen�n� ujedn�n�. Mus�te souhlasit s podm�nkami tohoto ujedn�n�, aby mohl instala�n� proces pokra�ovat.
LicenseAccepted=&Souhlas�m s podm�nkami Licen�n�ho ujedn�n�
LicenseNotAccepted=&Nesouhlas�m s podm�nkami Licen�n�ho ujedn�n�

; *** "Information" wizard pages
WizardInfoBefore=Informace
InfoBeforeLabel=Pros�m, p�e�t�te si pozorn� tyto d�le�it� informace p�edt�m, ne� budete pokra�ovat.
InfoBeforeClickLabel=Klepn�te na Dal�� pro pokra�ov�n� instala�n�ho procesu.
WizardInfoAfter=Informace
InfoAfterLabel=Pros�m, p�e�t�te si pozorn� tyto d�le�it� informace p�edt�m, ne� budete pokra�ovat.
InfoAfterClickLabel=Klepn�te na Dal�� pro pokra�ov�n� instala�n�ho procesu.

; *** "User Information" wizard page
WizardUserInfo=Informace o u�ivateli
UserInfoDesc=Pros�m, zadejte po�adovan� informace.
UserInfoName=&U�ivatelsk� jm�no:
UserInfoOrg=&Spole�nost:
UserInfoSerial=S�&riov� ��slo:
UserInfoNameRequired=U�ivatelsk� jm�no mus� b�t zad�no.

; *** "Select Destination Location" wizard page
WizardSelectDir=Zvolte c�lov� um�st�n�
SelectDirDesc=Kam m� b�t [name] nainstalov�n?
SelectDirLabel3=[name] bude nainstalov�n do n�sleduj�c� slo�ky.
SelectDirBrowseLabel=Klepn�te na Dal�� pro pokra�ov�n�. Chcete-li zvolit jinou slo�ku, klepn�te na Proch�zet.
DiskSpaceMBLabel=Je vy�adov�no nejm�n� [mb] MB voln�ho m�sta na disku.
ToUNCPathname=Pr�vodce instalac� nem��e instalovat do cesty UNC. Pokud se pokou��te instalovat po s�ti, mus�te pou��t n�kterou z dostupn�ch s�ov�ch jednotek.
InvalidPath=Mus�te zadat �plnou cestu v�etn� p�smene jednotky; nap��klad:%n%nC:\Aplikace%n%nnebo cestu UNC ve tvaru:%n%n\\server\sd�len� slo�ka
InvalidDrive=V�mi zvolen� jednotka nebo cesta UNC neexistuje nebo nen� dostupn�. Pros�m, zvolte jin� um�st�n�.
DiskSpaceWarningTitle=Nedostatek m�sta na disku
DiskSpaceWarning=Pr�vodce instalac� vy�aduje nejm�n� %1 KB voln�ho m�sta pro instalaci produktu, ale na zvolen� jednotce je dostupn�ch pouze %2 KB.%n%nP�ejete si p�esto pokra�ovat?
DirNameTooLong=N�zev slo�ky nebo cesty je p��li� dlouh�.
InvalidDirName=Toto nen� platn� n�zev slo�ky.
BadDirName32=N�zvy slo�ek nemohou obsahovat ��dn� z n�sleduj�c�ch znak�:%n%n%1
DirExistsTitle=Slo�ka existuje
DirExists=Slo�ka:%n%n%1%n%nji� existuje. M� se p�esto instalovat do t�to slo�ky?
DirDoesntExistTitle=Slo�ka neexistuje
DirDoesntExist=Slo�ka:%n%n%1%n%nneexistuje. M� b�t tato slo�ka vytvo�ena?

; *** "Select Components" wizard page
WizardSelectComponents=Vyberte sou��sti
SelectComponentsDesc=Jak� sou��sti maj� b�t nainstalov�ny?
SelectComponentsLabel2=Za�krtn�te sou��sti, kter� maj� b�t nainstalov�ny; sou��sti, kter� se nemaj� instalovat, ponechte neza�krtnut�. Klepn�te na Dal�� pro pokra�ov�n�.
FullInstallation=�pln� instalace
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Kompaktn� instalace
CustomInstallation=Voliteln� instalace
NoUninstallWarningTitle=Sou��sti existuj�
NoUninstallWarning=Pr�vodce instalac� zjistil, �e n�sleduj�c� sou��sti jsou ji� na Va�em po��ta�i nainstalov�ny:%n%n%1%n%nNezahrnut� t�chto sou��st� do v�b�ru zp�sob�, �e nebudou pozd�ji  odinstalov�ny.%n%nP�ejete si p�esto pokra�ovat?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Vybran� sou��sti vy�aduj� nejm�n� [mb] MB m�sta na disku.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Zvolte dal�� �lohy
SelectTasksDesc=Kter� dal�� �lohy maj� b�t vykon�ny?
SelectTasksLabel2=Zvolte dal�� �lohy, kter� maj� b�t vykon�ny v pr�b�hu instalace produktu [name] a pokra�ujte klepnut�m na Dal��.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Vyberte slo�ku v nab�dce Start
SelectStartMenuFolderDesc=Kam maj� b�t pr�vodcem instalace um�st�ni z�stupci aplikace?
SelectStartMenuFolderLabel3=Z�stupci aplikace budou vytvo�eny v n�sleduj�c� slo�ce nab�dky Start.
SelectStartMenuFolderBrowseLabel=Klepn�te na Dal�� pro pokra�ov�n�. Chcete-li zvolit jinou slo�ku, klepn�te na Proch�zet.
NoIconsCheck=&Nevytv��et ��dn� ikony
MustEnterGroupName=Mus�te zadat n�zev slo�ky.
GroupNameTooLong=N�zev slo�ky nebo cesty je p��li� dlouh�.
InvalidGroupName=Toto nen� platn� n�zev slo�ky.
BadGroupName=N�zev slo�ky nem��e obsahovat ��dn� z n�sleduj�c�ch znak�:%n%n%1
NoProgramGroupCheck2=&Nevytv��et slo�ku v nab�dce Start

; *** "Ready to Install" wizard page
WizardReady=Instalace p�ipravena
ReadyLabel1=Pr�vodce instalac� je nyn� p�ipraven nainstalovat [name] na V� po��ta�.
ReadyLabel2a=Klepn�te na Instalovat pro pokra�ov�n� instala�n�ho procesu nebo klepn�te na Zp�t, pokud si p�ejete zm�nit n�kter� nastaven� instalace.
ReadyLabel2b=Klepn�te na Instalovat pro pokra�ov�n� instala�n�ho procesu.
ReadyMemoUserInfo=Informace o u�ivateli:
ReadyMemoDir=C�lov� um�st�n�:
ReadyMemoType=Typ instalace:
ReadyMemoComponents=Vybran� sou��sti:
ReadyMemoGroup=Slo�ka v nab�dce Start:
ReadyMemoTasks=Dal�� �lohy:

; *** "Preparing to Install" wizard page
WizardPreparing=P��prava instalace
PreparingDesc=Pr�vodce instalac� p�ipravuje instalaci produktu [name] na V� po��ta�.
PreviousInstallNotCompleted=Proces instalace/odinstalace p�ede�l�ho produktu nebyl zcela dokon�en. Pro dokon�en� tohoto procesu je nezbytn� restartovat tento po��ta�.%n%nPo proveden�m  restartu po��ta�e spus�te znovu tohoto pr�vodce instalac� pro dokon�en� instalace produktu [name].
CannotContinue=Pr�vodce instalac� nem��e pokra�ovat. Pros�m, klepn�te na Storno pro ukon�en� pr�vodce instalac�.

; *** "Installing" wizard page
WizardInstalling=Instaluji
InstallingLabel=�ekejte pros�m, dokud pr�vodce instalac� nedokon�� instalaci produktu [name] na V� po��ta�.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Dokon�uje se instalace produktu [name]
FinishedLabelNoIcons=Pr�vodce instalac� dokon�il instalaci produktu [name] na V� po��ta�.
FinishedLabel=Pr�vodce instalac� dokon�il instalaci produktu [name] na V� po��ta�. Produkt lze spustit pomoc� nainstalovan�ch ikon a z�stupc�.
ClickFinish=Klepn�te na Dokon�it pro ukon�en� pr�vodce instalac�.
FinishedRestartLabel=Pro dokon�en� instalace produktu [name] je nezbytn�, aby pr�vodce instalac� restartoval V� po��ta�. P�ejete si nyn� restartovat V� po��ta�?
FinishedRestartMessage=Pro dokon�en� instalace produktu [name] je nezbytn�, aby pr�vodce instalac� restartoval V� po��ta�.%n%nP�ejete si nyn� restartovat V� po��ta�?
ShowReadmeCheck=Ano, chci zobrazit dokument "�TIMNE"
YesRadio=&Ano, chci nyn� restartovat po��ta�
NoRadio=&Ne, po��ta� restartuji pozd�ji
; used for example as 'Run MyProg.exe'
RunEntryExec=Spustit %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Zobrazit %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Pr�vodce instalac� vy�aduje dal�� disk
SelectDiskLabel2=Pros�m, vlo�te disk %1 a klepn�te na OK.%n%nPokud se soubory na tomto disku nach�zej� v jin� slo�ce, ne� v t�, kter� je zobrazena n�e, pak zadejte spr�vnou cestu nebo klepn�te na Proch�zet.
PathLabel=&Cesta:
FileNotInDir2=Soubor "%1" nelze naj�t v "%2". Pros�m vlo�te spr�vn� disk nebo zvolte jinou slo�ku.
SelectDirectoryLabel=Pros�m, specifikujte um�st�n� dal��ho disku.

; *** Installation phase messages
SetupAborted=Instalace nebyla zcela dokon�ena.%n%nPros�m, opravte chybu a spus�te pr�vodce instalac� znovu.
EntryAbortRetryIgnore=Klepn�te na Opakovat pro zopakov�n� akce, na P�esko�it pro vynech�n� akce nebo na P�eru�it pro stornov�n� instalace.

; *** Installation status messages
StatusCreateDirs=Vytv��ej� se slo�ky...
StatusExtractFiles=Extrahuj� se soubory...
StatusCreateIcons=Vytv��ej� se z�stupci...
StatusCreateIniEntries=Vytv��ej� se z�znamy v konfigura�n�ch souborech...
StatusCreateRegistryEntries=Vytv��ej� se z�znamy v syst�mov�m registru...
StatusRegisterFiles=Registruj� se soubory...
StatusSavingUninstall=Ukl�daj� se informace nezbytn� pro pozd�j�� odinstalaci produktu...
StatusRunProgram=Dokon�uje se instalace...
StatusRollback=Prob�h� zp�tn� vr�cen� v�ech proveden�ch zm�n...

; *** Misc. errors
ErrorInternal2=Intern� chyba: %1
ErrorFunctionFailedNoCode=%1 selhala
ErrorFunctionFailed=%1 selhala; k�d %2
ErrorFunctionFailedWithMessage=%1 selhala; k�d %2.%n%3
ErrorExecutingProgram=Nelze spustit soubor:%n%1

; *** Registry errors
ErrorRegOpenKey=Do�lo k chyb� p�i otev�r�n� kl��e syst�mov�ho registru:%n%1\%2
ErrorRegCreateKey=Do�lo k chyb� p�i vytv��en� kl��e syst�mov�ho registru:%n%1\%2
ErrorRegWriteKey=Do�lo k chyb� p�i z�pisu do kl��e syst�mov�ho registru:%n%1\%2

; *** INI errors
ErrorIniEntry=Do�lo k chyb� p�i vytv��en� z�znamu v konfigura�n�m souboru "%1".

; *** File copying errors
FileAbortRetryIgnore=Klepn�te na Opakovat pro zopakov�n� akce, na P�esko�it pro p�esko�en� tohoto souboru (nedoporu�uje se) nebo na P�eru�it pro stornov�n� instalace.
FileAbortRetryIgnore2=Klepn�te na Opakovat pro zopakov�n� akce, na P�esko�it pro pokra�ov�n� (nedoporu�uje se) nebo na P�eru�it pro stornov�n� instalace.
SourceIsCorrupted=Zdrojov� soubor je po�kozen
SourceDoesntExist=Zdrojov� soubor "%1" neexistuje
ExistingFileReadOnly=Existuj�c� soubor je ur�en pouze pro �ten�.%n%nKlepn�te na Opakovat pro odstran�n� atributu "pouze pro �ten�" a zopakov�n� akce, na P�esko�it pro p�esko�en� tohoto souboru nebo na P�eru�it pro stornov�n� instalace.
ErrorReadingExistingDest=Do�lo k chyb� p�i pokusu o �ten� existuj�c�ho souboru:
FileExists=Soubor ji� existuje.%n%nM� b�t pr�vodcem instalace p�eps�n?
ExistingFileNewer=Existuj�c� soubor je nov�j�� ne� ten, kter� se pr�vodce instalac� pokou�� nainstalovat. Doporu�uje se ponechat existuj�c� soubor.%n%nP�ejete si ponechat existuj�c� soubor?
ErrorChangingAttr=Do�lo k chyb� p�i pokusu o modifikaci atribut� existuj�c�ho souboru:
ErrorCreatingTemp=Do�lo k chyb� p�i pokusu o vytvo�en� souboru v c�lov� slo�ce:
ErrorReadingSource=Do�lo k chyb� p�i pokusu o �ten� zdrojov�ho souboru:
ErrorCopying=Do�lo k chyb� p�i pokusu o zkop�rov�n� souboru:
ErrorReplacingExistingFile=Do�lo k chyb� p�i pokusu o nahrazen� existuj�c�ho souboru:
ErrorRestartReplace=Funkce pr�vodce instalac� "RestartReplace" selhala:
ErrorRenamingTemp=Do�lo k chyb� p�i pokusu o p�ejmenov�n� souboru v c�lov� slo�ce:
ErrorRegisterServer=Nelze prov�st registraci DLL/OCX: %1
ErrorRegisterServerMissingExport=Nelze nal�zt export DllRegisterServer
ErrorRegisterTypeLib=Nelze prov�st registraci typov� knihovny: %1

; *** Post-installation errors
ErrorOpeningReadme=Do�lo k chyb� p�i pokusu o otev�en� dokumentu "�TIMNE".
ErrorRestartingComputer=Pr�vodci instalace se nepoda�ilo restartovat V� po��ta�. Ud�lejte to, pros�m, manu�ln�.

; *** Uninstaller messages
UninstallNotFound=Soubor "%1" neexistuje. Produkt nelze odinstalovat.
UninstallOpenError=Soubor "%1" nelze otev��t. Produkt nelze odinstalovat.
UninstallUnsupportedVer=Pr�vodci odinstalac� se nepoda�ilo rozpoznat form�t souboru obsahuj�c�ho informace pro odinstalaci produktu "%1". Produkt nelze odinstalovat
UninstallUnknownEntry=V souboru obsahuj�c�m informace pro odinstalaci produktu byla zji�t�na nezn�m� polo�ka (%1)
ConfirmUninstall=Jste si opravdu jist(a), �e chcete odinstalovat %1 a v�echny jeho sou��sti?
OnlyAdminCanUninstall=K odinstalov�n� tohoto produktu mus�te b�t p�ihl�en(a) jako administr�tor.
UninstallStatusLabel=�ekejte, pros�m, dokud %1 nebude odinstalov�n z Va�eho po��ta�e.
UninstalledAll=%1 byl �sp�n� odinstalov�n z Va�eho po��ta�e.
UninstalledMost=%1 byl odinstalov�n z Va�eho po��ta�e.%n%nN�kter� jeho sou��sti se v�ak nepoda�ilo odinstalovat. Tyto mohou b�t odebr�ny manu�ln�.
UninstalledAndNeedsRestart=Pro dokon�en� odinstalace produktu %1 je nezbytn�, aby pr�vodce odinstalac� restartoval V� po��ta�.%n%nP�ejete si nyn� restartovat V� po��ta�?
UninstallDataCorrupted=Soubor "%1" je po�kozen. Produkt nelze odinstalovat

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Odebrat sd�len� soubor?
ConfirmDeleteSharedFile2=Syst�m indikuje, �e n�sleduj�c� sd�len� soubor nen� pou��v�n ��dn�mi jin�mi aplikacemi. M� b�t tento sd�len� soubor pr�vodcem odinstalac� odstran�n?%n%nPokud n�kter�  aplikace tento soubor pou��vaj�, pak po jeho odstran�n� nemus� tyto aplikace pracovat spr�vn�. Pokud si nejste jist(a), zvolte Ne. Ponech�n� tohoto souboru ve Va�em  syst�mu nezp�sob� ��dnou �kodu.
SharedFileNameLabel=N�zev souboru:
SharedFileLocationLabel=Um�st�n�:
WizardUninstalling=Stav odinstalace
StatusUninstalling=Odinstalov�v�m %1...
