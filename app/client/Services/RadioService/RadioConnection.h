#ifndef RADIOCONNECTION_H
#define RADIOCONNECTION_H

#include "lib/listener/PlayerConnection.h"

class RadioConnection : public PlayerConnection
{
    Q_OBJECT
public:
    explicit RadioConnection( QObject* parent = 0 );

private:
    Track track() const;
    State state() const;

private slots:
    void onTrackSpooled( const Track& track );
};

#endif // RADIOCONNECTION_H
