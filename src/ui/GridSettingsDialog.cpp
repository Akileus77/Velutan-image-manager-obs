#include "GridSettingsDialog.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QSlider>
#include <QDialogButtonBox>
#include <QPushButton>

GridSettingsDialog::GridSettingsDialog(int gridSize, bool showInStream, bool snapEnabled,
                                       const QString &color, int opacity, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Grid System Settings");
    setMinimumWidth(500);
    
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);
    
    // Info label
    auto *infoLabel = new QLabel(
        "📐 <b>Grid System</b><br>"
        "Configure the grid overlay for precise source positioning and alignment.<br>"
        "<i>Grid helps you align characters and objects perfectly on the scene.</i>",
        this
    );
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet(
        "QLabel { "
        "   color: #E0E0E0; "
        "   padding: 12px; "
        "   background-color: #2D2D30; "
        "   border: 1px solid #3F3F46; "
        "   border-radius: 6px; "
        "   font-size: 11px; "
        "}"
    );
    mainLayout->addWidget(infoLabel);
    
    // Grid size setting
    auto *formLayout = new QFormLayout();
    formLayout->setSpacing(12);
    
    auto *sizeLabel = new QLabel("Grid Cell Size (px):", this);
    sizeLabel->setStyleSheet("QLabel { color: #E0E0E0; font-weight: 500; }");
    
    m_gridSizeSpinBox = new QSpinBox(this);
    m_gridSizeSpinBox->setRange(10, 200);
    m_gridSizeSpinBox->setSingleStep(5);
    m_gridSizeSpinBox->setValue(gridSize);
    m_gridSizeSpinBox->setSuffix(" px");
    m_gridSizeSpinBox->setStyleSheet(
        "QSpinBox { "
        "   background-color: #2D2D30; "
        "   border: 1px solid #3F3F46; "
        "   border-radius: 4px; "
        "   padding: 6px; "
        "   color: #E0E0E0; "
        "   font-size: 11px; "
        "}"
    );
    formLayout->addRow(sizeLabel, m_gridSizeSpinBox);
    
    // Grid color preset
    auto *colorLabel = new QLabel("Grid Color:", this);
    colorLabel->setStyleSheet("QLabel { color: #E0E0E0; font-weight: 500; }");
    
    m_colorCombo = new QComboBox(this);
    m_colorCombo->addItem("🟢 Green", "#00FF00");
    m_colorCombo->addItem("🔵 Blue", "#0080FF");
    m_colorCombo->addItem("🔴 Red", "#FF0000");
    m_colorCombo->addItem("🟡 Yellow", "#FFFF00");
    m_colorCombo->addItem("🟣 Purple", "#FF00FF");
    m_colorCombo->addItem("⚪ White", "#FFFFFF");
    m_colorCombo->addItem("⚫ Gray", "#808080");
    
    // Set current color
    for (int i = 0; i < m_colorCombo->count(); ++i) {
        if (m_colorCombo->itemData(i).toString() == color) {
            m_colorCombo->setCurrentIndex(i);
            break;
        }
    }
    
    m_colorCombo->setStyleSheet(
        "QComboBox { "
        "   background-color: #2D2D30; "
        "   border: 1px solid #3F3F46; "
        "   border-radius: 4px; "
        "   padding: 6px; "
        "   color: #E0E0E0; "
        "   font-size: 11px; "
        "}"
    );
    connect(m_colorCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GridSettingsDialog::onColorPresetChanged);
    formLayout->addRow(colorLabel, m_colorCombo);
    
    // Opacity slider
    auto *opacityLabel = new QLabel("Grid Opacity:", this);
    opacityLabel->setStyleSheet("QLabel { color: #E0E0E0; font-weight: 500; }");
    
    auto *opacityLayout = new QHBoxLayout();
    m_opacitySlider = new QSlider(Qt::Horizontal, this);
    m_opacitySlider->setRange(32, 255);
    m_opacitySlider->setValue(opacity);
    m_opacitySlider->setStyleSheet(
        "QSlider::groove:horizontal { "
        "   background: #2D2D30; "
        "   height: 8px; "
        "   border-radius: 4px; "
        "}"
        "QSlider::handle:horizontal { "
        "   background: #007ACC; "
        "   width: 16px; "
        "   margin: -4px 0; "
        "   border-radius: 8px; "
        "}"
    );
    connect(m_opacitySlider, &QSlider::valueChanged, this, &GridSettingsDialog::onOpacityChanged);
    
    m_opacityLabel = new QLabel(QString::number((opacity * 100) / 255) + "%", this);
    m_opacityLabel->setFixedWidth(50);
    m_opacityLabel->setStyleSheet("QLabel { color: #E0E0E0; }");
    
    opacityLayout->addWidget(m_opacitySlider);
    opacityLayout->addWidget(m_opacityLabel);
    formLayout->addRow(opacityLabel, opacityLayout);
    
    mainLayout->addLayout(formLayout);
    
    // Checkboxes
    auto *checkboxLayout = new QVBoxLayout();
    checkboxLayout->setSpacing(8);
    
    m_showInStreamCheckbox = new QCheckBox("📡 Show grid in stream output (visible to viewers)", this);
    m_showInStreamCheckbox->setChecked(showInStream);
    m_showInStreamCheckbox->setStyleSheet(
        "QCheckBox { "
        "   color: #E0E0E0; "
        "   font-size: 11px; "
        "   spacing: 6px; "
        "}"
        "QCheckBox::indicator { width: 18px; height: 18px; }"
    );
    m_showInStreamCheckbox->setToolTip("If unchecked, grid is only visible in OBS preview, not in stream");
    checkboxLayout->addWidget(m_showInStreamCheckbox);
    
    m_snapEnabledCheckbox = new QCheckBox("🧲 Enable snap to grid (auto-align sources)", this);
    m_snapEnabledCheckbox->setChecked(snapEnabled);
    m_snapEnabledCheckbox->setStyleSheet(
        "QCheckBox { "
        "   color: #E0E0E0; "
        "   font-size: 11px; "
        "   spacing: 6px; "
        "}"
        "QCheckBox::indicator { width: 18px; height: 18px; }"
    );
    m_snapEnabledCheckbox->setToolTip("Sources will automatically align to grid when moved");
    checkboxLayout->addWidget(m_snapEnabledCheckbox);
    
    mainLayout->addLayout(checkboxLayout);
    
    // Preview
    m_previewLabel = new QLabel(this);
    m_previewLabel->setFixedHeight(120);
    m_previewLabel->setStyleSheet(
        "QLabel { "
        "   background-color: #1E1E1E; "
        "   border: 1px solid #3F3F46; "
        "   border-radius: 6px; "
        "}"
    );
    m_previewLabel->setAlignment(Qt::AlignCenter);
    m_previewLabel->setText("Grid Preview\n(Will be implemented with canvas size)");
    m_previewLabel->setStyleSheet(
        "QLabel { "
        "   color: #808080; "
        "   background-color: #1E1E1E; "
        "   border: 1px solid #3F3F46; "
        "   border-radius: 6px; "
        "   font-size: 10px; "
        "}"
    );
    mainLayout->addWidget(m_previewLabel);
    
    // Dialog buttons
    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttonBox->setStyleSheet(
        "QPushButton { "
        "   background-color: #007ACC; "
        "   border: none; "
        "   border-radius: 4px; "
        "   padding: 8px 16px; "
        "   color: white; "
        "   font-weight: 500; "
        "   font-size: 11px; "
        "   min-width: 80px; "
        "}"
        "QPushButton:hover { background-color: #005FA3; }"
        "QPushButton:pressed { background-color: #004578; }"
    );
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
    
    // Apply dark theme
    setStyleSheet("QDialog { background-color: #1E1E1E; }");
}

int GridSettingsDialog::getGridSize() const
{
    return m_gridSizeSpinBox->value();
}

bool GridSettingsDialog::getShowInStream() const
{
    return m_showInStreamCheckbox->isChecked();
}

bool GridSettingsDialog::getSnapEnabled() const
{
    return m_snapEnabledCheckbox->isChecked();
}

QString GridSettingsDialog::getGridColor() const
{
    return m_colorCombo->currentData().toString();
}

int GridSettingsDialog::getGridOpacity() const
{
    return m_opacitySlider->value();
}

void GridSettingsDialog::onColorPresetChanged(int index)
{
    Q_UNUSED(index);
    // Could update preview here
}

void GridSettingsDialog::onOpacityChanged(int value)
{
    int percent = (value * 100) / 255;
    m_opacityLabel->setText(QString::number(percent) + "%");
}

