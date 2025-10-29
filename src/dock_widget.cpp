#include "dock_widget.hpp"
#include "setup_dialog.hpp"
#include "theme_constants.hpp"

#include "ui/HeaderBar.hpp"
#include "ui/AssetList.hpp"
#include "ui/TutorialCard.hpp"
#include "ui/Toast.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTabWidget>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QTimer>
#include <QComboBox>
#include <QPushButton>
#include <QDialog>
#include <QLabel>
#include <QCompleter>
#include <QSet>
#include <algorithm>
#include <cstring>

extern "C" {
#include <obs-module.h>
#include <obs-frontend-api.h>
}

VelutanDockWidget::VelutanDockWidget(QWidget *parent)
    : QWidget(parent)
{
    blog(LOG_INFO, "[Velutan] VelutanDockWidget constructor started");
    
    setObjectName("VelutanDockWidget");
    
    blog(LOG_INFO, "[Velutan] Setting up main widget...");
    // Set background style for the main widget
    setStyleSheet("QWidget { background-color: #1E1E1E; }");
    
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(8);

    // Header bar with scene/prefix controls and auto-setup button
    m_headerBar = new HeaderBar(this);
    layout->addWidget(m_headerBar);
    connect(m_headerBar, &HeaderBar::sceneChanged, this, &VelutanDockWidget::onSceneChanged);
    connect(m_headerBar, &HeaderBar::bgTargetChanged, this, &VelutanDockWidget::onBgTargetChanged);
    connect(m_headerBar, &HeaderBar::overlayPrefixChanged, this, &VelutanDockWidget::onOverlayPrefixChanged);
    connect(m_headerBar, &HeaderBar::autoSetupRequested, this, &VelutanDockWidget::onAutoSetup);
    connect(m_headerBar, &HeaderBar::autoStretchChanged, this, &VelutanDockWidget::onAutoStretchChanged);

    // Tutorial card appears conditionally
    m_tutorial = new TutorialCard(this);
    layout->addWidget(m_tutorial);
    connect(m_tutorial, &TutorialCard::dismissed, this, &VelutanDockWidget::onDismissTutorial);

    // Filter and search bar
    auto *filterLayout = new QHBoxLayout();
    filterLayout->setSpacing(8);
    
    // Search box for filtering assets with modern styling
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("🔍 " + QString(obs_module_text("Search...")));
    m_searchEdit->setStyleSheet(
        "QLineEdit { "
        "   background-color: #2D2D30; "
        "   border: 2px solid #3F3F46; "
        "   border-radius: 6px; "
        "   padding: 8px 12px; "
        "   color: #E0E0E0; "
        "   font-size: 11px; "
        "}"
        "QLineEdit:focus { "
        "   border: 2px solid #007ACC; "
        "   background-color: #252526; "
        "}"
    );
    filterLayout->addWidget(m_searchEdit, 2);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &VelutanDockWidget::onSearchChanged);

    // Theme filter combobox
    m_themeFilter = new QComboBox(this);
    m_themeFilter->setEditable(false);
    m_themeFilter->addItem("All Themes");
    m_themeFilter->setStyleSheet(
        "QComboBox { "
        "   background-color: #2D2D30; "
        "   border: 1px solid #3F3F46; "
        "   border-radius: 4px; "
        "   padding: 6px; "
        "   color: #E0E0E0; "
        "   font-size: 11px; "
        "   min-width: 100px; "
        "}"
        "QComboBox::drop-down { border: none; }"
        "QComboBox:hover { border: 1px solid #007ACC; }"
    );
    filterLayout->addWidget(m_themeFilter, 1);
    connect(m_themeFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &VelutanDockWidget::onFilterChanged);
    
    // Tag filter for backgrounds
    m_bgTagFilter = new QComboBox(this);
    m_bgTagFilter->setEditable(false);
    m_bgTagFilter->addItem("All Tags");
    m_bgTagFilter->setStyleSheet(
        "QComboBox { "
        "   background-color: #2D2D30; "
        "   border: 1px solid #3F3F46; "
        "   border-radius: 4px; "
        "   padding: 6px; "
        "   color: #E0E0E0; "
        "   font-size: 11px; "
        "   min-width: 100px; "
        "}"
        "QComboBox::drop-down { border: none; }"
        "QComboBox:hover { border: 1px solid #007ACC; }"
    );
    filterLayout->addWidget(m_bgTagFilter, 1);
    connect(m_bgTagFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &VelutanDockWidget::onFilterChanged);
    
    // Tag filter for characters
    m_charTagFilter = new QComboBox(this);
    m_charTagFilter->setEditable(false);
    m_charTagFilter->addItem("All Tags");
    m_charTagFilter->setStyleSheet(
        "QComboBox { "
        "   background-color: #2D2D30; "
        "   border: 1px solid #3F3F46; "
        "   border-radius: 4px; "
        "   padding: 6px; "
        "   color: #E0E0E0; "
        "   font-size: 11px; "
        "   min-width: 100px; "
        "}"
        "QComboBox::drop-down { border: none; }"
        "QComboBox:hover { border: 1px solid #007ACC; }"
    );
    filterLayout->addWidget(m_charTagFilter, 1);
    connect(m_charTagFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &VelutanDockWidget::onFilterChanged);
    
    // Refresh button (to manually reload library)
    QPushButton *refreshBtn = new QPushButton("🔄", this);
    refreshBtn->setToolTip("Refresh library");
    refreshBtn->setStyleSheet(
        "QPushButton { "
        "   background-color: #6C757D; "
        "   border: none; "
        "   border-radius: 4px; "
        "   padding: 8px 12px; "
        "   color: white; "
        "   font-weight: 500; "
        "   font-size: 14px; "
        "   min-width: 40px; "
        "}"
        "QPushButton:hover { background-color: #5A6268; }"
        "QPushButton:pressed { background-color: #545B62; }"
    );
    filterLayout->addWidget(refreshBtn);
    connect(refreshBtn, &QPushButton::clicked, this, &VelutanDockWidget::reloadLibrary);
    
    // Add Asset button
    QPushButton *addBtn = new QPushButton("➕ Add", this);
    addBtn->setStyleSheet(
        "QPushButton { "
        "   background-color: #28A745; "
        "   border: none; "
        "   border-radius: 4px; "
        "   padding: 8px 16px; "
        "   color: white; "
        "   font-weight: 500; "
        "   font-size: 11px; "
        "}"
        "QPushButton:hover { background-color: #218838; }"
        "QPushButton:pressed { background-color: #1E7E34; }"
    );
    filterLayout->addWidget(addBtn);
    connect(addBtn, &QPushButton::clicked, this, &VelutanDockWidget::onAddAsset);
    
    layout->addLayout(filterLayout);

    // Tab widget for backgrounds and characters with modern styling
    m_tabs = new QTabWidget(this);
    m_tabs->setStyleSheet(
        "QTabWidget::pane { "
        "   border: 1px solid #3F3F46; "
        "   border-radius: 4px; "
        "   background-color: #1E1E1E; "
        "   top: -1px; "
        "}"
        "QTabBar::tab { "
        "   background-color: #2D2D30; "
        "   border: 1px solid #3F3F46; "
        "   border-bottom: none; "
        "   border-top-left-radius: 4px; "
        "   border-top-right-radius: 4px; "
        "   padding: 8px 16px; "
        "   margin-right: 4px; "
        "   color: #9CA3AF; "
        "   font-weight: 500; "
        "   font-size: 11px; "
        "}"
        "QTabBar::tab:selected { "
        "   background-color: #007ACC; "
        "   color: white; "
        "   border-color: #007ACC; "
        "}"
        "QTabBar::tab:hover:!selected { "
        "   background-color: #3F3F46; "
        "   color: #E0E0E0; "
        "}"
    );
    
    m_bgList = new AssetList(true, m_tabs);
    m_charList = new AssetList(false, m_tabs);
    m_tabs->addTab(m_bgList, "🖼 " + QString(obs_module_text("Backgrounds")));
    m_tabs->addTab(m_charList, "👤 " + QString(obs_module_text("Characters")));
    layout->addWidget(m_tabs, 1);  // Give tabs stretch factor
    
    // React to tab changes to persist the last selected tab and update filter visibility
    connect(m_tabs, &QTabWidget::currentChanged, this, [this](int index) {
        m_config.lastTabIndex = index;
        saveConfig();
        
        // Show/hide filters based on tab
        // Index 0 = Backgrounds (show theme + bg tag filter, hide char tag filter)
        // Index 1 = Characters (hide theme + bg tag filter, show char tag filter)
        if (index == 0) {
            m_themeFilter->setVisible(true);
            m_bgTagFilter->setVisible(true);
            m_charTagFilter->setVisible(false);
        } else {
            m_themeFilter->setVisible(false);
            m_bgTagFilter->setVisible(false);
            m_charTagFilter->setVisible(true);
        }
    });

    // Asset actions propagate through this dock widget
    connect(m_bgList, &AssetList::assetActionTriggered, this, &VelutanDockWidget::onAssetAction);
    connect(m_charList, &AssetList::assetActionTriggered, this, &VelutanDockWidget::onAssetAction);

    // Toast for transient notifications
    m_toast = new Toast(this);
    // Place the toast above the tabs; layout order ensures it stays at
    // the bottom of the dock.  It is hidden by default.
    layout->addWidget(m_toast, 0, Qt::AlignCenter);

    setLayout(layout);

    // Load user config and asset library
    loadConfig();
    loadLibrary();
    
    blog(LOG_INFO, "[Velutan] Loaded config and library");
    
    // Set header values from config (scene list will be set in delayed initialization)
    m_headerBar->setBgTargetName(m_config.bgTargetName);
    m_headerBar->setOverlayPrefix(m_config.overlayPrefix);
    m_headerBar->setAutoStretch(m_config.autoStretchBackgrounds);
    
    blog(LOG_INFO, "[Velutan] Header bar configured");
    // Tutorial visibility
    m_tutorial->setVisibleByConfig(m_config.dismissedTutorial);
    // Restore last search and tab
    m_searchEdit->setText(m_config.lastSearch);
    m_tabs->setCurrentIndex(m_config.lastTabIndex);
    
    // Set initial filter visibility based on tab
    if (m_config.lastTabIndex == 0) {
        m_themeFilter->setVisible(true);
        m_bgTagFilter->setVisible(true);
        m_charTagFilter->setVisible(false);
    } else {
        m_themeFilter->setVisible(false);
        m_bgTagFilter->setVisible(false);
        m_charTagFilter->setVisible(true);
    }
    
    blog(LOG_INFO, "[Velutan] Widget setup complete, scheduling delayed initialization");
    
    // Populate lists and update scene list after a short delay to avoid startup crashes
    // Use a single-shot timer to defer heavy OBS API calls
    QTimer::singleShot(500, this, [this]() {
        blog(LOG_INFO, "[Velutan] Running delayed initialization");
        try {
            updateSceneList();
            updateFilterLists();
    refreshLists();
            blog(LOG_INFO, "[Velutan] Delayed initialization completed");
        } catch (...) {
            blog(LOG_ERROR, "[Velutan] Exception in delayed initialization");
        }
    });
    
    blog(LOG_INFO, "[Velutan] VelutanDockWidget constructor completed successfully");
}

VelutanDockWidget::~VelutanDockWidget()
{
    // Persist user preferences on destruction
    saveConfig();
}

void VelutanDockWidget::loadLibrary()
{
    // Attempt to load the user library from the config directory.  If
    // none exists we fall back to the default library bundled with the
    // plugin in the data folder.
    QString userPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)
            + "/velutan-image-manager/library.json";
    QFile userFile(userPath);
    if (userFile.exists()) {
        m_library = AssetLibrary::loadFromFile(userPath);
        return;
    }
    // Fallback: locate the default library relative to the module's
    // binary.  QCoreApplication::applicationDirPath() returns the
    // directory where the plugin DLL/SO lives when loaded by OBS.
    QString fallback = QCoreApplication::applicationDirPath()
            + "/data/velutan_library.json";
    m_library = AssetLibrary::loadFromFile(fallback);
}

void VelutanDockWidget::loadConfig()
{
    // Use the free function from persistence.cpp rather than the
    // member function recursively.  The :: prefix forces lookup in
    // global namespace.
    m_config = ::loadConfig();
}

void VelutanDockWidget::saveConfig()
{
    // Write the current configuration via the free function.  Without
    // the :: prefix this would recursively call the member function.
    ::saveConfig(m_config);
}

void VelutanDockWidget::updateSceneList()
{
    // Enumerate all scenes in OBS
    QStringList scenes;
    
    blog(LOG_INFO, "[Velutan] updateSceneList started");
    
    try {
        struct obs_frontend_source_list scene_list = {};
        obs_frontend_get_scenes(&scene_list);
        
        blog(LOG_INFO, "[Velutan] Found %zu scenes", scene_list.sources.num);
        
        if (scene_list.sources.array && scene_list.sources.num > 0) {
            for (size_t i = 0; i < scene_list.sources.num; i++) {
                obs_source_t *source = scene_list.sources.array[i];
                if (source) {
                    const char *name = obs_source_get_name(source);
                    if (name && strlen(name) > 0) {
                        scenes << QString::fromUtf8(name);
                        blog(LOG_INFO, "[Velutan] Added scene: %s", name);
                    }
                }
            }
        }
        
        obs_frontend_source_list_free(&scene_list);
    } catch (...) {
        blog(LOG_ERROR, "[Velutan] Exception in updateSceneList");
    }
    
    // Fallback: at least add current scene
    if (scenes.isEmpty()) {
        blog(LOG_WARNING, "[Velutan] Scene list is empty, using current scene");
        obs_source_t *cur = obs_frontend_get_current_scene();
        if (cur) {
            const char *name = obs_source_get_name(cur);
            if (name) {
                scenes << QString::fromUtf8(name);
                blog(LOG_INFO, "[Velutan] Fallback: added current scene %s", name);
            }
            obs_source_release(cur);
        }
    }
    
    // Set current scene as selected
    obs_source_t *cur = obs_frontend_get_current_scene();
    QString currentSceneName;
    if (cur) {
        currentSceneName = QString::fromUtf8(obs_source_get_name(cur));
        obs_source_release(cur);
    }
    
    blog(LOG_INFO, "[Velutan] Total scenes: %d, setting scene list", scenes.size());
    m_headerBar->setSceneList(scenes);
    
    // If config scene exists in list, use it; otherwise use current scene
    if (!m_config.selectedScene.isEmpty() && scenes.contains(m_config.selectedScene)) {
        m_headerBar->setSelectedScene(m_config.selectedScene);
        blog(LOG_INFO, "[Velutan] Selected scene from config: %s", m_config.selectedScene.toUtf8().constData());
    } else if (!currentSceneName.isEmpty()) {
        m_config.selectedScene = currentSceneName;
        m_headerBar->setSelectedScene(currentSceneName);
        blog(LOG_INFO, "[Velutan] Selected current scene: %s", currentSceneName.toUtf8().constData());
    }
}

void VelutanDockWidget::refreshLists()
{
    try {
        QString query = m_searchEdit->text().trimmed();
        QString selectedTheme = m_themeFilter->currentText();
        QString selectedBgTag = m_bgTagFilter->currentText();
        QString selectedCharTag = m_charTagFilter->currentText();
        
        // Apply search filter first
        QVector<Asset> bgMatches = AssetLibrary::search(m_library.backgrounds, query);
        QVector<Asset> charMatches = AssetLibrary::search(m_library.characters, query);
        
        // Apply theme filter (backgrounds only)
        if (selectedTheme != "🌍 All Themes" && !selectedTheme.isEmpty()) {
            QVector<Asset> filteredBg;
            for (const Asset &asset : bgMatches) {
                if (asset.theme == selectedTheme) {
                    filteredBg.append(asset);
                }
            }
            bgMatches = filteredBg;
        }
        
        // Apply background tag filter
        if (selectedBgTag != "🏷 All Tags" && !selectedBgTag.isEmpty()) {
            QVector<Asset> filteredBg;
            for (const Asset &asset : bgMatches) {
                if (asset.tags.contains(selectedBgTag)) {
                    filteredBg.append(asset);
                }
            }
            bgMatches = filteredBg;
        }
        
        // Apply character tag filter
        if (selectedCharTag != "🏷 All Tags" && !selectedCharTag.isEmpty()) {
            QVector<Asset> filteredChar;
            for (const Asset &asset : charMatches) {
                if (asset.tags.contains(selectedCharTag)) {
                    filteredChar.append(asset);
                }
            }
            charMatches = filteredChar;
        }
        
        // Get active background for current scene
        QStringList activeBgList;
        if (m_config.activeBackgrounds.contains(m_config.selectedScene)) {
            activeBgList << m_config.activeBackgrounds[m_config.selectedScene];
        }
        m_bgList->setActiveAssets(activeBgList);
        
        // Detect which characters are currently active (visible) in the scene
        // Use scene-specific source names
        QStringList activeCharacters;
        if (!m_config.selectedScene.isEmpty()) {
            for (const Asset &asset : charMatches) {
                QString sourceName = m_config.selectedScene + "_" + m_config.overlayPrefix + asset.id;
                try {
                    if (m_obs.isVisible(m_config.selectedScene, sourceName)) {
                        activeCharacters << asset.name;
                    }
                } catch (...) {
                    // Skip this asset if checking visibility fails
                }
            }
        }
        
        // Update character list with active status
        m_charList->setActiveAssets(activeCharacters);
        
        // Set the assets
        m_bgList->setAssets(bgMatches);
        m_charList->setAssets(charMatches);
    } catch (...) {
        blog(LOG_ERROR, "[Velutan] Exception in refreshLists");
    }
}

void VelutanDockWidget::onSearchChanged(const QString &text)
{
    m_config.lastSearch = text;
    refreshLists();
    saveConfig();
}

void VelutanDockWidget::onSceneChanged(const QString &name)
{
    m_config.selectedScene = name;
    
    // Ensure the background source exists in the newly selected scene
    m_obs.ensureBackgroundTarget(m_config.selectedScene, m_config.bgTargetName);
    
    // Refresh lists to update character active status for new scene
    refreshLists();
    
    saveConfig();
}

void VelutanDockWidget::onBgTargetChanged(const QString &name)
{
    m_config.bgTargetName = name;
    // Ensure the background source exists in the selected scene
    m_obs.ensureBackgroundTarget(m_config.selectedScene, m_config.bgTargetName);
    saveConfig();
}

void VelutanDockWidget::onOverlayPrefixChanged(const QString &prefix)
{
    m_config.overlayPrefix = prefix;
    saveConfig();
}

void VelutanDockWidget::onAutoSetup()
{
    autoSetup();
}

void VelutanDockWidget::autoSetup()
{
    // Show informative message about what auto-setup does
    QString infoMsg = QString(
        "<p><b>Auto-Setup will:</b></p>"
        "<ul>"
        "<li>Create scene: <b>%1</b> (if doesn't exist)</li>"
        "<li>Add background source: <b>%2</b></li>"
        "<li>Prepare character overlay prefix: <b>%3</b></li>"
        "</ul>"
        "<p>Current scene will switch to the setup scene.</p>"
    ).arg(m_config.selectedScene, m_config.bgTargetName, m_config.overlayPrefix);
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Auto Setup");
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(infoMsg);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    
    // Modern styling for the message box
    msgBox.setStyleSheet(
        "QMessageBox { background-color: #2D2D30; }"
        "QLabel { color: #E0E0E0; font-size: 11px; }"
        "QPushButton { "
        "   background-color: #007ACC; "
        "   border: none; "
        "   border-radius: 4px; "
        "   padding: 6px 16px; "
        "   color: white; "
        "   min-width: 70px; "
        "}"
        "QPushButton:hover { background-color: #005FA3; }"
    );
    
    if (msgBox.exec() != QMessageBox::Ok) {
        return;  // User cancelled
    }
    
    // Create scene and background if they do not exist
    bool ok1 = m_obs.ensureScene(m_config.selectedScene);
    bool ok2 = m_obs.ensureBackgroundTarget(m_config.selectedScene, m_config.bgTargetName);
    
    if (ok1 && ok2) {
        m_toast->showMessage("✓ Auto-setup completed successfully!");
    } else {
        m_toast->showMessage("✗ Auto-setup failed. Please check your settings.");
    }
}

void VelutanDockWidget::onAssetAction(const Asset &asset, const QString &action)
{
    // Resolve the asset's file path.  If the path is relative then we
    // assume it lives in the plugin's data directory.
    QString filePath = asset.file;
    if (!QDir::isAbsolutePath(filePath)) {
        filePath = QCoreApplication::applicationDirPath() + "/data/" + asset.file;
    }
    if (action == QLatin1String("set")) {
        // Ensure background target exists in the current scene
        m_obs.ensureBackgroundTarget(m_config.selectedScene, m_config.bgTargetName);
        // Set as background (with auto-stretch if enabled)
        m_obs.setBackground(m_config.selectedScene, m_config.bgTargetName, filePath, m_config.autoStretchBackgrounds);
        
        // Track active background for this scene
        m_config.activeBackgrounds[m_config.selectedScene] = asset.id;
        saveConfig();
        
        // Refresh to update UI (active background moves to top, green styling)
        refreshLists();
        
        m_toast->showMessage("✓ Background changed to: " + asset.name);
    } else if (action == QLatin1String("toggle")) {
        // Scene-specific character source name
        QString srcName = m_config.selectedScene + "_" + m_config.overlayPrefix + asset.id;
        bool visible = m_obs.isVisible(m_config.selectedScene, srcName);
        if (!visible) {
            m_obs.ensureCharacter(m_config.selectedScene, srcName, filePath);
            m_toast->showMessage("👤 " + asset.name + " added to scene");
        } else {
            m_obs.toggleCharacter(m_config.selectedScene, srcName, false);
            m_toast->showMessage("👁 " + asset.name + " hidden");
        }
        // Refresh the list to update active status
        refreshLists();
    } else if (action == QLatin1String("front")) {
        // Scene-specific character source name
        QString srcName = m_config.selectedScene + "_" + m_config.overlayPrefix + asset.id;
        m_obs.bringToFront(m_config.selectedScene, srcName);
        m_toast->showMessage("⬆ " + asset.name + " brought to front");
    } else if (action == QLatin1String("remove")) {
        // Remove character from scene completely
        QString srcName = m_config.selectedScene + "_" + m_config.overlayPrefix + asset.id;
        
        // Get the source and remove it from the scene
        obs_source_t *source = obs_get_source_by_name(srcName.toUtf8().constData());
        if (source) {
            // Remove from scene
            obs_scene_t *scene = obs_scene_from_source(obs_frontend_get_current_scene());
            if (scene) {
                obs_sceneitem_t *item = obs_scene_find_source(scene, srcName.toUtf8().constData());
                if (item) {
                    obs_sceneitem_remove(item);
                }
            }
            obs_source_release(source);
            m_toast->showMessage("🗑 " + asset.name + " removed from scene");
            refreshLists();
        }
    } else if (action == QLatin1String("edit")) {
        // Edit asset dialog
        QDialog editDialog(this);
        editDialog.setWindowTitle("Edit Asset");
        editDialog.setMinimumWidth(400);
        
        auto *layout = new QVBoxLayout(&editDialog);
        layout->setSpacing(12);
        
        // Name field
        layout->addWidget(new QLabel("Name:", &editDialog));
        auto *nameEdit = new QLineEdit(asset.name, &editDialog);
        layout->addWidget(nameEdit);
        
        // Tags field
        layout->addWidget(new QLabel("Tags (comma-separated):", &editDialog));
        auto *tagsEdit = new QLineEdit(asset.tags.join(", "), &editDialog);
        layout->addWidget(tagsEdit);
        
        // Theme field (only for backgrounds)
        QComboBox *themeCombo = nullptr;
        bool isBackground = false;
        for (const Asset &bg : m_library.backgrounds) {
            if (bg.id == asset.id) {
                isBackground = true;
                break;
            }
        }
        
        if (isBackground) {
            layout->addWidget(new QLabel("Theme:", &editDialog));
            themeCombo = new QComboBox(&editDialog);
            themeCombo->setEditable(true);
            themeCombo->addItems(ThemeConstants::getThemes());
            themeCombo->setCurrentText(asset.theme);
            
            // Add completer for search
            QCompleter *completer = new QCompleter(ThemeConstants::getThemes(), &editDialog);
            completer->setCaseSensitivity(Qt::CaseInsensitive);
            completer->setFilterMode(Qt::MatchContains);
            themeCombo->setCompleter(completer);
            
            layout->addWidget(themeCombo);
        }
        
        // Buttons
        auto *buttonLayout = new QHBoxLayout();
        auto *saveBtn = new QPushButton("💾 Save", &editDialog);
        auto *cancelBtn = new QPushButton("❌ Cancel", &editDialog);
        buttonLayout->addStretch();
        buttonLayout->addWidget(saveBtn);
        buttonLayout->addWidget(cancelBtn);
        layout->addLayout(buttonLayout);
        
        connect(saveBtn, &QPushButton::clicked, &editDialog, &QDialog::accept);
        connect(cancelBtn, &QPushButton::clicked, &editDialog, &QDialog::reject);
        
        if (editDialog.exec() == QDialog::Accepted) {
            // Update asset
            QString newName = nameEdit->text().trimmed();
            QString newTagsStr = tagsEdit->text().trimmed();
            QStringList newTags;
            if (!newTagsStr.isEmpty()) {
                for (const QString &tag : newTagsStr.split(',')) {
                    QString t = tag.trimmed();
                    if (!t.isEmpty()) {
                        newTags << t;
                    }
                }
            }
            
            // Find and update the asset
            bool updated = false;
            for (Asset &bg : m_library.backgrounds) {
                if (bg.id == asset.id) {
                    bg.name = newName;
                    bg.tags = newTags;
                    if (themeCombo) {
                        bg.theme = themeCombo->currentText().trimmed();
                    }
                    updated = true;
                    break;
                }
            }
            if (!updated) {
                for (Asset &ch : m_library.characters) {
                    if (ch.id == asset.id) {
                        ch.name = newName;
                        ch.tags = newTags;
                        updated = true;
                        break;
                    }
                }
            }
            
            if (updated) {
                // Save library
                QString userPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)
                        + "/velutan-image-manager/library.json";
                AssetLibrary::saveToFile(userPath, m_library);
                
                // Update filters and refresh lists
                updateFilterLists();
                refreshLists();
                m_toast->showMessage("✓ " + newName + " updated");
            }
        }
    } else if (action == QLatin1String("delete")) {
        // Confirm deletion
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Delete Asset");
        msgBox.setText(QString("Delete '%1' from library?").arg(asset.name));
        msgBox.setInformativeText("This cannot be undone.");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        
        if (msgBox.exec() == QMessageBox::Yes) {
            // Remove from library
            bool removed = false;
            for (int i = 0; i < m_library.backgrounds.size(); i++) {
                if (m_library.backgrounds[i].id == asset.id) {
                    m_library.backgrounds.removeAt(i);
                    removed = true;
                    break;
                }
            }
            if (!removed) {
                for (int i = 0; i < m_library.characters.size(); i++) {
                    if (m_library.characters[i].id == asset.id) {
                        m_library.characters.removeAt(i);
                        removed = true;
                        break;
                    }
                }
            }
            
            if (removed) {
                // Save library
                QString userPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)
                        + "/velutan-image-manager/library.json";
                AssetLibrary::saveToFile(userPath, m_library);
                
                // If it was a background, clear BG_Stage if it's showing this image
                bool wasBackground = false;
                for (const Asset &bg : m_library.backgrounds) {
                    if (bg.id == asset.id) {
                        wasBackground = true;
                        break;
                    }
                }
                
                // Actually check if we removed from backgrounds
                if (!wasBackground) {
                    // It was removed from backgrounds, clear BG_Stage
                    m_obs.setBackground(m_config.selectedScene, m_config.bgTargetName, "", false);
                }
                
                // Update filters and refresh lists
                updateFilterLists();
                refreshLists();
                m_toast->showMessage("🗑 " + asset.name + " deleted");
            }
        }
    }
}

void VelutanDockWidget::onDismissTutorial(bool remember)
{
    if (remember) {
        m_config.dismissedTutorial = true;
        saveConfig();
    }
}

void VelutanDockWidget::onAutoStretchChanged(bool enabled)
{
    m_config.autoStretchBackgrounds = enabled;
    saveConfig();
}

void VelutanDockWidget::updateFilterLists()
{
    // Populate theme filter
    QSet<QString> themes;
    for (const Asset &asset : m_library.backgrounds) {
        if (!asset.theme.isEmpty()) {
            themes.insert(asset.theme);
        }
    }
    
    m_themeFilter->clear();
    m_themeFilter->addItem("🌍 All Themes");
    QStringList themeList = themes.values();
    std::sort(themeList.begin(), themeList.end());
    for (const QString &theme : themeList) {
        m_themeFilter->addItem(theme);
    }
    
    // Populate background tag filter
    QSet<QString> bgTags;
    for (const Asset &asset : m_library.backgrounds) {
        for (const QString &tag : asset.tags) {
            if (!tag.isEmpty()) {
                bgTags.insert(tag);
            }
        }
    }
    
    m_bgTagFilter->clear();
    m_bgTagFilter->addItem("🏷 All Tags");
    QStringList bgTagList = bgTags.values();
    std::sort(bgTagList.begin(), bgTagList.end());
    for (const QString &tag : bgTagList) {
        m_bgTagFilter->addItem(tag);
    }
    
    // Populate character tag filter
    QSet<QString> charTags;
    for (const Asset &asset : m_library.characters) {
        for (const QString &tag : asset.tags) {
            if (!tag.isEmpty()) {
                charTags.insert(tag);
            }
        }
    }
    
    m_charTagFilter->clear();
    m_charTagFilter->addItem("🏷 All Tags");
    QStringList charTagList = charTags.values();
    std::sort(charTagList.begin(), charTagList.end());
    for (const QString &tag : charTagList) {
        m_charTagFilter->addItem(tag);
    }
}

void VelutanDockWidget::onFilterChanged()
{
    refreshLists();
}

void VelutanDockWidget::onAddAsset()
{
    // Show add asset dialog (reuse setup dialog code)
    auto *dlg = new VelutanSetupDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    connect(dlg, &QDialog::finished, this, [this](int result) {
        if (result == QDialog::Accepted) {
            reloadLibrary();
        }
    });
    dlg->show();
    dlg->raise();
    dlg->activateWindow();
}

void VelutanDockWidget::reloadLibrary()
{
    // Reload library from disk and refresh UI
    loadLibrary();
    updateFilterLists();
    refreshLists();
    m_toast->showMessage("✓ Library updated");
}