#ifndef ACTIVITY_LIST_MODEL_H_
#define ACTIVITY_LIST_MODEL_H_

#include <QAbstractItemModel>
#include <QImage>
#include <QPixmap>
#include <QIcon>
#include <QDebug>

#include <lastfm/ws.h>
#include <lastfm/Track>

#include "ImageTrack.h"

namespace unicorn { class Session; }


class ActivityListModel : public QAbstractItemModel {
    Q_OBJECT
public:
    enum
    {
        TrackNameRole = Qt::UserRole,
        ArtistNameRole,
        LovedRole,
        TimeStampRole,
        HoverStateRole,
        TrackRole
    };

    ActivityListModel();

    virtual QModelIndex index( int row, int column, const QModelIndex& parent = QModelIndex()) const { return createIndex( row, column ); }
    virtual QModelIndex parent( const QModelIndex& index ) const { return QModelIndex(); }
    virtual int rowCount( const QModelIndex& parent = QModelIndex()) const { return parent.isValid() ? 0 : m_tracks.length(); }
    virtual int columnCount( const QModelIndex& parent = QModelIndex()) const { return parent.isValid() ? 0 : 5; }
    virtual QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
    virtual bool setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    virtual Qt::ItemFlags flags( const QModelIndex& index ) const;
    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

signals:
    void refreshing( bool refreshing );

public slots:
    void onFoundIPodScrobbles( const QList<lastfm::Track>& tracks );
    void addTracks( const QList<lastfm::Track>& tracks );
    void onSessionChanged( unicorn::Session* session );
    void onTrackLoveToggled();
    void onGotRecentTracks();
    void refresh();

private slots:
    void write() const;

private:
    void read();
    void limit( int limit );

private:
    QList<ImageTrack> m_tracks;
    QString m_path;
    QIcon m_loveIcon;
    QIcon m_tagIcon;
    QIcon m_shareIcon;
    QImage m_noArt;
    QModelIndex m_hoverIndex;
};

#endif //ACTIVITY_LIST_MODEL_H_
