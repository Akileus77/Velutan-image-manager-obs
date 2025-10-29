#include "setup_dialog.hpp"
#include "theme_constants.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCompleter>
#include <QFormLayout>
#include <QDialogButtonBox>

extern "C" {
#include <obs-module.h>
}

static QString libraryFilePath()
{
    // Derive the library.json path from the app config location.  The
    // same directory is used by persistence.cpp for storing
    // configuration.
    QString baseDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString dir = baseDir + "/velutan-image-manager";
    QDir d;
    if (!d.exists(dir))
        d.mkpath(dir);
    return dir + "/library.json";
}

VelutanSetupDialog::VelutanSetupDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(obs_module_text("Velutan Image Manager (Setup)"));
    resize(500, 400);
    auto *layout = new QVBoxLayout(this);
    m_listWidget = new QListWidget(this);
    layout->addWidget(m_listWidget);
    auto *buttonRow = new QHBoxLayout();
    m_addBtn = new QPushButton(tr("Add Asset"), this);
    connect(m_addBtn, &QPushButton::clicked, this, &VelutanSetupDialog::onAddAsset);
    m_saveBtn = new QPushButton(tr("Save"), this);
    connect(m_saveBtn, &QPushButton::clicked, this, &VelutanSetupDialog::onSaveLibrary);
    buttonRow->addWidget(m_addBtn);
    buttonRow->addWidget(m_saveBtn);
    buttonRow->addStretch(1);
    layout->addLayout(buttonRow);
    loadLibrary();
    refreshList();
}

void VelutanSetupDialog::loadLibrary()
{
    QString path = libraryFilePath();
    QFile f(path);
    if (f.exists()) {
        m_library = AssetLibrary::loadFromFile(path);
        return;
    }
    // Fallback to bundled library in plugin directory
    QString fallback = QCoreApplication::applicationDirPath() + "/data/velutan_library.json";
    m_library = AssetLibrary::loadFromFile(fallback);
}

void VelutanSetupDialog::refreshList()
{
    m_listWidget->clear();
    for (const Asset &a : m_library.backgrounds) {
        QString themeInfo = a.theme.isEmpty() ? "" : QString(" [%1]").arg(a.theme);
        QString text = QString("[B] %1%2 (%3)").arg(a.name, themeInfo, a.tags.join(", "));
        m_listWidget->addItem(text);
    }
    for (const Asset &a : m_library.characters) {
        QString text = QString("[C] %1 (%2)").arg(a.name, a.tags.join(", "));
        m_listWidget->addItem(text);
    }
}

void VelutanSetupDialog::onAddAsset()
{
    // Choose file
    QString file = QFileDialog::getOpenFileName(this, tr("Select Image"), QString(), tr("Image Files (*.png *.jpg *.jpeg *.webp);;All Files (*)"));
    if (file.isEmpty())
        return;
    
    // Category selection first
    bool ok;
    QStringList categories;
    categories << tr("Background") << tr("Character");
    QString category = QInputDialog::getItem(this, tr("Asset Type"), tr("Select the type of asset"), categories, 0, false, &ok);
    if (!ok)
        return;
    
    // Custom dialog for asset details
    QDialog detailsDialog(this);
    detailsDialog.setWindowTitle(tr("Add %1").arg(category));
    detailsDialog.resize(400, 300);
    
    QVBoxLayout *layout = new QVBoxLayout(&detailsDialog);
    QFormLayout *formLayout = new QFormLayout();
    
    // Name field
    QLineEdit *nameEdit = new QLineEdit(&detailsDialog);
    formLayout->addRow(tr("Name:"), nameEdit);
    
    // Tags field
    QLineEdit *tagsEdit = new QLineEdit(&detailsDialog);
    tagsEdit->setPlaceholderText(tr("e.g., nature, outdoor, medieval"));
    formLayout->addRow(tr("Tags:"), tagsEdit);
    
    // Theme field (only for backgrounds)
    QComboBox *themeCombo = nullptr;
    if (category == tr("Background")) {
        themeCombo = new QComboBox(&detailsDialog);
        themeCombo->setEditable(true);  // Allow custom input
        themeCombo->setInsertPolicy(QComboBox::NoInsert);
        themeCombo->addItems(ThemeConstants::getThemes());
        
        // Add completer for search-as-you-type
        QCompleter *completer = new QCompleter(ThemeConstants::getThemes(), themeCombo);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setFilterMode(Qt::MatchContains);
        themeCombo->setCompleter(completer);
        
        formLayout->addRow(tr("Theme:"), themeCombo);
    }
    
    layout->addLayout(formLayout);
    
    // Dialog buttons
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &detailsDialog);
    connect(buttons, &QDialogButtonBox::accepted, &detailsDialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &detailsDialog, &QDialog::reject);
    layout->addWidget(buttons);
    
    if (detailsDialog.exec() != QDialog::Accepted)
        return;
    
    QString name = nameEdit->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Asset name cannot be empty."));
        return;
    }
    
    // Parse tags
    QStringList tags;
    QString tagsString = tagsEdit->text();
    for (const QString &t : tagsString.split(',', Qt::SkipEmptyParts))
        tags << t.trimmed();
    
    // Get theme (only for backgrounds)
    QString theme;
    if (themeCombo) {
        theme = themeCombo->currentText().trimmed();
    }
    
    // Compute id by sanitising the name
    QString id = name.toLower();
    id.replace(" ", "_");
    
    Asset asset;
    asset.id = id;
    asset.name = name;
    asset.file = file;
    asset.tags = tags;
    asset.theme = theme;
    
    if (category == tr("Background")) {
        m_library.backgrounds.append(asset);
    } else {
        m_library.characters.append(asset);
    }
    
    // Auto-save after adding asset
    QString userPath = libraryFilePath();
    if (AssetLibrary::saveToFile(userPath, m_library)) {
        emit libraryChanged();  // Notify dock widget
    }
    
    refreshList();
}

void VelutanSetupDialog::onSaveLibrary()
{
    QString path = libraryFilePath();
    if (AssetLibrary::saveToFile(path, m_library)) {
        emit libraryChanged();  // Notify that library has changed
        QMessageBox::information(this, tr("Saved"), tr("Library saved successfully."));
    } else {
        QMessageBox::warning(this, tr("Error"), tr("Could not save library."));
    }
}