#include "obs_integration.hpp"

#include <QDebug>

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