/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Jono Cole

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
#include "LoginPage.h"

#ifdef WIN32
#include <windows.h>
#endif // WIN32

#include <QVBoxLayout>
#include <QGridLayout>

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <lastfm/misc.h>
#include <lastfm/ws.h>
#include <lastfm/XmlQuery>

LoginPage::LoginPage( QWidget* parent )
          :QWizardPage( parent )
{
    setTitle(tr( "Login" ));
    setSubTitle(tr( "Please enter your last.fm username and password below:" ));
    new QVBoxLayout( this );

    QWidget* login = new QWidget( this );
    QGridLayout* loginLayout = new QGridLayout( login );
    
    ui.username = new QLineEdit( login );
    ui.password = new QLineEdit( login );
    ui.password->setEchoMode( QLineEdit::Password );

    registerField( "username*", ui.username );
    registerField( "password*", ui.password );

    connect( ui.username, SIGNAL(textChanged(QString)), SIGNAL( completeChanged()));
    connect( ui.password, SIGNAL(textChanged(QString)), SIGNAL( completeChanged()));
    connect( ui.username, SIGNAL(returnPressed()), ui.username->nextInFocusChain(), SLOT(setFocus()));
    connect( ui.password, SIGNAL(returnPressed()), ui.password->nextInFocusChain(), SLOT(setFocus()));

    ui.errorMsg = new QLabel( "", login );
    ui.errorMsg->setObjectName( "errorMsg" );
    ui.errorMsg->setWordWrap( true );
    ui.errorMsg->hide();


    loginLayout->addWidget( ui.errorMsg, 0, 0, 1, 2, Qt::AlignTop );

    loginLayout->addWidget( new QLabel( tr( "Username:" ), login ), 1, 0 );
    loginLayout->addWidget( ui.username, 1, 1 );
    loginLayout->addWidget( new QLabel( tr( "Password:" ), login ), 2, 0 );
    loginLayout->addWidget( ui.password, 2, 1 );

    QLabel* signupLink = new QLabel( "<a href=\"https://www.last.fm/join\">" + tr( "Sign up for a Last.fm account" ) + "</a>", login);
    signupLink->setOpenExternalLinks( true );

    QLabel* forgotPWLink = new QLabel( "<a href=\"http://www.last.fm/settings/lostpassword\">" + tr( "Forgot your password?" ) + "</a>", login);
    forgotPWLink->setOpenExternalLinks( true );

    loginLayout->addWidget( signupLink, 3, 0, 1, 2, Qt::AlignTop );
    loginLayout->addWidget( forgotPWLink, 4, 0, 1, 2, Qt::AlignTop );

    loginLayout->setRowStretch( 4, 1 );

    layout()->addWidget( login );
}


void
LoginPage::initializePage()
{
    QAbstractButton* b = wizard()->button( QWizard::NextButton );
    b->disconnect();
    connect( b, SIGNAL( clicked()), SLOT( authenticate()));
    
    // setFocus is not really necessary but gives nice visual hint as to
    // the action being performed by pushing return.
    connect( ui.password, SIGNAL( returnPressed()), wizard()->button( QWizard::NextButton ), SLOT(setFocus()));
    
    connect( ui.password, SIGNAL( returnPressed()), wizard()->button( QWizard::NextButton ), SLOT(click()));
}


void 
LoginPage::cleanupPage()
{
    QAbstractButton* b = wizard()->button( QWizard::NextButton );
    b->disconnect();
    connect( b, SIGNAL( clicked()), wizard(), SLOT( next()));
    ui.errorMsg->hide();
}

void 
LoginPage::authenticate() 
{
    QAbstractButton* b = wizard()->button( QWizard::NextButton );
    b->blockSignals( true );

    QString username = field( "username" ).toString().toLower();
    QString password = lastfm::md5( field( "password" ).toString().toUtf8() );

    QMap<QString, QString> params;
    params["method"] = "auth.getMobileSession";
    params["username"] = username;
    params["authToken"] = lastfm::md5( (username + password).toUtf8() );
    QNetworkReply* reply = lastfm::ws::post( params );
    reply->setParent( this );

    connect( reply, SIGNAL(finished()), SLOT(onAuthenticated()) );
}

void 
LoginPage::onAuthenticated()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
    QAbstractButton* b = wizard()->button( QWizard::NextButton );

    b->blockSignals( false );
    
    try {
        lastfm::XmlQuery lfm = lastfm::ws::parse( reply );
        lastfm::XmlQuery session = lfm["session"];
        
        // replace username; because eg. perhaps the user typed their
        // username with the wrong camel case
        QString username = session["name"].text();
            
        lastfm::ws::Username = username;
        lastfm::ws::SessionKey = session["key"].text();

        wizard()->next();

        // reattach the next button to
        // the qwizard::next() slot.
        cleanupPage();
        
        return;
    }
    catch (lastfm::ws::ParseError& e)
    {
        qWarning() << e.what();

        switch (e.enumValue())
        {
            case lastfm::ws::AuthenticationFailed:
                // COPYTODO
                /*QMessageBoxBuilder( this )
                        .setIcon( QMessageBox::Critical )
                        .setTitle( tr("Login Failed") ) */
                ui.errorMsg->setText( tr("That doesn't seem right, try again?") );
                        /*.exec();*/;
                break;
            
            default:
                // COPYTODO
                /*QMessageBoxBuilder( this )
                        .setIcon( QMessageBox::Critical )
                        .setTitle( tr("Last.fm Unavailable") ) */
                ui.errorMsg->setText( tr("There was a problem communicating with the Last.fm services. Please try again later.") );
                        /*.exec();*/
                break;
            
            case lastfm::ws::TryAgainLater:
            case lastfm::ws::UnknownError:
                switch ((int)reply->error())
                {
                    case QNetworkReply::ProxyConnectionClosedError:
                    case QNetworkReply::ProxyConnectionRefusedError:
                    case QNetworkReply::ProxyNotFoundError:
                    case QNetworkReply::ProxyTimeoutError:
                    case QNetworkReply::ProxyAuthenticationRequiredError: //TODO we are meant to prompt!
                    case QNetworkReply::UnknownProxyError:
                    case QNetworkReply::UnknownNetworkError:
                        break;
                    default:
                        return;
                }

/*                    // TODO proxy prompting?
                // COPYTODO
                QMessageBoxBuilder( this )
                        .setIcon( QMessageBox::Critical )
                        .setTitle( tr("Cannot connect to Last.fm") ) */
                ui.errorMsg->setText( tr("Last.fm cannot be reached. Please check your firewall or proxy settings.") );
                        /*.exec();*/

            #ifdef WIN32
                // show Internet Settings Control Panel
                HMODULE h = LoadLibraryA( "InetCpl.cpl" );
                if (!h) break;
                BOOL (WINAPI *cpl)(HWND) = (BOOL (WINAPI *)(HWND)) GetProcAddress( h, "LaunchConnectionDialog" );
                if (cpl) cpl( winId() );
                FreeLibrary( h );
            #endif
                break;
        }
        ui.password->setFocus();
        ui.password->selectAll();
        ui.errorMsg->show();
    }
}

