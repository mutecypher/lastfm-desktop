/*
   Copyright 2010 Last.fm Ltd.
      - Primarily authored by Jono Cole and Micahel Coffey

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

#include <QStatusBar>

#include <lastfm/Track>
#include <lastfm/UserDetails>

class QLabel;

namespace Phonon { class VolumeSlider; }

class StatusBar : public QStatusBar
{
    Q_OBJECT
    public:
        StatusBar( QWidget* parent = 0);

        void setSizeGripVisible( bool visible );

    private slots:
        void onMessagedChanged( const QString& text );
        void setStatus();

        void onGotUserInfo(lastfm::UserDetails userDetails);

        void onConnectionUp();
        void onConnectionDown();

        void onIPodDetected( QString );
        void onProcessingScrobbles();
        void onFoundScrobbles( QList<lastfm::Track> );
        void onNoScrobblesFound();

    private:
        struct
        {
            class QPushButton* cog;

            class QLabel* volMin;
            Phonon::VolumeSlider* volumeSlider;
            class QLabel* volMax;

            class QPushButton* scrobbleToggle;
            class QSizeGrip* sizeGrip;
        } ui;

        bool m_online;
};
