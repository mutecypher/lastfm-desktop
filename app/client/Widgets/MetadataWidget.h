/*
   Copyright 2005-2009 Last.fm Ltd.
      - Primarily authored by Jono Cole and Doug Mansell

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

#ifndef METADATAWIDGET_H
#define METADATAWIDGET_H

#include <QWidget>

#include <lastfm/Album>
#include <lastfm/Track>

#include "lib/unicorn/StylableWidget.h"
#include "lib/unicorn/widgets/HttpImageWidget.h"

namespace Ui { class MetadataWidget; }

class DataListWidget;
class HttpImageWidget;
class QLabel;
class QGroupBox;
class BioWidget;


class MetadataWidget : public StylableWidget
{
    Q_OBJECT
public:
    MetadataWidget( const Track& track, QWidget* p = 0 );
    ~MetadataWidget();

    class ScrobbleControls* scrobbleControls() const;

    QWidget* basicInfoWidget();
    void setBackButtonVisible( bool );

    static QString getContextString( const Track& track );

private slots:
    void onTrackGotInfo(const QByteArray& data);
    void onAlbumGotInfo();
    void onArtistGotInfo();
    void onArtistGotEvents();

    void onTrackGotYourTags();
    void onArtistGotYourTags();

    void onTrackCorrected( QString correction );
    void listItemClicked( const QModelIndex& );

    void onScrobblesCached( const QList<lastfm::Track>& tracks );
    void onScrobbleStatusChanged();

signals:
    void lovedStateChanged(bool loved);
    void backClicked();

private:
    void showIfRoom( const QLayout* layout );
    void resizeEvent( QResizeEvent* event );

    void checkFinished();

    void setTrackDetails( const Track& track );

    QString contextString( const Track& track );
    QString scrobbleString( const Track& track );

private:
    Ui::MetadataWidget *ui;

    Track m_track;

    int m_globalTrackScrobbles;
    int m_userTrackScrobbles;
    int m_userArtistScrobbles;

    Album m_albumGuess;
};

#endif // METADATAWIDGET_H
