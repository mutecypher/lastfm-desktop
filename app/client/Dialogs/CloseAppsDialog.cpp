#include <QTimer>
#include <QPushButton>

#include "CloseAppsDialog.h"
#include "ui_CloseAppsDialog.h"

CloseAppsDialog::CloseAppsDialog(QWidget *parent) :
    QDialog( parent, Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint ),
    ui(new Ui::CloseAppsDialog)
{
    ui->setupUi(this);

    setWindowTitle( tr( "Close Apps" ) );

    ui->text->setText( tr( "We are trying to install new media plugins. The following apps need to be closed before you can continue." ) );

    checkApps();

    QTimer* timer = new QTimer( this );
    connect( timer, SIGNAL(timeout()), SLOT(checkApps()) );
    timer->setInterval( 1000 );
    timer->start();
}

void
CloseAppsDialog::checkApps()
{
    QStringList apps = runningApps();

    ui->listWidget->setUpdatesEnabled( false );

    ui->listWidget->clear();

    foreach ( const QString& app, apps )
        new QListWidgetItem( app, ui->listWidget );

    ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( apps.count() == 0 );

    ui->listWidget->setUpdatesEnabled( true );
}


#ifndef Q_OS_MAC
QStringList
CloseAppsDialog::runningApps()
{
    return QStringList();
}

#endif

CloseAppsDialog::~CloseAppsDialog()
{
    delete ui;
}
