#ifndef UNICORN_SESSION_H_
#define UNICORN_SESSION_H_
#include <lastfm/XmlQuery>
#include <lastfm/misc.h>
#include <lastfm/User>
#include <lastfm/ws.h>

#include <QObject>
#include <QSharedData>

#include "lib/DllExportMacro.h"

namespace unicorn {

class UNICORN_DLLEXPORT SessionData : public QSharedData
{
public:
    SessionData(){};

    QString sessionKey;
    QString username;
};

class UNICORN_DLLEXPORT Session: public QObject
{
    Q_OBJECT
public:
    /** Return session object from stored session */
    Session();
    Session( const QString& username, QString sessionKey = "" );
    Session( QNetworkReply* reply ) throw( lastfm::ws::ParseError );


    bool isValid() const
    {
        return d;
    }

    QString sessionKey() const;
    QString username() const{ return m_userInfo.name(); }

    lastfm::UserDetails userInfo() const;

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
        out << data;
        return out;
    }

    QDataStream& read( QDataStream& in )
    {
        QMap<QString, QString> data;
        in >> data;
        if( !d ) 
            d = new SessionData();

        init( data[ "username" ], data[ "sessionkey" ] );
        
        return in;
    }

protected:
    void init( const QString& username, const QString& sessionKey );

private:
    void cacheUserInfo( const lastfm::UserDetails& userInfo );

private:
    QExplicitlySharedDataPointer<SessionData> d;
    QString m_prevUsername;
    lastfm::UserDetails m_userInfo;
};

}

QDataStream& operator<<( QDataStream& out, const unicorn::Session& s );
QDataStream& operator>>( QDataStream& in, unicorn::Session& s );

#endif //UNICORN_SESSION_H_
