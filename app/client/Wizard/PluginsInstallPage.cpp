
#include <QHBoxLayout>
#include <QLabel>

#include "FirstRunWizard.h"
#include "PluginsInstallPage.h"

PluginsInstallPage::PluginsInstallPage()
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 20 );

    layout->addWidget( ui.image = new QLabel( this ), 0, Qt::AlignCenter );
    ui.image->setObjectName( "image" );
    layout->addWidget( ui.description = new QLabel( tr( "<p>Please follow the instructions that appear from your operating system to install the plugins.</p>"
                                                          "<p>Once the plugins have been installed on you computer, click <strong>Continue</strong>.</p>"), this ),
                         0,
                         Qt::AlignTop);
    ui.description->setObjectName( "description" );
    ui.description->setWordWrap( true );
}


void
PluginsInstallPage::initializePage()
{
    setTitle( tr( "Your plugins are now being installed" ) );

    wizard()->setButton( FirstRunWizard::NextButton, tr( "Continue" ) );
    wizard()->setButton( FirstRunWizard::BackButton, tr( "<< Back" ) );
}


void
PluginsInstallPage::cleanupPage()
{
}
