/*
   Copyright 2005-2009 Last.fm Ltd. 

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

#include <QComboBox>
#include "lib/unicorn/widgets/HelpTextLineEdit.h"
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include "QuickStartWidget.h"
#include "../StationSearch.h"

#include <QStylePainter>
class AAQPushButton : public QPushButton
{
public:
    AAQPushButton( const QString& caption )
        :QPushButton( caption )
    {}

    virtual void paintEvent( QPaintEvent* e )
    {
        QStylePainter p( this );
        QStyleOptionButton option;
        initStyleOption( &option );

        p.setRenderHint( QPainter::HighQualityAntialiasing );
        p.drawControl( QStyle::CE_PushButton, option );
    }
};

QuickStartWidget::QuickStartWidget()
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QHBoxLayout* h1 = new QHBoxLayout(this);
    QPushButton* button = new AAQPushButton(tr("Play"));
    h1->addWidget(m_edit = new HelpTextLineEdit( tr("Type an artist or tag"), this ) );
    h1->addWidget(button);

    layout->addWidget( new QLabel( tr("Quick start") ), 0, Qt::AlignCenter );
    layout->addLayout( h1 );

    m_edit->setAttribute( Qt::WA_MacShowFocusRect, false );
    
    connect(m_edit, SIGNAL(returnPressed()), SLOT(search()));
    connect(button, SIGNAL(clicked()), SLOT(search()));
}

void
QuickStartWidget::search()
{
    QString trimmedText = m_edit->text().trimmed();
    if( trimmedText.startsWith("lastfm://")) {
        emit startRadio( RadioStation( trimmedText ) );
        return;
    }

    if (m_edit->text().length()) {
        StationSearch* s = new StationSearch();
        connect(s, SIGNAL(searchResult(RadioStation)), SIGNAL(startRadio(RadioStation)));
        s->startSearch(m_edit->text());
    }
}
