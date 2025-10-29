# Velutan Image Manager - Release Preparation Script
# This script prepares both manual and installer packages for distribution

param(
    [string]$Version = "1.0.0",
    [string]$OBSDir = "C:\Program Files\obs-studio",
    [string]$Qt6Dir = "C:\Qt\6.7.2\msvc2019_64"
)

$ErrorActionPreference = "Stop"

Write-Host "==================================" -ForegroundColor Cyan
Write-Host "Velutan Image Manager Release Prep" -ForegroundColor Cyan
Write-Host "Version: $Version" -ForegroundColor Cyan
Write-Host "==================================" -ForegroundColor Cyan
Write-Host ""

# Check if build exists
if (-not (Test-Path "build\Release\velutan-image-manager.dll")) {
    Write-Host "ERROR: Build not found! Please build the project first:" -ForegroundColor Red
    Write-Host "  cd build" -ForegroundColor Yellow
    Write-Host "  cmake --build . --config Release" -ForegroundColor Yellow
    exit 1
}

# Create release directory
$releaseDir = "release\v$Version"
if (Test-Path $releaseDir) {
    Write-Host "Cleaning old release directory..." -ForegroundColor Yellow
    Remove-Item $releaseDir -Recurse -Force
}
New-Item -ItemType Directory -Path $releaseDir | Out-Null

Write-Host "✓ Created release directory: $releaseDir" -ForegroundColor Green

# ==========================================
# Prepare Manual Installation Package
# ==========================================
Write-Host ""
Write-Host "Preparing manual installation package..." -ForegroundColor Cyan

$manualDir = "$releaseDir\manual"
New-Item -ItemType Directory -Path $manualDir | Out-Null

# Copy plugin DLL
Copy-Item "build\Release\velutan-image-manager.dll" "$manualDir\" -Force
Write-Host "  ✓ Copied plugin DLL" -ForegroundColor Green

# Copy Qt dependencies
$qtDlls = @("Qt6Core.dll", "Qt6Gui.dll", "Qt6Widgets.dll")
foreach ($dll in $qtDlls) {
    $source = "build\Release\$dll"
    if (Test-Path $source) {
        Copy-Item $source "$manualDir\" -Force
        Write-Host "  ✓ Copied $dll" -ForegroundColor Green
    } else {
        # Try Qt directory
        $qtSource = "$Qt6Dir\bin\$dll"
        if (Test-Path $qtSource) {
            Copy-Item $qtSource "$manualDir\" -Force
            Write-Host "  ✓ Copied $dll from Qt directory" -ForegroundColor Green
        } else {
            Write-Host "  ⚠ WARNING: $dll not found!" -ForegroundColor Yellow
        }
    }
}

# Copy data directory
if (Test-Path "data") {
    Copy-Item "data" "$manualDir\" -Recurse -Force
    Write-Host "  ✓ Copied data directory" -ForegroundColor Green
} else {
    Write-Host "  ⚠ WARNING: data directory not found!" -ForegroundColor Yellow
}

# Create installation instructions
$installInstructions = @"
VELUTAN IMAGE MANAGER - MANUAL INSTALLATION
Version: $Version
==========================================

INSTALLATION STEPS:
-------------------

1. COPY PLUGIN DLL:
   Copy: velutan-image-manager.dll
   To:   [OBS Directory]\obs-plugins\64bit\
   
   Default: C:\Program Files\obs-studio\obs-plugins\64bit\

2. COPY QT DEPENDENCIES:
   Copy: Qt6Core.dll, Qt6Gui.dll, Qt6Widgets.dll
   To:   [OBS Directory]\obs-plugins\64bit\
   
3. COPY DATA FOLDER:
   Copy: data\ (entire folder)
   To:   [OBS Directory]\data\obs-plugins\velutan-image-manager\
   
   Final path: C:\Program Files\obs-studio\data\obs-plugins\velutan-image-manager\

4. RESTART OBS STUDIO

5. ACCESS PLUGIN:
   View → Docks → Velutan Image Manager

NOTES:
------
- You may need administrator privileges
- Ensure OBS Studio is version 30.0+ (Qt6)
- See INSTALL.md for detailed instructions

For support, visit: https://github.com/Akileus77/Velutan-image-manager-obs/issues
"@

Set-Content -Path "$manualDir\INSTALL.txt" -Value $installInstructions
Write-Host "  ✓ Created installation instructions" -ForegroundColor Green

# Create ZIP archive for manual installation
$manualZip = "$releaseDir\Velutan-Image-Manager-v$Version-Manual.zip"
Compress-Archive -Path "$manualDir\*" -DestinationPath $manualZip -Force
Write-Host "✓ Created manual installation package: $manualZip" -ForegroundColor Green

# ==========================================
# Prepare Installer Package
# ==========================================
Write-Host ""
Write-Host "Preparing installer package..." -ForegroundColor Cyan

# Check if NSIS is installed
$nsisPath = "C:\Program Files (x86)\NSIS\makensis.exe"
if (-not (Test-Path $nsisPath)) {
    $nsisPath = "C:\Program Files\NSIS\makensis.exe"
}

if (Test-Path $nsisPath) {
    Write-Host "  Found NSIS at: $nsisPath" -ForegroundColor Green
    
    # Update version in installer script
    $nsiContent = Get-Content "installer.nsi" -Raw
    $nsiContent = $nsiContent -replace '!define PRODUCT_VERSION ".*"', "!define PRODUCT_VERSION `"$Version`""
    Set-Content -Path "installer.nsi" -Value $nsiContent
    
    # Build installer
    Write-Host "  Building installer..." -ForegroundColor Cyan
    & $nsisPath installer.nsi
    
    if ($LASTEXITCODE -eq 0) {
        # Move installer to release directory
        if (Test-Path "Velutan-Image-Manager-Installer.exe") {
            Move-Item "Velutan-Image-Manager-Installer.exe" "$releaseDir\Velutan-Image-Manager-v$Version-Installer.exe" -Force
            Write-Host "✓ Created installer: Velutan-Image-Manager-v$Version-Installer.exe" -ForegroundColor Green
        }
    } else {
        Write-Host "  ⚠ WARNING: Installer build failed!" -ForegroundColor Yellow
    }
} else {
    Write-Host "  ⚠ NSIS not found. Skipping installer creation." -ForegroundColor Yellow
    Write-Host "    Download NSIS from: https://nsis.sourceforge.io/Download" -ForegroundColor Yellow
}

# ==========================================
# Copy Documentation
# ==========================================
Write-Host ""
Write-Host "Copying documentation..." -ForegroundColor Cyan

$docs = @("README.md", "INSTALL.md", "LICENSE")
foreach ($doc in $docs) {
    if (Test-Path $doc) {
        Copy-Item $doc "$releaseDir\" -Force
        Write-Host "  ✓ Copied $doc" -ForegroundColor Green
    }
}

# ==========================================
# Generate Release Notes
# ==========================================
Write-Host ""
Write-Host "Generating release notes..." -ForegroundColor Cyan

$releaseNotes = @"
# Velutan Image Manager v$Version

## 📦 Release Files

- **Velutan-Image-Manager-v$Version-Installer.exe** - Automatic installer (recommended)
- **Velutan-Image-Manager-v$Version-Manual.zip** - Manual installation package

## 🆕 What's New

### Features
- Scene-specific background management
- Per-scene background tracking and persistence
- Active assets automatically move to top of list
- Modern dark theme UI with visual indicators
- Theme-based background organization
- Separate tag filtering for backgrounds and characters
- Asset editing (name, tags, theme)
- Asset deletion with confirmation
- Thumbnail previews (80x80px)
- Auto-stretch backgrounds to canvas
- Dynamic updates from Tools menu
- Searchable theme and tag dropdowns

### Improvements
- Improved UI readability and color scheme
- Green highlighting for active assets
- Scene-specific character management
- Better error handling and crash prevention
- Delayed initialization for stability

## 📋 Requirements

- OBS Studio 30.0+ (Qt6 version)
- Windows 10/11 (64-bit)

## 🚀 Installation

### Automatic (Recommended)
1. Download \`Velutan-Image-Manager-v$Version-Installer.exe\`
2. Run the installer as administrator
3. Follow the installation wizard
4. Restart OBS Studio

### Manual
1. Download \`Velutan-Image-Manager-v$Version-Manual.zip\`
2. Extract the archive
3. Follow instructions in \`INSTALL.txt\` or see \`INSTALL.md\`
4. Restart OBS Studio

## 📖 Documentation

- [README.md](README.md) - Full documentation
- [INSTALL.md](INSTALL.md) - Detailed installation guide

## 🐛 Known Issues

None at this time.

## 🙏 Credits

Built with ❤️ for the OBS Studio community.

---

**Full Changelog**: https://github.com/Akileus77/Velutan-image-manager-obs/commits/v$Version
"@

Set-Content -Path "$releaseDir\RELEASE_NOTES.md" -Value $releaseNotes
Write-Host "✓ Created release notes" -ForegroundColor Green

# ==========================================
# Generate Checksums
# ==========================================
Write-Host ""
Write-Host "Generating checksums..." -ForegroundColor Cyan

$checksums = @()
Get-ChildItem "$releaseDir" -File -Recurse | ForEach-Object {
    if ($_.Extension -in @('.exe', '.zip')) {
        $hash = (Get-FileHash $_.FullName -Algorithm SHA256).Hash
        $relativePath = $_.FullName.Substring($releaseDir.Length + 1)
        $checksums += "$hash  $relativePath"
        Write-Host "  ✓ $relativePath" -ForegroundColor Green
    }
}

Set-Content -Path "$releaseDir\SHA256SUMS.txt" -Value ($checksums -join "`n")
Write-Host "✓ Created checksums file" -ForegroundColor Green

# ==========================================
# Summary
# ==========================================
Write-Host ""
Write-Host "==================================" -ForegroundColor Cyan
Write-Host "RELEASE PREPARATION COMPLETE!" -ForegroundColor Green
Write-Host "==================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Release directory: $releaseDir" -ForegroundColor White
Write-Host ""
Write-Host "Files created:" -ForegroundColor White
Get-ChildItem "$releaseDir" -File | ForEach-Object {
    $size = [math]::Round($_.Length / 1MB, 2)
    Write-Host "  • $($_.Name) ($size MB)" -ForegroundColor Cyan
}
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "  1. Test the installer on a clean system" -ForegroundColor White
Write-Host "  2. Test the manual installation package" -ForegroundColor White
Write-Host "  3. Create a GitHub release with these files" -ForegroundColor White
Write-Host "  4. Copy RELEASE_NOTES.md content to the GitHub release" -ForegroundColor White
Write-Host "  5. Upload the files and SHA256SUMS.txt" -ForegroundColor White
Write-Host ""
Write-Host "Happy releasing! 🚀" -ForegroundColor Green

