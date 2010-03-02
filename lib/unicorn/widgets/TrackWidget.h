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
#ifndef TRACK_WIDGET_H
#define TRACK_WIDGET_H

#include <lastfm/global.h>
#include <lastfm/Track>
#include <QWidget>
#include <QPixmap>
#include "lib/DllExportMacro.h"
#include <QStackedWidget>

class UNICORN_DLLEXPORT TrackWidget : public QWidget
{
    Q_OBJECT   
public:
    enum Type
    {
        Artist,
        Album,
        Track
    } m_type;

private:
    struct
    {
        class QLabel* image;
        class QLabel* description;
        class QPixmap artistImage;
        class QPixmap albumImage;
    } ui;

public:
    TrackWidget( const lastfm::Track& track );

    void setType( Type type );
    
private slots:
    void onCoverDownloaded( const class QImage& );

private:
    const lastfm::Track& m_track;
    class TrackImageFetcher* m_fetcher;
};

#endif
