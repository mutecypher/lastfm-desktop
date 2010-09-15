#ifndef DEVICE_SCROBBLER_H_
#define DEVICE_SCROBBLER_H_

#include "lib/unicorn/UnicornSession.h"
#include "lib/unicorn/UnicornSettings.h"
#include <lastfm/User>
#include "MediaDevices/IpodDevice.h"

#ifdef Q_WS_X11
#include "MediaDevices/IpodDevice_linux.h"
#include <QPointer>
#endif

using unicorn::Session;

class DeviceScrobbler :public QObject {
Q_OBJECT
public:
    DeviceScrobbler();

    void checkCachedIPodScrobbles();
    void twiddled( QStringList arguments );
    void iPodDetected( const QStringList& arguments );

signals:
    void detectedIPod( const QString& serialNumber );
    void noScrobblesFound();
    void foundScrobbles( QList<Track> );

public slots:
#ifdef Q_WS_X11
    void onScrobbleIpodTriggered();
#endif

private slots:
    
#ifdef Q_WS_X11
    void onCalculatingScrobbles( int trackCount );
    void scrobbleIpodTracks( int trackCount );
    void onIpodScrobblingError();
#endif

private:
#ifdef Q_WS_X11
    QPointer<IpodDeviceLinux> iPod;
#endif


    void scrobbleIpodFile( QString iPodScrobblesFilename );
};

#endif //DEVICE_SCROBBLER_H_
