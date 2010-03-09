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
#include "TrackWidget.h"
#include "lib/unicorn/TrackImageFetcher.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QStackedWidget>


TrackWidget::TrackWidget( const lastfm::Track& track )
    :m_track( track )
{    
    QHBoxLayout* h = new QHBoxLayout( this );
    h->addWidget( ui.image = new QLabel );
    h->addWidget( ui.description = new QLabel );

    ui.image->setScaledContents( true );

    // start fetching the image
    m_fetcherAlbum = new TrackImageFetcher( track );
    connect( m_fetcherAlbum, SIGNAL(finished( QImage )), SLOT(onCoverDownloaded( QImage )) );
    m_fetcherAlbum->startAlbum();

    // start fetching the image
    m_fetcherArtist = new TrackImageFetcher( track );
    connect( m_fetcherArtist, SIGNAL(finished( QImage )), SLOT(onArtistDownloaded( QImage )) );
    m_fetcherArtist->startArtist();

    setType( Track );

    setCoverHeight( ui.description->sizeHint().height() );
}

void
TrackWidget::setCoverHeight( int height )
{
    ui.image->setFixedSize( height, height );
}

void
TrackWidget::setType( Type type )
{
    m_type = type;

    switch ( m_type )
    {
        case Artist:
            ui.description->setText( m_track.artist().name() + "\n" );
            ui.image->setPixmap( ui.artistImage );
            break;
        case Album:
            ui.description->setText( m_track.album().title() + "\n" + m_track.artist().name() );
            ui.image->setPixmap( ui.albumImage );
            break;
        case Track:
            ui.description->setText(  m_track.title() + " (" + m_track.durationString() + ")\n" + m_track.artist().name() );
            ui.image->setPixmap( ui.artistImage );
            break;
    }
}

void
TrackWidget::onCoverDownloaded( const QImage& image )
{
    ui.albumImage = QPixmap::fromImage( image );

    setType( m_type );

    sender()->deleteLater();
}

void
TrackWidget::onArtistDownloaded( const QImage& image )
{
    ui.artistImage = QPixmap::fromImage( image );

    setType( m_type );

    sender()->deleteLater();
}
