/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Jono Cole

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

#include <QLabel>
#include <QVBoxLayout>

#include "IPodScrobbleInfoWidget.h"

IPodScrobbleInfoWidget::IPodScrobbleInfoWidget( const QList<Track>& tracks, QWidget* p )
    :StylableWidget( p )
{
    QVBoxLayout* layout = new QVBoxLayout( this);

    QWidget* welcomeBox = new StylableWidget();
    {
        QHBoxLayout* h1 = new QHBoxLayout( welcomeBox );

        h1->addWidget( ui.iPod = new QLabel( "", this ) );
        ui.iPod->setObjectName("iPod");

        QVBoxLayout* v1 = new QVBoxLayout();

        QString format = (tracks.count() == 1 ?
            tr("%1 track from the iPod \"%2\""):
            tr("%1 tracks from the iPod \"%2\""));

        v1->addWidget( ui.title = new QLabel( format.arg( QString::number(tracks.count()), tracks[0].extra("deviceId") ), this ) );
        ui.title->setObjectName( "title" );

        foreach ( const Track& track, tracks )
        {
            QLabel* trackLabel = new QLabel( track.toString(), this );
            v1->addWidget( trackLabel );
            trackLabel->setObjectName("iPodScrobble");
        }

        v1->addStretch( 1 );
        h1->addLayout( v1 );
        h1->addStretch( 1 );
    }

    layout->addWidget( welcomeBox );
    layout->addStretch( 1 );
}

