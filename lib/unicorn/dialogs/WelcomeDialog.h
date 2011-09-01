#ifndef WELCOME_DIALOG_H_
#define WELCOME_DIALOG_H_

#include "../QMessageBoxBuilder.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <types/User.h>
#include "../Updater/PluginList.h"

class WelcomeDialog : public QMessageBox
{
Q_OBJECT
public:
    WelcomeDialog( const User& user, QWidget* parent = 0 )
    :QMessageBox( parent ) 
    {
        setText( tr( "<p><strong>Hi %1,<br/>Welcome to the Last.fm Scrobbler</strong>\t\t\t</p><br/>" ).arg( user.name()));
        setInformativeText( tr( "<p>The Scrobbler tracks what you listen to and updates your Last.fm profile.</p>"
                                "<p>Start listening to some music in %1.</p>"
                                "<p>If you close the Scrobbler, find it again in your menu bar or with Apple Ctrl Shift + S.</p>")
                            .arg( PluginList().availableDescription()));
        setIcon( QMessageBox::Information );
        setStandardButtons( QMessageBox::Ok );
    }
};

#endif //WELCOME_DIALOG_H_
