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

#include "lib/unicorn/Updater/PluginList.h"

#include <QWizardPage>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QNetworkReply>
#include <QLabel>
#include <QStyle>
#include "../Application.h"
#include <QProcess>
#include <QDebug>

class PluginPage : public QWizardPage
{
    Q_OBJECT
public:
    PluginPage()
    {
        setTitle( "Install some plugins!" );
        new QVBoxLayout( this );
    }
	
	
	bool validatePage()
	{
        QProcess pluginInstaller;
        qDebug() << "Starting PluginInstaller";
        pluginInstaller.start( "PluginInstaller.exe" );
        pluginInstaller.waitForFinished( -1 );
        qDebug() << "PluginInstaller finished.";
        cleanupPage();
        return true;
	}

    void cleanupPage()
    {
        wizard()->button( QWizard::NextButton )->setIcon( QIcon());
    }

    void initializePage()
    {
        const QIcon vistaShield = QApplication::style()->standardIcon(QStyle::SP_VistaShield);
        wizard()->button( QWizard::NextButton )->setIcon( vistaShield );
        QLabel* description = new QLabel( tr( "Some media players need a plugin to be installed in order to scrobble.\n\nIf you do not want to scrobble from any of the detected media players then please uncheck them below:\n\n" )); 
        description->setWordWrap( true );
        layout()->addWidget( description );
        PluginList list;
        QList<IPluginInfo*> supportedPlugins = list.supportedList();
        foreach( IPluginInfo* plugin, supportedPlugins)
        {
			if( !plugin->isAppInstalled()) continue;
            QCheckBox* cb;
            layout()->addWidget( cb = new QCheckBox( QString::fromStdString( plugin->name()), this ));
            if( plugin->isInstalled()) {
                cb->setChecked( true );
                cb->setDisabled( true );
                cb->setText( cb->text() + " " + tr( "(Plugin installed or not required)" ));
            }
        }
    }
};

#endif //PLUGIN_PAGE_H
