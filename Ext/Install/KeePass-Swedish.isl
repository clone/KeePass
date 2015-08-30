; Swedish KeePass Password Safe Localization Script
; updated for Inno Setup 4.2.1
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

;[INI]
;Filename: "{app}\KeePass.ini"; Section: "KeePass"; Key: "KeeLanguage"; String: "Swedish"; Languages: sv
