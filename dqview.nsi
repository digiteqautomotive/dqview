!include "MUI2.nsh"
!include "x64.nsh"
!include "WinVer.nsh"


; The name of the installer
Name "DQ Viewer"
; Program version
!define VERSION "2.3"

; The file to write
OutFile "DQView-${VERSION}.exe"
; Compression method
SetCompressor /SOLID lzma

; Required execution level
RequestExecutionLevel user

; Don't let the OS scale(blur) the installer GUI
ManifestDPIAware true

; The default installation directory
InstallDir "$LOCALAPPDATA\Programs\DQView"

; Installer executable info
VIProductVersion "${VERSION}.0.0"
VIAddVersionKey "ProductVersion" ${VERSION}
VIAddVersionKey "FileVersion" "${VERSION}.0.0"
VIAddVersionKey "ProductName" "DQ Viewer"
VIAddVersionKey "LegalCopyright" "Digiteq Automotive"
VIAddVersionKey "FileDescription" "DQ Viewer installer"

; Registry key to check for directory (so if you install again, it will
; overwrite the old one automatically)
InstallDirRegKey HKCU "Software\DQView" "Install_Dir"

; Registry key for uninstaller
!define REGENTRY "Software\Microsoft\Windows\CurrentVersion\Uninstall\DQView"

; Start menu page configuration
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU"
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
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\DQ Viewer.lnk" "$INSTDIR\dqview.exe"
  !insertmacro MUI_STARTMENU_WRITE_END

  ; Write the installation path into the registry
  DetailPrint "Registering application..."
  WriteRegStr HKCU SOFTWARE\DQView "Install_Dir" "$INSTDIR"

  ; Write the uninstall keys for Windows
  WriteRegStr HKCU "${REGENTRY}" "DisplayName" "DQ Viewer"
  WriteRegStr HKCU "${REGENTRY}" "Publisher" "Digiteq Automotive"
  WriteRegStr HKCU "${REGENTRY}" "DisplayVersion" "${VERSION}"
  WriteRegStr HKCU "${REGENTRY}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegStr HKCU "${REGENTRY}" "DisplayIcon" '"$INSTDIR\dqview.exe"'
  WriteRegStr HKCU "${REGENTRY}" "InstallLocation" '"$INSTDIR"'
  WriteRegStr HKCU "${REGENTRY}" "URLInfoAbout" "https://www.digiteqautomotive.com"
  WriteRegDWORD HKCU "${REGENTRY}" "NoModify" 1
  WriteRegDWORD HKCU "${REGENTRY}" "NoRepair" 1

SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"

  ; Remove registry keys
  DeleteRegKey HKCU "${REGENTRY}"
  DeleteRegKey HKCU SOFTWARE\DQView

  ; Remove directories used
  RMDir /r "$INSTDIR"

  ; Remove Start menu entries
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
  Delete "$SMPROGRAMS\$StartMenuFolder\*.*"
  RMDir "$SMPROGRAMS\$StartMenuFolder"

SectionEnd
