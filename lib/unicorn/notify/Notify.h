#ifndef NOTIFY_H
#define NOTIFY_H

#include <QObject>

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

public:
    void growlNotificationWasClicked();

};

}

#endif // NOTIFY_H
