/*
   Copyright 2010 Last.fm Ltd.
      - Primarily authored by Jono Cole, Michael Coffey, and William Viana

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SCROBBLESETUPDIALOG_H
#define SCROBBLESETUPDIALOG_H

#include "lib/unicorn/dialogs/UnicornDialog.h"

#include "../MediaDevices/IpodDevice.h"

namespace Ui {
    class ScrobbleSetupDialog;
}

class ScrobbleSetupDialog : public unicorn::Dialog
{
    Q_OBJECT

public:
    explicit ScrobbleSetupDialog( QString deviceId, QString deviceName, QStringList iPodFiles, QWidget* parent = 0 );
    ~ScrobbleSetupDialog();

signals:
    void clicked( bool scrobble, bool alwaysAsk, const QString&, const QString&, const QStringList& );

private slots:
    void onClicked( class QAbstractButton* button );

private:
    Ui::ScrobbleSetupDialog *ui;

    QStringList m_iPodFiles;
    QString m_deviceId;
    QString m_deviceName;
};

#endif // SCROBBLESETUPDIALOG_H
