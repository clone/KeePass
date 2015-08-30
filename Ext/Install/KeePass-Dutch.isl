; Dutch KeePass Password Safe Localization Script
; multilanguage adaptation by Lubos Stanek

;[Types]
; translate Descriptions
;Name: full; Description: "Volledige installatie"; Languages: nl
;Name: custom; Description: "Aangepaste installatie"; Flags: iscustom; Languages: nl

;[Components]
; translate Descriptions except {#CrossAppFullName}
;Name: main; Description: "{#CrossAppFullName} applicatie"; Types: full custom; Flags: fixed; Languages: nl
;Name: "otherlanguages"; Description: "Ondersteun andere talen"; Types: full; Languages: nl
;Name: "/czechlanguage"; Description: "Ondersteun Tsjechische taal"; Types: full; Languages: nl
;Name: "/dutchlanguage"; Description: "Ondersteun Nederlandse taal"; Types: full; Languages: nl
;Name: "/estonianlanguage"; Description: "Ondersteun Estische taal"; Types: full; Languages: nl
;Name: "/frenchlanguage"; Description: "Ondersteun Franse taal"; Types: full; Languages: nl
;Name: "/germanlanguage"; Description: "Ondersteun Duitse taal"; Types: full; Languages: nl
;Name: "/koreanlanguage"; Description: "Ondersteun Koreaanse taal"; Types: full; Languages: nl
;Name: "/swedishlanguage"; Description: "Ondersteun Zweedse taal"; Types: full; Languages: nl

[Tasks]
; translate Descriptions
Name: "desktopicon"; Description: "Maak een &Bureaublad pictogram"; GroupDescription: "Extra pictogrammen:"; Languages: nl
Name: "quicklaunchicon"; Description: "Maak een &Snelstart pictogram"; GroupDescription: "Extra pictogrammen:"; Flags: unchecked; Languages: nl

;[INI]
;Filename: "{app}\KeePass.ini"; Section: "KeePass"; Key: "KeeLanguage"; String: "Dutch"; Languages: nl

[Icons]
; translate Names except {#CrossAppFullName}
Name: "{group}\{#CrossAppFullName} op het web"; Filename: "{app}\KeePass.url"; Languages: nl
Name: "{group}\Deïnstalleer {#CrossAppFullName}"; Filename: "{uninstallexe}"; Languages: nl

[Run]
; translate Description except {#CrossAppFullName}
Filename: "{app}\KeePass.exe"; Description: "Start {#CrossAppFullName}"; Flags: nowait postinstall skipifsilent; Languages: nl

