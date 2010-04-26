/*
   Copyright 2005-2009 Last.fm Ltd.
      - Primarily authored by Jono Cole and Doug Mansell

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

#include <lastfm/ws.h>

#include "ScrobbleInfoFetcher.h"

ScrobbleInfoFetcher::ScrobbleInfoFetcher(QObject* parent)
    :QObject(parent)
{
}

void
ScrobbleInfoFetcher::onTrackStarted( const Track& t, const Track& oldTrack )
{
    connect(t.getInfo( lastfm::ws::Username , lastfm::ws::SessionKey ), SIGNAL(finished()), SLOT(onTrackGotInfo()));
}


void
ScrobbleInfoFetcher::onTrackGotInfo()
{
    XmlQuery lfm = static_cast<QNetworkReply*>(sender())->readAll();
    emit trackGotInfo(lfm);
}
