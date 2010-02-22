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

#include <QLayout>
#include <QScrollArea>

#include "DataListWidget.h"

DataListWidget::DataListWidget(QWidget* parent)
    :QListWidget(parent)
{
    setFlow( QListView::LeftToRight );
    setWrapping( true );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    setDragEnabled( true );
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
    setFrameShape( QFrame::NoFrame );

    //static_cast<QScrollArea*>(viewport())->setWidgetResizable( true );

    //viewport()->setMinimumHeight(0);
    //viewport()->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );

    //setResizeMode( QListWidget::Adjust );

    //setLayoutMode( QListWidget::SinglePass );
}


