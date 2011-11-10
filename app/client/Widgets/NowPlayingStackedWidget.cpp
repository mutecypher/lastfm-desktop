#include <QPushButton>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QStackedLayout>

#include <lastfm/Track.h>
#include <lastfm/RadioStation.h>

#include "../Services/RadioService/RadioService.h"
#include "../Services/ScrobbleService/ScrobbleService.h"

#include "NowPlayingStackedWidget.h"
#include "NowPlayingWidget.h"
#include "NothingPlayingWidget.h"

NowPlayingStackedWidget::NowPlayingStackedWidget( QWidget* parent )
    :unicorn::SlidingStackedWidget( parent )
{
    addWidget( ui.nothingPlaying = new NothingPlayingWidget( this ) );
    addWidget( ui.nowPlaying = new NowPlayingWidget( this ) );

    connect( &RadioService::instance(), SIGNAL(tuningIn(RadioStation)), SLOT(showNowPlaying()) );
    connect( &ScrobbleService::instance(), SIGNAL(trackStarted(Track,Track)), SLOT(showNowPlaying()));
    connect( &ScrobbleService::instance(), SIGNAL(stopped()), SLOT(showNothingPlaying()));
}

void
NowPlayingStackedWidget::showNowPlaying()
{
    slide( 1 );
}

void
NowPlayingStackedWidget::showNothingPlaying()
{
    slide( 0 );
}


