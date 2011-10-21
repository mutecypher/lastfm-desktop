
#include <QHBoxLayout>
#include <QLabel>

#include "FirstRunWizard.h"
#include "TourFinishPage.h"

TourFinishPage::TourFinishPage()
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 20 );

    layout->addWidget( ui.image = new QLabel( this ), 0, Qt::AlignTop | Qt::AlignHCenter );
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

    wizard()->setButton( FirstRunWizard::FinishButton, tr( "Finish" ) );

    if ( wizard()->canGoBack() )
        wizard()->setButton( FirstRunWizard::BackButton, tr( "<< Back" ) );
}

void
TourFinishPage::cleanupPage()
{
}
