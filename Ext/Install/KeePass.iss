; KeePass Password Safe Installation Script
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!
; Updated for Inno Setup 4.2.2

; Thanks to Lubos Stanek for making this installer script multi-language ready

; The name of the application should remain consistent across languages
#define CrossAppName "KeePass"
#define CrossAppFullName "KeePass Password Safe"
#define KeeVersion "0.99a"
; VersionInfo cannot contain letters, use 3rd number
#define KeeVVersion "0.9.9.1"

[Languages]
Name: en; MessagesFile: compiler:Default.isl; LicenseFile: License.txt
Name: cs; MessagesFile: compiler:Languages\Czech.isl; LicenseFile: License-Czech.txt
Name: nl; MessagesFile: compiler:Languages\Dutch.isl; LicenseFile: License-Dutch.txt
; Name: "et"; MessagesFile: "InnoMessages\Estonian.isl"; LicenseFile: "License-Estonian.txt"
Name: fr; MessagesFile: compiler:Languages\French.isl; LicenseFile: License-French.txt
Name: de; MessagesFile: compiler:Languages\German.isl; LicenseFile: License-German.txt
Name: ko; MessagesFile: InnoMessages\Korean.isl; LicenseFile: License-Korean.txt
Name: sv; MessagesFile: InnoMessages\Swedish.isl; LicenseFile: License-Swedish.txt

[Setup]
AppName={#CrossAppFullName}
AppVerName={#CrossAppFullName} {#KeeVersion}
AppVersion={#KeeVersion}
AppPublisher=Dominik Reichl
AppPublisherURL=http://keepass.sourceforge.net/
AppSupportURL=http://keepass.sourceforge.net/
AppUpdatesURL=http://keepass.sourceforge.net/
VersionInfoVersion={#KeeVVersion}
DefaultDirName={pf}\{#CrossAppFullName}
DefaultGroupName={#CrossAppFullName}
AllowNoIcons=yes
OutputDir=..\..\Distrib
OutputBaseFilename={#CrossAppName}-{#KeeVersion}-Setup
UninstallDisplayIcon={app}\{#CrossAppName}.exe
AppMutex=KeePassApplicationMutex
ChangesAssociations=yes
VersionInfoCompany=Dominik Reichl
VersionInfoDescription={#CrossAppFullName} Setup

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Languages: en cs nl fr de ko sv
Name: quicklaunchicon; Description: {cm:CreateQuickLaunchIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked; Languages: en cs nl fr de ko sv

[Files]
Source: ..\..\Distrib\KeePass.exe; DestDir: {app}; Flags: ignoreversion
Source: ..\..\Distrib\KeePass.chm; DestDir: {app}; Flags: ignoreversion
Source: ..\..\Distrib\KeePass.ini; DestDir: {app}; Flags: ignoreversion onlyifdoesntexist
Source: ..\..\Distrib\License.html; DestDir: {app}; Flags: ignoreversion

[INI]
Filename: {app}\{#CrossAppName}.url; Section: InternetShortcut; Key: URL; String: http://keepass.sourceforge.net/
; Filename: "{app}\KeePass.ini"; Section: "KeePass"; Key: "KeeLanguage"; String: "Standard"; Languages: en

[Icons]
Name: {group}\{#CrossAppFullName}; Filename: {app}\{#CrossAppName}.exe; Languages: en cs nl fr de ko sv
Name: {group}\{cm:ProgramOnTheWeb,{#CrossAppFullName}}; Filename: {app}\{#CrossAppName}.url; Languages: en cs nl fr de ko sv
Name: {group}\{cm:UninstallProgram,{#CrossAppFullName}}; Filename: {uninstallexe}; Languages: en cs nl fr de ko sv
Name: {userdesktop}\{#CrossAppFullName}; Filename: {app}\{#CrossAppName}.exe; Tasks: desktopicon; Languages: en cs nl fr de ko sv
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\{#CrossAppFullName}; Filename: {app}\{#CrossAppName}.exe; Tasks: quicklaunchicon; Languages: en cs nl fr de ko sv

[Run]
Filename: {app}\{#CrossAppName}.exe; Description: {cm:LaunchProgram,{#CrossAppFullName}}; Flags: nowait postinstall skipifsilent; Languages: en cs nl fr de ko sv

; A hack to unregister .kdb association
[Registry]
Root: HKCR; Subkey: .kdb; Flags: uninsdeletekey
Root: HKCR; Subkey: kdbfile; Flags: uninsdeletekey

[UninstallDelete]
Type: files; Name: {app}\{#CrossAppName}.url

#include "KeePass-Czech.isl"
#include "KeePass-Dutch.isl"
#include "KeePass-French.isl"
#include "KeePass-German.isl"
#include "KeePass-Korean.isl"
#include "KeePass-Swedish.isl"
