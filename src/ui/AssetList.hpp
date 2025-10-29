#pragma once

#include <QWidget>
#include <QVector>
#include "asset_library.hpp"

/*
 * AssetList
 *
 * Displays a list of assets (either backgrounds or characters) in a
 * scrollable view with action buttons.  Each row shows the asset name,
 * its tags and one or more buttons depending on whether the list
 * represents backgrounds or characters.  When the user presses a
 * button the assetActionTriggered signal is emitted with an action
 * string: "set" for backgrounds or "toggle" / "front" for
 * characters.
 */

class QListWidget;
class QListWidgetItem;

class AssetList : public QWidget
{
    Q_OBJECT
public:
    explicit AssetList(bool backgrounds, QWidget *parent = nullptr);

    /** Replace the contents of the list with the provided assets. */
    void setAssets(const QVector<Asset> &assets);
    
    /** Set which assets are currently active (visible in scene) */
    void setActiveAssets(const QStringList &activeNames);

signals:
    /**
     * Emitted when an action button is pressed on an asset.  The
     * action string is one of:
     *  - "set": set asset as background
     *  - "toggle": add/show/hide character
     *  - "front": bring character to front
     */
    void assetActionTriggered(const Asset &asset, const QString &action);

private:
    bool m_isBackgroundList;
    QListWidget *m_listWidget;
    QStringList m_activeAssets;
};
