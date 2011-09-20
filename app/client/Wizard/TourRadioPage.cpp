
#include <QHBoxLayout>
#include <QLabel>

#include "../Application.h"

#include "TourRadioPage.h"

TourRadioPage::TourRadioPage( QWidget* w )
               :QWizardPage( w )
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    layout->addWidget( ui.image = new QLabel( this ), 0, Qt::AlignCenter );
    ui.image->setObjectName( "image" );
    layout->addWidget( ui.description = new QLabel( "", this ), 0, Qt::AlignTop);
    ui.description->setObjectName( "description" );
    ui.description->setWordWrap( true );
}


void
TourRadioPage::initializePage()
{
    if ( aApp->currentSession() && aApp->currentSession()->userInfo().isSubscriber() )
    {
        setTitle( tr( "Listen to non-stop, personalised radio." ) );
        ui.description->setText( "<p>Use the Last.fm Desktop App to listen to non-stop personailsed radio based on the music you want to hear.</p>"
                                 "<p>Every play of every Last.fm station is totally different, from stations based on aritsts and tags to brand new recommendations tailored to your music taste.</p>" );
    }
    else
    {
        setTitle( tr( "Subscrobe and listen to non-stop, personalised radio." ) );
        ui.description->setText( "<p>Subscribe to Last.fm for just [3/$3/3 localised] a month and listen to radio using the Last.fm Desktop App.</p>"
                                 "<p>Every play of every Last.fm station is totally different, from stations based on aritsts and tags to brand new recommendations tailored to your music taste.</p>" );
        wizard()->setOption( QWizard::HaveCustomButton2, true );
        setButtonText( QWizard::CustomButton2, tr( "Subscribe" ) );
    }

    setButtonText( QWizard::NextButton, tr( "Continue" ) );
    setButtonText( QWizard::BackButton, tr( "<< Back" ) );

    wizard()->setOption( QWizard::HaveCustomButton1, true );
    setButtonText( QWizard::CustomButton1, tr( "Skip Tour >>" ) );
}

void
TourRadioPage::cleanupPage()
{
    wizard()->setOption( QWizard::HaveCustomButton1, false );
    wizard()->setOption( QWizard::HaveCustomButton2, false );
}
