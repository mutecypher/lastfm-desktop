#include <QTimer>
#include <QPushButton>
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#include <psapi.h>
#endif

#ifndef Q_OS_MAC
#include "../plugins/ITunesPluginInfo.h"
#endif

#include "CloseAppsDialog.h"
#include "ui_CloseAppsDialog.h"


unicorn::CloseAppsDialog::CloseAppsDialog( const QList<IPluginInfo*>& plugins, QWidget *parent )
    :QDialog( parent ),
      ui(new Ui::CloseAppsDialog),
      m_plugins( plugins ),
      m_ownsPlugins( false )
{
    commonSetup();
}

unicorn::CloseAppsDialog::CloseAppsDialog(QWidget *parent) :
    QDialog( parent ),
    ui(new Ui::CloseAppsDialog)
{
    commonSetup();
}

void
unicorn::CloseAppsDialog::setTitle( const QString& title )
{
    setWindowTitle( title );
}

void
unicorn::CloseAppsDialog::setDescription( const QString& description )
{
    ui->text->setText( description );
    ui->text->adjustSize();
    adjustSize();
}

void
unicorn::CloseAppsDialog::commonSetup()
{
    ui->setupUi(this);

    ui->text->setText( tr( "Please close the following apps to continue." ) );

    if ( runningApps().count() == 0 )
        hide();

    checkApps();

    QTimer* timer = new QTimer( this );
    connect( timer, SIGNAL(timeout()), SLOT(checkApps()) );
    timer->setInterval( 1000 );
    timer->start();
}

void
unicorn::CloseAppsDialog::setOwnsPlugins( bool ownsPlugins )
{
    m_ownsPlugins = ownsPlugins;
}

bool
unicorn::CloseAppsDialog::isITunesRunning()
{
    QStringList apps;
#ifndef Q_OS_MAC
    QList<IPluginInfo*> plugins;
    ITunesPluginInfo* iTunesPluginInfo = new ITunesPluginInfo;
    plugins << iTunesPluginInfo;
    apps = runningApps( plugins );
    delete iTunesPluginInfo;
#else
    apps = runningApps();
#endif
    return apps.count() == 1;
}

void
unicorn::CloseAppsDialog::checkApps()
{
    QStringList apps = runningApps();

    ui->listWidget->setUpdatesEnabled( false );

    ui->listWidget->clear();

    foreach ( const QString& app, apps )
        new QListWidgetItem( app, ui->listWidget );

    ui->listWidget->setUpdatesEnabled( true );

    if ( apps.count() == 0 )
        accept();
}

#ifndef Q_OS_MAC
QStringList
unicorn::CloseAppsDialog::runningApps()
{
    return runningApps( m_plugins );
}

QStringList
unicorn::CloseAppsDialog::runningApps( const QList<IPluginInfo*>& plugins )
{
    QStringList apps;

    DWORD aProcesses[1024];
    DWORD cbNeeded;
    DWORD cProcesses;
    unsigned int i;

    if ( EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
    {
        // Calculate how many process identifiers were returned.

        cProcesses = cbNeeded / sizeof(DWORD);

        // Print the name and process identifier for each process.

        for ( i = 0; i < cProcesses; ++i )
        {
            if( aProcesses[i] != 0 )
            {
                TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

                // Get a handle to the process.
                HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                PROCESS_VM_READ,
                FALSE, aProcesses[i] );

                if (NULL != hProcess )
                {
                    HMODULE hMod;
                    DWORD cbNeeded;

                    if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
                    {
                        GetModuleBaseName( hProcess, hMod, szProcessName, sizeof(szProcessName)/sizeof(TCHAR) );

                        qDebug() << QString( "%1  (PID: %2)").arg( QString::fromStdWString( szProcessName ), QString::number( aProcesses[i] ) );

                        foreach( IPluginInfo* plugin, plugins )
                            if ( plugin->processName().compare( QString::fromStdWString( szProcessName ), Qt::CaseInsensitive ) == 0 )
                                apps << plugin->name();
                    }
                }

                // Release the handle to the process.
                CloseHandle( hProcess );
            }
        }
    }

    return apps;
}
#endif

unicorn::CloseAppsDialog::~CloseAppsDialog()
{
    delete ui;
#ifdef Q_OS_WIN
    if ( m_ownsPlugins )
        foreach ( IPluginInfo* plugin, m_plugins )
            delete plugin;
#endif
}
