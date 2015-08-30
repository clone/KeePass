; German KeePass Password Safe Localization Script
; updated for Inno Setup 4.2.1
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

;[INI]
;Filename: "{app}\KeePass.ini"; Section: "KeePass"; Key: "KeeLanguage"; String: "German"; Languages: de
