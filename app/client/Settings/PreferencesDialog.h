#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

#include "lib/unicorn/dialogs/UnicornDialog.h"

namespace Ui {
    class PreferencesDialog;
}

class PreferencesDialog : public unicorn::MainWindow
{
    Q_OBJECT

public:
    explicit PreferencesDialog( QMenuBar* menuBar );
    ~PreferencesDialog();

signals:
    void saveNeeded();

private:
    void closeEvent(QCloseEvent *);

private slots:
    void onTabButtonClicked();

    void onAccepted();
    void onSettingsChanged();
    void onApplyButtonClicked();

private:
    Ui::PreferencesDialog *ui;

    class QToolBar* m_toolBar;
};

#endif // PREFERENCESDIALOG_H
