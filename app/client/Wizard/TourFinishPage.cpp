
#include <QHBoxLayout>
#include <QLabel>

#include "TourFinishPage.h"

TourFinishPage::TourFinishPage( QWidget* w )
               :QWizardPage( w )
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    layout->addWidget( ui.image = new QLabel( this ), 0, Qt::AlignCenter );
    ui.image->setObjectName( "image" );
    layout->addWidget( ui.description = new QLabel( tr( "<p>Now you're ready to get started! Just click <strong>Finish</strong> and start exploring.</p>"
                                                        "<p>Not only that, but we've also finished importing your listening history and we've added it to your Last.fm profile.</p>"
                                                        "<p>Thanks for installing the Last.fm Desktop App, we hope you enjoy using it!</p>"), this ),
                         0,
                         Qt::AlignTop);
    ui.description->setObjectName( "description" );
    ui.description->setWordWrap( true );
}


void
TourFinishPage::initializePage()
{
    setTitle( tr( "That's it, you're good to go!" ) );

    setFinalPage( true );

    setButtonText( QWizard::FinishButton, tr( "Finish" ) );
    setButtonText( QWizard::BackButton, tr( "<< Back" ) );
}
