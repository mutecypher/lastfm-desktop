/*
   Copyright 2005-2009 Last.fm Ltd. 

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

#ifndef PLAYABLE_ITEM_WIDGET_H
#define PLAYABLE_ITEM_WIDGET_H

#include <QPushButton>
#include <lastfm/RadioStation>

class QMouseEvent;

class PlayableItemWidget : public QPushButton
{
    Q_OBJECT;

public:
    PlayableItemWidget( const RadioStation& rs, QString title, QString description = "" );

    const RadioStation& station() const { return m_rs; }

public slots:
    void onRadioChanged();

    void play();
    void playNext();
    void playMulti();
    void playMultiNext();

private:
    RadioStation getMultiStation() const;

    void paintEvent( QPaintEvent* event );
    void contextMenuEvent( class QContextMenuEvent* event );

private:
    RadioStation m_rs;
    QString m_description;
};

#endif
