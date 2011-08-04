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
#include "lib/unicorn/widgets/HttpImageWidget.h"
#include "lib/unicorn/widgets/DataBox.h"
#include "lib/unicorn/widgets/DataListWidget.h"
#include "lib/unicorn/widgets/BannerWidget.h"
#include "lib/unicorn/widgets/LfmListViewWidget.h"
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
     m_showBack( showBack )
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

    connect(ui.artist.bio->document()->documentLayout(), SIGNAL( documentSizeChanged(QSizeF)), SLOT( onBioChanged(QSizeF)));
    connect(ui.artist.bio, SIGNAL(anchorClicked(QUrl)), SLOT(onAnchorClicked(QUrl)));

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

    //track.www().toString()
    ui.track.title->setText( track.title( Track::Corrected ));
    //track.artist( Track::Corrected ).www()
    ui.track.artist->setText( tr("by %1").arg(track.artist( Track::Corrected )));
    ui.artist.artist->setText( track.artist( Track::Corrected ));
    //track.album( Track::Corrected ).www().toString()
    ui.track.album->setText( tr("from %1").arg(track.album( Track::Corrected )));
   
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
    const XmlQuery& lfm = reply->readAll();
    //int scrobbles = lfm["artist"]["stats"]["playcount"].text().toInt();
    //int listeners = lfm["artist"]["stats"]["listeners"].text().toInt();
    //int userListens = lfm["artist"]["stats"]["userplaycount"].text().toInt();

    //model.similarArtists->clear();

    foreach(const XmlQuery& e, lfm["artist"]["similar"].children("artist").mid(0,4))
    {
        #if 0
        QListWidgetItem* lwi = new QListWidgetItem( e["name"].text());
        lwi->setData( Qt::DecorationRole, QUrl( e["image size=small"].text()));
        #endif
      //  model.similarArtists->addArtist( Artist( e ));
    }

    /*
    foreach(const XmlQuery& e, lfm["artist"]["tags"].children("tag"))
    {
        ui.tags->addItem( e["name"].text(), QUrl(e["url"].text()));
    }
    */

    //ui.track.artistScrobbles->setText(QString("%L1").arg(scrobbles) + " plays (" + QString("%L1").arg(listeners) + " listeners)" + "\n" + QString("%L1").arg(userListens) + " plays in your library");

    
    QUrl url = lfm["artist"]["image size=large"].text();
    //ui.track.artistImage->loadUrl( url );
    //ui.track.artistImage->setHref( lfm["artist"][ "url" ].text() );

    //TODO if empty suggest they edit it
    QString bio;
    {
        QStringList bioList = lfm["artist"]["bio"]["content"].text().trimmed().split( "\r" );
        foreach( const QString& p, bioList )
            bio += "<p>" + p + "</p>";
    }

    ui.artist.bio->setHtml( bio );
    ui.artist.bio->updateGeometry();
    ui.artist.image->setMaximumSize( QSize( 150, 125 ) );
    ui.artist.image->setAlignment( Qt::AlignTop );
    ui.artist.image->loadUrl( url );
    ui.artist.image->setHref( QUrl(lfm["artist"]["url"].text()));

    ui.artist.bio->insertWidget( ui.artist.banner );
    ui.artist.banner->setHref( QUrl(lfm["artist"]["url"].text()+"/+events"));

    QTextFrame* root = ui.artist.bio->document()->rootFrame();
    QTextFrameFormat f = root->frameFormat();
    f.setMargin(0);
    root->setFrameFormat(f);

    onBioChanged( ui.artist.bio->document()->documentLayout()->documentSize() );

    reply->close();
}

void
MetadataWidget::onArtistGotEvents()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    const XmlQuery& lfm = reply->readAll();

    if (lfm["events"].children("event").count() > 0)
    {
        // Display an on tour notification
        ui.artist.banner->setBannerVisible();
    }
    reply->close();
}

void
MetadataWidget::onAlbumGotInfo()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    const XmlQuery& lfm = reply->readAll();
    int scrobbles = lfm["album"]["playcount"].text().toInt();
    int listeners = lfm["album"]["listeners"].text().toInt();
    int userListens = lfm["album"]["userplaycount"].text().toInt();

//    ui.track.albumScrobbles->setText(QString("%L1").arg(scrobbles) + " plays (" + QString("%L1").arg(listeners) + " listeners)" + "\n" + QString("%L1").arg(userListens) + " plays in your library");;
    reply->close();
}

void
MetadataWidget::onTrackGotInfo( const QByteArray& data )
{
    try
    {
        XmlQuery lfm(data);
        m_scrobbles = lfm["track"]["playcount"].text().toInt();
        int listeners = lfm["track"]["listeners"].text().toInt();
        m_userListens = lfm["track"]["userplaycount"].text().toInt();

        ui.track.yourScrobbles->setText( QString("%L1").arg(m_userListens));
        ui.track.totalScrobbles->setText( QString("%L1").arg(m_scrobbles));
        ui.track.listeners->setText( QString("%L1").arg( listeners ));
        ui.track.albumImage->loadUrl( lfm["track"]["album"]["image size=extralarge"].text() );
        ui.track.albumImage->setHref( lfm["track"]["url"].text());
        ui.track.scrobbleControls->setLoveChecked( lfm["track"]["userloved"].text() == "1" );

        foreach(const XmlQuery& e, lfm["track"]["toptags"].children("tag").mid(0, 5 ))
        {
            ui.track.popTags->addItem( e["name"].text(), e["url"].text());
        }
    }
    catch (...)
    {
        // TODO: we were probably fetching info for an unknown track or something
    }
}

void
MetadataWidget::onTrackGotTopFans()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
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
    reply->close();
}

void 
MetadataWidget::onTrackGotYourTags()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    const XmlQuery& lfm = reply->readAll();

    foreach(const XmlQuery& e, lfm["tags"].children("tag").mid(0, 5))
    {
        ui.track.yourTags->addItem(e["name"].text(), e["url"].text());
    }
    
    reply->close();
}


void 
MetadataWidget::onTrackGotPopTags()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    const XmlQuery& lfm = reply->readAll();

    foreach(const XmlQuery& e, lfm["toptags"].children("tag").mid(0, 5))
    {
        ui.track.popTags->addItem(e["name"].text(), e["url"].text());
    }
    
    reply->close();
}


void
MetadataWidget::onAnchorClicked( const QUrl& link )
{
    QDesktopServices::openUrl( link );
}

void
MetadataWidget::onBioChanged( const QSizeF& size )
{
    ui.artist.bio->setFixedHeight( size.toSize().height() );
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
        ui.track.yourScrobbles->setText( QString("%L1").arg( ++m_userListens ));
        ui.track.totalScrobbles->setText( QString("%L1").arg( ++m_scrobbles ));
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
        widget->setStyleSheet( "color: #333;" );
        widget->setContentsMargins( 0, 0, 0, 0 );
        new QVBoxLayout( widget );
        widget->layout()->setSpacing( 0 );
        widget->layout()->addWidget( ui.track.title = new QLabel );
        ui.track.title->setStyleSheet( "font-size: 16pt; font-weight: bold; padding-bottom: 5px;" );
        widget->layout()->addWidget( ui.track.artist = new QLabel );

        ui.track.artist->setStyleSheet( "border: 1px solid #cdcdcd;"
                                  "border-width: 0px 0px 1px 0px;"
                                  "padding-bottom: 11px;" );


        widget->layout()->addWidget( ui.track.scrobbleControls = new ScrobbleControls(m_track) );
        ui.track.scrobbleControls->setStyleSheet( "ScrobbleControls{ border:none;" 
                                            "border-top: 1px solid #ffffff;"
                                            "border-bottom: 1px solid #cdcdcd;"
                                            "padding: 2px 0px; }" );

        widget->layout()->addWidget( ui.track.album = new QLabel );
        ui.track.album->setStyleSheet( "border-top: 1px solid #ffffff; padding-top: 11px; font-size: 11pt;" );
        w->layout()->addWidget( widget );
    }
    w->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );
}

void
MetadataWidget::setupTrackStats( QWidget* w )
{
    w->setStyleSheet( "color: #333;" );
    QGridLayout* wLayout = new QGridLayout( w );

    wLayout->addWidget( ui.track.listeners = new QLabel, 0, 0 );
    wLayout->addWidget( new QLabel( tr("Listeners" )), 1, 0 );
    ui.track.listeners->setStyleSheet( "font-size: 16pt; font-weight:bold;");

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
    ui.track.totalScrobbles->setStyleSheet( "font-size: 16pt; font-weight:bold;");

    wLayout->addWidget( new Divider, 0, 3, 2, 1 );
    
    wLayout->addWidget( ui.track.yourScrobbles = new QLabel, 0, 4 );
    wLayout->addWidget( new QLabel( tr("Plays in your library" )), 1, 4 );
    ui.track.yourScrobbles->setStyleSheet( "font-size: 16pt; font-weight: bold;");
    
    w->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );
}


void
MetadataWidget::setupTrackTags( QWidget* w )
{
    new QVBoxLayout( w );
    w->layout()->setContentsMargins( 0, 0, 0, 0 );
    w->layout()->setSpacing( 0 );
    QLabel* wTitle = new QLabel( tr( "Track Tags"));
    wTitle->setStyleSheet( "color:#333; font-size: 16pt; font-weight: bold;"
                                   "border: 1px solid #cdcdcd; border-width: 0px 0px 1px 0px;"
                                   "padding-bottom: 10px;");
    w->layout()->addWidget( wTitle );
    {
        QWidget* your = new QFrame;
        QHBoxLayout* layout = new QHBoxLayout( your ); QLabel* yourTagsTitle;
        your->setStyleSheet( ".QFrame{ border-top: 1px solid #ffffff; }" );
        layout->addWidget( yourTagsTitle = new QLabel( tr("Your Tags:")), Qt::AlignTop);

        yourTagsTitle->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );

        layout->addWidget( ui.track.yourTags = new DataListWidget);
        layout->setStretch( 1, 1 );
        w->layout()->addWidget( your );
        ui.track.yourTags->setStyleSheet( "border: none; margin: 0px 10px;");
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
        ui.track.popTags->setStyleSheet( "border: none; margin: 0px 10px;");
        w->layout()->addWidget( pop );
    }
}

void 
MetadataWidget::setupUi()
{
    QWidget* contents = new QWidget();
    contents->setStyleSheet( ".QWidget {background-color: #eeeeee;}" );
    QVBoxLayout* mainLayout = new QVBoxLayout( contents );

    QWidget* trackDetails = new QWidget;
    setupTrackDetails( trackDetails );
    
    QWidget* trackStats = new QWidget;
    setupTrackStats( trackStats );

    QWidget* trackTags = new QWidget;
    setupTrackTags( trackTags );

    QWidget* artistBio = new QWidget;
    {
        artistBio->setStyleSheet( ".QWidget {background-color:#dedede;}");
        new QVBoxLayout( artistBio );
        artistBio->setContentsMargins( 0, 0, 0, 0 );
        artistBio->layout()->addWidget( ui.artist.artist = new QLabel());
        ui.artist.artist->setStyleSheet( "font-size: 16pt; font-weight: bold;"
                                         "color: #333; padding-top: 7px;" );
       
        ui.artist.banner = new BannerWidget( tr("On Tour" ));
        ui.artist.banner->setBannerVisible( false );
        artistBio->layout()->addWidget( ui.artist.bio = new TB( artistBio ) );
        ui.artist.bio->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        ui.artist.image = new HttpImageWidget;
        ui.artist.banner->setWidget( ui.artist.image );
        
        QString stylesheet = ((audioscrobbler::Application*)qApp)->loadedStyleSheet() + styleSheet();
        ui.artist.bio->document()->setDefaultStyleSheet( stylesheet );
        ui.artist.bio->setStyleSheet( "background-color: #dedede; border: none; font-size: 12pt; color: #333;" );

    }
    qobject_cast<QBoxLayout*>(artistBio->layout())->addStretch(1);

    contents->layout()->setContentsMargins( 0, 0, 0, 0 );
    contents->layout()->setSpacing( 0 );

    contents->layout()->addWidget( trackDetails );
    contents->layout()->addWidget( trackStats );
    contents->layout()->addWidget( trackTags );
    contents->layout()->addWidget( artistBio );

    
    ui.scrollArea = new QScrollArea( this );
    ui.scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    ui.scrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    ui.scrollArea->setWidget( contents );
    ui.scrollArea->setWidgetResizable( true );

    new QVBoxLayout( this );

    if ( m_showBack )
    {
        ui.backButton = new QPushButton( "Back" );
        connect( ui.backButton, SIGNAL( clicked()), SIGNAL( backClicked()));
        QWidget* pushButtonWidget = new QWidget;
        new QVBoxLayout( pushButtonWidget );
        pushButtonWidget->layout()->addWidget( ui.backButton );
        this->layout()->addWidget( pushButtonWidget );
        ui.backButton->setContentsMargins( 0, 0, 0, 0 );
    }

    this->layout()->addWidget( ui.scrollArea );
    this->layout()->setContentsMargins( 0, 0, 0, 0 );
    this->layout()->setSpacing( 0 );
}
