#ifndef NOTIFY_H
#define NOTIFY_H

#include <QObject>
#include <QPointer>

#include "lib/unicorn/TrackImageFetcher.h"

namespace lastfm { class Track; }

namespace unicorn
{

class Notify : public QObject
{
    Q_OBJECT
public:
    explicit Notify(QObject *parent = 0);

signals:
    void clicked();

public slots:
    void newTrack( const lastfm::Track& track );
    void stopped();

private slots:
    void onFinished( const QPixmap& pixmap );

public:
    void growlNotificationWasClicked();

private:
    QPointer<TrackImageFetcher> m_trackImageFetcher;
};

}

#endif // NOTIFY_H
