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

#include <QWidget>

#include <lastfm/Track>

#include "lib/unicorn/StylableWidget.h"

class RecentTrackWidget : public StylableWidget
{
    Q_OBJECT
public:
    RecentTrackWidget( const Track& track );

    Track track() const { return m_track;}

    QString status() const { return m_status; }

signals:
    void loaded();

    void cogMenuAboutToShow();
    void cogMenuAboutToHide();

private:
    void enterEvent( class QEvent* event );
    void leaveEvent( class QEvent* event );
    void resizeEvent( class QResizeEvent* event );

    void setStatus( QString status ) { m_status = status; }

private slots:
    void onLoveToggled( bool loved );

    void onLoveClicked();
    void onTagClicked();
    void onShareClicked();

    void updateTimestamp();

    void onGotInfo( const XmlQuery& lfm );

    void onScrobbleStatusChanged();

private:
    struct
    {
        class QLabel* trackText;
        class HttpImageWidget* albumArt;
        class QLabel* love;
        class QToolButton* cog;
        class GhostWidget* ghostCog;
        class QLabel* timestamp;
    } ui;

    Track m_track;
    class QTimer* m_timestampTimer;

    Q_PROPERTY(QString status READ status WRITE setStatus);
    QString m_status;
};
