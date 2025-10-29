@echo off
REM Velutan Image Manager - Installer Builder
echo ==========================================
echo Velutan Image Manager - Installer Olusturucu
echo ==========================================
echo.

REM Check if NSIS is installed
set NSIS_PATH=C:\Program Files (x86)\NSIS\makensis.exe
set NSIS_PATH_ALT=C:\Program Files\NSIS\makensis.exe

if exist "%NSIS_PATH%" (
    echo NSIS bulundu: %NSIS_PATH%
    goto :build
)

if exist "%NSIS_PATH_ALT%" (
    echo NSIS bulundu: %NSIS_PATH_ALT%
    set NSIS_PATH=%NSIS_PATH_ALT%
    goto :build
)

echo.
echo HATA: NSIS bulunamadi!
echo.
echo Lutfen NSIS'i asagidaki linkten indirip kurun:
echo https://nsis.sourceforge.io/Download
echo.
echo Kurulum sonrasi bu script'i tekrar calistirin.
echo.
pause
exit /b 1

:build
echo.
echo Installer olusturuluyor...
echo.

"%NSIS_PATH%" installer.nsi

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ==========================================
    echo BASARILI!
    echo ==========================================
    echo.
    echo Installer olusturuldu:
    echo   Velutan-Image-Manager-Installer.exe
    echo.
    echo Bu dosyayi release klasorune tasiyabilirsiniz:
    move /Y "Velutan-Image-Manager-Installer.exe" "release\Velutan-Image-Manager-v1.0.0-Installer.exe"
    echo.
    echo Dosya: release\Velutan-Image-Manager-v1.0.0-Installer.exe
    echo.
) else (
    echo.
    echo HATA: Installer olusturulamadi!
    echo.
)

pause

