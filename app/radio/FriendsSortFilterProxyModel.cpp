

#include "FriendsSortFilterProxyModel.h"

#include "StationListModel.h"

FriendsSortFilterProxyModel::FriendsSortFilterProxyModel( QObject* parent )
    :QSortFilterProxyModel( parent )
{
}


bool
FriendsSortFilterProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    if ( sortRole() == StationListModel::TimestampRole )
        return left.data( sortRole() ).toDateTime() < right.data( sortRole() ).toDateTime();

    return left.data( sortRole() ).toString() < right.data( sortRole() ).toString();
}


bool
FriendsSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    QRegExp re = filterRegExp();
    re.setCaseSensitivity( Qt::CaseInsensitive );
    QString title = sourceModel()->index(sourceRow, 0, sourceParent).data( StationListModel::TitleRole ).toString();
    return ( re.indexIn( title ) != -1 );
}
