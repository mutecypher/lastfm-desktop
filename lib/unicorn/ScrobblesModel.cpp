
#include <QDebug>

#include "ScrobblesModel.h"

QVariant
ScrobblesModel::Scrobble::attribute( int index ) const
{
    switch( index )
    {
        case ScrobblesModel::Artist: return artist();
        case ScrobblesModel::Title: return title();
        case ScrobblesModel::Album: return album();
        case ScrobblesModel::TimeStamp: return timestamp();
        case ScrobblesModel::Plays: return track().extra( "playCount" ).toInt();
        case ScrobblesModel::Loved: return isLoved();
        default: break;
    }

    return QVariant();
}

ScrobblesModel::ScrobblesModel( QObject* parent )
    : QAbstractTableModel( parent ), m_readOnly( false )
{
    m_headerTitles.append( tr( "Artist" ) );
    m_headerTitles.append( tr( "Title" ) );
    m_headerTitles.append( tr( "Album" ) );
    m_headerTitles.append( tr( "Plays" ) );
    m_headerTitles.append( tr( "Last Played" ) );
    m_headerTitles.append( tr( "Loved" ) );
}

void
ScrobblesModel::addTracks( const QList<lastfm::Track>& tracks )
{
    beginInsertRows( QModelIndex(), m_scrobbleList.count(), m_scrobbleList.count() + tracks.count() - 1 );

    foreach( lastfm::Track t, tracks )
        m_scrobbleList.append( t );

    endInsertRows();
}

void
ScrobblesModel::setReadOnly()
{
    m_readOnly = true;
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
        return m_scrobbleList.at( index.row() ).attribute( index.column() );
    else if ( role == Qt::CheckStateRole && index.column() == 0 )
    {
        if ( m_readOnly )
            return QVariant();
        else
            return m_scrobbleList.at( index.row() ).isScrobblingEnabled() ? Qt::Checked : Qt::Unchecked;
    }

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

    if ( index.column() == ScrobblesModel::Artist )
        return QAbstractItemModel::flags( index ) | Qt::ItemIsUserCheckable;
    if ( index.column() == ScrobblesModel::Plays )
        return QAbstractItemModel::flags( index ) | Qt::ItemIsEditable;

    return QAbstractItemModel::flags( index );
}

bool
ScrobblesModel::setData( const QModelIndex& index, const QVariant& value, int role )
{
    if ( index.isValid() )
    {
        Scrobble s = m_scrobbleList.at( index.row() );

        if ( index.column() == ScrobblesModel::Artist && role == Qt::CheckStateRole )
        {
            s.setEnableScrobbling( value.toBool() );
            m_scrobbleList.replace( index.row(), s );
            emit dataChanged( index, index );
            return true;
        }
        else if ( index.column() == ScrobblesModel::Plays )
        {
            bool ok;
            int count = value.toInt( &ok );

            if ( ok )
            {
                lastfm::MutableTrack( s.track() ).setExtra( "playCount", QString::number( count ) );
                emit dataChanged( index, index );
            }

            return true;
        }
    }

    return false;
}

QList<lastfm::Track>
ScrobblesModel::tracksToScrobble() const
{
    QList<lastfm::Track> tracks;

    for ( int i = 0 ; i < m_scrobbleList.count() ; i ++ )
        if ( m_scrobbleList.at( i ).isScrobblingEnabled() )
            tracks.append( m_scrobbleList.at( i ).track() );

    return tracks;
}
