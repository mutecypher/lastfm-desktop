#ifndef FRIENDSSORTFILTERPROXYMODEL_H
#define FRIENDSSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class FriendsSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    FriendsSortFilterProxyModel( const class QItemSelectionModel& selectionModel, QObject* parent = 0 );

private:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    const class QItemSelectionModel& m_selectionModel;
};

#endif // FRIENDSSORTFILTERPROXYMODEL_H
