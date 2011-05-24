#ifndef STATIONLISTMODEL_H
#define STATIONLISTMODEL_H

#include <QAbstractItemModel>
#include <QIcon>

#include <lastfm/RadioStation>

class StationListModelItem : public QObject
{
    Q_OBJECT
public:
    StationListModelItem( const RadioStation& station, const QString& description, QObject* parent = 0 );

    void setName( const QString& );
    void setTimestamp( const QDateTime& );
    void setTasteometerScore( float );

    RadioStation station() const { return m_station; }
    QString description() const { return m_description; }
    QString name() const { return m_name; }
    QDateTime timestamp() const { return m_timestamp; }
    QIcon icon() const { return m_icon; }
    float tasteometerScore() const { return m_tasteometerScore; }

signals:
    void changed();

private slots:
    void onGotTasteometerScore();
    void onGotAvatar();

private:
    RadioStation m_station;
    QString m_description;
    QString m_name;
    QDateTime m_timestamp;
    QIcon m_icon;
    float m_tasteometerScore;
};

class StationListModel : public QAbstractItemModel
{
    Q_OBJECT
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
    QList<StationListModelItem*> m_model;
};

#endif // STATIONLISTMODEL_H
