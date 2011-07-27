#include <QPushButton>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QStackedLayout>

#include <lastfm/Track>
#include <lastfm/RadioStation>

#include "lib/unicorn/TrackImageFetcher.h"

#include "../Services/RadioService/RadioService.h"
#include "../Services/ScrobbleService/ScrobbleService.h"

#include "NowPlayingStackedWidget.h"
#include "NowPlayingWidget.h"
#include "NothingPlayingWidget.h"

NowPlayingStackedWidget::NowPlayingStackedWidget( QWidget* parent )
    :QWidget( parent )
{
    QStackedLayout* layout = new QStackedLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    layout->addWidget( ui.nothingPlaying = new NothingPlayingWidget( this ) );
    layout->addWidget( ui.nowPlaying = new NowPlayingWidget( this ) );

    connect( &RadioService::instance(), SIGNAL(tuningIn(RadioStation)), SLOT(showNowPlaying()) );
    connect( &ScrobbleService::instance(), SIGNAL(trackStarted(Track,Track)), SLOT(showNowPlaying()));
    connect( &ScrobbleService::instance(), SIGNAL(stopped()), SLOT(showNothingPlaying()));
}

void
NowPlayingStackedWidget::showNowPlaying()
{
    qobject_cast<QStackedLayout*>(layout())->setCurrentWidget( ui.nowPlaying );
}

void
NowPlayingStackedWidget::showNothingPlaying()
{
    qobject_cast<QStackedLayout*>(layout())->setCurrentWidget( ui.nothingPlaying );
}


