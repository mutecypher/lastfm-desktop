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

#include "RadioListWidget.h"
#include "RadioStationListModel.h"
#include <QAbstractListModel>
#include "Services/RadioService.h"
#include "../Application.h"
#include <lastfm/RadioStation>


RadioListWidget::RadioListWidget(QWidget* parent)
: QTreeView(parent)
{
    setModel( m_model = new RadioStationListModel());
    setHeaderHidden( true );
    setRootIsDecorated( false );
    setAttribute( Qt::WA_MacShowFocusRect, false );
    setAlternatingRowColors( true );

    connect( this, SIGNAL( clicked( QModelIndex )), SLOT( onStationClicked( QModelIndex )));
    connect( qApp, SIGNAL( sessionChanged(unicorn::Session* )), SLOT( onSessionChanged(unicorn::Session* )));
}

void 
RadioListWidget::onSessionChanged( unicorn::Session* session ) {
    QList<RadioStation> stationList;
    stationList << RadioStation::library( lastfm::ws::Username )
                << RadioStation::mix( lastfm::ws::Username)
                << RadioStation::recommendations( lastfm::ws::Username)
                << RadioStation::friends( lastfm::ws::Username)
                << RadioStation::neighbourhood( lastfm::ws::Username);
    
    m_model->setList( stationList );
}

void 
RadioListWidget::onStationClicked( const QModelIndex& index ) {
    RadioService::instance().play( RadioStation( model()->data( index, RadioStationListModel::UrlRole ).toString()));
}
