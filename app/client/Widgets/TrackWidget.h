#ifndef TRACKWIDGET_H
#define TRACKWIDGET_H

#include <QWidget>

#include <lastfm/Track.h>

#include <lib/unicorn/StylableWidget.h>

namespace Ui { class TrackWidget; }

class TrackWidget : public StylableWidget
{
    Q_OBJECT
    
public:
    explicit TrackWidget( Track& track, QWidget *parent = 0 );
    ~TrackWidget();

    void setTrack( lastfm::Track& track );
    lastfm::Track track() const;

private slots:
    void onLoveClicked( bool loved );
    void onTagClicked();
    void onShareClicked();
    void onBuyClicked();

    void onGotBuyLinks();

    void onBuyActionTriggered( QAction* buyAction );

    void onShareLastFm();
    void onShareTwitter();
    void onShareFacebook();

    // signals from the track
    void onLoveToggled( bool loved );
    void onScrobbleStatusChanged();
    void onCorrected( QString correction );

    void updateTimestamp();

private:
    QString price( const QString& price, const QString& currency ) const;

    void setTrackDetails();

    void resizeEvent(QResizeEvent *);

private:
    Ui::TrackWidget *ui;

    lastfm::Track m_track;

    QPointer<QMovie> m_movie;
    QPointer<QTimer> m_timestampTimer;
};

#endif // TRACKWIDGET_H
