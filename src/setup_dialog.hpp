#pragma once

#include <QDialog>

#include "asset_library.hpp"

/*
 * VelutanSetupDialog
 *
 * A simple dialog shown from the Tools menu.  It allows the user to
 * inspect their current asset library, add new assets and save
 * modifications.  For brevity this implementation supports only
 * adding new entries; removing or editing existing entries could be
 * added in the future.
 */

class QListWidget;
class QPushButton;

class VelutanSetupDialog : public QDialog
{
    Q_OBJECT
public:
    explicit VelutanSetupDialog(QWidget *parent = nullptr);

signals:
    void libraryChanged();

private slots:
    void onAddAsset();
    void onSaveLibrary();

private:
    void loadLibrary();
    void refreshList();

    Library m_library;
    QListWidget *m_listWidget;
    QPushButton *m_addBtn;
    QPushButton *m_saveBtn;
};
