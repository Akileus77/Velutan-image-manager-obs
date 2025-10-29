#include "AssetList.hpp"

#include <QListWidget>
#include <QListWidgetItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QPixmap>
#include <QFileInfo>

extern "C" {
#include <obs-module.h>
}

AssetList::AssetList(bool backgrounds, QWidget *parent)
    : QWidget(parent), m_isBackgroundList(backgrounds)
{
    m_listWidget = new QListWidget(this);
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_listWidget);
    m_listWidget->setSelectionMode(QAbstractItemView::NoSelection);
    m_listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    // Modern list styling
    m_listWidget->setStyleSheet(
        "QListWidget { "
        "   background-color: #1E1E1E; "
        "   border: 1px solid #3F3F46; "
        "   border-radius: 4px; "
        "   outline: none; "
        "}"
        "QListWidget::item { "
        "   border-bottom: 1px solid #2D2D30; "
        "   padding: 4px; "
        "}"
        "QListWidget::item:hover { background-color: #2D2D30; }"
    );
}

void AssetList::setActiveAssets(const QStringList &activeNames)
{
    m_activeAssets = activeNames;
    // Note: caller should refresh the list after updating active assets
}

void AssetList::setAssets(const QVector<Asset> &assets)
{
    m_listWidget->clear();
    
    // Active button style (orange for active characters)
    QString activeBtnStyle = 
        "QPushButton { "
        "   background-color: #FF8C00; "
        "   border: none; "
        "   border-radius: 4px; "
        "   padding: 6px 12px; "
        "   color: white; "
        "   font-weight: 500; "
        "   font-size: 10px; "
        "   min-width: 80px; "
        "}"
        "QPushButton:hover { background-color: #FF7700; }"
        "QPushButton:pressed { background-color: #E67300; }";
    
    // Primary button style (green for inactive)
    QString primaryBtnStyle = 
        "QPushButton { "
        "   background-color: #28A745; "
        "   border: none; "
        "   border-radius: 4px; "
        "   padding: 6px 12px; "
        "   color: white; "
        "   font-weight: 500; "
        "   font-size: 10px; "
        "   min-width: 80px; "
        "}"
        "QPushButton:hover { background-color: #218838; }"
        "QPushButton:pressed { background-color: #1E7E34; }";
    
    QString secondaryBtnStyle = 
        "QPushButton { "
        "   background-color: #6C757D; "
        "   border: none; "
        "   border-radius: 4px; "
        "   padding: 6px 12px; "
        "   color: white; "
        "   font-weight: 500; "
        "   font-size: 10px; "
        "   min-width: 60px; "
        "}"
        "QPushButton:hover { background-color: #5A6268; }"
        "QPushButton:pressed { background-color: #545B62; }";
    
    // Sort assets: put active ones first
    QVector<Asset> sortedAssets = assets;
    
    if (m_isBackgroundList && !m_activeAssets.isEmpty()) {
        // For backgrounds, move active background to top
        QString activeId = m_activeAssets.first();
        for (int i = 0; i < sortedAssets.size(); i++) {
            if (sortedAssets[i].id == activeId) {
                Asset activeAsset = sortedAssets.takeAt(i);
                sortedAssets.prepend(activeAsset);
                break;
            }
        }
    } else if (!m_isBackgroundList && !m_activeAssets.isEmpty()) {
        // For characters, move all active characters to top (in order)
        QVector<Asset> activeChars;
        QVector<Asset> inactiveChars;
        
        for (const Asset &asset : sortedAssets) {
            if (m_activeAssets.contains(asset.name)) {
                activeChars.append(asset);
            } else {
                inactiveChars.append(asset);
            }
        }
        
        // Combine: active first, then inactive
        sortedAssets = activeChars + inactiveChars;
    }
    
    for (const Asset &asset : sortedAssets) {
        auto *item = new QListWidgetItem(m_listWidget);
        // Use a QWidget as the item widget to allow layout and buttons.
        QWidget *row = new QWidget(m_listWidget);
        
        // Check if this asset is active
        bool isActiveBackground = m_isBackgroundList && !m_activeAssets.isEmpty() && m_activeAssets.first() == asset.id;
        bool isActiveCharacter = !m_isBackgroundList && m_activeAssets.contains(asset.name);
        bool isActive = isActiveBackground || isActiveCharacter;
        
        // Different styling for active assets
        if (isActive) {
            row->setStyleSheet(
                "QWidget { "
                "   background-color: #1A3A1A; "  // Dark green background for active
                "   border: 2px solid #28A745; "  // Green border for active
                "   border-radius: 8px; "
                "   padding: 8px; "
                "}"
                "QWidget:hover { "
                "   background-color: #1F4A1F; "
                "   border-color: #34D058; "
                "}"
            );
        } else {
            row->setStyleSheet(
                "QWidget { "
                "   background-color: #252526; "
                "   border: 1px solid #3F3F46; "
                "   border-radius: 8px; "
                "   padding: 8px; "
                "}"
                "QWidget:hover { "
                "   background-color: #2D2D30; "
                "   border-color: #007ACC; "
                "}"
            );
        }
        
        auto *hl = new QHBoxLayout(row);
        hl->setContentsMargins(8, 6, 8, 6);
        hl->setSpacing(12);
        
        // Thumbnail preview (80x80 pixels - larger for better visibility)
        QLabel *thumbnailLabel = new QLabel(row);
        thumbnailLabel->setFixedSize(80, 80);
        thumbnailLabel->setScaledContents(false);
        thumbnailLabel->setAlignment(Qt::AlignCenter);
        thumbnailLabel->setStyleSheet(
            "QLabel { "
            "   background-color: #1E1E1E; "
            "   border: 2px solid #3F3F46; "
            "   border-radius: 8px; "
            "}"
        );
        
        // Try to load thumbnail
        QPixmap thumbnail(asset.file);
        if (!thumbnail.isNull()) {
            thumbnailLabel->setPixmap(thumbnail.scaled(76, 76, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            thumbnailLabel->setText("📷");
            thumbnailLabel->setStyleSheet(thumbnailLabel->styleSheet() + "font-size: 32px;");
        }
        hl->addWidget(thumbnailLabel);
        
        // Info container (name + tags)
        auto *infoLayout = new QVBoxLayout();
        infoLayout->setSpacing(2);
        
        // Asset name with better styling
        QLabel *nameLabel = new QLabel(asset.name, row);
        nameLabel->setStyleSheet(
            "color: #FFFFFF; "
            "font-weight: 600; "
            "font-size: 13px;"
        );
        infoLayout->addWidget(nameLabel);
        
        // Theme with badge-like appearance (for backgrounds)
        if (m_isBackgroundList && !asset.theme.isEmpty()) {
            QLabel *themeLabel = new QLabel("🌍 " + asset.theme, row);
            themeLabel->setStyleSheet(
                "color: #60A5FA; "
                "font-size: 10px; "
                "font-weight: 600;"
            );
            infoLayout->addWidget(themeLabel);
        }
        
        // Tags with modern badge-like appearance
        if (!asset.tags.isEmpty()) {
            QLabel *tagsLabel = new QLabel("🏷 " + asset.tags.join(", "), row);
            tagsLabel->setStyleSheet(
                "color: #9CA3AF; "
                "font-size: 10px; "
                "font-style: italic;"
            );
            infoLayout->addWidget(tagsLabel);
        }
        
        hl->addLayout(infoLayout, 1);
        hl->addStretch(1);
        
        if (m_isBackgroundList) {
            // Backgrounds: set, edit, and delete buttons
            QString setBtnText = isActiveBackground ? "✓ Active" : "Set";
            QPushButton *setBtn = new QPushButton(setBtnText, row);
            
            // Active background gets blue/green style, inactive gets primary style
            if (isActiveBackground) {
                setBtn->setStyleSheet(
                    "QPushButton { "
                    "   background-color: #28A745; "  // Green for active
                    "   border: none; "
                    "   border-radius: 4px; "
                    "   padding: 6px 12px; "
                    "   color: white; "
                    "   font-weight: 600; "
                    "   font-size: 10px; "
                    "   min-width: 60px; "
                    "}"
                    "QPushButton:hover { background-color: #218838; }"
                    "QPushButton:pressed { background-color: #1E7E34; }"
                );
            } else {
                setBtn->setStyleSheet(primaryBtnStyle);
            }
            
            setBtn->setCursor(Qt::PointingHandCursor);
            connect(setBtn, &QPushButton::clicked, this, [this, asset]() {
                emit assetActionTriggered(asset, QStringLiteral("set"));
            });
            hl->addWidget(setBtn);
            
            // Edit button for backgrounds
            QPushButton *editBtn = new QPushButton("✏", row);
            editBtn->setToolTip("Edit asset details");
            editBtn->setStyleSheet(
                "QPushButton { "
                "   background-color: #007ACC; "
                "   border: none; "
                "   border-radius: 4px; "
                "   padding: 6px 10px; "
                "   color: white; "
                "   font-weight: 500; "
                "   font-size: 10px; "
                "   min-width: 32px; "
                "}"
                "QPushButton:hover { background-color: #005FA3; }"
                "QPushButton:pressed { background-color: #004578; }"
            );
            editBtn->setCursor(Qt::PointingHandCursor);
            connect(editBtn, &QPushButton::clicked, this, [this, asset]() {
                emit assetActionTriggered(asset, QStringLiteral("edit"));
            });
            hl->addWidget(editBtn);
            
            // Delete button for backgrounds
            QPushButton *delBtn = new QPushButton("🗑", row);
            delBtn->setToolTip("Delete from library");
            delBtn->setStyleSheet(
                "QPushButton { "
                "   background-color: #DC3545; "
                "   border: none; "
                "   border-radius: 4px; "
                "   padding: 6px 10px; "
                "   color: white; "
                "   font-weight: 500; "
                "   font-size: 10px; "
                "   min-width: 32px; "
                "}"
                "QPushButton:hover { background-color: #C82333; }"
                "QPushButton:pressed { background-color: #BD2130; }"
            );
            delBtn->setCursor(Qt::PointingHandCursor);
            connect(delBtn, &QPushButton::clicked, this, [this, asset]() {
                emit assetActionTriggered(asset, QStringLiteral("delete"));
            });
            hl->addWidget(delBtn);
        } else {
            // Characters: toggle and bring to front buttons
            // Use orange color if character is active
            QString toggleBtnText = isActiveCharacter ? "👁 " + QString(obs_module_text("Hide")) : "👤 " + QString(obs_module_text("Show"));
            QPushButton *toggleBtn = new QPushButton(toggleBtnText, row);
            toggleBtn->setStyleSheet(isActiveCharacter ? activeBtnStyle : primaryBtnStyle);
            toggleBtn->setCursor(Qt::PointingHandCursor);
            connect(toggleBtn, &QPushButton::clicked, this, [this, asset]() {
                emit assetActionTriggered(asset, QStringLiteral("toggle"));
            });
            hl->addWidget(toggleBtn);
            
            // Edit button for characters
            QPushButton *editBtn = new QPushButton("✏", row);
            editBtn->setToolTip("Edit asset details");
            editBtn->setStyleSheet(
                "QPushButton { "
                "   background-color: #007ACC; "
                "   border: none; "
                "   border-radius: 4px; "
                "   padding: 6px 10px; "
                "   color: white; "
                "   font-weight: 500; "
                "   font-size: 10px; "
                "   min-width: 32px; "
                "}"
                "QPushButton:hover { background-color: #005FA3; }"
                "QPushButton:pressed { background-color: #004578; }"
            );
            editBtn->setCursor(Qt::PointingHandCursor);
            connect(editBtn, &QPushButton::clicked, this, [this, asset]() {
                emit assetActionTriggered(asset, QStringLiteral("edit"));
            });
            hl->addWidget(editBtn);
            
            // Only show "Bring to Front" and "Remove from Scene" buttons if character is active
            if (isActiveCharacter) {
                QPushButton *frontBtn = new QPushButton("⬆ " + QString(obs_module_text("Front")), row);
                frontBtn->setStyleSheet(secondaryBtnStyle);
                frontBtn->setCursor(Qt::PointingHandCursor);
                connect(frontBtn, &QPushButton::clicked, this, [this, asset]() {
                    emit assetActionTriggered(asset, QStringLiteral("front"));
                });
                hl->addWidget(frontBtn);
                
                // Remove from Scene button
                QPushButton *removeBtn = new QPushButton("❌", row);
                removeBtn->setToolTip("Remove from scene");
                removeBtn->setStyleSheet(
                    "QPushButton { "
                    "   background-color: #FFC107; "
                    "   border: none; "
                    "   border-radius: 4px; "
                    "   padding: 6px 10px; "
                    "   color: white; "
                    "   font-weight: 500; "
                    "   font-size: 10px; "
                    "   min-width: 32px; "
                    "}"
                    "QPushButton:hover { background-color: #E0A800; }"
                    "QPushButton:pressed { background-color: #D39E00; }"
                );
                removeBtn->setCursor(Qt::PointingHandCursor);
                connect(removeBtn, &QPushButton::clicked, this, [this, asset]() {
                    emit assetActionTriggered(asset, QStringLiteral("remove"));
                });
                hl->addWidget(removeBtn);
            }
            
            // Delete button for characters
            QPushButton *delBtn = new QPushButton("🗑", row);
            delBtn->setToolTip("Delete from library");
            delBtn->setStyleSheet(
                "QPushButton { "
                "   background-color: #DC3545; "
                "   border: none; "
                "   border-radius: 4px; "
                "   padding: 6px 10px; "
                "   color: white; "
                "   font-weight: 500; "
                "   font-size: 10px; "
                "   min-width: 32px; "
                "}"
                "QPushButton:hover { background-color: #C82333; }"
                "QPushButton:pressed { background-color: #BD2130; }"
            );
            delBtn->setCursor(Qt::PointingHandCursor);
            connect(delBtn, &QPushButton::clicked, this, [this, asset]() {
                emit assetActionTriggered(asset, QStringLiteral("delete"));
            });
            hl->addWidget(delBtn);
        }
        
        item->setSizeHint(row->sizeHint());
        m_listWidget->setItemWidget(item, row);
    }
}