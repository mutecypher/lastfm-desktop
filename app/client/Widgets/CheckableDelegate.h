#ifndef CHECKABLEDELEGATE_H
#define CHECKABLEDELEGATE_H

#include <QStyledItemDelegate>

class CheckableDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    CheckableDelegate( QObject* parent = 0 );

    void paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
    bool testHover( const QModelIndex& index, const QStyleOptionViewItem& option ) const;
    virtual bool editorEvent( QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index ) ;
    virtual QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;

private:
    QRect iconRect( const QModelIndex& index, const QStyleOptionViewItem& option ) const;
    QString prettyTime( const QDateTime& timestamp ) const;
};

#endif // CHECKABLEDELEGATE_H
