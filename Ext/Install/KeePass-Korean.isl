; Korean KeePass Password Safe Localization Script
; multilanguage adaptation by Lubos Stanek

;[Types]
; translate Descriptions
;Name: full; Description: "Full installation"; Languages: ko
;Name: custom; Description: "Custom installation"; Flags: iscustom; Languages: ko

;[Components]
; translate Descriptions except {#CrossAppFullName}
;Name: main; Description: "{#CrossAppFullName} application"; Types: full custom; Flags: fixed; Languages: ko
;Name: "otherlanguages"; Description: "Other languages support"; Types: full; Languages: ko
;Name: "/czechlanguage"; Description: "Czech language support"; Types: full; Languages: ko
;Name: "/dutchlanguage"; Description: "Dutch language support"; Types: full; Languages: ko
;Name: "/estonianlanguage"; Description: "Estonian language support"; Types: full; Languages: ko
;Name: "/frenchlanguage"; Description: "French language support"; Types: full; Languages: ko
;Name: "/germanlanguage"; Description: "German language support"; Types: full; Languages: ko
;Name: "/koreanlanguage"; Description: "Korean language support"; Types: full; Languages: ko
;Name: "/swedishlanguage"; Description: "Swedish language support"; Types: full; Languages: ko

[Tasks]
; translate Descriptions
Name: "desktopicon"; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"; Languages: ko
Name: "quicklaunchicon"; Description: "Create a &Quick Launch icon"; GroupDescription: "Additional icons:"; Flags: unchecked; Languages: ko

;[INI]
;Filename: "{app}\KeePass.ini"; Section: "KeePass"; Key: "KeeLanguage"; String: "Korean"; Languages: ko

[Icons]
; translate Names except {#CrossAppFullName}
Name: "{group}\{#CrossAppFullName} on the Web"; Filename: "{app}\KeePass.url"; Languages: ko
Name: "{group}\Uninstall {#CrossAppFullName}"; Filename: "{uninstallexe}"; Languages: ko

[Run]
; translate Description except {#CrossAppFullName}
Filename: "{app}\KeePass.exe"; Description: "Launch {#CrossAppFullName}"; Flags: nowait postinstall skipifsilent; Languages: ko
