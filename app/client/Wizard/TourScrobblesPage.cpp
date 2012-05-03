
#include <QHBoxLayout>
#include <QLabel>

#include "FirstRunWizard.h"
#include "TourScrobblesPage.h"

TourScrobblesPage::TourScrobblesPage()
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 20 );

    layout->addWidget( ui.image = new QLabel( this ), 0, Qt::AlignTop | Qt::AlignHCenter );
    ui.image->setObjectName( "image" );
    layout->addWidget( ui.description = new QLabel( tr( "<p>The desktop client runs in the background, quietly updating your Last.fm profile with the music you're playing, which you can use to get music recommendations, gig tips and more. </p>"
                                                        "<p>You can also use the Last.fm Desktop App to find out more about the artist you're listening to, and to play personalised radio.</p>"), this ),
                         0,
                         Qt::AlignTop);
    ui.description->setObjectName( "description" );
    ui.description->setWordWrap( true );
}


void
TourScrobblesPage::initializePage()
{
    setTitle( tr( "Welcome to the Last.fm Desktop App!" ) );

    wizard()->setButton( FirstRunWizard::NextButton, tr( "Continue" ) );
    if ( wizard()->canGoBack() )
        wizard()->setButton( FirstRunWizard::BackButton, tr( "<< Back" ) );
    wizard()->setButton( FirstRunWizard::SkipButton, tr( "Skip Tour >>" ) );
}

void
TourScrobblesPage::cleanupPage()
{
}
