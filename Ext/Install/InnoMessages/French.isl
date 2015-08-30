; *** Inno Setup version 4.1.8+ French messages ***
;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/is3rdparty.php
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;
; $jrsoftware: issrc/Files/Default.isl,v 1.42 2004/01/22 00:49:23 jr Exp $
; Pierre Yager                                2004/01/29
; Fr�d�ric Bonduelle                          2004/01/30
; Pierre Yager                                2004/02/09
; Pierre Yager                                2004/02/26

[LangOptions]
LanguageName=Fran�ais
LanguageID=$040C
; Si la langue dans laquelle vous traduisez InnoSetup requiert des polices ou des
; tailles diff�rentes des valeurs par d�faut, d�-commentez les lignes suivantes 
; n�cessaires et modifiez les en cons�quence.
;DialogFontName=
;DialogFontSize=8
;WelcomeFontName=Verdana
;WelcomeFontSize=12
;TitleFontName=Arial
;TitleFontSize=29
;CopyrightFontName=Arial
;CopyrightFontSize=8

[Messages]

; *** Application titles
SetupAppTitle=Installation
SetupWindowTitle=Installation de %1
UninstallAppTitle=D�sinstallation
UninstallAppFullTitle=D�sinstallation de %1

; *** Misc. common
InformationTitle=Information
ConfirmTitle=Confirmation
ErrorTitle=Erreur

; *** SetupLdr messages
SetupLdrStartupMessage=Cet assistant va installer %1. Voulez-vous poursuivre ?
LdrCannotCreateTemp=Impossible de cr�er un fichier temporaire. Abandon de l'installation
LdrCannotExecTemp=Impossible d'ex�cuter un fichier depuis le dossier temporaire. Abandon de l'installation

; *** Startup error messages
LastErrorMessage=%1.%n%nErreur %2 : %3
SetupFileMissing=Le fichier %1 est absent du dossier d'installation. Veuillez corriger le probl�me ou vous procurer une nouvelle copie du logiciel.
SetupFileCorrupt=Les fichiers d'installation sont alt�r�s. Veuillez vous procurer une nouvelle copie du logiciel.
SetupFileCorruptOrWrongVer=Les fichiers d'installation sont alt�r�s ou ne sont pas compatibles avec cette version de l'assistant d'installation. Veuillez corriger le probl�me ou vous procurer une nouvelle copie du logiciel.
NotOnThisPlatform=Ce logiciel ne fonctionnera pas sous %1.
OnlyOnThisPlatform=Ce logiciel ne peut fonctionner que sous %1.
WinVersionTooLowError=Ce logiciel requiert la version %2 ou sup�rieure de %1.
WinVersionTooHighError=Ce logiciel ne peut pas �tre install� sous %1 version %2 ou sup�rieure.
AdminPrivilegesRequired=Vous devez disposer des droits d'administration de cet ordinateur pour installer ce logiciel.
PowerUserPrivilegesRequired=Vous devez disposer des droits d'administration ou faire partie du groupe "Utilisateurs avec pouvoir" de cet ordinateur pour installer ce logiciel.
SetupAppRunningError=L'assistant d'installation a d�tect� que %1 est actuellement en cours d'ex�cution.%n%nVeuillez fermer toutes les instances de cette application puis appuyer sur OK pour continuer, ou bien appuyer sur Annuler pour abandonner l'installation.
UninstallAppRunningError=La proc�dure de d�sinstallation a d�tect� que %1 est actuellement en cours d'ex�cution.%n%nVeuillez fermer toutes les instances de cette application  puis appuyer sur OK pour continuer, ou bien appuyer sur Annuler pour abandonner la d�sinstallation.

; *** Misc. errors
ErrorCreatingDir=L'assistant d'installation n'a pas pu cr�er le dossier "%1"
ErrorTooManyFilesInDir=L'assistant d'installation n'a pas pu cr�er un fichier dans le dossier "%1" car celui-ci contient trop de fichiers

; *** Setup common messages
ExitSetupTitle=Quitter l'installation
ExitSetupMessage=L'installation n'est pas termin�e. Si vous abandonnez maintenant, le logiciel ne sera pas install�.%n%nVous devrez relancer cet assistant pour finir l'installation.%n%nVoulez-vous quand m�me quitter l'assistant d'installation ?
AboutSetupMenuItem=&A propos...
AboutSetupTitle=A Propos de l'assistant d'installation
AboutSetupMessage=%1 version %2%n%3%n%n%1 page d'accueil :%n%4
AboutSetupNote=

; *** Buttons
ButtonBack=< &Pr�c�dent
ButtonNext=&Suivant >
ButtonInstall=&Installer
ButtonOK=OK
ButtonCancel=Annuler
ButtonYes=&Oui
ButtonYesToAll=Oui pour &tout
ButtonNo=&Non
ButtonNoToAll=N&on pour tout
ButtonFinish=&Terminer
ButtonBrowse=&Parcourir...
ButtonWizardBrowse=Pa&rcourir...
ButtonNewFolder=&Nouveau dossier

; *** "Select Language" dialog messages
SelectLanguageTitle=Langue de l'assistant d'installation
SelectLanguageLabel=Veuillez s�lectionner la langue qui sera utilis�e par l'assistant d'installation :

; *** Common wizard text
ClickNext=Appuyez sur Suivant pour continuer ou sur Annuler pour abandonner l'installation.
BeveledLabel=
BrowseDialogTitle=Choix d'un dossier
BrowseDialogLabel=Veuillez choisir un dossier de destination, puis appuyez sur OK.
NewFolderName=Nouveau dossier

; *** "Welcome" wizard page
WelcomeLabel1=Bienvenue dans l'installation de [name]
WelcomeLabel2=Cet assistant va vous guider pour l'installation de [name/ver] sur votre ordinateur.%n%nIl est recommand� de fermer toutes les applications actives avant de poursuivre.

; *** "Password" wizard page
WizardPassword=Mot de passe
PasswordLabel1=Cette installation est prot�g�e par un mot de passe.
PasswordLabel3=Veuillez saisir votre mot de passe (attention � la distinction entre majuscules et minuscules) puis appuyez sur Suivant pour continuer.
PasswordEditLabel=Mot de &passe :
IncorrectPassword=Le mot de passe saisi n'est pas valide. Essayez � nouveau.

; *** "License Agreement" wizard page
WizardLicense=Accord de la licence d'utilisation
LicenseLabel=Veuillez lire les informations importantes ci-dessous avant de continuer.
LicenseLabel3=Veuillez lire le contrat de la licence d'utilisation ci-dessous. Vous devez accepter tous les termes de ce contrat avant de poursuivre l'installation.
LicenseAccepted=J'&accepte les termes du contrat
LicenseNotAccepted=Je n'accepte &pas les termes du contrat

; *** "Information" wizard pages
WizardInfoBefore=Information
InfoBeforeLabel=Veuillez lire les informations importantes ci-dessous avant de continuer.
InfoBeforeClickLabel=Appuyez sur Suivant lorsque vous serez pr�t(e) � poursuivre l'installation.
WizardInfoAfter=Informations
InfoAfterLabel=Veuillez lire les informations importantes ci-dessous avant de continuer.
InfoAfterClickLabel=Appuyez sur Suivant lorsque vous serez pr�t(e) � poursuivre l'installation.

; *** "User Information" wizard page
WizardUserInfo=Informations sur l'Utilisateur
UserInfoDesc=Veuillez saisir les informations qui vous concernent.
UserInfoName=&Nom d'utilisateur :
UserInfoOrg=&Soci�t� :
UserInfoSerial=Num�ro de &s�rie :
UserInfoNameRequired=Vous devez au moins saisir un nom.

; *** "Select Destination Location" wizard page
WizardSelectDir=Dossier de destination
SelectDirDesc=O� [name] doit-il �tre install� ?
SelectDirLabel3=L'assistant va installer [name] dans le dossier suivant.
SelectDirBrowseLabel=Pour continuer, appuyez sur Suivant. Si vous souhaitez choisir un dossier diff�rent, appuyez sur Parcourir.
DiskSpaceMBLabel=Le logiciel requiert au moins [mb] Mo d'espace disque disponible.
ToUNCPathname=L'assistant ne peut pas proc�der � l'installation � un emplacement d�fini par un chemin UNC. Si vous souhaitez effectuer cette installation sur un r�seau, vous devez au pr�alable connecter un lecteur r�seau.
InvalidPath=Vous devez saisir un chemin complet comprenant la lettre identifiant l'unit�, par exemple :%n%nC:\DOSSIER%n%nou un chemin UNC de la forme :%n%n\\serveur\partage
InvalidDrive=L'unit� ou l'emplacement UNC que vous avez s�lectionn�(e) n'existe pas ou n'est pas accessible. Veuillez choisir une autre destination.
DiskSpaceWarningTitle=L'espace disque disponible est insuffisant
DiskSpaceWarning=L'assistant a besoin d'au moins %1 Ko d'espace disque disponible pour effectuer l'installation, mais l'unit� que vous avez s�lectionn�e ne dispose que de %2 Ko d'espace disponible.%n%nSouhaitez-vous poursuivre malgr� tout ?
DirNameTooLong=Le nom ou le chemin du dossier est trop long.
InvalidDirName=Le nom du dossier est invalide.
BadDirName32=Le nom du dossier ne doit contenir aucun des caract�res suivants :%n%n%1
DirExistsTitle=Dossier existant
DirExists=Le dossier :%n%n%1%n%nexiste d�j�. Souhaitez-vous l'utiliser quand m�me ?
DirDoesntExistTitle=Le dossier n'existe pas
DirDoesntExist=Le dossier %n%n%1%n%nn'existe pas. Souhaitez-vous que ce dossier soit cr�� ?

; *** "Select Components" wizard page
WizardSelectComponents=Composants � installer
SelectComponentsDesc=Quels composants de l'application souhaitez-vous installer ?
SelectComponentsLabel2=S�lectionnez les composants que vous d�sirez installer; d�cochez les composants que vous ne d�sirez pas installer. Appuyez ensuite sur Suivant pour poursuivre l'installation.
FullInstallation=Installation compl�te
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Installation compacte
CustomInstallation=Installation personnalis�e
NoUninstallWarningTitle=Composants existants
NoUninstallWarning=L'assistant d'installation a d�tect� que les composants suivants sont d�j� install�s sur votre syst�me :%n%n%1%n%nD�cocher ces composants ne les d�sinstallera pas pour autant.%n%nVoulez-vous tout-de-m�me continuer ?
ComponentSize1=%1 Ko
ComponentSize2=%1 Mo
ComponentsDiskSpaceMBLabel=Les composants s�lectionn�s n�cessitent au moins [mb] Mo d'espace disque disponible.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=T�ches suppl�mentaires
SelectTasksDesc=Quelles sont les t�ches suppl�mentaires qui doivent �tre effectu�es ?
SelectTasksLabel2=S�lectionnez les t�ches suppl�mentaires que l'assistant d'installation doit effectuer pendant l'installation de [name], puis appuyez sur Suivant.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=S�lection du dossier du menu D�marrer
SelectStartMenuFolderDesc=O� l'assistant d'installation doit-il placer les raccourcis du logiciel ?
SelectStartMenuFolderLabel3=L'assistant va cr�er les raccourcis du logiciel dans le dossier du menu D�marrer indiqu� ci-dessous.
SelectStartMenuFolderBrowseLabel=Appuyez sur Suivant pour continuer. Appuyez sur Parcourir si vous souhaitez s�lectionner un autre dossier du menu D�marrer.
NoIconsCheck=&Ne pas cr�er d'ic�ne
MustEnterGroupName=Vous devez saisir un nom de dossier du menu D�marrer.
GroupNameTooLong=Le nom ou le chemin du dossier est trop long.
InvalidGroupName=Le nom du dossier n'est pas valide.
BadGroupName=Le nom du dossier ne doit contenir aucun des caract�res suivants :%n%n%1
NoProgramGroupCheck2=Ne pas cr�er de &dossier dans le menu D�marrer

; *** "Ready to Install" wizard page
WizardReady=Pr�t � installer
ReadyLabel1=L'assistant dispose � pr�sent de toutes les informations pour installer [name] sur votre ordinateur.
ReadyLabel2a=Appuyez sur Installer pour proc�der � l'installation ou sur Pr�c�dent pour revoir ou modifier une option d'installation.
ReadyLabel2b=Appuyez sur Installer pour proc�der � l'installation.
ReadyMemoUserInfo=Informations sur l'Utilisateur :
ReadyMemoDir=Dossier de destination :
ReadyMemoType=Type d'installation :
ReadyMemoComponents=Composants s�lectionn�s :
ReadyMemoGroup=Dossier du menu D�marrer :
ReadyMemoTasks=T�ches suppl�mentaires :

; *** "Preparing to Install" wizard page
WizardPreparing=Pr�paration de l'installation
PreparingDesc=L'assistant d'installation pr�pare l'installation de [name] sur votre ordinateur.
PreviousInstallNotCompleted=L'installation ou la suppression d'un logiciel pr�c�dent n'est pas totalement achev�e. Veuillez red�marrer votre ordinateur pour achever cette installation ou suppression.%n%nUne fois votre ordinateur red�marr�, veuillez relancer cet assistant pour reprendre l'installation de [name].
CannotContinue=L'assistant ne peut pas continuer. Veuillez appuyer sur Annuler pour abandonner l'installation.

; *** "Installing" wizard page
WizardInstalling=Installation en cours
InstallingLabel=Veuillez patienter pendant que l'assistant installe [name] sur votre ordinateur.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Fin de l'installation de [name]
FinishedLabelNoIcons=L'assistant a termin� l'installation de [name] sur votre ordinateur.
FinishedLabel=L'assistant a termin� l'installation de [name] sur votre ordinateur. L'application peut �tre lanc�e � l'aide des ic�nes cr��es sur le Bureau par l'installation.
ClickFinish=Veuillez appuyer sur Terminer pour quitter l'assistant d'installation.
FinishedRestartLabel=L'assistant doit red�marrer votre ordinateur pour terminer l'installation de [name].%n%nVoulez-vous red�marrer maintenant ?
FinishedRestartMessage=L'assistant doit red�marrer votre ordinateur pour terminer l'installation de [name].%n%nVoulez-vous red�marrer maintenant ?
ShowReadmeCheck=Oui, je souhaite lire le fichier LisezMoi
YesRadio=&Oui, red�marrer l'ordinateur maintenant
NoRadio=&Non, je pr�f�re red�marrer l'ordinateur plus tard
; used for example as 'Run MyProg.exe'
RunEntryExec=Ex�cuter %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Voir %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=L'assistant a besoin du disque suivant
SelectDiskLabel2=Veuillez ins�rer le disque %1 et appuyer sur OK.%n%nSi les fichiers de ce disque se trouvent � un emplacement diff�rent de celui indiqu� ci-dessous, veuillez saisir le chemin correspondant ou appuyez sur Parcourir.
PathLabel=&Chemin :
FileNotInDir2=Le fichier "%1" ne peut pas �tre trouv� dans "%2". Veuillez ins�rer le disque correct ou s�lectionner un autre dossier.
SelectDirectoryLabel=Veuillez indiquer l'emplacement du disque suivant.

; *** Installation phase messages
SetupAborted=L'installation n'est pas termin�e.%n%nVeuillez corriger le probl�me et relancer l'installation.
EntryAbortRetryIgnore=Appuyez sur Reprendre pour essayer � nouveau, sur Ignorer pour continuer quand m�me, ou sur Annuler pour abandonner l'installation.

; *** Installation status messages
StatusCreateDirs=Cr�ation des dossiers...
StatusExtractFiles=Extraction des fichiers...
StatusCreateIcons=Cr�ation des raccourcis...
StatusCreateIniEntries=Cr�ation des entr�es du fichier INI...
StatusCreateRegistryEntries=Cr�ation des entr�es de registre...
StatusRegisterFiles=Enregistrement des fichiers...
StatusSavingUninstall=Sauvegarde des informations de d�sinstallation...
StatusRunProgram=Finalisation de l'installation...
StatusRollback=Annulation des modifications...

; *** Misc. errors
ErrorInternal2=Erreur interne : %1
ErrorFunctionFailedNoCode=%1 a �chou�
ErrorFunctionFailed=%1 a �chou�; code %2
ErrorFunctionFailedWithMessage=%1 a �chou�; code %2.%n%3
ErrorExecutingProgram=Impossible d'ex�cuter le fichier :%n%1

; *** Registry errors
ErrorRegOpenKey=Erreur lors de l'ouverture de la cl� de registre :%n%1\%2
ErrorRegCreateKey=Erreur lors de la cr�ation de la cl� de registre :%n%1\%2
ErrorRegWriteKey=Erreur lors de l'�criture de la cl� de registre :%n%1\%2

; *** INI errors
ErrorIniEntry=Erreur d'�criture d'une entr�e dans le fichier INI "%1" .

; *** File copying errors
FileAbortRetryIgnore=Appuyez sur Reprendre pour essayer � nouveau, sur Ignorer pour passer outre ce fichier (non recommand�), ou sur Annuler pour abandonner l'installation.
FileAbortRetryIgnore2=Appuyez sur Reprendre pour essayer � nouveau, sur Ignorer pour poursuivre malgr� tout (non recommand�), ou sur Annuler pour abandonner l'installation.
SourceIsCorrupted=Le fichier source est alt�r�
SourceDoesntExist=Le fichier source "%1" n'existe pas
ExistingFileReadOnly=Le fichier existant est marqu� en lecture seule.%n%nAppuyez sur Reprendre pour retirer l'attribut lecture seule et recommencer, sur Ignorer pour passer outre ce fichier, ou sur Annuler pour abandonner l'installation.
ErrorReadingExistingDest=Une erreur s'est produite lors de la tentative de lecture du fichier existant :
FileExists=Le fichier existe d�j�.%n%nSouhaitez-vous que l'installation le remplace ?
ExistingFileNewer=Le fichier existant est plus r�cent que celui que l'assistant essaie d'installer. Il est recommand� de conserver le fichier existant.%n%nSouhaitez-vous conserver le fichier existant ?
ErrorChangingAttr=Une erreur est survenue en essayant de modifier les attributs du fichier existant :
ErrorCreatingTemp=Une erreur est survenue en essayant de cr�er un fichier dans le dossier de destination :
ErrorReadingSource=Une erreur est survenue lors de la lecture du fichier source :
ErrorCopying=Une erreur est survenue lors de la copie d'un fichier :
ErrorReplacingExistingFile=Une erreur est survenue lors du remplacement d'un fichier existant :
ErrorRestartReplace=Le marquage d'un fichier pour remplacement au red�marrage de l'ordinateur a �chou�:
ErrorRenamingTemp=Une erreur est survenue en essayant de renommer un fichier dans le dossier de destination :
ErrorRegisterServer=Impossible d'enregistrer la DLL ou l'OCX : %1
ErrorRegisterServerMissingExport=La fonction export�e DllRegisterServer n'a pas �t� trouv�e
ErrorRegisterTypeLib=Impossible d'enregistrer la librairie de types : %1

; *** Post-installation errors
ErrorOpeningReadme=Une erreur est survenue � l'ouverture du fichier LisezMoi.
ErrorRestartingComputer=L'installation n'a pas pu red�marrer l'ordinateur. Merci de bien vouloir le faire vous-m�me.

; *** Uninstaller messages
UninstallNotFound=Le fichier "%1" n'existe pas. Impossible de d�sinstaller.
UninstallOpenError=Le fichier "%1" n'a pas pu �tre ouvert. Impossible de d�sinstaller
UninstallUnsupportedVer=Le format du fichier journal de d�sinstallation "%1" n'est pas reconnu par cette version de la proc�dure de d�sinstallation. Impossible de d�sinstaller
UninstallUnknownEntry=Une entr�e inconnue (%1) a �t� rencontr�e dans le fichier journal de d�sinstallation
ConfirmUninstall=Voulez-vous vraiment d�sinstaller compl�tement %1 ainsi que tous ses composants ?
OnlyAdminCanUninstall=Ce logiciel ne peut �tre d�sinstall� que par un utilisateur disposant des droits d'administration.
UninstallStatusLabel=Veuillez patienter pendant que %1 est retir� de votre ordinateur.
UninstalledAll=%1 a �t� correctement d�sinstall� de cet ordinateur.
UninstalledMost=La d�sinstallation de %1 est termin�e.%n%nCertains �l�ments n'ont pu �tre supprim�s automatiquement. Vous pouvez les supprimer manuellement.
UninstalledAndNeedsRestart=Vous devez red�marrer l'ordinateur pour terminer la d�sinstallation de %1.%n%nVoulez-vous red�marrer maintenant ?
UninstallDataCorrupted=Le ficher "%1" est alt�r�. Impossible de d�sinstaller

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Supprimer les fichiers partag�s ?
ConfirmDeleteSharedFile2=Le syst�me indique que les fichiers suivants sont partag�s mais ne sont plus utilis�s par d'autres applications. Souhaitez-vous que la proc�dure de d�sinstallation supprime ces fichiers partag�s ?%n%nSi certaines applications ont besoin de ces fichiers et qu'il sont supprim�s, ces applications risquent de ne plus fonctionner correctement. En cas de doute, appuyez sur Non; Le fait de laisser ce fichier sur votre syst�me ne causera aucun dommage.
SharedFileNameLabel=Nom du fichier :
SharedFileLocationLabel=Emplacement :
WizardUninstalling=Etat de la d�sinstallation
StatusUninstalling=D�sinstallation de %1...
