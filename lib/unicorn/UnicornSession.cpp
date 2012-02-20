
#include <QApplication>
#include <QDebug>

#include <lastfm/User.h>
#include <lastfm/Auth.h>
#include <lastfm/XmlQuery.h>

#include "UnicornSession.h"
#include "UnicornSettings.h"

namespace unicorn {


QMap<QString, QString>
Session::lastSessionData()
{
    Settings s;
    QMap<QString, QString> sessionData;

    //use the Username setting or the first username if there have been any logged in previously
    QString username = s.value( "Username", QString() ).toString();

    if( !username.isEmpty() )
    {
        UserSettings us( username );

        sessionData[ "username" ] = username;
        const QString sk = us.value( "SessionKey", "" ).toString();

        if( !sk.isEmpty() )
            sessionData[ "sessionKey" ] = sk;

        s.endGroup();
    }

    return sessionData;
}

Session::Session()
{
}

Session::Session( const QString& username, QString sessionKey )
{
    m_userInfo.setName( username );
    m_sessionKey = sessionKey;

    Settings s;
    s.setValue( "Username", username );

    init( username, sessionKey );
}

QString 
Session::sessionKey() const
{
    return m_sessionKey;
}

lastfm::User
Session::userInfo() const
{
    return m_userInfo;
}

void 
Session::init( const QString& username, const QString& sessionKey )
{
    m_sessionKey = sessionKey;

    UserSettings s( username );
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

    if ( sessionKey.isEmpty() )
        Q_ASSERT( false );
    else
        s.setValue( "SessionKey", sessionKey );

    fetchUserInfo();

    connect( qApp, SIGNAL( internetConnectionUp() ), SLOT( fetchUserInfo() ) );
}

void
Session::fetchUserInfo()
{
    qDebug() << "fetching user info";
    lastfm::ws::Username = m_userInfo.name();
    lastfm::ws::SessionKey = m_sessionKey;
    connect( lastfm::User::getInfo(), SIGNAL( finished() ), SLOT( onUserGotInfo() ) );
    connect( lastfm::Auth::getSessionInfo(), SIGNAL(finished()), SLOT(onAuthGotSessionInfo()) );
}

void
Session::onUserGotInfo()
{
    QNetworkReply* reply = ( QNetworkReply* )sender();

    if ( reply->error() == QNetworkReply::NoError  )
    {
        XmlQuery lfm;
        if ( lfm.parse( reply->readAll() ) )
        {
            lastfm::User userInfo( lfm["user"] );

            m_userInfo = userInfo;
            emit userInfoUpdated( m_userInfo );
            cacheUserInfo( m_userInfo );
        }
        else
        {
            qDebug() << lfm.parseError().message() << lfm.parseError().enumValue();
        }
    }
    else
    {
        qDebug() << "error getting user info: " << reply->errorString();
    }
}

void
Session::onAuthGotSessionInfo()
{
    XmlQuery lfm;

    /*
    <lfm status="ok">
        <application>
          <session>
            <name>eartle</name>
            <key>5be299e899764d175ebff77beb40d54b</key>
            <subscriber>1</subscriber>
          </session>
          <country>GB</country>
          <radioPermission>
            <user type="you">
              <radio>0</radio>
              <freetrial>0</freetrial>
            </user>
            <user type="registered">
              <radio>1</radio>
              <freetrial>0</freetrial>
            </user>
            <user type="subscriber">
              <radio>1</radio>
              <freetrial>0</freetrial>
            </user>
          </radioPermission>
        </application>
      </lfm>

     */

    if ( lfm.parse( static_cast<QNetworkReply*>( sender() )->readAll() ) )
    {
        qDebug() << lfm;

        bool radio = lfm["application"]["radioPermission"]["user type=you"]["radio"].text() != "0";
        bool canUpgrade = lfm["application"]["radioPermission"]["user type=subscriber"]["radio"].text() != "0";
        bool freeTrial = lfm["application"]["radioPermission"]["user type=you"]["freeTrial"].text() != "0";
    }
    else
    {
        qDebug() << lfm.parseError().message() << lfm.parseError().enumValue();
    }
}

void
Session::cacheUserInfo( const lastfm::User& userInfo )
{
    const char* key = UserSettings::subscriptionKey();

    UserSettings s( userInfo.name() );
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

}

}

QDataStream& operator<<( QDataStream& out, const unicorn::Session& s ){ return s.write( out ); }
QDataStream& operator>>( QDataStream& in, unicorn::Session& s ){ return s.read( in ); }

