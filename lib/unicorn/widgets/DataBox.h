/*
   Copyright 2010 Last.fm Ltd.
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

#include <QHBoxLayout>
#include <QLabel>

#include "lib/unicorn/StylableWidget.h"
#include "lib/DllExportMacro.h"

class UNICORN_DLLEXPORT DataBox : public StylableWidget {
    Q_OBJECT
public:
    DataBox( const QString& title, QWidget* child, QWidget* p = 0 )
        :StylableWidget( p )
    { 
        StylableWidget* w = new StylableWidget();
        w->setObjectName( "header" );
        new QHBoxLayout( w );
        w->layout()->setContentsMargins( 0, 0, 0, 0 );
        w->layout()->setSpacing( 0 );
        QLabel* icon;
        w->layout()->addWidget( icon = new QLabel());
        icon->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
        icon->setObjectName( "icon" );
        w->layout()->addWidget( new QLabel( title ));

        new QVBoxLayout( this );
        layout()->addWidget( w );
        QWidget* cw = new StylableWidget();
        cw->setObjectName( "contents" );
        QVBoxLayout* v = new QVBoxLayout( cw );
        v->setContentsMargins( 0, 0, 0, 0 );
        v->setSpacing( 0 );
        v->addWidget( child, Qt::AlignTop );
        v->addStretch();
        layout()->addWidget( cw );
        layout()->setContentsMargins( 0, 0, 0, 0 );
        layout()->setSpacing( 0 );
    }
};
