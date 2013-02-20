#ifndef UNICORNAPPLICATIONDELEGATE_H
#define UNICORNAPPLICATIONDELEGATE_H

#include <QPixmap>

#include "lib/DllExportMacro.h"

namespace unicorn {

class UnicornApplicationDelegateCommandObserver
{
public:
    virtual QString trackTitle() const = 0;
    virtual QString artist() const = 0;
    virtual QString album() const = 0;
    virtual int duration() = 0;
    virtual QPixmap artwork() = 0;
    virtual bool loved() = 0;
};

class UNICORN_DLLEXPORT UnicornApplicationDelegate : public QObject
{
    Q_OBJECT
public:
    explicit UnicornApplicationDelegate( QObject *parent = 0 );

    void showDockIcon( bool show );

    void setCommandObserver( UnicornApplicationDelegateCommandObserver* observer );

    void forceInitialize();
    void forceApplicationDidFinishLaunching( void* aNotification );
    
signals:
    void initialize();
    void applicationDidFinishLaunching( void* aNotification ); /* NSNotification */
    
public:
    UnicornApplicationDelegateCommandObserver* m_observer;
};

}

#endif // UNICORNAPPLICATIONDELEGATE_H
