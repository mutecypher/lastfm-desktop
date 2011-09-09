#include "UnicornSession.h"
#include "UnicornSettings.h"

#include <lastfm/User.h>

#include <QApplication>
#include <QDebug>

namespace unicorn {


QMap<QString, QString>
Session::lastSessionData()
{
    Settings s;
    QMap<QString, QString> sessionData;
    //use the Username setting or the first username if there have been any logged in previously
    QString username = s.value( "Username", QString()).toString();

    QStringList groups = s.childGroups();
    if( (username.isEmpty()||!groups.contains(username, Qt::CaseInsensitive ))
        && !groups.isEmpty()) {
        foreach( QString child, s.childGroups()) {
            if( child == "com" || !s.contains( child + "/SessionKey") ) continue;
            username = child;
            break;
        }
    }

    if( !username.isEmpty() )
    {
        s.beginGroup( username );
        sessionData[ "username" ] = username;
		const QString sk = s.value( "SessionKey", "" ).toString();
        if( !sk.isEmpty()) sessionData[ "sessionKey" ] = sk;
        s.endGroup();
    }

    return sessionData;
}

Session::Session()
{
}

Session::Session( const QString& username, QString sessionKey )
{
    Settings s;
    s.setValue( "Username", username );
    if( !s.childGroups().contains( username, Qt::CaseInsensitive )) return;

    if ( sessionKey.isEmpty() )
    {
        s.beginGroup( username );
        sessionKey = s.value( "SessionKey" ).toString();
    }

    init( username, sessionKey );
}

QString 
Session::sessionKey() const
{
    return m_sessionKey;
}

lastfm::UserDetails
Session::userInfo() const
{
    return m_userInfo;
}

void 
Session::init( const QString& username, const QString& sessionKey )
{
    m_sessionKey = sessionKey;
    Settings s;
    s.beginGroup( username );
    m_userInfo.setName( username );
    m_userInfo.setScrobbleCount( s.value( "ScrobbleCount", 0 ).toInt() );
    m_userInfo.setDateRegistered( s.value( "DateRegistered", QDateTime() ).toDateTime() );

    m_userInfo.setRealName( Settings().value( "RealName", "" ).toString() );

    QList<QUrl> imageUrls;
    int imageCount = s.beginReadArray( "ImageUrls" );

    for ( int i = 0; i < imageCount; i++ )
    {
        s.setArrayIndex( i );
        imageUrls.append( s.value( "Url", QUrl() ).toUrl() );
    }
    s.endArray();

    m_userInfo.setImages( imageUrls );

    s.setValue( "SessionKey", sessionKey );

    s.endGroup();

    fetchUserInfo();
    connect( qApp, SIGNAL( internetConnectionUp() ), this, SLOT( fetchUserInfo() ) );
}

void
Session::fetchUserInfo()
{
    qDebug() << "fetching user info";
    lastfm::ws::Username = m_userInfo.name();
    lastfm::ws::SessionKey = m_sessionKey;
    connect( lastfm::UserDetails::getInfo(), SIGNAL( finished() ), this, SLOT( onUserGotInfo() ) );
}

void
Session::onUserGotInfo()
{
    QNetworkReply* reply = ( QNetworkReply* )sender();

    if ( reply->error() == QNetworkReply::NoError  )
    {
        lastfm::UserDetails userInfo( reply );

        m_userInfo = userInfo;
        emit userInfoUpdated( m_userInfo );
        cacheUserInfo( m_userInfo );
    }
    else
    {
        qDebug() << "error getting user info: " << reply->errorString();
    }
}

void
Session::cacheUserInfo( const lastfm::UserDetails& userInfo )
{
    const char* key = UserSettings::subscriptionKey();
    Settings s;
    s.beginGroup( userInfo.name() );
    s.setValue( key, userInfo.isSubscriber() );
    s.setValue( "ScrobbleCount", userInfo.scrobbleCount() );
    s.setValue( "DateRegistered", userInfo.dateRegistered() );
    s.setValue( "RealName", userInfo.realName() );

    QList<lastfm::ImageSize> sizes;
    sizes << lastfm::Small << lastfm::Medium << lastfm::Large;

    s.beginWriteArray( "ImageUrls", sizes.count() );
    for ( int i = 0; i < sizes.count(); i++ )
    {
        s.setArrayIndex( i );
        s.setValue( "Url", userInfo.imageUrl( sizes[ i ] ) );
    }
    s.endArray();
    s.endGroup();

}

}

QDataStream& operator<<( QDataStream& out, const unicorn::Session& s ){ return s.write( out ); }
QDataStream& operator>>( QDataStream& in, unicorn::Session& s ){ return s.read( in ); }

