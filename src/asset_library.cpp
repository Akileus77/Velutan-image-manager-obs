#include "asset_library.hpp"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

Library AssetLibrary::loadFromFile(const QString &path)
{
    Library lib;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "[Velutan] Could not open library file" << path;
        return lib;
    }
    QByteArray data = file.readAll();
    file.close();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "[Velutan] Failed to parse library JSON:" << err.errorString();
        return lib;
    }
    QJsonObject root = doc.object();
    auto parseArray = [](const QJsonArray &arr) {
        QVector<Asset> list;
        list.reserve(arr.size());
        for (const QJsonValue &val : arr) {
            QJsonObject obj = val.toObject();
            Asset a;
            a.id = obj.value("id").toString();
            a.name = obj.value("name").toString();
            a.file = obj.value("file").toString();
            a.theme = obj.value("theme").toString();  // Read theme field
            QJsonArray tagsArr = obj.value("tags").toArray();
            for (const QJsonValue &tagVal : tagsArr) {
                a.tags << tagVal.toString();
            }
            list.push_back(a);
        }
        return list;
    };
    if (root.contains("backgrounds")) {
        lib.backgrounds = parseArray(root.value("backgrounds").toArray());
    }
    if (root.contains("characters")) {
        lib.characters = parseArray(root.value("characters").toArray());
    }
    return lib;
}

bool AssetLibrary::saveToFile(const QString &path, const Library &lib)
{
    QJsonObject root;
    auto buildArray = [](const QVector<Asset> &list) {
        QJsonArray arr;
        for (const Asset &a : list) {
            QJsonObject obj;
            obj.insert("id", a.id);
            obj.insert("name", a.name);
            obj.insert("file", a.file);
            if (!a.theme.isEmpty()) {
                obj.insert("theme", a.theme);  // Write theme field if set
            }
            QJsonArray tagsArr;
            for (const QString &t : a.tags)
                tagsArr.append(t);
            obj.insert("tags", tagsArr);
            arr.append(obj);
        }
        return arr;
    };
    root.insert("backgrounds", buildArray(lib.backgrounds));
    root.insert("characters", buildArray(lib.characters));
    QJsonDocument doc(root);
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "[Velutan] Could not write library file" << path;
        return false;
    }
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

QVector<Asset> AssetLibrary::search(const QVector<Asset> &list, const QString &query)
{
    QVector<Asset> result;
    QString q = query.trimmed().toLower();
    if (q.isEmpty()) {
        return list;
    }
    for (const Asset &a : list) {
        QString name = a.name.toLower();
        bool match = name.contains(q);
        // Search in theme
        if (!match && !a.theme.isEmpty()) {
            if (a.theme.toLower().contains(q)) {
                match = true;
            }
        }
        // Search in tags
        if (!match) {
            for (const QString &tag : a.tags) {
                if (tag.toLower().contains(q)) {
                    match = true;
                    break;
                }
            }
        }
        if (match) {
            result.push_back(a);
        }
    }
    return result;
}