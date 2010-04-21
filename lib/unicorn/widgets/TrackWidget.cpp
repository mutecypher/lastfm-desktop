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
#include <QRadioButton>


TrackWidget::TrackWidget( const lastfm::Track& track )
    :m_track( track )
{    
    // the radio buttons layout
    QVBoxLayout* radioButtons = new QVBoxLayout;

    radioButtons->addWidget( ui.trackShare = new QRadioButton( tr("Track"), this ) );
    radioButtons->addWidget( ui.albumShare = new QRadioButton( tr("Album"), this ) );
    radioButtons->addWidget( ui.artistShare = new QRadioButton( tr("Artist"), this ) );

    connect( ui.artistShare, SIGNAL(clicked(bool)), SLOT(onRadioButtonsClicked(bool)) );
    connect( ui.albumShare, SIGNAL(clicked(bool)), SLOT(onRadioButtonsClicked(bool)) );
    connect( ui.trackShare, SIGNAL(clicked(bool)), SLOT(onRadioButtonsClicked(bool)) );

    QHBoxLayout* h = new QHBoxLayout( this );
    h->addLayout( radioButtons );
    h->addWidget( ui.image = new QLabel, Qt::AlignLeft );
    h->addWidget( ui.description = new QLabel, Qt::AlignLeft );

    ui.image->setScaledContents( true );

    // start fetching the image
    m_fetcherAlbum = new TrackImageFetcher( track );
    connect( m_fetcherAlbum, SIGNAL(finished( QImage )), SLOT(onCoverDownloaded( QImage )) );
    m_fetcherAlbum->startAlbum();

    // start fetching the image
    m_fetcherArtist = new TrackImageFetcher( track );
    connect( m_fetcherArtist, SIGNAL(finished( QImage )), SLOT(onArtistDownloaded( QImage )) );
    m_fetcherArtist->startArtist();

    // default the track being selected
    ui.trackShare->setChecked( true );
    onRadioButtonsClicked( true );

    // we sometimes don't know the album name so disable the ablum option in that case
    if ( m_track.album().isNull() ) ui.albumShare->setEnabled( false );

    ui.image->setFixedSize( radioButtons->sizeHint().height(), radioButtons->sizeHint().height() );
}

void
TrackWidget::onRadioButtonsClicked( bool )
{
    // change the share desription to what we are now sharing

    if ( ui.artistShare->isChecked() )
    {
        ui.description->setText( m_track.artist().name() + "\n" );
        ui.image->setPixmap( ui.artistImage );
    }
    else if ( ui.albumShare->isChecked() )
    {
        ui.description->setText( m_track.album().title() + "\n" + m_track.artist().name() );
        ui.image->setPixmap( ui.albumImage );
    }
    else if ( ui.trackShare->isChecked() )
    {
        ui.description->setText(  m_track.title() + " (" + m_track.durationString() + ")\n" + m_track.artist().name() );
        ui.image->setPixmap( ui.artistImage );
    }
}

TrackWidget::Type
TrackWidget::type() const
{
    if ( ui.artistShare->isChecked() )
        return Artist;
    else if ( ui.albumShare->isChecked() )
        return Album;

    return Track;
}

void
TrackWidget::onCoverDownloaded( const QImage& image )
{
    ui.albumImage = QPixmap::fromImage( image );

    onRadioButtonsClicked( true );

    sender()->deleteLater();
}

void
TrackWidget::onArtistDownloaded( const QImage& image )
{
    ui.artistImage = QPixmap::fromImage( image );

    onRadioButtonsClicked( true );

    sender()->deleteLater();
}
