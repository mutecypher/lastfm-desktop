#ifndef SCROBBLES_MODEL_H
#define SCROBBLES_MODEL_H

#include <lastfm/Track>

#include <QAbstractTableModel>
#include <QStringList>

class ScrobblesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    ScrobblesModel( const QList<lastfm::Track> tracks, QObject* parent = 0 );

    int rowCount( const QModelIndex& parent = QModelIndex() ) const;
    int columnCount( const QModelIndex& parent = QModelIndex() ) const;

    QVariant data( const QModelIndex& index, int role ) const;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

private:
    enum
    {
        Artist,
        Title,
        Album,
        TimeStamp,
        Loved,
        AllowScrobbling,
    };

    class Scrobble
    {
    public:
        Scrobble( const lastfm::Track track ): m_track( track ), m_allowScrobbling( true ) {}

        lastfm::Track track() const{ return m_track; }
        QString title() const{ return m_track.title(); }
        QString artist() const{ return m_track.artist(); }
        QString album() const{ return m_track.album(); }
        QDateTime timeStamp() const{ return m_track.timestamp(); }
        bool isLoved() const{ return m_track.isLoved(); } 
        bool allowScrobbling() const{ return m_allowScrobbling; }

        void setAllowScrobbling( bool allow ) { m_allowScrobbling = allow; }

        QVariant attribute( int index ) const;

    private:
        lastfm::Track m_track;
        bool m_allowScrobbling;
    };

private:
    QList<Scrobble> m_scrobbleList;
    QStringList m_headerTitles;
};

#endif // SCROBBLES_MODEL_H
