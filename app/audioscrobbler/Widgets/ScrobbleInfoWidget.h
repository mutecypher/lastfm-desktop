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

#ifndef SCROBBLE_INFO_WIDGET_H_
#define SCROBBLE_INFO_WIDGET_H_

#include <lastfm/Track>

#include "lib/unicorn/StylableWidget.h"

class DataListWidget;
class HttpImageWidget;
class QLabel;
class QGroupBox;
class QTextBrowser;

class ScrobbleInfoWidget : public StylableWidget
{
    Q_OBJECT
public:
    ScrobbleInfoWidget( QWidget* p = 0 );

public slots:
    void onTrackGotInfo(const XmlQuery& lfm);
    void onAlbumGotInfo(const XmlQuery& lfm);
    void onArtistGotInfo(const XmlQuery& lfm);
    void onArtistGotEvents(const XmlQuery& lfm);
    void onTrackGotTopFans(const XmlQuery& lfm);
    void onTrackGotTags(const XmlQuery& lfm);

protected slots:
    void onAnchorClicked( const QUrl& link );
    void onBioChanged( const QSizeF& );

    void onTrackStarted(const Track&, const Track&);
    void onStopped();

    void listItemClicked( const QModelIndex& );

signals:
    void lovedStateChanged(bool loved);

protected:
    void setupUi();

    struct {
         class BannerWidget* onTourBanner;
         class QScrollArea* scrollArea;
         class QWidget* contents;
         class QListView* listeningNow;
         class QTextBrowser* bioText;
         class QListView* similarArtists;
         class QLabel* title1;
         class QLabel* title2;
         class HttpImageWidget* artistImage;
         class QLabel* yourScrobbles;
         class QLabel* totalScrobbles;
         class DataListWidget* yourTags;
         class DataListWidget* topTags;
    } ui;

    struct {
        class LfmListModel* similarArtists;
        class LfmListModel* listeningNow;
    } model;
};

#endif //SCROBBLE_INFO_WIDGET_H_
