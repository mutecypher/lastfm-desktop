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
        Scrobble( const lastfm::Track track );

        lastfm::Track track() const;
        QString title() const;
        QString artist() const;
        QString album() const;
        QDateTime timestamp() const;
        bool isLoved() const;
        bool isScrobblingEnabled() const;

        void setEnableScrobbling( bool allow );

        QVariant attribute( int index ) const;

        int originalPlayCount() const;

        bool operator<( const Scrobble& that ) const;

    private:
        lastfm::Track m_track;
        bool m_scrobblingEnabled;
        int m_originalPlayCount;
    };

private:
    QList<Scrobble> m_scrobbleList;
    QStringList m_headerTitles;
    bool m_readOnly;
};

#endif // SCROBBLES_MODEL_H
