#ifndef NOWPLAYINGSTACKEDWIDGET_H
#define NOWPLAYINGSTACKEDWIDGET_H

#include <QWidget>
#include <lastfm/Track.h>
#include <lastfm/RadioStation.h>

class TrackItem;
class RadioProgressBar;
class QLabel;
class QImage;

class NowPlayingStackedWidget : public QWidget
{
    Q_OBJECT

private:
    struct {
        class NothingPlayingWidget* nothingPlaying;
        class NowPlayingWidget* nowPlaying;
    } ui;

public:
    NowPlayingStackedWidget( QWidget* parent = 0 );

    class NowPlayingWidget* nowPlaying() const { return ui.nowPlaying; }

private slots:
    void showNowPlaying();
    void showNothingPlaying();
};

#endif // NOWPLAYINGSTACKEDWIDGET_H
