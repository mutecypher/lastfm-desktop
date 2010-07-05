#ifndef USER_MENU_H_
#define USER_MENU_H_

#include <QMenu>
#include <lastfm/User>
#include "lib/DllExportMacro.h"
#include "lib/unicorn/UnicornApplication.h"
#include "lib/unicorn/UnicornSettings.h"
#include "lib/unicorn/UnicornSession.h"
#include <QDebug>
#include <QTimer>

class UNICORN_DLLEXPORT UserMenu : public QMenu
{
    Q_OBJECT
public:
    UserMenu( QWidget* p = 0 )
    :QMenu( p )
    {
        connect( qApp, SIGNAL( rosterUpdated()), SLOT( refresh()));
        connect( qApp, SIGNAL( sessionChanged( unicorn::Session, unicorn::Session ))
                     , SLOT( onSessionChanged( unicorn::Session )));
        refresh();
    }

protected slots:
    void onSessionChanged( const unicorn::Session& s )
    { 
        foreach( QAction* a, actions() ) {
            if( a->text() == s.username())
                return a->setChecked( true );
        }
    }

    void onTriggered( QAction* a )
    {
        unicorn::Session s( a->text() );
        if( !s.isValid()) {
            //TODO Error handling
            return;
        }
        
        QMetaObject::invokeMethod( qApp, "changeSession", 
                                         Q_ARG( unicorn::Session, s));

        //Refresh the user list to be certain that 
        //the correct current user is checked.
        //(ie. the user change could be cancelled after confirmation.)
        refresh();

    }

    void manageUsers()
    {
        //This is required so that the menu popup call can be completed 
        //in the event loop before the manage users dialog is displayed.
        //(The manageUsers method may emit a signal which causes the menu
        // to be destroyed)
        QTimer::singleShot( 0, qApp, SLOT( manageUsers()));
    }

    void refresh()
    {
        clear();
        QActionGroup* ag = new QActionGroup( this );
        foreach( User u, unicorn::Settings().userRoster() ) {
            QAction* a = ag->addAction( new QAction( u.name(), this ));
            addAction( a );
            if( u == User()) a->setChecked( true );
            a->setCheckable( true );
        }
        ag->setExclusive( true );
        connect( ag, SIGNAL( triggered(QAction*)), SLOT( onTriggered(QAction*)));

        addSeparator();
        QAction* a = addAction( "Manage Users" );
        connect( a, SIGNAL( triggered()), SLOT( manageUsers()));

        onSessionChanged( qobject_cast<unicorn::Application*>(qApp)->currentSession());
    }
};

#endif
