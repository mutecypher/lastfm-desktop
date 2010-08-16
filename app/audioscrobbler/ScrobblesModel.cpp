#include "ScrobblesModel.h"

QVariant
ScrobblesModel::Scrobble::attribute( int index ) const
{
    switch( index )
    {
        case Artist:
            return artist();
        case Title:
            return title();
        case Album:
            return album();
        case TimeStamp:
            return timeStamp();
        case Loved:
            return isLoved();
        case AllowScrobbling:
            return allowScrobbling();
        default:
            return QVariant();
    }
}

ScrobblesModel::ScrobblesModel( const QList<lastfm::Track> tracks, QObject* parent )
    : QAbstractTableModel( parent )
{
    foreach( lastfm::Track t, tracks )
    {
        m_scrobbleList.append( t );
    }
    m_headerTitles.append( "Artist" );
    m_headerTitles.append( "Title" );
    m_headerTitles.append( "Album" );
    m_headerTitles.append( "Time listened" );
    m_headerTitles.append( "Loved" );
    m_headerTitles.append( "Scrobble track" );
}

int
ScrobblesModel::rowCount( const QModelIndex& parent ) const
{
    return m_scrobbleList.count();
}

int
ScrobblesModel::columnCount( const QModelIndex& parent ) const
{
    return m_headerTitles.count();
}

QVariant
ScrobblesModel::data( const QModelIndex& index, int role ) const
{
    if ( !index.isValid() )
        return QVariant();

    if ( index.row() >= m_scrobbleList.size() )
        return QVariant();

    if ( index.column() >= m_headerTitles.size() );
        return QVariant();

    if ( role == Qt::DisplayRole )
        return m_scrobbleList.at( index.row() ).attribute( index.column() );
    else
        return QVariant();
}

QVariant
ScrobblesModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( role != Qt::DisplayRole )
        return QVariant();

    if ( orientation == Qt::Horizontal )
        return m_headerTitles.at( section );
    else
        return QVariant();
}
