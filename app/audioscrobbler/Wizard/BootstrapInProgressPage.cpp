#include "BootstrapInProgressPage.h"
#include "../Bootstrapper/iTunesBootstrapper.h"
#include "../Bootstrapper/PluginBootstrapper.h"
#include <QVBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include <QVariant>
#include <QTimer>
#include <iostream>

BootstrapInProgressPage::BootstrapInProgressPage( QWizard* parent )
                        :QWizardPage( parent ),
                         m_isComplete( false )
{
    setTitle( "Importing your listening history!" );
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->addStretch();
    layout->addWidget( m_label = new QLabel( tr( "Processing Library" )));
    layout->addWidget( m_progressBar = new QProgressBar );
    m_progressBar->setValue( 0 );
    m_progressBar->setMaximum( 100 );
    layout->addStretch();
}

void 
BootstrapInProgressPage::initializePage()
{
    QString bootstrapId = field( "bootstrap_player" ).toString();
    IPluginInfo* bootstrapPlugin = m_pluginList.pluginById( bootstrapId );

    switch( bootstrapPlugin->bootstrapType() ) {
        case IPluginInfo::ClientBootstrap:
            Q_ASSERT( bootstrapId == "osx" || bootstrapId == "itw" );
            m_bootstrapper = new iTunesBootstrapper( this );
            connect( m_bootstrapper, SIGNAL( trackProcessed( int, Track )), SLOT( onTrackProcessed( int, Track )), Qt::DirectConnection);
        break;

        case IPluginInfo::PluginBootstrap:
            m_bootstrapper = new PluginBootstrapper( bootstrapId, this );
        break;
    }
    
    connect( m_bootstrapper, SIGNAL( percentageUploaded( int )), SLOT( onPercentageUpload( int )));
    connect( m_bootstrapper, SIGNAL( done( int )), SLOT( onBootstrapDone( int )));
    //Start the bootstrapping process after the Page is shown and the event loop
    //has has a chance to catch up with itself.
    QTimer::singleShot( 0, this, SLOT( startBootstrap()));
}

bool
BootstrapInProgressPage::isComplete() const
{
    return m_isComplete;
}

void
BootstrapInProgressPage::onTrackProcessed( int percentage, const Track& )
{
    m_progressBar->setValue( percentage );
}

void 
BootstrapInProgressPage::onPercentageUpload( int percentage )
{
    m_label->setText( tr( "Uploading Data" ));
    m_progressBar->setValue( percentage );
}

void 
BootstrapInProgressPage::startBootstrap()
{
    m_bootstrapper->bootStrap();
}

void 
BootstrapInProgressPage::onBootstrapDone( int status )
{
    m_isComplete = true;
    emit completeChanged();

    if( status == AbstractBootstrapper::Bootstrap_Ok ) {
        wizard()->next();
    }
}
