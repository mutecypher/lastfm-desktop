
#include <QHBoxLayout>
#include <QLabel>

#include "../Application.h"

#include "FirstRunWizard.h"
#include "BootstrapProgressPage.h"

BootstrapProgressPage::BootstrapProgressPage()
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 20 );

    layout->addWidget( ui.image = new QLabel( this ), 0, Qt::AlignTop );
    ui.image->setObjectName( "image" );
    layout->addWidget( ui.description = new QLabel( tr( "<p>Don't worry, the upload process shouldn't take more than a couple of minutes, depending on the size of your music library.</p>"
                                                          "<p>While we're hard at work adding your listening history to your Last.fm profile, why don't you check out the main features of the Last.fm Desktop App. Click <strong>Continue</strong> to take the tour.</p>"), this ),
                         0,
                         Qt::AlignTop);
    ui.description->setObjectName( "description" );
    ui.description->setWordWrap( true );
}

void
BootstrapProgressPage::setPluginId( const QString& pluginId )
{
    if ( pluginId == "itw"
         || pluginId == "osx" )
        ui.image->setPixmap( QPixmap( ":/graphic_import_itunes.png" ) );
    else if ( pluginId == "wmp" )
        ui.image->setPixmap( QPixmap( ":/graphic_import_wmp.png" ) );
    else if ( pluginId == "wa2" )
        ui.image->setPixmap( QPixmap( ":/graphic_import_winamp.png" ) );
    else if ( pluginId == "foo2"
              || pluginId == "foo3" )
        ui.image->setPixmap( QPixmap( ":/graphic_import_foobar.png" ) );
}

void
BootstrapProgressPage::initializePage()
{
    wizard()->setButton( FirstRunWizard::NextButton, tr( "Continue" ) );
    if ( wizard()->canGoBack() )
        wizard()->setButton( FirstRunWizard::BackButton, tr( "<< Back" ) );
}

void
BootstrapProgressPage::cleanupPage()
{
}
