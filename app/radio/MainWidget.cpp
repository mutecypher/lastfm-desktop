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
#include "MainWidget.h"
#include <lastfm.h>
#include <QtGui>
#include <stdarg.h>
#include "lib/unicorn/layouts/SideBySideLayout.h"
#include "widgets/MainStarterWidget.h"
#include "widgets/NowPlayingWidget.h"
#include "widgets/MultiStarterWidget.h"
#include "widgets/RadioListWidget.h"
#include "Radio.h"
#include "../PlaylistModel.h"


MainWidget::MainWidget( QWidget* parent )
           :StylableWidget( parent )
{
    m_nowPlaying = new NowPlayingState();
    connect(radio, SIGNAL(tuningIn(RadioStation)), m_nowPlaying, SLOT(onTuningIn(RadioStation)));
    connect(radio, SIGNAL(stopped()), m_nowPlaying, SLOT(onStopped()));

    m_layout = new SideBySideLayout( this );
    connect( m_layout, SIGNAL(moveStarted(QLayoutItem*, QLayoutItem*)), SLOT(onSlideStarted(QLayoutItem*, QLayoutItem*)));
    MainStarterWidget* w = new MainStarterWidget;
    w->setRecentStationsModel(&m_recentModel);
    connect(w, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));
    connect(w, SIGNAL(startRadio(RadioStation)), SLOT(onStartRadio(RadioStation)));
    connect(w, SIGNAL(showMoreRecentStations()), SLOT(onShowMoreRecentStations()));
    connect(w, SIGNAL(combo()), SLOT(onCombo()));
    connect(w, SIGNAL(yourFriends()), SLOT(onYourFriends()));

    BackForwardControls* ctrl = new BackForwardControls(QString(), QString(), m_nowPlaying, w);
    connect( this, SIGNAL( sessionChanged( unicorn::Session* ) ),
             ctrl, SLOT( onSessionChanged( unicorn::Session* ) ) );
    connect(ctrl, SIGNAL(forward()), SLOT(onForward()));
    m_layout->addWidget(ctrl);
    m_layout->setContentsMargins( 0, 0, 0, 0);
}


void 
MainWidget::onSlideStarted( QLayoutItem* next, QLayoutItem* prev )
{
    if( next->widget() )
        emit widgetChanged( next->widget());
}


void 
MainWidget::onStartRadio(RadioStation rs)
{
    if( radio->state() != Radio::Stopped ) {
        foreach( NowPlayingWidget* npw, findChildren<NowPlayingWidget*>())
        {
            m_layout->removeWidget( npw->parentWidget() );
            npw->parentWidget()->deleteLater();
        }
    }
    //Events must be processed in order to prepare the layout for a slide animation
    qApp->processEvents();

    NowPlayingWidget* w = new NowPlayingWidget;
    connect(radio, SIGNAL(tuningIn( RadioStation )), w, SLOT(onTuningIn( RadioStation )));
    connect(radio, SIGNAL(trackSpooled( Track )), w, SLOT(onTrackSpooled( Track )));
    connect(radio, SIGNAL(tick( qint64 )), w, SIGNAL( tick( qint64 )));
    connect(radio, SIGNAL(buffering( int )), w, SLOT(onBuffering( int )));
//    connect(radio, SIGNAL(stopped()), w, SLOT(onStopped()));

    BackForwardControls* ctrl = new BackForwardControls(tr("Back"), rs.title(), NULL, w);
    connect( this, SIGNAL( sessionChanged( unicorn::Session* ) ),
             ctrl, SLOT( onSessionChanged( unicorn::Session* ) ) );
    connect(ctrl, SIGNAL(back()), SLOT(onBack()));
    connect(radio, SIGNAL(stopped()), SLOT(onBackDelete()));
    m_layout->addWidget(ctrl);
    m_layout->moveForward();
}

void
MainWidget::onShowMoreRecentStations()
{
    RadioListWidget* w = new RadioListWidget( &m_recentModel );
    connect(w, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));
    connect(w, SIGNAL(startRadio(RadioStation)), SLOT(onStartRadio(RadioStation)));

    BackForwardControls* ctrl = new BackForwardControls(tr("Back"), "Recent Stations", NULL, w);
    connect( this, SIGNAL( sessionChanged( unicorn::Session* ) ),
             ctrl, SLOT( onSessionChanged( unicorn::Session* ) ) );
    connect(ctrl, SIGNAL(back()), SLOT(onBackDelete()));
    m_layout->addWidget(ctrl);
    m_layout->moveForward();
}

void
MainWidget::onCombo()
{
    MultiStarterWidget* w = new MultiStarterWidget(3, this);
    connect(w, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));
    connect(w, SIGNAL(startRadio(RadioStation)), SLOT(onStartRadio(RadioStation)));

    BackForwardControls* ctrl = new BackForwardControls(tr("Back"), "Start a combo station", m_nowPlaying, w);
    connect( this, SIGNAL( sessionChanged( unicorn::Session* ) ),
             ctrl, SLOT( onSessionChanged( unicorn::Session* ) ) );
    connect(ctrl, SIGNAL(back()), SLOT(onBackDelete()));
    connect(ctrl, SIGNAL(forward()), SLOT(onForward()));
    m_layout->insertWidget(1, ctrl);
    m_layout->moveForward();
}

// user clicks "Your Friends" button
void
MainWidget::onYourFriends()
{
    QListView* lv = new QListView();
    lv->setModel(new QStringListModel(m_friends));
    lv->setEditTriggers(QAbstractItemView::NoEditTriggers);

    BackForwardControls* ctrl = new BackForwardControls(tr("Back"), "Your Friends", m_nowPlaying, lv);
    connect( this, SIGNAL( sessionChanged( unicorn::Session* ) ),
             ctrl, SLOT( onSessionChanged( unicorn::Session* ) ) );
    connect(ctrl, SIGNAL(back()), SLOT(onBackDelete()));
    connect(ctrl, SIGNAL(forward()), SLOT(onForward()));
    connect(lv, SIGNAL(activated(QModelIndex)), SLOT(onFriendActivated(QModelIndex)));
    m_layout->insertWidget(1, ctrl);
    m_layout->moveForward();
}

// user clicks a friend from the Your Friends list:
// start friend's library radio
void
MainWidget::onFriendActivated(const QModelIndex& idx)
{
    QString f = idx.data().toString();
    RadioStation rs = RadioStation::library(User(f));
    rs.setTitle(f + "'s Library");
    emit startRadio(rs);
    onStartRadio(rs);
}

void
MainWidget::onBack()
{
    m_layout->moveBackward();
}

void
MainWidget::onBackDelete()
{
    connect(m_layout, SIGNAL(moveFinished(QLayoutItem*)), SLOT(onMoveFinished()));
    m_layout->moveBackward();
}

void
MainWidget::onMoveFinished()
{
    disconnect(m_layout, SIGNAL(moveFinished(QLayoutItem*)), this, SLOT(onMoveFinished()));
    QWidget* w = m_layout->nextWidget();
    if (w) {
        m_layout->removeWidget(w);
        w->deleteLater();
    }
}

void
MainWidget::onForward()
{
    m_layout->moveForward();
}

void
MainWidget::onUserGotFriends()
{
    sender()->deleteLater();
    QNetworkReply* r = (QNetworkReply*)sender();
    XmlQuery lfm = r->readAll();
    m_friends.clear();
    foreach (const XmlQuery& e, lfm["friends"].children("user")) {
        m_friends << e["name"].text();
    }
}

void
MainWidget::onUserGotRecentStations()
{
    sender()->deleteLater();
    m_recentModel.setList(
        RadioStation::list(
            qobject_cast<QNetworkReply*>(sender())));
}

void
MainWidget::rawrql()
{
    bool ok;
    QString rql = QInputDialog::getItem(this, 
        "Direct RQL tuning",
        "RQL", 
        m_rawrqlItems,
        0,
        true, //editable
        &ok);
    if (ok && !rql.isEmpty()) {
        m_rawrqlItems.prepend(rql);
        RadioStation rs = RadioStation::rql(rql);
        onStartRadio(rs);
        emit startRadio(rs);
    }
}

