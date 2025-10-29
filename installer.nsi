; Velutan Image Manager for OBS Studio - NSIS Installer Script
; Requires NSIS 3.0 or later

;--------------------------------
; Includes

!include "MUI2.nsh"
!include "FileFunc.nsh"
!include "x64.nsh"

;--------------------------------
; General

!define PRODUCT_NAME "Velutan Image Manager"
!define PRODUCT_VERSION "1.1.0"
!define PRODUCT_PUBLISHER "Velutan"
!define PRODUCT_WEB_SITE "https://github.com/Akileus77/Velutan-image-manager-obs"

; The name of the installer
Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"

; The file to write
OutFile "Velutan-Image-Manager-Installer.exe"

; Request application privileges for Windows Vista and higher
RequestExecutionLevel admin

; Default installation folder
InstallDir "$PROGRAMFILES\obs-studio"

; Get installation folder from registry if available
InstallDirRegKey HKLM "Software\OBS Studio" "InstallPath"

; Modern UI Configuration
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Finish page configuration
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_TEXT "Launch OBS Studio"
!define MUI_FINISHPAGE_RUN_FUNCTION "LaunchOBS"

;--------------------------------
; Pages

!insertmacro MUI_PAGE_LICENSE "LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
; Languages

!insertmacro MUI_LANGUAGE "English"

;--------------------------------
; Version Information

VIProductVersion "${PRODUCT_VERSION}.0"
VIAddVersionKey "ProductName" "${PRODUCT_NAME}"
VIAddVersionKey "CompanyName" "${PRODUCT_PUBLISHER}"
VIAddVersionKey "LegalCopyright" "© ${PRODUCT_PUBLISHER}"
VIAddVersionKey "FileDescription" "${PRODUCT_NAME} for OBS Studio"
VIAddVersionKey "FileVersion" "${PRODUCT_VERSION}"
VIAddVersionKey "ProductVersion" "${PRODUCT_VERSION}"

;--------------------------------
; Installer Sections

Section "MainSection" SEC01
    ; Check if OBS is running
    Call CheckOBSRunning
    
    ; Set output path to plugin directory
    SetOutPath "$INSTDIR\obs-plugins\64bit"
    
    ; Copy plugin DLL
    File "build\Release\velutan-image-manager.dll"
    
    ; Copy Qt dependencies (optional - OBS usually has these)
    ; If these fail, it's okay - OBS has its own Qt DLLs
    File /nonfatal "build\Release\Qt6Core.dll"
    File /nonfatal "build\Release\Qt6Gui.dll"
    File /nonfatal "build\Release\Qt6Widgets.dll"
    
    ; Set output path to data directory
    SetOutPath "$INSTDIR\data\obs-plugins\velutan-image-manager"
    
    ; Copy data files
    File /r "data\*.*"
    
    ; Write the installation path into the registry
    WriteRegStr HKLM "SOFTWARE\${PRODUCT_NAME}" "InstallPath" "$INSTDIR"
    WriteRegStr HKLM "SOFTWARE\${PRODUCT_NAME}" "Version" "${PRODUCT_VERSION}"
    
    ; Write the uninstall keys for Windows
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "DisplayName" "${PRODUCT_NAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "UninstallString" '"$INSTDIR\Uninstall-Velutan.exe"'
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "DisplayIcon" "$INSTDIR\obs-plugins\64bit\velutan-image-manager.dll"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "Publisher" "${PRODUCT_PUBLISHER}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "DisplayVersion" "${PRODUCT_VERSION}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "NoModify" 1
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "NoRepair" 1
    
    ; Create uninstaller
    WriteUninstaller "$INSTDIR\Uninstall-Velutan.exe"
    
    ; Show completion message
    DetailPrint "Installation complete!"
    DetailPrint "Please restart OBS Studio to use the plugin."
    DetailPrint "Access it via View → Docks → Velutan Image Manager"
SectionEnd

;--------------------------------
; Uninstaller Section

Section "Uninstall"
    ; Check if OBS is running
    Call un.CheckOBSRunning
    
    ; Remove plugin files
    Delete "$INSTDIR\obs-plugins\64bit\velutan-image-manager.dll"
    
    ; Remove Qt dependencies (only if not used by other plugins)
    ; Note: We don't delete Qt DLLs as they might be used by other plugins
    ; Users can manually delete them if needed
    
    ; Remove data directory
    RMDir /r "$INSTDIR\data\obs-plugins\velutan-image-manager"
    
    ; Remove registry keys
    DeleteRegKey HKLM "SOFTWARE\${PRODUCT_NAME}"
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
    
    ; Remove uninstaller
    Delete "$INSTDIR\Uninstall-Velutan.exe"
    
    ; Show completion message
    DetailPrint "Uninstallation complete!"
    DetailPrint "Your settings and asset library have been preserved."
    DetailPrint "They are located in: %APPDATA%\obs-studio\velutan-image-manager\"
SectionEnd

;--------------------------------
; Functions

; Check if OBS is running
Function CheckOBSRunning
    FindWindow $0 "Qt624QWindowIcon" ""
    StrCmp $0 0 notRunning
        MessageBox MB_OK|MB_ICONEXCLAMATION "OBS Studio is currently running.$\n$\nPlease close OBS Studio and try again."
        Abort
    notRunning:
FunctionEnd

Function un.CheckOBSRunning
    FindWindow $0 "Qt624QWindowIcon" ""
    StrCmp $0 0 notRunning
        MessageBox MB_OK|MB_ICONEXCLAMATION "OBS Studio is currently running.$\n$\nPlease close OBS Studio and try again."
        Abort
    notRunning:
FunctionEnd

; Launch OBS after installation
Function LaunchOBS
    ${If} ${FileExists} "$INSTDIR\bin\64bit\obs64.exe"
        Exec '"$INSTDIR\bin\64bit\obs64.exe"'
    ${EndIf}
FunctionEnd

;--------------------------------
; Initialization Functions

Function .onInit
    ; Check if running on 64-bit Windows
    ${IfNot} ${RunningX64}
        MessageBox MB_OK|MB_ICONSTOP "This plugin requires 64-bit Windows."
        Abort
    ${EndIf}
    
    ; Check if OBS Studio is installed
    ${If} ${FileExists} "$INSTDIR\bin\64bit\obs64.exe"
        ; OBS found, continue
    ${Else}
        ; Try to find OBS in registry
        ReadRegStr $0 HKLM "Software\OBS Studio" "InstallPath"
        ${If} ${FileExists} "$0\bin\64bit\obs64.exe"
            StrCpy $INSTDIR $0
        ${Else}
            ; OBS not found
            MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
                "OBS Studio installation not detected.$\n$\nPlease select your OBS Studio installation directory.$\n$\nClick OK to continue or Cancel to abort." \
                IDOK continue
                Abort
            continue:
        ${EndIf}
    ${EndIf}
FunctionEnd

Function un.onInit
    MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 \
        "Are you sure you want to uninstall ${PRODUCT_NAME}?$\n$\nYour settings and asset library will be preserved." \
        IDYES +2
    Abort
FunctionEnd

