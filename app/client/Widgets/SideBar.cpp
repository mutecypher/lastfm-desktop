
#include <QRadioButton>
#include <QVBoxLayout>

#include "../Application.h"
#include "SideBar.h"

SideBar::SideBar(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    layout->addWidget( ui.np = new QRadioButton( tr( "Now Playing" ), this ));
    layout->addWidget( ui.s = new QRadioButton( tr( "Scrobbles" ), this ));
    layout->addWidget( ui.r = new QRadioButton( tr( "Radio" ), this ));
    layout->addStretch( 1 );

    connect( ui.np, SIGNAL(clicked()), SLOT(onButtonClicked()));
    connect( ui.s, SIGNAL(clicked()), SLOT(onButtonClicked()));
    connect( ui.r, SIGNAL(clicked()), SLOT(onButtonClicked()));

    connect( aApp, SIGNAL(sessionChanged(unicorn::Session*)), SLOT(onSessionChanged(unicorn::Session*)));
}

void
SideBar::onButtonClicked()
{
    emit currentChanged( layout()->indexOf( qobject_cast<QRadioButton*>( sender() ) ) );
}


void
SideBar::onSessionChanged( unicorn::Session* newSession )
{
    QUrl imageUrl = newSession->userInfo().imageUrl( lastfm::ImageSize::Medium, true );
    connect( lastfm::nam()->get( QNetworkRequest( imageUrl ) ), SIGNAL(finished()), SLOT(onGotAvatar()));
}

void
SideBar::onGotAvatar()
{
    QPixmap avatar;
    avatar.loadFromData( qobject_cast<QNetworkReply*>(sender())->readAll() );
    ui.np->setIcon( QIcon( avatar ) );
}
