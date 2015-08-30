; Swedish KeePass Password Safe Localization Script
; multilanguage adaptation by Lubos Stanek

;[Types]
; translate Descriptions
;Name: full; Description: "Full installation"; Languages: sv
;Name: custom; Description: "Custom installation"; Flags: iscustom; Languages: sv

;[Components]
; translate Descriptions except {#CrossAppFullName}
;Name: main; Description: "{#CrossAppFullName} application"; Types: full custom; Flags: fixed; Languages: sv
;Name: "otherlanguages"; Description: "Other languages support"; Types: full; Languages: sv
;Name: "/czechlanguage"; Description: "Czech language support"; Types: full; Languages: sv
;Name: "/dutchlanguage"; Description: "Dutch language support"; Types: full; Languages: sv
;Name: "/estonianlanguage"; Description: "Estonian language support"; Types: full; Languages: sv
;Name: "/frenchlanguage"; Description: "French language support"; Types: full; Languages: sv
;Name: "/germanlanguage"; Description: "German language support"; Types: full; Languages: sv
;Name: "/koreanlanguage"; Description: "Korean language support"; Types: full; Languages: sv
;Name: "/swedishlanguage"; Description: "Swedish language support"; Types: full; Languages: sv

[Tasks]
; translate Descriptions
Name: "desktopicon"; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"; Languages: sv
Name: "quicklaunchicon"; Description: "Create a &Quick Launch icon"; GroupDescription: "Additional icons:"; Flags: unchecked; Languages: sv

;[INI]
;Filename: "{app}\KeePass.ini"; Section: "KeePass"; Key: "KeeLanguage"; String: "Swedish"; Languages: sv

[Icons]
; translate Names except {#CrossAppFullName}
Name: "{group}\{#CrossAppFullName} on the Web"; Filename: "{app}\KeePass.url"; Languages: sv
Name: "{group}\Uninstall {#CrossAppFullName}"; Filename: "{uninstallexe}"; Languages: sv

[Run]
; translate Description except {#CrossAppFullName}
Filename: "{app}\KeePass.exe"; Description: "Launch {#CrossAppFullName}"; Flags: nowait postinstall skipifsilent; Languages: sv
