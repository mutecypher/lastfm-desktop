

#include <QVBoxLayout>

#include <lastfm/RadioStation>

#include "../Services/RadioService/RadioService.h"

#include "../Application.h"

#include "PlayableItemWidget.h"
#include "QuickStartWidget.h"

#include "RadioWidget.h"

RadioWidget::RadioWidget(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout( this );

    QuickStartWidget* quickStartWidget = new QuickStartWidget();
    layout->addWidget( quickStartWidget );
    connect( quickStartWidget, SIGNAL(startRadio(RadioStation)), &RadioService::instance(), SLOT(play(RadioStation)));

    connect( aApp, SIGNAL(sessionChanged(unicorn::Session*)), SLOT(onSessionChanged(unicorn::Session*) ) );


}

void
RadioWidget::onSessionChanged( unicorn::Session* session )
{
    if ( !session->userInfo().name().isEmpty() )
    {
        layout()->addWidget( new PlayableItemWidget( tr( "My library radio" ), RadioStation::library( session->userInfo() ) ) );
        layout()->addWidget( new PlayableItemWidget( tr( "My mix radio" ), RadioStation::mix( session->userInfo() ) ) );
        layout()->addWidget( new PlayableItemWidget( tr( "My recommended radio" ), RadioStation::recommendations( session->userInfo() ) ) );

        //layout()->addStretch( 1 );
    }
}
