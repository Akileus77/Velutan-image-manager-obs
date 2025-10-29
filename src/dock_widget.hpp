#pragma once

#include <QWidget>

#include "persistence.hpp"
#include "asset_library.hpp"
#include "obs_integration.hpp"

/*
 * VelutanDockWidget
 *
 * The main widget exposed by the plugin as a dock.  It contains a header
 * bar for choosing the scene and naming conventions, a search box,
 * tabs listing background and character assets, a dismissable
 * tutorial and a toast for transient messages.  Most of the UI logic
 * lives here: loading the asset library, applying actions to the
 * running OBS instance and saving user preferences.
 */

class HeaderBar;
class QLineEdit;
class QTabWidget;
class QComboBox;
class AssetList;
class TutorialCard;
class Toast;

class VelutanDockWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VelutanDockWidget(QWidget *parent = nullptr);
    ~VelutanDockWidget();
    
    // Public method to refresh from external changes
    void reloadLibrary();

private slots:
    void refreshLists();
    void onSearchChanged(const QString &text);
    void onFilterChanged();
    void onAddAsset();
    void onSceneChanged(const QString &name);
    void onBgTargetChanged(const QString &name);
    void onOverlayPrefixChanged(const QString &prefix);
    void onAutoSetup();
    void onAssetAction(const Asset &asset, const QString &action);
    void onDismissTutorial(bool remember);
    void onAutoStretchChanged(bool enabled);
    void onPinnedSourcesSettings();
    void onGridToggled(bool enabled);
    void onGridSettings();

private:
    void loadLibrary();
    void loadConfig();
    void saveConfig();
    void updateSceneList();
    void updateFilterLists();
    void autoSetup();

    PersistenceConfig m_config;
    Library m_library;
    ObsIntegration m_obs;

    HeaderBar *m_headerBar;
    QLineEdit *m_searchEdit;
    QComboBox *m_themeFilter;
    QComboBox *m_bgTagFilter;  // Tag filter for backgrounds
    QComboBox *m_charTagFilter;  // Tag filter for characters
    QTabWidget *m_tabs;
    AssetList *m_bgList;
    AssetList *m_charList;
    TutorialCard *m_tutorial;
    Toast *m_toast;
};
