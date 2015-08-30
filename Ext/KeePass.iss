; KeePass Password Safe Installation Script
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!
; Thanks to Lubos Stanek for creating a template for this installer.
; Thanks to Hilbrand Edskes for installer improvements.

#define MyAppName "KeePass"
#define MyAppFullName "KeePass Password Safe"
#define MyAppPublisher "Dominik Reichl"
#define MyAppURL "http://keepass.info/"
#define MyAppExeName "KeePass.exe"
#define MyAppUrlName "KeePass.url"
#define MyAppHelpName "KeePass.chm"

#define KeeVersionStr "1.19"
#define KeeVersionWin "1.1.9.0"

#define KeeDevPeriod "2003-2011"

[Setup]
AppName={#MyAppFullName}
AppVerName={#MyAppFullName} {#KeeVersionStr}
AppVersion={#KeeVersionStr}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
AppCopyright=Copyright (c) {#KeeDevPeriod} {#MyAppPublisher}
MinVersion=4.1,4.0
DefaultDirName={pf}\{#MyAppFullName}
DefaultGroupName={#MyAppFullName}
AllowNoIcons=yes
LicenseFile=..\Docs\License_Install.txt
OutputDir=..\Build\WinGUI_Distrib
OutputBaseFilename={#MyAppName}-{#KeeVersionStr}-Setup
Compression=lzma/ultra
SolidCompression=yes
InternalCompressLevel=ultra
UninstallDisplayIcon={app}\{#MyAppExeName}
AppMutex=KeePassApplicationMutex,Global\KeePassAppMutexExI
ChangesAssociations=yes
VersionInfoVersion={#KeeVersionWin}
VersionInfoCompany={#MyAppPublisher}
VersionInfoDescription={#MyAppFullName} {#KeeVersionStr} Setup
VersionInfoCopyright=Copyright (c) {#KeeDevPeriod} {#MyAppPublisher}
WizardImageFile=compiler:WizModernImage-IS.bmp
WizardSmallImageFile=compiler:WizModernSmallImage-IS.bmp
DisableDirPage=auto
AlwaysShowDirOnReadyPage=yes
DisableProgramGroupPage=yes
AlwaysShowGroupOnReadyPage=no

[Languages]
Name: english; MessagesFile: compiler:Default.isl
Name: brazilianportuguese; MessagesFile: compiler:Languages\BrazilianPortuguese.isl
Name: catalan; MessagesFile: compiler:Languages\Catalan.isl
Name: czech; MessagesFile: compiler:Languages\Czech.isl
Name: danish; MessagesFile: compiler:Languages\Danish.isl
Name: dutch; MessagesFile: compiler:Languages\Dutch.isl
Name: finnish; MessagesFile: compiler:Languages\Finnish.isl
Name: french; MessagesFile: compiler:Languages\French.isl
Name: german; MessagesFile: compiler:Languages\German.isl
Name: hungarian; MessagesFile: compiler:Languages\Hungarian.isl
Name: italian; MessagesFile: compiler:Languages\Italian.isl
Name: norwegian; MessagesFile: compiler:Languages\Norwegian.isl
Name: polish; MessagesFile: compiler:Languages\Polish.isl
Name: portuguese; MessagesFile: compiler:Languages\Portuguese.isl
Name: russian; MessagesFile: compiler:Languages\Russian.isl
Name: slovak; MessagesFile: compiler:Languages\Slovak.isl
Name: slovenian; MessagesFile: compiler:Languages\Slovenian.isl
Name: spanish; MessagesFile: compiler:Languages\Spanish.isl

[Tasks]
Name: fileassoc; Description: {cm:AssocFileExtension,{#MyAppName},.kdb}
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked
Name: quicklaunchicon; Description: {cm:CreateQuickLaunchIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked

[Files]
Source: ..\Build\WinGUI_Distrib\KeePass.exe; DestDir: {app}; Flags: ignoreversion
Source: ..\Build\WinGUI_Distrib\KeePass.chm; DestDir: {app}; Flags: ignoreversion
Source: ..\Build\WinGUI_Distrib\KeePass.ini; DestDir: {app}; Flags: onlyifdoesntexist
Source: ..\Build\WinGUI_Distrib\License.txt; DestDir: {app}; Flags: ignoreversion

[Registry]
; Always unregister .kdb association at uninstall
Root: HKCR; Subkey: .kdb; Flags: uninsdeletekey; Tasks: not fileassoc
Root: HKCR; Subkey: kdbfile; Flags: uninsdeletekey; Tasks: not fileassoc
; Register .kdb association at install, and unregister at uninstall
Root: HKCR; Subkey: .kdb; ValueType: string; ValueData: kdbfile; Flags: uninsdeletekey; Tasks: fileassoc
Root: HKCR; Subkey: kdbfile; ValueType: string; ValueData: KeePass Password Database; Flags: uninsdeletekey; Tasks: fileassoc
Root: HKCR; Subkey: kdbfile; ValueType: string; ValueName: AlwaysShowExt; Flags: uninsdeletekey; Tasks: fileassoc
Root: HKCR; Subkey: kdbfile\DefaultIcon; ValueType: string; ValueData: """{app}\{#MyAppExeName}"",0"; Flags: uninsdeletekey; Tasks: fileassoc
Root: HKCR; Subkey: kdbfile\shell\open; ValueType: string; ValueData: &Open with {#MyAppName}; Flags: uninsdeletekey; Tasks: fileassoc
Root: HKCR; Subkey: kdbfile\shell\open\command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Flags: uninsdeletekey; Tasks: fileassoc

; [INI]
; Filename: {app}\{#MyAppUrlName}; Section: InternetShortcut; Key: URL; String: {#MyAppURL}

[Icons]
; Name: {group}\{#MyAppName}; Filename: {app}\{#MyAppExeName}
; Name: {group}\{cm:ProgramOnTheWeb,{#MyAppName}}; Filename: {app}\{#MyAppUrlName}
; Name: {group}\Help; Filename: {app}\{#MyAppHelpName}
; Name: {group}\{cm:UninstallProgram,{#MyAppName}}; Filename: {uninstallexe}
Name: {commonprograms}\{#MyAppName}; Filename: {app}\{#MyAppExeName}
Name: {userdesktop}\{#MyAppName}; Filename: {app}\{#MyAppExeName}; Tasks: desktopicon
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}; Filename: {app}\{#MyAppExeName}; Tasks: quicklaunchicon

[Run]
Filename: {app}\{#MyAppExeName}; Description: {cm:LaunchProgram,{#MyAppName}}; Flags: postinstall nowait skipifsilent unchecked
Filename: http://keepass.info/plugins.html; Description: Visit &plugins web page (browser integration, ...); Flags: postinstall shellexec skipifsilent

; Delete old files when upgrading
[InstallDelete]
Name: {app}\{#MyAppUrlName}; Type: files
Name: {group}\{#MyAppName}.lnk; Type: files
Name: {group}\{cm:ProgramOnTheWeb,{#MyAppName}}.lnk; Type: files
Name: {group}\Help.lnk; Type: files
Name: {group}\{cm:UninstallProgram,{#MyAppName}}.lnk; Type: files
Name: {group}; Type: dirifempty

; [UninstallDelete]
; Type: files; Name: {app}\{#MyAppUrlName}
