# Velutan Image Manager for OBS Studio

A powerful OBS Studio plugin for managing background images and character overlays with an intuitive, modern interface.

![Version](https://img.shields.io/badge/version-1.0.0-blue)
![OBS Studio](https://img.shields.io/badge/OBS%20Studio-30.0%2B-purple)
![Qt](https://img.shields.io/badge/Qt-6.x-green)
![Platform](https://img.shields.io/badge/platform-Windows-blue)

## ✨ Features

### 🎬 Scene Management
- **Per-Scene Backgrounds**: Each scene maintains its own background independently
- **Auto-Creation**: Automatically creates background sources when needed
- **Multi-Scene Support**: Seamlessly switch between scenes with different backgrounds

### 🖼️ Background Management
- **Theme-Based Organization**: Categorize backgrounds by themes (Desert, Forest, City, etc.)
- **Tag Filtering**: Filter backgrounds by custom tags
- **Auto-Stretch**: Automatically scale backgrounds to fit your canvas
- **Active Indicator**: Currently active background highlighted at the top with green styling
- **Thumbnail Previews**: 80x80px image previews for easy identification

### 👤 Character Management
- **Scene-Specific Characters**: Add characters independently to each scene
- **Show/Hide Toggle**: Quickly toggle character visibility
- **Active Characters on Top**: Active characters automatically move to the top of the list
- **Layer Control**: Bring characters to front or remove from scene
- **Tag-Based Filtering**: Organize and filter characters by tags

### ✏️ Asset Management
- **Edit Assets**: Modify name, tags, and theme information
- **Delete Assets**: Remove unwanted assets with confirmation
- **Dynamic Updates**: Changes reflect immediately in the UI
- **Persistent Storage**: All settings and configurations saved automatically

### 🎨 Modern UI/UX
- **Dark Theme**: Professional, eye-friendly interface
- **Visual Indicators**: Color-coded status (green for active, orange for visible)
- **Hover Effects**: Smooth animations and visual feedback
- **Searchable Dropdowns**: Quick access to themes and tags
- **Emoji Icons**: Intuitive visual elements

## 📋 Requirements

- **OBS Studio**: Version 30.0 or higher (Qt6-based builds)
- **Operating System**: Windows 10/11 (64-bit)
- **Qt Runtime**: Qt6 libraries (included in installation)

## 🚀 Installation

### Option 1: Automatic Installer (Recommended)

1. Download `Velutan-Image-Manager-Installer.exe` from [Releases](https://github.com/Akileus77/Velutan-image-manager-obs/releases)
2. Run the installer
3. Follow the on-screen instructions
4. Restart OBS Studio
5. Access the plugin via `View → Docks → Velutan Image Manager`

### Option 2: Manual Installation

1. Download `Velutan-Image-Manager-Manual.zip` from [Releases](https://github.com/Akileus77/Velutan-image-manager-obs/releases)
2. Extract the archive
3. Copy the contents:
   ```
   velutan-image-manager.dll → C:\Program Files\obs-studio\obs-plugins\64bit\
   Qt6*.dll files → C:\Program Files\obs-studio\obs-plugins\64bit\
   data\ folder → C:\Program Files\obs-studio\data\obs-plugins\velutan-image-manager\
   ```
4. Restart OBS Studio
5. Access the plugin via `View → Docks → Velutan Image Manager`

**Note**: You may need administrator privileges to copy files to the OBS installation directory.

## 📖 Usage

### First Time Setup

1. Open OBS Studio
2. Go to `View → Docks → Velutan Image Manager`
3. The plugin panel will appear on the right side (you can dock it anywhere)
4. Click `Tools → Velutan Image Manager (Setup)` to add your first assets

### Adding Assets

#### Via Tools Menu
1. `Tools → Velutan Image Manager (Setup)`
2. Click `Add Background` or `Add Character`
3. Browse for your image file
4. Enter details:
   - **Name**: Display name for the asset
   - **Tags**: Comma-separated tags for filtering
   - **Theme** (backgrounds only): Select or type a theme
5. Click `Save Library`

#### Via Widget
1. Click the `➕ Add` button in the plugin panel
2. Follow the same process as above

### Using Backgrounds

1. Select your desired scene from the dropdown
2. Go to the `🖼 Backgrounds` tab
3. Click `Set` on any background to apply it
4. The active background will:
   - Move to the top of the list
   - Show green highlighting
   - Display "✓ Active" button

### Using Characters

1. Select your scene from the dropdown
2. Go to the `👤 Characters` tab
3. Click `👤 Show` to add a character to the scene
4. Active characters will:
   - Move to the top of the list
   - Show green highlighting
   - Display `👁 Hide` button (orange)
   - Show additional buttons: `⬆ Front` and `❌ Remove from Scene`

### Filtering Assets

- **Search Bar**: Type to search by name, theme, or tags
- **Theme Filter** (Backgrounds): Filter by specific themes
- **Tag Filter**: Separate filters for backgrounds and characters
- Tab between Backgrounds and Characters to see relevant filters

### Editing Assets

1. Click the `✏` (edit) button on any asset
2. Modify name, tags, or theme
3. Click `💾 Save`
4. Changes are immediately applied and saved

### Deleting Assets

1. Click the `🗑` (delete) button on any asset
2. Confirm the deletion
3. Asset is removed from library
4. If it was an active background, it's cleared from the scene

## ⚙️ Configuration

The plugin stores configuration in:
```
%APPDATA%\obs-studio\velutan-image-manager\
├── config.json       (User settings)
└── library.json      (Asset library)
```

### Settings
- **Selected Scene**: Last used scene
- **Background Target**: Source name for backgrounds (default: `BG_Stage`)
- **Overlay Prefix**: Prefix for character sources (default: `CHAR_`)
- **Auto-Stretch Backgrounds**: Automatically scale backgrounds to canvas
- **Active Backgrounds**: Per-scene background tracking

## 🔧 Troubleshooting

### Plugin doesn't appear in OBS
- Ensure OBS Studio is version 30.0 or higher (Qt6-based)
- Check that all DLL files are in the correct locations
- Restart OBS Studio completely
- Check OBS logs: `Help → Log Files → Current Log`

### Backgrounds don't change
- Ensure you've selected the correct scene
- Check that the `BG_Stage` source exists in your scene
- Verify the image file path is valid

### Characters don't show
- Character sources are scene-specific (format: `SceneName_CHAR_AssetID`)
- Make sure you're on the correct scene
- Check source visibility in OBS Sources panel

### Qt DLL errors
- Ensure all Qt6 DLLs are copied to the plugin directory
- Download and reinstall if any DLLs are missing

## 🛠️ Building from Source

### Prerequisites
- Visual Studio 2019 or later
- CMake 3.16+
- Qt 6.7.2 or later
- OBS Studio SDK (built from source)

### Build Instructions

1. Clone the repository:
   ```bash
   git clone https://github.com/Akileus77/Velutan-image-manager-obs.git
   cd Velutan-image-manager-obs
   ```

2. Create build directory:
   ```bash
   mkdir build
   cd build
   ```

3. Configure with CMake:
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Release
   ```

4. Build:
   ```bash
   cmake --build . --config Release
   ```

5. The plugin DLL will be in `build/Release/velutan-image-manager.dll`

## 📝 License

This project is licensed under the GPL v2 License - see the [LICENSE](LICENSE) file for details.

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## 💬 Support

If you encounter any issues or have questions:
- Open an issue on [GitHub Issues](https://github.com/Akileus77/Velutan-image-manager-obs/issues)
- Check existing issues for solutions

## 🙏 Acknowledgments

- OBS Studio team for the excellent streaming software
- Qt Project for the UI framework
- All contributors and users of this plugin

## 📸 Screenshots

### Main Interface
*Modern dark theme with intuitive controls*

### Background Management
*Theme-based organization with visual previews*

### Character Management
*Scene-specific character control with easy toggling*

---

Made with ❤️ for the OBS Studio community
