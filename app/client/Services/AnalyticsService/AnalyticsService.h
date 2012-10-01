/*
   Copyright 2005-2012 Last.fm Ltd.
      - Primarily authored by Frantz Joseph

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
#pragma once

#include <QObject>

#define SETTINGS_CATEGORY "Settings"
#define GENERAL_SETTINGS "General"
#define ACCOUNTS_SETTINGS "Accounts"
#define SCROBBLING_SETTINGS "Scrobbling"
#define ADVANCED_SETTINGS "Advanced"
#define DEVICES_SETTINGS "Devices"

class AnalyticsService : public QObject
{
    Q_OBJECT

public:
    AnalyticsService();
    static AnalyticsService& instance(){ static AnalyticsService a; return a; }

    void SendEvent(QString category, QString action, QString label);
};
