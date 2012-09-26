#ifndef SCROBBLES_MODEL_H
#define SCROBBLES_MODEL_H

#include <QAbstractTableModel>
#include <QStringList>

#include <lastfm/Track.h>

#include "lib/DllExportMacro.h"

class UNICORN_DLLEXPORT ScrobblesModel : public QAbstractTableModel
{
    Q_OBJECT
public:

    enum
    {
        Artist,
        Title,
        Album,
        Plays,
        TimeStamp,
        Loved
    };


    ScrobblesModel( QObject* parent = 0 );
    void addTracks( const QList<lastfm::Track>& tracks );

    void setReadOnly();

    int rowCount( const QModelIndex& parent = QModelIndex() ) const;
    int columnCount( const QModelIndex& parent = QModelIndex() ) const;

    QVariant data( const QModelIndex& index, int role ) const;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    Qt::ItemFlags flags( const QModelIndex& index ) const;
    bool setData( const QModelIndex& index, const QVariant& value, int role );

    QList<lastfm::Track> tracksToScrobble() const;

private:
    class Scrobble
    {
    public:
        Scrobble( const lastfm::Track track ): m_track( track ), m_scrobblingEnabled( true ) {}

        lastfm::Track track() const{ return m_track; }
        QString title() const{ return m_track.title(); }
        QString artist() const{ return m_track.artist(); }
        QString album() const{ return m_track.album(); }
        QDateTime timestamp() const{ return m_track.timestamp(); }
        bool isLoved() const{ return m_track.isLoved(); } 
        bool isScrobblingEnabled() const{ return m_scrobblingEnabled; }

        void setEnableScrobbling( bool allow ) { m_scrobblingEnabled = allow; }

        QVariant attribute( int index ) const;

        bool operator<( const Scrobble& that ) const { return this->m_track < that.m_track;}

    private:
        lastfm::Track m_track;
        bool m_scrobblingEnabled;
    };

private:
    QList<Scrobble> m_scrobbleList;
    QStringList m_headerTitles;
    bool m_readOnly;
};

#endif // SCROBBLES_MODEL_H
