# Installation Guide - Velutan Image Manager

## 🎯 Quick Start

Choose your preferred installation method:
- **[Automatic Installer](#automatic-installation)** - Easiest, recommended for most users
- **[Manual Installation](#manual-installation)** - For advanced users or custom OBS locations

---

## 🚀 Automatic Installation

### Prerequisites
- OBS Studio 30.0+ installed (Qt6 version)
- Windows 10/11 (64-bit)
- Administrator privileges

### Steps

1. **Download the Installer**
   - Go to [Releases](https://github.com/Akileus77/Velutan-image-manager-obs/releases)
   - Download `Velutan-Image-Manager-Installer.exe`

2. **Run the Installer**
   - Right-click the installer → `Run as administrator`
   - Follow the installation wizard:
     - Accept the license agreement
     - Choose OBS installation directory (auto-detected)
     - Click `Install`

3. **Restart OBS Studio**
   - Close OBS completely if it's running
   - Start OBS Studio

4. **Access the Plugin**
   - In OBS, go to `View → Docks → Velutan Image Manager`
   - The plugin panel will appear (you can dock it anywhere)

5. **Start Using**
   - Click `Tools → Velutan Image Manager (Setup)` to add your first assets
   - Or click the `➕ Add` button in the plugin panel

---

## 🔧 Manual Installation

### Prerequisites
- OBS Studio 30.0+ installed (Qt6 version)
- Windows 10/11 (64-bit)
- Administrator privileges (to copy files to Program Files)

### Download Files

1. Go to [Releases](https://github.com/Akileus77/Velutan-image-manager-obs/releases)
2. Download `Velutan-Image-Manager-Manual.zip`
3. Extract the ZIP file to a temporary location

### Installation Steps

#### Step 1: Locate Your OBS Installation

**Default location:**
```
C:\Program Files\obs-studio\
```

**Steam version:**
```
C:\Program Files (x86)\Steam\steamapps\common\OBS Studio\
```

**To find your OBS location:**
1. Right-click OBS Studio shortcut → `Properties`
2. Look at the `Target` field
3. Note the directory path

#### Step 2: Copy Plugin Files

You need to copy files to **three locations**:

##### A. Plugin DLL
Copy the main plugin file:
```
FROM: velutan-image-manager.dll
TO:   [OBS Directory]\obs-plugins\64bit\velutan-image-manager.dll
```

**Example:**
```
C:\Program Files\obs-studio\obs-plugins\64bit\velutan-image-manager.dll
```

##### B. Qt Dependencies
Copy ALL Qt DLL files:
```
FROM: Qt6Core.dll, Qt6Gui.dll, Qt6Widgets.dll
TO:   [OBS Directory]\obs-plugins\64bit\
```

**Files to copy:**
- `Qt6Core.dll`
- `Qt6Gui.dll`
- `Qt6Widgets.dll`

**Example:**
```
C:\Program Files\obs-studio\obs-plugins\64bit\Qt6Core.dll
C:\Program Files\obs-studio\obs-plugins\64bit\Qt6Gui.dll
C:\Program Files\obs-studio\obs-plugins\64bit\Qt6Widgets.dll
```

##### C. Data Files
Copy the entire `data` folder:
```
FROM: data\ (entire folder)
TO:   [OBS Directory]\data\obs-plugins\velutan-image-manager\
```

**Example:**
```
C:\Program Files\obs-studio\data\obs-plugins\velutan-image-manager\
```

The final structure should look like:
```
C:\Program Files\obs-studio\data\obs-plugins\velutan-image-manager\
├── velutan_library.json
├── locale\
│   └── en-US.ini
└── (any other data files)
```

#### Step 3: Set Permissions

If you get "Access Denied" errors:

1. Open Command Prompt as Administrator
2. Run these commands:

```cmd
cd "C:\Program Files\obs-studio\obs-plugins\64bit"
icacls velutan-image-manager.dll /grant Users:RX
icacls Qt6*.dll /grant Users:RX

cd "C:\Program Files\obs-studio\data\obs-plugins"
icacls velutan-image-manager /grant Users:(OI)(CI)RX /T
```

#### Step 4: Restart OBS Studio

1. Close OBS Studio completely
2. Start OBS Studio
3. Check for the plugin:
   - Go to `View → Docks`
   - Look for `Velutan Image Manager`

---

## ✅ Verification

### Check Plugin is Loaded

1. Open OBS Studio
2. Go to `Help → Log Files → View Current Log`
3. Search for "Velutan" in the log
4. You should see:
   ```
   [Velutan] Loading Velutan Image Manager plugin
   [Velutan] Plugin loaded successfully
   ```

### Check Plugin Panel

1. In OBS, go to `View → Docks`
2. You should see `Velutan Image Manager` in the list
3. Click it to show/hide the panel

### Check Tools Menu

1. Go to `Tools` in OBS menu bar
2. You should see `Velutan Image Manager (Setup)`

---

## 🐛 Troubleshooting

### Plugin Doesn't Appear

**Symptom:** No "Velutan Image Manager" in View → Docks

**Solutions:**
1. **Check OBS Version:**
   - Help → About → Version should be 30.0+
   - Must be Qt6 version (not Qt5)

2. **Check DLL Location:**
   ```cmd
   dir "C:\Program Files\obs-studio\obs-plugins\64bit\velutan-image-manager.dll"
   ```
   Should show the file. If not, copy it again.

3. **Check Dependencies:**
   ```cmd
   dir "C:\Program Files\obs-studio\obs-plugins\64bit\Qt6*.dll"
   ```
   Should show all Qt6 DLLs. If missing, copy them.

4. **Check OBS Logs:**
   - `Help → Log Files → View Current Log`
   - Look for error messages containing "velutan"

### Qt DLL Errors

**Symptom:** Error about missing Qt6Core.dll, Qt6Gui.dll, or Qt6Widgets.dll

**Solution:**
1. Ensure ALL Qt DLLs are copied:
   - `Qt6Core.dll`
   - `Qt6Gui.dll`
   - `Qt6Widgets.dll`

2. Verify they're in the correct location:
   ```
   C:\Program Files\obs-studio\obs-plugins\64bit\
   ```

3. Try re-downloading and re-copying the files

### Access Denied Errors

**Symptom:** Cannot copy files to Program Files

**Solution:**
1. **Option A:** Use Administrator Command Prompt
   ```cmd
   copy /Y "path\to\velutan-image-manager.dll" "C:\Program Files\obs-studio\obs-plugins\64bit\"
   ```

2. **Option B:** Use PowerShell as Administrator
   ```powershell
   Copy-Item "path\to\velutan-image-manager.dll" "C:\Program Files\obs-studio\obs-plugins\64bit\" -Force
   ```

3. **Option C:** Change folder ownership
   - Right-click `obs-plugins` folder → Properties → Security
   - Add your user account with Full Control

### Data Files Not Found

**Symptom:** Plugin loads but shows errors about missing library file

**Solution:**
1. Check data folder exists:
   ```
   C:\Program Files\obs-studio\data\obs-plugins\velutan-image-manager\
   ```

2. Ensure it contains:
   - `velutan_library.json`
   - `locale` folder with `en-US.ini`

3. If missing, copy the entire `data` folder from the download

### Plugin Crashes OBS

**Symptom:** OBS crashes when plugin loads

**Possible Causes:**
1. **Qt Version Mismatch**
   - Plugin requires OBS built with Qt6
   - Check OBS version: Help → About
   - Reinstall latest OBS if needed

2. **Corrupted DLL Files**
   - Re-download the plugin
   - Verify file sizes match the release

3. **Conflicting Plugins**
   - Temporarily disable other plugins
   - Restart OBS and test

---

## 🔄 Updating the Plugin

### Automatic (If using installer)
1. Download the new installer
2. Run it (it will detect and upgrade the existing installation)
3. Restart OBS Studio

### Manual
1. Close OBS Studio
2. Download new version
3. Replace old files with new ones (same locations as installation)
4. Restart OBS Studio

**Note:** Your settings and asset library are preserved during updates!

---

## 🗑️ Uninstalling

### Automatic (If installed via installer)
1. Go to Windows Settings → Apps → Apps & features
2. Find "Velutan Image Manager for OBS Studio"
3. Click Uninstall

### Manual
1. Close OBS Studio
2. Delete these files:
   ```
   C:\Program Files\obs-studio\obs-plugins\64bit\velutan-image-manager.dll
   C:\Program Files\obs-studio\obs-plugins\64bit\Qt6Core.dll
   C:\Program Files\obs-studio\obs-plugins\64bit\Qt6Gui.dll
   C:\Program Files\obs-studio\obs-plugins\64bit\Qt6Widgets.dll
   ```

3. Delete data folder:
   ```
   C:\Program Files\obs-studio\data\obs-plugins\velutan-image-manager\
   ```

4. (Optional) Delete user settings:
   ```
   %APPDATA%\obs-studio\velutan-image-manager\
   ```

---

## 💡 Tips

### Best Practices
1. **Backup Your Library:**
   - Your asset library is at: `%APPDATA%\obs-studio\velutan-image-manager\library.json`
   - Copy this file to backup your assets before major updates

2. **Organize with Tags:**
   - Use consistent tag naming (e.g., "outdoor", "indoor", "night")
   - Makes filtering much easier

3. **Use Themes:**
   - Categorize backgrounds by theme for quick access
   - Themes are searchable and filterable

4. **Scene Naming:**
   - Use clear scene names
   - Each scene can have its own background

### Performance Tips
1. **Image Optimization:**
   - Use compressed PNG or JPG files
   - Recommended max size: 1920x1080
   - Larger images = more memory usage

2. **Asset Organization:**
   - Keep your asset library organized
   - Delete unused assets to reduce clutter

---

## 📞 Need Help?

If you encounter issues not covered here:
1. Check [GitHub Issues](https://github.com/Akileus77/Velutan-image-manager-obs/issues)
2. Open a new issue with:
   - Your OBS version
   - Plugin version
   - Error messages from OBS logs
   - Steps to reproduce the problem

---

**Happy Streaming! 🎬**

