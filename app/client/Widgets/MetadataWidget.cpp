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

#include <lastfm/XmlQuery>
#include <lastfm/ws.h>
#include <lastfm/User>

#include "lib/unicorn/widgets/HttpImageWidget.h"
#include "lib/unicorn/widgets/DataBox.h"
#include "lib/unicorn/widgets/DataListWidget.h"
#include "lib/unicorn/widgets/BannerWidget.h"
#include "lib/unicorn/widgets/LfmListViewWidget.h"
#include "lib/unicorn/widgets/Label.h"

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
    ui->trackTags->setAttribute( Qt::WA_LayoutUsesWidgetRect );
    ui->artistTags->setAttribute( Qt::WA_LayoutUsesWidgetRect );

    setTrackDetails( track );

    //setUpdatesEnabled( false );

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
}

MetadataWidget::~MetadataWidget()
{
    delete ui;
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
       const XmlQuery& lfm = reply->readAll();

       //int scrobbles = lfm["artist"]["stats"]["playcount"].text().toInt();
       //int listeners = lfm["artist"]["stats"]["listeners"].text().toInt();
       m_userArtistScrobbles = lfm["artist"]["stats"]["userplaycount"].text().toInt();

       // Update the context now that we have the user track listens
       ui->context->setText( contextString( m_track ) );

       //model.similarArtists->clear();

       #if 0
       foreach(const XmlQuery& e, lfm["artist"]["similar"].children("artist").mid(0,4))
       {
           QListWidgetItem* lwi = new QListWidgetItem( e["name"].text());
           lwi->setData( Qt::DecorationRole, QUrl( e["image size=small"].text()));
         //  model.similarArtists->addArtist( Artist( e ));
       }
       #endif

       foreach(const XmlQuery& e, lfm["artist"]["tags"].children("tag"))
       {
           ui->artistPopTagsList->addWidget( new TagWidget( e["name"].text(), e["url"].text(), this ) );
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

       bio = Label::boldLinkStyle( bio );

       ui->artistBio->append( bio );
       ui->artistBio->updateGeometry();
       QUrl url = lfm["artist"]["image size=large"].text();
       ui->artistBio->loadImage( url );
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
       const XmlQuery& lfm = reply->readAll();

       QList<XmlQuery> tags = lfm["tags"].children("tag").mid(0, 5);

       ui->artistYourTagsFrame->setVisible( tags.count() > 0 );

       foreach( const XmlQuery& e, tags )
           ui->artistYourTagsList->addWidget( new TagWidget( e["name"].text(), e["url"].text(), this ) );
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
       const XmlQuery& lfm = reply->readAll();

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
   QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
   try
   {
       const XmlQuery& lfm = reply->readAll();
       int scrobbles = lfm["album"]["playcount"].text().toInt();
       int listeners = lfm["album"]["listeners"].text().toInt();
       int userListens = lfm["album"]["userplaycount"].text().toInt();

       //ui->track.albumScrobbles->setText(QString("%L1").arg(scrobbles) + " plays (" + QString("%L1").arg(listeners) + " listeners)" + "\n" + QString("%L1").arg(userListens) + " plays in your library");;
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
MetadataWidget::onTrackGotInfo( const QByteArray& data )
{
   try
   {
       XmlQuery lfm(data);
       m_globalTrackScrobbles = lfm["track"]["playcount"].text().toInt();
       int listeners = lfm["track"]["listeners"].text().toInt();
       m_userTrackScrobbles = lfm["track"]["userplaycount"].text().toInt();

       // Update the context now that we have the user track listens
       ui->context->setText( contextString( m_track ) );

       ui->trackUserPlays->setText( QString("%L1").arg(m_userTrackScrobbles));
       ui->trackPlays->setText( QString("%L1").arg(m_globalTrackScrobbles));
       ui->trackListeners->setText( QString("%L1").arg( listeners ));
       ui->albumImage->loadUrl( lfm["track"]["album"]["image size=medium"].text() );
       ui->albumImage->setHref( lfm["track"]["url"].text());
       ui->scrobbleControls->setLoveChecked( lfm["track"]["userloved"].text() == "1" );

       foreach(const XmlQuery& e, lfm["track"]["toptags"].children("tag").mid(0, 5 ))
       {
           ui->trackPopTagsList->addWidget( new TagWidget( e["name"].text(), e["url"].text(), this ) );

       }

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
   QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

   try
   {
       const XmlQuery& lfm = reply->readAll();

       QList<XmlQuery> tags = lfm["tags"].children("tag").mid(0, 5);

       ui->trackYourTagsFrame->setVisible( tags.count() > 0 );

       foreach( const XmlQuery& e, tags )
           ui->trackYourTagsList->addWidget( new TagWidget( e["name"].text(), e["url"].text(), this ) );
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

QString userLibraryLink( const QString& user, const QString& artist )
{
   return QString("http://www.last.fm/user/%1/library/music/%2").arg( user, artist );
}

QString userLibrary( const QString& user, const QString& artist )
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


QString
MetadataWidget::scrobbleString( const Track& track )
{
   QString artistString = Label::anchor( userLibraryLink( User().name(), track.artist().toString()  ), track.artist().toString()  );
   QString userArtistScrobblesString = tr( m_userArtistScrobbles == 1 ? "%L1 time" : "%L1 times" ).arg( m_userArtistScrobbles );
   QString userTrackScrobblesString = tr( m_userTrackScrobbles == 1 ? "%L1 time" : "%L1 times" ).arg( m_userTrackScrobbles );

   return tr( "You've listened to %1 %2 and this track %3." ).arg( artistString, userArtistScrobblesString, userTrackScrobblesString );
}

void
MetadataWidget::setBackButtonVisible( bool visible )
{
   ui->context->setText( contextString( m_track ) );

   ui->back->setVisible( visible );
   ui->trackStats->setVisible( visible );
   ui->context->setVisible( !visible );
   ui->scrobbleControls->ui.love->setVisible( visible );
}
