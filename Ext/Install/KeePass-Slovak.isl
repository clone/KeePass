; Slovak KeePass Password Safe Localization Script
; multilanguage adaptation by Lubos Stanek

;[Types]
; translate Descriptions
;Name: full; Description: "Full installation"; Languages: sk
;Name: custom; Description: "Custom installation"; Flags: iscustom; Languages: sk

;[Components]
; translate Descriptions except {#CrossAppFullName}
;Name: main; Description: "{#CrossAppFullName} application"; Types: full custom; Flags: fixed; Languages: sk
;Name: "otherlanguages"; Description: "Other languages support"; Types: full; Languages: sk
;Name: "/czechlanguage"; Description: "Czech language support"; Types: full; Languages: sk
;Name: "/dutchlanguage"; Description: "Dutch language support"; Types: full; Languages: sk
;Name: "/estonianlanguage"; Description: "Estonian language support"; Types: full; Languages: sk
;Name: "/frenchlanguage"; Description: "French language support"; Types: full; Languages: sk
;Name: "/germanlanguage"; Description: "German language support"; Types: full; Languages: sk
;Name: "/koreanlanguage"; Description: "Korean language support"; Types: full; Languages: sk
;Name: "/swedishlanguage"; Description: "Swedish language support"; Types: full; Languages: sk

;[Tasks]
; translate Descriptions
;Name: "desktopicon"; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"; Languages: sk
;Name: "quicklaunchicon"; Description: "Create a &Quick Launch icon"; GroupDescription: "Additional icons:"; Flags: unchecked; Languages: sk

;[INI]
;Filename: "{app}\KeePass.ini"; Section: "KeePass"; Key: "KeeLanguage"; String: "Korean"; Languages: sk

;[Icons]
; translate Names except {#CrossAppFullName}
;Name: "{group}\{#CrossAppFullName} on the Web"; Filename: "{app}\KeePass.url"; Languages: sk
;Name: "{group}\Uninstall {#CrossAppFullName}"; Filename: "{uninstallexe}"; Languages: sk

;[Run]
; translate Description except {#CrossAppFullName}
;Filename: "{app}\KeePass.exe"; Description: "Launch {#CrossAppFullName}"; Flags: nowait postinstall skipifsilent; Languages: sk
