#ifndef NAMEDPIPESERVER_H
#define NAMEDPIPESERVER_H

#include <QThread>

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>

class NamedPipeServer : public QThread
{
    Q_OBJECT
public:
    explicit NamedPipeServer( QObject* parent = 0 );

private:
    void run();

    static VOID DisconnectAndReconnect(DWORD i);
    static BOOL ConnectToNewClient(HANDLE hPipe, LPOVERLAPPED lpo);

signals:
    QString lineReady( const QString& line );
};

#endif // NAMEDPIPESERVER_H
