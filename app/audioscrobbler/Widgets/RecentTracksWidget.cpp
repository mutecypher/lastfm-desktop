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

#include <QCoreApplication>
#include <QHBoxLayout>
#include <QIcon>
#include <QFile>
#include <QLabel>
#include <QTimer>

#include <lastfm/misc.h>

#include "lib/unicorn/layouts/AnimatedListLayout.h"

#include "RecentTracksWidget.h"
#include "RecentTrackWidget.h"


const int kNumRecentTracks(15);

RecentTracksWidget::RecentTracksWidget( QString username, QWidget* parent )
    :StylableWidget( parent )
{
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );

    AnimatedListLayout* layout = new AnimatedListLayout( kNumRecentTracks, this );
    layout->setSpacing( 0 );

    connect( layout, SIGNAL(moveFinished()), SLOT(onMoveFinished()));

    setUsername( username );

    m_writeTimer = new QTimer( this );
    m_writeTimer->setSingleShot( true );

    connect( m_writeTimer, SIGNAL(timeout()), SLOT(doWrite()) );
    connect( qApp, SIGNAL(scrobblesCached(QList<lastfm::Track>)), SLOT(onScrobblesCached(QList<lastfm::Track>)));

}


QEasingCurve::Type
RecentTracksWidget::easingCurve() const
{
    return m_easingCurve;
}


void
RecentTracksWidget::setEasingCurve( QEasingCurve::Type easingCurve )
{
    m_easingCurve = easingCurve;
    static_cast<AnimatedListLayout*>(layout())->setEasingCurve( m_easingCurve );
}


void
RecentTracksWidget::disableHover()
{
    for ( int i(0) ; i < layout()->count() ; ++i )
        layout()->itemAt( i )->widget()->setUpdatesEnabled( false );
}

void
RecentTracksWidget::enableHover()
{
    for ( int i(0) ; i < layout()->count() ; ++i )
        layout()->itemAt( i )->widget()->setUpdatesEnabled( true );
}

void
RecentTracksWidget::onTrackChanged()
{
    write();
}

void RecentTracksWidget::setUsername( QString username )
{
    QString path = lastfm::dir::runtimeData().filePath( username + "_recent_tracks.xml" );

    if ( m_path != path )
    {
        m_path = path;
        read();
    }
}

void
RecentTracksWidget::read()
{
    qDebug() << m_path;

    static_cast<AnimatedListLayout*>(layout())->setAnimated( false );

    for ( int i(0) ; i < layout()->count() ; ++i )
    {
        QLayoutItem* item = layout()->takeAt( i );
        delete item->widget();
        delete item;
    }


    QFile file( m_path );
    file.open( QFile::Text | QFile::ReadOnly );
    QTextStream stream( &file );
    stream.setCodec( "UTF-8" );

    QDomDocument xml;
    xml.setContent( stream.readAll() );

    for (QDomNode n = xml.documentElement().lastChild(); !n.isNull(); n = n.previousSibling())
    {
        // TODO: recognise all the other AbstractType classes
        if (n.nodeName() == "track")
        {
            Track* track = new Track( n.toElement() );
            RecentTrackWidget* item = new RecentTrackWidget( *track );
            layout()->addWidget( item );

            connect( item, SIGNAL(cogMenuAboutToHide()), SLOT(enableHover()));
            connect( item, SIGNAL(cogMenuAboutToShow()), SLOT(disableHover()));

            connect( track->signalProxy(), SIGNAL(loveToggled(bool)), SLOT(onTrackChanged()));
            connect( track->signalProxy(), SIGNAL(scrobbleStatusChanged()), SLOT(onTrackChanged()));
        }
    }

    static_cast<AnimatedListLayout*>(layout())->setAnimated( true );
}

void
RecentTracksWidget::write() const
{
    qDebug() << m_path;

    m_writeTimer->start( 200 );
}

void
RecentTracksWidget::doWrite() const
{
    qDebug() << m_path;

    if ( layout()->count() == 0 )
    {
        QFile::remove( m_path );
    }
    else
    {
        QDomDocument xml;
        QDomElement e = xml.createElement( "recent_tracks" );
        e.setAttribute( "product", QCoreApplication::applicationName() );
        e.setAttribute( "version", "2" );

        for ( int i(0) ; i < layout()->count() ; ++i )
            e.appendChild( static_cast<RecentTrackWidget*>(layout()->itemAt(i)->widget())->track().toDomElement( xml ) );

        xml.appendChild( e );

        QFile file( m_path );
        file.open( QIODevice::WriteOnly | QIODevice::Text );

        QTextStream stream( &file );
        stream.setCodec( "UTF-8" );
        stream << "<?xml version='1.0' encoding='utf-8'?>\n";
        stream << xml.toString( 2 );
    }
}

void
RecentTracksWidget::addCachedTrack( const Track& a_track )
{
    RecentTrackWidget* item = new RecentTrackWidget( a_track );
    layout()->addWidget( item );

    connect( a_track.signalProxy(), SIGNAL(loveToggled(bool)), SLOT(onTrackChanged()));
    connect( a_track.signalProxy(), SIGNAL(scrobbleStatusChanged()), SLOT(onTrackChanged()));

    connect( item, SIGNAL(cogMenuAboutToHide()), SLOT(enableHover()));
    connect( item, SIGNAL(cogMenuAboutToShow()), SLOT(disableHover()));

    write();
}

void
RecentTracksWidget::onMoveFinished()
{
    while ( layout()->count() > kNumRecentTracks )
    {
        QLayoutItem* item = layout()->takeAt( layout()->count() - 1 );
        delete item->widget();
        delete item;
    }

   write();
}

void
RecentTracksWidget::onScrobblesCached( const QList<lastfm::Track>& tracks )
{
    foreach ( lastfm::Track track, tracks )
        addCachedTrack( track );
}

