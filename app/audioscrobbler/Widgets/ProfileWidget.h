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
#include <lastfm/Track>

namespace unicorn{ class Session; }
namespace lastfm{ class Track; }
using lastfm::Track;
namespace lastfm{ class UserDetails; } 
namespace lastfm{ class XmlQuery; }
using lastfm::XmlQuery;

class ProfileWidget : public StylableWidget
{
    Q_OBJECT

public:
    ProfileWidget( QWidget* p = 0 );

private:
    void setTrackText();

private slots:
    void onSessionChanged( unicorn::Session* );
    void updateUserInfo( const lastfm::UserDetails& );
    void onScrobblesCached( const QList<lastfm::Track>& tracks );
    void onScrobbleStatusChanged();

    void onTrackStarted( const Track&, const Track& );
    void onPaused();
    void onResumed();
    void onStopped();

    void onCorrected( QString correction );

protected:
    struct {
        class QLabel* welcomeLabel;
        class ScrobbleMeter* scrobbleMeter;
        class QLabel* since;
        class HttpImageWidget* avatar;
        class QLabel* title1;
        class QLabel* title2;
        class QLabel* correction;
        class ScrobbleControls* sc;
    } ui;

    QString m_path;
    Track m_track;
};

#endif //PROFILE_WIDGET_H_
