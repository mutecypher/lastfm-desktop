#ifndef PLUGIN_INFO_H_
#define PLUGIN_INFO_H_

#include "lib/unicorn/plugins/Version.h"

#include <lib/DllExportMacro.h>

#include <QString>
#include <QSettings>

#ifdef WIN32
#include <windows.h>
#include <Shlobj.h>
#endif

namespace unicorn
{

class UNICORN_DLLEXPORT IPluginInfo : public QObject
{
    Q_OBJECT
public:
    IPluginInfo( QObject* parent = 0);

    enum BootstrapType{ NoBootstrap = 0, ClientBootstrap, PluginBootstrap };

    bool install() const;


    virtual QString name() const = 0;

    virtual Version version() const = 0;
    Version installedVersion() const;

    virtual QString id() const = 0;
    virtual BootstrapType bootstrapType() const = 0;

    virtual bool isAppInstalled() const = 0;

    virtual QString processName() const = 0;

    // DisplayName string value as found in the HKEY_LM/Software/Microsoft/CurrentVersion/Uninstall/{GUID}/
    virtual QString displayName() const = 0;

    virtual QString pluginInstaller() const = 0;

#ifdef Q_OS_WIN
    static BOOL isWow64();
#endif

    virtual bool isInstalled() const;
    bool canBootstrap() const;

    void setVerbose( bool verbose );

public slots:
    bool doInstall();
    void install( bool install );

protected:
    QString programFilesX86() const;
    QString programFiles64() const;

private:
    bool m_install;
    bool m_verbose;
};

}
#endif //PLUGIN_INFO_H_
