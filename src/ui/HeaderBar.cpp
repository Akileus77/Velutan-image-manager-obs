#include "HeaderBar.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>

extern "C" {
#include <obs-module.h>
}

HeaderBar::HeaderBar(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(8);

    // Modern styling for the header bar
    setStyleSheet(
        "QLabel { color: #E0E0E0; font-weight: 500; font-size: 11px; }"
        "QLineEdit { "
        "   background-color: #2D2D30; "
        "   border: 1px solid #3F3F46; "
        "   border-radius: 4px; "
        "   padding: 6px; "
        "   color: #E0E0E0; "
        "   font-size: 11px; "
        "}"
        "QLineEdit:focus { border: 1px solid: #007ACC; }"
        "QComboBox { "
        "   background-color: #2D2D30; "
        "   border: 1px solid #3F3F46; "
        "   border-radius: 4px; "
        "   padding: 6px; "
        "   color: #E0E0E0; "
        "   font-size: 11px; "
        "}"
        "QComboBox::drop-down { border: none; }"
        "QComboBox:hover { border: 1px solid #007ACC; }"
    );

    // Scene selector
    QLabel *sceneLabel = new QLabel(obs_module_text("Scene"), this);
    m_sceneCombo = new QComboBox(this);
    m_sceneCombo->setEditable(false);
    m_sceneCombo->setMinimumWidth(120);
    connect(m_sceneCombo, QOverload<int>::of(&QComboBox::activated),
            this, &HeaderBar::onSceneActivated);

    // Background target source name
    QLabel *bgLabel = new QLabel(obs_module_text("Background Target"), this);
    m_bgEdit = new QLineEdit(this);
    m_bgEdit->setPlaceholderText("BG_Stage");
    m_bgEdit->setMinimumWidth(100);
    connect(m_bgEdit, &QLineEdit::textEdited, this, &HeaderBar::onBgTextEdited);

    // Overlay prefix
    QLabel *prefixLabel = new QLabel(obs_module_text("Overlay Prefix"), this);
    m_prefixEdit = new QLineEdit(this);
    m_prefixEdit->setPlaceholderText("CHAR_");
    m_prefixEdit->setMinimumWidth(80);
    connect(m_prefixEdit, &QLineEdit::textEdited, this, &HeaderBar::onPrefixEdited);

    // Auto-stretch checkbox (for backgrounds)
    m_autoStretchCheckbox = new QCheckBox("🖼️ Auto-Stretch BG", this);
    m_autoStretchCheckbox->setToolTip("Automatically stretch backgrounds to fill the screen");
    m_autoStretchCheckbox->setChecked(true);  // Default on
    m_autoStretchCheckbox->setStyleSheet(
        "QCheckBox { "
        "   color: #E0E0E0; "
        "   font-size: 11px; "
        "   font-weight: 500; "
        "   spacing: 6px; "
        "}"
        "QCheckBox::indicator { "
        "   width: 16px; "
        "   height: 16px; "
        "   border: 2px solid #3F3F46; "
        "   border-radius: 3px; "
        "   background-color: #2D2D30; "
        "}"
        "QCheckBox::indicator:checked { "
        "   background-color: #007ACC; "
        "   border-color: #007ACC; "
        "}"
        "QCheckBox::indicator:hover { "
        "   border-color: #007ACC; "
        "}"
    );
    connect(m_autoStretchCheckbox, &QCheckBox::toggled, this, &HeaderBar::onAutoStretchToggled);
    
    // Auto-Setup button with modern styling
    m_autoButton = new QPushButton(obs_module_text("Auto Setup"), this);
    m_autoButton->setStyleSheet(
        "QPushButton { "
        "   background-color: #007ACC; "
        "   border: none; "
        "   border-radius: 4px; "
        "   padding: 8px 16px; "
        "   color: white; "
        "   font-weight: 500; "
        "   font-size: 11px; "
        "}"
        "QPushButton:hover { background-color: #005FA3; }"
        "QPushButton:pressed { background-color: #004578; }"
    );
    connect(m_autoButton, &QPushButton::clicked, this, &HeaderBar::onAutoSetupClicked);

    // Assemble layout
    layout->addWidget(sceneLabel);
    layout->addWidget(m_sceneCombo, 1);
    layout->addWidget(bgLabel);
    layout->addWidget(m_bgEdit, 1);
    layout->addWidget(prefixLabel);
    layout->addWidget(m_prefixEdit, 1);
    layout->addWidget(m_autoStretchCheckbox);
    layout->addWidget(m_autoButton);
}

void HeaderBar::setSceneList(const QStringList &scenes)
{
    blog(LOG_INFO, "[Velutan] HeaderBar::setSceneList called with %d scenes", scenes.size());
    m_sceneCombo->blockSignals(true);
    m_sceneCombo->clear();
    m_sceneCombo->addItems(scenes);
    m_sceneCombo->blockSignals(false);
    m_sceneCombo->update();
    m_sceneCombo->repaint();
    blog(LOG_INFO, "[Velutan] HeaderBar::setSceneList - ComboBox now has %d items", m_sceneCombo->count());
}

void HeaderBar::setSelectedScene(const QString &name)
{
    blog(LOG_INFO, "[Velutan] HeaderBar::setSelectedScene called with '%s'", name.toUtf8().constData());
    int index = m_sceneCombo->findText(name);
    blog(LOG_INFO, "[Velutan] HeaderBar::setSelectedScene - findText returned index %d", index);
    if (index < 0) {
        // Add the item if it doesn't exist
        m_sceneCombo->addItem(name);
        index = m_sceneCombo->count() - 1;
        blog(LOG_INFO, "[Velutan] HeaderBar::setSelectedScene - Added missing item, new index %d", index);
    }
    m_sceneCombo->setCurrentIndex(index);
    blog(LOG_INFO, "[Velutan] HeaderBar::setSelectedScene - ComboBox current text is '%s'", m_sceneCombo->currentText().toUtf8().constData());
}

void HeaderBar::setBgTargetName(const QString &name)
{
    m_bgEdit->setText(name);
}

void HeaderBar::setOverlayPrefix(const QString &prefix)
{
    m_prefixEdit->setText(prefix);
}

void HeaderBar::setAutoStretch(bool enabled)
{
    m_autoStretchCheckbox->setChecked(enabled);
}

void HeaderBar::onSceneActivated(int index)
{
    Q_UNUSED(index);
    emit sceneChanged(m_sceneCombo->currentText());
}

void HeaderBar::onBgTextEdited(const QString &text)
{
    emit bgTargetChanged(text);
}

void HeaderBar::onPrefixEdited(const QString &text)
{
    emit overlayPrefixChanged(text);
}

void HeaderBar::onAutoSetupClicked()
{
    emit autoSetupRequested();
}

void HeaderBar::onAutoStretchToggled(bool checked)
{
    emit autoStretchChanged(checked);
}