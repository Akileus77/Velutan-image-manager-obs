#pragma once

#include <QDialog>
#include <QStringList>

/*
 * PinnedSourcesDialog
 *
 * A dialog that allows the user to configure which OBS sources should
 * always stay on top in the scene (e.g., Camera, Player, Webcam).
 * These sources will be automatically brought to the front whenever
 * characters are shown, hidden, or added to ensure they're never
 * obscured by other scene items.
 */

class QListWidget;
class QLineEdit;
class QPushButton;

class PinnedSourcesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PinnedSourcesDialog(const QStringList &pinnedSources, QWidget *parent = nullptr);
    
    QStringList getPinnedSources() const;

private slots:
    void onAddClicked();
    void onRemoveClicked();

private:
    QListWidget *m_listWidget;
    QLineEdit *m_sourceNameEdit;
    QPushButton *m_addBtn;
    QPushButton *m_removeBtn;
};

