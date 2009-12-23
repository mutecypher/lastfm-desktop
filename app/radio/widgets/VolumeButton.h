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

#ifndef VOLUME_BUTTON_H
#define VOLUME_BUTTON_H

#include <QPushButton>

class VolumeSlider;
class QDockWidget;
class QTimer;

class VolumeButton : public QPushButton
{
    Q_OBJECT
public:
    VolumeButton(const QString& text, QWidget* parent = 0);

private:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

    QPoint widgetPosition(const QWidget& dock, const QWidget& child);

private slots:
    void onToggled(bool checked);
    void onTimeout();

    void onSliderEnter(QEvent* event);
    void onSliderLeave(QEvent* event);

private:
    VolumeSlider* volumeSlider;

    QTimer* timer;
};

#endif // VOLUME_BUTTON_H
