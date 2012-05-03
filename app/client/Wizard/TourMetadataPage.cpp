#include "TourMetadataPage.h"
#include <QTimer>
#include <QLabel>
#include <QLayout>
#include <QPixmap>
#include <QBoxLayout>

#include "FirstRunWizard.h"
#include "../Application.h"
#include "../Widgets/PointyArrow.h"

TourMetadataPage::TourMetadataPage()
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 20 );

    layout->addWidget( ui.image = new QLabel( this ), 0, Qt::AlignTop | Qt::AlignHCenter );
    ui.image->setObjectName( "image" );
    layout->addWidget( ui.description = new QLabel( tr( "<p>Find out more about the music you're listening to, including biographies, listening stats, photos and similar artists, as well as the tags listeners use to describe them.</p>"
                                                          "<p>Check out the <strong>Now Playing</strong> tab, or simply click on any track in your <strong>Scrobbles</strong> tab to learn more.</p>"), this ),
                         0,
                         Qt::AlignTop);
    ui.description->setObjectName( "description" );
    ui.description->setWordWrap( true );
}


void 
TourMetadataPage::initializePage()
{
    setTitle( tr( "Discover more about the artists you love" ) );

    wizard()->setButton( FirstRunWizard::NextButton, tr( "Continue" ) );
    if ( wizard()->canGoBack() )
        wizard()->setButton( FirstRunWizard::BackButton, tr( "<< Back" ) );
    wizard()->setButton( FirstRunWizard::SkipButton, tr( "Skip Tour >>" ) );
}

void
TourMetadataPage::cleanupPage()
{
}
