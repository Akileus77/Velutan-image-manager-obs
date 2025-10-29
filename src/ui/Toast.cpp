#include "Toast.hpp"

#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>

Toast::Toast(QWidget *parent)
    : QWidget(parent), m_label(new QLabel(this)), m_timer(new QTimer(this))
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    // Use ToolTip window flag so the toast floats above other widgets
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    // Styling: dark semi‑transparent background and white text
    setStyleSheet("background-color: rgba(50, 50, 50, 200);"
                  "color: white;"
                  "padding: 6px 12px;"
                  "border-radius: 4px;"
                  "font-size: 12px;");
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_label);
    m_label->setAlignment(Qt::AlignCenter);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &Toast::hideToast);
    hide();
}

void Toast::showMessage(const QString &message, int msec)
{
    m_label->setText(message);
    adjustSize();
    // Position the toast at the bottom centre of its parent if possible
    if (parentWidget()) {
        QSize parentSize = parentWidget()->size();
        move((parentSize.width() - width()) / 2,
             parentSize.height() - height() - 10);
    }
    show();
    raise();
    m_timer->start(msec);
}

void Toast::hideToast()
{
    hide();
}