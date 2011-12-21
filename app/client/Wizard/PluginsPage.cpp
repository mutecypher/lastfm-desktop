/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Jono Cole

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QProcess>
#include <QDebug>

#include "../Plugins/PluginList.h"

#include "FirstRunWizard.h"
#include "PluginsPage.h"

PluginsPage::PluginsPage()
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 20 );

    // add the radio buttons
    m_pluginsLayout = new QVBoxLayout( this );
    m_pluginsLayout->setContentsMargins( 0, 0, 0, 0 );
    m_pluginsLayout->setSpacing( 0 );

    layout->addLayout( m_pluginsLayout );

    layout->addWidget( ui.description = new QLabel( tr( "<p>Your media players need a special Last.fm plugin to be able to scrobble the music you listen to.</p>"
                                                       "<p>Please select the media players that you would like to scrobble your music from and click <strong>Install Plugins</strong></p>"), this ),
                                                       0,
                                                       Qt::AlignTop );
    ui.description->setObjectName( "description" );
    ui.description->setWordWrap( true );
}


bool
PluginsPage::validatePage()
{
    /// Check if media players are running and wait for them to be exited

    // we're not going to use this anymore we should bundle the installers
    // and just run them ourselves

    //wizard()->setCommitPage( true );


    // For all the plugins that have been selected install them to the correct loaction
    // and write to the reistry which version we have installed

    return true;
}

void
PluginsPage::cleanupPage()
{
}

void
PluginsPage::initializePage()
{
    QList<IPluginInfo*> supportedPlugins = wizard()->pluginList()->supportedList();
    foreach( IPluginInfo* plugin, supportedPlugins )
    {
        if( !plugin->isAppInstalled() )
            continue;

        QCheckBox* cb;
        m_pluginsLayout->addWidget( cb = new QCheckBox( plugin->name(), this ));
        connect( cb, SIGNAL(toggled(bool)), plugin, SLOT(install(bool)));

        cb->setObjectName( plugin->id() );
        cb->setChecked( true );

        if ( plugin->isInstalled() )
        {
            if ( plugin->version() > plugin->installedVersion() )
            {
                cb->setChecked( true );
                cb->setText( cb->text() + " " + tr( "(newer version)" ));
            }
            else
            {
                cb->setChecked( false );
                cb->setText( cb->text() + " " + tr( "(Plugin installed tick to reinstall)" ));
            }
        }
    }

    setTitle( tr( "Next step, install the Last.fm plugins to be able to scrobble the music you listen to." ));

    wizard()->setButton( FirstRunWizard::NextButton, tr( "Install Plugins" ) );
    if ( wizard()->canGoBack() )
        wizard()->setButton( FirstRunWizard::BackButton, tr( "<< Back" ) );
    wizard()->setButton( FirstRunWizard::SkipButton, tr( "Skip >>" ) );
}
