#include "WelcomePage.h"


WelcomePage::WelcomePage( QWidget* parent )
    :QWizardPage( parent )
{
    setTitle( tr( "First run wizard completed" ) );
    setSubTitle( tr( "Welcome to the Last.fm Scrobbler" ) );

    QVBoxLayout* layout = new QVBoxLayout( this );
    ui.welcomeLabel =  new QLabel( this );
    layout->addWidget( ui.welcomeLabel );
}

void
WelcomePage::initializePage()
{
//        QString username = unicorn::Session().userInfo().name();
    QString username = qobject_cast<unicorn::Application*>( qApp )->currentSession()->userInfo().name();

    PluginList list;
    ui.welcomeLabel->setText(
        tr( "<p><strong>Hi %1,<br/>Welcome to the Last.fm Scrobbler</strong>\t\t\t</p>" ).arg( username ) +
        tr( "<p>The Scrobbler tracks what you listen to and updates your Last.fm profile.</p>"
            "<p>Start listening to some music in %1.</p>"
            "<p>If you close the Scrobbler, find it again in your menu bar or with Apple Ctrl Shift + S.</p>" )
        .arg( list.availableDescription() ) );

}

void
WelcomePage::cleanupPage()
{
    ui.welcomeLabel->setText( "" );
}
