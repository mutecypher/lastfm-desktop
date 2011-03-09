/*
   Copyright 2005-2011 Last.fm Ltd.
      - Primarily authored by Jono Cole and Michael Coffey

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

#ifndef AUDIOSCROBBLER_SETTINGS_H
#define AUDIOSCROBBLER_SETTINGS_H

#include "lib/unicorn/UnicornSettings.h"

class AudioscrobblerSettings : public unicorn::AppSettings
{
public:
    AudioscrobblerSettings();
    Qt::KeyboardModifiers raiseShortcutModifiers() const { return (Qt::KeyboardModifiers)value( "raiseShortcutModifiers", (int)(Qt::ControlModifier | Qt::MetaModifier) ).toInt(); }
    int raiseShortcutKey() const {
#ifdef Q_WS_MAC
        const int sKeyCode = 1;
#elif defined Q_WS_WIN
        const int sKeyCode = 83;
#endif
        return value( "raiseShortcutKey", sKeyCode ).toInt();
    }
    QString raiseShortcutDescription() const { return value( "raiseShortcutDescription", QString::fromUtf8( "⌃⌘ S" ) ).toString(); }

    void setRaiseShortcutKey( int key ) { setValue( "raiseShortcutKey", key ); }
    void setRaiseShortcutModifiers( Qt::KeyboardModifiers m ) { setValue( "raiseShortcutModifiers", (int)m ); }
    void setRaiseShortcutDescription( QString d ) { setValue( "raiseShortcutDescription", d ); }
};

#endif // AUDIOSCROBBLER_SETTINGS_H
