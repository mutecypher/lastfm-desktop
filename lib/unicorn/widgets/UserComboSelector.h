#ifndef USER_COMBO_SELECTOR_H_
#define USER_COMBO_SELECTOR_H_

#include <QComboBox>
#include <lastfm/User>
#include "lib/unicorn/UnicornSettings.h"
#include "lib/unicorn/UnicornSession.h"

class UserComboSelector : public QComboBox
{
    Q_OBJECT
public:
    UserComboSelector( QWidget* p = 0 )
    {
        foreach( User u, unicorn::Settings().userRoster() ) {
            addItem( u.name() );
        }

        connect( this, SIGNAL( activated( QString)), SLOT( onChangeUser( QString )));
    }

protected slots:
    void onChangeUser( const QString& username )
    {
        unicorn::Session s( username );
        if( !s.isValid()) {
            //TODO Error handling
            return;
        }
        
        QMetaObject::invokeMethod( qApp, "changeSession", 
                                         Q_ARG( unicorn::Session, s));

    }
};

#endif
