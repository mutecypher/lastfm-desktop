#ifndef CLOSEAPPSDIALOG_H
#define CLOSEAPPSDIALOG_H

#include <QDialog>

namespace Ui { class CloseAppsDialog; }
class IPluginInfo;

class CloseAppsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CloseAppsDialog( const QList<IPluginInfo*>& plugins, QWidget *parent );
    explicit CloseAppsDialog(QWidget *parent = 0);
    ~CloseAppsDialog();

private:
    QStringList runningApps();

private slots:
    void checkApps();

private:
    void commonSetup();
    
private:
    Ui::CloseAppsDialog *ui;
    QList<IPluginInfo*> m_plugins;
};

#endif // CLOSEAPPSDIALOG_H
