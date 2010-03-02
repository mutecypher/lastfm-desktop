/*
   Copyright 2005-2009 Last.fm Ltd. 

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

#include <QLabel>
#include <QCheckBox>
#include <QTabWidget>
#include <QGridLayout>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTreeWidgetItem>
#include <lastfm/User>
#include <lastfm/XmlQuery>
#include "MultiStarterTabWidget.h"
#include "MultiStarterWidget.h"
#include "SourceSelectorWidget.h"
#include "SourceListWidget.h"
#include "../SourceListModel.h"
#include "lib/unicorn/layouts/SideBySideLayout.h"
#include "lib/unicorn/widgets/SearchBox.h"
#include "YouListWidget.h"
#include "AdvancedOptionsWidget.h"

#include <QGroupBox>

class LabelledSlider : public QSlider
{
public:
    LabelledSlider(const QString& leftText, const QString& rightText)
        : QSlider(Qt::Horizontal)
    {
        QLayout* layout = new QHBoxLayout(this);
        layout->addWidget(new QLabel(leftText));
//        layout->addWidget(this);
        layout->addWidget(new QLabel(rightText));
    }
};


MultiStarterWidget::MultiStarterWidget(int maxSources, QWidget *parent)
    : StylableWidget(parent)
{
    QTabWidget* tabWidget = new QTabWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(this);
    setLayout( layout );
    layout->addWidget( tabWidget );

    lastfm::AuthenticatedUser you;

    tabWidget->addTab(m_artists = new MultiStarterTabWidget(maxSources, RqlSource::Art, new ArtistSearch(), this), tr("Multi-artist"));
    connect(m_artists, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));
    connect(you.getTopArtists(), SIGNAL(finished()), m_artists, SLOT(onUserGotTopArtists()));

    tabWidget->addTab(m_tags = new MultiStarterTabWidget(maxSources, RqlSource::Tag, new TagSearch(), this), tr("Multi-tag"));
    connect(m_tags, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));
    connect(Tag::getTopTags(), SIGNAL(finished()), m_tags, SLOT(onGotTopTags()));

    tabWidget->addTab(m_users = new MultiStarterTabWidget(maxSources, RqlSource::User, new UserSearch(), this), tr("Multi-library"));
    connect(m_users, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));
    connect(you.getFriends(), SIGNAL(finished()), m_users, SLOT(onUserGotFriends()));
}
