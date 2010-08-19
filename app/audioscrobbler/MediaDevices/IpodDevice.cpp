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
#include "IpodDevice.h"
#include "lib/unicorn/QMessageBoxBuilder.h"
#include "lib/unicorn/UnicornSettings.h"
#include "lib/unicorn/UnicornSession.h"

#include <QApplication>
#include <QByteArray>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>


IpodDevice::IpodDevice( const QString& deviceId, const QString& deviceName )
    : m_deviceId( deviceId ), m_deviceName( deviceName )
{}

QString
IpodDevice::deviceId() const
{
    return m_deviceId;
}

QString
IpodDevice::deviceName() const
{
    return m_deviceName;
}
