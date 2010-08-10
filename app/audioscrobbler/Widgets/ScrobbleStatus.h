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

#ifndef SCROBBLE_STATUS_H_
#define SCROBBLE_STATUS_H_

#include "lib/unicorn/StylableWidget.h"
#include <lastfm/Track>
#include <QPointer>

class StopWatch;
class ScrobbleStatus : public StylableWidget 
{
Q_OBJECT
public:
    ScrobbleStatus( QWidget* parent = 0 );

signals:
    void clicked();

protected:
    virtual void paintEvent( class QPaintEvent* );
    virtual void mousePressEvent( class QMouseEvent* );
    virtual void mouseReleaseEvent( class QMouseEvent* );

private:
    void setStatusToCurrentTrack();

    struct {
        class QLabel* as;
        class QLabel* title;
        class QLabel* playerStatus;
    } ui;

    struct {
        class QMovie* scrobbler_as;
        class QMovie* scrobbler_paused;
    } movie;

    QPointer<StopWatch> m_stopWatch;
    bool m_mouseDown;

    Track m_currentTrack;

public slots:
    void onTrackStarted( const Track&, const Track& );
    void onTrackStopped();

private slots:
    void onWatchPaused( bool );
    void onWatchFinished();
};

#include <QMovie>

class Movie : public QMovie
{
Q_OBJECT
public:
    Movie( const QString& filename, const QByteArray& format = QByteArray(), QObject* parent = 0 )
    :QMovie( filename, format, parent )
    {
        connect( this, SIGNAL( frameChanged(int)), SLOT(onFrameChanged(int)));
    }

signals:
    void loopFinished();

private slots:
    void onFrameChanged( int f )
    {
        if( f == (frameCount() - 1)) {
            emit loopFinished();
        }
    }
};
#endif //SCROBBLE_STATUS_H_
