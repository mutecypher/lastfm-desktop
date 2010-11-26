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

#ifndef TRACK_WIDGET_H_
#define TRACK_WIDGET_H_

#include <QWidget>
#include <QMovie>
#include <QPointer>

#include <lastfm/Track>

#include "lib/unicorn/StylableWidget.h"

class ScrobbleInfoFetcher;
class ScrobbleInfoWidget;
class StopWatch;

class TrackWidget : public StylableWidget
{
    Q_OBJECT
public:
    TrackWidget();
    TrackWidget( const Track& track );

    void setFromIPodScrobble( const QList<lastfm::Track>& tracks );
    void setText( const QString& text );
    QString text() const;

    void setTrack( const Track& track );

    Track track() const { return m_track;}

    QString status() const { return m_status; }

    bool odd() const { return m_odd; }
    void setOdd( bool odd ) { m_odd = odd; }

    ScrobbleInfoFetcher* fetcher() const;
    ScrobbleInfoWidget* infoWidget() const;

signals:
    void cogMenuAboutToShow();
    void cogMenuAboutToHide();

    void clicked( TrackWidget* );

private:
    void mousePressEvent ( QMouseEvent * event );
    void enterEvent( class QEvent* event );
    void leaveEvent( class QEvent* event );
    void resizeEvent( class QResizeEvent* event );

    void paintEvent( class QPaintEvent* event );

    void setStatus( QString status ) { m_status = status; }

    void setupUI();
    void setStatusToCurrentTrack();

    void doSetTrack( const Track& track );
    void connectTrack();

public slots:
    void updateTimestamp();

private slots:
    void onLoveToggled( bool loved );

    void onLoveClicked();
    void onTagClicked();
    void onShareClicked();

    void onWatchPaused( bool isPaused );
    void onWatchFinished();

    void onScrobbleStatusChanged();

    void onCorrected( QString correction );

private:
    struct
    {
        class QLabel* as;
        class QWidget* trackTextArea;
        class QLabel* trackText;
        class QLabel* correction;
        class QLabel* love;
        class QToolButton* cog;
        class GhostWidget* ghostCog;
        class QLabel* timestamp;
    } ui;

    struct {
        class QMovie* scrobbler_as;
        class QMovie* scrobbler_paused;
    } movie;

    Track m_track;
    class QTimer* m_timestampTimer;

    QPointer<StopWatch> m_stopWatch;
    QPointer<ScrobbleInfoFetcher> m_fetcher;
    QPointer<ScrobbleInfoWidget> m_infoWidget;

    QAction* m_loveAction;

    Q_PROPERTY(QString status READ status WRITE setStatus);
    QString m_status;

    Q_PROPERTY(bool odd READ odd WRITE setOdd);
    bool m_odd;
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

#endif // TRACK_WIDGET_H_

