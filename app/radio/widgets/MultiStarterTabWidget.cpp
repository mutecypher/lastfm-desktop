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
#include <QGroupBox>
#include <QListWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QTreeWidgetItem>

#include <lastfm/User>
#include <lastfm/XmlQuery>

#include "MultiStarterTabWidget.h"
#include "SourceSelectorWidget.h"
#include "SourceListWidget.h"
#include "lib/unicorn/layouts/SideBySideLayout.h"
#include "lib/unicorn/widgets/SearchBox.h"
#include "YouListWidget.h"
#include "AdvancedOptionsWidget.h"


MultiStarterTabWidget::MultiStarterTabWidget(int maxSources, RqlSource::Type type, SearchBox* searchBox, QWidget *parent)
    : StylableWidget(parent)
    , m_minCount(10)
    , m_type(type)
{
    QHBoxLayout* layout = new QHBoxLayout;

    m_sourceSelector = new SourceSelectorWidget(searchBox);
    layout->addWidget(m_sourceSelector);

    m_sourceModel = new SourceListModel(maxSources, this);
    m_sourceList = new SourceListWidget(this);
    m_sourceList->setModel(m_sourceModel);

    QVBoxLayout* rightside = new QVBoxLayout;
    rightside->addWidget(m_sourceList);
    rightside->addWidget(m_playButton = new QPushButton(tr("Play combo")));

    layout->addLayout(rightside);

    QVBoxLayout* v1 = new QVBoxLayout(this);
    switch (type)
    {
    case RqlSource::Art:
        v1->addWidget(new QLabel( tr("Choose up to three artists and press play.") ), 0, Qt::AlignCenter);
        break;
    case RqlSource::User:
        v1->addWidget(new QLabel( tr("Choose up to three libraries and press play.") ), 0, Qt::AlignCenter);
        break;
    case RqlSource::Tag:
        v1->addWidget(new QLabel( tr("Choose up to three tags and press play.") ), 0, Qt::AlignCenter);
        break;
    }
    v1->addLayout( layout );

    connect(m_sourceList, SIGNAL(add(QString, QString)), SLOT(onAdd(QString, QString)));
    connect(m_sourceSelector, SIGNAL(add(QString)), SLOT(onAdd(QString)));
    connect(m_sourceSelector, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(onAddItem(QListWidgetItem*)));

    connect(m_playButton, SIGNAL(clicked()), SLOT(onPlayClicked()));

#if QT_VERSION >= 0x040500
    // this makes the left and right-hand side have the same horizontal width
    layout->setStretch(0, 1);
    layout->setStretch(1, 1);
#endif
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
        item->data(SourceListModel::ImageUrl).toString());
}

void
MultiStarterTabWidget::onGotTopTags()
{
    doPopulateModel("toptags", "tag", "name", "count", "count");
}

void
MultiStarterTabWidget::onUserGotTopArtists()
{
    doPopulateModel("topartists", "artist", "name", "playcount", "playcount");
}

void
MultiStarterTabWidget::onUserGotFriends()
{
    QListWidgetItem* item = new QListWidgetItem(m_sourceSelector->list());

    // Add the authenticated user as first in the list
    lastfm::User you;
    item->setData(Qt::DisplayRole, you.name().toLower());
    item->setData(Qt::ToolTipRole, you.realName());
    item->setData(SourceListModel::SourceType, m_type);
    item->setData(SourceListModel::Arg1, you.name());
    item->setData(SourceListModel::Sort, " ");

    item->setData(SourceListModel::ImageUrl, you.imageUrl(lastfm::Small));

    // get the icon image
    QNetworkReply* reply = lastfm::nam()->get(QNetworkRequest(you.imageUrl(lastfm::Small)));
    connect(reply, SIGNAL(finished()), SLOT(onImageGetFinished()));

    doPopulateModel("friends", "user", "name", "realname", "");

    // we sort friend lists
    m_sourceSelector->list()->sortItems(Qt::AscendingOrder);
}

void
MultiStarterTabWidget::doPopulateModel(QString listE, QString itemE, QString nameE, QString toolTipE, QString barChartE)
{
    sender()->deleteLater();
    QNetworkReply* r = (QNetworkReply*)sender();
    lastfm::XmlQuery lfm(r->readAll());

    m_sourceSelector->list()->setIconSize(QSize(32, 32));

    float biggest(0);

    if (!barChartE.isEmpty())
    {
        biggest = lfm[listE].children(itemE).at(0)[barChartE].text().toFloat();
    }

    foreach (lastfm::XmlQuery e, lfm[listE].children(itemE))
    {
        QListWidgetItem* item = new QListWidgetItem(m_sourceSelector->list());

        QString name = e[nameE].text();

        item->setData(Qt::DisplayRole, m_type == RqlSource::User ? name.toLower() : name);
        item->setData(Qt::ToolTipRole, e[toolTipE].text());
        item->setData(SourceListModel::SourceType, m_type);
        item->setData(SourceListModel::Arg1, name);
        item->setData(SourceListModel::Sort, name.toLower());

        if (!barChartE.isEmpty())
        {
            // set the bar chart background effect
            float pixelWidth = 1.0 / m_sourceSelector->list()->width();
            QLinearGradient linearGradient(20, 0, 165, 0);
            linearGradient.setColorAt( (e[barChartE].text().toFloat() / biggest) - pixelWidth, QColor(0x71, 0xB7, 0xE6) );
            linearGradient.setColorAt( e[barChartE].text().toFloat() / biggest, QColor(0xFF, 0xFF, 0xFF) );
            QBrush brush(linearGradient);
            item->setData(Qt::BackgroundRole, brush);
        }

        if (m_type == RqlSource::Tag)
        {
            item->setData(Qt::DecorationRole, QIcon(":/tag-small.png") );
        }
        else
        {
            QString imageUrl = e["image size=small"].text();

            item->setData(SourceListModel::ImageUrl, imageUrl);

            // get the icon image
            QNetworkReply* reply = lastfm::nam()->get(QNetworkRequest(QUrl(imageUrl)));
            connect(reply, SIGNAL(finished()), SLOT(onImageGetFinished()));
        }
    }

    if (m_sourceSelector->list()->count() < m_minCount)
    {
        // no items. so?
    }
}

void
MultiStarterTabWidget::onImageGetFinished()
{
    sender()->deleteLater();
    QNetworkReply* reply = (QNetworkReply*) sender();

    if (reply->error() == QNetworkReply::NoError)
    {
        QPixmap p;
        p.loadFromData(((QNetworkReply*)sender())->readAll());

        if (!p.isNull())
        {
            // find the item that this was downloaded for
            for (int idx(m_sourceSelector->list()->count() - 1) ; idx >= 0 ; --idx)
            {
                QString url = m_sourceSelector->list()->item(idx)->data(SourceListModel::ImageUrl).toString();
                QString replyUrl = reply->url().toString();

                if ( !url.isEmpty() && (url == replyUrl) )
                {
                    m_sourceSelector->list()->item(idx)->setData(Qt::DecorationRole, QIcon(p.scaled(17, 17, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation).copy(QRect(0, 0, 17, 17))));
                    break;
                }
            }
        }
    }
}

void
MultiStarterTabWidget::onPlayClicked()
{
    int count = 0;
    QString rql;
    foreach (const QString& src, m_sourceModel->rql())
    {
        if (count)
        {
            // get operator
            switch (m_type)
            {
                case RqlSource::User:
                case RqlSource::Art: rql += " or "; break;
                case RqlSource::Tag: rql += " and "; break;
            }
        }
        rql += src;
        count++;
    }

    if (rql.length())
    {
        RadioStation rs = RadioStation::rql(rql);

        count = 0;
        QString description;
        foreach (const QString& src, m_sourceModel->descriptions()) {
            if (count) {
                // get operator

                switch (m_type) {
                    case RqlSource::User:
                    case RqlSource::Art: description += " or "; break;
                    case RqlSource::Tag: description += " and "; break;
                }
            }
            description += src;
            count++;
        }

        rs.setTitle(description);

        emit startRadio(rs);
    }
}
