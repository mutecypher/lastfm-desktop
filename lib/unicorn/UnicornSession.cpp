#include "UnicornSession.h"
#include "UnicornSettings.h"

#include <lastfm/User>

#include <QDebug>

namespace unicorn {

/*void
SessionData::onUserGotInfo()
{
    QNetworkReply* reply = ( QNetworkReply* )sender();

    if ( reply->error() == QNetworkReply::NoError  )
    {
        lastfm::UserDetails userInfo( reply );

        emit userInfoUpdated( userInfo );

        //Save user info
        const char* key = UserSettings::subscriptionKey();
        Settings s;
        s.setValue( key, userInfo.isSubscriber() );
        s.setValue( "scrobbleCount", userInfo.scrobbleCount() );
        s.setValue( "dateRegistered", userInfo.dateRegistered() );
        s.setValue( "realName", userInfo.realName() );

        QList<lastfm::ImageSize> size;
        size << lastfm::Small << lastfm::Medium << lastfm::Large;

        s.beginWriteArray( "imageUrls", 3 );
        for ( int i = 0; i < 3; i++ )
        {
            s.setValue( "url", userInfo.imageUrl( size[ i ] ) );
        }
        s.endArray();
    }
    else
    {
        //there was a network error, so we fetch the user data from the settings
        lastfm::UserDetails userInfo;
        Settings s;
        userInfo.setScrobbleCount( s.value( "scrobbleCount", 0 ).toInt() );
        userInfo.setDateRegistered( s.value( "dateRegistered", QDateTime() ).toDateTime() );

        userInfo.setRealName( Settings().value( "realName", "" ).toString() );

        QList<QUrl> imageUrls;
        int imageCount = s.beginReadArray( "imageUrls" );

        for ( int i = 0; i < imageCount; i++ )
        {
            s.setArrayIndex( i );
            imageUrls.append( s.value( "url", QUrl() ).toUrl() );
        }
        s.endArray();

        userInfo.setImages( imageUrls );
        emit userInfoUpdated( userInfo );
    }

}*/

/*Session::Session()
        : d( 0 )
{
    Settings s;
    
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

    if( username.isEmpty()) return;

    m_prevUsername = username;

    s.beginGroup( username );
    init( username, s.value( "SessionKey", "" ).toString() );
}*/


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
        sessionData[ "sessionKey" ] = s.value( "SessionKey", "" ).toString();
        s.endGroup();
    }

    return sessionData;
}

Session::Session( QNetworkReply* reply ) throw( lastfm::ws::ParseError )
{
    lastfm::XmlQuery lfm = lastfm::ws::parse( reply );
    lastfm::XmlQuery session = lfm["session"];
    
    // replace username; because eg. perhaps the user typed their
    // username with the wrong camel case
    QString username = session["name"].text();
    QString sessionKey = session["key"].text();
    
    init( username, sessionKey );

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
}

void
Session::cacheUserInfo( const lastfm::UserDetails& userInfo )
{
    const char* key = UserSettings::subscriptionKey();
    Settings s;
    s.beginGroup( userInfo.name() );
    s.setValue( "Subscriber", userInfo.isSubscriber() );
    s.setValue( "ScrobbleCount", userInfo.scrobbleCount() );
    s.setValue( "DateRegistered", userInfo.dateRegistered() );
    s.setValue( "RealName", userInfo.realName() );

    QList<lastfm::ImageSize> size;
    size << lastfm::Small << lastfm::Medium << lastfm::Large;

    s.beginWriteArray( "ImageUrls", 3 );
    for ( int i = 0; i < 3; i++ )
    {
        s.setValue( "Url", userInfo.imageUrl( size[ i ] ) );
    }
    s.endArray();
    s.endGroup();

}

}

QDataStream& operator<<( QDataStream& out, const unicorn::Session& s ){ return s.write( out ); }
QDataStream& operator>>( QDataStream& in, unicorn::Session& s ){ return s.read( in ); }

