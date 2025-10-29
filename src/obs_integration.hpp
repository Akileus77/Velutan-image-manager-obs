#pragma once

#include <QObject>
#include <QString>

/*
 * obs_integration.hpp
 *
 * A thin wrapper around libobs and the OBS frontend API used by the
 * Velutan Image Manager.  This class provides helper methods to
 * create scenes, manage image sources for backgrounds and characters,
 * toggle their visibility and adjust ordering.  All methods should be
 * invoked from OBS's main thread.  See obs_integration.cpp for
 * implementation details and comments on thread safety.
 */

extern "C" {
#include <obs.h>
#include <obs-frontend-api.h>
}

class ObsIntegration : public QObject
{
    Q_OBJECT
public:
    explicit ObsIntegration(QObject *parent = nullptr);
    ~ObsIntegration();

    /** Ensure that a scene with the given name exists.  If the scene
     * cannot be found it will be created.  Returns true if the scene
     * exists after the call. */
    bool ensureScene(const QString &sceneName);

    /** Ensure that an image source for the background exists within
     * the specified scene.  If the source does not exist it will be
     * created with an empty file path. */
    bool ensureBackgroundTarget(const QString &sceneName, const QString &sourceName);

    /** Set the file path of the given image source in the specified scene.
     * The source must exist. If autoStretch is true, the source will be
     * scaled to fill the canvas. */
    void setBackground(const QString &sceneName, const QString &sourceName, const QString &filePath, bool autoStretch = true);

    /** Ensure that a character image source exists in the specified
     * scene.  If the source is not present it will be created with the
     * provided file path and made visible; otherwise its file will be
     * updated and it will be unhidden. */
    bool ensureCharacter(const QString &sceneName, const QString &sourceName, const QString &filePath);

    /** Toggle the visibility of the character image source.  The
     * source must reside in the given scene. */
    void toggleCharacter(const QString &sceneName, const QString &sourceName, bool visible);

    /** Bring the character image source to the top of the scene's
     * ordering. */
    void bringToFront(const QString &sceneName, const QString &sourceName);

    /** Return true if the given source is visible in the specified
     * scene.  If the source does not exist the return value is
     * false. */
    bool isVisible(const QString &sceneName, const QString &sourceName) const;
    
    /** Bring pinned sources (e.g., Camera, Player) to the front of the
     * scene, ensuring they always stay on top regardless of other source
     * additions or changes. */
    void bringPinnedToFront(const QString &sceneName, const QStringList &pinnedSourceNames);
    
    /** Get canvas (base) resolution from OBS */
    void getCanvasSize(uint32_t &width, uint32_t &height);
    
    /** Generate a grid overlay image and save it to a temporary file.
     * Returns the path to the generated image. */
    QString generateGridImage(uint32_t width, uint32_t height, int gridSize, 
                              const QString &color, int opacity);
    
    /** Ensure that a grid overlay source exists in the specified scene.
     * If showInStream is false, the source will be on a separate layer
     * that's only visible in OBS preview. */
    bool ensureGridOverlay(const QString &sceneName, const QString &gridImagePath, 
                           bool showInStream);
    
    /** Toggle grid overlay visibility */
    void toggleGridOverlay(const QString &sceneName, bool visible);
    
    /** Snap a source to grid alignment */
    void snapSourceToGrid(const QString &sceneName, const QString &sourceName, int gridSize);

private:
    obs_scene_t *getScene(const QString &sceneName);
    obs_sceneitem_t *findSceneItem(obs_scene_t *scene, const QString &sourceName);
};
