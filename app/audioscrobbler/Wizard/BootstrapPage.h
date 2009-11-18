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

#include "lib/unicorn/UpdateInfoFetcher.h"

class BootstrapPage: public QWizardPage
{
    Q_OBJECT
public:
    BootstrapPage( QWidget* parent = 0 )
    :QWizardPage( parent) 
    { 
        new QVBoxLayout( this );
    }
    
    void fetchInfo()
    {
        QNetworkReply* reply = UpdateInfoFetcher::fetchInfo();
        connect( reply, SIGNAL( finished()), SLOT(updateInfoFetched()));
    }

    void initializePage()
    {
        fetchInfo();
    }

private slots:
    void updateInfoFetched() 
    {
        QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
        UpdateInfoFetcher info( reply, this );
        QLabel* label = new QLabel( tr( "We can import your listening history from the following media players:"));
        label->setWordWrap( true );
        layout()->addWidget(label);
        foreach( const Plugin& plugin, info.plugins()) {
            if( plugin.isInstalled() && 
                plugin.isPluginInstalled() &&
                plugin.canBootstrap())
                layout()->addWidget( new QRadioButton( plugin.name(), this ));
        }
        layout()->addWidget( new QRadioButton( tr("None"), this ));
        ((QBoxLayout*)layout())->addStretch();
        update();
    }
};

#endif //BOOTSTRAP_WIZARD_H
