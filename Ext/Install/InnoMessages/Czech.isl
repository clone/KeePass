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
LanguageName=Èeština
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
SetupAppTitle=Prùvodce instalací
SetupWindowTitle=Prùvodce instalací - %1
UninstallAppTitle=Prùvodce odinstalací
UninstallAppFullTitle=Prùvodce odinstalací - %1

; *** Misc. common
InformationTitle=Informace
ConfirmTitle=Potvrzení
ErrorTitle=Chyba

; *** SetupLdr messages
SetupLdrStartupMessage=Tento prùvodce nainstaluje produkt %1. Pøejete si pokraèovat?
LdrCannotCreateTemp=Nelze vytvoøit doèasnı soubor. Prùvodce instalací bude ukonèen
LdrCannotExecTemp=Nelze spustit soubor v doèasné sloce. Prùvodce instalací bude ukonèen

; *** Startup error messages
LastErrorMessage=%1.%n%nChyba %2: %3
SetupFileMissing=Instalaèní sloka neobsahuje soubor %1. Opravte, prosím, tuto chybu nebo si poøiïte novou kopii tohoto produktu.
SetupFileCorrupt=Soubory prùvodce instalací jsou poškozeny. Poøiïte si, prosím, novou kopii tohoto produktu.
SetupFileCorruptOrWrongVer=Soubory prùvodce instalací jsou poškozeny nebo se nesluèují s touto verzí prùvodce. Opravte, prosím, tuto chybu nebo si poøiïte novou kopii tohoto produktu.
NotOnThisPlatform=Tento produkt nelze spustit ve %1.
OnlyOnThisPlatform=Tento produkt musí bıt spuštìn ve %1.
WinVersionTooLowError=Tento produkt vyaduje %1 verzi %2 nebo vyšší.
WinVersionTooHighError=Tento produkt nelze nainstalovat ve %1 verzi %2 nebo vyšší.
AdminPrivilegesRequired=Pøi instalaci tohoto produktu musíte bıt pøihlášeni s právy administrátora.
PowerUserPrivilegesRequired=Pøi instalaci tohoto produktu musíte bıt pøihlášeni s právy administrátora nebo èlena skupiny Power Users.
SetupAppRunningError=Prùvodce instalací zjistil, e aplikace %1 je ji spuštìna.%n%nUkonèete, prosím, všechny spuštìné instance produktu a pokraèujte tlaèítkem OK nebo instalaci ukonèete tlaèítkem Storno.
UninstallAppRunningError=Prùvodce odinstalací zjistil, e aplikace %1 je ji spuštìna.%n%nUkonèete, prosím, všechny spuštìné instance produktu a pokraèujte tlaèítkem OK nebo instalaci ukonèete tlaèítkem Storno.

; *** Misc. errors
ErrorCreatingDir=Prùvodce instalací nemohl vytvoøit sloku "%1"
ErrorTooManyFilesInDir=Nelze vytvoøit soubor ve sloce "%1", protoe tato sloka ji obsahuje pøíliš mnoho souborù

; *** Setup common messages
ExitSetupTitle=Ukonèit prùvodce instalací
ExitSetupMessage=Instalace nebyla zcela dokonèena. Jestlie nyní ukonèíte prùvodce instalací, produkt nebude nainstalován.%n%nPrùvodce instalací mùete znovu spustit jindy a dokonèit instalaci.%n%nChcete ukonèit prùvodce instalací?
AboutSetupMenuItem=&O prùvodci instalací...
AboutSetupTitle=O prùvodci instalací
AboutSetupMessage=%1 verze %2%n%3%n%n%1 domácí stránka:%n%4
AboutSetupNote=

; *** Buttons
ButtonBack=< &Zpìt
ButtonNext=&Další >
ButtonInstall=&Instalovat
ButtonOK=OK
ButtonCancel=Storno
ButtonYes=&Ano
ButtonYesToAll=Ano &všem
ButtonNo=&Ne
ButtonNoToAll=N&e všem
ButtonFinish=&Dokonèit
ButtonBrowse=&Procházet...
ButtonWizardBrowse=&Procházet...
ButtonNewFolder=&Vytvoøit novou sloku

; *** "Select Language" dialog messages
SelectLanguageTitle=Vıbìr jazyka prùvodce instalací
SelectLanguageLabel=Zvolte jazyk, kterı se má pouít pøi instalaci:

; *** Common wizard text
ClickNext=Pokraèujte tlaèítkem Další nebo tlaèítkem Storno ukonèete prùvodce instalací.
BeveledLabel=
BrowseDialogTitle=Vyhledat sloku
BrowseDialogLabel=Z níe uvedeného seznamu vyberte sloku a kliknìte na OK.
NewFolderName=Nová sloka

; *** "Welcome" wizard page
WelcomeLabel1=Vítá Vás prùvodce instalací produktu [name].
WelcomeLabel2=Produkt [name/ver] bude nainstalován na Váš poèítaè.%n%nDøíve ne budete pokraèovat, je doporuèeno ukonèit veškeré spuštìné aplikace.

; *** "Password" wizard page
WizardPassword=Heslo
PasswordLabel1=Tato instalace je chránìna heslem.
PasswordLabel3=Prosím, zadejte heslo a pokraèujte tlaèítkem Další. Pøi zadávání hesla rozlišujte malá a velká písmena.
PasswordEditLabel=&Heslo:
IncorrectPassword=Zadané heslo není správné. Prosím, zkuste to znovu.

; *** "License Agreement" wizard page
WizardLicense=Licenèní smlouva
LicenseLabel=Prosím, pøeètìte si pozornì následující dùleité informace døíve, ne budete pokraèovat.
LicenseLabel3=Prosím, pøeètìte si tuto licenèní smlouvu. Prùvodce instalací bude pokraèovat jen po vašem souhlasu s podmínkami této smlouvy.
LicenseAccepted=&Souhlasím s podmínkami licenèní smlouvy
LicenseNotAccepted=&Nesouhlasím s podmínkami licenèní smlouvy

; *** "Information" wizard pages
WizardInfoBefore=Informace
InfoBeforeLabel=Prosím, pøeètìte si pozornì následující dùleité informace døíve, ne budete pokraèovat.
InfoBeforeClickLabel=Tlaèítkem Další pokraèujte v instalaèním procesu.
WizardInfoAfter=Informace
InfoAfterLabel=Prosím, pøeètìte si pozornì následující dùleité informace døíve, ne budete pokraèovat.
InfoAfterClickLabel=Tlaèítkem Další pokraèujte v instalaèním procesu.

; *** "User Information" wizard page
WizardUserInfo=Informace o uivateli
UserInfoDesc=Prosím, zadejte vaše údaje.
UserInfoName=&Uivatelské jméno:
UserInfoOrg=&Spoleènost:
UserInfoSerial=Sé&riové èíslo:
UserInfoNameRequired=Uivatelské jméno musí bıt zadáno.

; *** "Select Destination Location" wizard page
WizardSelectDir=Zvolte cílové umístìní
SelectDirDesc=Kam má bıt produkt [name] nainstalován?
SelectDirLabel3=Prùvodce nainstaluje produkt [name] do následující sloky.
SelectDirBrowseLabel=Pokraèujte tlaèítkem Další. Chcete-li zvolit jinou sloku, vyberte ji stiskem tlaèítka Procházet.
DiskSpaceMBLabel=Instalace vyaduje nejménì [mb] MB volného místa na disku.
ToUNCPathname=Prùvodce instalací nemùe instalovat do cesty UNC. Pokud se pokoušíte instalovat na sí, budete potøebovat pøipojit síovou jednotku.
InvalidPath=Musíte zadat úplnou cestu vèetnì písmene jednotky; napøíklad:%n%nC:\Aplikace%n%nnebo cestu UNC ve tvaru:%n%n\\server\sdílená sloka
InvalidDrive=Vámi zvolená jednotka nebo cesta UNC neexistuje nebo není dostupná. Prosím, zvolte jiné umístìní.
DiskSpaceWarningTitle=Nedostatek místa na disku
DiskSpaceWarning=Prùvodce instalací vyaduje nejménì %1 KB volného místa pro instalaci produktu, ale na zvolené jednotce je dostupnıch pouze %2 KB.%n%nChcete pøesto pokraèovat?
DirNameTooLong=Název sloky nebo cesta jsou pøíliš dlouhé.
InvalidDirName=Název sloky není platnı.
BadDirName32=Názvy sloek nemohou obsahovat ádnı z následujících znakù:%n%n%1
DirExistsTitle=Sloka existuje
DirExists=Sloka:%n%n%1%n%nji existuje. Má se pøesto instalovat do této sloky?
DirDoesntExistTitle=Sloka neexistuje
DirDoesntExist=Sloka:%n%n%1%n%nneexistuje. Má bıt tato sloka vytvoøena?

; *** "Select Components" wizard page
WizardSelectComponents=Zvolte souèásti
SelectComponentsDesc=Jaké souèásti mají bıt nainstalovány?
SelectComponentsLabel2=Zaškrtnìte souèásti, které mají bıt nainstalovány; souèásti, které se nemají instalovat, ponechte nezaškrtnuté. Pokraèujte tlaèítkem Další.
FullInstallation=Úplná instalace
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Kompaktní instalace
CustomInstallation=Volitelná instalace
NoUninstallWarningTitle=Souèásti existují
NoUninstallWarning=Prùvodce instalací zjistil, e následující souèásti jsou ji na Vašem poèítaèi nainstalovány:%n%n%1%n%nJejich nezahrnutí do vıbìru je neodinstaluje.%n%nChcete pøesto pokraèovat?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Vybrané souèásti vyadují nejménì [mb] MB místa na disku.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Zvolte další úlohy
SelectTasksDesc=Které další úlohy mají bıt provedeny?
SelectTasksLabel2=Zvolte další úlohy, které mají bıt provedeny v prùbìhu instalace produktu [name] a pokraèujte tlaèítkem Další.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Vyberte sloku v nabídce Start
SelectStartMenuFolderDesc=Kam má prùvodce umístit zástupce aplikace?
SelectStartMenuFolderLabel3=Prùvodce vytvoøí zástupce aplikace v následující sloce nabídky Start.
SelectStartMenuFolderBrowseLabel=Pokraèujte tlaèítkem Další. Chcete-li zvolit jinou sloku, vyberte ji stiskem tlaèítka Procházet.
NoIconsCheck=&Nevytváøet ádné zástupce
MustEnterGroupName=Musíte zadat název sloky.
GroupNameTooLong=Název sloky nebo cesta jsou pøíliš dlouhé.
InvalidGroupName=Název sloky není platnı.
BadGroupName=Název sloky nemùe obsahovat ádnı z následujících znakù:%n%n%1
NoProgramGroupCheck2=&Nevytváøet sloku v nabídce Start

; *** "Ready to Install" wizard page
WizardReady=Instalace je pøipravena
ReadyLabel1=Prùvodce instalací je nyní pøipraven nainstalovat aplikaci [name] na Váš poèítaè.
ReadyLabel2a=Pokraèujte v instalaèním procesu tlaèítkem Instalovat nebo pouijte tlaèítko Zpìt, pøejete-li si zmìnit nìkterá nastavení instalace.
ReadyLabel2b=Pokraèujte v instalaèním procesu tlaèítkem Instalovat.
ReadyMemoUserInfo=Informace o uivateli:
ReadyMemoDir=Cílové umístìní:
ReadyMemoType=Typ instalace:
ReadyMemoComponents=Vybrané souèásti:
ReadyMemoGroup=Sloka v nabídce Start:
ReadyMemoTasks=Další úlohy:

; *** "Preparing to Install" wizard page
WizardPreparing=Pøíprava k instalaci
PreparingDesc=Prùvodce instalací pøipravuje instalaci produktu [name] na Váš poèítaè.
PreviousInstallNotCompleted=Proces instalace/odinstalace pøedchozího produktu nebyl zcela dokonèen. Dokonèení pøedchozího procesu instalace vyaduje restart tohoto poèítaèe.%n%nPo provedení restartu poèítaèe spuste znovu tohoto prùvodce instalací, aby bylo moné dokonèit instalaci produktu [name].
CannotContinue=Prùvodce instalací nemùe pokraèovat. Prosím, tlaèítkem Storno ukonèete prùvodce instalací.

; *** "Installing" wizard page
WizardInstalling=Instalování
InstallingLabel=Èekejte prosím, dokud prùvodce instalací nedokonèí instalaci produktu [name] na Váš poèítaè.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Dokonèuje se instalace produktu [name]
FinishedLabelNoIcons=Prùvodce instalací dokonèil instalaci produktu [name] na Váš poèítaè.
FinishedLabel=Prùvodce instalací dokonèil instalaci produktu [name] na Váš poèítaè. Produkt lze spustit pomocí nainstalovanıch zástupcù.
ClickFinish=Tlaèítkem Dokonèit ukonèíte prùvodce instalací.
FinishedRestartLabel=Pro dokonèení instalace produktu [name] je nezbytné, aby prùvodce instalací restartoval Váš poèítaè. Chcete jej restartovat nyní?
FinishedRestartMessage=Pro dokonèení instalace produktu [name] je nezbytné, aby prùvodce instalací restartoval Váš poèítaè.%n%nChcete jej restartovat nyní?
ShowReadmeCheck=Ano, chci zobrazit dokument "ÈTIMNE"
YesRadio=&Ano, chci nyní restartovat poèítaè
NoRadio=&Ne, poèítaè restartuji pozdìji
; used for example as 'Run MyProg.exe'
RunEntryExec=Spustit %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Zobrazit %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Prùvodce instalací vyaduje další disk
SelectDiskLabel2=Vlote, prosím, disk %1 a kliknìte na OK.%n%nPokud se soubory z tohoto disku nacházejí v jiné sloce, ne v té, která je zobrazena níe, pak zadejte správnou cestu nebo ji vyhledejte tlaèítkem Procházet.
PathLabel=&Cesta:
FileNotInDir2=Soubor "%1" nelze najít v "%2". Vlote, prosím, správnı disk nebo zvolte jinou sloku.
SelectDirectoryLabel=Prosím, specifikujte umístìní dalšího disku.

; *** Installation phase messages
SetupAborted=Instalace nebyla zcela dokonèena.%n%nProsím, opravte chybu a spuste prùvodce instalací znovu.
EntryAbortRetryIgnore=Tlaèítkem Opakovat akci zopakujete, tlaèítkem Pøeskoèit akci vynecháte nebo tlaèítkem Pøerušit instalaci pøerušíte.

; *** Installation status messages
StatusCreateDirs=Vytváøejí se sloky...
StatusExtractFiles=Extrahují se soubory...
StatusCreateIcons=Vytváøejí se zástupci...
StatusCreateIniEntries=Vytváøejí se INI poloky...
StatusCreateRegistryEntries=Vytváøejí se záznamy v registru...
StatusRegisterFiles=Registrují se soubory...
StatusSavingUninstall=Ukládají se informace pro odinstalaci...
StatusRunProgram=Dokonèuje se instalace...
StatusRollback=Vracejí se zpìt provedené zmìny...

; *** Misc. errors
ErrorInternal2=Interní chyba: %1
ErrorFunctionFailedNoCode=%1 selhala
ErrorFunctionFailed=%1 selhala; kód %2
ErrorFunctionFailedWithMessage=%1 selhala; kód %2.%n%3
ErrorExecutingProgram=Nelze spustit soubor:%n%1

; *** Registry errors
ErrorRegOpenKey=Chyba pøi otevírání klíèe registru:%n%1\%2
ErrorRegCreateKey=Chyba pøi vytváøení klíèe registru:%n%1\%2
ErrorRegWriteKey=Chyba pøi zápisu do klíèe registru:%n%1\%2

; *** INI errors
ErrorIniEntry=Chyba pøi vytváøení INI záznamu v souboru "%1".

; *** File copying errors
FileAbortRetryIgnore=Tlaèítkem Opakovat akci zopakujete, tlaèítkem Pøeskoèit akci tohoto souboru vynecháte (nedoporuèuje se) nebo tlaèítkem Pøerušit instalaci pøerušíte.
FileAbortRetryIgnore2=Tlaèítkem Opakovat akci zopakujete, tlaèítkem Pøeskoèit pøesto pokraèujete (nedoporuèuje se) nebo tlaèítkem Pøerušit instalaci pøerušíte.
SourceIsCorrupted=Zdrojovı soubor je poškozen
SourceDoesntExist=Zdrojovı soubor "%1" neexistuje
ExistingFileReadOnly=Existující soubor je urèen pouze pro ètení.%n%nTlaèítkem Opakovat odstraníte atribut "pouze pro ètení" a akci zopakujete, tlaèítkem Pøeskoèit tento soubor pøeskoèíte nebo tlaèítkem Pøerušit instalaci pøerušíte.
ErrorReadingExistingDest=Došlo k chybì pøi pokusu o ètení existujícího souboru:
FileExists=Soubor ji existuje.%n%nMá bıt prùvodcem instalace pøepsán?
ExistingFileNewer=Existující soubor je novìjší ne ten, kterı se prùvodce instalací pokouší nainstalovat. Je doporuèeno ponechat existující soubor.%n%nChcete ponechat existující soubor?
ErrorChangingAttr=Došlo k chybì pøi pokusu o zmìnu atributù existujícího souboru:
ErrorCreatingTemp=Došlo k chybì pøi pokusu o vytvoøení souboru v cílové sloce:
ErrorReadingSource=Došlo k chybì pøi pokusu o ètení zdrojového souboru:
ErrorCopying=Došlo k chybì pøi pokusu o zkopírování souboru:
ErrorReplacingExistingFile=Došlo k chybì pøi pokusu o nahrazení existujícího souboru:
ErrorRestartReplace=Funkce "RestartReplace" instalátoru selhala:
ErrorRenamingTemp=Došlo k chybì pøi pokusu o pøejmenování souboru v cílové sloce:
ErrorRegisterServer=Nelze zaregistrovat DLL/OCX: %1
ErrorRegisterServerMissingExport=Nelze nalézt export DllRegisterServer
ErrorRegisterTypeLib=Nelze zaregistrovat typovou knihovnu: %1

; *** Post-installation errors
ErrorOpeningReadme=Došlo k chybì pøi pokusu o otevøení dokumentu "ÈTIMNE".
ErrorRestartingComputer=Prùvodci instalace se nepodaøilo restartovat Váš poèítaè. Proveïte, prosím, restart ruènì.

; *** Uninstaller messages
UninstallNotFound=Soubor "%1" neexistuje. Produkt nelze odinstalovat.
UninstallOpenError=Soubor "%1" nelze otevøít. Produkt nelze odinstalovat.
UninstallUnsupportedVer=Prùvodci odinstalací se nepodaøilo rozpoznat formát souboru obsahujícího informace k odinstalaci produktu "%1". Produkt nelze odinstalovat
UninstallUnknownEntry=V souboru obsahujícím informace pro odinstalaci produktu byla zjištìna neznámá poloka (%1)
ConfirmUninstall=Urèitì chcete úplnì odinstalovat produkt %1 a všechny jeho souèásti?
OnlyAdminCanUninstall=Odinstalace tohoto produktu vyaduje práva administrátora.
UninstallStatusLabel=Èekejte, prosím, dokud produkt %1 nebude odinstalován z Vašeho poèítaèe.
UninstalledAll=Produkt %1 byl úspìšnì odinstalován z Vašeho poèítaèe.
UninstalledMost=Produkt %1 byl odinstalován z Vašeho poèítaèe.%n%nNìkteré jeho souèásti se však nepodaøilo odinstalovat. Ty lze odstranit ruènì.
UninstalledAndNeedsRestart=K dokonèení odinstalace produktu %1 je nezbytné, aby prùvodce odinstalací restartoval Váš poèítaè.%n%nChcete jej restartovat nyní?
UninstallDataCorrupted=Soubor "%1" je poškozen. Produkt nelze odinstalovat

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Odebrat sdílenı soubor?
ConfirmDeleteSharedFile2=Systém indikuje, e následující sdílenı soubor není pouíván ádnımi jinımi aplikacemi. Má bıt tento sdílenı soubor prùvodcem odinstalací odstranìn?%n%nPokud nìkteré aplikace tento soubor pouívají, pak po jeho odstranìní nemusí pracovat správnì. Nejste-li si jisti, zvolte Ne. Ponechání tohoto souboru ve Vašem systému nezpùsobí ádnou škodu.
SharedFileNameLabel=Název souboru:
SharedFileLocationLabel=Umístìní:
WizardUninstalling=Stav odinstalace
StatusUninstalling=Probíhá odinstalace %1...
