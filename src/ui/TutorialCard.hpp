#pragma once

#include <QWidget>

/*
 * TutorialCard
 *
 * A small widget shown when the dock is opened for the first time to
 * explain the basic workflow.  The user can dismiss it and choose
 * whether to hide it permanently via a checkbox.  When dismissed the
 * dismissed signal is emitted with the state of the checkbox.
 */

class QCheckBox;
class QPushButton;
class QLabel;

class TutorialCard : public QWidget
{
    Q_OBJECT
public:
    explicit TutorialCard(QWidget *parent = nullptr);
    void setVisibleByConfig(bool dismissed);

signals:
    void dismissed(bool remember);

private slots:
    void onDismissClicked();

private:
    QCheckBox *m_remember;
};
