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
            return timestamp();
        case Loved:
            return isLoved();
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
}

int
ScrobblesModel::rowCount( const QModelIndex& /*parent*/ ) const
{
    return m_scrobbleList.count();
}

int
ScrobblesModel::columnCount( const QModelIndex& /*parent*/ ) const
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

    if ( index.column() >= m_headerTitles.size() )
        return QVariant();


    if ( role == Qt::DisplayRole )
    {
        return m_scrobbleList.at( index.row() ).attribute( index.column() );
    }
    else if ( role == Qt::CheckStateRole && index.column() == 0 )
    {
        return m_scrobbleList.at( index.row() ).isScrobblingEnabled()? Qt::Checked : Qt::Unchecked;
    }
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

Qt::ItemFlags
ScrobblesModel::flags( const QModelIndex& index ) const
{
    if ( !index.isValid() )
        return Qt::ItemIsEnabled;

    if ( index.column() == Artist )
        return QAbstractItemModel::flags( index ) | Qt::ItemIsUserCheckable;

    return QAbstractItemModel::flags( index );
}

bool
ScrobblesModel::setData( const QModelIndex& index, const QVariant& value, int role )
{
    if ( index.isValid() && role == Qt::CheckStateRole )
    {
        Scrobble s = m_scrobbleList.at( index.row() );
        s.setEnableScrobbling( value.toBool() );
        m_scrobbleList.replace( index.row(), s );
        emit dataChanged( index, index );
        return true;
    }
    return false;
}

QList<lastfm::Track>
ScrobblesModel::tracksToScrobble() const
{
    QList<lastfm::Track> tracks;
    for ( int i = 0; i < m_scrobbleList.count(); i ++ )
    {
        if ( m_scrobbleList.at( i ).isScrobblingEnabled() )
        {
            tracks.append( m_scrobbleList.at( i ).track() );
        }
    }
    return tracks;
}
