#include "obs_integration.hpp"

#include <QDebug>
#include <QImage>
#include <QPainter>
#include <QColor>
#include <QStandardPaths>
#include <QDir>

ObsIntegration::ObsIntegration(QObject *parent)
    : QObject(parent)
{
}

ObsIntegration::~ObsIntegration()
{
}

bool ObsIntegration::ensureScene(const QString &sceneName)
{
    // Try to find an existing scene.  Scenes are sources of type "scene".
    obs_source_t *sceneSource = obs_get_source_by_name(sceneName.toUtf8().constData());
    if (sceneSource) {
        obs_source_release(sceneSource);
        return true;
    }
    // Create a new scene manually since obs_frontend_create_scene is no longer available
    obs_scene_t *scene = obs_scene_create(sceneName.toUtf8().constData());
    if (!scene) {
        qWarning() << "[Velutan] Failed to create scene" << sceneName;
        return false;
    }
    // Get the source from the scene
    obs_source_t *scene_source = obs_scene_get_source(scene);
    
    // Set it as current scene to add it to the frontend
    obs_frontend_set_current_scene(scene_source);
    
    // Release our reference
    obs_scene_release(scene);
    return true;
}

bool ObsIntegration::ensureBackgroundTarget(const QString &sceneName, const QString &sourceName)
{
    if (!ensureScene(sceneName))
        return false;
    obs_scene_t *scene = getScene(sceneName);
    if (!scene) {
        return false;
    }
    
    // Create scene-specific source name to avoid conflicts between scenes
    QString sceneSpecificName = sceneName + "_" + sourceName;
    
    // Check if the source already exists globally.
    obs_source_t *existing = obs_get_source_by_name(sceneSpecificName.toUtf8().constData());
    
    if (existing) {
        // Source exists, but check if it's in this scene
        obs_sceneitem_t *item = obs_scene_find_source(scene, sceneSpecificName.toUtf8().constData());
        if (!item) {
            // Source exists but not in this scene, add it
            item = obs_scene_add(scene, existing);
            if (item) {
                obs_sceneitem_set_order(item, OBS_ORDER_MOVE_BOTTOM);
            }
        }
        obs_source_release(existing);
        return true;
    }
    
    // Create a blank image source.  Settings require at least the file
    // parameter; we leave it empty so the user sees nothing until they
    // choose a background.
    obs_data_t *settings = obs_data_create();
    obs_data_set_string(settings, "file", "");
    obs_source_t *image = obs_source_create("image_source", sceneSpecificName.toUtf8().constData(), settings, nullptr);
    if (!image) {
        qWarning() << "[Velutan] Failed to create background source" << sceneSpecificName;
        obs_data_release(settings);
        return false;
    }
    // Add it to the scene; move it to the bottom so that it doesn't
    // obscure other items by default.
    obs_sceneitem_t *item = obs_scene_add(scene, image);
    if (item)
        obs_sceneitem_set_order(item, OBS_ORDER_MOVE_BOTTOM);
    // Release temporary references.  The scene now owns the sceneitem.
    obs_source_release(image);
    obs_data_release(settings);
    return true;
}

void ObsIntegration::setBackground(const QString &sceneName, const QString &sourceName, const QString &filePath, bool autoStretch)
{
    try {
        // Get the specified scene
        obs_scene_t *scene = getScene(sceneName);
        if (!scene) {
            qWarning() << "[Velutan] Scene not found:" << sceneName;
            return;
        }
        
        // Use scene-specific source name
        QString sceneSpecificName = sceneName + "_" + sourceName;
        
        // Get the background source by name
        obs_source_t *source = obs_get_source_by_name(sceneSpecificName.toUtf8().constData());
        if (!source) {
            qWarning() << "[Velutan] Background source not found:" << sceneSpecificName;
            return;
        }
        
        obs_data_t *settings = obs_source_get_settings(source);
        if (!settings) {
            obs_source_release(source);
            return;
        }
        obs_data_set_string(settings, "file", filePath.toUtf8().constData());
        
        // If auto-stretch is enabled, scale the image to fill canvas
        if (autoStretch) {
            obs_data_set_bool(settings, "unload", false);
            // Get canvas size
            obs_video_info ovi;
            if (obs_get_video_info(&ovi)) {
                // Find the scene item in the specified scene
                obs_sceneitem_t *item = obs_scene_find_source(scene, sceneSpecificName.toUtf8().constData());
                if (item) {
                    struct vec2 bounds;
                    bounds.x = (float)ovi.base_width;
                    bounds.y = (float)ovi.base_height;
                    obs_sceneitem_set_bounds_type(item, OBS_BOUNDS_STRETCH);
                    obs_sceneitem_set_bounds(item, &bounds);
                }
            }
        }
        
        obs_source_update(source, settings);
        obs_data_release(settings);
        obs_source_release(source);
    } catch (...) {
        blog(LOG_ERROR, "[Velutan] Exception in setBackground");
    }
}

bool ObsIntegration::ensureCharacter(const QString &sceneName, const QString &sourceName, const QString &filePath)
{
    if (!ensureScene(sceneName))
        return false;
    obs_scene_t *scene = getScene(sceneName);
    if (!scene) {
        return false;
    }
    obs_source_t *existing = obs_get_source_by_name(sourceName.toUtf8().constData());
    if (!existing) {
        // Create new image source with the specified file.
        obs_data_t *settings = obs_data_create();
        obs_data_set_string(settings, "file", filePath.toUtf8().constData());
        obs_source_t *image = obs_source_create("image_source", sourceName.toUtf8().constData(), settings, nullptr);
        if (!image) {
            qWarning() << "[Velutan] Failed to create character source" << sourceName;
            obs_data_release(settings);
            return false;
        }
        obs_sceneitem_t *item = obs_scene_add(scene, image);
        if (item) {
            obs_sceneitem_set_visible(item, true);
            obs_sceneitem_set_order(item, OBS_ORDER_MOVE_TOP);
        }
        obs_source_release(image);
        obs_data_release(settings);
        return true;
    } else {
        // Update existing source file and ensure it is visible.
        obs_sceneitem_t *item = findSceneItem(scene, sourceName);
        if (!item) {
            qWarning() << "[Velutan] Could not find scene item for" << sourceName;
            obs_source_release(existing);
            return false;
        }
        obs_sceneitem_set_visible(item, true);
        // Update file path
        obs_data_t *settings = obs_source_get_settings(existing);
        obs_data_set_string(settings, "file", filePath.toUtf8().constData());
        obs_source_update(existing, settings);
        obs_data_release(settings);
        obs_sceneitem_set_order(item, OBS_ORDER_MOVE_TOP);
        obs_source_release(existing);
        return true;
    }
}

void ObsIntegration::toggleCharacter(const QString &sceneName, const QString &sourceName, bool visible)
{
    obs_scene_t *scene = getScene(sceneName);
    if (!scene)
        return;
    obs_sceneitem_t *item = findSceneItem(scene, sourceName);
    if (item) {
        obs_sceneitem_set_visible(item, visible);
    }
}

void ObsIntegration::bringToFront(const QString &sceneName, const QString &sourceName)
{
    obs_scene_t *scene = getScene(sceneName);
    if (!scene)
        return;
    obs_sceneitem_t *item = findSceneItem(scene, sourceName);
    if (item) {
        obs_sceneitem_set_order(item, OBS_ORDER_MOVE_TOP);
    }
}

bool ObsIntegration::isVisible(const QString &sceneName, const QString &sourceName) const
{
    obs_scene_t *scene = const_cast<ObsIntegration*>(this)->getScene(sceneName);
    if (!scene)
        return false;
    obs_sceneitem_t *item = const_cast<ObsIntegration*>(this)->findSceneItem(scene, sourceName);
    if (!item)
        return false;
    return obs_sceneitem_visible(item);
}

void ObsIntegration::bringPinnedToFront(const QString &sceneName, const QStringList &pinnedSourceNames)
{
    if (pinnedSourceNames.isEmpty())
        return;
        
    obs_scene_t *scene = getScene(sceneName);
    if (!scene)
        return;
    
    // Bring each pinned source to the top, in reverse order so the first in
    // the list ends up at the very top
    for (int i = pinnedSourceNames.size() - 1; i >= 0; --i) {
        const QString &sourceName = pinnedSourceNames[i];
        obs_sceneitem_t *item = findSceneItem(scene, sourceName);
        if (item) {
            obs_sceneitem_set_order(item, OBS_ORDER_MOVE_TOP);
        }
    }
}

obs_scene_t *ObsIntegration::getScene(const QString &sceneName)
{
    obs_source_t *source = obs_get_source_by_name(sceneName.toUtf8().constData());
    if (!source)
        return nullptr;
    obs_scene_t *scene = obs_scene_from_source(source);
    obs_source_release(source);
    return scene;
}

obs_sceneitem_t *ObsIntegration::findSceneItem(obs_scene_t *scene, const QString &sourceName)
{
    if (!scene)
        return nullptr;
    // The obs_scene_find_source function returns the scene item associated
    // with a source if present, otherwise nullptr.  In the current OBS API
    // it takes a source name string.
    obs_sceneitem_t *item = obs_scene_find_source(scene, sourceName.toUtf8().constData());
    return item;
}

void ObsIntegration::getCanvasSize(uint32_t &width, uint32_t &height)
{
    // Get OBS video info to determine canvas size
    obs_video_info ovi;
    if (obs_get_video_info(&ovi)) {
        width = ovi.base_width;
        height = ovi.base_height;
    } else {
        // Default fallback
        width = 1920;
        height = 1080;
    }
}

QString ObsIntegration::generateGridImage(uint32_t width, uint32_t height, int gridSize,
                                          const QString &color, int opacity)
{
    // Create a transparent image
    QImage image(width, height, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    
    // Setup painter
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, false);  // Crisp lines
    
    // Parse color and set opacity
    QColor gridColor(color);
    gridColor.setAlpha(opacity);
    
    QPen pen(gridColor);
    pen.setWidth(1);
    painter.setPen(pen);
    
    // Draw vertical lines
    for (uint32_t x = 0; x <= width; x += gridSize) {
        painter.drawLine(x, 0, x, height);
    }
    
    // Draw horizontal lines
    for (uint32_t y = 0; y <= height; y += gridSize) {
        painter.drawLine(0, y, width, y);
    }
    
    painter.end();
    
    // Save to temporary file
    QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    QString gridPath = tempDir + "/velutan_grid_overlay.png";
    
    if (!image.save(gridPath, "PNG")) {
        qWarning() << "[Velutan] Failed to save grid image to" << gridPath;
        return QString();
    }
    
    qDebug() << "[Velutan] Grid image generated:" << gridPath;
    return gridPath;
}

bool ObsIntegration::ensureGridOverlay(const QString &sceneName, const QString &gridImagePath,
                                       bool showInStream)
{
    Q_UNUSED(showInStream);  // For future implementation with private sources
    
    if (!ensureScene(sceneName))
        return false;
        
    obs_scene_t *scene = getScene(sceneName);
    if (!scene)
        return false;
    
    QString gridSourceName = "Velutan_Grid_Overlay";
    
    // Check if grid source already exists
    obs_source_t *existing = obs_get_source_by_name(gridSourceName.toUtf8().constData());
    
    if (existing) {
        // Update the image path
        obs_data_t *settings = obs_source_get_settings(existing);
        obs_data_set_string(settings, "file", gridImagePath.toUtf8().constData());
        obs_source_update(existing, settings);
        obs_data_release(settings);
        
        // Ensure it's in the scene
        obs_sceneitem_t *item = obs_scene_find_source(scene, gridSourceName.toUtf8().constData());
        if (!item) {
            item = obs_scene_add(scene, existing);
            if (item) {
                // Move to top (above everything except pinned sources)
                obs_sceneitem_set_order(item, OBS_ORDER_MOVE_TOP);
                
                // Make it fullscreen
                obs_video_info ovi;
                if (obs_get_video_info(&ovi)) {
                    obs_bounds_type boundsType = OBS_BOUNDS_STRETCH;
                    struct vec2 bounds;
                    bounds.x = ovi.base_width;
                    bounds.y = ovi.base_height;
                    obs_sceneitem_set_bounds_type(item, boundsType);
                    obs_sceneitem_set_bounds(item, &bounds);
                }
            }
        }
        
        obs_source_release(existing);
        return true;
    }
    
    // Create new grid source
    obs_data_t *settings = obs_data_create();
    obs_data_set_string(settings, "file", gridImagePath.toUtf8().constData());
    
    obs_source_t *gridSource = obs_source_create("image_source", gridSourceName.toUtf8().constData(),
                                                  settings, nullptr);
    if (!gridSource) {
        qWarning() << "[Velutan] Failed to create grid overlay source";
        obs_data_release(settings);
        return false;
    }
    
    // Add to scene
    obs_sceneitem_t *item = obs_scene_add(scene, gridSource);
    if (item) {
        // Move to top
        obs_sceneitem_set_order(item, OBS_ORDER_MOVE_TOP);
        
        // Make it fullscreen
        obs_video_info ovi;
        if (obs_get_video_info(&ovi)) {
            obs_bounds_type boundsType = OBS_BOUNDS_STRETCH;
            struct vec2 bounds;
            bounds.x = ovi.base_width;
            bounds.y = ovi.base_height;
            obs_sceneitem_set_bounds_type(item, boundsType);
            obs_sceneitem_set_bounds(item, &bounds);
        }
    }
    
    obs_source_release(gridSource);
    obs_data_release(settings);
    return true;
}

void ObsIntegration::toggleGridOverlay(const QString &sceneName, bool visible)
{
    obs_scene_t *scene = getScene(sceneName);
    if (!scene)
        return;
    
    QString gridSourceName = "Velutan_Grid_Overlay";
    obs_sceneitem_t *item = findSceneItem(scene, gridSourceName);
    
    if (item) {
        obs_sceneitem_set_visible(item, visible);
        
        if (visible) {
            // Ensure it's on top when made visible
            obs_sceneitem_set_order(item, OBS_ORDER_MOVE_TOP);
        }
    }
}

void ObsIntegration::snapSourceToGrid(const QString &sceneName, const QString &sourceName, int gridSize)
{
    obs_scene_t *scene = getScene(sceneName);
    if (!scene)
        return;
    
    obs_sceneitem_t *item = findSceneItem(scene, sourceName);
    if (!item)
        return;
    
    // Get current position
    struct vec2 pos;
    obs_sceneitem_get_pos(item, &pos);
    
    // Snap to nearest grid point
    pos.x = round(pos.x / gridSize) * gridSize;
    pos.y = round(pos.y / gridSize) * gridSize;
    
    // Set snapped position
    obs_sceneitem_set_pos(item, &pos);
}