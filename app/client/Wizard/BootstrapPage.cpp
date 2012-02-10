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

#include "FirstRunWizard.h"
#include "BootstrapPage.h"

BootstrapPage::BootstrapPage()
{ 
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 20 );

    m_pluginsLayout = new QVBoxLayout();
    m_pluginsLayout->setContentsMargins( 0, 0, 0, 0 );
    m_pluginsLayout->setSpacing( 0 );

    layout->addLayout( m_pluginsLayout );

    layout->addWidget( ui.description = new QLabel( tr( "<p>For the best possible recommendations based on your music taste we advise that you import your listening history from your media player.</p>"
                                                        "<p>Please select your prefered media player and click <strong>Start Import</strong></p>" ) ),
                       0,
                       Qt::AlignTop);
    ui.description->setObjectName( "description" );
    ui.description->setWordWrap( true );
}

void
BootstrapPage::playerSelected()
{
    m_playerId = qobject_cast<QRadioButton*>(sender())->objectName();
    emit playerChanged();
}

bool
BootstrapPage::validatePage()
{
    /// start the bootstrap from whatever music player they chose.
    aApp->startBootstrap( m_playerId );

    // once you start importing you can't go back
    // if they didn't click "Start Import" they won't get here
    // so we allow them to go back and bootstrap
    wizard()->setCommitPage( true );

    return true;
}


void 
BootstrapPage::initializePage()
{
    QRadioButton* rb;
#ifdef Q_OS_WIN
    QList<IPluginInfo*> plugins = wizard()->pluginList()->bootstrappablePlugins();

    bool first = true;

    foreach ( IPluginInfo* plugin, plugins )
    {
        m_pluginsLayout->addWidget( rb = new QRadioButton( plugin->name()));
        rb->setObjectName( plugin->id() );

        connect( rb, SIGNAL(clicked()), SLOT(playerSelected()));

        if ( first )
        {
            rb->setChecked( true );
            m_playerId = plugin->id();
        }

        first = false;
    }
#else
    m_pluginsLayout->addWidget( rb = new QRadioButton( tr( "iTunes" ) ) );
    rb->setChecked( true );
    m_playerId = "osx";
#endif

    m_pluginsLayout->addStretch();

    setTitle( tr( "Now let's import your listening history" ) );

    wizard()->setButton( FirstRunWizard::NextButton, tr( "Start Import" ) );
    if ( wizard()->canGoBack() )
        wizard()->setButton( FirstRunWizard::BackButton, tr( "<< Back" ) );
    wizard()->setButton( FirstRunWizard::SkipButton, tr( "Skip >>" ) );
}

void
BootstrapPage::cleanupPage()
{
    while ( m_pluginsLayout->count() > 0 )
        m_pluginsLayout->takeAt( 0 )->widget()->deleteLater();
}


