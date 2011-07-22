#ifndef NOWPLAYINGSTACKEDWIDGET_H
#define NOWPLAYINGSTACKEDWIDGET_H

#include <QWidget>
#include <lastfm/Track>
#include <lastfm/RadioStation>

class TrackItem;
class RadioProgressBar;
class QLabel;
class QImage;

class NowPlayingStackedWidget : public QWidget
{
    Q_OBJECT;

public:
    NowPlayingStackedWidget( QWidget* parent = 0 );

private slots:
    void showNowPlaying();

private:
    struct {
        class NothingPlayingWidget* nothingPlaying;
        class NowPlayingWidget* nowPlaying;
    } ui;
};

#endif // NOWPLAYINGSTACKEDWIDGET_H
