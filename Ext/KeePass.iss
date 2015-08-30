; KeePass Password Safe Installation Script
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!
; Thanks to Lubos Stanek for creating a template for this installer.

#define MyAppName "KeePass"
#define MyAppFullName "KeePass Password Safe"
#define MyAppPublisher "Dominik Reichl"
#define MyAppURL "http://keepass.info/"
#define MyAppExeName "KeePass.exe"
#define MyAppUrlName "KeePass.url"

#define KeeVersionStr "1.08"
#define KeeVersionWin "1.0.8.1"

#define KeeDevPeriod "2003-2007"

[Setup]
AppName={#MyAppFullName}
AppVerName={#MyAppFullName} {#KeeVersionStr}
AppVersion={#KeeVersionStr}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppFullName}
DefaultGroupName={#MyAppFullName}
AllowNoIcons=yes
LicenseFile=..\Docs\License_Install.txt
OutputDir=..\Build\WinGUI_Distrib
OutputBaseFilename={#MyAppName}-{#KeeVersionStr}-Setup
Compression=lzma/ultra
SolidCompression=yes
InternalCompressLevel=ultra
UninstallDisplayIcon={app}\{#MyAppName}.exe
AppMutex=KeePassApplicationMutex
ChangesAssociations=yes
VersionInfoVersion={#KeeVersionWin}
VersionInfoCompany={#MyAppPublisher}
VersionInfoDescription={#MyAppFullName} {#KeeVersionStr} Setup
VersionInfoCopyright=Copyright (c) {#KeeDevPeriod} {#MyAppPublisher}

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
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked
Name: quicklaunchicon; Description: {cm:CreateQuickLaunchIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked

[Files]
Source: ..\Build\WinGUI_Distrib\KeePass.exe; DestDir: {app}; Flags: ignoreversion
Source: ..\Build\WinGUI_Distrib\KeePass.chm; DestDir: {app}; Flags: ignoreversion
Source: ..\Build\WinGUI_Distrib\KeePass.ini; DestDir: {app}; Flags: ignoreversion onlyifdoesntexist
Source: ..\Build\WinGUI_Distrib\License.txt; DestDir: {app}; Flags: ignoreversion

[INI]
Filename: {app}\{#MyAppUrlName}; Section: InternetShortcut; Key: URL; String: {#MyAppURL}

[Icons]
Name: {group}\{#MyAppName}; Filename: {app}\{#MyAppExeName}
Name: {group}\{cm:ProgramOnTheWeb,{#MyAppName}}; Filename: {app}\{#MyAppUrlName}
Name: {group}\{cm:UninstallProgram,{#MyAppName}}; Filename: {uninstallexe}
Name: {userdesktop}\{#MyAppName}; Filename: {app}\{#MyAppExeName}; Tasks: desktopicon
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}; Filename: {app}\{#MyAppExeName}; Tasks: quicklaunchicon

[Run]
Filename: {app}\{#MyAppExeName}; Description: {cm:LaunchProgram,{#MyAppName}}; Flags: nowait postinstall skipifsilent unchecked
Filename: http://keepass.info/plugins.html; Flags: postinstall skipifsilent shellexec nowait; Tasks: ; Description: Visit Plugins Website (Browser Integration, ...)

; Unregister .KDB association
[Registry]
Root: HKCR; Subkey: .kdb; Flags: uninsdeletekey
Root: HKCR; Subkey: kdbfile; Flags: uninsdeletekey

[UninstallDelete]
Type: files; Name: {app}\{#MyAppUrlName}
