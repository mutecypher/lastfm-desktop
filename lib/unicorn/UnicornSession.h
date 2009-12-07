#ifndef UNICORN_SESSION_H_
#define UNICORN_SESSION_H_
#include "lib/unicorn/UnicornSettings.h"

namespace unicorn {

class SessionData : QSharedData
{
public:
    SessionData(): isSubscriber( false );

    QString username;
    QString sessionKey;
    bool isSubscriber;
};

class Session : public QObject
{
public:
    Session(){}
  
    void setUsername( QString& username )
    {
        GlobalSettings s;
        s.value( "Username");
    }

    Session( QNetwork Reply* ) throw lastfm::ws::Error
    {
        QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
        
        lastfm::XmlQuery lfm = lastfm::ws::parse( reply );
        lastfm::XmlQuery session = lfm["session"];
        
        // replace username; because eg. perhaps the user typed their
        // username with the wrong camel case
        QString username = session["name"].text();
        QString sessionKey = session["key"].text();
        
        init( username, sessionKey );
    }


    static QNetworkReply* 
    getMobileSession( const QString& username, const QString& password )
    {
        QMap<QString, QString> params;
        params["method"] = "auth.getMobileSession";
        params["username"] = username;
        params["authToken"] = lastfm::md5( (username + password).toUtf8() );
        return lastfm::ws::post( params );
    }

protected:
    void init( const QString& username, const QString& sessionKey, const bool isSubscriber )
    {
        d = new SessionData;

        d->username = username;
        d->sessionKey = sessionKey;
        d->isSubscriber = isSubscriber;
    }

private:
    QExplicitlySharedDataPointer<SessionData> d;
    
};

}

#endif //UNICORN_SESSION_H_
