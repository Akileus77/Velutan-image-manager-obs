#pragma once

#include <QString>
#include <QStringList>
#include <QVector>

/*
 * asset_library.hpp
 *
 * Defines simple data structures representing background and character
 * assets as loaded from a JSON catalogue.  The AssetLibrary helper
 * class provides static functions for loading and saving the library as
 * well as performing case‑insensitive searches through lists of assets.
 */

struct Asset {
    QString id;
    QString name;
    QString file;
    QStringList tags;
    QString theme;  // For backgrounds: Desert, Forest, Mountain, etc.
};

struct Library {
    QVector<Asset> backgrounds;
    QVector<Asset> characters;
};

class AssetLibrary
{
public:
    /**
     * Load an asset library from a JSON file.  If the file cannot be
     * opened or parsed the returned library will be empty.
     *
     * @param path Path to the JSON file
     * @return Parsed library
     */
    static Library loadFromFile(const QString &path);

    /**
     * Save an asset library back to disk.  Returns true on success.
     *
     * @param path Path to write the JSON file
     * @param lib  Library to save
     * @return Success flag
     */
    static bool saveToFile(const QString &path, const Library &lib);

    /**
     * Search through a list of assets.  The search is case‑insensitive and
     * matches if the query is a substring of the asset name or any tag.
     *
     * @param list  Vector of assets to search
     * @param query Search string
     * @return Matching assets
     */
    static QVector<Asset> search(const QVector<Asset> &list, const QString &query);
};
