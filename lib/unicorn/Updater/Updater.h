#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>

#include "lib/DllExportMacro.h"

namespace unicorn
{

class UNICORN_DLLEXPORT Updater : public QObject
{
    Q_OBJECT
public:
    explicit Updater(QObject *parent = 0);

signals:

public slots:
    void checkForUpdates();

private:
};

}

#endif // UPDATER_H
