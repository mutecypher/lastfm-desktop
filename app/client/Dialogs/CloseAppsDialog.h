#ifndef CLOSEAPPSDIALOG_H
#define CLOSEAPPSDIALOG_H

#include <QDialog>

namespace Ui { class CloseAppsDialog; }

class CloseAppsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CloseAppsDialog(QWidget *parent = 0);
    ~CloseAppsDialog();

public:
    static QStringList runningApps();

private slots:
    void checkApps();
    
private:
    Ui::CloseAppsDialog *ui;
};

#endif // CLOSEAPPSDIALOG_H
