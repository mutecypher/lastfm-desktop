#include "DeviceScrobbler.h"
#include <QDebug>

#include "../Dialogs/ScrobbleConfirmationDialog.h"
#include "lib/unicorn/UnicornApplication.h"
#include "IpodDevice.h"

#ifdef Q_WS_X11
#include <QFileDialog>
#include "lib/unicorn/QMessageBoxBuilder.h"
#endif

QString getIpodMountPath();

DeviceScrobbler::DeviceScrobbler()
{
    qDebug() << "Instantiating DeviceScrobbler";
}

void 
DeviceScrobbler::checkCachedIPodScrobbles()
{
    // Check if there are any iPod scrobbles
    unicorn::Session* currentSession = qobject_cast<unicorn::Application*>(qApp)->currentSession();
    if( !currentSession )
        return;

    unicorn::UserSettings us( currentSession->userInfo().name() );
    int count = us.beginReadArray( "associatedDevices" );

    for ( int i = 0; i < count; i++ )
    {
        us.setArrayIndex( i );

        QString deviceId = us.value( "deviceId" ).toString();
        QString deviceName = us.value( "deviceName" ).toString();

        IpodDevice* ipod = new IpodDevice( deviceId, deviceName );

        // check if there are any iPod scrobbles in its folder
        QDir scrobblesDir = lastfm::dir::runtimeData();

        if ( scrobblesDir.cd( "devices/" + ipod->deviceId() + "/scrobbles" ) )
        {
            scrobblesDir.setFilter(QDir::Files | QDir::NoSymLinks);
            scrobblesDir.setNameFilters( QStringList() << "*.xml" );

            QFileInfoList list = scrobblesDir.entryInfoList();

            if ( list.count() > 0 )
            {
                // There are iPod files!

                QStringList iPodFiles;
                foreach ( QFileInfo fileInfo, list )
                    iPodFiles << fileInfo.filePath();

                IpodDevice::Scrobble scrobble = ipod->scrobble();

                if ( scrobble == IpodDevice::NotNow )
                {
                    // The user isn't sure if they want to scrobble this iPod yet so ask them
                    ScrobbleSetupDialog* scrobbleSetup = new ScrobbleSetupDialog( deviceId, deviceName, iPodFiles );
                    connect( scrobbleSetup, SIGNAL(clicked(IpodDevice::Scrobble,QString,QString,QStringList)), SLOT(onScrobbleSetupClicked(IpodDevice::Scrobble,QString,QString,QStringList)));
                    scrobbleSetup->show();
                }
                else
                    onScrobbleSetupClicked( scrobble, deviceId, deviceName, iPodFiles );
            }
        }

        delete ipod;
    }
    us.endArray();
}


void 
DeviceScrobbler::handleMessage( const QStringList& message )
{
    int pos = message.indexOf( "--twiddly" );
    const QString& action = message[ pos + 1 ];
    
    if( action == "starting" )
        emit processingScrobbles();
    else if( action == "no-tracks-found" )
        emit noScrobblesFound();
    else if( action == "complete" )
        twiddled( message );
}


void 
DeviceScrobbler::iPodDetected( const QStringList& arguments )
{
    bool newIpod = false;

    int pos = arguments.indexOf( "--ipod-detected" );

    if( pos == -1 )
    {
        pos = arguments.indexOf( "--new-ipod-detected" );
        newIpod = true;
    }

    QString serialNumber;
    
    if( pos > -1 ) serialNumber = arguments[ pos + 1 ];
   
    qDebug() << "emitting detectedIPod: " << (int)this;
    emit detectedIPod( serialNumber );
}

lastfm::User
DeviceScrobbler::associatedUser( QString deviceId )
{
    lastfm::User associatedUser( "" );

    // Check if the device has been associated with a user
    // and then if it is with the current user
    QList<lastfm::User> roster = unicorn::Settings().userRoster();
    foreach( lastfm::User user, roster )
    {
        unicorn::UserSettings us( user.name() );
        int count = us.beginReadArray( "associatedDevices" );

        for ( int i = 0; i < count; i++ )
        {
            us.setArrayIndex( i );

            QString tempDeviceId = us.value( "deviceId" ).toString();

            qDebug() << tempDeviceId;

            if ( tempDeviceId == deviceId )
            {
                associatedUser = user;
                break;
            }
        }
        us.endArray();

        if ( !associatedUser.name().isEmpty() ) break;
    }

    return associatedUser;
}

void 
DeviceScrobbler::twiddled( QStringList arguments )
{
    if( arguments.contains( "--twiddled-no-tracks" ) )
        return;

    // iPod scrobble time!

    // Check if this iPod has been associated to any of our users
    QString deviceId = arguments[ arguments.indexOf( "--deviceId" ) + 1 ];
    QString deviceName = arguments[ arguments.indexOf( "--deviceName" ) + 1 ];
    QString iPodPath = arguments[ arguments.indexOf( "--ipod-path" ) + 1 ];
    bool showSetup = false;

    // Check if the device has been associated with a user
    // and then if it is with the current user
    lastfm::User associatedUser = this->associatedUser( deviceId );

    if ( !associatedUser.name().isEmpty() )
    {
        if ( associatedUser.name() == lastfm::ws::Username )
        {
            IpodDevice* ipod = new IpodDevice( deviceId, deviceName );
            IpodDevice::Scrobble iPodScrobble = ipod->scrobble();
            delete ipod;

            if ( iPodScrobble == IpodDevice::NotNow )
                showSetup = true;
            else
                onScrobbleSetupClicked( iPodScrobble, deviceId, deviceName, QStringList( iPodPath ) );
        }
        else
        {
            // Show a warning
            // The iPod is associated with a differnt user
            // it will be scrobbled the next time that user is online
            // TODO: Needs better copy
        }
    }
    else
        showSetup = true;

    if ( showSetup )
    {
        // The device has not been associated yet
        ScrobbleSetupDialog* scrobbleSetup = new ScrobbleSetupDialog( deviceId, deviceName, QStringList( iPodPath ) );
        connect( scrobbleSetup, SIGNAL(clicked(IpodDevice::Scrobble,QString,QString,QStringList)), SLOT(onScrobbleSetupClicked(IpodDevice::Scrobble,QString,QString,QStringList)));
        scrobbleSetup->show();
    }
}


void
DeviceScrobbler::onScrobbleSetupClicked( IpodDevice::Scrobble result, QString deviceId, QString deviceName, QStringList iPodFiles )
{
    // We need to store the result so we can check it next time
    IpodDevice* ipod = new IpodDevice( deviceId, deviceName );

    if ( !ipod->isDeviceKnown() )
        ipod->associateDevice( lastfm::ws::Username );

    if ( result == IpodDevice::Yes )
    {
        foreach ( QString iPodFile, iPodFiles )
            scrobbleIpodFile( iPodFile );
    }
    else
    {
        foreach ( QString iPodFile, iPodFiles )
            QFile::remove( iPodFile );
    }

    if ( ipod->isDeviceKnown() )
        ipod->setScrobble( result );

    delete ipod;
}


void 
DeviceScrobbler::scrobbleIpodFile( QString iPodScrobblesFilename )
{
    qDebug() << iPodScrobblesFilename;

    QFile iPodScrobblesFile( iPodScrobblesFilename );

    if ( iPodScrobblesFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        QDomDocument iPodScrobblesDoc;
        iPodScrobblesDoc.setContent( &iPodScrobblesFile );
        QDomNodeList tracks = iPodScrobblesDoc.elementsByTagName( "track" );

        QList<lastfm::Track> scrobbles;

        for ( int i(0) ; i < tracks.count() ; ++i )
        {
            lastfm::Track track( tracks.at(i).toElement() );

            int playcount = track.extra("playCount").toInt();

            for ( int j(0) ; j < playcount ; ++j )
                scrobbles << track;
        }

        if( unicorn::UserSettings().value( "confirmIpodScrobbles", false ).toBool() )
        {
            ScrobbleConfirmationDialog confirmDialog( scrobbles );
            if ( confirmDialog.exec() == QDialog::Accepted )
            {
                scrobbles = confirmDialog.tracksToScrobble();

                // sort the iPod scrobbles before caching them
                if ( scrobbles.count() > 1 )
                    qSort ( scrobbles.begin(), scrobbles.end() );

                emit foundScrobbles( scrobbles );
            }
        }
        else
        {
            // sort the iPod scrobbles before caching them
            if ( scrobbles.count() > 1 )
                qSort ( scrobbles.begin(), scrobbles.end() );

            emit foundScrobbles( scrobbles );
        }
    }

    iPodScrobblesFile.remove();
}

#ifdef Q_WS_X11
void 
DeviceScrobbler::onScrobbleIpodTriggered() {
    if ( iPod )
    {
        qDebug() << "deleting ipod...";
        delete iPod;
    }
    qDebug() << "here";
    iPod = new IpodDeviceLinux;
    QString path;
    bool autodetectionSuceeded = true;

    if ( !iPod->autodetectMountPath() )
    {
        path = getIpodMountPath();
        iPod->setMountPath( path );
        autodetectionSuceeded = false;
    }

    if ( autodetectionSuceeded || !path.isEmpty() )
    {
        connect( iPod, SIGNAL( scrobblingCompleted( int ) ), this, SLOT( scrobbleIpodTracks( int ) ) );
        connect( iPod, SIGNAL( calculatingScrobbles( int ) ), this, SLOT( onCalculatingScrobbles( int ) ) );
        connect( iPod, SIGNAL( errorOccurred() ), this, SLOT( onIpodScrobblingError() ) );
        iPod->fetchTracksToScrobble();
    }
}


QString getIpodMountPath()
{
    QString path = "";
    QFileDialog dialog( 0, QObject::tr( "Where is your iPod mounted?" ), "/" );
    dialog.setOption( QFileDialog::ShowDirsOnly, true );
    dialog.setFileMode( QFileDialog::Directory );

    //The following lines are to make sure the QFileDialog looks native.
    QString backgroundColor( "transparent" );
    dialog.setStyleSheet( "QDockWidget QFrame{ background-color: " + backgroundColor + "; }" );

    if ( dialog.exec() )
    {
       path = dialog.selectedFiles()[ 0 ];
    }
    return path;
}

void 
DeviceScrobbler::onCalculatingScrobbles( int trackCount )
{
    qApp->setOverrideCursor( Qt::WaitCursor );
}

void 
DeviceScrobbler::scrobbleIpodTracks( int trackCount )
{
    qApp->restoreOverrideCursor();
    qDebug() << trackCount << " new tracks to scrobble.";

    bool bootStrapping = false;
    if ( iPod->lastError() != IpodDeviceLinux::NoError && !iPod->isDeviceKnown() )
    {
        bootStrapping = true;
        qDebug() << "Should we save it?";
        int result = QMessageBoxBuilder( 0 )
            .setIcon( QMessageBox::Question )
            .setTitle( tr( "Scrobble iPod" ) )
            .setText( tr( "Do you want to associate the device %1 to your audioscrobbler user account?" ).arg( iPod->deviceName() ) )
            .setButtons( QMessageBox::Yes | QMessageBox::No )
            .exec();

        if ( result == QMessageBox::Yes )
        {
            iPod->associateDevice();
            QMessageBoxBuilder( 0 )
                .setIcon( QMessageBox::Information )
                .setTitle( tr( "Scrobble iPod" ) )
                .setText( tr( "Device successfully associated to your user account. "
                            "From now on you can scrobble the tracks you listen on this device." ) )
                .exec();

        }
        else
        {
            unicorn::Session* currentSession = qobject_cast<unicorn::Application*>( qApp )->currentSession();
            if ( currentSession )
            {
                IpodDeviceLinux::deleteDeviceHistory( currentSession->userInfo().name(), iPod->deviceId() );
            }
        }
    }

    QList<lastfm::Track> tracks = iPod->tracksToScrobble();

    if ( tracks.count() )
    {
        if ( !bootStrapping )
        {
            if( unicorn::UserSettings().value( "confirmIpodScrobbles", false ).toBool() )
            {
                qDebug() << "showing confirm dialog";
                ScrobbleConfirmationDialog confirmDialog( tracks );
                if ( confirmDialog.exec() == QDialog::Accepted )
                {
                    tracks = confirmDialog.tracksToScrobble();

                    // sort the iPod scrobbles before caching them
                    if ( tracks.count() > 1 )
                        qSort ( tracks.begin(), tracks.end() );

                    emit foundScrobbles( tracks );
                }
            }
            else
            {
                // sort the iPod scrobbles before caching them
                if ( tracks.count() > 1 )
                    qSort ( tracks.begin(), tracks.end() );

                emit foundScrobbles( tracks );
                QMessageBoxBuilder( 0 )
                    .setIcon( QMessageBox::Information )
                    .setTitle( tr( "Scrobble iPod" ) )
                    .setText( tr( "%1 tracks scrobbled." ).arg( tracks.count() ) )
                    .exec();
            }
        }
    }
    else if ( !iPod->lastError() )
    {
        QMessageBoxBuilder( 0 )
            .setIcon( QMessageBox::Information )
            .setTitle( tr( "Scrobble iPod" ) )
            .setText( tr( "No tracks to scrobble since your last sync." ) )
            .exec();
        qDebug() << "No tracks to scrobble";
    }
    delete iPod;
    iPod = 0;
}

void 
DeviceScrobbler::onIpodScrobblingError()
{
    qDebug() << "iPod Error";
    qApp->restoreOverrideCursor();
    QString path;
    switch( iPod->lastError() )
    {
        case IpodDeviceLinux::AutodetectionError: //give it another try
            qDebug() << "giving another try";
            path = getIpodMountPath();
            if ( !path.isEmpty() )
            {
                iPod->setMountPath( path );
                iPod->fetchTracksToScrobble();
            }
            break;

        case IpodDeviceLinux::AccessError:
            QMessageBoxBuilder( 0 )
                .setIcon( QMessageBox::Critical )
                .setTitle( tr( "Scrobble iPod" ) )
                .setText( tr( "The iPod database could not be opened." ) )
                .exec();
            delete iPod;
            iPod = 0;
            break;
        case IpodDeviceLinux::UnknownError:
            QMessageBoxBuilder( 0 )
                .setIcon( QMessageBox::Critical )
                .setTitle( tr( "Scrobble iPod" ) )
                .setText( tr( "An unkown error occurred while trying to access the iPod database." ) )
                .exec();
            delete iPod;
            iPod = 0;
            break;
        default:
            qDebug() << "untracked error:" << iPod->lastError();
    }
}

#endif


