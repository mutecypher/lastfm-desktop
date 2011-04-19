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
        float tasteometerScore;
    };

public:
    enum DataRole
    {
        TitleRole = Qt::UserRole,
        UrlRole,
        TimestampRole,
        NameRole,
        TasteometerScoreRole
    };

    explicit StationListModel(QObject *parent = 0);

    void addItem( const RadioStation& station, const QString& description );

    void clear();

    QVariant data ( const QModelIndex & index, int role ) const;
    int rowCount( const QModelIndex & parent = QModelIndex() ) const;
    QModelIndex index( int row, int column, const QModelIndex& parent = QModelIndex() ) const;

private:    
    QModelIndex parent( const QModelIndex & index ) const;
    int columnCount( const QModelIndex & parent ) const;
    bool setData( const QModelIndex & index, const QVariant & value, int role );

private:
    QList<Data> m_model;
};

#endif // STATIONLISTMODEL_H
