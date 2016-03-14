;--------------------------------
;Includes

  !include "MUI2.nsh"

;--------------------------------
;General

  ;Name and file
  Name "ArgeeNBeats"
  OutFile "InstallArgeeNBeats.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES64\ArgeeNBeats"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\ArgeeNBeats" ""

  ;Request application privileges for Windows Vista
;  RequestExecutionLevel admin

;--------------------------------
;Variables

  Var StartMenuFolder

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING
  !define MUI_WELCOMEFINISHPAGE_BITMAP "InstallerImageLarge.bmp" 
  !define MUI_HEADERIMAGE 
  !define MUI_HEADERIMAGE_BITMAP "InstallerImageSmall.bmp"
  !define MUI_ICON "../Icons/AnBIcon_128.ico" 

;--------------------------------
;Pages

  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\ArgeeNBeats"
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE "LICENSE.rtf"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
  !insertmacro MUI_PAGE_INSTFILES
  !define MUI_FINISHPAGE_RUN "Run.bat"
  !define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\README.txt"
  !insertmacro MUI_PAGE_FINISH
  
  !insertmacro MUI_UNPAGE_WELCOME
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "ArgeeNBeats" SecArgeeNBeats

  SetShellVarContext all 
  SectionIn RO

  ;Add the files to be extracted (and specify where)
  SetOutPath "$INSTDIR"
  File "..\Client.exe"
  File "..\*.dll"
  File "..\Server.exe"
  File "..\Master.exe"
  File "..\LoggerProcess.exe"
  File "Run.bat"
  File "README.txt"
  File "..\External\x64\*.dll"
  
  SetOutPath "$INSTDIR\External\"
  File "..\External\x64\vc_redist.x64.exe"
  
  SetOutPath "$INSTDIR\ConfigurationFiles\"
  File /r "..\ConfigurationFiles\*.*"
  
  SetOutPath "$INSTDIR\Levels\"
  File /r "..\Levels\*.*"
  
  SetOutPath "$INSTDIR\Models\"
  File /r "..\Models\*.*"

  SetOutPath "$INSTDIR\PotentialFields\"
  File /r "..\PotentialFields\*.*"
  
  SetOutPath "$INSTDIR\ShaderFiles\"
  File /r "..\ShaderFiles\*.*"
  
  SetOutPath "$INSTDIR\Sounds\"
  File /r "..\Sounds\*.*"

  SetOutPath "$INSTDIR\Textures\"
  File /r "..\Textures\*.*"
  

  ;Create uninstaller
  SetOutPath "$INSTDIR"
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
	
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\ArgeeNBeats.lnk" "$INSTDIR\Client.exe"
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\Uninstall ArgeeNBeats.lnk" "$INSTDIR\Uninstall.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_END
  
  ; Install the Visual Studio 14 redistributable.
;  ReadRegDWORD $1 HKCU Software\Microsoft\VisualStudio\14.0_Config\VC\Runtimes\x64 Installed
;  IntCmp $1 1 redist_already_installed
;  ExecWait "$INSTDIR/vcredist_x86.exe"
;  redist_already_installed:
  
  ;Store installation folder
  WriteRegStr HKCU "Software\ArgeeNBeats" "" $INSTDIR

SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecArgeeNBeats ${LANG_ENGLISH} "Installs the complete game."

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecArgeeNBeats} $(DESC_SecArgeeNBeats)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"
  SetShellVarContext all
  SetOutPath "$TEMP"
  
  ; Delete the start menu if existing
  ReadRegStr $1 HKCU "Software\ArgeeNBeats" "Start Menu Folder"
  StrCmp $1 "" no_start_menu
  RMDir /r "$SMPROGRAMS\$StartMenuFolder\$1\"
  no_start_menu:
  
  ; Delete the install folder
  Delete "$INSTDIR\*.*"
  RMDir /r "$INSTDIR"
  
  ; Delete registry entries
  DeleteRegKey /ifempty HKCU "Software\ArgeeNBeats"
SectionEnd