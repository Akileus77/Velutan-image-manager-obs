#pragma once

#include <QString>
#include <QJsonObject>
#include <QMap>

/*
 * persistence.hpp
 *
 * Defines a simple persistence layer for the Velutan plugin.  The
 * PersistenceConfig structure holds user preferences such as the selected
 * scene, the name of the background target source and the overlay
 * prefix.  The loadConfig() and saveConfig() functions read from and
 * write to a JSON file in the user's configuration directory.
 */

struct PersistenceConfig {
    QString selectedScene = "Velutan_Main";
    QString bgTargetName = "BG_Stage";
    QString overlayPrefix = "CHAR_";
    int lastTabIndex = 0;
    QString lastSearch;
    bool dismissedTutorial = false;
    bool autoStretchBackgrounds = true;  // Auto-stretch backgrounds to screen size
    QMap<QString, QString> activeBackgrounds;  // scene name -> background asset ID
    QStringList pinnedSources;  // Sources that should always stay on top (e.g., Camera, Player)
    
    // Grid system settings
    bool gridEnabled = false;  // Grid overlay visible
    int gridSize = 50;  // Grid cell size in pixels
    bool gridShowInStream = false;  // Show grid in stream output (or only in preview)
    bool gridSnapEnabled = true;  // Snap sources to grid when moving
    QString gridColor = "#00FF00";  // Grid line color (green by default)
    int gridOpacity = 128;  // Grid opacity (0-255)
};

/**
 * Load user configuration from disk.  If no configuration exists a
 * default configuration is returned.
 */
PersistenceConfig loadConfig();

/**
 * Save user configuration to disk.  Returns true on success.
 */
bool saveConfig(const PersistenceConfig &config);
