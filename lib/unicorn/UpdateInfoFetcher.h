#ifndef UPDATE_INFO_FETCHER_H_
#define UPDATE_INFO_FETCHER_H_
#include <QObject>
#include "lib/DllExportMacro.h"
#include <QUrl>
#include <QDir>

namespace lastfm{ class XmlQuery; }

class UNICORN_DLLEXPORT Plugin
{
public:
    enum BootstrapType{ NoBootstrap, ClientBootstrap, PluginBootstrap };

    Plugin(): m_valid( false ){}
    Plugin( const lastfm::XmlQuery& );

    QString toString() const { 
         return QString( "%1 (%2 - %3)\n\tinstallDir: %4\n\targs:%5\n\tregDisplayName: %6" )
                    .arg( m_name )
                    .arg( m_minVersion )
                    .arg( m_maxVersion )
                    .arg( m_installDir.path() )
                    .arg( m_args )
                    .arg( m_regDisplayName );
    }
    
    QString name() const { return m_name; }
    QString regDisplayName() const { return m_regDisplayName; }

    bool isValid() const { return m_valid; }
    bool isInstalled() const;
    bool isPluginInstalled() const;
    bool canBootstrap() const;

private:
    QString m_name;
    QString m_id;
    QUrl m_url;
    QDir m_installDir;
    QString m_args;
    QString m_minVersion;
    QString m_maxVersion;
    QString m_regDisplayName;

    BootstrapType m_bootstrapType;

    bool m_valid;
};


class UNICORN_DLLEXPORT UpdateInfoFetcher : public QObject
{
Q_OBJECT
public:
    UpdateInfoFetcher( class QNetworkReply* reply, QObject* parent = 0 );
    static QNetworkReply* fetchInfo();
    const QList<Plugin>& plugins() const{ return m_plugins; }

private:
    QList<Plugin> m_plugins;
};

#endif //UPDATE_INFO_FETCHER_H_