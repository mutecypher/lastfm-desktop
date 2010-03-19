#include "UnicornSession.h"
#include "UnicornSettings.h"
#include <QDebug>

namespace unicorn {

Session::Session()
        :d( 0 )
{
    Settings s;
    
    //use the Username setting or the first username if there have been any logged in previously
    QString username = s.value( "Username", QString()).toString();

    if( username.isEmpty() && !s.childGroups().isEmpty()) {
        foreach( QString child, s.childGroups()) {
            if( child == "com" ) continue;
            username = child;
            break;
        }
    }

    if( username.isEmpty()) return;

    m_prevUsername = username;

    s.beginGroup( username );

    if( s.contains( "SessionKey" )) {
        init( username, s.value( "SessionKey" ).toString(), s.value("Subscriber", false).toBool());
    }

}

Session::Session( QNetworkReply* reply ) throw( lastfm::ws::ParseError )
        :d( 0 )
{
    lastfm::XmlQuery lfm = lastfm::ws::parse( reply );
    lastfm::XmlQuery session = lfm["session"];
    
    // replace username; because eg. perhaps the user typed their
    // username with the wrong camel case
    QString username = session["name"].text();
    QString sessionKey = session["key"].text();
    bool subscriber = (session["subscriber"].text().trimmed() == "1");
    
    init( username, sessionKey, subscriber );
}

Session::Session( const Session& other )
        : d( other.d )
{}

Session::Session( const QString& username )
        : d( 0 )
{
    Settings s;
    if( !s.childGroups().contains( username, Qt::CaseInsensitive )) return;
    s.beginGroup( username );
    bool subscriber = s.value( "Subscriber" ).toBool();
    QString sessionKey = s.value( "SessionKey" ).toString();

    init( username, sessionKey, subscriber );
}

QString 
Session::username() const
{
    if( isValid())
        return d->username;
    else
        return m_prevUsername;
}

QString 
Session::sessionKey() const
{
    if( isValid() )
        return d->sessionKey;
    
    return QString();
}

bool 
Session::isSubscriber() const 
{
    return d->isSubscriber;
}

void 
Session::init( const QString& username, const QString& sessionKey, const bool isSubscriber )
{
    d = new SessionData;

    d->username = username;
    d->sessionKey = sessionKey;
    d->isSubscriber = isSubscriber;
    
    Settings s;
    s.beginGroup( username );
    s.setValue( "Subscriber", isSubscriber );
    s.setValue( "SessionKey", sessionKey );
}

}

QDataStream& operator<<( QDataStream& out, const unicorn::Session& s ){ return s.write( out ); }
QDataStream& operator>>( QDataStream& in, unicorn::Session& s ){ return s.read( in ); }

