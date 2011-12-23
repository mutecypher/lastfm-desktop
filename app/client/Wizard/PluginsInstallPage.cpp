
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>

#include "FirstRunWizard.h"
#include "PluginsInstallPage.h"

PluginsInstallPage::PluginsInstallPage()
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 20 );

    layout->addWidget( ui.image = new QLabel( this ), 0, Qt::AlignTop );
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

    wizard()->setCommitPage( true );

    QAbstractButton* continueButton = wizard()->setButton( FirstRunWizard::NextButton, tr( "Continue" ) );
    if ( wizard()->canGoBack() )
        wizard()->setButton( FirstRunWizard::BackButton, tr( "<< Back" ) );

#ifdef Q_OS_WIN32
    if ( wizard()->pluginList()->installList().count() > 0 )
    {
        // get the install to happen a bit later so that
        // we actually show this page of the wizard
        // and the user has time to read what's happening
        QTimer::singleShot( 1000, this, SLOT(install()) );
        continueButton->setEnabled( false );
    }
    else
    {
        continueButton->click();
    }
#endif
}

void
PluginsInstallPage::install()
{
#ifdef Q_OS_WIN32
    foreach( IPluginInfo* plugin, wizard()->pluginList()->installList() )
        plugin->doInstall();
#endif

    QAbstractButton* continueButton = wizard()->setButton( FirstRunWizard::NextButton, tr( "Continue" ) );
    continueButton->setEnabled( true );
    QTimer::singleShot( 1000, continueButton, SLOT(click()) );
}


void
PluginsInstallPage::cleanupPage()
{
}
