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
#include "BootstrapPage.h"

BootstrapPage::BootstrapPage( QWidget* parent )
    :QWizardPage( parent)
{ 
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    QHBoxLayout* pluginsLayout = new QHBoxLayout( this );
    pluginsLayout->setContentsMargins( 0, 0, 0, 0 );
    pluginsLayout->setSpacing( 0 );

    layout->addLayout( pluginsLayout );

    QList<IPluginInfo*> plugins = m_pluginList.bootstrappablePlugins();

    bool first = true;

    QRadioButton* rb;

    foreach ( IPluginInfo* plugin, plugins )
    {
        pluginsLayout->addWidget( rb = new QRadioButton( QString::fromStdString( plugin->name())));
        rb->setObjectName( QString::fromStdString( plugin->id() ) );

        connect( rb, SIGNAL(clicked()), SLOT(playerSelected()));

        if ( first )
        {
            rb->setChecked( true );
            m_playerId = QString::fromStdString( plugin->id() );
        }

        first = false;
    }

    layout->addWidget( ui.description = new QLabel( tr( "<p>For the best possible recommendations based on your music taste we advise that you import your listening history from your media player.</p>"
                                                        "<p>Please select your prefered media player and click <strong>Start Import</strong></p>" ) ),
                       0,
                       Qt::AlignTop);
    ui.description->setObjectName( "description" );
    ui.description->setWordWrap( true );

    registerField( "bootstrap_player", this, "playerId", SIGNAL( playerChanged() ));
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

    return true;
}


void 
BootstrapPage::initializePage()
{
    setTitle( tr( "Now let's import your listening history" ) );

    wizard()->setButtonText( QWizard::NextButton, tr( "Start Import" ) );
    wizard()->setButtonText( QWizard::BackButton, tr( "<< Back" ) );
    wizard()->setOption( QWizard::HaveCustomButton1, true );
    wizard()->setButtonText( QWizard::CustomButton1, tr( "Skip >>" ) );
}

void
BootstrapPage::cleanupPage()
{
    wizard()->setOption( QWizard::HaveCustomButton1, false );
}


