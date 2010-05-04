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

#ifndef STYLABLE_WIDGET_H_
#define STYLABLE_WIDGET_H_
#include <QFrame>
#include <QStyleOptionFrameV3>
#include <QPainter>
#include "lib/DllExportMacro.h"

class UNICORN_DLLEXPORT StylableWidget: public QFrame{
Q_OBJECT
public:
    StylableWidget( QWidget* parent = 0, Qt::WindowFlags f = 0 ): QFrame( parent, f ), hasInit( false ){ setFrameStyle( QFrame::NoFrame ); setAutoFillBackground( true ); }

protected:
    void paintEvent(QPaintEvent*)
    {
        QStyleOptionFrameV3 opt;
        opt.init(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Frame, &opt, &p, this);
    }
    
    void showEvent(QShowEvent* e )
    {
        if( hasInit )
            return QFrame::showEvent( e );
        
        hasInit = true;
    }

private:
    bool hasInit;
};

#endif
