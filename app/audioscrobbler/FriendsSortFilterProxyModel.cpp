/*
   Copyright 2005-2009 Last.fm Ltd.
      - Primarily authored by Jono Cole and Micahel Coffey

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

#include <QItemSelectionModel>

#include "FriendsSortFilterProxyModel.h"
#include "StationListModel.h"


FriendsSortFilterProxyModel::FriendsSortFilterProxyModel( QObject* parent )
    :QSortFilterProxyModel( parent ), m_selectionModel( 0 )
{
}


void
FriendsSortFilterProxyModel::setSelectionModel( QItemSelectionModel* selectionModel )
{
    m_selectionModel = selectionModel;
}


bool
FriendsSortFilterProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    if ( sortRole() == StationListModel::TimestampRole )
        return left.data( sortRole() ).toDateTime() < right.data( sortRole() ).toDateTime();
    else if ( sortRole() == StationListModel::TasteometerScoreRole )
        return left.data( sortRole() ).toFloat() < right.data( sortRole() ).toFloat();

    return left.data( sortRole() ).toString() < right.data( sortRole() ).toString();
}


bool
FriendsSortFilterProxyModel::filterAcceptsRow( int sourceRow, const QModelIndex& sourceParent ) const
{
    if ( m_selectionModel )
    {
        bool selected = false;

        qDebug() << "Source parent row: " << sourceParent.row();

        foreach ( QModelIndex index, m_selectionModel->selectedRows() )
        {
            qDebug() << "Selected row: " << index.row();

            if ( index.row() == sourceParent.row() )
                selected = true;
        }

        if ( selected )
            return true;
    }

    QRegExp re = filterRegExp();
    re.setCaseSensitivity( Qt::CaseInsensitive );
    QString title = sourceModel()->index(sourceRow, 0, sourceParent).data( StationListModel::TitleRole ).toString();
    if ( re.indexIn( title ) != -1 )
        return true;

    return false;
}
