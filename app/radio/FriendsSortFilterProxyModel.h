#ifndef FRIENDSSORTFILTERPROXYMODEL_H
#define FRIENDSSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class FriendsSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    FriendsSortFilterProxyModel( QObject* parent = 0 );

private:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};

#endif // FRIENDSSORTFILTERPROXYMODEL_H
