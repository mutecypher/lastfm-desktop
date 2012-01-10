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
#include "AboutDialog.h"
#include <QApplication>
#include <QIcon>
#include <QLabel>
#include <QVBoxLayout>


static inline QLabel* label( const QString& text, Qt::WidgetAttribute size = Qt::WA_MacSmallSize )
{
    QLabel* l = new QLabel( text );
    l->setAttribute( size );
    l->setOpenExternalLinks( true );
    return l;
}


AboutDialog::AboutDialog( QWidget* parent )
           : QDialog( parent )
{
    Q_ASSERT( qApp->applicationVersion().size() );

    QVBoxLayout* v = new QVBoxLayout( this );
    v->addWidget( new QLabel( "<b>" + qApp->applicationName() ) );
    v->addWidget( label( tr( "%1 (built on Qt %2)"  ).arg( qApp->applicationVersion(), qVersion() ) ) );
    v->addSpacing( 10 );
    v->addWidget( label( "<a href='http://www.last.fm'>www.last.fm</a>" ) );
    v->addWidget( label( "<a href='irc://irc.audioscrobbler.com#audioscrobbler'>irc.audioscrobbler.com</a>" ) );
    v->addSpacing( 10 );
    v->addWidget( label( QString::fromUtf8("Copyright © 2005-2012 Last.fm Ltd.") ) );

    v->setSizeConstraint( QLayout::SetFixedSize );
    v->setSpacing( 2 );

#ifdef Q_WS_MAC
    foreach (QLabel* l, findChildren<QLabel*>())
        l->setAlignment( Qt::AlignHCenter );
#else
    // yeah, really, don't do it on Mac. Weird.
    setWindowTitle( tr("About") );
#endif
}
