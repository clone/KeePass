; Dutch KeePass Password Safe Localization Script
; multilanguage adaptation by Lubos Stanek

;[Types]
; translate Descriptions
;Name: full; Description: "Full installation"; Languages: nl
;Name: custom; Description: "Custom installation"; Flags: iscustom; Languages: nl

;[Components]
; translate Descriptions except {#CrossAppFullName}
;Name: main; Description: "{#CrossAppFullName} application"; Types: full custom; Flags: fixed; Languages: nl
;Name: "otherlanguages"; Description: "Other languages support"; Types: full; Languages: nl
;Name: "/czechlanguage"; Description: "Czech language support"; Types: full; Languages: nl
;Name: "/dutchlanguage"; Description: "Dutch language support"; Types: full; Languages: nl
;Name: "/estonianlanguage"; Description: "Estonian language support"; Types: full; Languages: nl
;Name: "/frenchlanguage"; Description: "French language support"; Types: full; Languages: nl
;Name: "/germanlanguage"; Description: "German language support"; Types: full; Languages: nl
;Name: "/koreanlanguage"; Description: "Korean language support"; Types: full; Languages: nl
;Name: "/swedishlanguage"; Description: "Swedish language support"; Types: full; Languages: nl

[Tasks]
; translate Descriptions
Name: "desktopicon"; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"; Languages: nl
Name: "quicklaunchicon"; Description: "Create a &Quick Launch icon"; GroupDescription: "Additional icons:"; Flags: unchecked; Languages: nl

;[INI]
;Filename: "{app}\KeePass.ini"; Section: "KeePass"; Key: "KeeLanguage"; String: "Dutch"; Languages: nl

[Icons]
; translate Names except {#CrossAppFullName}
Name: "{group}\{#CrossAppFullName} on the Web"; Filename: "{app}\KeePass.url"; Languages: nl
Name: "{group}\Uninstall {#CrossAppFullName}"; Filename: "{uninstallexe}"; Languages: nl

[Run]
; translate Description except {#CrossAppFullName}
Filename: "{app}\KeePass.exe"; Description: "Launch {#CrossAppFullName}"; Flags: nowait postinstall skipifsilent; Languages: nl
