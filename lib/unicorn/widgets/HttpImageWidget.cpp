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

#include "lib/unicorn/DesktopServices.h"

HttpImageWidget::HttpImageWidget( QWidget* parent )
    :QLabel( parent ), m_mouseDown( false )
{
    setAttribute( Qt::WA_LayoutUsesWidgetRect, true );
}

void 
HttpImageWidget::setPlaceholder( const QPixmap& placeholder )
{
    if( !pixmap() || pixmap()->isNull())
        setPixmap( placeholder.scaled( maximumWidth(), maximumHeight(), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
}


void
HttpImageWidget::loadUrl( const QUrl& url, ScaleType scale )
{
    m_scale = scale;
    connect( lastfm::nam()->get(QNetworkRequest(url)), SIGNAL(finished()), SLOT(onUrlLoaded()));
}

void HttpImageWidget::setHref( const QUrl& url )
{
#ifdef Q_OS_MAC
    //On OSX Percent encoding seems to get applied to the url again.
    m_href = QUrl::fromPercentEncoding( url.toString().toUtf8() );
#else
    m_href = url;
#endif

    setToolTip( m_href.toString() );

    if( m_href.isValid()) {
        setCursor( Qt::PointingHandCursor );
        connect( this, SIGNAL(clicked()), SLOT(onClick()));
    } else {
        unsetCursor();
        disconnect( this, SIGNAL( clicked()), this, SLOT(onClick()));
    }
}

    
void HttpImageWidget::mousePressEvent( QMouseEvent* /*event*/ )
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
    unicorn::DesktopServices::openUrl( m_href );
}

void HttpImageWidget::onUrlLoaded()
{
    if ( static_cast<QNetworkReply*>(sender())->error() == QNetworkReply::NoError )
    {
        QPixmap px;
        if ( px.loadFromData(static_cast<QNetworkReply*>(sender())->readAll()) )
        {
            switch ( m_scale )
            {
            case ScaleAuto:
                // Decide which way to scale based on the ratio of height to width
                // of the image and the area that the image is going to be drawn to
                if ( (px.height() * 1000) / px.width() > (height() * 1000) / width() )
                    px = px.scaledToWidth( contentsRect().width(), Qt::SmoothTransformation );
                else
                    px = px.scaledToHeight( contentsRect().height(), Qt::SmoothTransformation );

                break;
            case ScaleNone:
                break;
            case ScaleWidth:
                px = px.scaledToWidth( contentsRect().width(), Qt::SmoothTransformation );
                break;
            case ScaleHeight:
                px = px.scaledToHeight( contentsRect().height(), Qt::SmoothTransformation );
                break;
            }

            setPixmap( px );
        }
    }

    emit loaded();
}

