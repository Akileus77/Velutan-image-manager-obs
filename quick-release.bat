@echo off
REM Velutan Image Manager - Quick Release Script
REM This is a simplified version for quick releases

echo ==========================================
echo Velutan Image Manager - Quick Release
echo ==========================================
echo.

REM Check if build exists
if not exist "build\Release\velutan-image-manager.dll" (
    echo ERROR: Build not found!
    echo Please build the project first:
    echo   cd build
    echo   cmake --build . --config Release
    pause
    exit /b 1
)

REM Ask for version
set /p VERSION=Enter version number (e.g., 1.0.0): 

REM Create release directory
set RELEASE_DIR=release\v%VERSION%
if exist "%RELEASE_DIR%" (
    echo Cleaning old release directory...
    rmdir /s /q "%RELEASE_DIR%"
)
mkdir "%RELEASE_DIR%"
mkdir "%RELEASE_DIR%\manual"

echo.
echo Copying files...

REM Copy files to manual package
copy "build\Release\velutan-image-manager.dll" "%RELEASE_DIR%\manual\" >nul
copy "build\Release\Qt6Core.dll" "%RELEASE_DIR%\manual\" >nul
copy "build\Release\Qt6Gui.dll" "%RELEASE_DIR%\manual\" >nul
copy "build\Release\Qt6Widgets.dll" "%RELEASE_DIR%\manual\" >nul
xcopy "data" "%RELEASE_DIR%\manual\data\" /E /I /Y >nul

echo   - Plugin DLL
echo   - Qt dependencies
echo   - Data files

REM Copy documentation
if exist "README.md" copy "README.md" "%RELEASE_DIR%\" >nul
if exist "INSTALL.md" copy "INSTALL.md" "%RELEASE_DIR%\" >nul
if exist "LICENSE" copy "LICENSE" "%RELEASE_DIR%\" >nul

echo   - Documentation

REM Create installation instructions
echo Creating installation instructions...
(
echo VELUTAN IMAGE MANAGER - MANUAL INSTALLATION
echo Version: %VERSION%
echo ==========================================
echo.
echo QUICK INSTALLATION:
echo.
echo 1. Copy velutan-image-manager.dll to:
echo    C:\Program Files\obs-studio\obs-plugins\64bit\
echo.
echo 2. Copy Qt6*.dll files to:
echo    C:\Program Files\obs-studio\obs-plugins\64bit\
echo.
echo 3. Copy data folder to:
echo    C:\Program Files\obs-studio\data\obs-plugins\velutan-image-manager\
echo.
echo 4. Restart OBS Studio
echo.
echo 5. Access via: View -^> Docks -^> Velutan Image Manager
echo.
echo For detailed instructions, see INSTALL.md
) > "%RELEASE_DIR%\manual\INSTALL.txt"

echo.
echo ==========================================
echo MANUAL PACKAGE READY!
echo ==========================================
echo.
echo Location: %RELEASE_DIR%\manual\
echo.
echo Files included:
echo   - velutan-image-manager.dll
echo   - Qt6Core.dll
echo   - Qt6Gui.dll  
echo   - Qt6Widgets.dll
echo   - data\ folder
echo   - INSTALL.txt
echo.
echo Next steps:
echo   1. Create a ZIP of the manual folder
echo   2. Test the installation on a clean system
echo   3. Create a GitHub release
echo.
echo For automatic installer, run: prepare-release.ps1
echo.
pause

