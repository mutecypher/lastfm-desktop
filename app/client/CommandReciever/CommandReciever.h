#ifndef COMMANDRECIEVER_H
#define COMMANDRECIEVER_H

#include <QObject>
#include <QPointer>
#include <QPixmap>

#include <lib/unicorn/TrackImageFetcher.h>

class CommandReciever : public QObject
{
    Q_OBJECT
public:
    explicit CommandReciever(QObject *parent = 0);
    ~CommandReciever();

    void logo();
    QPixmap getLogo() const;

signals:

private slots:
    void onFinished( const class QPixmap& image );

private:
    QPointer<TrackImageFetcher> m_trackImageFetcher;
    QPixmap m_pixmap;
};

#endif // COMMANDRECIEVER_H
