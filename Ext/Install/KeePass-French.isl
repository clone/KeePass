; French KeePass Password Safe Localization Script
; multilanguage adaptation by Lubos Stanek

;[Types]
; translate Descriptions
;Name: full; Description: "Full installation"; Languages: fr
;Name: custom; Description: "Custom installation"; Flags: iscustom; Languages: fr

;[Components]
; translate Descriptions except {#CrossAppFullName}
;Name: main; Description: "{#CrossAppFullName} application"; Types: full custom; Flags: fixed; Languages: fr
;Name: "otherlanguages"; Description: "Other languages support"; Types: full; Languages: fr
;Name: "/czechlanguage"; Description: "Czech language support"; Types: full; Languages: fr
;Name: "/dutchlanguage"; Description: "Dutch language support"; Types: full; Languages: fr
;Name: "/estonianlanguage"; Description: "Estonian language support"; Types: full; Languages: fr
;Name: "/frenchlanguage"; Description: "French language support"; Types: full; Languages: fr
;Name: "/germanlanguage"; Description: "German language support"; Types: full; Languages: fr
;Name: "/koreanlanguage"; Description: "Korean language support"; Types: full; Languages: fr
;Name: "/swedishlanguage"; Description: "Swedish language support"; Types: full; Languages: fr

[Tasks]
; translate Descriptions
Name: "desktopicon"; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"; Languages: fr
Name: "quicklaunchicon"; Description: "Create a &Quick Launch icon"; GroupDescription: "Additional icons:"; Flags: unchecked; Languages: fr

;[INI]
;Filename: "{app}\KeePass.ini"; Section: "KeePass"; Key: "KeeLanguage"; String: "French"; Languages: fr

[Icons]
; translate Names except {#CrossAppFullName}
Name: "{group}\{#CrossAppFullName} on the Web"; Filename: "{app}\KeePass.url"; Languages: fr
Name: "{group}\Uninstall {#CrossAppFullName}"; Filename: "{uninstallexe}"; Languages: fr

[Run]
; translate Description except {#CrossAppFullName}
Filename: "{app}\KeePass.exe"; Description: "Launch {#CrossAppFullName}"; Flags: nowait postinstall skipifsilent; Languages: fr
