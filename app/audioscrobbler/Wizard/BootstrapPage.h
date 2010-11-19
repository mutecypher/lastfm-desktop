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
#ifndef BOOTSTRAP_WIZARD_H
#define BOOTSTRAP_WIZARD_H

#include <QWizardPage>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QRadioButton>
#include <QNetworkReply>

#include "lib/unicorn/Updater/PluginList.h"
#include "../Application.h"

class BootstrapPage: public QWizardPage
{
    Q_OBJECT
public:
    BootstrapPage( QWidget* parent = 0 )
    :QWizardPage( parent) 
    { 
        new QVBoxLayout( this );
    }
    
    void initializePage()
    {
        foreach( QWidget* w, findChildren<QWidget*>()) {
            layout()->removeWidget( w );
            delete w;
        }
        delete layout();
        new QVBoxLayout( this );

        QList<IPluginInfo*> plugins = m_pluginList.bootstrappablePlugins();

        QLabel* label = new QLabel( tr( "Hi %1,\nWe can import your listening history from the following media players:").arg( aApp->currentSession()->userInfo().name())); 
        label->setWordWrap( true );
        layout()->addWidget(label);
        foreach( IPluginInfo* plugin, plugins ) {
                layout()->addWidget( new QRadioButton( QString::fromStdString( plugin->name())));
        }

        layout()->addWidget( new QRadioButton( tr("None")));
        ((QBoxLayout*)layout())->addStretch();
    }

private:
    PluginList m_pluginList;
};

#endif //BOOTSTRAP_WIZARD_H
