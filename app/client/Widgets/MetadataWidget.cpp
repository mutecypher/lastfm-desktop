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
#include <QDebug>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QTextBrowser>
#include <QDesktopServices>
#include <QNetworkReply>
#include <QApplication>
#include <QScrollArea>
#include <QListView>
#include <QPushButton>
#include <QDesktopServices>
#include <QAbstractTextDocumentLayout>
#include <QTextFrame>
#include <QScrollBar>
#include <QMenu>
#include <QDebug>

#include <lastfm/XmlQuery.h>
#include <lastfm/ws.h>
#include <lastfm/User.h>
#include <lastfm/Track.h>
#include <lastfm/Artist.h>

#include "lib/unicorn/widgets/HttpImageWidget.h"
#include "lib/unicorn/widgets/DataBox.h"
#include "lib/unicorn/widgets/DataListWidget.h"
#include "lib/unicorn/widgets/BannerWidget.h"
#include "lib/unicorn/widgets/LfmListViewWidget.h"
#include "lib/unicorn/widgets/Label.h"
#include "lib/unicorn/layouts/FlowLayout.h"

#include "../Application.h"
#include "../Services/ScrobbleService.h"
#include "../Services/RadioService.h"
#include "ScrobbleControls.h"
#include "BioWidget.h"
#include "TagWidget.h"

#include "MetadataWidget.h"
#include "ui_MetadataWidget.h"


MetadataWidget::MetadataWidget( const Track& track, QWidget* p )
   :StylableWidget( p ),
    ui( new Ui::MetadataWidget ),
    m_track( track ),
    m_globalTrackScrobbles( 0 ),
    m_userTrackScrobbles( 0 ),
    m_userArtistScrobbles( 0 )
{
    ui->setupUi( this );

    ui->scrollArea->setAttribute( Qt::WA_LayoutUsesWidgetRect );
    ui->back->setAttribute( Qt::WA_LayoutUsesWidgetRect );
    ui->trackTagsFrame->setAttribute( Qt::WA_LayoutUsesWidgetRect );

    ui->trackPlays->setAttribute( Qt::WA_LayoutUsesWidgetRect );
    ui->trackPlaysLabel->setAttribute( Qt::WA_LayoutUsesWidgetRect );
    ui->trackUserPlays->setAttribute( Qt::WA_LayoutUsesWidgetRect );
    ui->trackUserPlaysLabel->setAttribute( Qt::WA_LayoutUsesWidgetRect );
    ui->trackListeners->setAttribute( Qt::WA_LayoutUsesWidgetRect );
    ui->trackListenersLabel->setAttribute( Qt::WA_LayoutUsesWidgetRect );

    ui->artistPlays->setAttribute( Qt::WA_LayoutUsesWidgetRect );
    ui->artistPlaysLabel->setAttribute( Qt::WA_LayoutUsesWidgetRect );
    ui->artistUserPlays->setAttribute( Qt::WA_LayoutUsesWidgetRect );
    ui->artistUserPlaysLabel->setAttribute( Qt::WA_LayoutUsesWidgetRect );
    ui->artistListeners->setAttribute( Qt::WA_LayoutUsesWidgetRect );
    ui->artistListenersLabel->setAttribute( Qt::WA_LayoutUsesWidgetRect );

    ui->trackYourTags->setLinkColor( QRgb( 0x008AC7 ) );
    ui->trackPopTags->setLinkColor( QRgb( 0x008AC7 ) );
    ui->artistYourTags->setLinkColor( QRgb( 0x008AC7 ) );
    ui->artistPopTags->setLinkColor( QRgb( 0x008AC7 ) );

    ui->scrobbleControls->setTrack( track );

    setTrackDetails( track );

    ui->albumImage->setPixmap( QPixmap( ":/meta_album_no_art.png" ) );
    ui->artistBio->setPixmap( QPixmap( ":/meta_artist_no_photo.png" ) );

    connect( &ScrobbleService::instance(), SIGNAL(scrobblesCached(QList<lastfm::Track>)), SLOT(onScrobblesCached(QList<lastfm::Track>)));
    connect( track.signalProxy(), SIGNAL(corrected(QString)), SLOT(onTrackCorrected(QString)));

    connect( ui->back, SIGNAL(clicked()), SIGNAL(backClicked()));

    // fetch Track info
    connect( m_track.signalProxy(), SIGNAL( gotInfo(QByteArray)), SLOT( onTrackGotInfo(QByteArray)));

    m_track.getInfo();

    if( !m_track.album().isNull() )
        connect( m_track.album().getInfo(), SIGNAL(finished()), SLOT(onAlbumGotInfo()));

    connect( m_track.artist().getInfo(), SIGNAL(finished()), SLOT(onArtistGotInfo()));

    connect( m_track.getTags(), SIGNAL(finished()), SLOT(onTrackGotYourTags()));
    connect( m_track.artist().getTags(), SIGNAL(finished()), SLOT(onArtistGotYourTags()));
    connect( m_track.artist().getEvents(), SIGNAL(finished()), SLOT(onArtistGotEvents()));

    connect( m_track.getBuyLinks( "united kingdom" /*aApp->currentSession()->userInfo().country()*/ ), SIGNAL(finished()), SLOT(onTrackGotBuyLinks()) );
}

MetadataWidget::~MetadataWidget()
{
    delete ui;
}

void
MetadataWidget::paintEvent( QPaintEvent* event )
{
    StylableWidget::paintEvent( event );

    // draw the arrow on the context
    if ( ui->context->isVisible() )
    {
        static QPixmap arrow( ":/meta_context_arrow.png" );

        QPainter p( this );

        //QPoint arrowPoint = ui->context->geometry().topLeft() - QPoint( 0, arrow.size().height() );
        QPoint arrowPoint(20, 20);

        p.drawPixmap( arrowPoint, arrow );
    }
}

ScrobbleControls*
MetadataWidget::scrobbleControls() const
{
    return ui->scrobbleControls;
}


void
MetadataWidget::onTrackCorrected( QString )
{
   setTrackDetails( m_track );
}

void
MetadataWidget::setTrackDetails( const Track& track )
{
   if ( ui->scrollArea->verticalScrollBar()->isVisible() )
       ui->scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );

   ui->trackTitle->setText( Label::anchor( track.www().toString(), track.title( Track::Corrected ) ) );
   ui->trackArtist->setText( tr("by %1").arg( Label::anchor( track.artist( Track::Corrected ).www().toString(), track.artist( Track::Corrected ))));
   ui->artistArtist->setText( Label::anchor( track.artist( Track::Corrected ).www().toString(),track.artist( Track::Corrected )));

   if ( !m_albumGuess.isNull() )
       ui->trackAlbum->setText( tr("from %1").arg( Label::anchor( m_albumGuess.www().toString(), m_albumGuess)));
   else
   {
       if ( m_track.album().isNull() )
           ui->trackAlbum->setText( tr("from %1").arg( track.album( Track::Corrected )));
       else
           ui->trackAlbum->setText( tr("from %1").arg( Label::anchor( track.album( Track::Corrected ).www().toString(), track.album( Track::Corrected ))));
   }

   ui->radio->setStation( RadioStation::similar( Artist( track.artist().name() ) ), tr( "Play %1 Radio" ).arg( track.artist().name() ) );

   ui->back->setDescription( tr( "%1 by %2" ).arg( track.title(), track.artist().name() ) );

   connect( track.signalProxy(), SIGNAL(loveToggled(bool)), ui->scrobbleControls, SLOT(setLoveChecked(bool)));

   // Add the green astrix to the title, if it has been corrected
   if ( track.corrected() )
   {
       // TODO: The hover text doesn't work at the moment.
       QString toolTip = tr("Auto-corrected from: %1").arg( track.toString( Track::Original ) );
       ui->trackTitle->setText( ui->trackTitle->text() + "<img src=\":/asterisk_small.png\" alt=\"" + toolTip + "\" title=\"" + toolTip + "\" />" );
   }
}

void
MetadataWidget::onArtistGotInfo()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    try
    {
        XmlQuery lfm;
        lfm.parse( reply->readAll() );

        int scrobbles = lfm["artist"]["stats"]["playcount"].text().toInt();
        int listeners = lfm["artist"]["stats"]["listeners"].text().toInt();
        m_userArtistScrobbles = lfm["artist"]["stats"]["userplaycount"].text().toInt();

        ui->artistPlays->setText( tr( "%L1" ).arg( scrobbles ) );
        ui->artistUserPlays->setText( tr( "%L1" ).arg( m_userArtistScrobbles ) );
        ui->artistListeners->setText( tr( "%L1" ).arg( listeners ) );

        // Update the context now that we have the user track listens
        ui->context->setText( contextString( m_track ) );

        {
            // Similar artists!
            QList<XmlQuery> artists = lfm["artist"]["similar"].children("artist").mid( 0, 4 );

            if ( artists.count() != 0 )
            {
                ui->similarArtistFrame->show();
                ui->similarArtists->show();

                QList<HttpImageWidget*> widgets;
                widgets << ui->artist1 << ui->artist2 << ui->artist3 << ui->artist4;

                QRegExp re( "/serve/(\\d*)s?/" );

                for ( int i = 0 ; i < artists.count() ; ++i )
                {
                    widgets[i]->setText( artists[i]["name"].text() );
                    widgets[i]->setToolTip( artists[i]["name"].text() );
                    widgets[i]->loadUrl( artists[i]["image size=medium"].text().replace( re, "/serve/\\1s/" ), HttpImageWidget::ScaleNone );
                    widgets[i]->setHref( artists[i]["url"].text() );
                }

                // "With yeah, blah and more."
                if ( artists.count() == 1 )
                    ui->radio->setDescription( tr( "With %1 and more." ).arg( artists[0]["name"].text() ) );
                else if ( artists.count() >= 2 )
                    ui->radio->setDescription( tr( "With %1, %2 and more.").arg( artists[0]["name"].text(), artists[1]["name"].text() ) );
            }

        }

        QList<XmlQuery> tags =  lfm["artist"]["tags"].children("tag");

        if ( tags.count() == 0 )
            ui->artistTagsFrame->hide();
        else
        {
            QString tagString = tr( "Popular tags:" );

            for ( int i = 0 ; i < tags.count() ; ++i )
            {
                if ( i ==0 )
                    tagString.append( tr( " %1" ).arg( Label::anchor( tags.at(i)["url"].text(), tags.at(i)["name"].text() ) ) );
                else
                    tagString.append( tr( " %1 %2" ).arg( QString::fromUtf8( "・" ), Label::anchor( tags.at(i)["url"].text(), tags.at(i)["name"].text() ) ) );
            }

            ui->artistPopTags->setText( tagString );
        }

        //TODO if empty suggest they edit it
        QString bio;
        {
            QStringList bioList = lfm["artist"]["bio"]["summary"].text().trimmed().split( "\r" );
            foreach( const QString& p, bioList ) {
                QString pTrimmed = p.trimmed();
                if( pTrimmed.isEmpty()) continue;
                bio += "<p>" + pTrimmed + "</p>";
            }
        }

        bio = Label::boldLinkStyle( bio, Qt::black );

        ui->artistBio->append( bio );
        ui->artistBio->updateGeometry();
        QUrl url = lfm["artist"]["image size=extralarge"].text();
        ui->artistBio->loadImage( url, HttpImageWidget::ScaleWidth );
        ui->artistBio->setImageHref( QUrl(lfm["artist"]["url"].text()));
        ui->artistBio->setOnTourVisible( false, QUrl(lfm["artist"]["url"].text()+"/+events"));
   }
   catch ( lastfm::ws::ParseError e )
   {
       // TODO: what happens when we fail?
       qDebug() << e.message() << e.enumValue();
   }
   catch ( lastfm::ws::Error e )
   {
       qDebug() << e;
   }
}

void
MetadataWidget::onArtistGotYourTags()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    try
    {
        XmlQuery lfm;
        lfm.parse( reply->readAll() );

        QList<XmlQuery> tags = lfm["tags"].children("tag").mid(0, 5);

        if ( tags.count() == 0 )
            ui->artistYourTags->hide();
        else
        {
            QString tagString = tr( "Your tags:" );

            for ( int i = 0 ; i < tags.count() ; ++i )
            {
                if ( i ==0 )
                    tagString.append( tr( " %1" ).arg( Label::anchor( tags.at(i)["url"].text(), tags.at(i)["name"].text() ) ) );
                else
                    tagString.append( tr( " %1 %2" ).arg( QString::fromUtf8( "・" ), Label::anchor( tags.at(i)["url"].text(), tags.at(i)["name"].text() ) ) );
            }

            ui->artistYourTags->setText( tagString );
        }
    }
    catch ( lastfm::ws::ParseError e )
    {
       // TODO: what happens when we fail?
       qDebug() << e.message() << e.enumValue();
    }
    catch ( lastfm::ws::Error e )
    {
       qDebug() << e;
    }
}

void
MetadataWidget::onArtistGotEvents()
{
   QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

   try
   {
       XmlQuery lfm;
       lfm.parse( reply->readAll() );

       if (lfm["events"].children("event").count() > 0)
       {
           // Display an on tour notification
           ui->artistBio->setOnTourVisible( true );
       }
   }
   catch ( lastfm::ws::ParseError e )
   {
       // TODO: what happens when we fail?
       qDebug() << e.message() << e.enumValue();
   }
   catch ( lastfm::ws::Error e )
   {
       qDebug() << e;
   }
}

void
MetadataWidget::onAlbumGotInfo()
{
    try
    {
        XmlQuery lfm;
        lfm.parse( qobject_cast<QNetworkReply*>(sender())->readAll() );
//        int scrobbles = lfm["album"]["playcount"].text().toInt();
//        int listeners = lfm["album"]["listeners"].text().toInt();
//        int userListens = lfm["album"]["userplaycount"].text().toInt();

        ui->albumImage->loadUrl( lfm["album"]["image size=large"].text() );
    }
    catch ( lastfm::ws::ParseError e )
    {
       // TODO: what happens when we fail?
       qDebug() << e.message() << e.enumValue();
    }
    catch ( lastfm::ws::Error e )
    {
       qDebug() << e;
    }
}

void
MetadataWidget::onTrackGotBuyLinks()
{
    try
    {
        QByteArray data = qobject_cast<QNetworkReply*>(sender())->readAll() ;
        qDebug() << data;
        XmlQuery lfm;
        lfm.parse( data );

        QMenu* menu = new QMenu( this );

        menu->addAction( tr("Physical") )->setEnabled( false );

        foreach ( const XmlQuery& affiliation, lfm["affiliations"]["physicals"].children( "affiliation" ) )
        {
            bool isSearch = affiliation["isSearch"].text() == "1";

            QAction* buyAction = 0;

            if ( isSearch )
                buyAction = menu->addAction( tr("Search on %1").arg( affiliation["supplierName"].text() ) );
            else
                buyAction = menu->addAction( tr("Buy on %1 %2 %3").arg( affiliation["supplierName"].text(), affiliation["price"]["amount"].text(), affiliation["price"]["currency"].text() ) );

            buyAction->setData( affiliation["buyLink"].text() );
        }

        menu->addSeparator();
        menu->addAction( tr("Downloads") )->setEnabled( false );

        foreach ( const XmlQuery& affiliation, lfm["affiliations"]["downloads"].children( "affiliation" ) )
        {
            bool isSearch = affiliation["isSearch"].text() == "1";

            QAction* buyAction = 0;

            if ( isSearch )
                buyAction = menu->addAction( tr("Search on %1").arg( affiliation["supplierName"].text() ) );
            else
                buyAction = menu->addAction( tr("Buy on %1 %2 %3").arg( affiliation["supplierName"].text(), affiliation["price"]["amount"].text(), affiliation["price"]["currency"].text() ) );

            buyAction->setData( affiliation["buyLink"].text() );
        }

        ui->scrobbleControls->ui.buy->setMenu( menu );

        connect( menu, SIGNAL(triggered(QAction*)), SLOT(onBuyActionTriggered(QAction*)) );
    }
    catch ( lastfm::ws::ParseError e )
    {
        // TODO: what happens when we fail?
        qDebug() << e.message() << e.enumValue();
    }
    catch ( lastfm::ws::Error e )
    {
        qDebug() << e;
    }
}

void
MetadataWidget::onBuyActionTriggered( QAction* buyAction )
{
    QDesktopServices::openUrl( buyAction->data().toString() );
}

void
MetadataWidget::onTrackGotInfo( const QByteArray& data )
{
    try
    {
        XmlQuery lfm;
        lfm.parse( data );

        m_globalTrackScrobbles = lfm["track"]["playcount"].text().toInt();
        int listeners = lfm["track"]["listeners"].text().toInt();
        m_userTrackScrobbles = lfm["track"]["userplaycount"].text().toInt();

        // Update the context now that we have the user track listens
        ui->context->setText( contextString( m_track ) );

        ui->trackUserPlays->setText( QString("%L1").arg(m_userTrackScrobbles));
        ui->trackPlays->setText( QString("%L1").arg(m_globalTrackScrobbles));
        ui->trackListeners->setText( QString("%L1").arg( listeners ));

        //ui->albumImage->loadUrl( lfm["track"]["album"]["image size=medium"].text() );
        ui->albumImage->setHref( lfm["track"]["url"].text());

        ui->scrobbleControls->setLoveChecked( lfm["track"]["userloved"].text() == "1" );

        // get the popular tags
        QList<XmlQuery> tags = lfm["track"]["toptags"].children("tag").mid( 0, 5 );

        if ( tags.count() == 0 )
            ui->trackTagsFrame->hide();
        else
        {
            QString tagString = tr( "Popular tags:" );

            for ( int i = 0 ; i < tags.count() ; ++i )
            {
                if ( i ==0 )
                    tagString.append( tr( " %1" ).arg( Label::anchor( tags.at(i)["url"].text(), tags.at(i)["name"].text() ) ) );
                else
                    tagString.append( tr( " %1 %2" ).arg( QString::fromUtf8( "・" ), Label::anchor( tags.at(i)["url"].text(), tags.at(i)["name"].text() ) ) );
            }

            ui->trackPopTags->setText( tagString );

            // If we don't know the album then get it from this response
            if ( m_track.album().isNull() )
            {
                QString albumTitle = lfm["track"]["album"]["title"].text();

                if ( !albumTitle.isEmpty() )
                {
                    m_albumGuess = lastfm::Album( m_track.artist().name(), albumTitle );
                    connect( m_albumGuess.getInfo(), SIGNAL(finished()), SLOT(onAlbumGotInfo()) );
                    setTrackDetails( m_track );
                }
            }
        }
    }
    catch ( lastfm::ws::ParseError e )
    {
        // TODO: what happens when we fail?
        qDebug() << e.message() << e.enumValue();
    }
    catch ( lastfm::ws::Error e )
    {
        qDebug() << e;
    }
}


void
MetadataWidget::onTrackGotYourTags()
{
    try
    {
        XmlQuery lfm;
        lfm.parse( qobject_cast<QNetworkReply*>(sender())->readAll() );

        QList<XmlQuery> tags = lfm["tags"].children("tag").mid(0, 5);

        if ( tags.count() == 0 )
            ui->trackYourTags->hide();
        else
        {
            QString tagString = tr( "Popular tags:" );

            for ( int i = 0 ; i < tags.count() ; ++i )
            {
                if ( i ==0 )
                    tagString.append( tr( " %1" ).arg( Label::anchor( tags.at(i)["url"].text(), tags.at(i)["name"].text() ) ) );
                else
                    tagString.append( tr( " %1 %2" ).arg( QString::fromUtf8( "・" ), Label::anchor( tags.at(i)["url"].text(), tags.at(i)["name"].text() ) ) );
            }

            ui->trackYourTags->setText( tagString );
        }
    }
    catch ( lastfm::ws::ParseError e )
    {
        // TODO: what happens when we fail?
        qDebug() << e.message() << e.enumValue();
    }
    catch ( lastfm::ws::Error e )
    {
        qDebug() << e;
    }
}


void
MetadataWidget::listItemClicked( const QModelIndex& i )
{
   const QUrl& url = i.data( LfmListModel::WwwRole ).toUrl();
   QDesktopServices::openUrl( url );
}

void
MetadataWidget::onScrobblesCached( const QList<lastfm::Track>& tracks )
{
   foreach ( lastfm::Track track, tracks )
       connect( track.signalProxy(), SIGNAL(scrobbleStatusChanged()), SLOT(onScrobbleStatusChanged()));
}

void
MetadataWidget::onScrobbleStatusChanged()
{

   if (static_cast<lastfm::TrackData*>(sender())->scrobbleStatus == lastfm::Track::Submitted)
   {
       // update total scrobbles and your scrobbles!
       ui->trackUserPlays->setText( QString("%L1").arg( ++m_userTrackScrobbles ));
       ui->trackPlays->setText( QString("%L1").arg( ++m_globalTrackScrobbles ));
       ++m_userArtistScrobbles;

       ui->context->setText( contextString( m_track ) );
   }
}

QString userLibraryLink( const QString& user, const lastfm::Artist& artist )
{
    return QString("http://www.last.fm/user/%1/library/music/%2").arg( user, artist.name() );
}

QString userLibraryLink( const QString& user, const lastfm::Track& track )
{
    return QString("http://www.last.fm/user/%1/library/music/%2/_/%3").arg( user, track.artist( Track::Corrected ).name(), track.title( Track::Corrected ) );
}

QString userLibrary( const QString& user, const lastfm::Artist& artist )
{
   return Label::anchor( userLibraryLink( user, artist ), user );
}

QString
MetadataWidget::getContextString( const Track& track )
{
   QString contextString;

   lastfm::TrackContext context = track.context();

   if ( context.values().count() > 0 )
   {
       switch ( context.type() )
       {
       case lastfm::TrackContext::Artist:
           {
           switch ( context.values().count() )
               {
               default:
               case 1: contextString = tr( "Recommended because you listen to %1." ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ) ); break;
               case 2: contextString = tr( "Recommended because you listen to %1 and %2." ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ), Label::anchor( Artist( context.values().at(1) ).www().toString(), context.values().at(1) ) ); break;
               case 3: contextString = tr( "Recommended because you listen to %1, %2, and %3." ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ) , Label::anchor( Artist( context.values().at(1) ).www().toString(), context.values().at(1) ), Label::anchor( Artist( context.values().at(2) ).www().toString(), context.values().at(2) ) ); break;
               case 4: contextString = tr( "Recommended because you listen to %1, %2, %3, and %4." ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ), Label::anchor( Artist( context.values().at(1) ).www().toString(), context.values().at(1) ), Label::anchor( Artist( context.values().at(2) ).www().toString(), context.values().at(2) ), Label::anchor( Artist( context.values().at(3) ).www().toString(), context.values().at(3) ) ); break;
               case 5: contextString = tr( "Recommended because you listen to %1, %2, %3, %4, and %5." ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ), Label::anchor( Artist( context.values().at(1) ).www().toString(), context.values().at(1) ), Label::anchor( Artist( context.values().at(2) ).www().toString(), context.values().at(2) ), Label::anchor( Artist( context.values().at(3) ).www().toString(), context.values().at(3) ), Label::anchor( Artist( context.values().at(4) ).www().toString(), context.values().at(4) ) ); break;
               }
           }
           break;
       case lastfm::TrackContext::User:
           // Whitelist multi-user station
           if ( !RadioService::instance().station().url().startsWith("lastfm://users/") )
               break;
       case lastfm::TrackContext::Friend:
       case lastfm::TrackContext::Neighbour:
           {
           switch ( context.values().count() )
               {
               default:
               case 1: contextString = tr( "From %2%1s library." ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ) ); break;
               case 2: contextString = tr( "From %2 and %3%1s libraries." ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ), userLibrary( context.values().at(1), track.artist().name() ) ); break;
               case 3: contextString = tr( "From %2, %3, and %4%1s libraries." ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ), userLibrary( context.values().at(1), track.artist().name() ), userLibrary( context.values().at(2), track.artist().name() ) ); break;
               case 4: contextString = tr( "From %2, %3, %4, and %5%1s libraries." ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ),userLibrary(  context.values().at(1), track.artist().name() ), userLibrary( context.values().at(2), track.artist().name() ), userLibrary( context.values().at(3), track.artist().name() ) ); break;
               case 5: contextString = tr( "From %2, %3, %4, %5, and %6%1s libraries." ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ), userLibrary( context.values().at(1), track.artist().name() ), userLibrary( context.values().at(2), track.artist().name() ), userLibrary( context.values().at(3), track.artist().name() ), userLibrary( context.values().at(4), track.artist().name() ) ); break;
               }
           }
           break;
       }
   }
   else
   {
       // There's no context so just give them some scrobble counts
   }

   return contextString;
}


QString
MetadataWidget::contextString( const Track& track )
{
   QString context = getContextString( track );

   if ( context.isEmpty() )
       context = scrobbleString( track );

   return context;
}

QString numberOfTimes( int num )
{
    QString string;

    switch ( num )
    {
    case 1:
        string = QObject::tr( "once" );
        break;
    case 2:
        string = QObject::tr( "twice" );
        break;
    default:
        string = QObject::tr( num == 1 ? "%L1 time" : "%L1 times" ).arg( num );
        break;
    }

    return string;
}

QString
MetadataWidget::scrobbleString( const Track& track )
{
    QString artistString = Label::anchor( userLibraryLink( User().name(), track.artist( Track::Corrected ).toString()  ), track.artist( Track::Corrected )  );
    QString trackString = Label::anchor( userLibraryLink( User().name(), track  ), track.title( Track::Corrected )  );

    QString userArtistScrobblesString = numberOfTimes( m_userArtistScrobbles );
    QString userTrackScrobblesString = numberOfTimes( m_userTrackScrobbles );

    return m_userTrackScrobbles != 0 ?
        tr( "You've listened to %1 %2 and %3 %4." ).arg( artistString, userArtistScrobblesString, trackString, userTrackScrobblesString ):
        tr( "You've listened to %1 %2, but not this track." ).arg( artistString, userArtistScrobblesString );
}

void
MetadataWidget::setBackButtonVisible( bool visible )
{
   ui->context->setText( contextString( m_track ) );

   ui->back->setVisible( visible );
   ui->trackStats->setVisible( visible );
   ui->context->setVisible( !visible );

   ui->scrobbleControls->ui.love->setVisible( visible );

   // keep the love button on for iTunes tracks
   if ( !visible && m_track.source() != Track::LastFmRadio )
       ui->scrobbleControls->ui.love->setVisible( true );
}
