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
#include "SourceSelectorWidget.h"
#include "SourceListWidget.h"
#include "lib/unicorn/layouts/SideBySideLayout.h"
#include "SearchBox.h"
#include "YouListWidget.h"
#include "AdvancedOptionsWidget.h"

#include <QGroupBox>


MultiStarterTabWidget::MultiStarterTabWidget(int maxSources, RqlSource::Type type, SearchBox* searchBox, QWidget *parent)
    : StylableWidget(parent)
    , m_minCount(10)
    , m_type(type)
{
    QHBoxLayout* layout = new QHBoxLayout(this);

    m_sourceSelector = new SourceSelectorWidget(searchBox);
    layout->addWidget(m_sourceSelector);

    m_sourceModel = new SourceListModel(maxSources, this);
    m_sourceList = new SourceListWidget(this);
    m_sourceList->setModel(m_sourceModel);

    QVBoxLayout* rightside = new QVBoxLayout(this);
    rightside->addWidget(m_sourceList);
    rightside->addWidget(m_playButton = new QPushButton(tr("Play combo")));

    layout->addLayout(rightside);

    setLayout(layout);

    connect(m_sourceSelector, SIGNAL(add(QString)), SLOT(onAdd(QString)));
    connect(m_sourceSelector, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(onAddItem(QListWidgetItem*)));

    connect(m_playButton, SIGNAL(clicked()), SLOT(onPlayClicked()));
}

void
MultiStarterTabWidget::onAdd(const QString& item, const QString& imgUrl)
{

    if (m_sourceModel->addSource(RqlSource(m_type, item, QString(), 1.0, imgUrl)))
    {
        // todo: grey it out if it's in the list?  or grey it some other way?
    }
}

void 
MultiStarterTabWidget::onAddItem(QListWidgetItem* item)
{
    onAdd(
        item->data(Qt::DisplayRole).toString(), 
        item->data(Qt::DecorationRole).toString());
}

void
MultiStarterTabWidget::onGotTopTags()
{
    sender()->deleteLater();
    QNetworkReply* r = (QNetworkReply*)sender();
    lastfm::XmlQuery lfm(r->readAll());

    QStringList tags;
    foreach (lastfm::XmlQuery e, lfm["toptags"].children("tag")) {
        tags += e["name"].text();
    }
    m_sourceSelector->list()->insertItems(0, tags);
}

void
MultiStarterTabWidget::onUserGotTopArtists()
{
    sender()->deleteLater();
    QNetworkReply* r = (QNetworkReply*)sender();
    lastfm::XmlQuery lfm(r->readAll());

    foreach (lastfm::XmlQuery e, lfm["topartists"].children("artist")) {
        QListWidgetItem* item = new QListWidgetItem(m_sourceSelector->list());
        item->setData(Qt::DisplayRole, e["name"].text());
        item->setData(Qt::DecorationRole, e["image size=small"].text());
    }
    if (m_sourceSelector->list()->count() < m_minCount) {
        // get global top artists
    }
}

void
MultiStarterTabWidget::onUserGotFriends()
{
    sender()->deleteLater();
    QNetworkReply* r = (QNetworkReply*)sender();
    lastfm::XmlQuery lfm(r->readAll());

    foreach (lastfm::XmlQuery e, lfm["friends"].children("user")) {
        QListWidgetItem* item = new QListWidgetItem(m_sourceSelector->list());
        item->setData(Qt::DisplayRole, e["name"].text());
        item->setData(Qt::ToolTipRole, e["realname"].text());
        item->setData(Qt::DecorationRole, e["image size=small"].text());
    }
    if (m_sourceSelector->list()->count() < m_minCount) {
        // no friends. so?
    }
}

void
MultiStarterTabWidget::onPlayClicked()
{
    QString rql = m_sourceList->rql();
    if (rql.length()) {
        RadioStation rs = RadioStation::rql(rql);
        rs.setTitle(m_sourceList->stationDescription());
        emit startRadio(rs);
    }
}
