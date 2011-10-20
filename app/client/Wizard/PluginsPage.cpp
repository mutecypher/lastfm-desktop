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

#include "lib/unicorn/Updater/PluginList.h"

#include "FirstRunWizard.h"
#include "PluginsPage.h"

PluginsPage::PluginsPage()
{
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 20 );

    // add the radio buttons
    QVBoxLayout* pluginsLayout = new QVBoxLayout( this );
    pluginsLayout->setContentsMargins( 0, 0, 0, 0 );
    pluginsLayout->setSpacing( 0 );

    layout->addLayout( pluginsLayout );

    PluginList list;
    QList<IPluginInfo*> supportedPlugins = list.supportedList();
    foreach( IPluginInfo* plugin, supportedPlugins )
    {
        if( !plugin->isAppInstalled() )
            continue;

        QCheckBox* cb;
        pluginsLayout->addWidget( cb = new QCheckBox( QString::fromStdString( plugin->name() ), this ));
        cb->setObjectName( QString::fromStdString( plugin->id() ) );
        cb->setChecked( true );

        if ( plugin->isInstalled() )
        {
            cb->setChecked( true );
            cb->setDisabled( true );
            cb->setText( cb->text() + " " + tr( "(Plugin installed or not required)" ));
        }
    }

//    pluginsLayout->addWidget( ui.iTunes = new QCheckBox( tr("iTunes"), this ) );
//    pluginsLayout->addWidget( ui.iWMP = new QCheckBox( tr("Windows Media Player"), this ) );
//    pluginsLayout->addWidget( ui.iWinAmp = new QCheckBox( tr("Winamp"), this ) );
//    pluginsLayout->addWidget( ui.iFoo = new QCheckBox( tr("Foobar 2000"), this ) );

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

//    QProcess pluginInstaller;
//    qDebug() << "Starting PluginInstaller";
//    pluginInstaller.start( "PluginInstaller.exe" );
//    pluginInstaller.waitForFinished( -1 );
//    qDebug() << "PluginInstaller finished.";
    return true;
}

void
PluginsPage::cleanupPage()
{
}

void
PluginsPage::initializePage()
{
    setTitle( tr( "Next step, install the Last.fm plugins to be able to scrobble the music you listen to." ));

    wizard()->setButton( FirstRunWizard::NextButton, tr( "Install Plugins" ) );
    if ( wizard()->canGoBack() )
        wizard()->setButton( FirstRunWizard::BackButton, tr( "<< Back" ) );
    wizard()->setButton( FirstRunWizard::SkipButton, tr( "Skip >>" ) );
}
