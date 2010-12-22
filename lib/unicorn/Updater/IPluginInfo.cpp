
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
	pluginInstallPath();
	return true;
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
