/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Jono Cole

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
#ifndef PROFILE_WIDGET_H_
#define PROFILE_WIDGET_H_

#include "lib/unicorn/StylableWidget.h"

namespace unicorn{ class Session; }
namespace lastfm{ class Track; }
namespace lastfm{ class UserDetails; } 

class ProfileWidget : public StylableWidget
{
    Q_OBJECT

public:
    ProfileWidget( QWidget* p = 0 );

protected slots:
    void onSessionChanged( const unicorn::Session& );
    void onGotUserInfo( const lastfm::UserDetails& );
    void onScrobblesCached( const QList<lastfm::Track>& tracks );
    void onScrobblesSubmitted( const QList<lastfm::Track>& tracks, int succeeded );

protected:
    struct {
        class QLabel* welcomeLabel;
        class ScrobbleMeter* scrobbleMeter;
        class QLabel* since;
        class HttpImageWidget* avatar;
        class RecentTracksWidget* recentTracks;
    } ui;

    QString m_path;
};

#endif //PROFILE_WIDGET_H_
