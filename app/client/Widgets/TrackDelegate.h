#ifndef TRACKDELEGATE_H
#define TRACKDELEGATE_H

#include <QStyledItemDelegate>

class TrackDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    TrackDelegate( QObject* parent = 0 );

private:
    void paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
};

#endif // TRACKDELEGATE_H
