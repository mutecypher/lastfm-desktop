#ifndef CRASHREPORTER_H
#define CRASHREPORTER_H

#include <QObject>

#include "lib/DllExportMacro.h"

namespace unicorn
{

class UNICORN_DLLEXPORT CrashReporter : public QObject
{
    Q_OBJECT
public:
    explicit CrashReporter(QObject *parent = 0);
    ~CrashReporter();
    
signals:
    
public slots:
    
};

}

#endif // CRASHREPORTER_H
