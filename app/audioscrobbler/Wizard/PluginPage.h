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
#ifndef PLUGIN_PAGE_H
#define PLUGIN_PAGE_H

#include "lib/unicorn/UpdateInfoFetcher.h"

#include <QWizardPage>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QNetworkReply>
#include <QDebug>

class PluginPage : public QWizardPage
{
    Q_OBJECT
public:
    PluginPage()
    {
        new QVBoxLayout( this );
        QNetworkReply* rep = UpdateInfoFetcher::fetchInfo();
        connect( rep, SIGNAL(finished()), SLOT(onUpdateInfoFetched()));
    }


private slots:
    void onUpdateInfoFetched()
    {
        QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
        UpdateInfoFetcher info( reply );
        foreach( const Plugin& plugin, info.plugins())
        {
            layout()->addWidget( new QCheckBox( plugin.name(), this ));
        }
    }
};

#endif //PLUGIN_PAGE_H
