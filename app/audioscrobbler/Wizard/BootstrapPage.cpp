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

PlayerSelectorListWidget::PlayerSelectorListWidget( QWidget* p )
                         :QWidget( p )
{
    QList<IPluginInfo*> plugins = m_pluginList.bootstrappablePlugins();

    new QVBoxLayout( this );
    bool first = true;
    QRadioButton* rb;
    foreach( IPluginInfo* plugin, plugins ) {
        layout()->addWidget( rb = new QRadioButton( QString::fromStdString( plugin->name())));
        rb->setObjectName( QString::fromStdString( plugin->id() ) );
        connect( rb, SIGNAL( toggled( bool )), SLOT( onPluginToggled( bool )));

        if( first ) {
            rb->setChecked( true );
            m_playerId = rb->objectName();
        }
        first = false;
    }

    layout()->addWidget( rb = new QRadioButton( tr("None")));
    connect( rb, SIGNAL( toggled( bool )), SLOT( onPluginToggled( bool )));
}


void
PlayerSelectorListWidget::onPluginToggled( bool checked )
{
    if( !checked ) return;
    QAbstractButton* rb = qobject_cast<QAbstractButton*>( sender());
    m_playerId = rb->objectName();
    emit playerChanged();
}

void 
BootstrapPage::initializePage()
{
    setTitle( tr( "Hi, %1" ).arg( aApp->currentSession()->userInfo().name()));
    foreach( QWidget* w, findChildren<QWidget*>()) {
        layout()->removeWidget( w );
        delete w;
    }
    delete layout();
    new QVBoxLayout( this );


#ifdef Q_OS_MAC
    QLabel* label = new QLabel( tr( "We recommend importing your listening history from your media player. Please select your prefered option below:")); 
#else
    QLabel* label = new QLabel( tr( "Cool, we recommend importing your listening history from your media player. Please select your prefered option below:")); 
#endif
    label->setWordWrap( true );
    layout()->addWidget(label);
    PlayerSelectorListWidget* psl = new PlayerSelectorListWidget( this );
    layout()->addWidget( psl );
    registerField( "bootstrap_player", psl, "playerId", SIGNAL( playerChanged() ));
    layout()->addWidget( new QLabel( tr( "Why? So we can give you better recommendations." )));
    
    ((QBoxLayout*)layout())->addStretch();
}

