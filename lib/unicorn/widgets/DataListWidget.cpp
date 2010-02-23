/*
   Copyright 2005-2009 Last.fm Ltd.
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

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

#include <QDebug>
#include <QLayout>
#include <QScrollArea>
#include <QUrl>
#include <QDesktopServices>
#include <QMimeData>

#include "DataListWidget.h"

DataListWidget::DataListWidget(QWidget* parent)
    :QListWidget(parent)
{
    setFlow( QListView::LeftToRight );
    setWrapping( true );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setFrameShape( QFrame::NoFrame );
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
    setSelectionMode( QAbstractItemView::SingleSelection );
    setDragEnabled( true );
    model()->setSupportedDragActions( Qt::CopyAction );

    connect(this, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(onItemActivated(QListWidgetItem*)));
}

QMimeData* DataListWidget::mimeData(const QList<QListWidgetItem*> items) const
{
    if (items.count() < 1)
        return 0;

    QMimeData* data = new QMimeData();
    data->setText( items[0]->text() );

    QList<QUrl> urls;
    urls.append( items[0]->data(LastFMUrl).toUrl() );
    data->setUrls( urls );

    return data;
}

void DataListWidget::onItemActivated(QListWidgetItem* item)
{
    QDesktopServices::openUrl( item->data(LastFMUrl).toUrl() );
}

