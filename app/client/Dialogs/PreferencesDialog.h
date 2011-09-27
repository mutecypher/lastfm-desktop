#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

#include "lib/unicorn/dialogs/UnicornDialog.h"

namespace Ui {
    class PreferencesDialog;
}

class PreferencesDialog : public QMainWindow
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = 0);
    ~PreferencesDialog();

signals:
    void saveNeeded();

private slots:
    void onTabButtonClicked();

    void onAccepted();
    void onSettingsChanged();
    void onApplyButtonClicked();

private:
    Ui::PreferencesDialog *ui;
};

#endif // PREFERENCESDIALOG_H
