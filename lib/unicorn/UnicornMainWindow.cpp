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
#include "UnicornMainWindow.h"
#include "UnicornCoreApplication.h"
#include "dialogs/AboutDialog.h"
#include "dialogs/UpdateDialog.h"
#include "UnicornSettings.h"
#include <lastfm/User>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QMenuBar>
#include <QShortcut>
#include <QMouseEvent>


#define SETTINGS_POSITION_KEY "MainWindowPosition"


unicorn::MainWindow::MainWindow()
:QMainWindow()
{
    new QShortcut( QKeySequence(Qt::CTRL+Qt::Key_W), this, SLOT(close()) );
    new QShortcut( QKeySequence(Qt::ALT+Qt::SHIFT+Qt::Key_L), this, SLOT(openLog()) );
    connect( qApp, SIGNAL(gotUserInfo( const lastfm::UserDetails& )), SLOT(onGotUserInfo( const lastfm::UserDetails& )) );
    connect( qApp, SIGNAL(sessionChanged( unicorn::Session, unicorn::Session )), SLOT(onSessionChanged( unicorn::Session )));
    connect( qApp->desktop(), SIGNAL( resized(int)), SLOT( cleverlyPosition()));
}


unicorn::MainWindow::~MainWindow()
{
}


void
unicorn::MainWindow::finishUi()
{
    ui.account = menuBar()->addMenu( User().name() );
    ui.profile = ui.account->addAction( tr("Visit &Profile"), this, SLOT(visitProfile()) );
    ui.account->addSeparator();
    ui.account->addAction( tr("Log &Out"), qApp, SLOT(logout()) );
    QAction* quit = ui.account->addAction( tr("&Quit"), qApp, SLOT(quit()) );
    quit->setMenuRole( QAction::QuitRole );
#ifdef Q_OS_WIN
    quit->setShortcut( Qt::Alt + Qt::Key_F4 );
#else
    quit->setShortcut( Qt::CTRL + Qt::Key_Q );
#endif

    menuBar()->insertMenu( menuBar()->actions().first(), ui.account );
    QMenu* help = menuBar()->addMenu( tr("Help") );
    QAction* about = help->addAction( tr("About"), this, SLOT(about()) );
    QAction* c4u = help->addAction( tr("Check for Updates"), this, SLOT(checkForUpdates()) );

#ifndef NDEBUG
    QMenu* debug = menuBar()->addMenu( tr("Debug") );
    QAction* rss = debug->addAction( tr("Refresh Stylesheet"), qApp, SLOT(refreshStyleSheet()) );
    rss->setShortcut( Qt::CTRL + Qt::Key_R );
#endif

#ifdef __APPLE__
    about->setMenuRole( QAction::AboutRole );
    c4u->setMenuRole( QAction::ApplicationSpecificRole );
#endif

    ui.update = new UpdateDialog( this );


    cleverlyPosition();
}


void
unicorn::MainWindow::onGotUserInfo( const lastfm::UserDetails& details )
{
    ui.account->setTitle( details );
    QString const text = details.getInfoString();
    if (text.size() && ui.account) {
        QAction* a = ui.account->addAction( text );
        a->setEnabled( false );
        a->setObjectName( "UserBlurb" );
        ui.account->insertAction( ui.profile, a );
    }
}


void
unicorn::MainWindow::visitProfile()
{
    QDesktopServices::openUrl( User().www() );
}


void
unicorn::MainWindow::about()
{
    if (!ui.about) ui.about = new AboutDialog( this );
    ui.about.show();
}


void
unicorn::MainWindow::checkForUpdates()
{
    if (!ui.update) ui.update = new UpdateDialog( this );
    ui.update.show();
}


void
unicorn::MainWindow::openLog()
{
    QDesktopServices::openUrl( QUrl::fromLocalFile( unicorn::CoreApplication::log().absoluteFilePath() ) );    
}



bool 
unicorn::MainWindow::eventFilter( QObject* o, QEvent* event )
{
#ifdef SUPER_MEGA_DEBUG
	qDebug() << o << event;
#endif
	
    QWidget* obj = qobject_cast<QWidget*>( o );
    if (!obj)
        return false;
    
    QMouseEvent* e = static_cast<QMouseEvent*>( event );
    
    switch ((int)e->type())
    {
        case QEvent::MouseButtonPress:
            m_dragHandleMouseDownPos[ obj ] = e->globalPos() - pos();
            return false;

        case QEvent::MouseButtonRelease:
            m_dragHandleMouseDownPos[ obj ] = QPoint();
            return false;
            
        case QEvent::MouseMove:
            if (m_dragHandleMouseDownPos.contains( obj ) && !m_dragHandleMouseDownPos[ obj ].isNull()) {
                move( e->globalPos() - m_dragHandleMouseDownPos[ obj ] );
                return true;
            }
    }

    return QMainWindow::eventFilter(o, event);
}


void 
unicorn::MainWindow::addDragHandleWidget( QWidget* w )
{
    w->installEventFilter( this );
}


void 
unicorn::MainWindow::hideEvent( QHideEvent* )
{
    emit hidden( false );
}


void 
unicorn::MainWindow::showEvent( QShowEvent* )
{
    emit shown( true );
}


void
unicorn::MainWindow::onSessionChanged( const Session& session )
{
    ui.account->findChild<QAction*>("UserBlurb")->deleteLater();
    ui.account->setTitle( session.username());
}

void 
unicorn::MainWindow::storeGeometry() const
{
    AppSettings s;
    s.beginGroup( metaObject()->className());
        s.setValue( "geometry", frameGeometry());
    s.endGroup();
}


void 
unicorn::MainWindow::moveEvent( QMoveEvent* )
{
    storeGeometry();
}


void
unicorn::MainWindow::resizeEvent( QResizeEvent* )
{
    storeGeometry();
}


void
unicorn::MainWindow::cleverlyPosition()
{
    AppSettings s;
    s.beginGroup( metaObject()->className());
        QRect geo = s.value( "geometry", QRect()).toRect();
    s.endGroup();

    if( !geo.isValid())
        return;

    move( geo.topLeft());
    resize( geo.size());
    
    int screenNum = qApp->desktop()->screenNumber( this );
    QRect screenRect = qApp->desktop()->availableGeometry( screenNum );
    if( !screenRect.contains( frameGeometry(), true)) {
        QRect diff;

        diff = screenRect.intersected( frameGeometry() );

        if (diff.left() == screenRect.left() )
            move( diff.left(), pos().y());
        if( diff.right() == screenRect.right())
            move( diff.right() - width(), pos().y());
        if( diff.top() == screenRect.top())
            move( pos().x(), diff.top());
        if( diff.bottom() == screenRect.bottom())
            move( pos().x(), diff.bottom() - height());
    }
}
