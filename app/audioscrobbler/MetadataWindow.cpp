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
#include "MetadataWindow.h"

#include "ScrobbleStatus.h"
#include "ScrobbleControls.h"
#include "Application.h"
#include "RestWidget.h"
#include "lib/unicorn/widgets/DataListWidget.h"
#include "lib/unicorn/widgets/MessageBar.h"
#include "lib/unicorn/StylableWidget.h"
#include "lib/unicorn/qtwin.h"

#include <lastfm/Artist>
#include <lastfm/XmlQuery>
#include <lastfm/ws.h>

#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QTextBrowser>
#include <QNetworkReply>
#include <QTextFrame>
#include <QVBoxLayout>
#include <QStackedLayout>
#include <QScrollArea>
#include <QStatusBar>
#include <QSizeGrip>
#include <QDesktopServices>
#include <QAbstractTextDocumentLayout>

MetadataWindow::MetadataWindow()
{
    setWindowFlags( Qt::Tool );
    setAttribute( Qt::WA_MacAlwaysShowToolWindow );
    //Enable aero blurry window effect:
    QtWin::extendFrameIntoClientArea( this );
    
    setCentralWidget(new QWidget);



    QStackedLayout* stackLayout = new QStackedLayout( centralWidget());

    stackLayout->addWidget( stack.rest = new RestWidget());


    stack.nowScrobbling = new QWidget( centralWidget() );
    stack.nowScrobbling->setObjectName( "NowScrobbling" );
    QVBoxLayout* v = new QVBoxLayout( stack.nowScrobbling );
    stackLayout->addWidget( stack.nowScrobbling );
    setMinimumWidth( 410 );

    v->addWidget(ui.now_playing_source = new ScrobbleStatus());
    ui.now_playing_source->setObjectName("now_playing");
    ui.now_playing_source->setFixedHeight( 22 );
    QVBoxLayout* vs;
    {
        QWidget* scrollWidget;
        QScrollArea* sa = new QScrollArea();
        sa->setWidgetResizable( true );
        sa->setWidget( scrollWidget = new StylableWidget(sa));
        sa->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        vs = new QVBoxLayout( scrollWidget );
        v->addWidget( sa );
    }



    // listeners, scrobbles, tags:
    {
       
        QLabel* label;
        QGridLayout* grid = new QGridLayout();
        grid->setSpacing( 0 );

        {
            QVBoxLayout* v2 = new QVBoxLayout();
            grid->addWidget(ui.artist_image = new QLabel, 0, 0, Qt::AlignTop | Qt::AlignLeft );
            ui.artist_image->setObjectName("artist_image");
            v2->addWidget(ui.title = new QLabel);
            v2->addWidget(ui.album = new QLabel);
            v2->addStretch();
            ui.title->setObjectName("title1");
            ui.title->setTextInteractionFlags( Qt::TextSelectableByMouse );
            ui.title->setWordWrap(true);
            ui.album->setObjectName("title2");
            grid->addLayout(v2, 0, 1, Qt::AlignTop );
        }

        // On tour
        ui.onTour = new QLabel(tr("On tour"));
        ui.onTour->setObjectName("name");
        ui.onTour->setProperty("alternate", QVariant(true));
        ui.onTour->setWordWrap(true);
        ui.onTour->hide();
        grid->addWidget(ui.onTour, 1, 0, 1, 2);

        // Similar artists
        label = new QLabel(tr("Similar artists"));
        label->setObjectName("name");
        label->setProperty("alternate", QVariant(true));
        label->setAlignment( Qt::AlignTop );
        grid->addWidget( label, 2, 0 );
        ui.similarArtists = new DataListWidget(this);
        ui.similarArtists->setObjectName("value");
        ui.similarArtists->setProperty("alternate", QVariant(true));
        //ui.similarArtists->setWordWrap(true);
        grid->addWidget(ui.similarArtists, 2, 1);

        // Top fans
        label = new QLabel(tr("Top fans"));
        label->setObjectName("name");
        label->setProperty("alternate", QVariant(true));
        label->setAlignment( Qt::AlignTop );
        grid->addWidget( label, 3, 0 );
        ui.topFans = new DataListWidget(this);
        ui.topFans->setObjectName("value");
        ui.topFans->setProperty("alternate", QVariant(true));
       // ui.topFans->setWordWrap(true);
        grid->addWidget(ui.topFans, 3, 1);

        {
            // Scrobbles (artist, album, track)
            label = new QLabel(tr("Scrobbles"));
            label->setObjectName("name");
            label->setAlignment( Qt::AlignTop );
            grid->addWidget( label, 4, 0 );

            QVBoxLayout* vp = new QVBoxLayout( this );

            ui.artistScrobbles = new QLabel;
            ui.artistScrobbles->setObjectName("value");
            ui.artistScrobbles->hide();
            vp->addWidget(ui.artistScrobbles);
            ui.albumScrobbles = new QLabel;
            ui.albumScrobbles->setObjectName("value");
            ui.albumScrobbles->hide();
            vp->addWidget(ui.albumScrobbles);
            ui.trackScrobbles = new QLabel;
            ui.trackScrobbles->setObjectName("value");
            vp->addWidget(ui.trackScrobbles);

            grid->addLayout(vp, 4, 1);
        }

        // Top tags
        label = new QLabel(tr("Tagged as"));
        label->setObjectName("name");
        label->setProperty("alternate", QVariant(true));
        label->setAlignment( Qt::AlignTop );
        grid->addWidget( label, 5, 0 );
        ui.tags = new DataListWidget(this);
        ui.tags->setObjectName("value");
        ui.tags->setProperty("alternate", QVariant(true));
        //ui.tags->setWordWrap(true);
        grid->addWidget(ui.tags, 5, 1);

        // bio:
        label = new QLabel(tr("Biography"));
        label->setObjectName("name");
        label->setAlignment( Qt::AlignTop );
        grid->addWidget( label, 6, 0 );
        grid->addWidget(ui.bio = new QTextBrowser, 6, 1);
        ui.bio->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        ui.bio->setOpenLinks( false );

        //grid->setRowStretch( 1, 1 );
        //grid->setRowStretch( 2, 1 );
        //grid->setRowStretch( 3, 1 );
        //grid->setRowStretch( 4, 1 );
        //grid->setRowStretch( 5, 1 );
        grid->setRowStretch( 6, 1 );

        vs->addLayout(grid, 1);
        vs->addStretch();
    }
    connect(ui.bio->document()->documentLayout(), SIGNAL( documentSizeChanged(QSizeF)), SLOT( onBioChanged(QSizeF)));
    connect(ui.bio, SIGNAL(anchorClicked(QUrl)), SLOT(onAnchorClicked(QUrl)));
    vs->setStretchFactor(ui.bio, 1);

    // status bar and scrobble controls
    {
        QStatusBar* status = new QStatusBar( this );
        status->setContentsMargins( 0, 0, 0, 0 );
        addDragHandleWidget( status );

#ifdef Q_WS_WIN
        status->setSizeGripEnabled( false );
#else
        //FIXME: this code is duplicated in the radio too
        //In order to compensate for the sizer grip on the bottom right
        //of the window, an empty QWidget is added as a spacer.
        QSizeGrip* sg = status->findChild<QSizeGrip *>();
        if( sg ) {
            int gripWidth = sg->sizeHint().width();
            QWidget* w = new QWidget( status );
            w->setFixedWidth( gripWidth );
            status->addWidget( w );
        }
#endif
        //Seemingly the only way to get a central widget in a QStatusBar
        //is to add an empty widget either side with a stretch value.
        status->addWidget( new QWidget( status), 1 );
        status->addWidget( ui.sc = new ScrobbleControls());
        status->addWidget( new QWidget( status), 1 );
        setStatusBar( status );
    }

    v->setSpacing(0);
    v->setMargin(0);
    vs->setSpacing(0);
    vs->setMargin(0);


    setWindowTitle(tr("Last.fm Audioscrobbler"));
    setUnifiedTitleAndToolBarOnMac( true );
    setMinimumHeight( 80 );
    resize(20, 500);

    ui.message_bar = new MessageBar( centralWidget());
    finishUi();
}

//ScrobbleControls* 
//MetadataWindow::scrobbleControls() const
//{
//    return ui.sc;
//}

void
MetadataWindow::onAnchorClicked( const QUrl& link )
{
    QDesktopServices::openUrl( link );
}

void
MetadataWindow::onBioChanged( const QSizeF& size )
{
    ui.bio->setMinimumHeight( size.toSize().height() );
}

void
MetadataWindow::onTrackStarted(const Track& t, const Track& previous)
{
    setCurrentWidget( stack.nowScrobbling );
    const unsigned short em_dash = 0x2014;
    QString title = QString("%1 ") + QChar(em_dash) + " %2";
    ui.title->setText(title.arg(t.artist()).arg(t.title()));
    ui.album->setText("from " + t.album().title());

    ui.onTour->hide();

    m_currentTrack = t;
    ui.now_playing_source->onTrackStarted(t, previous);
    
    if (t.artist() != previous.artist())
    {
        ui.artist_image->clear();
        ui.bio->clear();
        ui.onTour->setEnabled( false );
        ui.similarArtists->clear();
        ui.tags->clear();

        connect(t.artist().getInfo( lastfm::ws::Username , lastfm::ws::SessionKey ), SIGNAL(finished()), SLOT(onArtistGotInfo()));
        connect(t.artist().getEvents(), SIGNAL(finished()), SLOT(onArtistGotEvents()));
    }

    connect(t.album().getInfo( lastfm::ws::Username , lastfm::ws::SessionKey ), SIGNAL(finished()), SLOT(onAlbumGotInfo()));
    connect(t.getInfo( lastfm::ws::Username , lastfm::ws::SessionKey ), SIGNAL(finished()), SLOT(onTrackGotInfo()));

    ui.topFans->clear();
    connect(t.getTopFans(), SIGNAL(finished()), SLOT(onTrackGotTopFans()));
}

void
MetadataWindow::onArtistGotInfo()
{
    XmlQuery lfm = static_cast<QNetworkReply*>(sender())->readAll();

    int scrobbles = lfm["artist"]["stats"]["playcount"].text().toInt();
    int listeners = lfm["artist"]["stats"]["listeners"].text().toInt();
    int userListens = lfm["artist"]["stats"]["userplaycount"].text().toInt();

    //QString simArt;
    foreach(const XmlQuery& e, lfm["artist"]["similar"].children("artist"))
    {
//        if (simArt.length())
//        {
//            simArt += ", ";
//        }
//        simArt += e["name"].text();
        ui.similarArtists->addItem(e["name"].text());
        ui.similarArtists->updateGeometry();
    }

    //ui.similarArtists->setText(simArt);

    //QString tags;
    foreach(const XmlQuery& e, lfm["artist"]["tags"].children("tag"))
    {
//        if (tags.length())
//        {
//            tags += ", ";
//        }
//        tags += e["name"].text();

        ui.tags->addItem(e["name"].text());
    }

    //ui.tags->setText(tags);

    ui.artistScrobbles->setText(QString("%L1").arg(scrobbles) + " plays (" + QString("%L1").arg(listeners) + " listeners)" + "\n" + QString("%L1").arg(userListens) + " plays in your library");

    QString stylesheet = ((audioscrobbler::Application*)qApp)->loadedStyleSheet() + styleSheet();
    QString style = "<style>" + stylesheet + "</style>";
    
    //TODO if empty suggest they edit it
    QString bio;
    {
        QStringList bioList = lfm["artist"]["bio"]["summary"].text().trimmed().split( "\r" );
        foreach( const QString& p, bioList )
            bio += "<p>" + p + "</p>";
    }

    ui.bio->setHtml( style + bio );

    QTextFrame* root = ui.bio->document()->rootFrame();
    QTextFrameFormat f = root->frameFormat();
    f.setMargin(12);
    root->setFrameFormat(f);

    QUrl url = lfm["artist"]["image size=large"].text();
    QNetworkReply* reply = lastfm::nam()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), SLOT(onArtistImageDownloaded()));
}


void
MetadataWindow::onArtistGotEvents()
{
    XmlQuery lfm = lastfm::ws::parse( static_cast<QNetworkReply*>(sender()) );

    if (lfm["events"].children("event").count() > 0)
    {
        // Display an on tour notification
        ui.onTour->show();
    }
}

void
MetadataWindow::onAlbumGotInfo()
{
    XmlQuery lfm = static_cast<QNetworkReply*>(sender())->readAll();

    int scrobbles = lfm["album"]["playcount"].text().toInt();
    int listeners = lfm["album"]["listeners"].text().toInt();
    int userListens = lfm["album"]["userplaycount"].text().toInt();

//    QString tags;
//    foreach(const XmlQuery& e, lfm["artist"]["tags"].children("tag")) {
//        if (tags.length()) {
//            tags += ", ";
//        }
//        tags += e["name"].text();
//    }

    ui.albumScrobbles->setText(QString("%L1").arg(scrobbles) + " plays (" + QString("%L1").arg(listeners) + " listeners)" + "\n" + QString("%L1").arg(userListens) + " plays in your library");;
//    ui.tags->setText(tags);
}

void
MetadataWindow::onTrackGotInfo()
{
    XmlQuery lfm = static_cast<QNetworkReply*>(sender())->readAll();

    int scrobbles = lfm["track"]["playcount"].text().toInt();
    int listeners = lfm["track"]["listeners"].text().toInt();
    int userListens = lfm["track"]["userplaycount"].text().toInt();

//    QString tags;
//    foreach(const XmlQuery& e, lfm["track"]["toptags"].children("tag")) {
//        if (tags.length()) {
//            tags += ", ";
//        }
//        tags += e["name"].text();
//    }
//
//    ui.tags->setText(tags);

    ui.trackScrobbles->setText(QString("%L1").arg(scrobbles) + " plays (" + QString("%L1").arg(listeners) + " listeners)" + "\n" + QString("%L1").arg(userListens) + " plays in your library");;

}

void
MetadataWindow::onTrackGotTopFans()
{
    XmlQuery lfm = static_cast<QNetworkReply*>(sender())->readAll();

    //QString topFans;
    foreach(const XmlQuery& e, lfm["topfans"].children("user").mid(0, 5))
    {
//        if (topFans.length())
//        {
//            topFans += ", ";
//        }
//        topFans += e["name"].text();
        ui.topFans->addItem(e["name"].text());
    }

//    ui.topFans->setText(topFans);
}

void
MetadataWindow::onArtistImageDownloaded()
{
    QPixmap px;
    px.loadFromData(static_cast<QNetworkReply*>(sender())->readAll());

    QLinearGradient g(QPoint(), px.rect().bottomLeft());
    g.setColorAt( 0.0, QColor(0, 0, 0, 0.11*255));
    g.setColorAt( 1.0, QColor(0, 0, 0, 0.88*255));

    QPainter p(&px);
    p.setCompositionMode(QPainter::CompositionMode_Multiply);
    p.fillRect(px.rect(), g);
    p.end();

    ui.artist_image->setFixedSize( px.size());
    ui.artist_image->setPixmap(px);
}

void
MetadataWindow::onStopped()
{
    setCurrentWidget( stack.rest );
    ui.bio->clear();
    ui.artist_image->clear();
    ui.title->clear();
    ui.tags->clear();
    ui.album->clear();
    ui.artistScrobbles->clear();
    ui.albumScrobbles->clear();
    ui.trackScrobbles->clear();
    m_currentTrack = Track();
    ui.now_playing_source->onTrackStopped();
}

void
MetadataWindow::onResumed()
{

}

void
MetadataWindow::onPaused()
{
}

void
MetadataWindow::setCurrentWidget( QWidget* w )
{
    ((QStackedLayout*)centralWidget()->layout())->setCurrentWidget( w );
}

