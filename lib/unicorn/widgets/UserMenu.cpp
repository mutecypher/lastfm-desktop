
#include <QDebug>
#include <QTimer>
#include <QDesktopServices>

#include <lastfm/User.h>
#include <lastfm/UrlBuilder.h>

#include "lib/unicorn/UnicornApplication.h"
#include "lib/unicorn/UnicornSettings.h"

#include "UserMenu.h"

UserMenu::UserMenu( QWidget* p )
    :QMenu( p )
{
    connect( qApp, SIGNAL(rosterUpdated()), SLOT(refresh()));
    connect( qApp, SIGNAL(sessionChanged( unicorn::Session* ) ), SLOT( onSessionChanged( unicorn::Session* ) ) );

    refresh();
}


void
UserMenu::onSessionChanged( unicorn::Session* s )
{
    if ( !s )
        return;

    foreach( QAction* a, actions() )
    {
        if( a->text() == s->userInfo().name() )
            return a->setChecked( true );
    }
}

void
UserMenu::onTriggered( QAction* a )
{
    unicorn::Settings s;

    s.beginGroup( a->text() );
    QString username = a->text();
    QString sessionKey = s.value( "SessionKey", "" ).toString();
    QMetaObject::invokeMethod( qApp, "changeSession",
                                     Q_ARG( const QString, username ),
                                     Q_ARG( const QString, sessionKey ) );

    //Refresh the user list to be certain that
    //the correct current user is checked.
    //(ie. the user change could be cancelled after confirmation.)
    refresh();

}

void
UserMenu::manageUsers()
{
    //This is required so that the menu popup call can be completed
    //in the event loop before the manage users dialog is displayed.
    //(The manageUsers method may emit a signal which causes the menu
    // to be destroyed)
    QTimer::singleShot( 0, qApp, SLOT(manageUsers()) );
}

void
UserMenu::subscribe()
{
    QDesktopServices::openUrl( lastfm::UrlBuilder( "join" ).url() );
}

void
UserMenu::refresh()
{
    clear();

    addAction( tr( "Manage Accounts" ), this, SLOT(manageUsers()));
    addAction( tr( "Subscribe" ), this, SLOT(subscribe()));

    addSeparator();

    QActionGroup* ag = new QActionGroup( this );

    foreach( User u, unicorn::Settings().userRoster() )
    {
        QAction* a = ag->addAction( new QAction( u.name(), this ));
        addAction( a );
        if( u == User()) a->setChecked( true );
        a->setCheckable( true );
    }

    ag->setExclusive( true );
    connect( ag, SIGNAL(triggered(QAction*)), SLOT( onTriggered(QAction*)));

    onSessionChanged( qobject_cast<unicorn::Application*>(qApp)->currentSession());
}

