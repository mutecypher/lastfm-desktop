#include "UpdateInfoFetcher.h"
#include <lastfm/ws.h>
#include <lastfm/XmlQuery>
#include <QDebug>
#include <QSettings>
#include <QString>

Plugin::Plugin( const XmlQuery& query )
       :m_valid( true ),
        m_bootstrapType( NoBootstrap )
{
    m_name = query.attribute( "name" );
    m_id = query.attribute( "id" );
    m_url = QUrl( query["Url"].text() );
    m_installDir = QDir( query[ "InstallDir" ].text() );
    m_args = query["Args"].text();
    m_minVersion = query["MinVersion"].text();
    m_maxVersion = query["MaxVersion"].text();
    m_regDisplayName = query["RegDisplayName"].text();

    QString bs = query["Bootstrap"].text();
    if( bs.compare( "Client", Qt::CaseInsensitive ) == 0) {
        m_bootstrapType = ClientBootstrap;
    } else if( bs.compare( "Plugin", Qt::CaseInsensitive ) == 0) {
        m_bootstrapType = PluginBootstrap;
    }
}


bool 
Plugin::isInstalled() const
{
    QSettings s("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\", QSettings::NativeFormat );
    foreach( QString group, s.childGroups()) {
        s.beginGroup( group );
            QString name = s.value( "DisplayName" ).toString();
            if( name.contains( m_regDisplayName ) ||
                group.contains( m_regDisplayName )) {
                    return true;
            }
        s.endGroup();
    }
    return false;
}


bool 
Plugin::isPluginInstalled() const
{
    QSettings s( "HKEY_LOCAL_MACHINE\\SOFTWARE\\Last.fm\\Client\\Plugins", QSettings::NativeFormat );
    return s.childGroups().contains( m_id );
}


bool
Plugin::canBootstrap() const
{
    return m_bootstrapType != NoBootstrap;
}


UpdateInfoFetcher::UpdateInfoFetcher( QNetworkReply* reply, QObject* parent )
                  :QObject( parent )
{
    XmlQuery xq = lastfm::ws::parse( reply );
    QList<XmlQuery> plugins = xq.children( "Plugin" );
    foreach( const XmlQuery& plugin, plugins ) {
        m_plugins << Plugin( plugin );
    }
}


QNetworkReply* 
UpdateInfoFetcher::fetchInfo() //static
{
    QString url = QString( "http://%1/ass/upgrade.xml.php?platform=win&lang=en" ).arg( lastfm::ws::host() );
    QNetworkRequest req( url );
    return lastfm::nam()->get( req );
}
