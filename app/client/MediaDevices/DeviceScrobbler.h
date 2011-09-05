#ifndef DEVICE_SCROBBLER_H_
#define DEVICE_SCROBBLER_H_

#include <QDialogButtonBox>

#include "../Dialogs/ScrobbleSetupDialog.h"
#include "lib/unicorn/UnicornSession.h"
#include "lib/unicorn/UnicornSettings.h"
#include <lastfm/User.h>
#include "IpodDevice.h"

#ifdef Q_WS_X11
#include <QPointer>
#include "IpodDevice_linux.h"
#endif

using unicorn::Session;

class DeviceScrobbler :public QObject {
Q_OBJECT
public:
    DeviceScrobbler();

    void checkCachedIPodScrobbles();
    void handleMessage( const QStringList& );
    void twiddled( QStringList arguments );
    void iPodDetected( const QStringList& arguments );

signals:
    void detectedIPod( const QString& serialNumber );
    void processingScrobbles();
    void noScrobblesFound();
    void foundScrobbles( const QList<lastfm::Track>& );

public slots:
#ifdef Q_WS_X11
    void onScrobbleIpodTriggered();
#endif

private slots:
    void onScrobbleSetupClicked( bool scrobble, bool alwaysAsk, QString deviceId, QString deviceName, QStringList iPodFiles );

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

    lastfm::User associatedUser( QString deviceId );
};

#endif //DEVICE_SCROBBLER_H_
