#include <QTcpServer>
#include <QTcpSocket>

#include "SkipListener.h"

#include "Radio.h"

SkipListener::SkipListener(QObject *parent) :
    QObject(parent)
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
    m_users.clear();
}

void
SkipListener::onNewConnection()
{
    QTcpSocket* socket = m_server->nextPendingConnection();

    socket->waitForReadyRead();
    QString data = socket->readAll();

    qDebug() << data;

    if ( !m_users.contains( data ) )
        m_users.append( data );

    if ( m_users.count() >= 2 )
        radio->skip();

    socket->close();
}
