

#include <QItemSelectionModel>

#include "FriendsSortFilterProxyModel.h"
#include "StationListModel.h"



FriendsSortFilterProxyModel::FriendsSortFilterProxyModel(  const QItemSelectionModel& selectionModel, QObject* parent )
    :QSortFilterProxyModel( parent ), m_selectionModel( selectionModel )
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
FriendsSortFilterProxyModel::filterAcceptsRow( int sourceRow, const QModelIndex& sourceParent ) const
{
    if ( m_selectionModel.rowIntersectsSelection( sourceRow, sourceModel()->index(sourceRow, 0, sourceParent) ) )
        return true;

    QRegExp re = filterRegExp();
    re.setCaseSensitivity( Qt::CaseInsensitive );
    QString title = sourceModel()->index(sourceRow, 0, sourceParent).data( StationListModel::TitleRole ).toString();
    if ( re.indexIn( title ) != -1 )
        return true;

    return false;
}
