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

#ifndef NOW_PLAYING_WIDGET_H
#define NOW_PLAYING_WIDGET_H

#include <QWidget>
#include <QPointer>

#include <lastfm/Track>
#include <lastfm/RadioStation>

#include "MetadataWidget.h"

class NowPlayingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NowPlayingWidget(QWidget *parent = 0);

signals:

private slots:
    void onTuningIn( const RadioStation& );
    void onTrackStarted( const Track& track, const Track& );
    void onStopped();

private:
    QPointer<MetadataWidget> m_metadata;
};

#endif
