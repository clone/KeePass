; Czech KeePass Password Safe Localization Script
; multilanguage adaptation by Lubos Stanek

;[Types]
; translate Descriptions
;Name: full; Description: "Kompletn� instalace"; Languages: cs
;Name: custom; Description: "U�ivatelsk� instalace"; Flags: iscustom; Languages: cs

;[Components]
; translate Descriptions except {#CrossAppFullName}
;Name: main; Description: "Aplikace {#CrossAppFullName}"; Types: full custom; Flags: fixed; Languages: cs
;Name: "otherlanguages"; Description: "Podpora dal��ch jazyk�"; Types: full; Languages: cs
;Name: "/czechlanguage"; Description: "�esk� jazykov� soubory"; Types: full; Languages: cs
;Name: "/dutchlanguage"; Description: "Holandsk� jazykov� soubory"; Types: full; Languages: cs
;Name: "/estonianlanguage"; Description: "Estonsk� jazykov� soubory"; Types: full; Languages: cs
;Name: "/frenchlanguage"; Description: "Francouzsk� jazykov� soubory"; Types: full; Languages: cs
;Name: "/germanlanguage"; Description: "N�meck� jazykov� soubory"; Types: full; Languages: cs
;Name: "/koreanlanguage"; Description: "Korejsk� jazykov� soubory"; Types: full; Languages: cs
;Name: "/swedishlanguage"; Description: "�v�dsk� jazykov� soubory"; Types: full; Languages: cs

[Tasks]
; translate Descriptions, GroupDescriptions
Name: "desktopicon"; Description: "Vytvo�it z�stupce na &plo�e"; GroupDescription: "Dal�� ikony:"; Languages: cs
Name: "quicklaunchicon"; Description: "Vytvo�it z�stupce na panelu &Snadn� spu�t�n�"; GroupDescription: "Dal�� ikony:"; Flags: unchecked; Languages: cs

;[INI]
;Filename: "{app}\KeePass.ini"; Section: "KeePass"; Key: "KeeLanguage"; String: "Czech"; Languages: cs

[Icons]
; translate Names except {#CrossAppFullName}
Name: "{group}\{#CrossAppFullName} na Internetu"; Filename: "{app}\KeePass.url"; Languages: cs
Name: "{group}\Odinstalovat {#CrossAppFullName}"; Filename: "{uninstallexe}"; Languages: cs

[Run]
; translate Description except {#CrossAppFullName}
Filename: "{app}\KeePass.exe"; Description: "Spustit {#CrossAppFullName}"; Flags: nowait postinstall skipifsilent; Languages: cs
