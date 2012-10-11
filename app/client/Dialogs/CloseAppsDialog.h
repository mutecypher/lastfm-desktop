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

    static bool isITunesRunning();

private:
#ifndef Q_OS_MAC
    static QStringList runningApps( const QList<IPluginInfo*>& plugins );
#else
    static // this method is only statis on mac
#endif
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
