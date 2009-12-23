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

#include <QLayout>
#include <QDockWidget>
#include <QTimer>
#include <QEvent>
#include <QSlider>

#include "VolumeButton.h"
#include "VolumeSlider.h"
#include "../Radio.h"

VolumeButton::VolumeButton(const QString& text, QWidget* parent)
    :QPushButton(text, parent), volumeSlider(0)
{
    setMouseTracking(true);

    setCheckable(true);
    setChecked(false);

    volumeSlider = new VolumeSlider(this);
    volumeSlider->setObjectName("volumeSlider");
    volumeSlider->setWindowFlags( Qt::Dialog | Qt::FramelessWindowHint );
    volumeSlider->setOrientation( Qt::Vertical );
    volumeSlider->setWindowModality( Qt::NonModal );
    volumeSlider->setFixedWidth( 30 );
    volumeSlider->setContentsMargins( 0, 4, 0, 4 );
    volumeSlider->layout()->setSpacing( 5 );
    volumeSlider->resize(volumeSlider->width(), 120);
    volumeSlider->setMuteVisible(false);
    volumeSlider->setMouseTracking(true);
    volumeSlider->hide();

    connect(this, SIGNAL(toggled(bool)), SLOT(onToggled(bool)));

    connect(volumeSlider, SIGNAL(enter(QEvent*)), SLOT(onSliderEnter(QEvent*)));
    connect(volumeSlider, SIGNAL(leave(QEvent*)), SLOT(onSliderLeave(QEvent*)));

    timer = new QTimer(this);
    timer->setSingleShot(true);

    connect(timer, SIGNAL(timeout()), SLOT(onTimeout()));
}

void
VolumeButton::onToggled(bool checked)
{
    radio->audioOutput()->setMuted(checked);
}

void
VolumeButton::onTimeout()
{
    volumeSlider->hide();
}

QPoint
VolumeButton::widgetPosition(const QWidget& dock, const QWidget& child)
{
    return mapToGlobal(dock.pos()) - QPoint(0, child.height() + (dock.height() / 2) );
}

void
VolumeButton::enterEvent(QEvent *event)
{
    timer->stop();

    if (volumeSlider->isHidden())
    {
        volumeSlider->setAudioOutput(radio->audioOutput());
        volumeSlider->move(widgetPosition(*this, *volumeSlider));
        volumeSlider->raise();
        volumeSlider->show();
    }
}

void
VolumeButton::leaveEvent(QEvent *event)
{
    timer->start(1000);
}

void VolumeButton::onSliderEnter(QEvent* event)
{
    enterEvent(event);
}

void VolumeButton::onSliderLeave(QEvent* event)
{
    leaveEvent(event);
}
