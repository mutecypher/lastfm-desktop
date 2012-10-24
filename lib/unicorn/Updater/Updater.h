#ifndef UPDATER_H
#define UPDATER_H

#include <QWidget>

#include "lib/DllExportMacro.h"

namespace qtsparkle { class Updater; }

namespace unicorn
{

class UNICORN_DLLEXPORT Updater : public QWidget
{
    Q_OBJECT
public:
    explicit Updater( QWidget *parent = 0 );
    ~Updater();

public slots:
    void checkForUpdates();

private:
    qtsparkle::Updater* m_updater;
};

}

#endif // UPDATER_H
