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
#ifndef ANIMATED_LIST_LAYOUT_H
#define ANIMATED_LIST_LAYOUT_H

#include <lib/DllExportMacro.h>
#include <QVBoxLayout>
#include <QEasingCurve>

class UNICORN_DLLEXPORT AnimatedListLayout : public QLayout
{
    Q_OBJECT

public:
    AnimatedListLayout( int size, class QWidget* parent = 0 );
    ~AnimatedListLayout();

    void setAnimated( bool animated );
    void setEasingCurve( QEasingCurve::Type easingCurve );

    Qt::Orientations expandingDirections() const;
    bool hasHeightForWidth() const;
    int count() const;
    QLayoutItem *itemAt(int index) const;
    QSize minimumSize() const;
    void setGeometry(const QRect &rect);
    QSize sizeHint() const;
    QLayoutItem* takeAt(int index);

    void insertWidget( int index, QWidget* widget );

signals:
    void moveFinished();

private slots:
    void onFrameChanged( int frame );
    void onItemLoaded();

private:
    void doLayout( const QRect &rect, int vOffset = 0 );

    void addItem( QLayoutItem* item );

    void insertItem( int index, QLayoutItem * item );

private:
    QList<QLayoutItem *> m_itemList;
    QList<QLayoutItem *> m_newItemList;
    class QTimeLine* m_timeLine;
    bool m_animated;
    int m_size;
};

#endif // ANIMATED_LIST_LAYOUT_H
