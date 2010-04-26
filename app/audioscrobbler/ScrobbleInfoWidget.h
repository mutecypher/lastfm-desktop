#ifndef SCROBBLE_INFO_WIDGET_H_
#define SCROBBLE_INFO_WIDGET_H_
#include "lib/unicorn/StylableWidget.h"
#include <lastfm/Track>

class HttpImageWidget;
class QLabel;
class DataListWidget;
class QTextBrowser;

class ScrobbleInfoWidget : public StylableWidget
{
    Q_OBJECT
public:
    ScrobbleInfoWidget( QWidget* p = 0 );

public slots:
    void onTrackGotInfo(const XmlQuery& lfm);

protected slots:
    void onArtistGotInfo();
    void onArtistGotEvents();
    void onAlbumGotInfo();

    void onTrackGotTopFans();

    void onAnchorClicked( const QUrl& link );
    void onBioChanged( const QSizeF& );

    void onTrackStarted(const Track&, const Track&);
    void onStopped();

signals:
    void lovedStateChanged(bool loved);

protected:
    struct {
        HttpImageWidget* artist_image;
        QLabel* title;
        QLabel* album;
        DataListWidget* topFans;
        QLabel* onTour;
        QLabel* onTourBlank;
        DataListWidget* similarArtists;
        QLabel* artistScrobbles;
        QLabel* albumScrobbles;
        QLabel* trackScrobbles;
        DataListWidget* tags;
        QTextBrowser* bio;
    } ui;
};

#endif //SCROBBLE_INFO_WIDGET_H_
