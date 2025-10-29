#pragma once

#include <QDialog>

/*
 * GridSettingsDialog
 *
 * A dialog that allows the user to configure grid system settings:
 * - Grid cell size (auto-calculated based on canvas or manual)
 * - Grid visibility (show in OBS preview and/or stream)
 * - Snap to grid feature
 * - Grid appearance (color, opacity, line width)
 */

class QSpinBox;
class QCheckBox;
class QComboBox;
class QSlider;
class QLabel;

class GridSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit GridSettingsDialog(int gridSize, bool showInStream, bool snapEnabled, 
                                const QString &color, int opacity, QWidget *parent = nullptr);
    
    int getGridSize() const;
    bool getShowInStream() const;
    bool getSnapEnabled() const;
    QString getGridColor() const;
    int getGridOpacity() const;

private slots:
    void onColorPresetChanged(int index);
    void onOpacityChanged(int value);

private:
    QSpinBox *m_gridSizeSpinBox;
    QCheckBox *m_showInStreamCheckbox;
    QCheckBox *m_snapEnabledCheckbox;
    QComboBox *m_colorCombo;
    QSlider *m_opacitySlider;
    QLabel *m_opacityLabel;
    QLabel *m_previewLabel;
};

