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
#include <QAbstractTextDocumentLayout>
#include <QTextFrame>
#include <lastfm/XmlQuery>
#include <lastfm/ws.h>
#include "Application.h"
#include "lib/unicorn/widgets/HttpImageWidget.h"
#include "lib/unicorn/widgets/DataListWidget.h"

#include "lib/unicorn/widgets/BannerWidget.h"

ScrobbleInfoWidget::ScrobbleInfoWidget( QWidget* p )
                   :StylableWidget( p )
{
    ui.setupUi( this );

    ui.bruce = new BannerWidget( tr("On Tour"), ui.artistImage );

    connect( qApp, SIGNAL( trackStarted( Track, Track)), SLOT( onTrackStarted( Track, Track )));
    connect(ui.bioText->document()->documentLayout(), SIGNAL( documentSizeChanged(QSizeF)), SLOT( onBioChanged(QSizeF)));
    connect(ui.bioText, SIGNAL(anchorClicked(QUrl)), SLOT(onAnchorClicked(QUrl)));
}

void 
ScrobbleInfoWidget::onTrackStarted( const Track& t, const Track& previous )
{
    const unsigned short em_dash = 0x2014;
    QString title = QString("<a class='title' href=\"%1\">%2</a> ") + QChar(em_dash) + " <a class='title' href=\"%3\">%4</a>";
    const unicorn::Application* uApp = qobject_cast<unicorn::Application*>(qApp);

    ui.title->setText( "<style>" + uApp->loadedStyleSheet() + "</style>" + title.arg(t.artist().www().toString())
                                                                                .arg(t.artist())
                                                                                .arg(t.www().toString())
                                                                                .arg(t.title()));
    if( !t.album().isNull() )
    {
        QString album("from <a class='title' href=\"%1\">%2</a>");
        ui.album->setText("<style>" + uApp->loadedStyleSheet() + "</style>" + album.arg( t.album().www().toString())
                                                                               .arg( t.album().title()));
    }
    else
    {
        ui.album->clear();
    }

    if (t.artist() != previous.artist())
    {
        // it is a different artist so clear anything that is artist specific
        ui.artistImage->clear();
        ui.artistImage->setHref( QUrl());
        ui.bioText->clear();
        //ui.yourTags->clear();
    }

    //ui.topFans->clear();

    //ui.trackScrobbles->clear();
}


void
ScrobbleInfoWidget::onArtistGotInfo(const XmlQuery& lfm)
{
    int scrobbles = lfm["artist"]["stats"]["playcount"].text().toInt();
    int listeners = lfm["artist"]["stats"]["listeners"].text().toInt();
    int userListens = lfm["artist"]["stats"]["userplaycount"].text().toInt();

    foreach(const XmlQuery& e, lfm["artist"]["similar"].children("artist"))
    {
        //ui.similarArtists->addItem( e["name"].text(), QUrl(e["url"].text()));
    }

    foreach(const XmlQuery& e, lfm["artist"]["tags"].children("tag"))
    {
        //ui.tags->addItem( e["name"].text(), QUrl(e["url"].text()));
    }

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
    f.setMargin(12);
    root->setFrameFormat(f);

    QUrl url = lfm["artist"]["image size=large"].text();
    ui.artistImage->loadUrl( url, true );
    ui.artistImage->setHref( lfm["artist"][ "url" ].text());
}


void
ScrobbleInfoWidget::onStopped()
{
    //ui.bio->clear();
    ui.artistImage->clear();
    ui.artistImage->setHref(QUrl());
    ui.title->clear();
    //ui.tags->clear();
    ui.album->clear();
    //ui.artistScrobbles->clear();
    //ui.albumScrobbles->clear();
    //ui.trackScrobbles->clear();
}


void
ScrobbleInfoWidget::onArtistGotEvents(const XmlQuery& lfm)
{
    if (lfm["events"].children("event").count() > 0)
    {
        // Display an on tour notification
        //ui.onTour->show();
        //ui.onTourBlank->show();
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

    ui.yourScrobbles->setText(QString("%L1").arg(scrobbles) + " plays (" + QString("%L1").arg(listeners) + " listeners)" + "\n" + QString("%L1").arg(userListens) + " plays in your library");;
}

void
ScrobbleInfoWidget::onTrackGotTopFans(const XmlQuery& lfm)
{
    foreach(const XmlQuery& e, lfm["topfans"].children("user").mid(0, 5))
    {
        //ui.topFans->addItem(e["name"].text(), QUrl(e["url"].text()));
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
    ui.bio->setMinimumHeight( size.toSize().height() );
}
