#ifndef SKIPLISTENER_H
#define SKIPLISTENER_H

#include <QObject>
#include <QStringList>

class SkipListener : public QObject
{
    Q_OBJECT
public:
    explicit SkipListener(QObject *parent = 0);

signals:

private slots:
    void onTrackSpooled();
    void onNewConnection();

private:
    class QTcpServer* m_server;
    QStringList m_skippers;
};

#endif // SKIPLISTENER_H
