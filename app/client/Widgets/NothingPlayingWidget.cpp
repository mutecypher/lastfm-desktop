
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "../Application.h"

#include "QuickStartWidget.h"

#include "lib/unicorn/widgets/Label.h"

#include "NothingPlayingWidget.h"
#include "ui_NothingPlayingWidget.h"

NothingPlayingWidget::NothingPlayingWidget( QWidget* parent )
    :StylableWidget( parent ),
      ui( new Ui::NothingPlayingWidget )
{   
    ui->setupUi( this );

    setAttribute( Qt::WA_LayoutUsesWidgetRect );
    ui->top->setAttribute( Qt::WA_LayoutUsesWidgetRect );

    ui->contents->setAttribute( Qt::WA_LayoutUsesWidgetRect );

    setUser( User() );

    ui->scrobble->setText( tr( "<h2>Scrobble from your music player</h2>"
                               "<p>Start listening to some music in your media player. You can see more information about the tracks you play on the Now Playing tab.</p>") );

    ui->itunes->hide();
    ui->wmp->hide();
    ui->winamp->hide();
    ui->foobar->hide();

#if  defined( Q_OS_WIN32 ) || defined( Q_OS_MAC )
    ui->itunes->show();
    ui->itunes->setAttribute( Qt::WA_LayoutUsesWidgetRect );

    connect( ui->itunes, SIGNAL(clicked()), SLOT(oniTunesClicked()));

#ifndef Q_OS_MAC
    ui->wmp->show();
    ui->wmp->setAttribute( Qt::WA_LayoutUsesWidgetRect );
    ui->winamp->show();
    ui->winamp->setAttribute( Qt::WA_LayoutUsesWidgetRect );
    ui->foobar->show();
    ui->foobar->setAttribute( Qt::WA_LayoutUsesWidgetRect );
#endif
#endif

    connect( aApp, SIGNAL(sessionChanged(unicorn::Session*)), SLOT(onSessionChanged(unicorn::Session*)) );
}


void
NothingPlayingWidget::onSessionChanged( unicorn::Session* session )
{
    setUser( session->userInfo() );
}

void
NothingPlayingWidget::setUser( const lastfm::User& user )
{
    if ( !user.name().isEmpty() )
        ui->top->setText( tr(  "Hello, %1!" ).arg( user.name() ) );
}

#ifndef Q_OS_MAC
void
NothingPlayingWidget::oniTunesClicked()
{
}
#endif
