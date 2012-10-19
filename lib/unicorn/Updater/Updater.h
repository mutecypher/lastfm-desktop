
#pragma once

#include <QWidget>

#include "lib/DllExportMacro.h"

namespace qtsparkle { class Updater; }

namespace unicorn
{

class UNICORN_DLLEXPORT Updater : public QObject
{
    Q_OBJECT
public:
    explicit Updater( QWidget *parent = 0 );

public slots:
    void checkForUpdates();

private:
    qtsparkle::Updater* m_updater;
};

}

