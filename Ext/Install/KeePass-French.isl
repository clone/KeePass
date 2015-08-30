; Script de localisation Française pour KeePass Password Safe 
; updated for Inno Setup 4.2.1
; Adaptation multi langage par Lubos Stanek

;[Types]
; translate Descriptions
;Name: full; Description: "Installation complète"; Languages: fr
;Name: custom; Description: "Installation personnalisée"; Flags: iscustom; Languages: fr

;[Components]
; translate Descriptions except {#CrossAppFullName}
;Name: main; Description: "{#CrossAppFullName} application"; Types: full custom; Flags: fixed; Languages: fr
;Name: "otherlanguages"; Description: "Autre support de langue"; Types: full; Languages: fr
;Name: "/czechlanguage"; Description: "Support du Tchèque"; Types: full; Languages: fr
;Name: "/dutchlanguage"; Description: "Support du Flamand"; Types: full; Languages: fr
;Name: "/estonianlanguage"; Description: "Support de l'Estonien"; Types: full; Languages: fr
;Name: "/frenchlanguage"; Description: "Support du Français"; Types: full; Languages: fr
;Name: "/germanlanguage"; Description: "Support de l'Allemand"; Types: full; Languages: fr
;Name: "/koreanlanguage"; Description: "Support du Coréen"; Types: full; Languages: fr
;Name: "/swedishlanguage"; Description: "Support du Suédois"; Types: full; Languages: fr

;[INI]
;Filename: "{app}\KeePass.ini"; Section: "KeePass"; Key: "KeeLanguage"; String: "French"; Languages: fr
