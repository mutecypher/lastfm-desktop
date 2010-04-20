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
#ifndef SLIDE_OVER_LAYOUT_H
#define SLIDE_OVER_LAYOUT_H

#include <lib/DllExportMacro.h>
#include <QLayout>

/** @brief: A layout that allows 2 more widgets to be laid out 
  *         with one sliding over the other with nice animation.
  */

/** Warning: this is still pretty hacky and heavily based on code
  *          in my SideBySideLayout!!
  */

class UNICORN_DLLEXPORT SlideOverLayout : public QLayout
{
    Q_OBJECT

public:
    SlideOverLayout( class QWidget* parent = 0 );
    ~SlideOverLayout();
    
    void addItem(QLayoutItem *item);
    Qt::Orientations expandingDirections() const;
    bool hasHeightForWidth() const;
    int count() const;
    QLayoutItem *itemAt(int index) const;
    QSize minimumSize() const;
    void setGeometry(const QRect &rect);
    QSize sizeHint() const;
    QLayoutItem *takeAt(int index);
    QWidget* currentWidget();
    
    /** Return previous widget or 0 if first */
    QWidget* prevWidget();

    /** Return next widget or 0 if last */
    QWidget* nextWidget();

    void insertWidget(int index, QWidget* widget);

signals:
    void moveStarted( QLayoutItem* next, QLayoutItem* prev );
    void moveFinished( QLayoutItem* );
    
public slots:
    void revealWidget( QWidget* w );

private slots:
    void onFrameChanged( int frame );
    void onMoveFinished();


private:
    void doLayout( const QRect &rect, int vOffset = -1 );
    QList<QLayoutItem *> m_itemList;
    QLayoutItem* m_currentItem;
    QLayoutItem* m_previousItem;
    class QTimeLine* m_timeLine;

};

#endif //SLIDE_OVER_LAYOUT_H
