#pragma once

#include <QWidget>

/*
 * Toast
 *
 * A small widget used to display transient messages to the user.  The
 * toast appears on top of its parent and disappears automatically
 * after a timeout.  It is implemented as a semi‑transparent frame
 * containing a label.
 */

class QLabel;
class QTimer;

class Toast : public QWidget
{
    Q_OBJECT
public:
    explicit Toast(QWidget *parent = nullptr);

    /** Show the toast with the given message for the specified
     * duration in milliseconds. */
    void showMessage(const QString &message, int msec = 3000);

private slots:
    void hideToast();

private:
    QLabel *m_label;
    QTimer *m_timer;
};
