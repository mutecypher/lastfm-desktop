#include "StationListModel.h"

StationListModel::StationListModel(QObject *parent)
    :QAbstractItemModel(parent)
{
}


void
StationListModel::addItem( const RadioStation& station, const QString& description )
{
    Data data;
    data.timestamp = QDateTime::currentDateTime();
    data.description = description;
    data.station = station;
    data.icon.addFile(";/station-start-rest.png", QSize(17, 17), QIcon::Normal, QIcon::On);
    data.icon.addFile(":/station-start-onpress.png", QSize(17, 17), QIcon::Normal, QIcon::Off);

    m_model << data;

    emit layoutChanged();
}


QModelIndex
StationListModel::index( int row, int column, const QModelIndex& parent ) const
{
    return createIndex( row, column );
}


QModelIndex
StationListModel::parent( const QModelIndex & index ) const
{
    return QModelIndex();
}


int
StationListModel::rowCount( const QModelIndex & parent ) const
{
    return m_model.count();
}


int
StationListModel::columnCount( const QModelIndex & parent ) const
{
    return 2;
}


QVariant
StationListModel::data( const QModelIndex & index, int role ) const
{
    QVariant data;

    switch (role)
    {
    case Qt::DisplayRole:
        if ( index.column() == 0 )
            data = m_model[index.row()].station.title();
        else if ( index.column() == 1 )
            data = m_model[index.row()].description;
        break;
    case Qt::DecorationRole:
        if ( index.column() == 0 )
            data = m_model[index.row()].icon;
        break;
    case TitleRole:
        data = m_model[index.row()].station.title();
        break;
    case UrlRole:
        data = m_model[index.row()].station.url();
        break;
    case TimestampRole:
        data = m_model[index.row()].timestamp;
        break;
    case NameRole:
        data = m_model[index.row()].name;
        break;
    }

    return data;
}
