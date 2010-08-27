#ifndef WELCOME_PAGE_H
#define WELCOME_PAGE_H

#include "lib/unicorn/Updater/PluginList.h"
#include "lib/unicorn/UnicornSession.h"

#include <QDebug>
#include <QLabel>
#include <QVariant>
#include <QVBoxLayout>
#include <QWizardPage>

class WelcomePage : public QWizardPage
{
    Q_OBJECT
public:
    WelcomePage( QWidget* parent = 0 )
    :QWizardPage( parent )
    {
        setTitle( tr( "First run wizard completed" ) );
        setSubTitle( tr( "Welcome to the Last.fm Scrobbler" ) );
        QVBoxLayout* layout = new QVBoxLayout( this );
        ui.welcomeLabel =  new QLabel( this );
        layout->addWidget( ui.welcomeLabel );
    }

    virtual void initializePage()
    {
        QString username = unicorn::Session().userInfo().name();
        ui.welcomeLabel->setText(
            tr( "<p><strong>Hi %1,<br/>Welcome to the Last.fm Scrobbler</strong>\t\t\t</p>" ).arg( username ) +
            tr( "<p>The Scrobbler tracks what you listen to and updates your Last.fm profile.</p>"
                "<p>Start listening to some music in %1.</p>"
                "<p>If you close the Scrobbler, find it again in your menu bar or with Apple Ctrl Shift + S.</p>" )
            .arg( PluginList().availableDescription() ) );

    }

    virtual void cleanupPage()
    {
        ui.welcomeLabel->setText( "" );
    }

private:
    struct Ui
    {
        QLabel* welcomeLabel;
    }ui;
};

#endif // WELCOME_PAGE_H
