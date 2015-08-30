; Czech KeePass Password Safe Localization Script
; updated for Inno Setup 4.2.1
; multilanguage adaptation by Lubos Stanek

;[Types]
; translate Descriptions
;Name: full; Description: "Kompletní instalace"; Languages: cs
;Name: custom; Description: "Uživatelská instalace"; Flags: iscustom; Languages: cs

;[Components]
; translate Descriptions except {#CrossAppFullName}
;Name: main; Description: "Aplikace {#CrossAppFullName}"; Types: full custom; Flags: fixed; Languages: cs
;Name: "otherlanguages"; Description: "Podpora dalších jazykù"; Types: full; Languages: cs
;Name: "/czechlanguage"; Description: "Èeské jazykové soubory"; Types: full; Languages: cs
;Name: "/dutchlanguage"; Description: "Holandské jazykové soubory"; Types: full; Languages: cs
;Name: "/estonianlanguage"; Description: "Estonské jazykové soubory"; Types: full; Languages: cs
;Name: "/frenchlanguage"; Description: "Francouzské jazykové soubory"; Types: full; Languages: cs
;Name: "/germanlanguage"; Description: "Nìmecké jazykové soubory"; Types: full; Languages: cs
;Name: "/koreanlanguage"; Description: "Korejské jazykové soubory"; Types: full; Languages: cs
;Name: "/swedishlanguage"; Description: "Švédské jazykové soubory"; Types: full; Languages: cs

;[INI]
;Filename: "{app}\KeePass.ini"; Section: "KeePass"; Key: "KeeLanguage"; String: "Czech"; Languages: cs
