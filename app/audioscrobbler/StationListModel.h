#ifndef STATIONLISTMODEL_H
#define STATIONLISTMODEL_H

#include <QAbstractItemModel>
#include <QIcon>

#include <lastfm/RadioStation>

class StationListModel : public QAbstractItemModel
{
    Q_OBJECT
private:
    struct Data
    {
        RadioStation station;
        QString description;
        QString name;
        QDateTime timestamp;
        QIcon icon;
    };

public:
    enum DataRole
    {
        TitleRole = Qt::UserRole,
        UrlRole,
        TimestampRole,
        NameRole
    };

    explicit StationListModel(QObject *parent = 0);

    void addItem( const RadioStation& station, const QString& description );

    QVariant data ( const QModelIndex & index, int role ) const;

private:
    QModelIndex index( int row, int column, const QModelIndex& parent ) const;
    QModelIndex parent( const QModelIndex & index ) const;
    int rowCount( const QModelIndex & parent ) const;
    int columnCount( const QModelIndex & parent ) const;

private:
    QList<Data> m_model;
};

#endif // STATIONLISTMODEL_H
