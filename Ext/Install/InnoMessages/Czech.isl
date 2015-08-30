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
ConfirmTitle=Dotaz
ErrorTitle=Chyba

; *** SetupLdr messages
SetupLdrStartupMessage=Toto je prùvodce instalací produktu %1. Pøejete si pokraèovat?
LdrCannotCreateTemp=Nelze vytvoøit doèasnı soubor. Prùvodce instalací bude ukonèen
LdrCannotExecTemp=Nelze spustit soubor v doèasné sloce. Prùvodce instalací bude ukonèen

; *** Startup error messages
LastErrorMessage=%1.%n%nChyba %2: %3
SetupFileMissing=Instalaèní sloka neobsahuje soubor %1. Opravte, prosím, tuto chybu nebo si opatøete novou kopii tohoto produktu.
SetupFileCorrupt=Soubory prùvodce instalací jsou poškozeny. Opatøete si, prosím, novou kopii tohoto produktu.
SetupFileCorruptOrWrongVer=Soubory prùvodce instalací jsou poškozeny nebo se nesluèují s touto verzí prùvodce instalací. Opravte, prosím, tuto chybu nebo si opatøete novou kopii tohoto produktu.
NotOnThisPlatform=Tento produkt nelze spustit pod %1.
OnlyOnThisPlatform=Tento produkt musí bıt spuštìn pod %1.
WinVersionTooLowError=Tento produkt vyaduje %1 verze %2 nebo vyšší.
WinVersionTooHighError=Tento produkt nelze nainstalovat ve %1 verze %2 nebo vyšší.
AdminPrivilegesRequired=K provedení instalace tohoto produktu musíte bıt pøihlášen(a) jako administrátor.
PowerUserPrivilegesRequired=K provedení instalace tohoto produktu musíte bıt pøihlášen(a) jako administrátor nebo èlen skupiny Power Users.
SetupAppRunningError=Prùvodce instalací zjistil, e %1 je nyní spuštìn.%n%nUkonèete, prosím, všechny spuštìné instance tohoto produktu a klepnìte na OK pro pokraèování nebo na Storno pro ukonèení.
UninstallAppRunningError=Prùvodce odinstalací zjistil, e %1 je nyní spuštìn.%n%nUkonèete, prosím, všechny spuštìné instance tohoto produktu a klepnìte na OK pro pokraèování nebo na Storno pro  ukonèení.

; *** Misc. errors
ErrorCreatingDir=Prùvodce instalací nemohl vytvoøit sloku "%1"
ErrorTooManyFilesInDir=Nelze vytvoøit soubor ve sloce "%1", protoe tato sloka ji obsahuje pøíliš mnoho souborù

; *** Setup common messages
ExitSetupTitle=Ukonèit prùvodce instalací
ExitSetupMessage=Instalace nebyla zcela dokonèena. Jestlie nyní ukonèíte prùvodce instalací, produkt nebude nainstalován.%n%nPrùvodce instalací mùete znovu spustit pozdìji a dokonèit tak instalaci.%n%nUkonèit prùvodce instalací?
AboutSetupMenuItem=&O prùvodci instalací...
AboutSetupTitle=O prùvodci instalací
AboutSetupMessage=%1 verze %2%n%3%n%n%1 domovská stránka:%n%4
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
SelectLanguageTitle=Zvolit jazyk prùvodce instalací
SelectLanguageLabel=Zvolte jazyk, kterı se má pouít pøi instalaci:

; *** Common wizard text
ClickNext=Klepnìte na Další pro pokraèování nebo na Storno pro ukonèení prùvodce instalací.
BeveledLabel=
BrowseDialogTitle=Vyhledat sloku
BrowseDialogLabel=Z níe uvedeného seznamu vyberte sloku a klepnìte na OK.
NewFolderName=Nová sloka

; *** "Welcome" wizard page
WelcomeLabel1=Vítá Vás prùvodce instalací produktu [name].
WelcomeLabel2=[name/ver] bude nainstalován na Váš poèítaè.%n%nDoporuèuje se ukonèit veškeré spuštìné aplikace pøedtím, ne budete pokraèovat.

; *** "Password" wizard page
WizardPassword=Heslo
PasswordLabel1=Tato instalace je chránìna heslem.
PasswordLabel3=Prosím, zadejte heslo a klepnìte na Další pro pokraèování. Pøi zadávání hesla rozlišujte malá a velká písmena.
PasswordEditLabel=&Heslo:
IncorrectPassword=Zadané heslo není správné. Prosím, zkuste to znovu.

; *** "License Agreement" wizard page
WizardLicense=Licenèní ujednání
LicenseLabel=Prosím, pøeètìte si pozornì tyto dùleité informace pøedtím, ne budete pokraèovat.
LicenseLabel3=Prosím, pøeètìte si toto Licenèní ujednání. Musíte souhlasit s podmínkami tohoto ujednání, aby mohl instalaèní proces pokraèovat.
LicenseAccepted=&Souhlasím s podmínkami Licenèního ujednání
LicenseNotAccepted=&Nesouhlasím s podmínkami Licenèního ujednání

; *** "Information" wizard pages
WizardInfoBefore=Informace
InfoBeforeLabel=Prosím, pøeètìte si pozornì tyto dùleité informace pøedtím, ne budete pokraèovat.
InfoBeforeClickLabel=Klepnìte na Další pro pokraèování instalaèního procesu.
WizardInfoAfter=Informace
InfoAfterLabel=Prosím, pøeètìte si pozornì tyto dùleité informace pøedtím, ne budete pokraèovat.
InfoAfterClickLabel=Klepnìte na Další pro pokraèování instalaèního procesu.

; *** "User Information" wizard page
WizardUserInfo=Informace o uivateli
UserInfoDesc=Prosím, zadejte poadované informace.
UserInfoName=&Uivatelské jméno:
UserInfoOrg=&Spoleènost:
UserInfoSerial=Sé&riové èíslo:
UserInfoNameRequired=Uivatelské jméno musí bıt zadáno.

; *** "Select Destination Location" wizard page
WizardSelectDir=Zvolte cílové umístìní
SelectDirDesc=Kam má bıt [name] nainstalován?
SelectDirLabel3=[name] bude nainstalován do následující sloky.
SelectDirBrowseLabel=Klepnìte na Další pro pokraèování. Chcete-li zvolit jinou sloku, klepnìte na Procházet.
DiskSpaceMBLabel=Je vyadováno nejménì [mb] MB volného místa na disku.
ToUNCPathname=Prùvodce instalací nemùe instalovat do cesty UNC. Pokud se pokoušíte instalovat po síti, musíte pouít nìkterou z dostupnıch síovıch jednotek.
InvalidPath=Musíte zadat úplnou cestu vèetnì písmene jednotky; napøíklad:%n%nC:\Aplikace%n%nnebo cestu UNC ve tvaru:%n%n\\server\sdílená sloka
InvalidDrive=Vámi zvolená jednotka nebo cesta UNC neexistuje nebo není dostupná. Prosím, zvolte jiné umístìní.
DiskSpaceWarningTitle=Nedostatek místa na disku
DiskSpaceWarning=Prùvodce instalací vyaduje nejménì %1 KB volného místa pro instalaci produktu, ale na zvolené jednotce je dostupnıch pouze %2 KB.%n%nPøejete si pøesto pokraèovat?
DirNameTooLong=Název sloky nebo cesty je pøíliš dlouhı.
InvalidDirName=Toto není platnı název sloky.
BadDirName32=Názvy sloek nemohou obsahovat ádnı z následujících znakù:%n%n%1
DirExistsTitle=Sloka existuje
DirExists=Sloka:%n%n%1%n%nji existuje. Má se pøesto instalovat do této sloky?
DirDoesntExistTitle=Sloka neexistuje
DirDoesntExist=Sloka:%n%n%1%n%nneexistuje. Má bıt tato sloka vytvoøena?

; *** "Select Components" wizard page
WizardSelectComponents=Vyberte souèásti
SelectComponentsDesc=Jaké souèásti mají bıt nainstalovány?
SelectComponentsLabel2=Zaškrtnìte souèásti, které mají bıt nainstalovány; souèásti, které se nemají instalovat, ponechte nezaškrtnuté. Klepnìte na Další pro pokraèování.
FullInstallation=Úplná instalace
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Kompaktní instalace
CustomInstallation=Volitelná instalace
NoUninstallWarningTitle=Souèásti existují
NoUninstallWarning=Prùvodce instalací zjistil, e následující souèásti jsou ji na Vašem poèítaèi nainstalovány:%n%n%1%n%nNezahrnutí tìchto souèástí do vıbìru zpùsobí, e nebudou pozdìji  odinstalovány.%n%nPøejete si pøesto pokraèovat?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Vybrané souèásti vyadují nejménì [mb] MB místa na disku.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Zvolte další úlohy
SelectTasksDesc=Které další úlohy mají bıt vykonány?
SelectTasksLabel2=Zvolte další úlohy, které mají bıt vykonány v prùbìhu instalace produktu [name] a pokraèujte klepnutím na Další.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Vyberte sloku v nabídce Start
SelectStartMenuFolderDesc=Kam mají bıt prùvodcem instalace umístìni zástupci aplikace?
SelectStartMenuFolderLabel3=Zástupci aplikace budou vytvoøeny v následující sloce nabídky Start.
SelectStartMenuFolderBrowseLabel=Klepnìte na Další pro pokraèování. Chcete-li zvolit jinou sloku, klepnìte na Procházet.
NoIconsCheck=&Nevytváøet ádné ikony
MustEnterGroupName=Musíte zadat název sloky.
GroupNameTooLong=Název sloky nebo cesty je pøíliš dlouhı.
InvalidGroupName=Toto není platnı název sloky.
BadGroupName=Název sloky nemùe obsahovat ádnı z následujících znakù:%n%n%1
NoProgramGroupCheck2=&Nevytváøet sloku v nabídce Start

; *** "Ready to Install" wizard page
WizardReady=Instalace pøipravena
ReadyLabel1=Prùvodce instalací je nyní pøipraven nainstalovat [name] na Váš poèítaè.
ReadyLabel2a=Klepnìte na Instalovat pro pokraèování instalaèního procesu nebo klepnìte na Zpìt, pokud si pøejete zmìnit nìkterá nastavení instalace.
ReadyLabel2b=Klepnìte na Instalovat pro pokraèování instalaèního procesu.
ReadyMemoUserInfo=Informace o uivateli:
ReadyMemoDir=Cílové umístìní:
ReadyMemoType=Typ instalace:
ReadyMemoComponents=Vybrané souèásti:
ReadyMemoGroup=Sloka v nabídce Start:
ReadyMemoTasks=Další úlohy:

; *** "Preparing to Install" wizard page
WizardPreparing=Pøíprava instalace
PreparingDesc=Prùvodce instalací pøipravuje instalaci produktu [name] na Váš poèítaè.
PreviousInstallNotCompleted=Proces instalace/odinstalace pøedešlého produktu nebyl zcela dokonèen. Pro dokonèení tohoto procesu je nezbytné restartovat tento poèítaè.%n%nPo provedeném  restartu poèítaèe spuste znovu tohoto prùvodce instalací pro dokonèení instalace produktu [name].
CannotContinue=Prùvodce instalací nemùe pokraèovat. Prosím, klepnìte na Storno pro ukonèení prùvodce instalací.

; *** "Installing" wizard page
WizardInstalling=Instaluji
InstallingLabel=Èekejte prosím, dokud prùvodce instalací nedokonèí instalaci produktu [name] na Váš poèítaè.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Dokonèuje se instalace produktu [name]
FinishedLabelNoIcons=Prùvodce instalací dokonèil instalaci produktu [name] na Váš poèítaè.
FinishedLabel=Prùvodce instalací dokonèil instalaci produktu [name] na Váš poèítaè. Produkt lze spustit pomocí nainstalovanıch ikon a zástupcù.
ClickFinish=Klepnìte na Dokonèit pro ukonèení prùvodce instalací.
FinishedRestartLabel=Pro dokonèení instalace produktu [name] je nezbytné, aby prùvodce instalací restartoval Váš poèítaè. Pøejete si nyní restartovat Váš poèítaè?
FinishedRestartMessage=Pro dokonèení instalace produktu [name] je nezbytné, aby prùvodce instalací restartoval Váš poèítaè.%n%nPøejete si nyní restartovat Váš poèítaè?
ShowReadmeCheck=Ano, chci zobrazit dokument "ÈTIMNE"
YesRadio=&Ano, chci nyní restartovat poèítaè
NoRadio=&Ne, poèítaè restartuji pozdìji
; used for example as 'Run MyProg.exe'
RunEntryExec=Spustit %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Zobrazit %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Prùvodce instalací vyaduje další disk
SelectDiskLabel2=Prosím, vlote disk %1 a klepnìte na OK.%n%nPokud se soubory na tomto disku nacházejí v jiné sloce, ne v té, která je zobrazena níe, pak zadejte správnou cestu nebo klepnìte na Procházet.
PathLabel=&Cesta:
FileNotInDir2=Soubor "%1" nelze najít v "%2". Prosím vlote správnı disk nebo zvolte jinou sloku.
SelectDirectoryLabel=Prosím, specifikujte umístìní dalšího disku.

; *** Installation phase messages
SetupAborted=Instalace nebyla zcela dokonèena.%n%nProsím, opravte chybu a spuste prùvodce instalací znovu.
EntryAbortRetryIgnore=Klepnìte na Opakovat pro zopakování akce, na Pøeskoèit pro vynechání akce nebo na Pøerušit pro stornování instalace.

; *** Installation status messages
StatusCreateDirs=Vytváøejí se sloky...
StatusExtractFiles=Extrahují se soubory...
StatusCreateIcons=Vytváøejí se zástupci...
StatusCreateIniEntries=Vytváøejí se záznamy v konfiguraèních souborech...
StatusCreateRegistryEntries=Vytváøejí se záznamy v systémovém registru...
StatusRegisterFiles=Registrují se soubory...
StatusSavingUninstall=Ukládají se informace nezbytné pro pozdìjší odinstalaci produktu...
StatusRunProgram=Dokonèuje se instalace...
StatusRollback=Probíhá zpìtné vrácení všech provedenıch zmìn...

; *** Misc. errors
ErrorInternal2=Interní chyba: %1
ErrorFunctionFailedNoCode=%1 selhala
ErrorFunctionFailed=%1 selhala; kód %2
ErrorFunctionFailedWithMessage=%1 selhala; kód %2.%n%3
ErrorExecutingProgram=Nelze spustit soubor:%n%1

; *** Registry errors
ErrorRegOpenKey=Došlo k chybì pøi otevírání klíèe systémového registru:%n%1\%2
ErrorRegCreateKey=Došlo k chybì pøi vytváøení klíèe systémového registru:%n%1\%2
ErrorRegWriteKey=Došlo k chybì pøi zápisu do klíèe systémového registru:%n%1\%2

; *** INI errors
ErrorIniEntry=Došlo k chybì pøi vytváøení záznamu v konfiguraèním souboru "%1".

; *** File copying errors
FileAbortRetryIgnore=Klepnìte na Opakovat pro zopakování akce, na Pøeskoèit pro pøeskoèení tohoto souboru (nedoporuèuje se) nebo na Pøerušit pro stornování instalace.
FileAbortRetryIgnore2=Klepnìte na Opakovat pro zopakování akce, na Pøeskoèit pro pokraèování (nedoporuèuje se) nebo na Pøerušit pro stornování instalace.
SourceIsCorrupted=Zdrojovı soubor je poškozen
SourceDoesntExist=Zdrojovı soubor "%1" neexistuje
ExistingFileReadOnly=Existující soubor je urèen pouze pro ètení.%n%nKlepnìte na Opakovat pro odstranìní atributu "pouze pro ètení" a zopakování akce, na Pøeskoèit pro pøeskoèení tohoto souboru nebo na Pøerušit pro stornování instalace.
ErrorReadingExistingDest=Došlo k chybì pøi pokusu o ètení existujícího souboru:
FileExists=Soubor ji existuje.%n%nMá bıt prùvodcem instalace pøepsán?
ExistingFileNewer=Existující soubor je novìjší ne ten, kterı se prùvodce instalací pokouší nainstalovat. Doporuèuje se ponechat existující soubor.%n%nPøejete si ponechat existující soubor?
ErrorChangingAttr=Došlo k chybì pøi pokusu o modifikaci atributù existujícího souboru:
ErrorCreatingTemp=Došlo k chybì pøi pokusu o vytvoøení souboru v cílové sloce:
ErrorReadingSource=Došlo k chybì pøi pokusu o ètení zdrojového souboru:
ErrorCopying=Došlo k chybì pøi pokusu o zkopírování souboru:
ErrorReplacingExistingFile=Došlo k chybì pøi pokusu o nahrazení existujícího souboru:
ErrorRestartReplace=Funkce prùvodce instalací "RestartReplace" selhala:
ErrorRenamingTemp=Došlo k chybì pøi pokusu o pøejmenování souboru v cílové sloce:
ErrorRegisterServer=Nelze provést registraci DLL/OCX: %1
ErrorRegisterServerMissingExport=Nelze nalézt export DllRegisterServer
ErrorRegisterTypeLib=Nelze provést registraci typové knihovny: %1

; *** Post-installation errors
ErrorOpeningReadme=Došlo k chybì pøi pokusu o otevøení dokumentu "ÈTIMNE".
ErrorRestartingComputer=Prùvodci instalace se nepodaøilo restartovat Váš poèítaè. Udìlejte to, prosím, manuálnì.

; *** Uninstaller messages
UninstallNotFound=Soubor "%1" neexistuje. Produkt nelze odinstalovat.
UninstallOpenError=Soubor "%1" nelze otevøít. Produkt nelze odinstalovat.
UninstallUnsupportedVer=Prùvodci odinstalací se nepodaøilo rozpoznat formát souboru obsahujícího informace pro odinstalaci produktu "%1". Produkt nelze odinstalovat
UninstallUnknownEntry=V souboru obsahujícím informace pro odinstalaci produktu byla zjištìna neznámá poloka (%1)
ConfirmUninstall=Jste si opravdu jist(a), e chcete odinstalovat %1 a všechny jeho souèásti?
OnlyAdminCanUninstall=K odinstalování tohoto produktu musíte bıt pøihlášen(a) jako administrátor.
UninstallStatusLabel=Èekejte, prosím, dokud %1 nebude odinstalován z Vašeho poèítaèe.
UninstalledAll=%1 byl úspìšnì odinstalován z Vašeho poèítaèe.
UninstalledMost=%1 byl odinstalován z Vašeho poèítaèe.%n%nNìkteré jeho souèásti se však nepodaøilo odinstalovat. Tyto mohou bıt odebrány manuálnì.
UninstalledAndNeedsRestart=Pro dokonèení odinstalace produktu %1 je nezbytné, aby prùvodce odinstalací restartoval Váš poèítaè.%n%nPøejete si nyní restartovat Váš poèítaè?
UninstallDataCorrupted=Soubor "%1" je poškozen. Produkt nelze odinstalovat

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Odebrat sdílenı soubor?
ConfirmDeleteSharedFile2=Systém indikuje, e následující sdílenı soubor není pouíván ádnımi jinımi aplikacemi. Má bıt tento sdílenı soubor prùvodcem odinstalací odstranìn?%n%nPokud nìkteré  aplikace tento soubor pouívají, pak po jeho odstranìní nemusí tyto aplikace pracovat správnì. Pokud si nejste jist(a), zvolte Ne. Ponechání tohoto souboru ve Vašem  systému nezpùsobí ádnou škodu.
SharedFileNameLabel=Název souboru:
SharedFileLocationLabel=Umístìní:
WizardUninstalling=Stav odinstalace
StatusUninstalling=Odinstalovávám %1...
