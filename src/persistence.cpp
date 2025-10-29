#include "persistence.hpp"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

static QString configFilePath()
{
    // Determine a writable location for our configuration.  The
    // AppConfigLocation typically resolves to something like
    // %APPDATA%/obs-studio or ~/.config/obs-studio on Unix systems.
    // We add a subdirectory to avoid cluttering the root.
    QString baseDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString dir = baseDir + "/velutan-image-manager";
    QDir d;
    if (!d.exists(dir)) {
        d.mkpath(dir);
    }
    return dir + "/config.json";
}

PersistenceConfig loadConfig()
{
    PersistenceConfig cfg;
    QString path = configFilePath();
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        // No config yet; return defaults.
        return cfg;
    }
    QByteArray data = file.readAll();
    file.close();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        return cfg;
    }
    QJsonObject obj = doc.object();
    cfg.selectedScene = obj.value("selectedScene").toString(cfg.selectedScene);
    cfg.bgTargetName = obj.value("bgTargetName").toString(cfg.bgTargetName);
    cfg.overlayPrefix = obj.value("overlayPrefix").toString(cfg.overlayPrefix);
    cfg.lastTabIndex = obj.value("lastTabIndex").toInt(cfg.lastTabIndex);
    cfg.lastSearch = obj.value("lastSearch").toString(cfg.lastSearch);
    cfg.dismissedTutorial = obj.value("dismissedTutorial").toBool(cfg.dismissedTutorial);
    cfg.autoStretchBackgrounds = obj.value("autoStretchBackgrounds").toBool(cfg.autoStretchBackgrounds);
    
    // Load active backgrounds map
    QJsonObject activeBgs = obj.value("activeBackgrounds").toObject();
    for (auto it = activeBgs.begin(); it != activeBgs.end(); ++it) {
        cfg.activeBackgrounds[it.key()] = it.value().toString();
    }
    
    // Load pinned sources
    QJsonArray pinnedArray = obj.value("pinnedSources").toArray();
    for (const QJsonValue &val : pinnedArray) {
        cfg.pinnedSources << val.toString();
    }
    
    // Load grid settings
    cfg.gridEnabled = obj.value("gridEnabled").toBool(cfg.gridEnabled);
    cfg.gridSize = obj.value("gridSize").toInt(cfg.gridSize);
    cfg.gridShowInStream = obj.value("gridShowInStream").toBool(cfg.gridShowInStream);
    cfg.gridSnapEnabled = obj.value("gridSnapEnabled").toBool(cfg.gridSnapEnabled);
    cfg.gridColor = obj.value("gridColor").toString(cfg.gridColor);
    cfg.gridOpacity = obj.value("gridOpacity").toInt(cfg.gridOpacity);
    
    return cfg;
}

bool saveConfig(const PersistenceConfig &config)
{
    QJsonObject obj;
    obj.insert("selectedScene", config.selectedScene);
    obj.insert("bgTargetName", config.bgTargetName);
    obj.insert("overlayPrefix", config.overlayPrefix);
    obj.insert("lastTabIndex", config.lastTabIndex);
    obj.insert("lastSearch", config.lastSearch);
    obj.insert("dismissedTutorial", config.dismissedTutorial);
    obj.insert("autoStretchBackgrounds", config.autoStretchBackgrounds);
    
    // Save active backgrounds map
    QJsonObject activeBgs;
    for (auto it = config.activeBackgrounds.begin(); it != config.activeBackgrounds.end(); ++it) {
        activeBgs.insert(it.key(), it.value());
    }
    obj.insert("activeBackgrounds", activeBgs);
    
    // Save pinned sources
    QJsonArray pinnedArray;
    for (const QString &source : config.pinnedSources) {
        pinnedArray.append(source);
    }
    obj.insert("pinnedSources", pinnedArray);
    
    // Save grid settings
    obj.insert("gridEnabled", config.gridEnabled);
    obj.insert("gridSize", config.gridSize);
    obj.insert("gridShowInStream", config.gridShowInStream);
    obj.insert("gridSnapEnabled", config.gridSnapEnabled);
    obj.insert("gridColor", config.gridColor);
    obj.insert("gridOpacity", config.gridOpacity);
    
    QJsonDocument doc(obj);
    QString path = configFilePath();
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "[Velutan] Could not write config file" << path;
        return false;
    }
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}