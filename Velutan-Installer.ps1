# Velutan Image Manager - PowerShell Installer
# Version 1.0.0

# Require Administrator
if (-NOT ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")) {
    Write-Host "Bu installer yonetici yetkisi gerektirir!" -ForegroundColor Red
    Write-Host "Lutfen sag tikla -> 'Yonetici olarak calistir' seciniz." -ForegroundColor Yellow
    pause
    exit 1
}

# Banner
Clear-Host
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Velutan Image Manager v1.0.0" -ForegroundColor Cyan
Write-Host "  OBS Studio Plugin Installer" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Find OBS installation
Write-Host "OBS Studio aranıyor..." -ForegroundColor Yellow

$obsLocations = @(
    "C:\Program Files\obs-studio",
    "C:\Program Files (x86)\obs-studio",
    "C:\Program Files (x86)\Steam\steamapps\common\OBS Studio"
)

$obsPath = $null
foreach ($loc in $obsLocations) {
    if (Test-Path "$loc\bin\64bit\obs64.exe") {
        $obsPath = $loc
        break
    }
}

if (-not $obsPath) {
    Write-Host ""
    Write-Host "HATA: OBS Studio bulunamadi!" -ForegroundColor Red
    Write-Host ""
    Write-Host "OBS Studio kurulu degilse, lutfen once OBS Studio'yu kurun:" -ForegroundColor Yellow
    Write-Host "https://obsproject.com/" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "OBS farkli bir yerdeyse, manuel kurulum talimatlarini okuyun." -ForegroundColor Yellow
    pause
    exit 1
}

Write-Host "  Bulundu: $obsPath" -ForegroundColor Green
Write-Host ""

# Check if OBS is running
Write-Host "OBS Studio kontrolu..." -ForegroundColor Yellow
$obsProcess = Get-Process obs64 -ErrorAction SilentlyContinue
if ($obsProcess) {
    Write-Host ""
    Write-Host "UYARI: OBS Studio calisiyor!" -ForegroundColor Yellow
    Write-Host "Lutfen OBS'yi kapatip devam edin." -ForegroundColor Yellow
    Write-Host ""
    $response = Read-Host "OBS'yi kapattiniz mi? (E/H)"
    if ($response -ne "E" -and $response -ne "e") {
        Write-Host "Kurulum iptal edildi." -ForegroundColor Red
        pause
        exit 0
    }
}

# Get script directory
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

# Check if files exist
Write-Host "Dosyalar kontrol ediliyor..." -ForegroundColor Yellow

$requiredFiles = @(
    "velutan-image-manager.dll",
    "Qt6Core.dll",
    "Qt6Gui.dll",
    "Qt6Widgets.dll"
)

$missingFiles = @()
foreach ($file in $requiredFiles) {
    if (-not (Test-Path "$scriptDir\$file")) {
        $missingFiles += $file
    }
}

if ($missingFiles.Count -gt 0) {
    Write-Host ""
    Write-Host "HATA: Gerekli dosyalar bulunamadi:" -ForegroundColor Red
    foreach ($file in $missingFiles) {
        Write-Host "  - $file" -ForegroundColor Red
    }
    Write-Host ""
    Write-Host "Lutfen tum dosyalari ayni klasore cikarttiginizdan emin olun." -ForegroundColor Yellow
    pause
    exit 1
}

Write-Host "  Tum dosyalar mevcut" -ForegroundColor Green
Write-Host ""

# Confirm installation
Write-Host "Kurulum Bilgileri:" -ForegroundColor Cyan
Write-Host "  OBS Konumu: $obsPath" -ForegroundColor White
Write-Host "  Plugin: velutan-image-manager.dll" -ForegroundColor White
Write-Host "  Qt Kutuphaneleri: 3 dosya" -ForegroundColor White
Write-Host "  Data Klasoru: velutan-image-manager" -ForegroundColor White
Write-Host ""

$response = Read-Host "Kuruluma devam edilsin mi? (E/H)"
if ($response -ne "E" -and $response -ne "e") {
    Write-Host "Kurulum iptal edildi." -ForegroundColor Yellow
    pause
    exit 0
}

Write-Host ""
Write-Host "Kurulum basliyor..." -ForegroundColor Cyan
Write-Host ""

# Copy plugin DLL
Write-Host "[1/3] Plugin DLL kopyalanıyor..." -ForegroundColor Yellow
$pluginDest = "$obsPath\obs-plugins\64bit"
try {
    Copy-Item "$scriptDir\velutan-image-manager.dll" "$pluginDest\" -Force
    Write-Host "      OK - Plugin DLL kopyalandı" -ForegroundColor Green
} catch {
    Write-Host "      HATA: $_" -ForegroundColor Red
    pause
    exit 1
}

# Copy Qt DLLs
Write-Host "[2/3] Qt kutuphaneleri kopyalanıyor..." -ForegroundColor Yellow
$qtFiles = @("Qt6Core.dll", "Qt6Gui.dll", "Qt6Widgets.dll")
foreach ($qtFile in $qtFiles) {
    try {
        if (Test-Path "$scriptDir\$qtFile") {
            Copy-Item "$scriptDir\$qtFile" "$pluginDest\" -Force
            Write-Host "      OK - $qtFile" -ForegroundColor Green
        }
    } catch {
        Write-Host "      UYARI: $qtFile kopyalanamadı - $_" -ForegroundColor Yellow
    }
}

# Copy data folder
Write-Host "[3/3] Data klasoru kopyalanıyor..." -ForegroundColor Yellow
$dataDest = "$obsPath\data\obs-plugins\velutan-image-manager"
try {
    if (Test-Path "$scriptDir\data") {
        if (-not (Test-Path $dataDest)) {
            New-Item -ItemType Directory -Path $dataDest -Force | Out-Null
        }
        Copy-Item "$scriptDir\data\*" "$dataDest\" -Recurse -Force
        Write-Host "      OK - Data klasoru kopyalandı" -ForegroundColor Green
    } else {
        Write-Host "      UYARI: Data klasoru bulunamadı" -ForegroundColor Yellow
    }
} catch {
    Write-Host "      HATA: $_" -ForegroundColor Red
}

# Success
Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "  KURULUM TAMAMLANDI!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "Sonraki Adımlar:" -ForegroundColor Cyan
Write-Host "  1. OBS Studio'yu baslatin" -ForegroundColor White
Write-Host "  2. View -> Docks -> Velutan Image Manager" -ForegroundColor White
Write-Host "  3. Tools -> Velutan Image Manager (Setup)" -ForegroundColor White
Write-Host "     ile asset ekleyin" -ForegroundColor White
Write-Host ""
Write-Host "Iyi yayinlar! " -ForegroundColor Green -NoNewline
Write-Host "🎬" -ForegroundColor Cyan
Write-Host ""

# Open OBS option
$response = Read-Host "OBS Studio'yu simdi acmak ister misiniz? (E/H)"
if ($response -eq "E" -or $response -eq "e") {
    if (Test-Path "$obsPath\bin\64bit\obs64.exe") {
        Start-Process "$obsPath\bin\64bit\obs64.exe"
        Write-Host "OBS Studio baslatiliyor..." -ForegroundColor Green
    }
}

Write-Host ""
pause

