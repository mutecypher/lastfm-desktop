#ifndef ACTIVITY_LIST_WIDGET_H
#define ACTIVITY_LIST_WIDGET_H

#include <QListWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QPointer>

#include <lastfm/Track.h>

namespace lastfm { class Track; }
using lastfm::Track;

namespace unicorn { class Session; }

class QNetworkReply;

class ActivityListWidget : public QListWidget
{
    Q_OBJECT
public:
    ActivityListWidget( QWidget* parent = 0 );

signals:
    void trackClicked( const Track& );
    void refreshing( bool refreshing );

public slots:
    void refresh();

private slots: 
    void onItemClicked( const QModelIndex& index );

    void onTrackStarted( const Track& track, const Track& );

    void onSessionChanged( unicorn::Session* session );
    void onSessionChanged( const QString& username );

    void onResumed();
    void onPaused();
    void onStopped();

    void onGotRecentTracks();

    void onScrobblesSubmitted( const QList<lastfm::Track>& tracks );

    void write();
    void doWrite();

#ifdef Q_OS_MAC
    void scroll();
#endif

private:
    QString price( const QString& price, const QString& currency ) const;

    void read();

    QList<lastfm::Track> addTracks( const QList<lastfm::Track>& tracks );
    void limit( int limit );

    void hideScrobbledNowPlaying();

private:
    class ActivityListModel* m_model;

    QString m_path;

    QModelIndex m_shareIndex;

    QPoint m_buyCursor;

    QPointer<QTimer> m_writeTimer;
    QPointer<QNetworkReply> m_recentTrackReply;

    class ActivityListWidgetItem* m_trackItem;
    lastfm::Track m_track;
};


#endif //ACTIVITY_LIST_WIDGET_H

