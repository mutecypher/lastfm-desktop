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

#include "MetadataWidget.h"
#include "ScrobbleControls.h"
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
#include "../Application.h"
#include "../Services/ScrobbleService.h"
#include "../Services/RadioService.h"
#include "BioWidget.h"
#include "lib/unicorn/widgets/HttpImageWidget.h"
#include "lib/unicorn/widgets/DataBox.h"
#include "lib/unicorn/widgets/DataListWidget.h"
#include "lib/unicorn/widgets/BannerWidget.h"
#include "lib/unicorn/widgets/LfmListViewWidget.h"
#include "lib/unicorn/widgets/Label.h"
#include <QDebug>
#include <lastfm/User>


MetadataWidget::MetadataWidget( const Track& track, bool showBack, QWidget* p )
    :StylableWidget( p ), 
     m_track( track ), 
     m_trackInfoReply( 0 ),
     m_albumInfoReply( 0 ),
     m_artistInfoReply( 0 ),
     m_artistEventsReply( 0 ),
     m_trackTopFansReply( 0 ),
     m_trackTagsReply( 0 ),
     m_globalTrackScrobbles( 0 ),
     m_userTrackScrobbles( 0 ),
     m_userArtistScrobbles( 0 )
{
    setupUi();
    
    /*
    ui.similarArtists->setModel( model.similarArtists = new LfmListModel());
    ui.listeningNow->setModel( model.listeningNow = new LfmListModel());
    
    ui.similarArtists->setItemDelegate( new LfmDelegate( ui.similarArtists ));
    ui.listeningNow->setItemDelegate( new LfmDelegate( ui.listeningNow ));

    connect( ui.similarArtists, SIGNAL( clicked( QModelIndex )), SLOT( listItemClicked( QModelIndex )));
    connect( ui.listeningNow, SIGNAL( clicked( QModelIndex )), SLOT( listItemClicked( QModelIndex )));
    */


    setTrackDetails( track );

    connect( &ScrobbleService::instance(), SIGNAL(scrobblesCached(QList<lastfm::Track>)), SLOT(onScrobblesCached(QList<lastfm::Track>)));
    connect( track.signalProxy(), SIGNAL(corrected(QString)), SLOT(onTrackCorrected(QString)));

    fetchTrackInfo();
}

void
MetadataWidget::fetchTrackInfo()
{
    connect( m_track.signalProxy(), SIGNAL( gotInfo(QByteArray)), SLOT( onTrackGotInfo(QByteArray)));
    m_track.getInfo();
    if( !m_track.album().isNull() )
        connect( m_track.album().getInfo(), SIGNAL(finished()), SLOT(onAlbumGotInfo()));
    connect( m_track.artist().getInfo(), SIGNAL(finished()), SLOT(onArtistGotInfo()));
    //connect( m_track.getTopTags(), SIGNAL(finished()), SLOT(onTrackGotPopTags()));
    connect( m_track.getTags(), SIGNAL(finished()), SLOT(onTrackGotYourTags()));
    connect( m_track.artist().getEvents(), SIGNAL(finished()), SLOT(onArtistGotEvents()));
    m_trackTopFansReply = m_track.getTopFans();

}


void
MetadataWidget::onTrackCorrected( QString )
{
    setTrackDetails( m_track );
}

void
MetadataWidget::setTrackDetails( const Track& track )
{
    if ( ui.scrollArea->verticalScrollBar()->isVisible() )
        ui.scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );

    ui.track.title->setText( Label::anchor( track.www().toString(), track.title( Track::Corrected ) ) );
    ui.track.artist->setText( tr("by %1").arg( Label::anchor( track.artist( Track::Corrected ).www().toString(), track.artist( Track::Corrected ))));
    ui.artist.artist->setText( Label::anchor( track.artist( Track::Corrected ).www().toString(),track.artist( Track::Corrected )));

    if ( !m_albumGuess.isNull() )
        ui.track.album->setText( tr("from %1").arg( Label::anchor( m_albumGuess.www().toString(), m_albumGuess)));
    else
    {
        if ( m_track.album().isNull() )
            ui.track.album->setText( tr("from %1").arg( track.album( Track::Corrected )));
        else
            ui.track.album->setText( tr("from %1").arg( Label::anchor( track.album( Track::Corrected ).www().toString(), track.album( Track::Corrected ))));
    }

    connect( track.signalProxy(), SIGNAL(loveToggled(bool)), ui.track.scrobbleControls, SLOT(setLoveChecked(bool)));

    // Add the green astrix to the title, if it has been corrected
    if ( track.corrected() )
    {
        // TODO: The hover text doesn't work at the moment.
        QString toolTip = tr("Auto-corrected from: %1").arg( track.toString( Track::Original ) );
        ui.track.title->setText( ui.track.title->text() + "<img src=\":/asterisk_small.png\" alt=\"" + toolTip + "\" title=\"" + toolTip + "\" />" );
    }
}


void
MetadataWidget::onFinished()
{
    if( (void*)sender() == (void*)m_albumInfoReply )
        qDebug() << "AlbumInfoReply";
    if( sender() == m_artistInfoReply )
        qDebug() << "ArtistInfoReply";
    if( sender() == m_trackTopFansReply ) 
        qDebug() << "TrackTopFansReply";
    if( sender() == m_artistEventsReply )
        qDebug() << "ArtistEventsReply";
    if( sender() == m_trackTagsReply )
        qDebug() << "TrackTagsReply";

    if( 
        ( m_albumInfoReply && m_albumInfoReply->isOpen()) ||
        ( m_artistInfoReply && m_artistInfoReply->isOpen()) ||
        ( m_artistEventsReply && m_artistEventsReply->isOpen()) ||
        ( m_trackTopFansReply && m_trackTopFansReply->isOpen()) ||
        ( m_trackTagsReply && m_trackTagsReply->isOpen() )) 
    { qDebug() << "Remaining unserviced requests.."; return; }
    
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
        ui.track.context->setText( contextString( m_track ) );

        //model.similarArtists->clear();

        #if 0
        foreach(const XmlQuery& e, lfm["artist"]["similar"].children("artist").mid(0,4))
        {
            QListWidgetItem* lwi = new QListWidgetItem( e["name"].text());
            lwi->setData( Qt::DecorationRole, QUrl( e["image size=small"].text()));
          //  model.similarArtists->addArtist( Artist( e ));
        }
        #endif

        /*
        foreach(const XmlQuery& e, lfm["artist"]["tags"].children("tag"))
        {
            ui.tags->addItem( e["name"].text(), QUrl(e["url"].text()));
        }
        */


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
        ui.artist.bio->append( bio );
        ui.artist.bio->updateGeometry();
        QUrl url = lfm["artist"]["image size=large"].text();
        ui.artist.bio->loadImage( url );
        ui.artist.bio->setImageHref( QUrl(lfm["artist"]["url"].text()));
        ui.artist.bio->setOnTourVisible( false, QUrl(lfm["artist"]["url"].text()+"/+events"));
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

    reply->close();
    
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
            ui.artist.bio->setOnTourVisible( true );
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

    reply->close();
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

        //ui.track.albumScrobbles->setText(QString("%L1").arg(scrobbles) + " plays (" + QString("%L1").arg(listeners) + " listeners)" + "\n" + QString("%L1").arg(userListens) + " plays in your library");;
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

    reply->close();
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
        ui.track.context->setText( contextString( m_track ) );

        ui.track.yourScrobbles->setText( QString("%L1").arg(m_userTrackScrobbles));
        ui.track.totalScrobbles->setText( QString("%L1").arg(m_globalTrackScrobbles));
        ui.track.listeners->setText( QString("%L1").arg( listeners ));
        ui.track.albumImage->loadUrl( lfm["track"]["album"]["image size=extralarge"].text() );
        ui.track.albumImage->setHref( lfm["track"]["url"].text());
        ui.track.scrobbleControls->setLoveChecked( lfm["track"]["userloved"].text() == "1" );

        foreach(const XmlQuery& e, lfm["track"]["toptags"].children("tag").mid(0, 5 ))
        {
            ui.track.popTags->addItem( e["name"].text(), e["url"].text());
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
MetadataWidget::onTrackGotTopFans()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    try
    {
        const XmlQuery& lfm = reply->readAll();

        model.listeningNow->clear();

        QList<XmlQuery> users = lfm["topfans"].children("user");

        // Add users that have avatars
        foreach(const XmlQuery& e, users)
        {
            if (model.listeningNow->rowCount() == 4)
                break;

            User u(e);
            if ( !u.imageUrl( lastfm::Small ).toString().isEmpty() )
                model.listeningNow->addUser(u);
        }

        // If we still haven't got 4, add the ones that don't have avatars
        foreach(const XmlQuery& e, users)
        {
            if (model.listeningNow->rowCount() == 4)
                break;

            User u(e);
            if ( u.imageUrl( lastfm::Small ).toString().isEmpty() )
                model.listeningNow->addUser(u);
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

    reply->close();
}

void 
MetadataWidget::onTrackGotYourTags()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    try
    {
        const XmlQuery& lfm = reply->readAll();

        foreach(const XmlQuery& e, lfm["tags"].children("tag").mid(0, 5))
        {
            ui.track.yourTags->addItem(e["name"].text(), e["url"].text());
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
    
    reply->close();
}


void 
MetadataWidget::onTrackGotPopTags()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    try
    {
        const XmlQuery& lfm = reply->readAll();

        foreach(const XmlQuery& e, lfm["toptags"].children("tag").mid(0, 5))
        {
            ui.track.popTags->addItem(e["name"].text(), e["url"].text());
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
    
    reply->close();
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
        ui.track.yourScrobbles->setText( QString("%L1").arg( ++m_userTrackScrobbles ));
        ui.track.totalScrobbles->setText( QString("%L1").arg( ++m_globalTrackScrobbles ));
        ++m_userArtistScrobbles;

        ui.track.context->setText( contextString( m_track ) );
    }
}

void 
MetadataWidget::setupTrackDetails( QWidget* w )
{
    new QHBoxLayout( w );   
    w->layout()->addWidget( ui.track.albumImage = new HttpImageWidget );
    ui.track.albumImage->setMaximumSize( QSize( 126, 126 ));
    {
        QWidget* widget = new QWidget;
        widget->setContentsMargins( 0, 0, 0, 0 );
        new QVBoxLayout( widget );
        widget->layout()->setSpacing( 0 );
       
        widget->layout()->addWidget( ui.track.title = new QLabel );
        ui.track.title->setWordWrap( true );
        ui.track.title->setObjectName( "trackTitle" );
        ui.track.title->setOpenExternalLinks( true );

        widget->layout()->addWidget( ui.track.artist = new QLabel );
        ui.track.artist->setWordWrap( true );
        ui.track.artist->setObjectName( "trackArtist" );
        ui.track.artist->setOpenExternalLinks( true );

        widget->layout()->addWidget( ui.track.scrobbleControls = new ScrobbleControls(m_track) );
        ui.track.scrobbleControls->setObjectName( "scrobbleControls" );

        widget->layout()->addWidget( ui.track.album = new QLabel );
        ui.track.album->setWordWrap( true );
        ui.track.album->setObjectName( "trackAlbum" );
        ui.track.album->setOpenExternalLinks( true );

        w->layout()->addWidget( widget );
    }
    w->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );
}

void
MetadataWidget::setupTrackStats( QWidget* w )
{
    QGridLayout* wLayout = new QGridLayout( w );

    wLayout->addWidget( ui.track.listeners = new QLabel, 0, 0 );
    wLayout->addWidget( new QLabel( tr("Listeners" )), 1, 0 );
    ui.track.listeners->setObjectName( "trackListeners" );

    struct Divider : QFrame {
        Divider() {
            setFrameShape( QFrame::VLine );
            setFrameShadow( QFrame::Raised );
            setLineWidth( 1 );
            setMidLineWidth( 1 );
            QPalette p;
            p.setColor( QPalette::Light, Qt::transparent );
            p.setColor( QPalette::Mid, 0xcdcdcd );
            p.setColor( QPalette::Dark, Qt::white );
            setPalette( p );
        }
    };
    
    wLayout->addWidget( new Divider, 0, 1, 2, 1 );
    wLayout->addWidget( ui.track.totalScrobbles = new QLabel, 0, 2 );
    wLayout->addWidget( new QLabel( tr("Plays" )), 1, 2 );
    ui.track.totalScrobbles->setObjectName( "trackTotalScrobbles" );

    wLayout->addWidget( new Divider, 0, 3, 2, 1 );
    
    wLayout->addWidget( ui.track.yourScrobbles = new QLabel, 0, 4 );
    wLayout->addWidget( new QLabel( tr("Plays in your library" )), 1, 4 );
    ui.track.yourScrobbles->setObjectName( "trackYourScrobbles" );
    
    w->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );
}


void
MetadataWidget::setupTrackTags( QWidget* w )
{
    new QVBoxLayout( w );
    w->layout()->setContentsMargins( 0, 0, 0, 0 );
    w->layout()->setSpacing( 0 );
    QLabel* wTitle = new QLabel( tr( "Track Tags"));
    wTitle->setObjectName( "trackTagsTitle" );

    w->layout()->addWidget( wTitle );
    {
        QWidget* your = new QFrame;
        QHBoxLayout* layout = new QHBoxLayout( your ); QLabel* yourTagsTitle;
        your->setObjectName( "your" );
        layout->addWidget( yourTagsTitle = new QLabel( tr("Your Tags:")), Qt::AlignTop);

        yourTagsTitle->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );

        layout->addWidget( ui.track.yourTags = new DataListWidget);
        layout->setStretch( 1, 1 );
        w->layout()->addWidget( your );
        ui.track.yourTags->setObjectName( "trackYourTags" );
    }
    {
        QWidget* pop = new QFrame;
        QHBoxLayout* layout = new QHBoxLayout( pop );
        QLabel* popTagsTitle;
        layout->addWidget( popTagsTitle = new QLabel( tr("Popular Tags:")), Qt::AlignTop);
        popTagsTitle->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
        layout->addWidget( popTagsTitle, Qt::AlignLeft );

        layout->addWidget( ui.track.popTags = new DataListWidget);
        layout->setStretch( 1, 1 );
        ui.track.popTags->setObjectName( "trackPopTags" );
        w->layout()->addWidget( pop );
    }
}

void 
MetadataWidget::setupUi()
{
    QWidget* contents = new QWidget();
    contents->setObjectName( "contents" );
    QVBoxLayout* mainLayout = new QVBoxLayout( contents );

    QWidget* trackDetails = new QWidget;
    trackDetails->setObjectName( "trackDetails" );
    setupTrackDetails( trackDetails );
    
    ui.track.trackStats = new QWidget;
    setupTrackStats( ui.track.trackStats );

    QWidget* trackTags = new QWidget;
    setupTrackTags( trackTags );

    QWidget* artistBio = new QWidget;
    {
        artistBio->setObjectName( "artistBio");
        new QVBoxLayout( artistBio );
        artistBio->setContentsMargins( 0, 0, 0, 0 );
        artistBio->layout()->addWidget( ui.artist.artist = new QLabel());
        ui.artist.artist->setObjectName( "artistArtist" );
        ui.artist.artist->setOpenExternalLinks( true );
       
        artistBio->layout()->addWidget( ui.artist.bio = new BioWidget( artistBio ) );
        
        QString stylesheet = ((audioscrobbler::Application*)qApp)->loadedStyleSheet() + styleSheet();
        ui.artist.bio->document()->setDefaultStyleSheet( stylesheet );
        ui.artist.bio->setObjectName( "bio" );

    }
    qobject_cast<QBoxLayout*>(artistBio->layout())->addStretch(1);

    contents->layout()->setContentsMargins( 0, 0, 0, 0 );
    contents->layout()->setSpacing( 0 );

    contents->layout()->addWidget( trackDetails );
    contents->layout()->addWidget( ui.track.trackStats );
    contents->layout()->addWidget( ui.track.context = new QLabel( this ) );
    ui.track.context->hide();
    ui.track.context->setObjectName( "context" );
    ui.track.context->setOpenExternalLinks( true );
    ui.track.context->setWordWrap( true );
    contents->layout()->addWidget( trackTags );
    contents->layout()->addWidget( artistBio );

    
    ui.scrollArea = new QScrollArea( this );
    ui.scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    ui.scrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    ui.scrollArea->setWidget( contents );
    ui.scrollArea->setWidgetResizable( true );

    new QVBoxLayout( this );

    ui.backButton = new QPushButton( "Back" );
    connect( ui.backButton, SIGNAL( clicked()), SIGNAL( backClicked()));
    QWidget* pushButtonWidget = new QWidget;
    new QVBoxLayout( pushButtonWidget );
    pushButtonWidget->layout()->addWidget( ui.backButton );
    this->layout()->addWidget( pushButtonWidget );
    ui.backButton->setContentsMargins( 0, 0, 0, 0 );

    this->layout()->addWidget( ui.scrollArea );
    this->layout()->setContentsMargins( 0, 0, 0, 0 );
    this->layout()->setSpacing( 0 );
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
                case 1: contextString = tr( "Recommended because you listen to %1" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ) ); break;
                case 2: contextString = tr( "Recommended because you listen to %1 and %2" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ), Label::anchor( Artist( context.values().at(1) ).www().toString(), context.values().at(1) ) ); break;
                case 3: contextString = tr( "Recommended because you listen to %1, %2, and %3" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ) , Label::anchor( Artist( context.values().at(1) ).www().toString(), context.values().at(1) ), Label::anchor( Artist( context.values().at(2) ).www().toString(), context.values().at(2) ) ); break;
                case 4: contextString = tr( "Recommended because you listen to %1, %2, %3, and %4" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ), Label::anchor( Artist( context.values().at(1) ).www().toString(), context.values().at(1) ), Label::anchor( Artist( context.values().at(2) ).www().toString(), context.values().at(2) ), Label::anchor( Artist( context.values().at(3) ).www().toString(), context.values().at(3) ) ); break;
                case 5: contextString = tr( "Recommended because you listen to %1, %2, %3, %4, and %5" ).arg( Label::anchor( Artist( context.values().at(0) ).www().toString(), context.values().at(0) ), Label::anchor( Artist( context.values().at(1) ).www().toString(), context.values().at(1) ), Label::anchor( Artist( context.values().at(2) ).www().toString(), context.values().at(2) ), Label::anchor( Artist( context.values().at(3) ).www().toString(), context.values().at(3) ), Label::anchor( Artist( context.values().at(4) ).www().toString(), context.values().at(4) ) ); break;
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
                case 1: contextString = tr( "From %2%1s library" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ) ); break;
                case 2: contextString = tr( "From %2 and %3%1s libraries" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ), userLibrary( context.values().at(1), track.artist().name() ) ); break;
                case 3: contextString = tr( "From %2, %3, and %4%1s libraries" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ), userLibrary( context.values().at(1), track.artist().name() ), userLibrary( context.values().at(2), track.artist().name() ) ); break;
                case 4: contextString = tr( "From %2, %3, %4, and %5%1s libraries" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ),userLibrary(  context.values().at(1), track.artist().name() ), userLibrary( context.values().at(2), track.artist().name() ), userLibrary( context.values().at(3), track.artist().name() ) ); break;
                case 5: contextString = tr( "From %2, %3, %4, %5, and %6%1s libraries" ).arg( QChar( 0x2019 ), userLibrary( context.values().at(0), track.artist().name() ), userLibrary( context.values().at(1), track.artist().name() ), userLibrary( context.values().at(2), track.artist().name() ), userLibrary( context.values().at(3), track.artist().name() ), userLibrary( context.values().at(4), track.artist().name() ) ); break;
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
    ui.track.context->setText( contextString( m_track ) );

    ui.backButton->parentWidget()->setVisible( visible );
    ui.track.trackStats->setVisible( visible );
    ui.track.context->setVisible( !visible );
}
