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

#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include "lib/unicorn/dialogs/UnicornDialog.h"

#include <QObject>

#define PAGE_LIST_WIDTH 145

class SettingsDialog : public unicorn::Dialog
{
Q_OBJECT
public:
    SettingsDialog( QWidget *parent = 0 );

signals:
    void saveChanges();

private:
    void setupUi();

private slots:
    void onAccepted();
    void onSettingsChanged();
    void onApplyButtonClicked();

private:
    struct Ui
    {
        class QDialogButtonBox* buttons;
        class QListWidget* pageList;
        class QStackedWidget* pageStack;
        class IpodSettingsWidget* ipodSettings;
        class ScrobbleSettingsWidget* scrobbleSettings;
    }ui;

};

#endif // SETTINGS_DIALOG_H
