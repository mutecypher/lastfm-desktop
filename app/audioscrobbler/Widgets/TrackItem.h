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

#ifndef TRACK_ITEM_H_
#define TRACK_ITEM_H_

#include <QWidget>
#include <QMovie>
#include <QPointer>

#include <lastfm/Track>

#include "lib/unicorn/StylableWidget.h"

#include "ActivityListItem.h"
#include "../ScrobbleInfoFetcher.h"


class ScrobbleInfoFetcher;
class ScrobbleInfoWidget;
class StopWatch;

class TrackItem : public ActivityListItem
{
    Q_OBJECT
public:
    TrackItem( const Track& track );
    TrackItem( const TrackItem& that );
    TrackItem( const QDomElement& element );

    QWidget* infoWidget() const;
    ScrobbleInfoFetcher* fetcher() const { return m_fetcher; }

    Track track() const { return m_track; }

signals:
    void cogMenuAboutToShow();
    void cogMenuAboutToHide();

private:
    void enterEvent( class QEvent* event );
    void leaveEvent( class QEvent* event );

    QDomElement toDomElement( QDomDocument xml ) const;

protected:
    void doSetTrack( const Track& track );

public slots:
    virtual void updateTimestamp();

private slots:
    void onLoveToggled( bool loved );

    void onLoveClicked();
    void onTagClicked();
    void onShareClicked();

    void onScrobbleStatusChanged();

    void onCorrected( QString correction );

protected:
    Track m_track;

    QPointer<ScrobbleInfoFetcher> m_fetcher;
    QPointer<ScrobbleInfoWidget> m_infoWidget;
};

#endif // TRACK_ITEM_H_

