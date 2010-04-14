#ifndef UNICORN_SESSION_H_
#define UNICORN_SESSION_H_
#include <lastfm/XmlQuery>
#include <lastfm/misc.h>
#include <lastfm/ws.h>
#include <QSharedData>

#include "lib/DllExportMacro.h"

namespace unicorn {

class UNICORN_DLLEXPORT SessionData : public QSharedData
{
public:
    SessionData(): isSubscriber( false ){}

    QString username;
    QString sessionKey;
    bool isSubscriber;
    bool remember;
};

class UNICORN_DLLEXPORT Session
{
public:
    Session();
    Session( const Session& other );
    Session( QNetworkReply* reply ) throw( lastfm::ws::ParseError );

    /** Return session object from stored session */
    Session( const QString& username );

    bool isValid() const
    {
        return d;
    }

    QString username() const;
    QString sessionKey() const;

    bool isSubscriber() const;

    static QNetworkReply* 
    getToken()
    {
        QMap<QString, QString> params;
        params["method"] = "auth.getToken";
        return lastfm::ws::get( params );
    }

    static QNetworkReply*
    getSession( QString token )
    {
        QMap<QString, QString> params;
        params["method"] = "auth.getSession";
        params["token"] = token;
        return lastfm::ws::post( params, false );
    }

    QDataStream& write( QDataStream& out ) const
    {
        if( !d ) return out;

        QMap<QString, QString> data;
        data[ "username" ] = d->username;
        data[ "sessionkey" ] = d->sessionKey;
        data[ "subscriber" ] = d->isSubscriber ? "1" : "0";
        data[ "remember" ] = d->remember ? "1" : "0";
        out << data;
        return out;
    }

    QDataStream& read( QDataStream& in )
    {
        QMap<QString, QString> data;
        in >> data;
        if( !d ) 
            d = new SessionData();

        init( data[ "username" ], data[ "sessionkey" ], data[ "subscriber" ] == "1");
        d->remember = data[ "remember" ] == "1";
        
        return in;
    }

protected:
    void init( const QString& username, const QString& sessionKey, const bool isSubscriber );

private:
    QExplicitlySharedDataPointer<SessionData> d;
    QString m_prevUsername;
};

}

QDataStream& operator<<( QDataStream& out, const unicorn::Session& s );
QDataStream& operator>>( QDataStream& in, unicorn::Session& s );

#endif //UNICORN_SESSION_H_
