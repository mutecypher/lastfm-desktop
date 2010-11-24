#include "BootstrapInProgressPage.h"
#include "../Bootstrapper/iTunesBootstrapper.h"
#include "../Bootstrapper/PluginBootstrapper.h"
#include <QVariant>
#include <iostream>

BootstrapInProgressPage::BootstrapInProgressPage( QWizard* parent )
                        :QWizardPage( parent ),
                         m_isComplete( false )
{
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
            
        break;

        case IPluginInfo::PluginBootstrap:
            m_bootstrapper = new PluginBootstrapper( bootstrapId, this );
        break;
    }

    m_bootstrapper->bootStrap();
}

bool
BootstrapInProgressPage::isComplete() const
{
    return m_isComplete;
}
