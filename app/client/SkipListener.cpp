#include <QTcpServer>
#include <QTcpSocket>
#include <qmath.h>

#include "SkipListener.h"

#include "Services/RadioService.h"

SkipListener::SkipListener(QObject *parent)
    :QObject(parent)
{
    m_server = new QTcpServer( this );

    bool success = m_server->listen( QHostAddress::Any, 54321 );

    if ( !success )
        qDebug() << m_server->errorString();

    connect( m_server, SIGNAL(newConnection()), SLOT(onNewConnection()));

    connect( radio, SIGNAL(trackSpooled(Track)), SLOT(onTrackSpooled()));
}


void
SkipListener::onTrackSpooled()
{
    m_skippers.clear();
}


void
SkipListener::onNewConnection()
{
    QTcpSocket* socket = m_server->nextPendingConnection();

    socket->waitForReadyRead();
    QString data = socket->readAll();

    qDebug() << data;

    if ( !m_skippers.contains( data ) )
        m_skippers.append( data );

    int totalUsers = radio->station().url().count(",") + 1;

    if ( m_skippers.count()  >= qCeil( totalUsers / 2.0f ) )
        radio->skip();

    socket->close();
}
