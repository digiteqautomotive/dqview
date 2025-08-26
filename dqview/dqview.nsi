!include "MUI2.nsh"
!include "x64.nsh"
!include "WinVer.nsh"

Unicode true

; The name of the installer
Name "DQ Viewer"
; Program version
!define VERSION "4.3"

; The file to write
!ifndef BUILD
  OutFile "DQView-${VERSION}.exe"
  !define BUILD "0"
!else
  OutFile "DQView-${VERSION}.${BUILD}.exe"
!endif

; Compression method
SetCompressor /SOLID lzma

; Required execution level
RequestExecutionLevel admin

; Don't let the OS scale(blur) the installer GUI
ManifestDPIAware true

; The default installation directory
InstallDir "$PROGRAMFILES64\DQView"

; Installer executable info
VIProductVersion "${VERSION}.${BUILD}.0"
VIAddVersionKey "ProductVersion" ${VERSION}
VIAddVersionKey "FileVersion" "${VERSION}.${BUILD}.0"
VIAddVersionKey "ProductName" "DQ Viewer"
VIAddVersionKey "LegalCopyright" "Digiteq Automotive"
VIAddVersionKey "FileDescription" "DQ Viewer installer"

; Registry key to check for directory (so if you install again, it will
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\DQView" "Install_Dir"

; Registry key for uninstaller
!define REGENTRY "Software\Microsoft\Windows\CurrentVersion\Uninstall\DQView"

; Start menu page configuration
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\DQView"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "DQView"

Var StartMenuFolder

;--------------------------------

; Pages

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "licence.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------

; Languages
!insertmacro MUI_LANGUAGE "English"

Function .onInit
  ${IfNot} ${AtLeastWin7}
    MessageBox MB_OK "DQ Viewer can only be installed on Windows 7 or later."
    Abort
  ${EndIf}
FunctionEnd 

; The stuff to install
Section "DQ Viewer" SEC_APP

  SectionIn RO

  ; Set output path to the installation directory
  SetOutPath $INSTDIR

  ; Put the files there
  File "dqview.exe"
  File /r "platforms"
  File /r "plugins"
  File /r "styles"
  File *.dll
  
  ; Create the uninstaller
  WriteUninstaller "$INSTDIR\uninstall.exe"

  ; Create start menu entry and add links
  SetShellVarContext all
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\DQ Viewer.lnk" "$INSTDIR\dqview.exe"
  !insertmacro MUI_STARTMENU_WRITE_END

  ; Write the installation path into the registry
  DetailPrint "Registering application..."
  WriteRegStr HKLM SOFTWARE\DQView "Install_Dir" "$INSTDIR"

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "${REGENTRY}" "DisplayName" "DQ Viewer"
  WriteRegStr HKLM "${REGENTRY}" "Publisher" "Digiteq Automotive"
  WriteRegStr HKLM "${REGENTRY}" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "${REGENTRY}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegStr HKLM "${REGENTRY}" "QuietUninstallString" "$\"$INSTDIR\uninstall.exe$\" /S"
  WriteRegStr HKLM "${REGENTRY}" "DisplayIcon" '"$INSTDIR\dqview.exe"'
  WriteRegStr HKLM "${REGENTRY}" "InstallLocation" '"$INSTDIR"'
  WriteRegStr HKLM "${REGENTRY}" "URLInfoAbout" "https://github.com/digiteqautomotive/dqview"
  WriteRegDWORD HKLM "${REGENTRY}" "NoModify" 1
  WriteRegDWORD HKLM "${REGENTRY}" "NoRepair" 1

SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"

  ; Remove registry keys
  DeleteRegKey HKLM "${REGENTRY}"
  DeleteRegKey HKLM SOFTWARE\DQView

  ; Remove directories used
  RMDir /r "$INSTDIR"

  ; Remove Start menu entries
  SetShellVarContext all
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
  Delete "$SMPROGRAMS\$StartMenuFolder\*.*"
  RMDir "$SMPROGRAMS\$StartMenuFolder"

SectionEnd
