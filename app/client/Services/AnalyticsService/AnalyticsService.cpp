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

#include <QString>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "lastfm/ws.h"
#include "AnalyticsService.h"


AnalyticsService::AnalyticsService()
{
}


void
AnalyticsService::SendEvent(QString category, QString action, QString label)
{
    QString url =
    QString("http://gaservicelb-1257881933.us-east-1.elb.amazonaws.com/AnalyticsService/track.asmx/SendGAEvent?category=%1&action=%2&label=%3").arg(
                category, action, label);
    lastfm::nam()->get( QNetworkRequest(url));
}
