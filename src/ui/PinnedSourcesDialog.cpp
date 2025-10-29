#include "PinnedSourcesDialog.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>

PinnedSourcesDialog::PinnedSourcesDialog(const QStringList &pinnedSources, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Pinned Sources Settings");
    setMinimumWidth(450);
    setMinimumHeight(400);
    
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    
    // Info label
    auto *infoLabel = new QLabel(
        "📌 <b>Sources that always stay on top</b><br>"
        "These sources will never be obscured by characters or other scene items.<br>"
        "<i>Examples: Camera, Player, Webcam, Overlay</i>",
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
    
    // List of pinned sources
    auto *listLabel = new QLabel("Current pinned sources:", this);
    listLabel->setStyleSheet("QLabel { color: #E0E0E0; font-weight: 500; font-size: 12px; }");
    mainLayout->addWidget(listLabel);
    
    m_listWidget = new QListWidget(this);
    m_listWidget->addItems(pinnedSources);
    m_listWidget->setStyleSheet(
        "QListWidget { "
        "   background-color: #1E1E1E; "
        "   border: 1px solid #3F3F46; "
        "   border-radius: 4px; "
        "   color: #E0E0E0; "
        "   padding: 4px; "
        "   font-size: 11px; "
        "}"
        "QListWidget::item { "
        "   padding: 8px; "
        "   border-radius: 3px; "
        "}"
        "QListWidget::item:selected { "
        "   background-color: #007ACC; "
        "}"
        "QListWidget::item:hover { "
        "   background-color: #2D2D30; "
        "}"
    );
    mainLayout->addWidget(m_listWidget, 1);
    
    // Add new source section
    auto *addLayout = new QHBoxLayout();
    addLayout->setSpacing(8);
    
    m_sourceNameEdit = new QLineEdit(this);
    m_sourceNameEdit->setPlaceholderText("Enter source name (e.g., Camera)");
    m_sourceNameEdit->setStyleSheet(
        "QLineEdit { "
        "   background-color: #2D2D30; "
        "   border: 1px solid #3F3F46; "
        "   border-radius: 4px; "
        "   padding: 8px; "
        "   color: #E0E0E0; "
        "   font-size: 11px; "
        "}"
        "QLineEdit:focus { border: 1px solid #007ACC; }"
    );
    connect(m_sourceNameEdit, &QLineEdit::returnPressed, this, &PinnedSourcesDialog::onAddClicked);
    addLayout->addWidget(m_sourceNameEdit, 1);
    
    m_addBtn = new QPushButton("➕ Add", this);
    m_addBtn->setStyleSheet(
        "QPushButton { "
        "   background-color: #28A745; "
        "   border: none; "
        "   border-radius: 4px; "
        "   padding: 8px 16px; "
        "   color: white; "
        "   font-weight: 500; "
        "   font-size: 11px; "
        "}"
        "QPushButton:hover { background-color: #218838; }"
        "QPushButton:pressed { background-color: #1E7E34; }"
    );
    connect(m_addBtn, &QPushButton::clicked, this, &PinnedSourcesDialog::onAddClicked);
    addLayout->addWidget(m_addBtn);
    
    m_removeBtn = new QPushButton("🗑️ Remove", this);
    m_removeBtn->setStyleSheet(
        "QPushButton { "
        "   background-color: #DC3545; "
        "   border: none; "
        "   border-radius: 4px; "
        "   padding: 8px 16px; "
        "   color: white; "
        "   font-weight: 500; "
        "   font-size: 11px; "
        "}"
        "QPushButton:hover { background-color: #C82333; }"
        "QPushButton:pressed { background-color: #BD2130; }"
    );
    connect(m_removeBtn, &QPushButton::clicked, this, &PinnedSourcesDialog::onRemoveClicked);
    addLayout->addWidget(m_removeBtn);
    
    mainLayout->addLayout(addLayout);
    
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
    
    // Apply dark theme to the dialog
    setStyleSheet(
        "QDialog { "
        "   background-color: #1E1E1E; "
        "}"
    );
}

QStringList PinnedSourcesDialog::getPinnedSources() const
{
    QStringList sources;
    for (int i = 0; i < m_listWidget->count(); ++i) {
        sources << m_listWidget->item(i)->text();
    }
    return sources;
}

void PinnedSourcesDialog::onAddClicked()
{
    QString sourceName = m_sourceNameEdit->text().trimmed();
    if (sourceName.isEmpty())
        return;
    
    // Check if already exists
    for (int i = 0; i < m_listWidget->count(); ++i) {
        if (m_listWidget->item(i)->text() == sourceName) {
            m_sourceNameEdit->clear();
            return;
        }
    }
    
    // Add to list
    m_listWidget->addItem(sourceName);
    m_sourceNameEdit->clear();
    m_sourceNameEdit->setFocus();
}

void PinnedSourcesDialog::onRemoveClicked()
{
    auto *current = m_listWidget->currentItem();
    if (current) {
        delete current;
    }
}

