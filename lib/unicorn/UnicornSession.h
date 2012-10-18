#ifndef UNICORN_SESSION_H_
#define UNICORN_SESSION_H_

#include <lastfm/XmlQuery.h>
#include <lastfm/misc.h>
#include <lastfm/User.h>
#include <lastfm/ws.h>

#include <QObject>
#include <QSharedData>

#include "lib/DllExportMacro.h"

namespace unicorn {

class UNICORN_DLLEXPORT Session : public QObject
{
    Q_OBJECT
public:
    /** Return session object from stored session */
    Session();
    Session( const QString& username, QString sessionKey = "" );

    QString sessionKey() const;

    lastfm::User userInfo() const;

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

    static QMap<QString, QString>
    lastSessionData();

    QDataStream& write( QDataStream& out ) const;
    QDataStream& read( QDataStream& in );

signals:
    void userInfoUpdated( const lastfm::User& userInfo );

protected:
    void init( const QString& username, const QString& sessionKey );

private:
    void cacheUserInfo( const lastfm::User& userInfo );

private slots:
    void fetchUserInfo();
    void onUserGotInfo();
    void onAuthGotSessionInfo();

private:
    QString m_prevUsername;
    QString m_sessionKey;
    lastfm::User m_userInfo;
};

}

QDataStream& operator<<( QDataStream& out, const unicorn::Session& s );
QDataStream& operator>>( QDataStream& in, unicorn::Session& s );

#endif //UNICORN_SESSION_H_
