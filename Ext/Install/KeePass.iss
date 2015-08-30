; KeePass Password Safe Installation Script
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

; Thanks to Lubos Stanek for making this installer script multi-language ready

; The name of the application should remain consistent across languages
#define CrossAppName "KeePass"
#define CrossAppFullName "KeePass Password Safe"
#define KeeVersion "0.93a"
; VersionInfo cannot contain letters, use 3rd number
#define KeeVVersion "4.0.93.10"

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"; LicenseFile: "License.txt"
Name: "cs"; MessagesFile: "InnoMessages\Czech.isl"; LicenseFile: "License-Czech.txt"
Name: "nl"; MessagesFile: "InnoMessages\Dutch.isl"; LicenseFile: "License-Dutch.txt"
; Needs full translation
;Name: "et"; MessagesFile: "InnoMessages\Estonian-???.isl"; LicenseFile: "License-Estonian.txt"
; Needs translation of:
; DirNameTooLong, InvalidDirName on "Select Destination Location"
; GroupNameTooLong, InvalidGroupName on "Select Start Menu Folder"
Name: "fr"; MessagesFile: "InnoMessages\French.isl"; LicenseFile: "License-French.txt"
Name: "de"; MessagesFile: "InnoMessages\German.isl"; LicenseFile: "License-German.txt"
Name: "ko"; MessagesFile: "InnoMessages\Korean.isl"; LicenseFile: "License-Korean.txt"
Name: "sv"; MessagesFile: "InnoMessages\Swedish.isl"; LicenseFile: "License-Swedish.txt"

[Setup]
AppName={#CrossAppFullName}
AppVerName={#CrossAppFullName} {#KeeVersion}
AppVersion={#KeeVersion}
AppPublisher=ReichlSoft
AppPublisherURL=http://keepass.sourceforge.net
AppSupportURL=http://keepass.sourceforge.net
AppUpdatesURL=http://keepass.sourceforge.net
VersionInfoVersion={#KeeVVersion}
DefaultDirName={pf}\{#CrossAppFullName}
DefaultGroupName={#CrossAppFullName}
AllowNoIcons=yes
OutputDir="..\..\Distrib"
OutputBaseFilename={#CrossAppName}-{#KeeVersion}-Setup
UninstallDisplayIcon={app}\{#CrossAppName}.exe

;[Tasks]
; NOTE: The following entry contains English phrases ("Create a desktop icon" and "Additional icons"). You are free to translate them into another language if required.
;Name: "desktopicon"; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"
; NOTE: The following entry contains English phrases ("Create a Quick Launch icon" and "Additional icons"). You are free to translate them into another language if required.
;Name: "quicklaunchicon"; Description: "Create a &Quick Launch icon"; GroupDescription: "Additional icons:"; Flags: unchecked

[Files]
Source: "..\..\Distrib\KeePass.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\Distrib\KeePass.html"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\Distrib\KeePass.ini"; DestDir: "{app}"; Flags: ignoreversion onlyifdoesntexist
Source: "..\..\Distrib\License.html"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[INI]
Filename: "{app}\{#CrossAppName}.url"; Section: "InternetShortcut"; Key: "URL"; String: "http://keepass.sourceforge.net"

[Icons]
Name: "{group}\{#CrossAppFullName}"; Filename: "{app}\{#CrossAppName}.exe"
; NOTE: The following entry contains an English phrase ("on the Web"). You are free to translate it into another language if required.
;Name: "{group}\{#CrossAppFullName} on the Web"; Filename: "{app}\{#CrossAppName}.url"
; NOTE: The following entry contains an English phrase ("Uninstall"). You are free to translate it into another language if required.
;Name: "{group}\Uninstall {#CrossAppFullName}"; Filename: "{uninstallexe}"
Name: "{userdesktop}\{#CrossAppFullName}"; Filename: "{app}\{#CrossAppName}.exe"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#CrossAppFullName}"; Filename: "{app}\{#CrossAppName}.exe"; Tasks: quicklaunchicon

;[Run]
; NOTE: The following entry contains an English phrase ("Launch"). You are free to translate it into another language if required.
;Filename: "{app}\{#CrossAppName}.exe"; Description: "Launch {#CrossAppFullName}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: files; Name: "{app}\{#CrossAppName}.url"

#include "KeePass-Czech.isl"
; needs full translation
#include "KeePass-Dutch.isl"
; needs full translation
#include "KeePass-French.isl"
; needs full translation
#include "KeePass-German.isl"
; needs full translation
#include "KeePass-Korean.isl"
; needs full translation
#include "KeePass-Swedish.isl"

