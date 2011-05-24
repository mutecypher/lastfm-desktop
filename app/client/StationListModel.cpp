#include "StationListModel.h"

#include <lastfm/XmlQuery>
#include <lastfm/Tasteometer>

StationListModelItem::StationListModelItem( const RadioStation& station, const QString& description, QObject* parent )
    :QObject( parent ), m_station( station ), m_description( description ), m_tasteometerScore( 0 )
{
    m_icon.addFile(";/station-start-rest.png", QSize(17, 17), QIcon::Normal, QIcon::On);
    m_icon.addFile(":/station-start-onpress.png", QSize(17, 17), QIcon::Normal, QIcon::Off);
}


void
StationListModelItem::setName( const QString& name )
{
    m_name = name;
    emit changed();

    connect( lastfm::Tasteometer::compare( User(), User(m_name)), SIGNAL(finished()), SLOT(onGotTasteometerScore()));
}


void
StationListModelItem::setTimestamp( const QDateTime& timestamp )
{
    m_timestamp = timestamp;
    emit changed();
}


void
StationListModelItem::setTasteometerScore( float tasteometerScore )
{
    m_tasteometerScore = tasteometerScore;
    emit changed();
}


void
StationListModelItem::onGotTasteometerScore()
{
    lastfm::XmlQuery lfm = lastfm::XmlQuery( static_cast<QNetworkReply*>( sender() )->readAll() );

    m_tasteometerScore = lfm["comparison"]["result"]["score"].text().toFloat();
    //emit changed();
}


void
StationListModelItem::onGotAvatar()
{

}


StationListModel::StationListModel(QObject *parent)
    :QAbstractItemModel(parent)
{
}


void StationListModel::clear()
{
    m_model.clear();
    emit layoutChanged();
}

void
StationListModel::addItem( const RadioStation& station, const QString& description )
{
    StationListModelItem* data = new StationListModelItem( station, description, this );
    data->setTimestamp( QDateTime::currentDateTime() );

    m_model << data;

    connect( data, SIGNAL(changed()), SIGNAL(layoutChanged()));

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

bool
StationListModel::setData( const QModelIndex & index, const QVariant & value, int role )
{
    bool result = false;

    if ( role == NameRole )
    {
        m_model[ index.row() ]->setName( value.toString() );
        result = true;
    }
    else if ( role == TasteometerScoreRole )
    {
        m_model[ index.row() ]->setTasteometerScore( value.toFloat() );
        result = true;
    }

    if (result)
        emit dataChanged(index, index);

    return result;
}

QVariant
StationListModel::data( const QModelIndex & index, int role ) const
{
    QVariant data;

    switch (role)
    {
    case Qt::DisplayRole:
        if ( index.column() == 0 )
            data = m_model[index.row()]->station().title();
        else if ( index.column() == 1 )
        {
            float tasteometerScore = m_model[ index.row() ]->tasteometerScore();

            if ( tasteometerScore == 0 )
                data = m_model[index.row()]->description();
            else
                data = QString( "%1 %2" ).arg( QString( "%1" ).arg( tasteometerScore * 100, 0, 'f', 2, '0' ), m_model[index.row()]->description() );
        }
        break;
    case Qt::DecorationRole:
        if ( index.column() == 0 )
            data = m_model[index.row()]->icon();
        break;
    case TitleRole:
        data = m_model[index.row()]->station().title();
        break;
    case UrlRole:
        data = m_model[index.row()]->station().url();
        break;
    case TimestampRole:
        data = m_model[index.row()]->timestamp();
        break;
    case NameRole:
        data = m_model[index.row()]->name();
        break;
    case TasteometerScoreRole:
        data = m_model[index.row()]->tasteometerScore();
        break;
    }

    return data;
}
