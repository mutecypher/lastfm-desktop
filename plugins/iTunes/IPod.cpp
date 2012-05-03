/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole, Erik Jaelevik, 
        Christian Muehlhaeuser

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
#include "IPod.h"
#include <sys/stat.h>
#include <iostream>
#include <sstream>

#ifndef WIN32
    #include <dirent.h>
#endif

#ifndef WIN32
std::string escape(std::string const &s)
#else
std::wstring escape(std::wstring const &s)
#endif
{
    std::size_t n = s.length();
#ifndef WIN32
    std::string escaped;
#else
	std::wstring escaped;
#endif
    escaped.reserve(n * 2);        // pessimistic preallocation
    
    for (std::size_t i = 0; i < n; ++i) {
        if (s[i] == '\\' || s[i] == '\'')
            escaped += '\\';
        escaped += s[i];
    }
    return escaped;
}

const COMMON_STD_STRING 
IPod::twiddlyFlags() const
{
    LFM_STRINGSTREAM ss;
    
    ss << "--device ";
    ss << device();
    
    ss << " --connection ";
    switch( m_connectionType )
    {
        case usb:
            ss << "usb";
            break;
            
        case fireWire:
            ss << "fireWire";
            break;
    }
    
    ss << " --pid " << m_pid;
    ss << " --vid " << m_vid;
    ss << " --serial " << m_serial;
    ss << " --name " << escape( m_displayName );
    
    if( m_manualMode )
        ss << " --manual";

    return ss.str();
}


COMMON_STD_STRING
IPod::device() const
{
    #ifdef WIN32
        #define _(x) L##x
    #else
        #define _(x) x
    #endif

    switch( m_type )
    {
        case iPod:   return _( "ipod" );
        case iTouch: return _( "itouch" );
        case iPhone: return _( "iphone" );
        case iPad:   return _( "ipad" );            
        default:     return _( "unknown" );
    }

    #undef _
}
