#pragma once

#include <QWidget>

/*
 * HeaderBar
 *
 * This widget appears at the top of the Velutan dock.  It provides
 * controls for choosing which scene to operate on, specifying the
 * name of the image source used for backgrounds, selecting a prefix
 * used when creating character sources and executing the automatic
 * setup routine.  Signals are emitted whenever the user changes
 * values or triggers setup.
 */

class QComboBox;
class QLineEdit;
class QPushButton;
class QCheckBox;

class HeaderBar : public QWidget
{
    Q_OBJECT
public:
    explicit HeaderBar(QWidget *parent = nullptr);

    void setSceneList(const QStringList &scenes);
    void setSelectedScene(const QString &name);
    void setBgTargetName(const QString &name);
    void setOverlayPrefix(const QString &prefix);
    void setAutoStretch(bool enabled);
    void setGridEnabled(bool enabled);

signals:
    void sceneChanged(const QString &name);
    void bgTargetChanged(const QString &name);
    void overlayPrefixChanged(const QString &prefix);
    void autoSetupRequested();
    void autoStretchChanged(bool enabled);
    void addAssetRequested();
    void pinnedSourcesSettingsRequested();
    void gridToggled(bool enabled);
    void gridSettingsRequested();

private slots:
    void onSceneActivated(int index);
    void onBgTextEdited(const QString &text);
    void onPrefixEdited(const QString &text);
    void onAutoSetupClicked();
    void onAutoStretchToggled(bool checked);

private:
    QComboBox *m_sceneCombo;
    QLineEdit *m_bgEdit;
    QLineEdit *m_prefixEdit;
    QPushButton *m_autoButton;
    QCheckBox *m_autoStretchCheckbox;
    QCheckBox *m_gridCheckbox;
};
