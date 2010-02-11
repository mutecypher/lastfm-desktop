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

#include <QMimeData>

#include "SourceSelectorListWidget.h"
#include "SourceListWidget.h"

SourceSelectorListWidget::SourceSelectorListWidget()
    :QListWidget()
{
}

QMimeData* SourceSelectorListWidget::mimeData( const QList<QListWidgetItem*> items ) const
{
    if( items.count() < 1 )
        return 0;

    QMimeData* mimeData = new QMimeData;

    // add this mime type so that we know it is coming from us
    mimeData->setData("application/x-rql-source", "");

    // set the actual data
    mimeData->setText(items.at(0)->data(Qt::DisplayRole).toString());
    mimeData->setImageData(items.at(0)->data(SourceListModel::ImageUrl).toString());

    return mimeData;
}
