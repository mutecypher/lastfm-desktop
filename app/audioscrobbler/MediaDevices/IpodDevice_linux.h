/*
   Copyright 2005-2010 Last.fm Ltd.
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
#ifndef IPOD_DEVICE_H
#define IPOD_DEVICE_H

#include "MediaDevice_linux.h"

typedef struct _Itdb_iTunesDB Itdb_iTunesDB;
typedef struct _Itdb_Track Itdb_Track;
typedef struct _Itdb_Playlist Itdb_Playlist;

class IpodDevice: public MediaDevice
{
    Q_OBJECT
public:
    IpodDevice();
    ~IpodDevice();

    virtual QString deviceId() const { return m_deviceId; }
    virtual QString deviceName() const;
    virtual QString tableName() const { return m_deviceId; }
    virtual QList<Track> tracksToScrobble();

private:
    void commit( Itdb_Track* iTrack );
    void open();
    void setTrackInfo( Track& lstTrack, Itdb_Track* iTrack );
    uint previousPlayCount( Itdb_Track* iTrack ) const;
    QDateTime previousPlayTime( Itdb_Track* track ) const;

private:
    Itdb_iTunesDB* m_itdb;
    Itdb_Playlist* m_mpl;
    QString m_deviceId;
    QString m_ipodModel;
};

#endif // IPOD_DEVICE_H
