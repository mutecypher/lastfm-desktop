/*
   Copyright 2005-2009 Last.fm Ltd.
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

#ifndef HTTP_IMAGE_WIDGET_H_
#define HTTP_IMAGE_WIDGET_H_

#include <QLabel>
#include <QUrl>
#include <QDesktopServices>
#include <QPainter>
#include <QMouseEvent>
#include "lib/DllExportMacro.h"

#include <lastfm/ws.h>

class UNICORN_DLLEXPORT HttpImageWidget : public QLabel
{
    Q_OBJECT
public:
    HttpImageWidget( QWidget* parent = 0 );

public slots:
    void loadUrl( const QUrl& url, bool gradient = false );
    void setHref( const QUrl& url );
    
protected:
    virtual void mousePressEvent( QMouseEvent* event );
    virtual void mouseReleaseEvent( QMouseEvent* event );

private slots:
    void onClick();
    void onUrlLoaded();

signals:
    void clicked();
    void loaded();

private:
    bool m_mouseDown;
    bool m_gradient;
    QUrl m_href;
};

#endif
