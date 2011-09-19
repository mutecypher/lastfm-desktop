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
#ifndef PLUGINS_PAGE_H
#define PLUGINS_PAGE_H

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

class PluginsPage : public QWizardPage
{
    Q_OBJECT
public:
    PluginsPage();

private:
    bool validatePage();
    void cleanupPage();
    void initializePage();
};

#endif //PLUGINS_PAGE_H
