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

#include "MediaDevice.h"

typedef struct _Itdb_iTunesDB Itdb_iTunesDB;
typedef struct _Itdb_Track Itdb_Track;
typedef struct _Itdb_Playlist Itdb_Playlist;

class IpodDevice: public MediaDevice
{
    Q_OBJECT
public:
    IpodDevice();
    ~IpodDevice();

    /**
     * Deletes the table with the device scrobbled tracks information.
     * @return true on success.
     */
    static bool deleteDeviceHistory( QString username, QString deviceId );

    /**
     * Delete the database containing all the devices scrobbled tracks tables.
     * @return true on success.
     */
    static bool deleteDevicesHistory();

    /**
     * @return the database to sync tracks to be scrobbled.
     */
    QSqlDatabase database() const;


    /**
     * Try to detect if there is any of the user associated devices already mounted and use it.
     * If more than one device is detected then nothing would be done.
     * @return true if there was just one of the user's devices mounted, otherwise returns false.
     */
    bool autoDetectMountPath();

    /**
     * Sets the mount path where the device is mounted.
     * @param path The mount path of the mounted device.
     */
    void setMountPath( const QString& path ){ m_mountPath = path; }

    /**
     * @return The mount path of the device.
     */
    QString mountPath() const { return m_mountPath; }

    virtual QString deviceId() const { return m_deviceId; }
    virtual QString deviceName() const;

    /**
     * @return an unique table name to store the device scrobbled tracks.
     */
    QString tableName() const;

    /**
     * @return a list of tracks to be scrobbled.
     */
    QList<Track> tracksToScrobble();


private:
    void commit( Itdb_Track* iTrack );
    void open();
    void setTrackInfo( Track& lstTrack, Itdb_Track* iTrack );
    uint previousPlayCount( Itdb_Track* iTrack ) const;
    QDateTime previousPlayTime( Itdb_Track* track ) const;

private:
    struct DeviceInfo
    {
        QString mountPath;
        QString prettyName;
    };

private:
    Itdb_iTunesDB* m_itdb;
    Itdb_Playlist* m_mpl;
    QString m_deviceId;
    QMap<QString, DeviceInfo> m_detectedDevices;
    QString m_mountPath;
    QString m_ipodModel;
};

#endif // IPOD_DEVICE_H
