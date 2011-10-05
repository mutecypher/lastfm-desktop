
#include <QHBoxLayout>
#include <QLabel>

#include "BootstrapProgressPage.h"

BootstrapProgressPage::BootstrapProgressPage( QWidget* parent )
    :QWizardPage( parent )
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    layout->addWidget( ui.image = new QLabel( this ), 0, Qt::AlignCenter );
    ui.image->setObjectName( "image" );
    layout->addWidget( ui.description = new QLabel( tr( "<p>Don't worry, the upload process shouldn't take more than a couple of minutes, depending on the size of your music library.</p>"
                                                          "<p>While we're hard at work adding your listening history to your Last.fm profile, why don't you check out the main features of the Last.fm Desktop App. Click <strong>Continue</strong> to take the tour.</p>"), this ),
                         0,
                         Qt::AlignTop);
    ui.description->setObjectName( "description" );
    ui.description->setWordWrap( true );
}


void
BootstrapProgressPage::initializePage()
{
    setButtonText( QWizard::NextButton, tr( "Continue" ) );
    setButtonText( QWizard::BackButton, tr( "<< Back" ) );
}

