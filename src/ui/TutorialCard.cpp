#include "TutorialCard.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>

extern "C" {
#include <obs-module.h>
}

TutorialCard::TutorialCard(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet("background-color: #f0f4ff; border: 1px solid #a3baff;"
                  "border-radius: 4px; padding: 8px;");
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(4);
    QLabel *title = new QLabel(QString("<b>%1</b>").arg(obs_module_text("WelcomeMessage")), this);
    title->setWordWrap(true);
    layout->addWidget(title);
    m_remember = new QCheckBox(obs_module_text("DismissTutorial"), this);
    layout->addWidget(m_remember);
    // Buttons row
    auto *buttonRow = new QHBoxLayout();
    buttonRow->addStretch(1);
    QPushButton *dismissBtn = new QPushButton(tr("Close"), this);
    connect(dismissBtn, &QPushButton::clicked, this, &TutorialCard::onDismissClicked);
    buttonRow->addWidget(dismissBtn);
    layout->addLayout(buttonRow);
    hide();
}

void TutorialCard::setVisibleByConfig(bool dismissed)
{
    setVisible(!dismissed);
}

void TutorialCard::onDismissClicked()
{
    bool remember = m_remember->isChecked();
    hide();
    emit dismissed(remember);
}