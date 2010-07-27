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
#ifndef MEDIA_DEVICE_H
#define MEDIA_DEVICE_H

#include <QObject>
#include <QSqlDatabase>
#include <lastfm/Track>

class MediaDevice: public QObject
{
    Q_OBJECT

public:
    MediaDevice();
    ~MediaDevice();

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

    /**
     * Associates the device to the audioscrobbler user account.
     * @return true if it succeeds, false otherwise.
     */
    bool associateDevice();

    /**
     * @return the last error ocurred or empty string if there wasn't any.
     */
    QString lastError() const { return m_error; }

    /**
     * @return an unique ID for the device.
     */
    virtual QString deviceId() const = 0;

    /**
     * @return the device name.
     */
    virtual QString deviceName() const = 0;

    /**
     * @return an unique table name.
     */
    virtual QString tableName() const = 0;

    /**
     * @return a list of tracks to be scrobbled.
     */
    virtual QList<Track> tracksToScrobble() = 0;

    /**
     * @return true if the device is already associated with the user account, false otherwise.
     */
    bool isDeviceKnown() const;


protected:
    QString m_error;

private:
    struct DeviceInfo
    {
        QString mountPath;
        QString prettyName;
    };

private:
    QMap<QString, DeviceInfo> m_detectedDevices;
    QString m_mountPath;
};


#endif // MEDIA_DEVICE_H
