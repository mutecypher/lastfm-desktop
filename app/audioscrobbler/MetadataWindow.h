/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

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
#include "lib/unicorn/UnicornMainWindow.h"
#include "lib/unicorn/StylableWidget.h"
#include <lastfm/Track>

class MetadataWindow : public unicorn::MainWindow
{
    Q_OBJECT

    struct{ 
        class FirstRunWizard* firstrun;
        class RestWidget* rest;
        QWidget* nowScrobbling;
    } stack;

    struct{
        class ScrobbleStatus* now_playing_source;
        class HttpImageWidget* artist_image;
        class QLabel* title;
        class QLabel* album;
        class DataListWidget* topFans;
        class QLabel* onTour;
        class QLabel* onTourBlank;
        class DataListWidget* similarArtists;
        class QLabel* artistScrobbles;
        class QLabel* albumScrobbles;
        class QLabel* trackScrobbles;
        class DataListWidget* tags;
        class QTextBrowser* bio;

        class QPushButton* love;
        class QPushButton* tag;
        class QPushButton* share;
        class ScrobbleControls* sc;

        class MessageBar* message_bar;
    } ui;

public:
    MetadataWindow();
    const Track& currentTrack() const{ return m_currentTrack; }
    class ScrobbleControls* scrobbleControls() const{ return ui.sc; }

public slots:
    void onTrackStarted(const Track&, const Track&);
    void onStopped();
    void onPaused();
    void onResumed();

private slots:
    void onArtistGotInfo();
    void onArtistGotEvents();

    void onAlbumGotInfo();

    void onTrackGotInfo();
    void onTrackGotTopFans();

    void onAnchorClicked( const QUrl& link );
    void onBioChanged( const QSizeF& );

    void onArtistImageClicked();
    
private:
    Track m_currentTrack;
    void setCurrentWidget( QWidget* );
};

class TitleBar : public StylableWidget {
Q_OBJECT
public:
    TitleBar(const QString&);

signals:
    void closeClicked();
};
