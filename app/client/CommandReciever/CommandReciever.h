#ifndef COMMANDRECIEVER_H
#define COMMANDRECIEVER_H

#include <QObject>
#include <QPointer>

#include <lib/unicorn/TrackImageFetcher.h>

class CommandReciever : public QObject
{
    Q_OBJECT
public:
    explicit CommandReciever(QObject *parent = 0);
    ~CommandReciever();

    void logo();

signals:

private slots:
    void onFinished( const class QImage& image );

private:
    QPointer<TrackImageFetcher> m_trackImageFetcher;
};

#endif // COMMANDRECIEVER_H
