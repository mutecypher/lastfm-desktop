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

#include "ScrobbleInfoWidget.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QTextBrowser>
#include <QDesktopServices>
#include <QNetworkReply>
#include <QApplication>
#include <QScrollArea>
#include <QListView>
#include <QDesktopServices>
#include <QAbstractTextDocumentLayout>
#include <QTextFrame>
#include <lastfm/XmlQuery>
#include <lastfm/ws.h>
#include "../Application.h"
#include "lib/unicorn/widgets/HttpImageWidget.h"
#include "lib/unicorn/widgets/DataBox.h"
#include "lib/unicorn/widgets/DataListWidget.h"
#include "lib/unicorn/widgets/BannerWidget.h"
#include "lib/unicorn/widgets/LfmListViewWidget.h"
#include <QDebug>
#include <lastfm/User>



ScrobbleInfoWidget::ScrobbleInfoWidget( QWidget* p )
                   :StylableWidget( p )
{
    //ui.setupUi( this );
    
    setupUi();
    
    ui.onTourBanner = new BannerWidget( tr("On Tour"), ui.contents );
    ui.onTourBanner->setCursor( Qt::PointingHandCursor );
    ui.onTourBanner->hide();

    ui.similarArtists->setAttribute( Qt::WA_MacShowFocusRect, false );
    ui.listeningNow->setAttribute( Qt::WA_MacShowFocusRect, false );

    ui.listeningNow->setSpacing( 3 );
    ui.similarArtists->setSpacing( 3 );

    ui.similarArtists->setModel( model.similarArtists = new LfmListModel());
    ui.listeningNow->setModel( model.listeningNow = new LfmListModel());
    
    ui.similarArtists->setItemDelegate( new LfmDelegate( ui.similarArtists ));
    ui.listeningNow->setItemDelegate( new LfmDelegate( ui.listeningNow ));

    connect( ui.similarArtists, SIGNAL( clicked( QModelIndex )), SLOT( listItemClicked( QModelIndex )));
    connect( ui.listeningNow, SIGNAL( clicked( QModelIndex )), SLOT( listItemClicked( QModelIndex )));

    connect( qApp, SIGNAL( trackStarted( Track, Track)), SLOT( onTrackStarted( Track, Track )));
    connect(ui.bioText->document()->documentLayout(), SIGNAL( documentSizeChanged(QSizeF)), SLOT( onBioChanged(QSizeF)));
    connect(ui.bioText, SIGNAL(anchorClicked(QUrl)), SLOT(onAnchorClicked(QUrl)));
}

void 
ScrobbleInfoWidget::setupUi()
{
    ui.scrollArea = new QScrollArea( this );
    
    ui.contents = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout( ui.contents );
    
    ui.scrollArea->setWidget( ui.contents );
    ui.scrollArea->setWidgetResizable( true );

    QWidget* titleBox = new QWidget();
    {
        QHBoxLayout* layout = new QHBoxLayout( titleBox );
        
        QVBoxLayout* vl = new QVBoxLayout();
        vl->addWidget( ui.title1 = new QLabel());
        ui.title1->setObjectName( "title1" );
        ui.title1->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
        ui.title1->setOpenExternalLinks( true );

        vl->addWidget( ui.title2 = new QLabel());
        ui.title2->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
        ui.title2->setObjectName( "title2" );
        ui.title2->setOpenExternalLinks( true );
        vl->addStretch();
        
        layout->addLayout( vl , 1);
        layout->addStretch();
        layout->addWidget( ui.artistImage = new HttpImageWidget(), 1);
        ui.artistImage->setObjectName("artistImage");
        layout->setContentsMargins( 0, 0, 0, 0 );
        layout->setSpacing( 0 );
    }
    layout->addWidget(titleBox);

    QWidget* scrobbles = new QWidget();
    {
        QGridLayout* layout = new QGridLayout( scrobbles );
        QLabel* label,* label_2;
        layout->addWidget( label = new QLabel( tr( "Yours:" )), 0, 0 );
        layout->addWidget( label_2 = new QLabel( tr( "Total:" )), 1, 0 );
        label->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
        label_2->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );

        layout->addWidget( ui.yourScrobbles = new QLabel(), 0, 1 );
        layout->addWidget( ui.totalScrobbles = new QLabel(), 1, 1 );
        layout->setContentsMargins( 0, 0, 0, 0 );
        layout->setSpacing( 0 );
    }
    DataBox* scrobBox = new DataBox( tr( "Scrobbles for this track" ), scrobbles );
    scrobBox->setObjectName( "scrobbles" );
    layout->addWidget( scrobBox );

    QWidget* tags = new QWidget();
    {
        QGridLayout* layout = new QGridLayout( tags );
        QLabel* label,* label_2;
        layout->addWidget( label = new QLabel( tr( "Yours:" )), 0, 0 );
        layout->addWidget( label_2 = new QLabel( tr( "Popular:" )), 1, 0 );
        label->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
        label_2->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );

        layout->addWidget( ui.yourTags = new DataListWidget(), 0, 1 );
        layout->addWidget( ui.topTags = new DataListWidget(), 1, 1 );
        layout->setContentsMargins( 0, 0, 0, 0 );
        layout->setSpacing( 0 );
    }
    DataBox* tagsBox = new DataBox( tr( "Tags for this track" ), tags );
    tagsBox->setObjectName( "tags" );
    layout->addWidget( tagsBox );

    QWidget* listeners = new QWidget();
    {
        QVBoxLayout* layout = new QVBoxLayout( listeners );
        layout->addWidget( new QLabel( tr( "People listening to this track right now:" )) );
        layout->addWidget( ui.listeningNow = new LfmListView() );
        layout->setContentsMargins( 0, 0, 0, 0 );
        layout->setSpacing( 0 );
    }
    DataBox* listenersBox = new DataBox( tr( "Listeners" ), listeners );
    listenersBox->setObjectName( "listeners" );
    layout->addWidget( listenersBox );
    
    QWidget* bio = new QWidget();
    {
        QVBoxLayout* layout = new QVBoxLayout( bio );
        layout->addWidget( ui.bioText = new QTextBrowser() );
        layout->setContentsMargins( 0, 0, 0, 0 );
        layout->setSpacing( 0 );
    }
    DataBox* bioBox = new DataBox( tr( "Artist biography" ), bio );
    bioBox->setObjectName( "bio" );
    layout->addWidget( bioBox );

    QWidget* simart = new QWidget();
    {
        QVBoxLayout* layout = new QVBoxLayout( simart );
        layout->addWidget( ui.similarArtists = new LfmListView() );
        layout->setContentsMargins( 0, 0, 0, 0 );
        layout->setSpacing( 0 );
    }
    DataBox* simartBox = new DataBox( tr( "People who listen to this artist also like" ), simart );
    simartBox->setObjectName( "similarArtists" );
    layout->addWidget( simartBox );
    
    layout->addStretch(1);

    new QVBoxLayout( this );
    this->layout()->setContentsMargins( 0, 0, 0, 0 );
    this->layout()->addWidget( ui.scrollArea );
}

void 
ScrobbleInfoWidget::onTrackStarted( const Track& t, const Track& previous )
{
    const unsigned short em_dash = 0x2014;
    QString title = QString("<a class='title' href=\"%1\">%2</a> ") + QChar(em_dash) + " <a class='title' href=\"%3\">%4</a>";
    const unicorn::Application* uApp = qobject_cast<unicorn::Application*>(qApp);

    ui.title1->setText( "<style>" + uApp->loadedStyleSheet() + "</style>" + title.arg(t.artist().www().toString(),
                                                                                      t.artist(),
                                                                                      t.www().toString(),
                                                                                      t.title()));
    if( !t.album().isNull() )
    {
        QString album("from <a class='title' href=\"%1\">%2</a>");
        ui.title2->setText("<style>" + uApp->loadedStyleSheet() + "</style>" + album.arg( t.album().www().toString(),
                                                                                          t.album().title()));
    }
    else
    {
        ui.title2->clear();
    }

    if (t.artist() != previous.artist())
    {
        // it is a different artist so clear anything that is artist specific
        ui.artistImage->clear();
        ui.artistImage->setHref( QUrl());
        ui.bioText->clear();
        ui.onTourBanner->hide();
        model.similarArtists->clear();
    }
    if( t != previous )
    {
        ui.topTags->clear();
        ui.yourTags->clear();
        model.listeningNow->clear();
    }
}


void
ScrobbleInfoWidget::onArtistGotInfo(const XmlQuery& lfm)
{
    int scrobbles = lfm["artist"]["stats"]["playcount"].text().toInt();
    int listeners = lfm["artist"]["stats"]["listeners"].text().toInt();
    int userListens = lfm["artist"]["stats"]["userplaycount"].text().toInt();

    ui.onTourBanner->setHref( QUrl( lfm["artist"]["url"].text() + "/+events" ) );

    model.similarArtists->clear();

    foreach(const XmlQuery& e, lfm["artist"]["similar"].children("artist").mid(0,4))
    {
        #if 0
        QListWidgetItem* lwi = new QListWidgetItem( e["name"].text());
        lwi->setData( Qt::DecorationRole, QUrl( e["image size=small"].text()));
        #endif
        model.similarArtists->addArtist( Artist( e ));
    }

    /*
    foreach(const XmlQuery& e, lfm["artist"]["tags"].children("tag"))
    {
        ui.tags->addItem( e["name"].text(), QUrl(e["url"].text()));
    }
    */

    //ui.artistScrobbles->setText(QString("%L1").arg(scrobbles) + " plays (" + QString("%L1").arg(listeners) + " listeners)" + "\n" + QString("%L1").arg(userListens) + " plays in your library");

    QString stylesheet = ((audioscrobbler::Application*)qApp)->loadedStyleSheet() + styleSheet();
    QString style = "<style>" + stylesheet + "</style>";
    
    //TODO if empty suggest they edit it
    QString bio;
    {
        QStringList bioList = lfm["artist"]["bio"]["summary"].text().trimmed().split( "\r" );
        foreach( const QString& p, bioList )
            bio += "<p>" + p + "</p>";
    }

    ui.bioText->setHtml( style + bio );

    QTextFrame* root = ui.bioText->document()->rootFrame();
    QTextFrameFormat f = root->frameFormat();
    f.setMargin(0);
    root->setFrameFormat(f);

    QUrl url = lfm["artist"]["image size=large"].text();
    ui.artistImage->loadUrl( url );
    ui.artistImage->setHref( lfm["artist"][ "url" ].text());
}


void
ScrobbleInfoWidget::onStopped()
{
    ui.artistImage->clear();
    ui.artistImage->setHref(QUrl());
    ui.title1->clear();
    ui.title2->clear();
    ui.onTourBanner->hide();
}

void
ScrobbleInfoWidget::onArtistGotEvents(const XmlQuery& lfm)
{
    if (lfm["events"].children("event").count() > 0)
    {
        // Display an on tour notification
        ui.onTourBanner->show();
    }
}

void
ScrobbleInfoWidget::onAlbumGotInfo(const XmlQuery& lfm)
{
    int scrobbles = lfm["album"]["playcount"].text().toInt();
    int listeners = lfm["album"]["listeners"].text().toInt();
    int userListens = lfm["album"]["userplaycount"].text().toInt();

    //ui.albumScrobbles->setText(QString("%L1").arg(scrobbles) + " plays (" + QString("%L1").arg(listeners) + " listeners)" + "\n" + QString("%L1").arg(userListens) + " plays in your library");;
}

void
ScrobbleInfoWidget::onTrackGotInfo(const XmlQuery& lfm)
{
    int scrobbles = lfm["track"]["playcount"].text().toInt();
    int listeners = lfm["track"]["listeners"].text().toInt();
    int userListens = lfm["track"]["userplaycount"].text().toInt();

    ui.yourScrobbles->setText( QString("%L1").arg(userListens));
    ui.totalScrobbles->setText( QString("%L1").arg(scrobbles));

    foreach(const XmlQuery& e, lfm["track"]["toptags"].children("tag").mid(0, 5 ))
    {
        ui.topTags->addItem( e["name"].text(), e["url"].text());
    }
}

void
ScrobbleInfoWidget::onTrackGotTopFans(const XmlQuery& lfm)
{
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


void 
ScrobbleInfoWidget::onTrackGotTags( const XmlQuery& lfm )
{
    foreach(const XmlQuery& e, lfm["tags"].children("tag").mid(0, 5))
    {
        ui.yourTags->addItem(e["name"].text(), e["url"].text());
    }
}


void
ScrobbleInfoWidget::onAnchorClicked( const QUrl& link )
{
    QDesktopServices::openUrl( link );
}

void
ScrobbleInfoWidget::onBioChanged( const QSizeF& size )
{
    ui.bioText->setFixedHeight( size.toSize().height() );
}

void
ScrobbleInfoWidget::listItemClicked( const QModelIndex& i )
{
    const QUrl& url = i.data( LfmListModel::WwwRole ).toUrl();
    QDesktopServices::openUrl( url );
}
