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

#include "HttpImageWidget.h"

HttpImageWidget::HttpImageWidget( QWidget* parent )
    :QLabel( parent ), m_mouseDown( false )
{
}


void HttpImageWidget::loadUrl( const QUrl& url, bool gradient )
{
    m_gradient = gradient;
    connect( lastfm::nam()->get(QNetworkRequest(url)), SIGNAL(finished()), SLOT(onUrlLoaded()));
}

void HttpImageWidget::setHref( const QUrl& url )
{
    m_href = url;
    if( m_href.isValid()) {
        setCursor( Qt::PointingHandCursor );
        connect( this, SIGNAL(clicked()), SLOT(onClick()));
    } else {
        setCursor( Qt::ArrowCursor );
        disconnect( this, SIGNAL( clicked()), this, SLOT(onClick()));
    }
}

    
void HttpImageWidget::mousePressEvent( QMouseEvent* event )
{
    m_mouseDown = true;
}

void HttpImageWidget::mouseReleaseEvent( QMouseEvent* event )
{
    if( m_mouseDown &&
        contentsRect().contains( event->pos() )) emit clicked();

    m_mouseDown = false;
}

void HttpImageWidget::onClick()
{
    QDesktopServices::openUrl( m_href);
}

void HttpImageWidget::onUrlLoaded()
{
    if ( static_cast<QNetworkReply*>(sender())->error() == QNetworkReply::NoError )
    {
        QPixmap px;
        px.loadFromData(static_cast<QNetworkReply*>(sender())->readAll());

        if( m_gradient ) {
            QLinearGradient g(QPoint(), px.rect().bottomLeft());
            g.setColorAt( 0.0, QColor(0, 0, 0, 0.11*255));
            g.setColorAt( 1.0, QColor(0, 0, 0, 0.88*255));

            QPainter p(&px);
            p.setCompositionMode(QPainter::CompositionMode_Multiply);
            p.fillRect(px.rect(), g);
            p.end();
        }

        setPixmap(px);
    }

    emit loaded();
}

