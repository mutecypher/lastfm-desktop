/*
   Copyright 2005-2009 Last.fm Ltd.
      - Primarily authored by Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef METADATA_WIDGET_H_
#define METADATA_WIDGET_H_

#include <lastfm/Track>

#include "lib/unicorn/StylableWidget.h"

class DataListWidget;
class HttpImageWidget;
class QLabel;
class QGroupBox;
class BioWidget;

#include "lib/unicorn/widgets/HttpImageWidget.h"
class MetadataWidget : public StylableWidget
{
    Q_OBJECT
public:
    MetadataWidget( const Track& track, bool showBack, QWidget* p = 0 );
    ~MetadataWidget() {}

    class ScrobbleControls* scrobbleControls() const { return ui.track.scrobbleControls; }

    QWidget* basicInfoWidget();
    void setBackButtonVisible( bool );

    QString contextString( const Track& track );

private slots:

    void onTrackGotInfo(const QByteArray& data);
    void onAlbumGotInfo();
    void onArtistGotInfo();
    void onArtistGotEvents();
    void onTrackGotTopFans();
    
    void onTrackGotYourTags();
    void onTrackGotPopTags();

    void onFinished();

    void onTrackCorrected( QString correction );
    void listItemClicked( const QModelIndex& );

    void onScrobblesCached( const QList<lastfm::Track>& tracks );
    void onScrobbleStatusChanged();

signals:
    void lovedStateChanged(bool loved);
    void backClicked();

private:
    void setTrackDetails( const Track& track );

protected:
    void setupTrackStats( QWidget* w );
    void setupTrackDetails( QWidget* w );
    void setupTrackTags( QWidget* w );
    void setupUi();
    void fetchTrackInfo();

    struct {
         class QScrollArea* scrollArea;
         class QPushButton* backButton;

         struct {
             class QLabel* title;
             class QLabel* album;
             class QLabel* artist;
             
             class ScrobbleControls* scrobbleControls;

             QWidget* trackStats;
             class QLabel* yourScrobbles;
             class QLabel* totalScrobbles;
             class QLabel* listeners;

             class QLabel* context;

             class HttpImageWidget* albumImage;
             class DataListWidget* yourTags;
             class DataListWidget* popTags;
         } track;

         struct {
            class QLabel* artist;
            class BioWidget* bio;
            class BannerWidget* banner;
            class HttpImageWidget* image;
         } artist;
    } ui;

    Track m_track;
    int m_globalTrackScrobbles;
    int m_userTrackScrobbles;
    int m_userArtistScrobbles;

    struct {
        class LfmListModel* similarArtists;
        class LfmListModel* listeningNow;
    } model;

    QNetworkReply* m_trackInfoReply;

    QNetworkReply* m_albumInfoReply;
    QNetworkReply* m_artistInfoReply;
    QNetworkReply* m_artistEventsReply;
    QNetworkReply* m_trackTopFansReply;
    QNetworkReply* m_trackTagsReply;
};

#endif //METADATA_WIDGET_H_
