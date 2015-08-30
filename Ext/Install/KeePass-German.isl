; German KeePass Password Safe Localization Script
; multilanguage adaptation by Lubos Stanek

;[Types]
; translate Descriptions
;Name: full; Description: "Full installation"; Languages: de
;Name: custom; Description: "Custom installation"; Flags: iscustom; Languages: de

;[Components]
; translate Descriptions except {#CrossAppFullName}
;Name: main; Description: "{#CrossAppFullName} application"; Types: full custom; Flags: fixed; Languages: de
;Name: "otherlanguages"; Description: "Other languages support"; Types: full; Languages: de
;Name: "/czechlanguage"; Description: "Czech language support"; Types: full; Languages: de
;Name: "/dutchlanguage"; Description: "Dutch language support"; Types: full; Languages: de
;Name: "/estonianlanguage"; Description: "Estonian language support"; Types: full; Languages: de
;Name: "/frenchlanguage"; Description: "French language support"; Types: full; Languages: de
;Name: "/germanlanguage"; Description: "German language support"; Types: full; Languages: de
;Name: "/koreanlanguage"; Description: "Korean language support"; Types: full; Languages: de
;Name: "/swedishlanguage"; Description: "Swedish language support"; Types: full; Languages: de

[Tasks]
; translate Descriptions
Name: "desktopicon"; Description: "&Desktop-Icon erstellen"; GroupDescription: "Zusätzliche Icons:"; Languages: de
Name: "quicklaunchicon"; Description: "&Schnellstart-Icon in Taskleiste erstellen"; GroupDescription: "Zusätzliche Icons:"; Flags: unchecked; Languages: de

;[INI]
;Filename: "{app}\KeePass.ini"; Section: "KeePass"; Key: "KeeLanguage"; String: "German"; Languages: de

[Icons]
; translate Names except {#CrossAppFullName}
Name: "{group}\{#CrossAppFullName}-Homepage besuchen"; Filename: "{app}\KeePass.url"; Languages: de
Name: "{group}\{#CrossAppFullName} deinstallieren"; Filename: "{uninstallexe}"; Languages: de

[Run]
; translate Description except {#CrossAppFullName}
Filename: "{app}\KeePass.exe"; Description: "{#CrossAppFullName} starten"; Flags: nowait postinstall skipifsilent; Languages: de
