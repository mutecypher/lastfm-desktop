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

#ifndef SCROBBLE_SETUP_DIALOG_H
#define SCROBBLE_SETUP_DIALOG_H

#include "lib/unicorn/dialogs/UnicornDialog.h"
#include "../MediaDevices/IpodDevice.h"

#include <QDialogButtonBox>

class ScrobbleSetupDialog : public unicorn::Dialog
{
    Q_OBJECT
public:
    ScrobbleSetupDialog( QString deviceId, QString deviceName, QStringList iPodFiles, QWidget* parent = 0 );

signals:
    void clicked( IpodDevice::Scrobble result, QString deviceId, QString deviceName, QStringList iPodFiles );

private slots:
    void onClicked( class QAbstractButton* button );

protected:
    struct
    {
        class QLabel* iPod;
        class QLabel* title;
        class QLabel* description;
        class QCheckBox* dontRemind;
        QDialogButtonBox* buttons;
    }ui;

    QStringList m_iPodFiles;
    QString m_deviceId;
    QString m_deviceName;

};

#endif // SCROBBLE_SETUP_DIALOG_H
