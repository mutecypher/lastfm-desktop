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

#include <QDebug>
#include <QLayout>
#include <QScrollArea>
#include <QUrl>
#include <QDesktopServices>
#include <QMimeData>
#include <QUrl>
#include <QLabel>
#include <QDrag>
#include <QMouseEvent>
#include <QApplication>

#include "DataListWidget.h"

#include "layouts/flowlayout.h"

class DataItem : public QLabel
{
public:
    explicit DataItem( const QString& text, const QUrl& url ) 
    :m_url(url)
    {
        setText( text );
    }

    QUrl url() const{ return m_url; }

protected:
    void mouseReleaseEvent( QMouseEvent* event )
    {
        if ((event->pos() - m_dragStartPosition).manhattanLength()
                >= QApplication::startDragDistance())
            return;
        QDesktopServices::openUrl( url() );
    }

    void mousePressEvent(QMouseEvent *event)
    {
        if (event->button() == Qt::LeftButton)
            m_dragStartPosition = event->pos();
    }

    void mouseMoveEvent(QMouseEvent *event)
    {
        if (!(event->buttons() & Qt::LeftButton))
            return;
        if ((event->pos() - m_dragStartPosition).manhattanLength()
                < QApplication::startDragDistance())
            return;

        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        mimeData->setText( text() );

        QList<QUrl> urls;
        urls.append( url() );
        mimeData->setUrls( urls );
        drag->setMimeData(mimeData);

        Qt::DropAction dropAction = drag->exec(Qt::CopyAction);
    }

    QUrl m_url;
    QPoint m_dragStartPosition;
};

DataListWidget::DataListWidget(QWidget* parent)
    :StylableWidget(parent)
{
    new FlowLayout( this, 0, 0, 0 );
    setContentsMargins( 0, 3, 0, 7 );
    connect(this, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(onItemActivated(QListWidgetItem*)));
}

/*QMimeData* DataListWidget::mimeData(const QList<QListWidgetItem*> items) const
{
    if (items.count() < 1)
        return 0;

    QMimeData* data = new QMimeData();

    return data;
}
*/
void 
DataListWidget::clear()
{
    foreach( QObject* c, findChildren<QWidget*>())
        c->deleteLater();
}

void 
DataListWidget::addItem( const QString& text, const QUrl& url )
{
    layout()->addWidget( new DataItem( text, url ));
}

/*void DataListWidget::onItemActivated(QListWidgetItem* item)
{
    QDesktopServices::openUrl( item->data(LastFMUrl).toUrl() );
}*/

