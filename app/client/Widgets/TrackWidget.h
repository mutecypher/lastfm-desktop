#ifndef TRACKWIDGET_H
#define TRACKWIDGET_H

#include <QWidget>

#include <lastfm/Track.h>

namespace Ui { class TrackWidget; }

class TrackWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit TrackWidget( Track& track, QWidget *parent = 0 );
    ~TrackWidget();

    void setTrack( lastfm::Track& track );
    lastfm::Track track() const;

private slots:
    void onLoveClicked();
    void onTagClicked();
    void onShareClicked();
    void onBuyClicked();

    void onGotBuyLinks();

    void onBuyActionTriggered( QAction* buyAction );

    void onShareLastFm();
    void onShareTwitter();
    void onShareFacebook();

private:
    QString price( const QString& price, const QString& currency ) const;
    QString prettyTime( const QDateTime& timestamp ) const;

private:
    Ui::TrackWidget *ui;

    lastfm::Track m_track;

    QPoint m_buyCursor;
};

#endif // TRACKWIDGET_H
