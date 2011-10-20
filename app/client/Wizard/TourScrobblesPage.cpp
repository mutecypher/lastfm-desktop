
#include <QHBoxLayout>
#include <QLabel>

#include "FirstRunWizard.h"
#include "TourScrobblesPage.h"

TourScrobblesPage::TourScrobblesPage()
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 20 );

    layout->addWidget( ui.image = new QLabel( this ), 0, Qt::AlignTop );
    ui.image->setObjectName( "image" );
    layout->addWidget( ui.description = new QLabel( tr( "<p>Use the Last.fm Desktop App to listen to non-stop personailsed radio based on the music you want to hear.</p>"
                                                          "<p>Every play of every Last.fm station is totally different, from stations based on aritsts and tags to brand new recommendations tailored to your music taste.</p>"), this ),
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
