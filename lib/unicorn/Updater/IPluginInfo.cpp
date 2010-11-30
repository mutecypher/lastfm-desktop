
#include <QSettings>
#include <QStringList>
#include "IPluginInfo.h"
#include "KillProcess.h"


IPluginInfo::IPluginInfo()
{
}

IPluginInfo::~IPluginInfo()
{
}

#ifdef QT_VERSION
bool
IPluginInfo::isAppInstalled() const
{
#ifdef Q_OS_WIN
	QString displayNameValue = QString::fromStdString( displayName() );
	QSettings s("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\", QSettings::NativeFormat );
	foreach( QString group, s.childGroups()) {
		s.beginGroup( group );
			QString name = s.value( "DisplayName" ).toString();
			if( name.contains( displayNameValue ) ||
				group.contains( displayNameValue )) {
					if( !minVersion().isValid() && 
						!maxVersion().isValid())
						return true;

					QStringList verParts = s.value( "DisplayVersion" ).toString().split( "." );
					QList<int> verInts;
					foreach( QString part, verParts ) {
						bool ok;
						int i = part.toInt( &ok );
						verInts << (ok ? i : 0);
					}
					while (verInts.count() < 4) {
						verInts << 0;
					}
					Version installedVersion( verInts[0], verInts[1], verInts[2], verInts[3] );
					if( minVersion() < installedVersion &&
						installedVersion < maxVersion())
						return true;
			}
		s.endGroup();
	}
	return false;
#elif defined Q_OS_MAC
	return true;
#endif
}


bool IPluginInfo::isInstalled() const
{
#ifdef Q_OS_WIN
	QSettings s( "HKEY_LOCAL_MACHINE\\SOFTWARE\\Last.fm\\Client\\Plugins", QSettings::NativeFormat );
	return s.childGroups().contains( QString::fromStdString(id()) );
#elif defined (Q_OS_MAC)
	return true;
#endif
}


bool IPluginInfo::canBootstrap() const
{
	return (isPlatformSupported() && 
		   bootstrapType() != NoBootstrap);
}


void IPluginInfo::restartProcess() const
{
#ifdef WIN32
    CKillProcessHelper killer;
    killer.KillProcess( processName().c_str());
#endif //WIN32
}


#endif //QT_VERSION
