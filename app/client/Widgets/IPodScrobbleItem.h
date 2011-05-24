/*
   Copyright 2005-2009 Last.fm Ltd.
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

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

#ifndef IPOD_SCROBBLE_ITEM_H_
#define IPOD_SCROBBLE_ITEM_H_

#include <QWidget>

#include <lastfm/Track>

#include "ActivityListItem.h"

class IPodScrobbleItem : public ActivityListItem
{
    Q_OBJECT
public:
    IPodScrobbleItem( const QList<Track>& tracks );
    IPodScrobbleItem( const QDomElement& element );

    QWidget* infoWidget() const;

private:
    QDomElement toDomElement( QDomDocument xml ) const;
    void finishUi();

private:
    QWidget* m_info;
    QList<Track> m_tracks;
};


#endif // IPOD_SCROBBLE_ITEM_H_

