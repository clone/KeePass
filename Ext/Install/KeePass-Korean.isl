; Korean KeePass Password Safe Localization Script
; updated for Inno Setup 4.2.1
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

;[INI]
;Filename: "{app}\KeePass.ini"; Section: "KeePass"; Key: "KeeLanguage"; String: "Korean"; Languages: ko
