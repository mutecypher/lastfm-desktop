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

ScrobbleInfoWidget::ScrobbleInfoWidget( QWidget* p )
                   :StylableWidget( p )
{

    connect( qApp, SIGNAL( trackStarted( Track, Track)), SLOT( onTrackStarted( Track, Track )));
    connect( qApp, SIGNAL( stopped()), SLOT( onStopped()));
    
    QVBoxLayout* v = new QVBoxLayout( this );

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
        QWidget* gridWidget = new QWidget();
        QGridLayout* grid = new QGridLayout(gridWidget);
        grid->setSpacing( 0 );

        {
            QVBoxLayout* v2 = new QVBoxLayout();
            grid->addWidget(ui.artist_image = new HttpImageWidget, 0, 0, Qt::AlignTop | Qt::AlignLeft );
            ui.artist_image->setObjectName("artist_image");
            v2->addWidget(ui.title = new QLabel);
            v2->addWidget(ui.album = new QLabel);
            v2->addStretch();
            ui.title->setOpenExternalLinks( true );
            ui.title->setObjectName("title1");
            ui.title->setTextInteractionFlags( Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse );
            ui.title->setWordWrap(true);
            ui.album->setObjectName("title2");
            ui.album->setTextInteractionFlags( Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse );
            ui.album->setOpenExternalLinks( true );
            grid->addLayout(v2, 0, 1, Qt::AlignTop );
        }

        // On tour
        ui.onTour = new QLabel(tr("On tour"));
        ui.onTour->setObjectName("name");
        ui.onTour->setProperty("alternate", QVariant(true));
        ui.onTour->setWordWrap(true);
        ui.onTour->hide();
        
        ui.onTourBlank = new QLabel();
        ui.onTourBlank->setObjectName("value");
        ui.onTourBlank->setProperty("alternate", QVariant(true));
        grid->addWidget(ui.onTour, 1, 0 );
        grid->addWidget(ui.onTourBlank, 1, 1 );

        // Similar artists
        label = new QLabel(tr("Similar artists"));
        label->setObjectName("name");
        label->setAlignment( Qt::AlignTop );
        grid->addWidget( label, 2, 0 );
        ui.similarArtists = new DataListWidget(this);
        ui.similarArtists->setObjectName("value");
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
        grid->addWidget(ui.topFans, 3, 1);

        {
            // Scrobbles (artist, album, track)
            label = new QLabel(tr("Scrobbles"));
            label->setObjectName("name");
            label->setAlignment( Qt::AlignTop );
            grid->addWidget( label, 4, 0 );

            QVBoxLayout* vp = new QVBoxLayout();

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
        grid->addWidget(ui.tags, 5, 1);

        // bio:
        label = new QLabel(tr("Biography"));
        label->setObjectName("name");
        label->setAlignment( Qt::AlignTop );
        grid->addWidget( label, 6, 0 );
        grid->addWidget(ui.bio = new QTextBrowser, 6, 1);
        ui.bio->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        ui.bio->setOpenLinks( false );

        grid->setRowStretch( 6, 1 );
        gridWidget->setObjectName( "data_grid" );

        vs->addWidget(gridWidget, 1);
        vs->addStretch();
    }
    connect(ui.bio->document()->documentLayout(), SIGNAL( documentSizeChanged(QSizeF)), SLOT( onBioChanged(QSizeF)));
    connect(ui.bio, SIGNAL(anchorClicked(QUrl)), SLOT(onAnchorClicked(QUrl)));
    vs->setStretchFactor(ui.bio, 1);

    v->setSpacing(0);
    v->setMargin(0);
    vs->setSpacing(0);
    vs->setMargin(0);
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
                                                                                .arg(t.title()));    if( !t.album().isNull() ) {
        QString album("from <a class='title' href=\"%1\">%2</a>");
        ui.album->setText("<style>" + uApp->loadedStyleSheet() + "</style>" + album.arg( t.album().www().toString())
                                                                               .arg( t.album().title()));
    } else {
        ui.album->clear();
    }

    ui.onTour->hide();
    ui.onTourBlank->hide();

    if (t.artist() != previous.artist())
    {
        ui.artist_image->clear();
        ui.artist_image->setHref( QUrl());
        ui.bio->clear();
        ui.onTour->setEnabled( false );
        ui.similarArtists->clear();
        ui.tags->clear();

        connect(t.artist().getInfo( lastfm::ws::Username , lastfm::ws::SessionKey ), SIGNAL(finished()), SLOT(onArtistGotInfo()));
        connect(t.artist().getEvents( 1 ), SIGNAL(finished()), SLOT(onArtistGotEvents()));
    }

    connect(t.album().getInfo( lastfm::ws::Username , lastfm::ws::SessionKey ), SIGNAL(finished()), SLOT(onAlbumGotInfo()));
    connect(t.getInfo( lastfm::ws::Username , lastfm::ws::SessionKey ), SIGNAL(finished()), SLOT(onTrackGotInfo()));

    ui.topFans->clear();
    connect(t.getTopFans(), SIGNAL(finished()), SLOT(onTrackGotTopFans()));

}


void
ScrobbleInfoWidget::onArtistGotInfo()
{
    XmlQuery lfm = static_cast<QNetworkReply*>(sender())->readAll();

    int scrobbles = lfm["artist"]["stats"]["playcount"].text().toInt();
    int listeners = lfm["artist"]["stats"]["listeners"].text().toInt();
    int userListens = lfm["artist"]["stats"]["userplaycount"].text().toInt();

    foreach(const XmlQuery& e, lfm["artist"]["similar"].children("artist"))
    {
        ui.similarArtists->addItem( e["name"].text(), QUrl(e["url"].text()));
    }

    foreach(const XmlQuery& e, lfm["artist"]["tags"].children("tag"))
    {
        ui.tags->addItem( e["name"].text(), QUrl(e["url"].text()));
    }

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
    ui.artist_image->loadUrl( url, true );
    ui.artist_image->setHref( lfm["artist"][ "url" ].text());
}


void
ScrobbleInfoWidget::onStopped()
{
    ui.bio->clear();
    ui.artist_image->clear();
    ui.artist_image->setHref(QUrl());
    ui.title->clear();
    ui.tags->clear();
    ui.album->clear();
    ui.artistScrobbles->clear();
    ui.albumScrobbles->clear();
    ui.trackScrobbles->clear();
}


void
ScrobbleInfoWidget::onArtistGotEvents()
{
    XmlQuery lfm = lastfm::ws::parse( static_cast<QNetworkReply*>(sender()) );

    if (lfm["events"].children("event").count() > 0)
    {
        // Display an on tour notification
        ui.onTour->show();
        ui.onTourBlank->show();
    }
}

void
ScrobbleInfoWidget::onAlbumGotInfo()
{
    XmlQuery lfm = static_cast<QNetworkReply*>(sender())->readAll();

    int scrobbles = lfm["album"]["playcount"].text().toInt();
    int listeners = lfm["album"]["listeners"].text().toInt();
    int userListens = lfm["album"]["userplaycount"].text().toInt();

    ui.albumScrobbles->setText(QString("%L1").arg(scrobbles) + " plays (" + QString("%L1").arg(listeners) + " listeners)" + "\n" + QString("%L1").arg(userListens) + " plays in your library");;
}

void
ScrobbleInfoWidget::onTrackGotInfo()
{
    XmlQuery lfm = static_cast<QNetworkReply*>(sender())->readAll();

    int scrobbles = lfm["track"]["playcount"].text().toInt();
    int listeners = lfm["track"]["listeners"].text().toInt();
    int userListens = lfm["track"]["userplaycount"].text().toInt();

    ui.trackScrobbles->setText(QString("%L1").arg(scrobbles) + " plays (" + QString("%L1").arg(listeners) + " listeners)" + "\n" + QString("%L1").arg(userListens) + " plays in your library");;
}

void
ScrobbleInfoWidget::onTrackGotTopFans()
{
    XmlQuery lfm = static_cast<QNetworkReply*>(sender())->readAll();

    foreach(const XmlQuery& e, lfm["topfans"].children("user").mid(0, 5))
    {
        ui.topFans->addItem(e["name"].text(), QUrl(e["url"].text()));
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
